/**
 * Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

   when        who         what, where, why
   --------    -----       -----------------------------------------------------
   22/08/21    mdgreen     Initial version
   30/12/21    shaygold    Adding qc_load_privkey

 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <openssl/crypto.h>
#include <openssl/engine.h>
#include <openssl/opensslv.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <syslog.h>

#include "TEE_client_api.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define ALOG(p, ...)  syslog(p,  __VA_ARGS__)
#ifdef PRINT_LOG_TO_STDOUT
#define LOGI(fmt, x...) printf("qautheng: info: %s: " fmt "\n", __func__,##x)
#define LOGV(fmt, x...) printf("qautheng: verb: %s: " fmt "\n", __func__,##x)
#define LOGD(fmt, x...) printf("qautheng: dbg: %s: " fmt "\n", __func__,##x)
#define LOGE(fmt, x...) printf("qautheng: err: %s: " fmt "\n", __func__,##x)
#define LOGW(fmt, x...) printf("qautheng: wrn: %s: " fmt "\n", __func__,##x)
#else
#define LOGI(fmt, x...) syslog(LOG_NOTICE, "INFO: [%s:%d] " fmt , __func__, __LINE__, ##x)
#define LOGV(fmt, x...) syslog(LOG_NOTICE,"VERB: [%s:%d] " fmt , __func__, __LINE__, ##x)
#define LOGD(fmt, x...) syslog(LOG_DEBUG,"DBG: [%s:%d] " fmt , __func__, __LINE__, ##x)
#define LOGE(fmt, x...) syslog(LOG_ERR,"ERR: [%s:%d] " fmt , __func__, __LINE__, ##x)
#define LOGW(fmt, x...) syslog(LOG_WARNING,"WRN: [%s:%d] " fmt , __func__, __LINE__, ##x)
#endif

/* openssl command type send to this engine */
#define QC_CMD_KEY_TYPE	ENGINE_CMD_BASE

/* RSA private key types */
#define QC_REGPRIV_KEY "RegPriv"
#define QC_INITPRIV_KEY "InitPriv"
#define QC_REMOTEUIPRIV_KEY "RemoteUIPriv"
#define QC_MAX_PARAM_LEN 32

/* TA Command ID */
#define OEM_SERVICE_ASYMMETRIC_DECRYPT 5


/* Signing Key type */
typedef enum {
    RSA_PRIV_INIT_4096,     /* 4096-bit Initial Private Key */
    RSA_PRIV_END_4096,      /* 4096-bit Endorsement Private Key */
    RSA_PRIV_REG_4096,      /* 4096-bit Regular Private Key */
    RSA_PRIV_REMOTEUI_4096, /**< 4096-bit Remote UI Private Key */
    RSA_PRIV_INVALID = 0xffffffff,
} oem_srv_sign_key_type_e;


/* UUID for service TA */
const TEEC_UUID gp_service_UUID = {
        0x5f189f9f, 0x8c36, 0x411f, {0x90, 0xcc, 0x76, 0xe4, 0x96, 0x10, 0x0b, 0xa4}
};

/* openssl command definition */
static const ENGINE_CMD_DEFN qc_cmd_defns[] = {
        {
                QC_CMD_KEY_TYPE,
                "PrivateKey",
                "Define private key type",
                ENGINE_CMD_FLAG_STRING
        },
        {0, NULL, NULL, 0}
};

static const char *qc_engine_id = "qautheng";
static const char *qc_engine_name = "QC authentication engine";
static oem_srv_sign_key_type_e sign_key_type = RSA_PRIV_INVALID;

static inline size_t memscpy(void *dst, size_t dst_size, const void *src, size_t src_size) {
    size_t  copy_size = (dst_size <= src_size) ? dst_size : src_size;
    memcpy(dst, src, copy_size);
    return copy_size;
}

static int qc_ctrl(ENGINE *e, int cmd, long i, void *p, void (*f) (void))
{
    int ret = ENGINE_R_CTRL_COMMAND_NOT_IMPLEMENTED;
    LOGD("cmd= %d, i= %ld, param= %s\n", cmd, i, (p==NULL) ? "NULL" : (char*)p);
    do {
        if (cmd != QC_CMD_KEY_TYPE || NULL == p) {
            LOGD("Nothing to do");
            break;
        }
            
        char *pchar = (char*)p;
        size_t key_len = strlen(QC_REGPRIV_KEY);
        if (!strncmp(pchar, QC_REGPRIV_KEY, key_len) &&
                (0 == pchar[key_len])) {
            sign_key_type = RSA_PRIV_REG_4096;
            LOGD("Request RSA_PRIV_REG_4096");
            ret = 0;
            break;
        }

        key_len = strlen(QC_INITPRIV_KEY);
        if (!strncmp(pchar, QC_INITPRIV_KEY, key_len) &&
                (0 == pchar[key_len])) {
            sign_key_type = RSA_PRIV_INIT_4096;
            LOGD("Request RSA_PRIV_INIT_4096");
            ret = 0;
            break;
        }
        key_len = strlen(QC_REMOTEUIPRIV_KEY);
        if (!strncmp(pchar, QC_REMOTEUIPRIV_KEY, key_len) &&
                (0 == pchar[key_len])) {
            sign_key_type = RSA_PRIV_REMOTEUI_4096;
            LOGD("Request RSA_PRIV_REMOTEUI_4096");
            ret = 0;
            break;
        }

        char tmp_str[QC_MAX_PARAM_LEN+1];
        snprintf(tmp_str, QC_MAX_PARAM_LEN, "%s", pchar);
        LOGE("Invalid ctrl param [%s]", (pchar == NULL) ? "NULL" : tmp_str);
        break;
    } while(0);

    return ret;
}

static int qc_rsa_priv_enc(int flen, const unsigned char *from,
        unsigned char *to, RSA *rsa, int padding)
{
    TEEC_Context context = {};
    TEEC_Session session = {};
    TEEC_SharedMemory sm_in = {};
    TEEC_SharedMemory sm_out = {};
    TEEC_Operation operation = {};
    uint32_t ret_orig = 0xFFFFFFFF;
    TEEC_UUID uuid = {};
    uint32_t command = OEM_SERVICE_ASYMMETRIC_DECRYPT;
    TEEC_Result teec_ret;

    int ret = -1;

    LOGD("Entry");
    if (!flen || !from || !to || !rsa || (RSA_NO_PADDING != padding)) {
        LOGE("Invalid params (NULL)");
        return ret;
    }

    if ((0 > flen) || (flen > RSA_size(rsa))) {
        LOGE("Invalid params flen [%d], RSA_size [%d]", flen, RSA_size(rsa));
        return ret;
    }

    if (RSA_PRIV_INVALID == sign_key_type) {
        LOGE("Invalid configuration file");
        return ret;
    }

    memscpy(&uuid, sizeof(TEEC_UUID), &gp_service_UUID, sizeof(TEEC_UUID));

    do {
        if (TEEC_SUCCESS != (teec_ret = TEEC_InitializeContext(NULL, &context))) {
            LOGE("TEEC_InitializeContext failed ret [%d]", teec_ret);
            break;
        }

        sm_in.size = flen;
        sm_in.flags = TEEC_MEM_INPUT;
        if (TEEC_SUCCESS != (teec_ret = TEEC_AllocateSharedMemory(&context, &sm_in))) {
            LOGE("TEEC_RegisterSharedMemory failed ret [%d]", teec_ret);
            break;
        }

        /* Copy input data in to shared buffer */
        memscpy(sm_in.buffer, flen, from, flen);

        sm_out.size = flen;
        sm_out.flags = TEEC_MEM_OUTPUT;
        if (TEEC_SUCCESS != (teec_ret = TEEC_AllocateSharedMemory(&context, &sm_out))) {
            LOGE("TEEC_RegisterSharedMemory failed ret [%d]", teec_ret);
            break;
        }

        if (TEEC_SUCCESS != (teec_ret = TEEC_OpenSession(&context, &session, &uuid,
                TEEC_LOGIN_USER, NULL, NULL, &ret_orig))) {
            LOGE("TEEC_OpenSession failed ret [%d]", teec_ret);
            break;
        }

        operation.paramTypes = 0;
        operation.paramTypes =
                TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_MEMREF_PARTIAL_INPUT, TEEC_MEMREF_PARTIAL_OUTPUT, TEEC_NONE);
        operation.params[0].value.a = sign_key_type;
        operation.params[1].memref.parent = &sm_in;
        operation.params[1].memref.offset = 0;
        operation.params[1].memref.size = flen;
        operation.params[2].memref.parent = &sm_out;
        operation.params[2].memref.offset = 0;
        operation.params[2].memref.size = flen;

        if (TEEC_SUCCESS != (teec_ret = TEEC_InvokeCommand(&session, command,
                &operation, &ret_orig))) {
            LOGD("TEEC_InvokeCommand ret_orig [%d]", ret_orig);
            LOGE("TEEC_InvokeCommand failed ret [%d]", teec_ret);
            break;
        }

        if (flen != sm_out.size) {
            LOGE("TEEC_InvokeCommand failed, return len [%ld]", sm_out.size);
            break;
        }

        /* Copy output data from shared memory */
        memscpy(to, flen, sm_out.buffer, sm_out.size);
        ret = sm_out.size;
    } while(0);

    if (sm_in.buffer) {
        TEEC_ReleaseSharedMemory(&sm_in);
    }

    if (sm_out.buffer) {
        TEEC_ReleaseSharedMemory(&sm_out);
    }

    TEEC_CloseSession(&session);
    TEEC_FinalizeContext(&context);

    LOGD("Exit ret [%d]", ret);
    return ret;
}

static EVP_PKEY* qc_load_privkey(ENGINE* e, char const* key_id, UI_METHOD* ui_method, void* cb_data)
{
    EVP_PKEY* pkey = NULL;
    EVP_PKEY* tmp_key = NULL;
    int bits = 0;
    X509* cert = NULL;
    BIO* bp = NULL;
    const RSA* pub_rsa;
    RSA* rsa = NULL;
    const BIGNUM* pub_n;
    const BIGNUM* pub_e;
    BIGNUM* pub_n_dp1;
    BIGNUM* pub_e_dp1;

    LOGD("Starting qc_load_privkey, for sign_key_type = %d", sign_key_type);
    do {
        if(!key_id){
            LOGE("key_id not provided.");
            break;
        }
        LOGD("key_id = %s", key_id);

        bp = BIO_new_file(key_id, "r");
        if (!bp) {
            LOGE("BIO_new_file failed to open %s", key_id);
            break;
        }

        cert = PEM_read_bio_X509(bp, NULL, 0, NULL);
        if (!cert) {
            LOGE("PEM_read_bio_X509 failed.");
            break;
        }

        tmp_key = X509_get_pubkey(cert);
        if (!tmp_key) {
            LOGE("X509_get_pkey failed.");
            break;
        }

        pub_rsa = EVP_PKEY_get0_RSA(tmp_key);
        if(!pub_rsa) {
            LOGE("EVP_PKEY_get0_RSA failed.");
            break;
        }

        bits = RSA_bits(pub_rsa);
        RSA_get0_key(pub_rsa, &pub_n, &pub_e, NULL);

        rsa = RSA_new();
        if(!rsa) {
            LOGE("RSA_new failed.");
            break;
        }

        LOGD("Bits = %d", bits);
        LOGD("n = %s", BN_bn2hex(pub_n));
        LOGD("e = %s", BN_bn2hex(pub_e));

        pub_n_dp1 = BN_dup(pub_n);
        pub_e_dp1 = BN_dup(pub_e);
        if (!pub_n_dp1 || !pub_e_dp1) {
            LOGE("BN_dup failed.");
            break;
        }

        if ((1 != RSA_generate_key_ex(rsa, bits, pub_e_dp1, NULL)) ||
                (1 != RSA_set0_key(rsa, pub_n_dp1, NULL , NULL))||
                (1 != EVP_PKEY_assign_RSA(tmp_key, rsa))) {
            LOGE("Failed to update mock key failed");
        }
        pkey = tmp_key;
        tmp_key = NULL;
    } while (0);

    if(cert){
        X509_free(cert);
    }
    if (bp) {
        BIO_free_all(bp);
    }
    if (tmp_key) {
        EVP_PKEY_free(tmp_key);
    }
    LOGD("qc_load_privkey done.");

    return pkey;
}

#define VER_MAJOR_MASK 0xF0000000L
#define VER_MINOR_MASK 0x0FF00000L
#define VER_PATCH_MASK 0x000FF000L

static int bind(ENGINE *e, const char *id)
{
    RSA_METHOD *qc_rsa_method = NULL;
    int ret = 0;
    bool major_ok = false;
    bool minor_ok = false;
    bool patch_ok = false;
    unsigned long engmajor, engminor, engpatch;
    unsigned long engversion = OPENSSL_VERSION_NUMBER;
    unsigned long libmajor, libminor, libpatch;
    unsigned long libversion = OpenSSL_version_num();

    engmajor = engversion & VER_MAJOR_MASK;
    engminor = engversion & VER_MINOR_MASK;
    engpatch = engversion & VER_PATCH_MASK;

    libmajor = libversion & VER_MAJOR_MASK;
    libminor = libversion & VER_MINOR_MASK;
    libpatch = libversion & VER_PATCH_MASK;

    do {
        if (!e) {
            LOGE("Invalid engine");
            break;
        }

        if ((libversion == engversion) || (libmajor > engmajor)) {
            major_ok = minor_ok = patch_ok = true;
        } else if (libmajor == engmajor) {
            major_ok = true;
            if (libminor > engminor) {
                minor_ok = patch_ok = true;
            } else if (libminor == engminor) {
                minor_ok = true;
                if (libpatch >= engpatch) {
                    patch_ok = true;
                }
            }
        }

        if (!(major_ok && minor_ok && patch_ok)) {
            LOGE("Incompatible versions. Built with %s running with %s",
                    OPENSSL_VERSION_TEXT, OpenSSL_version(OPENSSL_VERSION));
            break;
        }

        LOGD("Engine built with openssl %s", OPENSSL_VERSION_TEXT);

        LOGD("Binding engine %s with %s", id, OpenSSL_version(OPENSSL_VERSION));

        /* Setup QC RSA_METHOD */
        if (NULL == (qc_rsa_method = RSA_meth_dup(RSA_PKCS1_OpenSSL()))) {
            LOGE("RSA_meth_dup failed");
            break;
        }
        if (!RSA_meth_set1_name(qc_rsa_method, qc_engine_name)) {
            LOGE("RSA_meth_set1_name failed");
            break;
        }

        if (!RSA_meth_set_priv_enc(qc_rsa_method, qc_rsa_priv_enc)) {
            LOGE("RSA_meth_set_priv_enc failed");
            break;
        }

        if (!ENGINE_set_id(e, qc_engine_id)
                || !ENGINE_set_name(e, qc_engine_name)
                || !ENGINE_set_cmd_defns(e, qc_cmd_defns)
                || !ENGINE_set_ctrl_function(e, qc_ctrl)
                || !ENGINE_set_RSA(e, qc_rsa_method)
                || !ENGINE_set_load_privkey_function(e, qc_load_privkey)) {
            LOGE("ENGINE_set_* failed");
            break;
        }
        ret = 1;
    } while(0);

    return ret;
}

IMPLEMENT_DYNAMIC_BIND_FN(bind)
IMPLEMENT_DYNAMIC_CHECK_FN()


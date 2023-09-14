/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdint.h>
#include <stdio.h>
#include <pthread.h>

#include <keymaster_defs.h>
#include <keymaster.h>
#include <keymaster_utils.h>

#include "sample_utils.h"

extern int keymaster_aes_sample(bool multi_thread);
extern int keymaster_rsa_sample(bool multi_thread);
extern int keymaster_import_export_sample(bool multi_thread);
extern int keymaster_delete_sample(bool multi_thread);
extern int keymaster_hmac_sample(bool multi_thread);
extern int keymaster_ecdsa_sample(bool multi_thread);

/* defien the number of threads */
#define THREAD_NUMS    8
#define KM_LOGE(fmt, args...)    printf("ERROR: %s " fmt "", __func__, ##args)

static uint8_t IV[128/8] = {0x01};

typedef struct {
    keymaster_key_blob_t old_key_blob;
    keymaster_key_blob_t upgraded_key_blob;
    keymaster_error_t err;
} keymaster_key_upgraded_blob_t;

static keymaster_error_t pthread_join_check(pthread_t *tids, size_t tids_size, keymaster_error_t *err) {
    if (tids == NULL || tids_size == 0 || err == NULL)
        return KM_ERROR_UNKNOWN_ERROR;

    for (int i = 0; i < tids_size; i++) {
        if (pthread_join(tids[i], NULL) != 0 || err[i] != KM_ERROR_OK) {
            KM_LOGE("pthread_join\n");
            return KM_ERROR_UNKNOWN_ERROR;
        }
    }

    return KM_ERROR_OK;
}

static void *keymaster_aes_multi(void *arg) {
    keymaster_error_t *err = (keymaster_error_t *)arg;

    *err = keymaster_aes_sample(true);
    if (*err != KM_ERROR_OK)
        KM_LOGE("return %d\n", *err);

    pthread_exit(0);
}

static void *keymaster_rsa_multi(void *arg) {
    keymaster_error_t *err = (keymaster_error_t *)arg;

    *err = keymaster_rsa_sample(true);
    if (*err != KM_ERROR_OK)
        KM_LOGE("return %d\n", *err);

    pthread_exit(0);
}

static void *keymaster_ecdsa_multi(void *arg) {
    keymaster_error_t *err = (keymaster_error_t *)arg;

    *err = keymaster_ecdsa_sample(true);
    if (*err != KM_ERROR_OK)
        KM_LOGE("return %d\n", *err);

    pthread_exit(0);
}

static void *keymaster_hmac_multi(void *arg) {
    keymaster_error_t *err = (keymaster_error_t *)arg;

    *err = keymaster_hmac_sample(true);
    if (*err != KM_ERROR_OK)
        KM_LOGE("return %d\n", *err);

    pthread_exit(0);
}

static void *keymaster_impexp_multi(void *arg) {
    keymaster_error_t *err = (keymaster_error_t *)arg;

    *err = keymaster_import_export_sample(true);
    if (*err != KM_ERROR_OK)
        KM_LOGE("return %d\n", *err);

    pthread_exit(0);
}

static void *keymaster_delete_multi(void *arg) {
    keymaster_error_t *err = (keymaster_error_t *)arg;

    *err = keymaster_aes_sample(true);
    if (*err != KM_ERROR_OK)
        KM_LOGE("return %d\n", *err);

    pthread_exit(0);
}

/*
 * just an attempt for km_begin operation with the AES key. (will abort the operation in case of success.)
 */
static keymaster_error_t keymaster_attempt_begin(const keymaster_key_blob_t* key_blob)
{
    keymaster_operation_handle_t op_handle;

    keymaster_key_param_t op_params[] = {
            keymaster_param_enum(KM_TAG_PADDING, KM_PAD_PKCS7),
            keymaster_param_enum(KM_TAG_BLOCK_MODE, KM_MODE_CBC),
            keymaster_param_blob(KM_TAG_NONCE, IV, sizeof(IV))
    };
    keymaster_key_param_set_t op_params_set = {
            .params = op_params,
            .length = sizeof(op_params)/sizeof(op_params[0]),
    };

    keymaster_key_param_set_t op_out_params_set = {0};

    keymaster_error_t err = km_begin(KM_PURPOSE_ENCRYPT, key_blob, &op_params_set, &op_out_params_set, &op_handle);
    if (err != KM_ERROR_OK)
        return err;

    err = km_abort(op_handle);

    return err;
}

static keymaster_error_t keymaster_prepare_old_key(keymaster_key_upgraded_blob_t *key_blob) {
    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;

    err = get_old_aes_key(&key_blob->old_key_blob);
    if (err != KM_ERROR_OK) {
        KM_LOGE("return %d\n", err);
        return err;
    }

    err = initialize_keymaster();
    if (err != KM_ERROR_OK) {
        KM_LOGE("initialize_keymaster\n");
        goto exit;
    }

    /*
     * try to start encrypt operation with old blob
     */
    err = keymaster_attempt_begin(&key_blob->old_key_blob);
    /*
     * expect it to fail and return "key upgrade required"
     */
    if (err != KM_ERROR_KEY_REQUIRES_UPGRADE) {
        printf("ERROR: km_begin() was expected to return KM_ERROR_KEY_REQUIRES_UPGRADE (%d). (actual: %d)\n",
             KM_ERROR_KEY_REQUIRES_UPGRADE, err);
        printf("Make sure the OS version/patch passed during initialization differ from the values\n");
        printf("embedded in the old key material.\n");
        /*
         * return error to make sure the sample fails even if OK was returned (which is unexpected)
         */
        err == KM_ERROR_UNKNOWN_ERROR;
        goto exit;
    }

    printf("km_begin() returned with KM_ERROR_KEY_REQUIRES_UPGRADE as expected for old key.\n");

exit:
    km_close();
    return KM_ERROR_OK;
}

static keymaster_error_t keymaster_verify_upgrade_key(keymaster_key_upgraded_blob_t *key_blob) {
    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;

    err = keymaster_attempt_begin(&key_blob->upgraded_key_blob);
    if (err != KM_ERROR_OK) {
        printf("ERROR: attempt_begin() was expected to return KM_ERROR_OK (%d). (actual: %d)\n", KM_ERROR_OK, err);
        return err;
    }

    printf("key_upgraded verify success\n");
    return err;
}

static void *keymaster_key_upgrade_multi(void *arg) {
    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;
    keymaster_key_upgraded_blob_t *key_blob = (keymaster_key_upgraded_blob_t *)arg;

    if (arg == NULL) {
        KM_LOGE("arg is invalid\n");
        goto exit;
    }

    err = km_upgrade_key(&key_blob->old_key_blob, NULL, &key_blob->upgraded_key_blob);
    if (err != KM_ERROR_OK) {
        KM_LOGE("km_upgrade_key fail return %d\n", err);
        goto exit;
    }

exit:
    key_blob->err = err;
    pthread_exit(0);
}

keymaster_error_t keymaster_multithreading(void) {
    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;
    keymaster_error_t arg[THREAD_NUMS];
    pthread_t tids[THREAD_NUMS];

    keymaster_key_upgraded_blob_t key_blob[THREAD_NUMS];

    printf("keymaster_multithreading\n");

    /*
     * the old key blob will be generated by the old OS version
     */
    for (int i = 0; i < THREAD_NUMS; i++) {
        err = keymaster_prepare_old_key(&key_blob[i]);
        if (err != KM_ERROR_OK)
            return err;
    }

    err = initialize_keymaster();
    if (err != KM_ERROR_OK) {
        KM_LOGE("initialize_keymaster\n");
        return err;
    }

    /*
     * multiple aes sample thread
     */
    for (int i = 0; i < THREAD_NUMS; i++) {
        if (pthread_create(&tids[i], NULL, keymaster_aes_multi, (void *)&arg[i]) != 0) {
            KM_LOGE("pthread create fail\n");
            return KM_ERROR_UNKNOWN_ERROR;
        }
    }
    if (pthread_join_check(tids, THREAD_NUMS, arg) != KM_ERROR_OK)
        return KM_ERROR_UNKNOWN_ERROR;

    /*
     * multiple rsa sample thread
     */
    for (int i = 0; i < THREAD_NUMS; i++) {
        if (pthread_create(&tids[i], NULL, keymaster_rsa_multi, (void *)&arg[i]) != 0) {
            KM_LOGE("pthread create fail\n");
            return KM_ERROR_UNKNOWN_ERROR;
        }
    }
    if (pthread_join_check(tids, THREAD_NUMS, arg) != KM_ERROR_OK)
        return KM_ERROR_UNKNOWN_ERROR;

    /*
     * multiple ecdsa sample thread
     */
    for (int i = 0; i < THREAD_NUMS; i++) {
        if (pthread_create(&tids[i], NULL, keymaster_ecdsa_multi, (void *)&arg[i]) != 0) {
            KM_LOGE("pthread create fail\n");
            return KM_ERROR_UNKNOWN_ERROR;
        }
    }
    if (pthread_join_check(tids, THREAD_NUMS, arg) != KM_ERROR_OK)
        return KM_ERROR_UNKNOWN_ERROR;

    /*
     * multiple hmac sample thread
     */
    for (int i = 0; i < THREAD_NUMS; i++) {
        if (pthread_create(&tids[i], NULL, keymaster_hmac_multi, (void *)&arg[i]) != 0) {
            KM_LOGE("pthread create fail\n");
            return KM_ERROR_UNKNOWN_ERROR;
        }
    }
    if (pthread_join_check(tids, THREAD_NUMS, arg) != KM_ERROR_OK)
        return KM_ERROR_UNKNOWN_ERROR;

    /*
     * multiple import export sample thread
     */
    for (int i = 0; i < THREAD_NUMS; i++) {
        if (pthread_create(&tids[i], NULL, keymaster_impexp_multi, (void *)&arg[i]) != 0) {
            KM_LOGE("pthread create fail\n");
            return KM_ERROR_UNKNOWN_ERROR;
        }
    }
    if (pthread_join_check(tids, THREAD_NUMS, arg) != KM_ERROR_OK)
        return KM_ERROR_UNKNOWN_ERROR;

    /*
     * multiple delete sample thread
     */
    for (int i = 0; i < THREAD_NUMS; i++) {
        if (pthread_create(&tids[i], NULL, keymaster_delete_multi, (void *)&arg[i]) != 0) {
            KM_LOGE("pthread create fail\n");
            return KM_ERROR_UNKNOWN_ERROR;
        }
    }
    if (pthread_join_check(tids, THREAD_NUMS, arg) != KM_ERROR_OK)
        return KM_ERROR_UNKNOWN_ERROR;

    /*
     * multiple key upgrade sample thread
     */
    for (int i = 0; i < THREAD_NUMS; i++) {
        if (pthread_create(&tids[i], NULL, keymaster_key_upgrade_multi, (void *)&key_blob[i]) != 0) {
            KM_LOGE("pthread create fail\n");
            return KM_ERROR_UNKNOWN_ERROR;
        }
    }

    for (int i = 0; i < THREAD_NUMS; i++) {
        if (pthread_join(tids[i], NULL) != 0) {
            KM_LOGE("pthread join fail");
            return KM_ERROR_UNKNOWN_ERROR;
        }
        if (key_blob[i].err != KM_ERROR_OK || keymaster_verify_upgrade_key(&key_blob[i]) != KM_ERROR_OK)
            return KM_ERROR_UNKNOWN_ERROR;
    }

    km_close();
    printf("SUCCESS: km lite multithreadding test pass\n");
    return KM_ERROR_OK;
}

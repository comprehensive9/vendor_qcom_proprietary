/*
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "sample_utils.h"
#include "keymaster_utils.h"

//OS version/patch are used to handle key upgrades
//in production should be provided by the client based on current OS/client version
#define OS_VERSION 110000
#define OS_SECURITY_PATCH 202103 // Feb-2000

/*************************************************************************************/
/* INITIALIZE KEYMASTER */
/*************************************************************************************/
keymaster_error_t init_km(uint32_t OS_version) {

    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;
    const uint8_t rot[32] = { 0xa, 0x20, 0xef };  //some unique sequence to be used as ROT
    keymaster_blob_t rot_blob = {
            .data = rot,
            .data_length = sizeof(rot)
    };

    err = km_init(&rot_blob, OS_version, OS_SECURITY_PATCH );
    return err;
}

keymaster_error_t initialize_keymaster(void) {

    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;
    err = init_km( OS_VERSION );
    return err;
}

/*************************************************************************************/
/* DEINITIALIZE KEYMASTER */
/*************************************************************************************/
keymaster_error_t deinitialize_keymaster(void) {

    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;
    err = km_close();
    return err;
}

//helper function to print a specific keymaster tag value from key characteristics
void print_tag_value_from_characterstic(keymaster_tag_t tag,
        const keymaster_key_characteristics_t* characterstics)
{
    if (!characterstics) {
        printf("characterstics param is NULL.\n");
        return;
    }
    for(uint32_t i = 0; i < characterstics->hw_enforced.length; i++) {
        if(characterstics->hw_enforced.params[i].tag == tag) {
            switch(keymaster_tag_get_type(characterstics->hw_enforced.params[i].tag)) {
            case KM_UINT:
                printf("%u", characterstics->hw_enforced.params[i].integer);
                return;
            case KM_BOOL:
                printf("%s", characterstics->hw_enforced.params[i].boolean ? "true": "false");
                return;
            default:
                //more tag types cases can be added as needed
                printf("Unsupported tag type in print_tag_value_from_characterstic for tag:%d\n", tag);
            }
        }
    }
    printf("false");
    return;
}

/*************************************************************************************/
/* GENERATE KEY */
/*************************************************************************************/
keymaster_error_t get_old_aes_key(keymaster_key_blob_t* aes_key_blob) {
    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;

    err = init_km( OS_VERSION -1 );
    if (err != KM_ERROR_OK)
        return err;

    keymaster_key_param_t params[] = {
            keymaster_param_enum(KM_TAG_ALGORITHM, KM_ALGORITHM_AES),
            keymaster_param_int(KM_TAG_KEY_SIZE, 128),

            //allowed modes
            keymaster_param_enum(KM_TAG_BLOCK_MODE, KM_MODE_CBC),
            keymaster_param_enum(KM_TAG_BLOCK_MODE, KM_MODE_CTR),

            //allowed paddings
            keymaster_param_enum(KM_TAG_PADDING, KM_PAD_PKCS7),
            keymaster_param_enum(KM_TAG_PADDING, KM_PAD_NONE),

            //allow custom IV
            keymaster_param_bool(KM_TAG_CALLER_NONCE),

            //allow encrypt and decrypt operations
            keymaster_param_enum(KM_TAG_PURPOSE, KM_PURPOSE_ENCRYPT),
            keymaster_param_enum(KM_TAG_PURPOSE, KM_PURPOSE_DECRYPT),

    };
    const keymaster_key_param_set_t param_set = {
            .params = params,
            .length = sizeof(params)/sizeof(params[0]),
    };

    keymaster_key_characteristics_t aes_key_characteristics;

    err = km_generate_key(&param_set, aes_key_blob, &aes_key_characteristics);
    if (err != KM_ERROR_OK)
        return err;

    //cleanup memory allocated by KM
    keymaster_free_characteristics(&aes_key_characteristics);

    err = deinitialize_keymaster();
    if (err != KM_ERROR_OK)
        return err;

    return KM_ERROR_OK;
}

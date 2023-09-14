/*
 * Copyright (c) 2019-2020, 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdint.h>
#include <stdio.h>

#include <keymaster_defs.h>
#include <keymaster.h>
#include <keymaster_utils.h>

#include "sample_utils.h"

/*************************************************************************************/
/* GENERATE KEY */
/*************************************************************************************/
static keymaster_error_t create_aes_key(keymaster_key_blob_t* aes_key_blob) {
    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;

    keymaster_key_param_t params[] = {
            keymaster_param_enum(KM_TAG_ALGORITHM, KM_ALGORITHM_AES),
            keymaster_param_int(KM_TAG_KEY_SIZE, 128),

            //allowed modes
            keymaster_param_enum(KM_TAG_BLOCK_MODE, KM_MODE_ECB),

            //allowed paddings
            keymaster_param_enum(KM_TAG_PADDING, KM_PAD_PKCS7),
            keymaster_param_enum(KM_TAG_PADDING, KM_PAD_NONE),

            //allow custom IV
            keymaster_param_bool(KM_TAG_CALLER_NONCE),

            //allow encrypt and decrypt operations
            keymaster_param_enum(KM_TAG_PURPOSE, KM_PURPOSE_ENCRYPT),
            keymaster_param_enum(KM_TAG_PURPOSE, KM_PURPOSE_DECRYPT),
            keymaster_param_bool(KM_TAG_ROLLBACK_RESISTANCE),
    };
    const keymaster_key_param_set_t param_set = {
            .params = params,
            .length = sizeof(params)/sizeof(params[0]),
    };

    keymaster_key_characteristics_t aes_key_characteristics;

    err = km_generate_key(&param_set, aes_key_blob, &aes_key_characteristics);
    if (err != KM_ERROR_OK)
        return err;

    printf("AES key created successfully with KM_TAG_ROLLBACK_RESISTANCE is ");
    print_tag_value_from_characterstic(KM_TAG_ROLLBACK_RESISTANCE, &aes_key_characteristics);
    printf("\n");

    //cleanup memory allocated by KM
    keymaster_free_characteristics(&aes_key_characteristics);

    return KM_ERROR_OK;
}

/*************************************************************************************/
/* DELETE KEY */
/*************************************************************************************/
//demonstrates key deletion procedure and use-after-delete error
keymaster_error_t keymaster_delete_sample(bool multi_thread)
{
    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;
    if (!multi_thread) {
        err = initialize_keymaster();
        if (err != KM_ERROR_OK)
            return err;
    }

    //generate AES key

    keymaster_key_blob_t aes_key_blob = { 0 };
    err = create_aes_key(&aes_key_blob);
    if (err != KM_ERROR_OK) {
        printf("error: create_aes_key fail\n");
        goto exit;
    }

    //delete key
    err = km_delete_key(&aes_key_blob);

    //try to call begin and observe the operation fails as the key was marked 'deleted'

    keymaster_operation_handle_t op_handle;
    keymaster_key_param_t op_params[] = {
            keymaster_param_enum(KM_TAG_PADDING, KM_PAD_PKCS7),
            keymaster_param_enum(KM_TAG_BLOCK_MODE, KM_MODE_ECB),
    };
    keymaster_key_param_set_t op_params_set = {
            .params = op_params,
            .length = sizeof(op_params) / sizeof(op_params[0]),
    };

    err = km_begin(KM_PURPOSE_DECRYPT, &aes_key_blob, &op_params_set, NULL, &op_handle);

    //we expect the key to be invalid at this point if deletion was successful
    if (err == KM_ERROR_INVALID_KEY_BLOB) {
        printf("Key was deleted successfully!\n");
        err = KM_ERROR_OK; //return OK result
    } else {
        km_abort(op_handle);
        printf("Key deletion failed. Please make sure the RPMB is provisioned and is not full!\n");
        err = KM_ERROR_UNKNOWN_ERROR; //return generic error result
    }

    keymaster_free_key_blob(&aes_key_blob);

exit:
    if (!multi_thread)
        km_close();

    return err;

}

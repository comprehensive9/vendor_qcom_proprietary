/*
 * Copyright (c) 2019-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdint.h>
#include <stdio.h>

#include <keymaster_defs.h>
#include <keymaster.h>
#include <keymaster_utils.h>

#include "sample_utils.h"

//text size to be signed
#define TEXT_SIZE (32*1024 + 1)
#define MAX_MAC_SIZE (512)

/*************************************************************************************/
/* GENERATE KEY */
/*************************************************************************************/
static keymaster_error_t import_hmac_key(keymaster_key_blob_t* hmac_key_blob) {
    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;

    //HMAC key (example)
    keymaster_blob_t raw_key_data = {
        .data = (uint8_t*)"1234567812345678",
        .data_length = 16,
    };

    keymaster_key_param_t params[] = {
            keymaster_param_enum(KM_TAG_ALGORITHM, KM_ALGORITHM_HMAC),
            keymaster_param_int(KM_TAG_KEY_SIZE, raw_key_data.data_length*8),

            //minimal MAC length required for usage with this key
            keymaster_param_int(KM_TAG_MIN_MAC_LENGTH, 64),

            //allowed 256bit HASH
            keymaster_param_enum(KM_TAG_DIGEST, KM_DIGEST_SHA_2_256),

            //allow sign and verify operations
            keymaster_param_enum(KM_TAG_PURPOSE, KM_PURPOSE_SIGN),
            keymaster_param_enum(KM_TAG_PURPOSE, KM_PURPOSE_VERIFY),

    };
    const keymaster_key_param_set_t param_set = {
            .params = params,
            .length = sizeof(params)/sizeof(params[0]),
    };

    keymaster_key_characteristics_t hmac_key_characteristics;



    err = km_import_key(&param_set, KM_KEY_FORMAT_RAW, &raw_key_data, hmac_key_blob, &hmac_key_characteristics);
    if (err != KM_ERROR_OK)
        return err;

    printf("hmac key imported successfully with key size of ");
    print_tag_value_from_characterstic(KM_TAG_KEY_SIZE, &hmac_key_characteristics);
    printf("\n");

    //cleanup memory allocated by KM
    keymaster_free_characteristics(&hmac_key_characteristics);

    return KM_ERROR_OK;
}


/*************************************************************************************/
/* SIGN */
/*************************************************************************************/
static keymaster_error_t sign_hmac(const keymaster_key_blob_t* key_blob,
                              const uint8_t* text, size_t text_size,
                              uint8_t* signature, size_t signature_max_size, size_t* signature_size) {

    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;
    keymaster_operation_handle_t op_handle = 0;
    size_t copied_bytes = 0;

    keymaster_key_param_t op_params[] = {
            //Use 256bit hash for HMAC
            keymaster_param_enum(KM_TAG_DIGEST, KM_DIGEST_SHA_2_256),
            //specify the MAC length to be used from the full HASH output
            keymaster_param_enum(KM_TAG_MAC_LENGTH, 128),
    };
    keymaster_key_param_set_t op_params_set = {
            .params = op_params,
            .length = sizeof(op_params)/sizeof(op_params[0]),
    };

    keymaster_key_param_set_t op_out_params_set = {0};

    err = km_begin(KM_PURPOSE_SIGN, key_blob, &op_params_set, &op_out_params_set, &op_handle);
    if (err != KM_ERROR_OK)
        return err;

    printf("HMAC Sign operation handle is 0x%lx\n", op_handle);

    size_t input_consumed = 0;
    keymaster_blob_t plaintext_blob = {
            .data = text,
            .data_length = text_size,
    };

    keymaster_blob_t output_blob = { 0 };
    do {
        err = km_update(op_handle, NULL, &plaintext_blob, &input_consumed, NULL, &output_blob);
        if (err != KM_ERROR_OK)
            break;

        //advance plain text buffer according the processed input
        plaintext_blob.data_length -= input_consumed;
        plaintext_blob.data += input_consumed;

        printf("Sign processed %lu out of %lu bytes.(consumed: %lu) \n",
               text_size-plaintext_blob.data_length, text_size, input_consumed);

    } while(plaintext_blob.data_length > 0); //repeat till all input is consumed

    keymaster_blob_t signature_blob = {0};

    err = km_finish(op_handle, NULL, &plaintext_blob, NULL, NULL, &signature_blob);
    if (err != KM_ERROR_OK)
        return err;

    //copy the signature to the output buffer
    copied_bytes = memscpy(signature, signature_max_size,
                           signature_blob.data, signature_blob.data_length);
    if (copied_bytes != signature_blob.data_length)
        err = KM_ERROR_INSUFFICIENT_BUFFER_SPACE;
    keymaster_free_blob(&signature_blob); //frees internal blob data allocated by KM
    if (err != KM_ERROR_OK)
        return err;

    *signature_size = copied_bytes;

    printf("Sign operation was finished successfully!\n");
    return KM_ERROR_OK;
}

/*************************************************************************************/
/* VERIFY */
/*************************************************************************************/
static keymaster_error_t verify_hmac(const keymaster_key_blob_t* key_blob,
                              const uint8_t* text, size_t text_size,
                              const uint8_t* signature, size_t signature_size) {

    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;
    keymaster_operation_handle_t op_handle;

    keymaster_blob_t signature_blob = {
            .data = signature,
            .data_length = signature_size,
    };

    keymaster_key_param_t op_params[] = {
            keymaster_param_enum(KM_TAG_DIGEST, KM_DIGEST_SHA_2_256),
    };
    keymaster_key_param_set_t op_params_set = {
            .params = op_params,
            .length = sizeof(op_params)/sizeof(op_params[0]),
    };

    keymaster_key_param_set_t op_out_params_set = {0};

    err = km_begin(KM_PURPOSE_VERIFY, key_blob, &op_params_set, &op_out_params_set, &op_handle);
    if (err != KM_ERROR_OK)
        return err;

    printf("HMAC Verify operation handle is 0x%lx\n", op_handle);

    size_t input_consumed = 0;
    keymaster_blob_t plaintext_blob = {
            .data = text,
            .data_length = text_size,
    };

    keymaster_blob_t output_blob = { 0 };
    do {
        err = km_update(op_handle, NULL, &plaintext_blob, &input_consumed, NULL, &output_blob);
        if (err != KM_ERROR_OK)
            break;

        //advance plain text buffer according the processed input
        plaintext_blob.data_length -= input_consumed;
        plaintext_blob.data += input_consumed;

        printf("Verify processed %lu out of %lu bytes.(consumed: %lu) \n",
               text_size-plaintext_blob.data_length, text_size, input_consumed);

    } while(plaintext_blob.data_length > 0); //repeat till all input is consumed



    err = km_finish(op_handle, NULL, &plaintext_blob, &signature_blob, NULL, &output_blob);
    if (err == KM_ERROR_VERIFICATION_FAILED) {
        printf("Verify failed: signature is not matching the input data!\n");
    }
    if (err != KM_ERROR_OK)
        return err;

    printf("Verify operation was finished successfully!\n");
    return KM_ERROR_OK;
}

//sample flow of hmac sign/verify
keymaster_error_t keymaster_hmac_sample(bool multi_thread) {
    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;

    //plain text input
    uint8_t text[TEXT_SIZE] = {0};
    memset(text, 1, sizeof(text));

    //holds the signature
    uint8_t signature[MAX_MAC_SIZE] = { 0 };

    //holds key material generated by keymaster
    keymaster_key_blob_t hmac_key_blob = { 0 };

    size_t signature_size = 0;


    do {
        if (!multi_thread) {
            err = initialize_keymaster();
            if (err != KM_ERROR_OK)
                break;
        }

        err = import_hmac_key(&hmac_key_blob);
        if (err != KM_ERROR_OK)
            break;

        err = sign_hmac(&hmac_key_blob, text, sizeof(text),
                          signature, sizeof(signature), &signature_size);
        if (err != KM_ERROR_OK)
            break;

        printf("Signature of size %lu bytes:\n", signature_size);
        hexdump(signature, signature_size);
        printf("\n");

        err = verify_hmac(&hmac_key_blob, text, sizeof(text), signature, signature_size);
        if (err != KM_ERROR_OK)
            break;

        printf("SUCCESS - HMAC sign and verify completed!\n");

    } while (0);

    if (err != KM_ERROR_OK) {
        printf("Error encountered during keymaster sample: %d\n", err);
    }

    //memory cleanup
    keymaster_free_key_blob(&hmac_key_blob);

    if (!multi_thread) {
        km_close();
    }
    return err;
}

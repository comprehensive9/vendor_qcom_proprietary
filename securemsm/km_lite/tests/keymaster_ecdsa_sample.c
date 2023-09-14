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
#define SIG_SIZE (1024)

/*************************************************************************************/
/* GENERATE KEY */
/*************************************************************************************/
static keymaster_error_t create_ecdsa_key(keymaster_key_blob_t* ecdsa_key_blob) {
    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;

    keymaster_key_param_t params[] = {
            keymaster_param_enum(KM_TAG_ALGORITHM, KM_ALGORITHM_EC),
            keymaster_param_int(KM_TAG_KEY_SIZE, 256),

            //digest
            keymaster_param_enum(KM_TAG_DIGEST, KM_DIGEST_SHA_2_256),

            //allow sign and verify operations
            keymaster_param_enum(KM_TAG_PURPOSE, KM_PURPOSE_SIGN),
            keymaster_param_enum(KM_TAG_PURPOSE, KM_PURPOSE_VERIFY),

    };
    const keymaster_key_param_set_t param_set = {
            .params = params,
            .length = sizeof(params)/sizeof(params[0]),
    };

    keymaster_key_characteristics_t ecdsa_key_characteristics;

    err = km_generate_key(&param_set, ecdsa_key_blob, &ecdsa_key_characteristics);
    if (err != KM_ERROR_OK)
        return err;

    printf("ECDSA key created successfully with key size of ");
    print_tag_value_from_characterstic(KM_TAG_KEY_SIZE, &ecdsa_key_characteristics);
    printf("\n");

    //cleanup memory allocated by KM
    keymaster_free_characteristics(&ecdsa_key_characteristics);

    return KM_ERROR_OK;
}


/*************************************************************************************/
/* SIGN */
/*************************************************************************************/
static keymaster_error_t sign_ecdsa(const keymaster_key_blob_t* key_blob,
                              const uint8_t* text, size_t text_size,
                              uint8_t* signature, size_t signature_max_size, size_t* signature_size) {

    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;
    keymaster_operation_handle_t op_handle = 0;
    size_t copied_bytes = 0;

    keymaster_key_param_t op_params[] = {
            keymaster_param_enum(KM_TAG_DIGEST, KM_DIGEST_SHA_2_256),
    };
    keymaster_key_param_set_t op_params_set = {
            .params = op_params,
            .length = sizeof(op_params)/sizeof(op_params[0]),
    };

    keymaster_key_param_set_t op_out_params_set = {0};

    err = km_begin(KM_PURPOSE_SIGN, key_blob, &op_params_set, &op_out_params_set, &op_handle);
    if (err != KM_ERROR_OK)
        return err;

    printf("Sign operation handle is 0x%lx\n", op_handle);

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
static keymaster_error_t verify_ecdsa(const keymaster_key_blob_t* key_blob,
                              const uint8_t* text, size_t text_size,
                              const uint8_t* signature, size_t signature_size) {

    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;
    keymaster_operation_handle_t op_handle = 0;

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

    printf("Verify operation handle is 0x%lx\n", op_handle);

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

//sample flow of ECDSA sign/verify
keymaster_error_t keymaster_ecdsa_sample(bool multi_thread) {
    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;

    //plain text input
    uint8_t text[TEXT_SIZE] = {0};
    memset(text, 1, sizeof(text));

    //holds the signature
    uint8_t signature[SIG_SIZE] = { 0 };

    //holds key material generated by keymaster
    keymaster_key_blob_t ecdsa_key_blob = { 0 };

    size_t signature_size = 0;


    do {
        if (!multi_thread) {
            err = initialize_keymaster();
            if (err != KM_ERROR_OK)
                break;
        }

        err = create_ecdsa_key(&ecdsa_key_blob);
        if (err != KM_ERROR_OK)
            break;

        err = sign_ecdsa(&ecdsa_key_blob, text, sizeof(text),
                          signature, sizeof(signature), &signature_size);
        if (err != KM_ERROR_OK)
            break;
	printf("signature = %s\n", signature);
        printf("Signature of size %lu bytes:\n", signature_size);
        hexdump(signature, signature_size);
        printf("\n");

        err = verify_ecdsa(&ecdsa_key_blob, text, sizeof(text), signature, signature_size);
        if (err != KM_ERROR_OK)
            break;

        printf("SUCCESS - ECDSA sign and verify completed!\n");

    } while (0);

    if (err != KM_ERROR_OK) {
        printf("Error encountered during keymaster sample: %d\n", err);
    }

    //memory cleanup
    keymaster_free_key_blob(&ecdsa_key_blob);

    //unload the keymaster TA at the end of the test. This will clear the internal keymaster state

    if (!multi_thread)
        km_close();

    return err;
}

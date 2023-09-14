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

//text size to be encrypted
#define TEXT_SIZE  (32*1024 + 1)
//maximum plan/cipher text buffer size for input or output processing by the sample
#define MAX_TEXT_SIZE (TEXT_SIZE + 1024)
//custom IV (in production this need to be randomized)
static uint8_t IV[128/8] = {0x01};

/*************************************************************************************/
/* GENERATE KEY */
/*************************************************************************************/
static keymaster_error_t create_aes_key(keymaster_key_blob_t* aes_key_blob) {
    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;

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

    printf("AES key created successfully with key size of ");
    print_tag_value_from_characterstic(KM_TAG_KEY_SIZE, &aes_key_characteristics);
    printf("\n");

    //cleanup memory allocated by KM
    keymaster_free_characteristics(&aes_key_characteristics);

    return KM_ERROR_OK;
}


/*************************************************************************************/
/* ENCRYPT */
/*************************************************************************************/
static keymaster_error_t encrypt_aes(const keymaster_key_blob_t* key_blob,
                              const uint8_t* plaintext, size_t plaintext_size,
                              uint8_t* ciphertext, size_t ciphertext_max_size, size_t* ciphertext_size) {

    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;
    keymaster_operation_handle_t op_handle = 0;
    size_t copied_bytes = 0;

    keymaster_key_param_t op_params[] = {
            keymaster_param_enum(KM_TAG_PADDING, KM_PAD_PKCS7),
            keymaster_param_enum(KM_TAG_BLOCK_MODE, KM_MODE_CBC),
            keymaster_param_blob(KM_TAG_NONCE, IV, sizeof(IV)),
    };
    keymaster_key_param_set_t op_params_set = {
            .params = op_params,
            .length = sizeof(op_params)/sizeof(op_params[0]),
    };

    keymaster_key_param_set_t op_out_params_set = {0};

    err = km_begin(KM_PURPOSE_ENCRYPT, key_blob, &op_params_set, &op_out_params_set, &op_handle);
    if (err != KM_ERROR_OK)
        return err;

    printf("Encrypt operation handle is 0x%lx\n", op_handle);

    size_t input_consumed = 0;
    *ciphertext_size = 0;
    keymaster_blob_t plaintext_blob = {
            .data = plaintext,
            .data_length = plaintext_size,
    };
    //encrypted text returned by keymaster
    keymaster_blob_t cyphertext_blob = {0};

    do {
        err = km_update(op_handle, NULL, &plaintext_blob, &input_consumed, NULL, &cyphertext_blob);
        if (err != KM_ERROR_OK)
            break;

        //copy encrypted output to the receiving buffer
        copied_bytes = memscpy(ciphertext + *ciphertext_size, ciphertext_max_size - *ciphertext_size,
                                      cyphertext_blob.data, cyphertext_blob.data_length);
        if (copied_bytes != cyphertext_blob.data_length)
            err = KM_ERROR_INSUFFICIENT_BUFFER_SPACE;

        keymaster_free_blob(&cyphertext_blob); //frees internal blob data allocated by KM

        if (err != KM_ERROR_OK)
            return err;

        *ciphertext_size += copied_bytes;

        //advance plain text buffer according the processed input
        plaintext_blob.data_length -= input_consumed;
        plaintext_blob.data += input_consumed;

        printf("Encrypted %lu out of %lu bytes.(consumed: %lu) \n",
               plaintext_size-plaintext_blob.data_length, plaintext_size, input_consumed);

    } while(plaintext_blob.data_length > 0); //repeat till all input is consumed

    err = km_finish(op_handle, NULL, &plaintext_blob, NULL, NULL, &cyphertext_blob);
    if (err != KM_ERROR_OK)
        return err;

    //copy rest of encrypted output (if any) to the receiving buffer
    //copy encrypted output to the receiving buffer
    copied_bytes = memscpy(ciphertext + *ciphertext_size, ciphertext_max_size - *ciphertext_size,
                                  cyphertext_blob.data, cyphertext_blob.data_length);
    if (copied_bytes != cyphertext_blob.data_length)
        err = KM_ERROR_INSUFFICIENT_BUFFER_SPACE;
    keymaster_free_blob(&cyphertext_blob); //frees internal blob data allocated by KM
    if (err != KM_ERROR_OK)
        return err;

    *ciphertext_size += copied_bytes;

    printf("Encryption operation was finished successfully!\n");
    return KM_ERROR_OK;
}


/*************************************************************************************/
/* DECRYPT */
/*************************************************************************************/
static keymaster_error_t decrypt_aes(const keymaster_key_blob_t* key_blob,
                              const uint8_t* ciphertext, size_t ciphertext_size,
                              uint8_t* decryptedtext,
                              size_t decrypted_text_max_size,
                              size_t* decrypted_text_size) {
    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;
    keymaster_operation_handle_t op_handle = 0;
    size_t copied_bytes = 0;

    //using same parameters as encrypt
    //note, that if IV (nonce) was generated by begin during encrypt it has to be passed here as well
    keymaster_key_param_t op_params[] = {
            keymaster_param_enum(KM_TAG_PADDING, KM_PAD_PKCS7),
            keymaster_param_enum(KM_TAG_BLOCK_MODE, KM_MODE_CBC),
            keymaster_param_blob(KM_TAG_NONCE, IV, sizeof(IV)),
    };
    keymaster_key_param_set_t op_params_set = {
            .params = op_params,
            .length = sizeof(op_params) / sizeof(op_params[0]),
    };

    err = km_begin(KM_PURPOSE_DECRYPT, key_blob, &op_params_set, NULL, &op_handle);
    if (err != KM_ERROR_OK)
        return err;

    printf("Decrypt operation handle is 0x%lx\n", op_handle);

    size_t input_consumed = 0;
    *decrypted_text_size = 0;
    keymaster_blob_t decrypted_plaintext_blob = { 0 };
    keymaster_blob_t cyphertext_blob = {
            .data = ciphertext,
            .data_length = ciphertext_size,
    };

    do {
        err = km_update(op_handle, NULL, &cyphertext_blob, &input_consumed,
                        NULL, &decrypted_plaintext_blob);
        if (err != KM_ERROR_OK)
            break;

        //copy decrypted output to the receiving buffer
        copied_bytes = memscpy(decryptedtext + *decrypted_text_size,
                               decrypted_text_max_size - *decrypted_text_size,
                               decrypted_plaintext_blob.data,
                               decrypted_plaintext_blob.data_length);

        if (copied_bytes != decrypted_plaintext_blob.data_length)
            err = KM_ERROR_INSUFFICIENT_BUFFER_SPACE;
        keymaster_free_blob(&decrypted_plaintext_blob);  //frees internal blob data allocated by KM
        if (err != KM_ERROR_OK)
            return err;

        *decrypted_text_size += copied_bytes;

        //advance input buffer according the already processed input
        cyphertext_blob.data_length -= input_consumed;
        cyphertext_blob.data += input_consumed;

        printf("Decrypted %lu out of %lu bytes.(consumed: %lu) \n",
               ciphertext_size - cyphertext_blob.data_length, ciphertext_size,
               input_consumed);

    } while (cyphertext_blob.data_length > 0);  //repeat till all input is consumed

    err = km_finish(op_handle, NULL, &cyphertext_blob, NULL, NULL,
                    &decrypted_plaintext_blob);
    if (err != KM_ERROR_OK)
        return err;

    //copy rest of decrypted output (if any) to the receiving buffer
    copied_bytes = memscpy(decryptedtext + *decrypted_text_size,
                           decrypted_text_max_size - *decrypted_text_size,
                           decrypted_plaintext_blob.data,
                           decrypted_plaintext_blob.data_length);
    if (copied_bytes != decrypted_plaintext_blob.data_length)
        err = KM_ERROR_INSUFFICIENT_BUFFER_SPACE;
    keymaster_free_blob(&decrypted_plaintext_blob);  //frees internal blob data allocated by KM
    if (err != KM_ERROR_OK)
        return err;

    *decrypted_text_size += copied_bytes;

    printf("Decryption operation was finished successfully!\n");
    return KM_ERROR_OK;
}


//sample flow of AES encrypt/decrypt
keymaster_error_t keymaster_aes_sample(bool multi_thread) {
    keymaster_error_t err = KM_ERROR_UNKNOWN_ERROR;

    //plain text input
    uint8_t plaintext[TEXT_SIZE] = {0};
    memset (plaintext, 1, sizeof(plaintext));

    //to hold full cihpertext and decrypted text data
    uint8_t ciphertext[MAX_TEXT_SIZE] = { 0 };
    uint8_t decryptedtext[MAX_TEXT_SIZE] = { 0 };

    //holds key material generated by keymaster
    keymaster_key_blob_t aes_key_blob = { 0 };

    size_t encrypted_size = 0;
    size_t decrypted_size = 0;

    do {
	if (!multi_thread) {
            err = initialize_keymaster();
            if (err != KM_ERROR_OK)
                break;
        }

        err = create_aes_key(&aes_key_blob);
        if (err != KM_ERROR_OK)
            break;

        err = encrypt_aes(&aes_key_blob, plaintext, sizeof(plaintext),
                          ciphertext, sizeof(ciphertext), &encrypted_size);
        if (err != KM_ERROR_OK)
            break;

        err = decrypt_aes(&aes_key_blob, ciphertext, encrypted_size,
                          decryptedtext, sizeof(decryptedtext),
                          &decrypted_size);
        if (err != KM_ERROR_OK)
            break;

        //compare the decrypted result to original text to make sure all went as expected
        if (decrypted_size != sizeof(plaintext)) {
            printf("ERROR: decrypted text size not the same as plain text!(%lu,%lu)\n",
                   decrypted_size, sizeof(plaintext));
            err = KM_ERROR_UNKNOWN_ERROR;
            break;
        }

        if (memcmp(plaintext, decryptedtext, decrypted_size) != 0) {
            printf("ERROR: plain and decrypted text do not match!\n");
            err = KM_ERROR_UNKNOWN_ERROR;
            break;
        }
        printf("SUCCESS - plain and decrypted text are matching!\n");

    } while (0);

    if (err != KM_ERROR_OK) {
        printf("Error encountered during keymaster sample: %d\n", err);
    }

    //memory cleanup
    keymaster_free_key_blob(&aes_key_blob);

    if (!multi_thread)
        km_close();

    return err;
}

/*
 * Copyright (c) 2019-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 */

/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HARDWARE_KEYMASTER2_H
#define HARDWARE_KEYMASTER2_H

#include <keymaster_defs.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

    /**
     * Generates a key, or key pair, returning a key blob and/or a description of the key.
     *
     * Key generation parameters are defined as keymaster tag/value pairs, provided in \p params.
     * See keymaster_tag_t for the full list.  Some values that are always required for generation
     * of useful keys are:
     *
     * - KM_TAG_ALGORITHM;
     * - KM_TAG_PURPOSE;
     *
     * KM_TAG_BLOCK_MODE, KM_TAG_PADDING, KM_TAG_MIN_MAC_LENGTH, KM_TAG_RSA_PUBLIC_EXPONENT
     * and KM_TAG_DIGEST must be specified for algorithms that require them.
     *
     * The following tags may not be specified; their values will be provided by the implementation.
     *
     * - KM_TAG_ORIGIN,
     * - KM_TAG_ROLLBACK_RESISTANCE (or deprecated KM_TAG_ROLLBACK_RESISTANT),
     * - KM_TAG_CREATION_DATETIME
     *
     * \param[in] params Array of key generation param
     *
     * \param[out] key_blob returns the generated key. \p key_blob must not be NULL.  The caller
     * assumes ownership key_blob->key_material and must call keymaster_free_key_blob() to free it.
     *
     * \param[out] characteristics returns the characteristics of the key that was, generated, if
     * non-NULL.  If non-NULL, the caller assumes ownership and must deallocate with
     * keymaster_free_characteristics().  Note that KM_TAG_ROOT_OF_TRUST, KM_TAG_APPLICATION_ID and
     * KM_TAG_APPLICATION_DATA are never returned.
     */
    keymaster_error_t km_generate_key(const keymaster_key_param_set_t* params,
                                      keymaster_key_blob_t* key_blob,
                                      keymaster_key_characteristics_t* characteristics);

    /**
     * Returns the characteristics of the specified key, or KM_ERROR_INVALID_KEY_BLOB if the
     * key_blob is invalid (implementations must fully validate the integrity of the key).
     * app_id and app_data must be the ID and data provided when the key was generated or
     * imported, or empty if KM_TAG_APPLICATION_ID and/or KM_TAG_APPLICATION_DATA were not provided
     * during generation.  Those values are not included in the returned characteristics.  The
     * caller assumes ownership of the allocated characteristics object, which must be deallocated
     * with keymaster_free_characteristics().
     *
     * Note that KM_TAG_APPLICATION_ID and KM_TAG_APPLICATION_DATA are never returned.
     *
     * \param[in] key_blob The key to retrieve characteristics from.
     *
     * \param[in] app_id The client ID data, or NULL if none associated.
     *
     * \param[in] app_id The app data, or NULL if none associated.
     *
     * \param[out] characteristics The key characteristics. Must not be NULL.  The caller assumes
     * ownership of the contents and must deallocate with keymaster_free_characteristics().
     */
    keymaster_error_t km_get_key_characteristics(const keymaster_key_blob_t* key_blob,
                                                 const keymaster_blob_t* app_id,
                                                 const keymaster_blob_t* app_data,
                                                 keymaster_key_characteristics_t* characteristics);

    /**
     * Imports a key, or key pair, returning a key blob and/or a description of the key.
     *
     * Most key import parameters are defined as keymaster tag/value pairs, provided in "params".
     * See keymaster_tag_t for the full list.  Values that are always required for import of useful
     * keys are:
     *
     * - KM_TAG_ALGORITHM;
     * - KM_TAG_PURPOSE;
     *
     * The following tags will take default values if unspecified:
     *
     * - KM_TAG_KEY_SIZE will default to the size of the key provided.
     * - KM_TAG_RSA_PUBLIC_EXPONENT will default to the value in the key provided (for RSA keys)
     *
     * The following tags may not be specified; their values will be provided by the implementation.
     *
     * - KM_TAG_ORIGIN,
     * - KM_TAG_ROLLBACK_RESISTANCE (or deprecated KM_TAG_ROLLBACK_RESISTANT),
     * - KM_TAG_CREATION_DATETIME
     *
     * \param[in] params Parameters defining the imported key.
     *
     * \param[in] key_format specifies the format of the key data in key_data.
     *
     * \param[in] key_data Key data blob provided in the format specified above.
     *
     * \param[out] key_blob Used to return the opaque key blob.  Must be non-NULL.  The caller
     * assumes ownership of the contained key_material.
     *
     * \param[out] characteristics Used to return the characteristics of the imported key.  May be
     * NULL, in which case no characteristics will be returned.  If non-NULL, the caller assumes
     * ownership of the contents and must deallocate with keymaster_free_characteristics().  Note
     * that KM_TAG_APPLICATION_ID and KM_TAG_APPLICATION_DATA are never returned.
     */
    keymaster_error_t km_import_key(const keymaster_key_param_set_t* params,
                                    keymaster_key_format_t key_format,
                                    const keymaster_blob_t* key_data,
                                    keymaster_key_blob_t* key_blob,
                                    keymaster_key_characteristics_t* characteristics);

    /**
     * Exports a public key, returning a byte array in the specified format.
     *
     * Note that symmetric key export is not support
     *
     * \param[in] export_format The format to be used for exporting the key.
     *
     * \param[in] key_to_export The key to export.
     *
     * \param[in] app_id Client ID blob, which must match the blob provided in
     * KM_TAG_APPLICATION_ID during key generation (if any).
     *
     * \param[in] app_data Application data blob, which must match the blob provided in
     * KM_TAG_APPLICATION_DATA during key generation (if any).
     *
     * \param[out] export_data The exported key material.  The caller assumes ownership
     * and should free using keymaster_free_blob()
     */
    keymaster_error_t km_export_key(keymaster_key_format_t export_format,
                                    const keymaster_key_blob_t* key_to_export,
                                    const keymaster_blob_t* app_id,
                                    const keymaster_blob_t* app_data,
                                    keymaster_blob_t* export_data);

    /**
     * Upgrades an old key.  Keys can become "old" in two ways: Keymaster can be upgraded to a new
     * version, or the system can be updated to invalidate the OS version and/or patch level.  In
     * either case, attempts to use an old key will result in keymaster returning
     * KM_ERROR_KEY_REQUIRES_UPGRADE.  This method should then be called to upgrade the key.
     *
     * \param[in] key_to_upgrade The keymaster key to upgrade.
     *
     * \param[in] upgrade_params Parameters needed to complete the upgrade. In particular,
     * KM_TAG_APPLICATION_ID and KM_TAG_APPLICATION_DATA will be required if they were defined for
     * the key.
     *
     * \param[out] upgraded_key The upgraded key blob.
     */
    keymaster_error_t km_upgrade_key(const keymaster_key_blob_t* key_to_upgrade,
                                     const keymaster_key_param_set_t* upgrade_params,
                                     keymaster_key_blob_t* upgraded_key);

    /**
     * Deletes the key, or key pair, associated with the key blob.  After calling this function it
     * will be impossible to use the key for any other operations.  May be applied to keys from
     * foreign roots of trust (keys not usable under the current root of trust).
     *
     * Note that only keys which contain KM_TAG_ROLLBACK_RESISTANCE (or deprecated KM_TAG_ROLLBACK_RESISTANT)
     * in key characteristics can be deleted.
     * Calling this function on non-rollback resistant keys will have no effect.
     *
     * \param[in] key The key to be deleted.
     */
    keymaster_error_t km_delete_key(const keymaster_key_blob_t* key);

    /**
     * Deletes all keys in the hardware keystore. Used when keystore is reset completely.  After
     * calling this function it will be impossible to use any previously generated or imported key
     * blobs for any operations.
     *
     * Note that only keys which contain KM_TAG_ROLLBACK_RESISTANCE (or deprecated KM_TAG_ROLLBACK_RESISTANT)
     * in key characteristics will be deleted.
     * Calling this function will have no effect on non-rollback resistant keys.
     */
    keymaster_error_t km_delete_all_keys(void);

    /**
     * Begins a cryptographic operation using the specified key.  If all is well, begin() will
     * return KM_ERROR_OK and create an operation handle which must be passed to subsequent calls to
     * update(), finish() or abort().
     *
     * It is critical that each call to begin() be paired with a subsequent call to finish() or
     * abort(), to allow the keymaster implementation to clean up any internal operation state.
     * Failure to do this may leak internal state space or other internal resources and may
     * eventually cause begin() to return KM_ERROR_TOO_MANY_OPERATIONS when it runs out of space for
     * operations.  Any result other than KM_ERROR_OK from begin(), update() or finish() implicitly
     * aborts the operation, in which case abort() need not be called (and will return
     * KM_ERROR_INVALID_OPERATION_HANDLE if called).
     *
     * \param[in] purpose The purpose of the operation, one of KM_PURPOSE_ENCRYPT,
     * KM_PURPOSE_DECRYPT, KM_PURPOSE_SIGN or KM_PURPOSE_VERIFY. Note that for AEAD modes,
     * encryption and decryption imply signing and verification, respectively, but should be
     * specified as KM_PURPOSE_ENCRYPT and KM_PURPOSE_DECRYPT.
     *
     * \param[in] key The key to be used for the operation. \p key must have a purpose compatible
     * with \p purpose and all of its usage requirements must be satisfied, or begin() will return
     * an appropriate error code.
     *
     * \param[in] in_params Additional parameters for the operation.
     * KM_TAG_DIGEST and KM_TAG_PADDING might be required based on selected algorithm and parameters.
     * If KM_TAG_APPLICATION_ID or
     * KM_TAG_APPLICATION_DATA were provided during generation, they must be provided here, or the
     * operation will fail with KM_ERROR_INVALID_KEY_BLOB.
     * For operations that require a nonce or IV, on keys that were generated with KM_TAG_CALLER_NONCE,
     * in_params may contain a tag KM_TAG_NONCE.
     * For operations which produce MAC (AES-GCM, HMAC), KM_TAG_MAC_LENGTH must be specified.
     * For AES operations KM_TAG_BLOCK_MODE must be specified.
     *
     * \param[out] out_params Output parameters.  Used to return additional data from the operation
     * initialization, notably to return the IV or nonce (KM_TAG_NONCE) from operations that generate
     *  an IV or nonce. The caller takes ownership of the output parameters array and must free it with
     * keymaster_free_param_set().  out_params may be set to NULL if no output parameters are
     * expected.  If out_params is NULL, and output parameters are generated, begin() will return
     * KM_ERROR_OUTPUT_PARAMETER_NULL.
     *
     * \param[out] operation_handle The newly-created operation handle which must be passed to
     * update(), finish() or abort().  If operation_handle is NULL, begin() will return
     * KM_ERROR_OUTPUT_PARAMETER_NULL.
     */
    keymaster_error_t km_begin(keymaster_purpose_t purpose,
                               const keymaster_key_blob_t* key,
                               const keymaster_key_param_set_t* in_params,
                               keymaster_key_param_set_t* out_params,
                               keymaster_operation_handle_t* operation_handle);

    /**
     * Provides data to, and possibly receives output from, an ongoing cryptographic operation begun
     * with begin().
     *
     * If operation_handle is invalid, update() will return KM_ERROR_INVALID_OPERATION_HANDLE.
     *
     * update() may not consume all of the data provided in the data buffer.  update() will return
     * the amount consumed in *data_consumed.  The caller should provide the unconsumed data in a
     * subsequent call.
     *
     * \param[in] operation_handle The operation handle returned by begin().
     *
     * \param[in] in_params Additional parameters for the operation.  For AEAD modes (AES-GCM),
     * this is used to specify KM_TAG_ADDITIONAL_DATA.  Note that additional data may be provided
     * in multiple calls to update(), but only until input data has been provided.
     *
     * \param[in] input Data to be processed, per the parameters established in the call to begin().
     * Note that update() may or may not consume all of the data provided.  See \p input_consumed.
     *
     * \param[out] input_consumed Amount of data that was consumed by update().  If this is less
     * than the amount provided, the caller should provide the remainder in a subsequent call to
     * update().
     *
     * \param[out] out_params can be set to NULL as it is not used by any of the
     * supported algorithms.
     *
     * \param[out] output The output data, if any.  The caller assumes ownership of the allocated
     * buffer.  output must not be NULL.
     *
     * Note that update() may not provide any output, in which case output->data_length will be
     * zero, and output->data may be either NULL or zero-length (so the caller should always free
     * it).
     */
    keymaster_error_t km_update(keymaster_operation_handle_t operation_handle,
                                const keymaster_key_param_set_t* in_params,
                                const keymaster_blob_t* input, size_t* input_consumed,
                                keymaster_key_param_set_t* out_params, keymaster_blob_t* output);

    /**
     * Finalizes a cryptographic operation begun with begin() and invalidates \p operation_handle.
     *
     * \param[in] operation_handle The operation handle returned by begin().  This handle will be
     * invalidated.
     *
     * \param[in] in_params Additional parameters for the operation.  For AEAD modes, this is used
     * to specify KM_TAG_ADDITIONAL_DATA, but only if no input data was provided to update().
     *
     * \param[in] input Data to be processed, per the parameters established in the call to
     * begin(). finish() must consume all provided data or return KM_ERROR_INVALID_INPUT_LENGTH.
     *
     * \param[in] signature The signature to be verified if the purpose specified in the begin()
     * call was KM_PURPOSE_VERIFY.
     *
     * \param[out] out_params can be set to NULL as it is not used by any of the
     * supported algorithms.
     *
     * \param[out] output The output data, if any.  The caller assumes ownership of the allocated
     * buffer.
     *
     * If the operation being finished is a signature verification or an AEAD-mode decryption and
     * verification fails then finish() will return KM_ERROR_VERIFICATION_FAILED.
     */
    keymaster_error_t km_finish(keymaster_operation_handle_t operation_handle,
                                const keymaster_key_param_set_t* in_params,
                                const keymaster_blob_t* input, const keymaster_blob_t* signature,
                                keymaster_key_param_set_t* out_params, keymaster_blob_t* output);

    /**
     * Aborts a cryptographic operation begun with begin(), freeing all internal resources and
     * invalidating \p operation_handle.
     */
    keymaster_error_t km_abort(keymaster_operation_handle_t operation_handle);

    /**
     * Initializes keymaster library. The initialization should be performed only once.
     * Any keymaster API invoked before the km_init() was successfully called will
     * return KM_ERROR_KEYMASTER_NOT_CONFIGURED.
     *
     * It is the client responsibility to ensure that when km_init() is called there is no other keymaster API
     * operation in the progress.
     *
     * \param[in] ROT "Root of Trust" blob that will be verified for any existing key and
     * embedded in every newly generated key. Changing the ROT will disallow usage of any keys
     * created with different ROT.
     *
     * \param[in] OS_version Version of the OS that will be used to check if key upgrade is needed.
     * Version can be any integer, for example multiple sub-versions can be embedded in it
     * at different bits. The only requirement is that as the actual OS version increases the
     * number passed to this API will increase as well.
     *
     * \param[in] OS_security_patch Used the same way as the OS version number. Can be increased independently
     * of the OS version. By default, when value of OS_security_patch is changed, key upgrade only considers
     * the change in month and year, ignores the day.
     */
    keymaster_error_t km_init(const keymaster_blob_t* ROT, uint32_t OS_version, uint32_t OS_security_patch);
   
    /**
     * keymaster implementation of free(). Called to free memory allocated by keymaster
     * library and returned to the caller. Must be used with APIs which explicitly state that the
     * caller needs to assume responsibility for the allocated memory.
     * It is advised not to call this function directly, but rather use keymaster_free_... wrappers based
     * on the type/struct which contains the memory to be freed.
     */
    void km_free(void* ptr);

    /**
     * calling this function invalidates the library context and releases the KM TA handle. Note that km_init()
     * must be called again after calling km_close() to use any of the keymaster APIs.
     * KM_ERROR_OK is returned if KM was released successfully.
     *
     * It is the client responsibility to ensure that when km_close() is called there is no other keymaster API
     * operation in the progress.
     */
    keymaster_error_t km_close(void);
    
    /**
     * This API is for the use-case when Keymaster TA is fully owned by GuestVM 
     * and should not be called during normal mode of operation
     * 
     * Initializes keymaster library without setting TA and HAL version, 
     * in order to allow to HOST set the proper version of HAL.
     * The initialization should be performed only once.
     * Any keymaster API invoked before the km_init() was successfully called will
     * return KM_ERROR_KEYMASTER_NOT_CONFIGURED.
     *
     * \param[in] ROT "Root of Trust" blob that will be verified for any existing key and
     * embedded in every newly generated key. Changing the ROT will disallow usage of any keys
     * created with different ROT.
     *
     * \param[in] OS_version Version of the OS that will be used to check if key upgrade is needed.
     * Version can be any integer, for example multiple sub-versions can be embedded in it
     * at different bits. The only requirement is that as the actual OS version increases the
     * number passed to this API will increase as well.
     *
     * \param[in] OS_security_patch Used the same way as the OS version number. Can be increased independently
     * of the OS version.
     */
    keymaster_error_t km_vm_init(const keymaster_blob_t* ROT, uint32_t OS_version, uint32_t OS_security_patch);

#ifdef QTI_VIRTUAL_KM
    /**
     * Deletes the key, or key pair, associated with the key blob.  After calling this function it
     * will be impossible to use the key for any other operations.  May be applied to keys from
     * foreign roots of trust (keys not usable under the current root of trust).
     *
     * Note that only keys which contain KM_TAG_ROLLBACK_RESISTANCE (or deprecated KM_TAG_ROLLBACK_RESISTANT) in key characteristics can be deleted.
     * Calling this function on non-rollback resistant keys will have no effect.
     *
     * \param[in] key The key to be deleted.
     *
     * \param[in] in_params Additional parameters for the operation.
     * If KM_TAG_APPLICATION_ID or
     * KM_TAG_APPLICATION_DATA were provided during generation, they must be provided here, or the
     * operation will fail
     */
    keymaster_error_t km_delete_key_ext(const keymaster_key_blob_t* key,
                                        const keymaster_key_param_set_t* in_params);
    /**
     * Deletes the shared key associated with the key blob.  After calling this function it
     * will be impossible to use the shared key for any other operations.
     *
     * Note that only shared keys which contain KM_TAG_ROLLBACK_RESISTANCE (or deprecated KM_TAG_ROLLBACK_RESISTANT) in key characteristics can be deleted.
     * Calling this function on non-rollback resistant shared keys will have no effect.
     *
     * \param[in] key The key to be deleted.
     *
     * \param[in] in_params Additional parameters for the operation.
     * If KM_TAG_APPLICATION_ID or
     * KM_TAG_APPLICATION_DATA were provided during generation, they must be provided here, or the
     * operation will fail
     */
    keymaster_error_t km_delete_shared_key(const keymaster_key_blob_t* key,
                                            const keymaster_key_param_set_t* in_params);

    /**
     * Deletes all shared keys, which the caller VM is the owner,in the hardware keystore. Used when keystore is reset completely.
     * After calling this function it will be impossible to use any previously generated or imported shared keys
     * blobs of the calling VM for any operations.
     *
     * Note that only shared keys which contain KM_TAG_ROLLBACK_RESISTANCE (or deprecated KM_TAG_ROLLBACK_RESISTANT) in key characteristics will be deleted.
     * Calling this function will have no effect on non-rollback resistant keys.
     */
    keymaster_error_t km_delete_all_shared_keys(void);
#endif
#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // HARDWARE_KEYMASTER2_H

/*
 * Copyright (c) 2018,2020-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/**
 * @section IPFM_Overview IPFM Overview
 * IPFM is the interface to the Platform Feature Manager (PFM),
 * which provides access to a managed collection of feature licenses.
 * PFM also refers to an ASCII-encoded license format based on PEM.
 * A complete description of feature licenses is beyond the scope of
 * this document.  From a high level, a feature license is an X.509
 * certificate with extensions that define what features are enabled
 * and under what conditions.  The extensions provide a blacklist.
 * If a value specified by any extension does not match either the query or
 * the state of the device, license validation will fail, with an error
 * code that indicates the first extension where a mismatch was detected.
 * Technically, a Feature ID or FID is an integer that identifies a feature.
 * However, the terms feature, Feature ID, and FID are used somewhat
 * interchangeably in the rest of this document.
 *
 * @subsection IPFM_CBOR CBOR
 * Many of the IPFM methods use CBOR to encode inputs or outputs.  A full
 * description of CBOR is beyond the scope of this document.  One
 * reason it is used in the API is so responses can include additional
 * data in the future without requiring changes to client code or the .idl.
 * With that in mind, client code should not depend on the exact order
 * of items in a CBOR map in a response, or the exact number of items in
 * a CBOR structure/array.  Clients should skip unexpected trailing items in
 * arrays and unknown keys in maps, and should only fail if any expected
 * fields are NOT present.
 *
 * CBOR inputs and outputs are described using both CDDL and a CBOR diagnostic
 * format, which a JSON-like text version of the CBOR data.  Square braces
 * indicate a CBOR array, which may be heterogeneous.  Curly braces
 * indicate a CBOR map.  CBOR maps used by IPFM always use text strings as
 * map key values.
 *
 * @note CBOR permits both definite- and indefinite-length arrays and maps,
 * but IPFM methods only accept and produce definite-length arrays and maps.
 * Depending on the CBOR library used by the client, client code may have to
 * provide additional options to generate definite-length arrays and maps.
 *
 * @subsection IPFM_Distinguished_Names Distinguished Names
 * The normal purpose of an X.509 certificate is for a certificate Issuer
 * to assert that a public key is owned by a Subject.  The Issuer and Subject
 * are identified by Distinguished Names.  A Distingushed Name (DN) is a series
 * of well-known attribute types (keys) and values.  Most key / value pairs are
 * optional.  Several IPFM methods return
 * the Issuer and Subject DNs in a CBOR map.  The values in the CBOR map
 * have been converted from the raw DER encoding into a CBOR map.  Both the
 * keys and values in the map are TSTR values.  The following key values are
 * supported at a minimum.
 * @li @c "O" - Organization
 * @li @c "OU" - Organizational Unit
 * @li @c "CN" - Common Name
 *
 * @subsection IPFM_Validation_Error_Codes Common Validation Error Codes
 * A number of IPFM methods return errors that indicate which restriction
 * failed to match, as described above, or a more general problem with the
 * X.509 certificate structure.  These error codes all include _CERT in
 * their names and are listed here.
 *
 * @par Restriction Mismatch Errors
 * @li @ref ERROR_CERT_PKHASH
 * @li @ref ERROR_CERT_FEATUREID
 * @li @ref ERROR_CERT_EXPIRED
 * @li @ref ERROR_CERT_OEM
 * @li @ref ERROR_CERT_HWVERSION
 * @li @ref ERROR_CERT_LICENSEE_HASH
 * @li @ref ERROR_CERT_DEVICEID
 * @li @ref ERROR_CERT_NOTYETVALID
 * @li @ref ERROR_CERT_PRODUCTID
 *
 * @par Errors for corrupted or incorrectly generated licenses
 * @li @ref ERROR_INVALID_CERT
 * @li @ref ERROR_CERT_NOT_TRUSTED
 * @li @ref ERROR_CERT_GENERAL_ERR
 * @li @ref ERROR_CERT_LEAF_IS_CA
 *
 * @subsection IPFM_Common_Error_Codes Other Common Error Codes
 * @par Object_ERROR_SIZE_OUT
 * Many IPFM methods return variable-length data in CBOR format.  If the output
 * buffer provided by the client is not large enough to hold the data,
 * Object_ERROR_SIZE_OUT will be returned, and the output buffer length will
 * be set to a value large enough to contain the data.  Note that this is
 * an upper bound on the data length, and not necessarily the exact data length.
 * If the client allocates a new buffer of this length and calls the method again
 * with the new buffer, it will succeed, and the buffer length will be set to
 * the exact data size.
 *
 * @par Object_ERROR
 * Object_ERROR can be returned from most methods.  This indicates an
 * internal error of some kind.  It may indicate a system misconfiguration.
 * For example, it can be returned if PFM can not find the persistent storage
 * for licenses. The circumstances under which the error occurred and the
 * qsee_log should be reported to the maintainers of PFM.
 *
 * @par Other error codes
 * Note that while every effort is made to correctly list the errors that
 * each method can return, the list should not be considered exhaustive.
 * A method may return error codes in addition to those listed, either now
 * or in the future.
 */
#pragma once
// AUTOGENERATED FILE: DO NOT EDIT

#include <stdint.h>
#include "object.h"
#include "IDeviceAttestation.h"
#include "IProvisioning.h"

#define IPFMUpdater_OP_Update 0
#define IPFMUpdater_OP_Finalize 1

static inline int32_t
IPFMUpdater_release(Object self)
{
  return Object_invoke(self, Object_OP_release, 0, 0);
}

static inline int32_t
IPFMUpdater_retain(Object self)
{
  return Object_invoke(self, Object_OP_retain, 0, 0);
}

static inline int32_t
IPFMUpdater_Update(Object self, const void *PFMLicense_ptr, size_t PFMLicense_len)
{
  ObjectArg a[1]={{{0,0}}};
  a[0].bi = (ObjectBufIn) { PFMLicense_ptr, PFMLicense_len * 1 };

  return Object_invoke(self, IPFMUpdater_OP_Update, a, ObjectCounts_pack(1, 0, 0, 0));
}

static inline int32_t
IPFMUpdater_Finalize(Object self, void *TransactionInfo_ptr, size_t TransactionInfo_len, size_t *TransactionInfo_lenout)
{
  ObjectArg a[1]={{{0,0}}};
  a[0].b = (ObjectBuf) { TransactionInfo_ptr, TransactionInfo_len * 1 };

  int32_t result = Object_invoke(self, IPFMUpdater_OP_Finalize, a, ObjectCounts_pack(0, 1, 0, 0));

  *TransactionInfo_lenout = a[0].b.size / 1;

  return result;
}


#define IPFM_OPT_NO_OEMID_CHECK UINT64_C(0x00000001)
#define IPFM_OPT_NO_HWVERSION_CHECK UINT64_C(0x00000002)
#define IPFM_OPT_NO_FEATUREID_CHECK UINT64_C(0x00000004)
#define IPFM_OPT_NO_TIME_CHECK UINT64_C(0x00000008)
#define IPFM_OPT_CLOCK_NOT_SET_OK UINT64_C(0x00000010)
#define IPFM_OPT_SELECT_TEST_ROOT UINT64_C(0x00000020)
#define IPFM_OPT_USAGE_ENCRYPT UINT64_C(0x00000040)
#define IPFM_OPT_NO_PKHASH_CHECK UINT64_C(0x00000080)
#define IPFM_OPT_NO_DEVICE_ID_CHECK UINT64_C(0x00000100)
#define IPFM_OPT_NO_START_LICENSE_DATE_CHECK UINT64_C(0x00000200)
#define IPFM_OPT_TRUST_INSECURE_CLK UINT64_C(0x00000400)
#define IPFM_OPT_NO_PRODUCTID_CHECK UINT64_C(0x00000800)
#define IPFM_OPT_CONSUME_GRACE_COUNT UINT64_C(0x00001000)
#define IPFM_OPT_DO_NOT_USE_GRACE_COUNT_FEATURE UINT64_C(0x00002000)
#define IPFM_OPT_SELECT_TEST_LICENSEE_HASH UINT64_C(0x00004000)
#define IPFM_OPT_CLIENT_IS_LICENSEE UINT64_C(0x00008000)
#define IPFM_LICENSE_HAS_OEMID UINT64_C(0x00000001)
#define IPFM_LICENSE_HAS_HWVERSION UINT64_C(0x00000002)
#define IPFM_LICENSE_HAS_DEVICEID UINT64_C(0x00000004)
#define IPFM_LICENSE_HAS_PKHASH UINT64_C(0x00000008)
#define IPFM_LICENSE_HAS_LICENSEEHASH UINT64_C(0x00000010)
#define IPFM_LICENSE_HAS_VALIDTIME UINT64_C(0x00000020)
#define IPFM_LICENSE_HAS_PRODUCTID UINT64_C(0x00000040)
#define IPFM_LICENSE_HAS_GRACE_PERIOD UINT64_C(0x00000080)
#define IPFM_CAPS_ATOMIC_LICENSE_OP UINT64_C(0x00000001)

#define IPFM_ERROR_NOMEM INT32_C(10)
#define IPFM_ERROR_INVALID_PFM_EXTENSION INT32_C(11)
#define IPFM_ERROR_CBOR_ENCODE_ERR INT32_C(12)
#define IPFM_ERROR_CERT_PKHASH INT32_C(13)
#define IPFM_ERROR_INVALID_CERT INT32_C(14)
#define IPFM_ERROR_CERT_FEATUREID INT32_C(15)
#define IPFM_ERROR_CERT_EXPIRED INT32_C(16)
#define IPFM_ERROR_CERT_OEM INT32_C(17)
#define IPFM_ERROR_CERT_HWVERSION INT32_C(18)
#define IPFM_ERROR_CERT_LICENSEE_HASH INT32_C(19)
#define IPFM_ERROR_PFMFILER_FAILED INT32_C(20)
#define IPFM_ERROR_HASH_GENERATION INT32_C(21)
#define IPFM_ERROR_CERT_NOT_TRUSTED INT32_C(22)
#define IPFM_ERROR_CERT_GENERAL_ERR INT32_C(23)
#define IPFM_ERROR_CBOR_DECODE_ERR INT32_C(24)
#define IPFM_ERROR_CBOR_DECODE_DATATYPE_ERR INT32_C(25)
#define IPFM_ERROR_PFMFILER_FILE_REMOVE_FAILED INT32_C(26)
#define IPFM_ERROR_PFMCACHE_REMOVE_FAILED INT32_C(27)
#define IPFM_ERROR_RPMB_ERR INT32_C(28)
#define IPFM_ERROR_FILE_NOT_FOUND INT32_C(29)
#define IPFM_ERROR_PFMFILER_GETFILECONTENTS_FAILED INT32_C(30)
#define IPFM_ERROR_LICENSE_TOO_BIG INT32_C(31)
#define IPFM_ERROR_PRIVILEGE_ERR INT32_C(32)
#define IPFM_ERROR_LICENSE_STORE_FULL INT32_C(33)
#define IPFM_ERROR_CERT_FIELD_TOO_BIG INT32_C(34)
#define IPFM_ERROR_INVALID_CURRENT_TIME INT32_C(35)
#define IPFM_ERROR_FID_LIST_TRUNCATED INT32_C(36)
#define IPFM_ERROR_BLOB_ENCAP_FAILED INT32_C(37)
#define IPFM_ERROR_BLOB_DECAP_FAILED INT32_C(38)
#define IPFM_ERROR_CERT_DEVICEID INT32_C(39)
#define IPFM_ERROR_SFS_ERR INT32_C(40)
#define IPFM_ERROR_CERT_NOTYETVALID INT32_C(41)
#define IPFM_ERROR_OPTS_NOT_SUPPORTED INT32_C(42)
#define IPFM_ERROR_OVERFLOW INT32_C(43)
#define IPFM_ERROR_NOT_GRACE_BOUND INT32_C(44)
#define IPFM_ERROR_CERT_PRODUCTID INT32_C(45)
#define IPFM_ERROR_CERT_LEAF_IS_CA INT32_C(46)
#define IPFM_ERROR_NO_FEATURE_CONFIG INT32_C(47)
#define IPFM_ERROR_DUPLICATE INT32_C(48)
#define IPFM_ERROR_NO_LICENSES INT32_C(49)
#define IPFM_ERROR_INCORRECT_LICENSEE_HASH INT32_C(50)
#define IPFM_ERROR_ACTIVATION_NOT_NEEDED INT32_C(51)
#define IPFM_ERROR_REVOCATION_LIST INT32_C(52)
#define IPFM_ERROR_UNSUPPORTED_REVOCATION_FID INT32_C(53)
#define IPFM_ERROR_REVOCATION_UNSUPPORTED INT32_C(54)
#define IPFM_ERROR_REVOCATION_LIST_FULL INT32_C(55)
#define IPFM_ERROR_REV_LIC_ALREADY_PROCESSED INT32_C(56)
#define IPFM_ERROR_LICENSE_ALREADY_REVOKED INT32_C(57)
#define IPFM_ERROR_INVALID_TRANSACTION_COUNT INT32_C(58)
#define IPFM_ERROR_REVOCATION_FID_UNAVAILABLE INT32_C(59)
#define IPFM_ERROR_REVOKE_SERIAL_NOT_INSTALLED INT32_C(60)
#define IPFM_ERROR_REVOKE_LICENSE INT32_C(61)
#define IPFM_ERROR_FLUSH INT32_C(62)
#define IPFM_ERROR_PFM_SUBMOD_REVOCATION_SERIAL INT32_C(63)
#define IPFM_ERROR_PFM_REPORT_REVOCATION_LICENSE INT32_C(64)
#define IPFM_ERROR_INVALID_PARAM INT32_C(65)

#define IPFM_OP_InstallLicense 0
#define IPFM_OP_CheckInstalledLicense 1
#define IPFM_OP_GetInstalledLicenseInfo 2
#define IPFM_OP_CheckLicenseBuffer 3
#define IPFM_OP_CheckSecured 4
#define IPFM_OP_GetAllInstalledFeatureIDs 5
#define IPFM_OP_GetAllInstalledSerialNumbers 6
#define IPFM_OP_CheckFeatureIds 7
#define IPFM_OP_CheckFIDAndGetAllSerialNums 8
#define IPFM_OP_SetOptions 9
#define IPFM_OP_RemoveLicense 10
#define IPFM_OP_RemoveLicenseExpired 11
#define IPFM_OP_GetLicenseCertPFM 12
#define IPFM_OP_GetFeatureConfig 13
#define IPFM_OP_SetTrustedTime 14
#define IPFM_OP_GetNextExpiration 15
#define IPFM_OP_CheckFIDAndGetAllWithGrace 16
#define IPFM_OP_ActivateGracePeriod 17
#define IPFM_OP_GeneratePFMReport 18
#define IPFM_OP_CreateProvisioning 19
#define IPFM_OP_BeginUpdate 20
#define IPFM_OP_GetAppCapabilities 21

static inline int32_t
IPFM_release(Object self)
{
  return Object_invoke(self, Object_OP_release, 0, 0);
}

static inline int32_t
IPFM_retain(Object self)
{
  return Object_invoke(self, Object_OP_retain, 0, 0);
}

static inline int32_t
IPFM_InstallLicense(Object self, const void *PFMLicense_ptr, size_t PFMLicense_len, void *LicenseSerialNumber_ptr, size_t LicenseSerialNumber_len, size_t *LicenseSerialNumber_lenout, void *FeatureIDsCBOR_ptr, size_t FeatureIDsCBOR_len, size_t *FeatureIDsCBOR_lenout, uint64_t *LicenseRestrictions_ptr)
{
  ObjectArg a[4]={{{0,0}}};
  a[0].bi = (ObjectBufIn) { PFMLicense_ptr, PFMLicense_len * 1 };
  a[1].b = (ObjectBuf) { LicenseSerialNumber_ptr, LicenseSerialNumber_len * 1 };
  a[2].b = (ObjectBuf) { FeatureIDsCBOR_ptr, FeatureIDsCBOR_len * 1 };
  a[3].b = (ObjectBuf) { LicenseRestrictions_ptr, sizeof(uint64_t) };

  int32_t result = Object_invoke(self, IPFM_OP_InstallLicense, a, ObjectCounts_pack(1, 3, 0, 0));

  *LicenseSerialNumber_lenout = a[1].b.size / 1;
  *FeatureIDsCBOR_lenout = a[2].b.size / 1;

  return result;
}

static inline int32_t
IPFM_CheckInstalledLicense(Object self, uint32_t FeatureID_val, const void *LicenseeHash_ptr, size_t LicenseeHash_len, void *LicenseSerialNumber_ptr, size_t LicenseSerialNumber_len, size_t *LicenseSerialNumber_lenout)
{
  ObjectArg a[3]={{{0,0}}};
  a[0].b = (ObjectBuf) { &FeatureID_val, sizeof(uint32_t) };
  a[1].bi = (ObjectBufIn) { LicenseeHash_ptr, LicenseeHash_len * 1 };
  a[2].b = (ObjectBuf) { LicenseSerialNumber_ptr, LicenseSerialNumber_len * 1 };

  int32_t result = Object_invoke(self, IPFM_OP_CheckInstalledLicense, a, ObjectCounts_pack(2, 1, 0, 0));

  *LicenseSerialNumber_lenout = a[2].b.size / 1;

  return result;
}

static inline int32_t
IPFM_GetInstalledLicenseInfo(Object self, const void *LicenseSerialNumber_ptr, size_t LicenseSerialNumber_len, void *LicenseInfoCBOR_ptr, size_t LicenseInfoCBOR_len, size_t *LicenseInfoCBOR_lenout)
{
  ObjectArg a[2]={{{0,0}}};
  a[0].bi = (ObjectBufIn) { LicenseSerialNumber_ptr, LicenseSerialNumber_len * 1 };
  a[1].b = (ObjectBuf) { LicenseInfoCBOR_ptr, LicenseInfoCBOR_len * 1 };

  int32_t result = Object_invoke(self, IPFM_OP_GetInstalledLicenseInfo, a, ObjectCounts_pack(1, 1, 0, 0));

  *LicenseInfoCBOR_lenout = a[1].b.size / 1;

  return result;
}

static inline int32_t
IPFM_CheckLicenseBuffer(Object self, const void *PFMLicense_ptr, size_t PFMLicense_len, uint32_t FeatureID_val, const void *LicenseeHash_ptr, size_t LicenseeHash_len, void *LicenseBufferCBOR_ptr, size_t LicenseBufferCBOR_len, size_t *LicenseBufferCBOR_lenout)
{
  ObjectArg a[4]={{{0,0}}};
  a[0].bi = (ObjectBufIn) { PFMLicense_ptr, PFMLicense_len * 1 };
  a[1].b = (ObjectBuf) { &FeatureID_val, sizeof(uint32_t) };
  a[2].bi = (ObjectBufIn) { LicenseeHash_ptr, LicenseeHash_len * 1 };
  a[3].b = (ObjectBuf) { LicenseBufferCBOR_ptr, LicenseBufferCBOR_len * 1 };

  int32_t result = Object_invoke(self, IPFM_OP_CheckLicenseBuffer, a, ObjectCounts_pack(3, 1, 0, 0));

  *LicenseBufferCBOR_lenout = a[3].b.size / 1;

  return result;
}

static inline int32_t
IPFM_CheckSecured(Object self, const void *SecuredRequest_ptr, size_t SecuredRequest_len, void *SecuredResponse_ptr, size_t SecuredResponse_len, size_t *SecuredResponse_lenout)
{
  ObjectArg a[2]={{{0,0}}};
  a[0].bi = (ObjectBufIn) { SecuredRequest_ptr, SecuredRequest_len * 1 };
  a[1].b = (ObjectBuf) { SecuredResponse_ptr, SecuredResponse_len * 1 };

  int32_t result = Object_invoke(self, IPFM_OP_CheckSecured, a, ObjectCounts_pack(1, 1, 0, 0));

  *SecuredResponse_lenout = a[1].b.size / 1;

  return result;
}

static inline int32_t
IPFM_GetAllInstalledFeatureIDs(Object self, void *FeatureIDsCBOR_ptr, size_t FeatureIDsCBOR_len, size_t *FeatureIDsCBOR_lenout)
{
  ObjectArg a[1]={{{0,0}}};
  a[0].b = (ObjectBuf) { FeatureIDsCBOR_ptr, FeatureIDsCBOR_len * 1 };

  int32_t result = Object_invoke(self, IPFM_OP_GetAllInstalledFeatureIDs, a, ObjectCounts_pack(0, 1, 0, 0));

  *FeatureIDsCBOR_lenout = a[0].b.size / 1;

  return result;
}

static inline int32_t
IPFM_GetAllInstalledSerialNumbers(Object self, void *SerialNumbersCBOR_ptr, size_t SerialNumbersCBOR_len, size_t *SerialNumbersCBOR_lenout)
{
  ObjectArg a[1]={{{0,0}}};
  a[0].b = (ObjectBuf) { SerialNumbersCBOR_ptr, SerialNumbersCBOR_len * 1 };

  int32_t result = Object_invoke(self, IPFM_OP_GetAllInstalledSerialNumbers, a, ObjectCounts_pack(0, 1, 0, 0));

  *SerialNumbersCBOR_lenout = a[0].b.size / 1;

  return result;
}

static inline int32_t
IPFM_CheckFeatureIds(Object self, const void *RequestCBOR_ptr, size_t RequestCBOR_len, void *ResponseCBOR_ptr, size_t ResponseCBOR_len, size_t *ResponseCBOR_lenout)
{
  ObjectArg a[2]={{{0,0}}};
  a[0].bi = (ObjectBufIn) { RequestCBOR_ptr, RequestCBOR_len * 1 };
  a[1].b = (ObjectBuf) { ResponseCBOR_ptr, ResponseCBOR_len * 1 };

  int32_t result = Object_invoke(self, IPFM_OP_CheckFeatureIds, a, ObjectCounts_pack(1, 1, 0, 0));

  *ResponseCBOR_lenout = a[1].b.size / 1;

  return result;
}

static inline int32_t
IPFM_CheckFIDAndGetAllSerialNums(Object self, const void *RequestCBOR_ptr, size_t RequestCBOR_len, void *ResponseCBOR_ptr, size_t ResponseCBOR_len, size_t *ResponseCBOR_lenout)
{
  ObjectArg a[2]={{{0,0}}};
  a[0].bi = (ObjectBufIn) { RequestCBOR_ptr, RequestCBOR_len * 1 };
  a[1].b = (ObjectBuf) { ResponseCBOR_ptr, ResponseCBOR_len * 1 };

  int32_t result = Object_invoke(self, IPFM_OP_CheckFIDAndGetAllSerialNums, a, ObjectCounts_pack(1, 1, 0, 0));

  *ResponseCBOR_lenout = a[1].b.size / 1;

  return result;
}

static inline int32_t
IPFM_SetOptions(Object self, uint64_t uOpts_val)
{
  ObjectArg a[1]={{{0,0}}};
  a[0].b = (ObjectBuf) { &uOpts_val, sizeof(uint64_t) };

  return Object_invoke(self, IPFM_OP_SetOptions, a, ObjectCounts_pack(1, 0, 0, 0));
}

static inline int32_t
IPFM_RemoveLicense(Object self, const void *LicenseSerialNumber_ptr, size_t LicenseSerialNumber_len)
{
  ObjectArg a[1]={{{0,0}}};
  a[0].bi = (ObjectBufIn) { LicenseSerialNumber_ptr, LicenseSerialNumber_len * 1 };

  return Object_invoke(self, IPFM_OP_RemoveLicense, a, ObjectCounts_pack(1, 0, 0, 0));
}

static inline int32_t
IPFM_RemoveLicenseExpired(Object self)
{
  return Object_invoke(self, IPFM_OP_RemoveLicenseExpired, 0, 0);
}

static inline int32_t
IPFM_GetLicenseCertPFM(Object self, const void *LicenseSerialNumber_ptr, size_t LicenseSerialNumber_len, void *PFMLicense_ptr, size_t PFMLicense_len, size_t *PFMLicense_lenout)
{
  ObjectArg a[2]={{{0,0}}};
  a[0].bi = (ObjectBufIn) { LicenseSerialNumber_ptr, LicenseSerialNumber_len * 1 };
  a[1].b = (ObjectBuf) { PFMLicense_ptr, PFMLicense_len * 1 };

  int32_t result = Object_invoke(self, IPFM_OP_GetLicenseCertPFM, a, ObjectCounts_pack(1, 1, 0, 0));

  *PFMLicense_lenout = a[1].b.size / 1;

  return result;
}

static inline int32_t
IPFM_GetFeatureConfig(Object self, uint64_t *fuses_ptr)
{
  ObjectArg a[1]={{{0,0}}};
  a[0].b = (ObjectBuf) { fuses_ptr, sizeof(uint64_t) };

  return Object_invoke(self, IPFM_OP_GetFeatureConfig, a, ObjectCounts_pack(0, 1, 0, 0));
}

static inline int32_t
IPFM_SetTrustedTime(Object self, uint64_t time_val, uint32_t trust_level_val)
{
  ObjectArg a[1]={{{0,0}}};
  struct {
    uint64_t m_time;
    uint32_t m_trust_level;
  } i;
  a[0].b = (ObjectBuf) { &i, 12 };
  i.m_time = time_val;
  i.m_trust_level = trust_level_val;

  return Object_invoke(self, IPFM_OP_SetTrustedTime, a, ObjectCounts_pack(1, 0, 0, 0));
}

static inline int32_t
IPFM_GetNextExpiration(Object self, uint64_t *expiration_ptr)
{
  ObjectArg a[1]={{{0,0}}};
  a[0].b = (ObjectBuf) { expiration_ptr, sizeof(uint64_t) };

  return Object_invoke(self, IPFM_OP_GetNextExpiration, a, ObjectCounts_pack(0, 1, 0, 0));
}

static inline int32_t
IPFM_CheckFIDAndGetAllWithGrace(Object self, const void *RequestCBOR_ptr, size_t RequestCBOR_len, void *ResponseCBOR_ptr, size_t ResponseCBOR_len, size_t *ResponseCBOR_lenout)
{
  ObjectArg a[2]={{{0,0}}};
  a[0].bi = (ObjectBufIn) { RequestCBOR_ptr, RequestCBOR_len * 1 };
  a[1].b = (ObjectBuf) { ResponseCBOR_ptr, ResponseCBOR_len * 1 };

  int32_t result = Object_invoke(self, IPFM_OP_CheckFIDAndGetAllWithGrace, a, ObjectCounts_pack(1, 1, 0, 0));

  *ResponseCBOR_lenout = a[1].b.size / 1;

  return result;
}

static inline int32_t
IPFM_ActivateGracePeriod(Object self, const void *LicenseSerialNumber_ptr, size_t LicenseSerialNumber_len)
{
  ObjectArg a[1]={{{0,0}}};
  a[0].bi = (ObjectBufIn) { LicenseSerialNumber_ptr, LicenseSerialNumber_len * 1 };

  return Object_invoke(self, IPFM_OP_ActivateGracePeriod, a, ObjectCounts_pack(1, 0, 0, 0));
}

static inline int32_t
IPFM_GeneratePFMReport(Object self, const void *license_ptr, size_t license_len, uint32_t options_val, Object *attestation_builder_ptr)
{
  ObjectArg a[3]={{{0,0}}};
  a[0].bi = (ObjectBufIn) { license_ptr, license_len * 1 };
  a[1].b = (ObjectBuf) { &options_val, sizeof(uint32_t) };

  int32_t result = Object_invoke(self, IPFM_OP_GeneratePFMReport, a, ObjectCounts_pack(2, 0, 0, 1));

  *attestation_builder_ptr = a[2].o;

  return result;
}

static inline int32_t
IPFM_CreateProvisioning(Object self, Object *Provisioning_ptr)
{
  ObjectArg a[1]={{{0,0}}};

  int32_t result = Object_invoke(self, IPFM_OP_CreateProvisioning, a, ObjectCounts_pack(0, 0, 0, 1));

  *Provisioning_ptr = a[0].o;

  return result;
}

static inline int32_t
IPFM_BeginUpdate(Object self, Object *Updater_ptr)
{
  ObjectArg a[1]={{{0,0}}};

  int32_t result = Object_invoke(self, IPFM_OP_BeginUpdate, a, ObjectCounts_pack(0, 0, 0, 1));

  *Updater_ptr = a[0].o;

  return result;
}

static inline int32_t
IPFM_GetAppCapabilities(Object self, uint64_t *caps_ptr)
{
  ObjectArg a[1]={{{0,0}}};
  a[0].b = (ObjectBuf) { caps_ptr, sizeof(uint64_t) };

  return Object_invoke(self, IPFM_OP_GetAppCapabilities, a, ObjectCounts_pack(0, 1, 0, 0));
}




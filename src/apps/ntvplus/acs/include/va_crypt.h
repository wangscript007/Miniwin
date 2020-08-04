/******************************************************************************
 
    Copyright
    This code is strictly confidential and the receiver is obliged to use it 
    exclusively for his or her own purposes. No part of Viaccess code may be 
    reproduced or transmitted in any form or by any means, electronic or 
    mechanical, including photocopying, recording, or by any information storage 
    and retrieval system, without permission in writing from Viaccess. 
    The information in this code is subject to change without notice. Viaccess 
    does not warrant that this code is error free. If you find any problems 
    with this code or wish to make comments, please report them to Viaccess.
 
    Trademarks 
    Viaccess is a registered trademark of Viaccess ® in France and/or other 
    countries. All other product and company names mentioned herein are the 
    trademarks of their respective owners.
    Viaccess may hold patents, patent applications, trademarks, copyrights 
    or other intellectual property rights over the code hereafter. Unless 
    expressly specified otherwise in a Viaccess written license agreement, the
    delivery of this code does not imply the concession of any license over 
    these patents, trademarks, copyrights or other intellectual property.
 
******************************************************************************/


#ifndef _VA_CRYPT_H_
#define _VA_CRYPT_H_

#include "va_types.h"
#include "va_errors.h"

/**
 @ingroup sec_drivers
 */

/**
 @defgroup cryptDriver VA_CRYPT API
 @brief 
 The functions described in this section provide mechanisms for the Viaccess-Orca ACS to manage key
 storage, data ciphering and pseudo random number generation. They are supposed to rely on a secure chipset.
* @{ */

/**
 @defgroup cryptConst VA_CRYPT Constants
 @brief The VA_CRYPT constants
* @{ */

/** Predefined ID of the chipset root key */
#define kVA_CRYPT_ROOT_KEY_LVL2_ID1  ((DWORD)0xFFFFFFFD)

/** Predefined ID used for a clear (plain-text) key import */
#define kVA_CRYPT_CLEAR_KEY      ((DWORD)0xFFFFFFFE)

/** Value to specify an invalid key id  */
#define kVA_CRYPT_INVALID_ID     ((DWORD)0xFFFFFFFF)

/** The minimum number of wrapping or ciphering keys that can be stored into the chipset at the same time.
    It means that (at least) kVA_CRYPT_NB_SLOT keys may be imported before @ref kVA_RESOURCE_BUSY is returned. */
#define kVA_CRYPT_NB_SLOT         10

/** Minimal number of simultaneous cipher sessions.
    Means that (at least) kVA_CRYPT_NB_CIPHER sessions of ciphering may be opened before @ref kVA_RESOURCE_BUSY is returned. */
#define kVA_CRYPT_NB_CIPHER        5

/** Few crypto constants */
#define kVA_CRYPT_KEY_SIZE        16  /**< Key size in bytes */
#define kVA_CRYPT_AES_IV_SIZE     16  /**< Initial AES vector size in bytes */
#define kVA_CRYPT_TDES_IV_SIZE     8  /**< Initial TDES vector size in bytes */
#define kVA_CRYPT_AES_BLOCK_SIZE  16  /**< Submultiple of the size of data to be AES ciphered */
#define kVA_CRYPT_TDES_BLOCK_SIZE  8  /**< Submultiple of the size of data to be TDES ciphered */

/**@}*/

/**
  @defgroup cryptEnum VA_CRYPT Enumerations
  @brief The VA_CRYPT enumerations
  @{*/

/** Cipher block algorithm */
typedef enum
{
    eVA_CRYPT_AES_ECB    = 0x01,      /**< AES algorithm in ECB mode */
    eVA_CRYPT_AES_CBC    = 0x02,      /**< AES algorithm in CBC mode */
    eVA_CRYPT_TDES_ECB   = 0x04,      /**< TDES algorithm in ECB mode */
    eVA_CRYPT_TDES_CBC   = 0x08       /**< TDES algorithm in CBC mode */
} tVA_CRYPT_Algorithm ;

/**@}*/

/**
 @defgroup cryptStruct VA_CRYPT Structures
 @brief The VA_CRYPT structures
 @{*/

/** Capabilities supported by the Security chipset */
typedef struct
{
    const char * szChipIdentifier ;   /**< Not used (deprecated, must be set to NULL) */
    DWORD  dwAlgorithms ;             /**< Not used (deprecated, must be set to 0) */
    UINT32 uiNbKeySlots ;             /**< Number of key slots, i.e. maximum number of wrapping and ciphering keys stored at the same time (it must be >= @ref kVA_CRYPT_NB_SLOT) */
    BOOL   bAllowImportClearKey ;     /**< If TRUE, the chipset allows to import a clear key (using kVA_CRYPT_CLEAR_KEY as key id) */
    BOOL   bAllowExportKey ;          /**< If TRUE, the chipset allows to export a wrapped key */
    BOOL   bAllowRandomGeneration ;   /**< If TRUE, the chipset allows pseudo random number generation */
    UINT32 uiMinKeyLevelForCipher ;   /**< Not used (deprecated, must be set to 2) */
    UINT32 uiMaxKeyLevelForCipher ;   /**< Not used (deprecated, must be set to 2) */
} tVA_CRYPT_Capabilities ;

/** This structure specifies the parameters for ciphering operations */
typedef struct
{
    DWORD                dwKeyId ;            /**< The key used to cipher */
    BOOL                 bEncryptNotDecrypt ; /**< Encrypt if true, decrypt if false */
    tVA_CRYPT_Algorithm  eAlgorithm ;         /**< Algorithm used for the ciphering operation */
    const BYTE         * pIV ;                /**< Initial vector (if NULL, a default IV fulfilled with zeros is used) */
    UINT32               uiSizeIV ;           /**< Size of the IV (must be @ref kVA_CRYPT_AES_IV_SIZE for AES and @ref kVA_CRYPT_TDES_IV_SIZE for TDES).
                                                   It is ignored if pIV is NULL.*/
} tVA_CRYPT_Config ;

/**@}*/


/**
 @defgroup cryptFunc VA_CRYPT STB Manufacturer Functions
 @brief The VA_CRYPT functions to be implemented by the STB manufacturer
 @{
*/


/**
*  Gets the cryptographic capabilities of the Secure Chipset device.
*
*  @param[out] pCapabilities     Pointer on a @ref tVA_CRYPT_Capabilities structure that contains the capabilities of the chipset.
*
*  @retval kVA_OK                On success.
*  @retval kVA_INVALID_PARAMETER If `pCapabilities` is NULL.
*/
INT VA_CRYPT_GetCapabilities ( tVA_CRYPT_Capabilities * pCapabilities ) ;

/******************************************************************************/
/* Key management */

/**
   Imports a key into the chipset key store.

   @param[in]  pWrappedKey      The buffer containing the wrapped key to be imported. The key may be in plain-text if the chipset allows
                                clear key import.
   @param[in]  uiWrappedKeySize The size (in bytes) of the buffer. It must be @ref kVA_CRYPT_KEY_SIZE for AES and TDES keys.
   @param[in]  dwWrappingKeyId  The id of the key that wraps the input key. It must be @ref kVA_CRYPT_ROOT_KEY_LVL2_ID1 (the input key is wrapped by the root key),
                                @ref kVA_CRYPT_CLEAR_KEY (the input key is a plain-text key) or one of the already returned id.
   @param[out] pImportedKeyId   Contains the id of the imported key. This id cannot be @ref kVA_CRYPT_ROOT_KEY_LVL2_ID1 or @ref kVA_CRYPT_CLEAR_KEY, since these values are reserved.

   @retval kVA_OK                On success.
   @retval kVA_INVALID_PARAMETER If `dwWrappingKeyId` is not an already returned id or if one of pWrappedKey and pImportedKeyId is null.
   @retval kVA_RESOURCE_BUSY     If there is no more free slot to store this new key.
                                 The chipset provides at least @ref kVA_CRYPT_NB_SLOT slots for the import operation.
   @retval kVA_INVALID_OPERATION If `dwWrappingKeyId` refers to an inappropriate key (not a wrapping key, not a key of level N-1 etc.)
                                 Returned also if the clear keys import is not allowed and @ref kVA_CRYPT_CLEAR_KEY is used as wrapping key.

@b Comments @n In the current implementation, the expected unwrapping algorithm is AES-128 ECB.
*/
INT VA_CRYPT_ImportKey ( const BYTE * pWrappedKey , UINT32 uiWrappedKeySize , DWORD dwWrappingKeyId , DWORD * pImportedKeyId ) ;

/**
   Exports a key from the chipset key store.

   @param[out]    pWrappedKey     The buffer that will contain the output wrapped key.
   @param[in,out] pWrappedKeySize As input, contains the size (in bytes) of the buffer pWrappedKey.
                                  As output, contains the actual size of the wrapped key.
   @param[in]     dwWrappingKeyId The id of the already imported key that wraps the exported key.
                                  This id cannot be @ref kVA_CRYPT_CLEAR_KEY.
   @param[in]     dwExportKeyId   The id of the already imported key that is to be exported.
 
   @retval kVA_OK                 On success.
   @retval kVA_INVALID_PARAMETER  If `dwWrappingKeyId` or `dwExportedKeyId` is not an already returned id, or if pWrappedKey or
                                  pWrapppedKeySize is null.
   @retval kVA_RESOURCE_BUSY      If the buffer `pWrappedKey` is too small. In such a case,
                                  the output value of pWrappedKeySize will contain the actual size needed to achieve the export.
   @retval kVA_NOT_IMPLEMENTED    If the export operation is not allowed by the chipset (see @ref tVA_CRYPT_Capabilities.bAllowExportKey).
   @retval kVA_INVALID_OPERATION  If `dwWrappingKeyId` refers to an inappropriate key for wrapping (not a wrapping key or @ref kVA_CRYPT_CLEAR_KEY),
                                  or if `dwExportKeyId` refers to a key that cannot be exported (@ref kVA_CRYPT_ROOT_KEY_LVL2_ID1).

@b Comments
- In the current implementation, the expected wrapping algorithm is AES-128 ECB.
- A wrapping key of level N can be used only for exporting a key of level N+1.
*/
INT VA_CRYPT_ExportKey ( BYTE * pWrappedKey , UINT32 * pWrappedKeySize , DWORD dwWrappingKeyId , DWORD dwExportKeyId ) ;

/**
   Release a key from the chipset key store.

   @param[in] dwKeyId             The id of the key that is to be released.
                                  From then on, this id is no longer valid and may be returned again
                                  by @ref VA_CRYPT_ImportKey to refer to another key.

   @retval kVA_OK                 On success.
   @retval kVA_INVALID_PARAMETER  If `dwKeyId` is not an already returned id.
*/
INT VA_CRYPT_ReleaseKey ( DWORD dwKeyId ) ;


/******************************************************************************/
/* Ciphering functions */

/**
   Starts a ciphering (encryption or decryption) session.

   @param[in]  pConfig           The parameters of the ciphering session (see @ref tVA_CRYPT_Config).
   @param[out] pHandle           The handle of this session.

   @retval kVA_OK                On success.
   @retval kVA_INVALID_PARAMETER If `pConfig` or `pHandle` is null, or if
                                 `pConfig->dwKeyId` is not a valid id (i.e. not an imported key).
   @retval kVA_INVALID_OPERATION If `pConfig->dwKeyId` is not the id of a ciphering key.
   @retval kVA_RESOURCE_BUSY     If more than @ref kVA_CRYPT_NB_CIPHER simultaneous sessions are opened.
*/
INT VA_CRYPT_CipherOpen ( const tVA_CRYPT_Config * pConfig , DWORD * pHandle ) ;

/**
   Ciphers (encrypts or decrypts) a chunk of data. The data is sent to the function
   in one step (single-part operation) or in multiple steps (multiple-part operation).
   In the case of CBC multiple-part operations, the resulting last 16 bytes of the ciphered block N - 1 are used as IV for the
   next ciphering operation applied to the data block N.@n
   This function does not manage padding for the input data (see the `uiSize` parameter below).
   
   @param[in]  dwHandle          Ciphering session handler (as returned by @ref VA_CRYPT_CipherOpen).
   @param[in]  pSrc              Source buffer to cipher.
   @param[out] pDst              Destination buffer (may overlap the source buffer).
   @param[in]  uiSize            Size in bytes of data (must be a multiple of @ref  kVA_CRYPT_AES_BLOCK_SIZE
                                 for AES or @ref kVA_CRYPT_TDES_BLOCK_SIZE for TDES).

   @retval kVA_OK                On success.
   @retval kVA_INVALID_PARAMETER If `dwHandle` is not a valid handle, or if pSrc or pDst is null, or if uiSize is not correct
                                (must be a multiple of @ref kVA_CRYPT_AES_BLOCK_SIZE for AES and a multiple of @ref kVA_CRYPT_TDES_BLOCK_SIZE for TDES).
   @retval kVA_INVALID_OPERATION If the key used to cipher does no longer exist (released by @ref VA_CRYPT_ReleaseKey)
                                 or it is not a ciphering key (level 2).

@b Comments @n The source and destination buffers may overlap.
*/
INT VA_CRYPT_CipherProcess ( DWORD dwHandle , const BYTE * pSrc , BYTE * pDst , UINT32 uiSize ) ;

/**
   Closes a ciphering session.
   Both single-part operations and multiple-part operations are no longer available with this session.

   @param[in] dwHandle           Ciphering session handler. From then on, this handler is no longer valid and may be returned
                                 again by @ref VA_CRYPT_CipherOpen.
 
   @retval kVA_OK                On success.
   @retval kVA_INVALID_PARAMETER If `dwHandle` is not a valid handle.
*/
INT VA_CRYPT_CipherClose ( DWORD dwHandle ) ;

/******************************************************************************/
/* Random function */

/**
   Generates 32 bits of random data.

  @return A randomly generated 32 bits unsigned integer.

@b Comments @n This function just returns an undefined, useless value if @ref tVA_CRYPT_Capabilities.bAllowRandomGeneration
is FALSE ( as returned by @ref VA_CRYPT_GetCapabilities).
*/
UINT32 VA_CRYPT_GetRandom ( void ) ;

/**@}*/
/**@}*/

#endif /* _VA_CRYPT_H_ */

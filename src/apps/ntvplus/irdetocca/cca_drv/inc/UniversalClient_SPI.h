/*
 * Copyright 2009-2016 Irdeto B.V.
 *
 * This file and the information contained herein are the subject of copyright
 * and intellectual property rights under international convention. All rights
 * reserved. No part of this file may be reproduced, stored in a retrieval
 * system or transmitted in any form by any means, electronic, mechanical or
 * optical, in whole or in part, without the prior written permission of Irdeto
 * B.V.
 */

/**
 * @file UniversalClient_SPI.h
 * Header for Service Provider Interfaces (SPIs). This file contains the headers for the common Service Provider 
 * Interfaces that must be implemented on a platform. 
 *
 * \note It is the integrator's responsibility to implement these interfaces for a particular platform.
 */

#ifndef UNIVERSALCLIENT_SPI_H__INCLUDED__
#define UNIVERSALCLIENT_SPI_H__INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

#include "UniversalClient_API.h"

/**
 * (0x00000001) Current version of the Cloaked CA Agent SPI
 *
 * This is the value returned from ::UniversalClientSPI_GetVersion. 
 * It is used by the Cloaked CA Agent to ensure that the version of the
 * SPI implementation is not newer than the version of the client. 
 * A newer version of the SPI implementation 
 * is not compatible with an old version of the Cloaked CA Agent. 
 */
#define UNIVERSALCLIENTSPI_VERSION 0x00000001

/* Types */
/**
 * Semaphore handle type
 *
 * This is an implementation-specific handle type that is used in the \ref semaphorespi "Cloaked CA Agent Semaphore SPIs".
 *
 * \see ::UniversalClientSPI_Semaphore_Open
 * \see ::UniversalClientSPI_Semaphore_Close
 * \see ::UniversalClientSPI_Semaphore_Post
 * \see ::UniversalClientSPI_Semaphore_Wait
 * \see ::UniversalClientSPI_Semaphore_WaitTimeout
 */
typedef void *uc_semaphore_handle;

/**
 * Mutex handle type
 *
 * This is an implementation-specific handle type that is used in the \ref mutexspi "Cloaked CA Agent Mutex SPIs".
 *
 * \see ::UniversalClientSPI_Mutex_Open
 * \see ::UniversalClientSPI_Mutex_Close
 * \see ::UniversalClientSPI_Mutex_Unlock
 * \see ::UniversalClientSPI_Mutex_Lock
 */
typedef void *uc_mutex_handle;


/**
 * Thread handle type
 *
 * This is an implementation-specific handle type that is used in the \ref threadspi "Cloaked CA Agent Thread SPIs".
 *
 * \see ::UniversalClientSPI_Thread_Open
 * \see ::UniversalClientSPI_Thread_Sleep
 * \see ::UniversalClientSPI_Thread_Close
 */
typedef void *uc_thread_handle;

/**
 * Thread procedure type
 *
 * This is the prototype of the thread procedure pointer that is passed to ::UniversalClientSPI_Thread_Open. 
 */
typedef void (*uc_threadproc)(void* lpVoid);

/**
 * Copy capability information for copy control.
 *
 * The STB driver should use these values to control actions of content copy.
 */
typedef enum _uc_copycontrol_cci
{
    UC_COPYCONTROL_COPYFREELY = 0x0,/*Copy free:Copy is permitted*/
    UC_COPYCONTROL_NOFURTHERCOPY,   /*Copy No More: No further copying is permitted*/
    UC_COPYCONTROL_COPYONCE,        /*Copy Once: One generation copy is permitted*/
    UC_COPYCONTROL_COPYNEVER,       /*Copy never: Copying is prohibited*/
    UC_COPYCONTROL_COPYINVALID      /*Initial value*/
}uc_copycontrol_cci;


/**
 * The max size of URI raw message length.
 */
#define RAW_URI_MESSAGE_LENGTH 128

/**
 * Data structure for CI+ raw data.
 */
typedef struct _uc_service_data_type_ciplus_uri_raw_bytes
{
    /**
     * CI+ raw data length.
     */
    uc_byte length;

    /**
     * Store CI+ raw data.
     */
    uc_byte uri_raw_message[RAW_URI_MESSAGE_LENGTH]; 
}uc_service_data_type_ciplus_uri_raw_bytes;


/**
 * Data structure for Copy Control Information, and it's known as CI Plus Uniform Rules Information (URI) Message.
 */
typedef struct _uc_copy_control_info
{
    /**
     * 8-bits, this field is the Version of URI, default value is 0x01, 0x00 is an invalid value.
     */
    uc_byte protocol_version;
    
    /**
     * If the value is not 0xFF, then it is a 2-bit field with the lower 2 bits.
     * 2-bit field describes the Analogue Protection System(APS) information. It is known as CCI analogue bits in ECM Header. 
     * The full 8 bits of the uc_byte are initialized to 0xFF, which can be taken as an invalid value. 
     */
    uc_byte aps_copy_control_info;
    
    /**
     * If the value is not 0xFF, then it is a 2-bit field with the lower 2 bits.
     * 2-bit field describes the Encryption Mode Indicator(EMI) information. It is known as CCI digital bits in ECM Header. 
     * The full 8 bits of the uc_byte are initialized to 0xFF, which can be taken as an invalid value. 
     */
    uc_byte emi_copy_control_info;
    
    /**
     * If the value is not 0xFF, then it is a 1-bit field with the lower 1 bit.
     * 1-bit field describes the Image Constrained Trigger(ICT) information. 
     * The full 8 bits of the uc_byte are initialized to 0xFF, which can be taken as an invalid value. 
     */
    uc_byte ict_copy_control_info;
    
    /**
     * If the value is not 0xFF, then it is a 1-bit field with the lower 1 bit.
     * 1-bit field describes the Encryption Plus Non-assert(RCT) information.
     * The full 8 bits of the uc_byte are initialized to 0xFF, which can be taken as an invalid value. 
     */
    uc_byte rct_copy_control_info;
    
    /**
     * If protocol_version=1, then it is a 6-bits field with the lower 6 bits; if protocol_version=2, then it is a 8-bits field.
     * This field describes the retention limit after the recording and / or time-shift or an event is completed.
     * The full 8 bits of the uc_byte are initialized to 0xFF, which can be taken as an invalid value, 
     * with one exception, which is if protocol_version=2 and emi_copy_control_info=3, 0xFF indicates unlimited retention period.
     */
    uc_byte rl_copy_control_info;
    
    /**
     * If the value is not 0xFF, then it is a 1-bit field with the lower 1 bit.
     * 1-bit field describes the Digital Only Token(DOT) information. 
     * The full 8 bits of the uc_byte are initialized to 0xFF, which can be taken as an invalid value. 
     */
    uc_byte dot_copy_control_info;

    /**
     * the URI message is stored in this section. 
     */
    uc_service_data_type_ciplus_uri_raw_bytes uri_raw_bytes;
}uc_copy_control_info;

/**
 * Data structure for Macrovision mode. 
 */
typedef struct _uc_macrovision_mode
{
    /**
     *Only lower 6 bits are valid.
     */
    uc_byte mode;
} uc_macrovision_mode;

/**
 * Data structure for Macrovision configuration data.
 */
typedef struct _uc_macrovision_config
{
    /**
     *Version of macrovision, version=7 by default
     */
    uc_byte macrovisionVersion;
    /**
     *Configure data for macrovision
     */
    uc_buffer_st macrovisionConfig;
} uc_macrovision_config;

/**
 * Data structure for extra trick mode control.
 */
typedef struct _uc_extra_trick_mode_control
{
    /**
     * Is extra trick mode control enabled.
     *
     * If this value is true, extra trick mode control is enabled and controlInfo is valid.
     * If this value is false, extra trick mode control is disabled and controlInfo is invalid.
     */
    uc_bool isEnabled;

    /**
     * Information of extra trick mode control.
     *
     * \code 
     * Field Description            Byte#   Bit#    Value   Description
     * extra trick mode control     0       7..5    000     No restrictions
     *                                              001     No skipping / jumping, fast forward limited to 2x speed, visible content
     *                                              010     No skipping / jumping, fast forward limited to 4x speed, visible content
     *                                              011     No skipping / jumping, fast forward limited to 8x speed, visible content
     *                                              100     Reserved for future use
     *                                              101     Reserved for future use
     *                                              110     Reserved for future use
     *                                              111     No skipping / jumping / fast forward allowed    
     *
     * Reserved                      0      4..0    -       Reserved bits for future usage
     *
     * Reserved                      1..2   -       -       Reserved bytes for future usage
     *
     * \endcode 
     */

     /**
     * \note There are no restrictions to fast re-wind, reverse jumping or reverse skipping.  \n\n
     *       Skip: The facility to move through the recording by a fixed amount of time, for example 10 seconds, 1 minute, 5 minutes.  \n
     *       Jump: The facility to go directly to a specific location within the recording(for example bookmarked locations, chapters etc.). 
     */     
    uc_byte controlInfo[3];

} uc_extra_trick_mode_control;

/**
 * Fatal error type.
 */
typedef enum _uc_fatal_error_type
{
    /**
     *Out of memory.
     */
    UC_FATAL_ERROR_MEMORY = 0x0,
    
    /**
     *Persistent storage is damaged.
     */
    UC_FATAL_ERROR_PS_DAMAGED = 0x1
}uc_fatal_error_type;

/**
 * Data structure for smartcard interfaces. 
 */

/** Type of smartcard errors.
 *
 */
typedef enum _uc_smartcard_error_type
{
    /** An error has happened on communications. 
     */
    UC_SC_COMM_ERROR,

    /** Some hardware error occurred. 
     */
    UC_SC_HARDWARE_ERROR,

    /** The card is removed from the reader. 
     */
    UC_SC_CARD_OUT_ERROR,

    /** The communiations have timed-out. 
     */
    UC_SC_TIME_OUT,

    /** The ATR received from the card is not up to the ISO7816-3 spec. 
     */
    UC_SC_ATR_INVALID_FORMAT,

    /** The ATR received from the card is not an Irdeto Access ATR. 
     */
    UC_SC_ATR_INVALID_HISTORY,

    /** JavaCard does not support Logical Channels.
     */
    UC_SC_NO_LOGICAL_CHAN_SUPPORT
} uc_smartcard_error_type;

/**
 * The error info for smartcard.
 */
typedef struct _uc_smartcard_error_info
{
    /** The card error type.
     */
    uc_smartcard_error_type cardErrorType;    
} uc_smartcard_error_info;

/** The smartcard ATR info
 * 
 */
typedef struct _uc_smartcard_ATR_info
{
    /** Length of the ATRData in bytes.
     */
    uc_uint16 ATRLength;

    /** The ATR data.
     * The buffer of ATRData shall be managed by application.
     */
    uc_byte* ATRData;
} uc_smartcard_ATR_info;

/** ATR info for Javecard channel.
 * 
 */
typedef struct _uc_smartcard_ATR_java_card_channel_info
{
    /** The ATR Info.
     * In some cases where the low-level SD-card smartcard driver does not send an ATR, the ATR that can be 
     * used to inform CloakedCA Agent is 0x3B 0x80 0x01 0x81
     */
    uc_smartcard_ATR_info ATRInfo;

    /** The channel number.
     */
    uc_byte channelNumber;
} uc_smartcard_ATR_java_card_channel_info;

/** The type of the smartcard status.
 *
 * This is used in the callback method provided by CloakedCA Agent to application ::uc_sc_status_notify_fn.
 * The corresponding pvInfo parameter in that callback method is also defined here.
 */
typedef enum _uc_sc_status_type
{
    /**
     * A card is inserted into the slot.
     * 
     * The pvInfo carries the ATR read from smartcard:
     * pvInfo.bytes points to an instance of ::uc_smartcard_ATR_info and pvInfo.length is the length of the instance.
     */
    UC_SC_CARD_IN,
    
    /**
     * A card is removed from the slot.
     * 
     * The pvInfo parameter shall be set to NULL.
     */
    UC_SC_CARD_OUT,
    
    /**
     * An error has occurred on insertion of the card.
     *
     * The pvInfo carries the actual error info:
     * pvInfo.bytes points to an instance of ::uc_smartcard_error_info and pvInfo.length is set to the
     * length of the instance.
     * 
     */
    UC_SC_CARD_IN_ERROR,
    
    /**
     * A JavaCard / SIM smartcard has been inserted.
     *
     * This is only applicable to JavaCards.
     *
     * The pvInfo carries the ATR info and Javecard channel number:
     * pvInfo.bytes points an instance of ::uc_smartcard_ATR_java_card_channel_info, and pvInfo.length is set to
     * the length of the instance.
     */
    UC_SC_JAVACARD_CHANNEL_OPENED
} uc_sc_status_type;

/**
 * Handle to a smartcard object 
 *
 * A smartcard handle is an implementation-specific type that is returned from
 * a call to ::UniversalClientSPI_Smartcard_Open. 
 * A smartcard object is a logical construct that represents a source smartcard. 
 */
typedef uc_uintptr uc_smartcard_handle;

/**
 * To save time info.
 *
 * Cloaked CA Agent uses the time info for features where timing control is needed, 
 * and also uses it to evaluate the performance.
 * The time info can be a real time or the time since reboot. 
 */
typedef struct _uc_time
{
    /**
     * seconds of current time.
     */
    uc_uint32 second;

    /**
     * millisecond of current time. NOTE: millisecond but NOT microsecond!
     */
    uc_uint32 millisecond;
} uc_time;

/**
 * cryptography type enumeration.
 *
 * This type is used by the ::UniversalClientSPI_Crypto_Verify to denote the cryptography algorithm 
 * type which used in manufacturer provided verification process. 
 */

 typedef enum
 {
    /**
     * UNKNOWN asymmetric cryptography
     */
    UC_CRIPYTO_UNKNOWN = 0,
 
    /**
     * RSA asymmetric cryptography
     */
    UC_CRYPTO_RSA = 1
 }uc_crypto_type;

/**
 *max Length of rsa modulus.
 *
 *Define the max size of rsa MODULUS that support by rsa key currently. 
 */
#define MAX_RSA_MODULUS_LEN 256

/**
 *max Length of rsa exponent.
 *
 *Define the max size of rsa EXPONENT that support by rsa key currently. 
 */
#define MAX_RSA_EXPONENT_LEN 256


/**
 *RSA algorithm mode been used.
 *
 *Currently, only SHA256 is supported.
 */
typedef enum
{
  /**
   *SHA256 mode
   */
   UC_SHA_ALGORITHM_SHA256  =  0x0
}uc_sha_algorith_mode;

/**
 *RSA algorithm mode been used for digital signature verification.
 *
 *Currently, only PSS is supported.
 */
typedef enum _uc_rsa_alg_mode
{
  /**
   *PSS mode
   */
    UC_RSASSA_PSS      =   0x0
}uc_rsa_algorith_mode;

/**
 *Used to indicate if the secure chipset supports the IFCP module.
 *
 *As a part of uc_device_security_state to provide support information.
 */
typedef enum {
    /** 
    * Not support IFCP. 
    */
    UC_IFCP_MODE_NOT_SUPPORTED = 0,

    /** 
    * Support IFCP. 
    */
    UC_IFCP_MODE_STANDARD = 1
} uc_IFCP_mode;

/**
 *RSA key information for the rsa digital signature verification.
 *
 *As a part of uc_rsa_st to provide key info.
 */
typedef struct _uc_rsa_pk_st
{
  /**
   *length of modulus in byte
   */
    uc_uint32  modulusLength;                              

  /**
   *public/private key's Modulus
   */
    uc_byte modulus[MAX_RSA_MODULUS_LEN];    
 
   /**
   *length of Exponent in byte
   */
    uc_uint32  exponentLength;   

  /**
   *public/private key's Exponent
   */
    uc_byte exponent[MAX_RSA_EXPONENT_LEN];  
} uc_rsa_pk_st;



/**
  *RSA information for the RSA digital signature verification.
  *
  *If the cryptoType equals to UC_CRYPTO_RSA, this structure will be used as the parameter be
  *passed to UniversalClientSPI_Crypto_Verify
  */
typedef struct _uc_rsa_st
{
    /**
     *data to be signed
     */
    uc_byte*      pDataToBeSigned;     

    /**
     *size of data to be signed, i.e. buffer size of pDataToBeSigned.
     */
    uc_uint32       dataLength;   

    /**
     *signature for data in the buffer pDataToBeSigned.
     */
    const uc_byte*      pSignature;  

    /**
     *size of signature, i.e. buffer size of pSignature.
     */
    uc_uint32       signatureLength;

    /**
     * key for the signature
     */
    uc_rsa_pk_st*           pKey;       

    /**
     *RSA algorithm mode
     */
    uc_rsa_algorith_mode        rsaMode;    

    /**
     *SHA algorithm mode
     */
    uc_sha_algorith_mode        shaMode;  

}uc_rsa_st;

/**
 * cryptography information structure
 *
 * This is the structure containing cryptography information, which is used by the manufacturer to provide verification.
 * This structure is used by ::UniversalClientSPI_Crypto_Verify.
 */
 typedef struct _uc_crypto_info
 {
    /**
     * crypto type
     */
    uc_crypto_type cryptoType;

    /**
     * parameter of crypto info can change according to crypto algorithm.
     *
     * currently, only RSA crypto algorithm is supported,.
     */
    union
    {
        /**
         * if  cryptoType equals to UC_CRYPTO_RSA. 
         */
        uc_rsa_st rsaInfo;
    }param;
 }uc_crypto_info;


/**
 * Device security state structure
 *
 * This is a structure that is passed to a stream implementation from 
 * a call to ::UniversalClientSPI_Device_GetSecurityState. It contains security state information about
 * the device. 
 * \note The status data of RSA Mode, JTAG and CW Mode should be read from secure chipset registers.
 */
typedef struct _uc_device_security_state
{
    /**
     * Status of the secure boot loader.
     *
     * \code     
     * Field Description                    Bit# (LSB)   Value   Description
     * Secure Boot                          0            0       Secure boot disabled
     *                                                   1       Secure boot enabled
     * Background Boot Sector Checking      1            0       Background boot check disabled
     *                                                   1       Background boot check enabled
     * Clear Content Protection in Memory   2            0       Clear content protection disabled
     *                                                   1       Clear content protection enabled
     * Reserved for future                  3..31        0..0    Set all bits to zero
     *
     * \endcode 
     * Examples: 
     * 0x05 represents Secure boot enabled, Background boot check disabled and Clear content protection enabled.
     */
    uc_uint32 rsaMode;

    /**
     * Status of the JTAG port.
     *
     * \code 
     * Field Description    Bit# (LSB)   Value   Description
     * JTAG_KEYED           0            0       JTAG interface does not require a password
     *                                   1       JTAG interface requires a password to re-open it
     * JTAG_DIS             1            0       JTAG interface is not disabled
     *                                   1       JTAG interface is disabled (never possible to access or re-open JTAG port)
     * Reserved for future  2..31        0..0    Set all bits to zero
     *
     * \endcode 
     * Examples: 
     * 0x01 represents JTAG interface requires a password to re-open it.
     * 0x02 represents JTAG interface is disabled (never possible to access or re-open JTAG port).
     */
    uc_uint32 jtag;

    /**
     * Type of CW the descrambler accepts.
     *
     * \code 
     * Field Description                    Bit# (LSB)   Value   Description
     * CW_ENF                               0            0       Clear CW mode is not disabled
     *                                                   1       Clear CW mode is disabled and secure CW mode is enforced
     * Reserved for future                  1            0       Set to zero
     * CW_3L_ENF                            2            0       3-level key ladder for secure CW is not enforced
     *                                                   1       3-level key ladder for secure CW is enforced
     * Reserved for future                  3..31        0..0    Set all bits to zero
     *
     * \endcode 
     * Examples: 
     * 0x01 represents Clear CW mode is disabled and secure CW mode is enforced.
     * 0x04 represents 3-level key ladder for secure CW is enforced.
     *
     */
    uc_uint32 cwMode;

    /**
     * crypto type for protecting key.
     *
     * \code 
     * Field Description        Bit# (LSB)   Value   Description
     * Secure CW mode Cipher    0            0       TDES not supported
     *                                       1       TDES supported
     *                          1            0       AES not supported
     *                                       1       AES supported
     *                          2            0       2-level ladder only
     *                                       1       3-level ladder supported
     *                          3            Reserve for future, set to zero.
     *
     * Secure PVR mode Cipher   4            0       TDES not supported
     *                                       1       TDES supported
     *                          5            0       AES not supported
     *                                       1       AES supported
     *                          6            0       2-level ladder only
     *                                       1       3-level ladder supported
     *                          7           Reserve for future, set to zero.
     *
     * \endcode 
     * Examples: 
     * 0x21 represents secure CW mode supporting the 2-level TDES cipher, and secure PVR mode supporting the 2-level AES cipher.
     * 0x13 represents secure CW mode supporting the 2-level both TDES and AES cipher, and secure PVR mode supporting the 2-level TDES cipher.
     */
    uc_byte crypto;
    
     /**
     * Type of IFCP support mode.
     *
     * \code 
     * Field Description                    Value   Description
     * IFCP support mode                    0       Not support IFCP
     *                                      1       Support IFCP
     * \endcode
     *
     * \note Refer to chip's specification or contact chip vendor to know the IFCP support mode.
     */
    uc_IFCP_mode modeIFCP;

} uc_device_security_state;


/**
 * PVR key ladder security state structure
 *
 * This is a structure that is passed to a SPI ::UniversalClientSPI_Device_GetPVRSecurityState. 
 */
typedef struct _uc_pvr_security_state
{
    /**
     * Whether the PVR key ladder is valid.
     */
    uc_bool valid;

    /**
     * Count of protecting levels in the PVR key ladder.
     * for example it may be 2 or 3.
     * If the PVR key ladder is used for PVR recording/playback, only 3 is supported currently.
     */
    uc_uint32 level;

    /**
     * The algorithm used in the key ladder.
     */
    uc_dk_protection algorithm;
} uc_pvr_security_state;

/**
 * Size of CCC MAC
 */
#define UC_CCC_MAC_SIZE 16

/**
 * Identifier of encryption key
 */
typedef enum
{
    /**
     * CCCK
     */
    UC_ENCRYPTION_KEY_ID_CCCK   = 6
}uc_encryption_key_id;

/**
 * Encryption algorithm
 */
typedef enum
{
    /**
     * TDES
     */
    UC_ENCRYPTION_ALGORITHM_TDES        = 1,

    /**
     * AES
     */
    UC_ENCRYPTION_ALGORITHM_AES         = 2
}uc_encryption_algorithm;

/**
 * Chip configuration request structure
 *
 * This is a structure that is passed to a SPI ::UniversalClientSPI_Device_GetChipConfigurationCheck. 
 */
typedef struct _uc_chip_configuration_request 
{
    /**
    * Identifier of encryption key
    */     
    uc_encryption_key_id keyId; 

    /**
    * Encryption algorithm
    */     
    uc_encryption_algorithm algorithm; 

    /**
    * The mask used for the Chip Configuration Check.
    */     
    uc_byte mask[UC_CCC_MAC_SIZE];
} uc_chip_configuration_request;

/**
 * Chip configuration response structure
 *
 * This is a structure that is passed to a SPI ::UniversalClientSPI_Device_GetChipConfigurationCheck. 
 */
typedef struct _uc_chip_configuration_response 
{
    /**
    * 128-bit CBC-MAC 
    */
    uc_byte chipConfigurationMac[UC_CCC_MAC_SIZE];        
} uc_chip_configuration_response;

/**
 * Device platform identifiers structure
 *
 * This is a structure that is passed to a stream implementation from 
 * a call to ::UniversalClientSPI_Device_GetPlatformIdentifiers. It contains platform identifiers about
 * the device. 
 * \note Some platforms may not have all the information in this structure if no Irdeto Loader exists.
 */
typedef struct _uc_device_platform_identifiers
{
    /**
     * System id from loader.
     */
    uc_uint16 systemId;

    /**
     * Variant from loader.
     */    
    uc_uint16 variant;

    /**
     * Key version from loader.
     */ 
    uc_uint16 keyVersion;

    /**
     * Signature version from loader.
     */
    uc_uint16 signatureVersion;
    
    /**
     * Manufacturer ID from loader.
     */
    uc_uint16 manufacturerId;

    /**
     * Loader version from loader.
     * The most significant 8 bits stands for Loader Major Version whereas the least significant 8 bits stands for
     * Loader Minor Version.
     */
    uc_uint16 loaderVersion;

    /**
     * Hardware version from loader.
     */
    uc_uint16 hardwareVersion;

    /**
     * Download version number from loader.
     */
    uc_uint16 loadVersion;

} uc_device_platform_identifiers;

/**
 * Multi-2 parameters structure
 *
 * This is a structure that is passed to a SPI ::UniversalClientSPI_Device_SetMulti2Parameter. 
 */
typedef struct _uc_device_multi2_parameter
{
    /**
     * System Key. (32 bytes)
     */
    uc_buffer_st systemKey;

    /**
     * Initialization data. (8 bytes)
     */
    uc_buffer_st initializationData;

} uc_device_multi2_parameter;

/**
 * Function pointer type used to notify the client of the smartcard status.
 *
 * A function pointer of this type is passed  via the ::UniversalClientSPI_Smartcard_Open
 * method. The implementation is expected to send the current status of smartcard to Cloaked CA Agent.
 *
 *
 * @param[in] smartcardID the smartcard id previously return from the ::UniversalClientSPI_Smartcard_Open method. 
 * @param[in] smartcardHandle the smartcard handle previously passed by the ::UniversalClientSPI_Smartcard_Open method.
 * @param[in] reason the notification reason, see ::uc_sc_status_type.
 * @param[in] pvInfo Buffer containing data to send to Cloaked CA Agent. The uc_buffer_st::bytes member points to 
 *            the payload of the info, and the uc_buffer_st::length member contains the number of bytes of the info
 *            to send. The definition of format of the payload info can be found in ::uc_sc_status_type.
 */
typedef void (* uc_sc_status_notify_fn)(
            uc_uint32 smartcardID,
            uc_smartcard_handle smartcardHandle,
            uc_sc_status_type reason, 
            uc_buffer_st *pvInfo);

/**
 * Data structure of notification function for the smartcard interfaces. 
 */                    
typedef struct _uc_sc_open_parameter
{
    /**
     * notification function, this function shall be called to notify the changed smartcard status 
     * to Cloaked CA Agent.
     */
    uc_sc_status_notify_fn    pfnStatusNotify;
    /**
     * smartcard handle associated with this open operation.
     */
    uc_smartcard_handle       smartcardHandle;
} uc_sc_open_parameter;

/**
 * Function pointer type used to notify the client with a timeout event.
 *
 * A function pointer of this type is passed to the implementation of a timer via the ::UniversalClientSPI_Timer_Open
 * method. The timer implementation is expected to notify a timeout event.
 *
 *
 * @param[in] timerId The id of the timer which has triggered a timeout event. 
 * @param[in] timerContext timer context parameter which is passed to the implementation via ::UniversalClientSPI_Timer_Open 
 */
typedef void (*uc_timer_callback)(
    uc_uint32 timerId,
    void * timerContext);

/**
 * Data structure of notification function for the timer interfaces. 
 */           
typedef struct _uc_timer_info
{
    /**
     * Timer context, this should be sent back to Cloaked CA via the notification function \b onTimerCallBack.
     */
    void * timerContext;

    /**
     * Specify the timeout value, in milliseconds.
     */
    uc_uint32 timeout;

    /**
     * Indicate if the timer is repetitive.
     * If the timer is repetitive, once the timeout event happens, the implementation should notify Cloaked CA Agent 
     * and reclock to wait the next timeout. 
     * The process will go on until ::UniversalClientSPI_Timer_Close or ::UniversalClientSPI_Timer_Stop is called.
     */
    uc_bool isRepetitive;

    /**
     * Notification function, this function shall be called to notify the timeout event 
     * to Cloaked CA Agent.
     */
    uc_timer_callback onTimeCallback;
}uc_timer_info;

/**
 * Data structure for the soapAction and its payload data.
 *
 */
typedef struct _uc_web_service_info
{
    /**
     * SOAP Action
     *
     * The action for this request.
     *
     * bytes in soapAction points to a null-terminated string, e.g. "SOAPAction: "http://irdeto.com/ccis/requestEntitlement""
     *
     * length in soapAction equals to strlen("SOAPAction: "http://irdeto.com/ccis/requestEntitlement"").
     */
    uc_buffer_st soapAction;
    /**
     * Message data
     *
     * The XML message is the payload for this request.
     *
     * bytes in messageData points to a null-terminated string, e.g. "<?xml version="1.0" encoding="utf-8"?> <soap:Envelope><soap:Body><clientRegister><deviceId>000000000000</deviceId><Pin>00000000</Pin></clientRegister></soap:Body></soap:Envelope>"
     *
     * length in messageData equals to strlen("<?xml version="1.0" encoding="utf-8"?> <soap:Envelope><soap:Body><clientRegister><deviceId>000000000000</deviceId><Pin>00000000</Pin></clientRegister></soap:Body></soap:Envelope>").
     */
    uc_buffer_st messageData;
    /**
     * Request ID
     *
     * Added for the Anti-Cloning feature. Each EMM request takes a request ID.
     *
     * To match the response with the request, SPI must use the same request ID to call the notify callback method after getting the EMM response. 
     */
    uc_uint32 requestId;
}uc_web_service_info;

/**
 * Data structure for proximity message delivery.
 *
 */
typedef struct _uc_proximity_message
{
    /**
     * Indicates the peer device to which the message is sent.
     */
    uc_proximity_peer peer;

    /**
     * Indicates whether the following TTL value needs to be set into IP packet.
     */
    uc_bool isTtlSpecified;
    /**
     * Indicates the TTL value. If uc_proximity_message::isTtlSpecified is ::UC_TRUE, this value must be set into IP packet.
     */
    uc_uint32 ttl;
    /**
     * Message data
     */
    uc_buffer_st messageData;

} uc_proximity_message;

/**
 * Data structure for stream send ::UniversalClientSPI_Stream_Send.
 *
 */
typedef struct _uc_stream_send_payload
{
    /**
     * Indicates the type of stream.
     *
     * This determines which members of the union is valid.
     * If stream type is ::UC_CONNECTION_STREAM_TYPE_IP, the union's member must be ::uc_web_service_info.
     * If stream type is ::UC_CONNECTION_STREAM_TYPE_PD, the union's member must be ::uc_proximity_message.
     */
    uc_connection_stream_type streamType;
    /**
     * Lists all the members of the union.
     */
    union
    {
        /**
         * The payload is sent to the web service. 
         */
        uc_web_service_info webServicePayload;
        
        /**
         * The payload is sent to another peer device for proximity detection. 
         */
        uc_proximity_message proximityPayload;
    } payload;
} uc_stream_send_payload;

/**
 * IFCP key ladder command information structure, used in ::uc_IFCP_info
 *
 */
typedef struct _uc_KLC_info
{
    /**
     * The header of IFCP key ladder command.
     */
    uc_byte header[3];

    /**
     * The control byte of extra data.
     *
     * \note if ED_Ctrl is 0: STB manufacturers must follow the chip's specification to set the ED_Ctrl for IFCP command construction.
     * \note if ED_Ctrl is 4: CUR data is filled in the extraData buffer. STB manufacturers should also follow the chip's specification to set the other ED_Ctrl for IFCP command construction.
     */
    uc_byte ED_Ctrl;
    
    /**
     * The content of extra data.
     *
     * \note if extraData.bytes is NULL. STB manufacturers must follow the chip's specification to set the Extra Data for IFCP command construction.
     */
    uc_buffer_st extraData;
    
    /**
     * The payload of IFCP key ladder command.
     */
    uc_buffer_st payload;

} uc_KLC_info;

/**
 * The size of application control header data in ::uc_application_control_info.
 */
#define IFCP_APP_CONTROL_INFO_HEADER_LEN    12

/**
 * Application control information structure, used in ::uc_IFCP_input
 *
 * This structure is used to deliver application control data to the STB application in ::uc_IFCP_input passed to 
 * ::UniversalClientSPI_IFCP_Communicate.
 *
 */
typedef struct _uc_application_control_info
{
    /**
     * The application control header data.
     */
    uc_byte header[IFCP_APP_CONTROL_INFO_HEADER_LEN];
    
    /**
     * The application control payload data.
     */
    uc_buffer_st payload;

} uc_application_control_info;


/**
 * IFCP command information structure, used in ::uc_IFCP_input
 *
 * This structure is used to deliver IFCP command information to the STB application in ::uc_IFCP_input passed to 
 * ::UniversalClientSPI_IFCP_Communicate.
 *
 * The STB application uses the information to construct the IFCP command, and sends it to the IFCP module in the secure chipset through the IFCP mailbox interface.
 * The IFCP command is encoded as a string of bytes with a syntax defined as below.
 * 
 \code

 IFCP Command()
 {
     Field Name         Field Length                 Field Value

     header              3  bytes                    from pKLCInfo
     ED_Ctrl             1  byte                     from pKLCInfo
     Extra Data          4  bytes * m, 0<=m<=8       from pKLCInfo
     KLC Payload         16 bytes * n, 0<n<=31       from pKLCInfo
     AppCtrl Header      12 bytes                    from pApplicationControlInfo, optional
     AppCtrl Payload     variable bytes              from pApplicationControlInfo, optional
     Paddings            variable bytes              zeros are appended to the IFCP data by the STB application according to the rules in the note below
 }
 
 \endcode
 *
 * \note Currently, the ED_Ctrl and Extra Data are only available for CUR Data from the Cloaked CA Agent. STB manufacturers must follow the chip's specification to set other ED_Ctrl and Extra Data for IFCP command construction.\n 
 *
 * \n
 *
 * \note The STB application will append zeros at the end of the IFCP Command Data 
 *       to make it 4-bytes aligned. However, it is important to ensure 
 *       that the other data is not changed by the appended zeros. 
 *       This rule is applicable for all IFCP secure chipsets. \n 
 */
typedef struct _uc_IFCP_info
{
    /**
     * The key ladder information for IFCP command.
     *
     * Key ladder information is mandatory.
     */
    uc_KLC_info *pKLCInfo;

    /**
     * The application control information.
     *
     * Application control information is optional. If there is no application control information, the pointer pApplicationControlInfo is NULL.  
     */
    uc_application_control_info *pApplicationControlInfo;

} uc_IFCP_info;

/**
 * Used to indicate the command type in ::uc_IFCP_input.
 *
 */
typedef enum {
    /** 
     * Set descrambling key by IFCP command, where the descrambling key is protected by IFCP key ladder.
     *
     * \note Not support clear CW, e.g. 911 mode. Clear CW is set via ::UniversalClientSPI_Stream_SetDescramblingKey.
     */
    UC_IFCP_COMMAND_SET_DESCRAMBLING_KEY = 0,

    /** 
     * Set Secure PVR session key by IFCP command, where the secure PVR session key is protected by IFCP key ladder.
     *
     * \note Not support clear PVR session key, e.g. FTA PVR without initialization. Clear PVR session key is set via ::UniversalClientSPI_PVR_SetExtendedSessionKey.
     */
    UC_IFCP_COMMAND_SET_PVR_SESSION_KEY = 1,

    /** 
     * Load the Transformation Data Container (TDC) structure using the IFCP command, where TDC is protected by the TASK.
     * 
     * \note This command may be called serveal times to load all the TDCs to the IFCP. 
     * If errors happen during loading TDC data, ::UC_ERROR_IFCP_TDC_LOAD_FAILED should be returned when implementing ::UniversalClientSPI_IFCP_Communicate.
     */
    UC_IFCP_COMMAND_LOAD_TDC = 2

} uc_IFCP_command_type;

/**
 * The data structure for IFCP communication ::UniversalClientSPI_IFCP_Communicate.
 *
 */
typedef struct _uc_IFCP_input
{
    /**
     * Command type.
     */
    uc_IFCP_command_type commandType;

    /**
     * Includes IFCP command information for IFCP command construction.
     */
    uc_IFCP_info *pIFCPInfo;

    /**
     * Additional information that is needed by STB Application when commandType is ::UC_IFCP_COMMAND_SET_DESCRAMBLING_KEY or  ::UC_IFCP_COMMAND_SET_PVR_SESSION_KEY
     */
    union
    {
        /**
         * Additional information when command type is ::UC_IFCP_COMMAND_SET_DESCRAMBLING_KEY. 
         */
        struct _uc_additional_info_for_IFCP_set_descrambling_key
        {
            /**
             * Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
             */
            uc_stream_handle streamHandle;

            /**
             * Key version that this descrambling key is associated with, 
             * refer to keyVersion in ::UniversalClientSPI_Stream_SetDescramblingKey for more details.
             */
            uc_uint32 keyVersion;

            /**
             * The scrambling algorithm that this key will be used for.
             */
            uc_dk_algorithm descramblingKeyAlgorithm;
        } forDescramblingKey;
        
        /**
         * Additional information when command type is ::UC_IFCP_COMMAND_SET_PVR_SESSION_KEY. 
         */
        struct _uc_additional_info_for_IFCP_set_PVR_session_key
        {
            /**
             * Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
             */
            uc_stream_handle streamHandle;

            /**
             * The recommended algorithm used to encrypt the recorded content.
             */
            uc_dk_algorithm algorithm;

            /**
             * Indicate that the index of this recording key is associated with.
              * For one recording, there maybe more than one recording key needed to encrypt the content.
              * Different key index refer to potentially different recording keys.
              * The PVR encrypt engine should support smooth transition from one recording key to the next.
              * The key index typically could increase from 0 to 0xFFFFFFFF. 
              * The PVR encrypt engine that supports recording key polarity, 
              * could consider the key index that can be divided exactly by 2 as Even key, 
              * and consider the key index that can not be divided exactly by 2 as Odd key.
              * The recording key index is also added into the Metadata and notify to API layer by the message ::UC_SERVICE_PVR_SESSION_METADATA_REPLY.
              */
            uc_uint32 keyIndex;
        } forPVRSessionKey;
        
    } additionalInfo;

} uc_IFCP_input;

/**
 * Data structure for IFCP communication ::UniversalClientSPI_IFCP_Communicate.
 *
 */
typedef struct _uc_IFCP_output
{
    /**
     * Response from IFCP key ladder module.
     *
     * The uc_buffer_st::bytes member points to a buffer in memory which is allocated by the Cloaked CA Agent,
     * and the uc_buffer_st::length member is the maximum length of the buffer, in bytes. 
     * The STB application copies the response data into the buffer uc_buffer_st::bytes, 
     * and sets the uc_buffer_st::length member to the actual length of data returned.
     * If there is not any response data, set the uc_buffer_st::length to 0.
     * If the uc_buffer_st::length member is not large enough to hold the entire data, 
     * just copy part of the response data according to the uc_buffer_st::length provided by the Cloaked CA Agent.
     *
     * \note Refer to the chip's specification to know whether there is any response data and how to get response data.
     */
    uc_buffer_st response;

    /**
     * Response from IFCP Application module.
     *
     * If the application control information is provided in ::uc_IFCP_input::uc_IFCP_info, 
     * the application control information must be contained in the IFCP command which is sent to the IFCP module, 
     * and there must be an application response from the IFCP module. 
     * Refer to the chip's specification to know how to get the application response data from the IFCP module.
     *
     * The usage of ::uc_IFCP_output::appResponse is the same as ::uc_IFCP_output::response.
     */
    uc_buffer_st appResponse;

} uc_IFCP_output;

/**
 * Data structure for IFCP image loading ::UniversalClientSPI_IFCP_LoadImage.
 *
 */
typedef struct _uc_IFCP_image
{
    /**
     * Activation message.
     */
    uc_buffer_st activationMessage;

    /**
     * IFCP RAM image.
     */
    uc_buffer_st ramImage;
    
} uc_IFCP_image;

/**
 * Data structure for PS property, used in ::UniversalClientSPI_PS_GetProperty.
 *
 * The property corresponds to a specified PS index.
 */
typedef struct _uc_ps_property
{
    /**
     * Reserved Size.
     * 
     * The PS space in \b kilobytes that is reserved for the specified index.
     * The requirements of the reserved size are in the Integration Overview document. 
     *
     */
    uc_uint32 reservedSize;
    
} uc_ps_property;

/**
 * Data structure for TDC and TASK loading ::UniversalClientSPI_SCOT_LoadTransformationData.
 *
 */
typedef struct _uc_tdc_data_for_spi
{
    /**
    * Length of the TDC.
    */
    uc_uint32 length;
    /**
    * Buffer of the TDC.
    */
    uc_byte tdc[UC_MAX_TRANSFORMATION_TDC_SIZE];
} uc_tdc_data_for_spi;

/**
 * Used to indicate the SPI message type in ::uc_message_callback.
 *
 */
typedef enum _uc_spi_message_type
{
    /** 
     * (0x00000000) SMP status message type for the ::uc_message_callback callback.
     *
     * HDCP status messages are provided to the callback when there is HDCP status changed.
     *
     * For messages with this type, the lpVoid argument is NULL.
     */
     UC_SMP_HDCP_STATUS_CHANGED = 0x00000000


} uc_spi_message_type;

/**
 * Function pointer type used by downstream components to notify Cloaked CA Agent on the status or request actions.
 *
 * A function pointer of this type is passed to the implementation of a stream via the ::UniversalClientSPI_Message_CallBack method.
 * The SPI implementation is expected to report the SMP HDCP status change or request action to Cloaked CA Agent, after a call to ::UniversalClient_StartCaClient and until a call to ::UniversalClient_StopCaClient occurs.
 *
 * @param[in] type The message type for this message, See the definition of ::uc_spi_message_type for a description of different types.
 * @param[in] pVoid Message defined pointer. The value of this depends on the message type. See ::uc_spi_message_type for details. 
 */
typedef void(*uc_message_callback)(
    uc_spi_message_type type,
    void *pVoid);

/** @defgroup universalclient_spi Cloaked CA Agent SPIs

 *  Cloaked CA Agent SPIs
 *
 *  Service Provider Interfaces(SPIs) are driver-level APIs called by Cloaked CA Agent.
 *  Client device platform must implement these SPIs in order to run Cloaked CA Agent.
 *
 *  \note In the SPI implementation, APIs shall not be called to avoid dead lock between the 
 *  application and Cloaked CA Agent thread.
 *  @{
 */

/** @page allspis Cloaked CA Agent SPIs
 * Cloaked CA Agent SPIs
 *
 * Service Provider Interfaces(SPIs) are the components that must be implemented on a per-platform basis.
 * These SPIs are grouped into sections according to 
 * common functionality:
 *
 * - \ref memoryspi "Cloaked CA Agent Memory SPIs"
 * - \ref semaphorespi "Cloaked CA Agent Semaphore SPIs"
 * - \ref mutexspi "Cloaked CA Agent Mutex SPIs"
 * - \ref threadspi "Cloaked CA Agent Thread SPIs"
 * - \ref psspi "Cloaked CA Agent Persistent Storage SPIs"
 * - \ref devicespi "Cloaked CA Agent Device SPIs"
 * - \ref pvrspi "Cloaked CA Agent PVR SPIs"
 * - \ref streamspi "Cloaked CA Agent Stream SPIs"
 * - \ref copycontrolspi "Cloaked CA Agent Copy Control SPIs"
 * - \ref smartcardspi "Cloaked CA Agent Smart Card SPIs"
 * - \ref datetimespi "Cloaked CA Agent Date Time SPIs"
 * - \ref manufacturercryptospi "Cloaked CA Agent Crypto SPIs"
 * - \ref timerspi "Cloaked CA Agent Timer SPIs"
 * - \ref ifcpspi "Cloaked CA Agent IFCP SPIs"
 * - \ref trickmodecontrolspi "Cloaked CA Agent Trick Mode Control SPIs"
 * - \ref hgpcspi "Cloaked CA Agent Home Gateway Proximity Control SPIs"
 * - \ref scotspi "Cloaked CA Agent SCOT SPIs"
 * - \ref message_callback "Cloaked CA Agent Message Callback SPI"
 *
 * In order to indicate which SPIs are implemented, the integrator MUST
 * implement the top-level SPIs 
 * ::UniversalClientSPI_GetVersion and ::UniversalClientSPI_GetImplementation.
 */


/** @defgroup toplevelspi Cloaked CA Agent Top Level SPIs
 *  Cloaked CA Agent Top Level SPIs
 *
 *  These SPIs implement a version compatibility mechanism that allows
 *  new version releases of the Cloaked CA Agent API to be linked with
 *  an existing SPI implementation without having to update the SPI implementation
 *  in order to build. 
 * 
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *
 *  @{
 */

/**
 * Get the current version of the Cloaked CA Agent SPI.
 *
 * This function is called when the client is started is called in order to check
 * that the version of the SPI implementation is compatible with the version of the client.
 *
 * This function MUST return #UNIVERSALCLIENTSPI_VERSION. Recompiling an existing SPI implementation
 * will update this version automatically. 
 * @retval the version of the SPI implementation
 */
uc_uint32 UniversalClientSPI_GetVersion(void);

/**
 * SPI implementation dispatch table
 *
 * This structure is passed to ::UniversalClientSPI_GetImplementation,
 * and must be filled out by the integrator. Each member of 
 * this structure must be set to the implementation of the
 * corresponding methods. Any methods not explicitly set in this structure
 * will be considered 'not implemented' by the Cloaked CA Agent and ignored. 
 */
typedef struct _uc_spi_implementation_st
{
    /**
     * See ::UniversalClientSPI_Memory_Malloc
     */
    void* (*Memory_Malloc)(uc_uint32 bytes);

    /**
     * See ::UniversalClientSPI_Memory_Free
     */
    void (*Memory_Free)(void * lpVoid);

    /**
     * See ::UniversalClientSPI_Semaphore_Open
     */
    uc_result (*Semaphore_Open)(uc_uint32 initialValue, 
                                             uc_semaphore_handle * pSemaphoreHandle);

    /**
     * See ::UniversalClientSPI_Semaphore_Post
     */
    uc_result (*Semaphore_Post)(uc_semaphore_handle semaphoreHandle);

    /**
     * See ::UniversalClientSPI_Semaphore_Wait
     */
    uc_result (*Semaphore_Wait)(uc_semaphore_handle semaphoreHandle);

    /**
     * See ::UniversalClientSPI_Semaphore_Close
     */
    uc_result (*Semaphore_Close)(uc_semaphore_handle * pSemaphoreHandle);

    /**
     * See ::UniversalClientSPI_Mutex_Open
     */
    uc_result (*Mutex_Open)(uc_mutex_handle * pMutexHandle);

    /**
     * See ::UniversalClientSPI_Mutex_Lock
     */
    uc_result (*Mutex_Lock)(uc_mutex_handle mutexHandle);

    /**
     * See ::UniversalClientSPI_Mutex_Unlock
     */
    uc_result (*Mutex_Unlock)(uc_mutex_handle mutexHandle);

    /**
     * See ::UniversalClientSPI_Mutex_Close
     */
    uc_result (*Mutex_Close)(uc_mutex_handle * pMutexHandle);


    /**
     * See ::UniversalClientSPI_Thread_Open
     */
    uc_result (*Thread_Open)(uc_threadproc threadProc, 
                                       void* lpVoid, uc_thread_handle * pThreadHandle);
    /**
     * See ::UniversalClientSPI_Thread_Sleep
     */
    uc_result (*Thread_Sleep)(uc_thread_handle hThreadHandle, uc_uint16 wDelay);

    /**
     * See ::UniversalClientSPI_Thread_Close
     */
    uc_result (*Thread_Close)(uc_thread_handle * pThreadHandle);

    /**
     * See ::UniversalClientSPI_PS_Delete
     */
    uc_result (*PS_Delete)(uc_uint32 index);

    /**
     * See ::UniversalClientSPI_PS_Write
     */
    uc_result (*PS_Write)(uc_uint32 index, uc_uint32 offset, const uc_buffer_st *pData);

    /**
     * See ::UniversalClientSPI_PS_Read
     */
    uc_result (*PS_Read)(uc_uint32 index, uc_uint32 offset, uc_buffer_st *pData);

    /**
     * See ::UniversalClientSPI_PS_GetProperty
     */
    uc_result (*PS_GetProperty)(uc_uint32 index, uc_ps_property *pProperty);

    /**
     * See ::UniversalClientSPI_PS_Initialize
     */
    uc_result (*PS_Initialize)(void);

    /**
     * See ::UniversalClientSPI_PS_Terminate
     */
    uc_result (*PS_Terminate)(void);

    /**
     * See ::UniversalClientSPI_Device_GetCSSN
     */
    uc_result (*Device_GetCSSN)(uc_buffer_st * pData);

    /**
     * See ::UniversalClientSPI_Device_SetCSSK
     * \note For new integration, this method is not used. Please use ::UniversalClientSPI_Device_SetExtendedCSSK.
     * This is only provided for compatibility.
     */
    uc_result (*Device_SetCSSK)(const uc_buffer_st * pKeyMaterial);

    /**
     * See ::UniversalClientSPI_Device_SetExtendedCSSK
     * \note For new integration, this method is used instead of UniversalClientSPI_Device_SetCSSK.
     */
    uc_result (*Device_SetExtendedCSSK)(const uc_cssk_info * pCSSKInfo);

    /**
     * See ::UniversalClientSPI_Stream_Open
     *
     * \note ::UniversalClientSPI_Stream_Open is a special SPI.
     * For DVB client, the prototype for this SPI in fact is
     * ::uc_result ::UniversalClientSPI_Stream_Open(
     *       ::uc_connection_stream_type \b streamType,
     *       \b const ::uc_stream_open_params \b *pStreamOpenParams,
     *       ::uc_stream_handle \b *pStreamHandle);
     * The SPI implementation can make necessary type convertion for this SPI if there is any compile warning.
     *
     */
    uc_result (*Stream_Open)(
        uc_connection_stream_type streamType,
        void *pStreamOpenParams,
        uc_stream_handle *pStreamHandle);

    /**
     * See ::UniversalClientSPI_Stream_AddComponent
     *
     * \note ::UniversalClientSPI_Stream_AddComponent is a special SPI.
     * For DVB client, the prototype for this SPI in fact is
     * ::uc_result ::UniversalClientSPI_Stream_AddComponent(
     *    ::uc_stream_handle \b streamHandle,
     *    \b const ::uc_elementary_component \b *pComponent);
     * The SPI implementation can make necessary type convertion for this SPI if there is any compile warning.
     *
     */
    uc_result (*Stream_AddComponent)(
        uc_stream_handle streamHandle,
        const uc_component *pComponent);

    /**
     * See ::UniversalClientSPI_Stream_RemoveComponent
     *
     * \note ::UniversalClientSPI_Stream_RemoveComponent is a special SPI.
     * For DVB client, the prototype for this SPI in fact is
     * ::uc_result ::UniversalClientSPI_Stream_RemoveComponent(
     *   ::uc_stream_handle \b streamHandle,
     *   \b const ::uc_elementary_component \b *pComponent);
     * The SPI implementation can make necessary type convertion for this SPI if there is any compile warning.
     *
     */
    uc_result (*Stream_RemoveComponent)(
        uc_stream_handle streamHandle,
        const uc_component *pComponent);

    /**
     * See ::UniversalClientSPI_Stream_Start
     */
    uc_result (*Stream_Start)(uc_stream_handle streamHandle);

    /**
     * See ::UniversalClientSPI_Stream_Stop
     */
    uc_result (*Stream_Stop)(uc_stream_handle streamHandle);

    /**
     * See ::UniversalClientSPI_Stream_OpenFilter
     */
    uc_result (*Stream_OpenFilter)(
        uc_stream_handle streamHandle,
        uc_filter_handle *pFilterHandle);

    /**
     * See ::UniversalClientSPI_Stream_SetFilter
     */
    uc_result (*Stream_SetFilter)(
        uc_filter_handle filterHandle,
        const uc_filter *pFilterRules);

    /**
     * See ::UniversalClientSPI_Stream_CloseFilter
     */
    uc_result (*Stream_CloseFilter)(
        uc_stream_handle streamHandle,
        uc_filter_handle *pFilterHandle);

    /**
     * See ::UniversalClientSPI_Stream_Connect
     */
    uc_result (*Stream_Connect)(
        uc_stream_handle streamHandle,
        uc_connection_handle connectionHandle,
        uc_notify_callback notifyCallback);

    /**
     * See ::UniversalClientSPI_Stream_Extended_Connect
     */
    uc_result (*Stream_Extended_Connect)(
        uc_stream_handle streamHandle,
        uc_connection_handle connectionHandle,
        uc_notify_callback_extended notifyCallback);

    /**
     * See ::UniversalClientSPI_Stream_Disconnect
     */
    uc_result (*Stream_Disconnect)(
        uc_stream_handle streamHandle,
        uc_connection_handle connectionHandle);

    /**
     * See ::UniversalClientSPI_Stream_SetDescramblingKey
     */
    uc_result (*Stream_SetDescramblingKey)(
        uc_stream_handle streamHandle,
        const uc_key_info *pKeyInfo,
        uc_uint32 keyVersion);

    /**
     * See ::UniversalClientSPI_Stream_CleanDescramblingKey
     */
    uc_result (*Stream_CleanDescramblingKey)(
        uc_stream_handle streamHandle);        

    /**
     * See ::UniversalClientSPI_Stream_Close
     */
    uc_result (*Stream_Close)(
        uc_stream_handle *pStreamHandle);

    /**
     * See ::UniversalClientSPI_Device_GetPrivateData
     */
    uc_result (*Device_GetPrivateData)(uc_buffer_st * pData);


    /**
     * See ::UniversalClientSPI_Device_GetSecurityState
     */
    uc_result (*Device_GetSecurityState)(uc_device_security_state * pDeviceSecurityState);

    /**
     * See ::UniversalClientSPI_Device_GetPlatformIdentifiers
     */
    uc_result (*Device_GetPlatformIdentifiers)(uc_device_platform_identifiers * pDevicePlatformIdentifiers);

    /**
     * See ::UniversalClientSPI_PVR_SetSessionKey
     */
   uc_result (*PVR_SetSessionKey)(uc_stream_handle streamHandle, const uc_buffer_st *pPVRSessionKey);

    /**
     * See ::UniversalClientSPI_PVR_SetExtendedSessionKey
     */
   uc_result (*PVR_SetExtendedSessionKey)(uc_stream_handle streamHandle, const uc_pvrsk_info *pPVRSKInfo);

    /**
     * See ::UniversalClientSPI_CopyControl_Macrovision_SetConfig
     */
   uc_result (*CopyControl_Macrovision_SetConfig)(uc_macrovision_config mac_config_data);
    /**
     * See ::UniversalClientSPI_CopyControl_Macrovision_SetMode
     */
   uc_result  (*CopyControl_Macrovision_SetMode)(uc_stream_handle handle,uc_macrovision_mode mac_mode);
    /**
     * See ::UniversalClientSPI_CopyControl_SetCCI
     */
   uc_result  (*CopyControl_SetCCI)(uc_stream_handle streamHandle,uc_copy_control_info* pCopyControlInfo);
   /**
     * See ::UniversalClientSPI_FatalError
     */
   void  (*FatalError)(uc_fatal_error_type type,void* lpVoid);
   /**
     * See ::UniversalClientSPI_Smartcard_Open
     */
   uc_result  (*Smartcard_Open)(uc_uint32 *pSmartcardID, uc_sc_open_parameter *pSCOpenData);
   /**
     * See ::UniversalClientSPI_Smartcard_Close
     */
   uc_result  (*Smartcard_Close)(uc_uint32 smartcardID);
   /**
     * See ::UniversalClientSPI_Smartcard_Reset
     */
   uc_result  (*Smartcard_Reset)(uc_uint32 smartcardID);
   /**
     * See ::UniversalClientSPI_Smartcard_Communicate
     */ 
   uc_result  (*Smartcard_Communicate)(uc_uint32 smartcardID, uc_uint32 headerLen, uc_uint32 payloadLen,uc_byte *pSendBuffer, uc_uint32 *pRecvDataLen, uc_byte *pRecvBuffer );

    /**
      * See ::UniversalClientSPI_DateTime_GetTimeOfDay. Optional!
      */ 
    uc_result  (*DateTime_GetTimeOfDay)(uc_time *pCurrentTime);

    /**
      * See ::UniversalClientSPI_Crypto_Verify. Optional!
      */ 
    uc_result  (*Crypto_Verify)(uc_crypto_info *pCryptoInfo);

    /**
     * See ::UniversalClientSPI_Device_GetPersonalizedData
     */
    uc_result (*Device_GetPersonalizedData)(uc_buffer_st* pData);

    /**
     * See ::UniversalClientSPI_Timer_Open
     */
    uc_result (*Timer_Open)(uc_uint32* pTimerId,uc_timer_info* pTimerInfo);

    /**
     * See ::UniversalClientSPI_Timer_Close
     */
    uc_result (*Timer_Close)(uc_uint32 timerId);

    /**
     * See ::UniversalClientSPI_Timer_Start
     */
    uc_result (*Timer_Start)(uc_uint32 timerId);

    /**
     * See ::UniversalClientSPI_Timer_Stop
     */
    uc_result (*Timer_Stop)(uc_uint32 timerId);

    /**
     * See ::UniversalClientSPI_Semaphore_WaitTimeout
     */
    uc_result (*Semaphore_WaitTimeout)(uc_semaphore_handle semaphoreHandle, uc_uint32 milliseconds);

    /**
     * See ::UniversalClientSPI_Device_GetDeviceID
     */
    uc_result (*Device_GetDeviceID)(uc_buffer_st * pData);

    /**
     * See ::UniversalClientSPI_Device_GetIPAddress
     */
    uc_result (*Device_GetIPAddress)(uc_buffer_st * pData);

    /**
     * See ::UniversalClientSPI_Stream_Send
     */
    uc_result (*Stream_Send)(
     uc_stream_handle streamHandle,
     const uc_stream_send_payload *pBytes);

    /**
     * See ::UniversalClientSPI_Device_GetPVRSecurityState(uc_pvr_security_state * pPVRSecurityState)
     */
    uc_result (*Device_GetPVRSecurityState)(uc_pvr_security_state * pPVRSecurityState);

    /**
     * See ::UniversalClientSPI_Device_GetChipConfigurationCheck
     */
    uc_result (*Device_GetChipConfigurationCheck)(
                uc_chip_configuration_request chipConfigurationRequest, 
                uc_chip_configuration_response *pChipConfigurationResponse);

    /**
     * See ::UniversalClientSPI_Device_SetCSSN
     */
    uc_result (*Device_SetCSSN)(const uc_buffer_st * pData);

    /**
     * See ::UniversalClientSPI_Device_GetPINCode
     */
    uc_result (*Device_GetPINCode)(uc_buffer_st * pData);

    /**
     * See ::UniversalClientSPI_Device_SetMulti2Parameter
     */
    uc_result (*Device_SetMulti2Parameter)(
        uc_stream_handle streamHandle, 
        uc_device_multi2_parameter *pMulti2Parameter);

    /**
     * See ::UniversalClientSPI_IFCP_Communicate
     */
    uc_result (*IFCP_Communicate)(uc_IFCP_input *pInput, uc_IFCP_output *pOutput);

    /**
     * See ::UniversalClientSPI_IFCP_LoadImage
     */
    uc_result (*IFCP_LoadImage)(uc_IFCP_image *pImage);

    /**
     * See ::UniversalClientSPI_SetExtraTrickModeControl
     */
    uc_result (*SetExtraTrickModeControl)(uc_stream_handle streamHandle,uc_extra_trick_mode_control *pExtraTrickModeControl);

    /**
     * See ::UniversalClientSPI_HGPC_SendHNAMessage
     */
    uc_result (*HGPC_SendHNAMessage)(uc_byte* pHNAMessage);

    /**
     * See ::UniversalClientSPI_SCOT_LoadTransformationData
     */
    uc_result (*SCOT_LoadTransformationData)(const uc_buffer_st * pEncryptedTask, uc_uint32 tdcCount, uc_tdc_data_for_spi *pTdcData);

    /**
     * See ::UniversalClientSPI_Message_CallBack
     */
    uc_result (*Message_CallBack)(uc_message_callback messageCallback);

} uc_spi_implementation_st;

/**
 * Get the list of implemented methods
 *
 * This method is called when the client is started in order to 
 * get the list of SPI methods currently available on the platform.
 *
 * @param [in,out] pImpl On input, this structure is zeroed out. 
 *    The implementation MUST populate this structure in order to successfully
 *    integrate with the platform.
 *
 */
extern void UniversalClientSPI_GetImplementation(
    uc_spi_implementation_st *pImpl);

/** @} */ /* End of toplevelspi*/

/** @defgroup memoryspi Cloaked CA Agent Memory SPIs
 *  Cloaked CA Agent Memory SPIs
 *
 *  These SPIs implement basic memory management functionality required for most operations.  
 * 
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *
 *  @{
 */
/**
 * Allocates a block of uninitialized memory. 
 *
 * This function is called to allocate a block of memory and return a pointer to the beginning
 * of the block. If the system is out of memory, or another failure occurs, it must return NULL.
 * When the memory is no longer needed, ::UniversalClientSPI_Memory_Free will be called to free it.
 *
 * @param[in] bytes Number of bytes to allocate. If this is 0, the implementation MUST return an empty block
 *     of allocated memory.
 */
extern void* UniversalClientSPI_Memory_Malloc(uc_uint32 bytes);
/**
 * Frees a block of memory.
 *
 * This function is called to free a block of memory previously allocated using ::UniversalClientSPI_Memory_Malloc.
 *
 * @param[in] lpVoid Pointer to a block of previously allocated memory. If this is NULL, the implementation
 *     MUST handle it gracefully and ignore it. If this points to an area of memory that was not previously
 *     allocated by ::UniversalClientSPI_Memory_Malloc, OR it has already been freed, the result is undefined.
 */
extern void UniversalClientSPI_Memory_Free(void * lpVoid);

/**
 * Report a fatal error.
 *
 * This function is called to report a fatal error.
 * The device application does not need to take any specific operation when it's triggered. 
 *
 * @param[in] type Error code, see ::uc_fatal_error_type.
 * @param[in] lpVoid Pointer to the null-terminated string to describe this error.
 */
extern void  UniversalClientSPI_FatalError(uc_fatal_error_type type,void* lpVoid);

/** @} */ /* End of memory */

/** @defgroup semaphorespi Cloaked CA Agent Semaphore SPIs
 *  Cloaked CA Agent Semaphore SPIs
 *
 * These SPIs implement basic thread synchronization, semaphores.
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *
 *  @{
 */
/**
 * Create a semaphore
 *
 * This function is called to create a semaphore.
 *
 * A 'semaphore' is a basic synchronization object with the following properties:
 * - It maintains a value, which is used internally to determine when to unblock waiters.
 * - Calling ::UniversalClientSPI_Semaphore_Post atomically increments the value.
 * - Calling ::UniversalClientSPI_Semaphore_Wait waits until the value > 0, then atomically decrements the value. 
 *
 * Semaphores are used by the Cloaked CA Agent to signal when events occur and to protect
 * data structures from simultaneous access.
 *
 * @param[in] initialValue Initial value for the semaphore.
 * @param[out] pSemaphoreHandle Receives a handle to a newly created semaphore. When the semaphore is no longer needed,
 *     it will be closed using ::UniversalClientSPI_Semaphore_Close. 
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Semaphore_Open(uc_uint32 initialValue, 
                                             uc_semaphore_handle * pSemaphoreHandle);
/**
 * Increment the semaphore value
 *
 * This function is called to atomically increment the value of a semaphore. 
 * If the value goes above 0, this should cause threads waiting in a call to 
 * ::UniversalClientSPI_Semaphore_Wait to be unblocked accordingly. 
 *
 * @param[in] semaphoreHandle Handle of semaphore previously created using ::UniversalClientSPI_Semaphore_Open.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Semaphore_Post(uc_semaphore_handle semaphoreHandle);
/**
 * Decrement the semaphore value
 *
 * This function is called to atomically decrement the value of a semaphore.
 * It waits for the semaphore value to be greater than 0, then atomically decrements the value.
 * For example, if there are 3 threads blocked waiting on the semaphore, the value will be 0. 
 * If another thread calls ::UniversalClientSPI_Semaphore_Post, it will increment the value to 1, and
 * ONE of the threads will be unblocked so that it can decrement the value back to 0. The other two
 * threads remain blocked. 
 * 
 *
 * @param[in] semaphoreHandle Handle of semaphore previously created using ::UniversalClientSPI_Semaphore_Open.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Semaphore_Wait(uc_semaphore_handle semaphoreHandle);

/**
 * Decrement the semaphore value with timeout interval.
 *
 * This function is called to atomically decrement the value of a semaphore.
 * It waits for the semaphore value to be greater than 0, then atomically decrements the value.
 * For example, if there are 3 threads blocked waiting on the semaphore, the value will be 0. 
 * If another thread calls ::UniversalClientSPI_Semaphore_Post, it will increment the value to 1, and
 * ONE of the threads will be unblocked so that it can decrement the value back to 0. The other two
 * threads remain blocked. 
 * 
 *
 * @param[in] semaphoreHandle Handle of semaphore previously created using ::UniversalClientSPI_Semaphore_Open.
 * @param[in] milliseconds : The time-out interval, in milliseconds. If a nonzero value is specified, the function waits until the 
 * object is signaled or the interval elapses. 
 * If milliseconds are zero, the function does not enter a wait state if the object is not signaled; it always returns immediately.
 * If milliseconds is 0xffffffff (INFINITE), the function will return only when the object is signaled, this is exactly identical with
 * ::UniversalClientSPI_Semaphore_Wait. 
 *
 * \note This SPI is for PULL EMM. \n
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_WAIT_ABORTED
 * @retval
 *     ::UC_ERROR_OS_FAILURE
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 *
 *\note ::UC_ERROR_SUCCESS , this error code means the semaphore is signaled without timeout.
 *      ::UC_ERROR_NULL_PARAM , this error code means null input parameters.
 *      ::UC_ERROR_WAIT_ABORTED , this error code means timeout(time value of milliseconds)for the semaphore waiting.
 *      ::UC_ERROR_OS_FAILURE , this error code means error happened for the semaphore waiting.
 */
extern uc_result UniversalClientSPI_Semaphore_WaitTimeout(uc_semaphore_handle semaphoreHandle, uc_uint32 milliseconds);

/**
 * Destroy a semaphore object.
 *
 * This function is called to destroy a semaphore previously created using ::UniversalClientSPI_Semaphore_Open.
 * 
 * @param[in] pSemaphoreHandle Pointer to handle of semaphore to destroy. The implementation
 *     should free any resources associated with the semaphore, and MUST set the value of *pSemaphoreHandle to NULL. 
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Semaphore_Close(uc_semaphore_handle * pSemaphoreHandle);
/** @} */ /* End of semaphore */


/** @defgroup mutexspi Cloaked CA Agent Mutex SPIs
 *  Cloaked CA Agent Mutex SPIs
 *
 * These SPIs implement basic thread mutual exclusion via Mutex objects. This
 * is used to prevent multiple threads from simultaneously accessing 
 * mutex-protected objects at the same time. 
 *
 * Recursive mutex support is \b not required. That is, one thread will never
 * attempt to lock a mutex that is already locked by the thread. The mutex 
 * implementation may safely ignore this case. 
 *
 * \note The 'mutex' abstraction is provided to allow platforms to implement
 *    mutexes in a higher-performance manner than a typical semaphore implementation.
 *    Platforms which do not support a separate 'mutex' concept may safely use
 *    a semaphore internal to the implementation of the mutex.
 * 
 *  
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *
 *  @{
 */
/**
 * Create a mutex
 *
 * This function is called to create a mutex object.
 *
 * A 'mutex' is a basic synchronization object with the following properties:
 * - Calling ::UniversalClientSPI_Mutex_Lock locks a mutex. If the mutex was already locked, this
 *   waits until the mutex is unlocked, then locks it.
 * - Calling ::UniversalClientSPI_Mutex_Unlock unlocks the mutex.
 *
 * Mutexes are used by the Cloaked CA Agent API to protect internal data structures
 * from being corrupted by simultaneous access. 
 *
 * @param[out] pMutexHandle Receives a handle to a newly created mutex. When the mutex is no longer needed,
 *     it will be closed using ::UniversalClientSPI_Mutex_Close. 
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Mutex_Open(uc_mutex_handle * pMutexHandle);

/**
 * Locks a mutex
 *
 * This function is called to lock a mutex to prevent other threads from entering
 * a potentially unsafe section of code until the mutex is unlocked. 
 *
 * @param[in] mutexHandle Handle of mutex previously created using ::UniversalClientSPI_Mutex_Open.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Mutex_Lock(uc_mutex_handle mutexHandle);
/**
 * Unlocks a mutex
 *
 * This function is called to unlock a mutex, and release a single thread waiting in 
 * call to ::UniversalClientSPI_Mutex_Lock, if any.  If no threads are waiting, then
 * the first thread to attempt to lock the mutex will not have to wait.
 *
 * @param[in] mutexHandle Handle of mutex previously created using ::UniversalClientSPI_Mutex_Open.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Mutex_Unlock(uc_mutex_handle mutexHandle);

/**
 * Destroy a mutex object.
 *
 * This function is called to destroy a mutex object previously created 
 * using ::UniversalClientSPI_Mutex_Open.
 * 
 * @param[in] pMutexHandle Pointer to handle of semaphore to destroy. 
 *     The implementation
 *     should free any resources associated with the mutex, and MUST set 
 *     the value of *pMutexHandle to NULL. 
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Mutex_Close(uc_mutex_handle * pMutexHandle);
/** @} */ /* End of mutexspi */


/** @defgroup threadspi Cloaked CA Agent Thread SPIs
 *  Cloaked CA Agent Thread SPIs
 *
 *  These SPIs implement basic thread management functions. The Cloaked CA Agent requires
 *  one background thread that is uses for processing commands, ECMs, and EMMs. 
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *  @{
 */
/**
 * Create a new thread and begin executing it.
 *
 * \note It is better to let the threads opened by Cloaked CA Agent have a higher priority. Choose a suitable priority when implementing UniversalClientSPI_Thread_Open.
 * Cloaked CA Agent may have requirements on the threads, for example the stack size, the count of threads. Please find the details in the integration documents. Be careful when implementing UniversalClientSPI_Thread_Open!
 *
 * This function is called to create a new instance of a thread, a thread procedure and associated parameter.
 *
 * @param[in] threadProc Starting point of new thread
 * @param[in] lpVoid Parameter to pass to newly created thread
 * @param[out] pThreadHandle Receives an implementation-specific handle to the newly created thread.
 *     The caller will close the handle in a subsequent call to ::UniversalClientSPI_Thread_Close.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Thread_Open(uc_threadproc threadProc, 
                                       void* lpVoid, uc_thread_handle * pThreadHandle);
/**
 * Suspend the thread for the period specified.
 * 
 * This function is called to suspend the thread for at least the period specified. The minimum granularity of the timer should be 10ms.
 *
 * \note If the requested delay time does not correspond with a boundary of the granularity, the delay time needs to be extended to the next available boundary of the granularity. \n
 *     For example: \n
 *     for platforms that have  1 ms granularity, UniversalClientSPI_Thread_Sleep(hThreadHandle, 1) is to suspend the thread  1 ms; \n
 *     for platforms that have  5 ms granularity, UniversalClientSPI_Thread_Sleep(hThreadHandle, 1) is to suspend the thread 5 ms; \n
 *     for platforms that have 10 ms granularity, UniversalClientSPI_Thread_Sleep(hThreadHandle, 1) is to suspend the thread 10 ms. \n
 *
 * @param[in] hThreadHandle Pointer to handle of thread.
 * @param[in] wDelay is the time in milliseconds to suspend the thread.  
 *
 * \note If the value of hThreadHandle is NULL, it is an invalid value, the SPI implementation shall get the real handle of the thread which is calling ::UniversalClientSPI_Thread_Sleep. 
 * If the value of hThreadHandle is not NULL, it is a valid thread handle.
 *
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Thread_Sleep(uc_thread_handle hThreadHandle, uc_uint16 wDelay);
/**
 * Closes a thread 
 * 
 * This function is called to close a thread object. This function should block until
 * the thread has completed execution. After this, the function should clean up any data associated with
 * the thread and free the handle.
 *
 * \note Threads are typically not closed until ::UniversalClient_StopCaClient is called. 
 *     For platforms that do not have the ability to properly wait for a thread to complete execution (i.e. thread 'join'),
 *     this can be simulated as necessary using some platform-specific approximation-- it will only affect the client during 
 *     shutdown. For platforms that do not have the means to shut down, this can be ignored. 
 *
 * @param[in] pThreadHandle Pointer to handle of thread to close. 
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Thread_Close(uc_thread_handle * pThreadHandle);
/** @} */ /* End of thread */

/** @defgroup psspi Cloaked CA Agent Persistent Storage SPIs
 *  Cloaked CA Agent Persistent Storage SPIs
 *
 *  These SPIs implement the Persistent Storage layer that is required in order to save contents of EMMs
 *  that are delivered to the device. This persistent storage layer must be transaction safe, such that if
 *  the device loses power in the middle of a 'write' operation, the data is not corrupted and the device
 *  can continue to function when it is powered back on. 
 *
 *  \note It is the integrator's responsibility to implement these methods for a particular platform.
 *  Please refer to \ref Note9 "Persistent storage requirements" to get more information.
 *
 *  @{
 */
/**
 * Deletes an existing resource.
 *
 * This function is called to delete an existing resource in the Persistent Store, identified by 'index'. 
 * If the resource
 * cannot be found, the function should return ::UC_ERROR_RESOURCE_NOT_FOUND.
 *
 * The index may have previously been created by a call to ::UniversalClientSPI_PS_Write.
 * A very simple implementation of this may just delete the file corresponding with the index of the resource.
 *
 * @param[in] index Index of the resource to delete. Different indexes correspond to independent resources.
 *     For example, resources for index 0 and 1 should be stored in two separate files.\n 
 *     Index 0 is used to store CA data.\n
 *     Index 41~47 are also used to store CA data for multiple secure cores feature.\n
 *     Index 1~8 and 31~38 are used to store Cloakded CA Agent Package if FlexiFlash is enabled. In most cases, only part of these indexes 
 *     need to be supported. For example if only one Cloaked CA Agent Package is deployed, only index 1 and 31 are needed.\n
 *     Index 9 is used to store the configuration file which is provided by Irdeto. The file must be embedded by manufacturer.\n
 *     Index 10, 11 and 12 are used for OTP(one time program) storage on the device. 
 *     Currently, only devices with Pre-enablement feature need to access these OTP areas. 
 *     Devices shall ingore these 3 indexes if not support Pre-enablement feature.
 *     Secure Pre-enablement feature does not need the above 3 indexes either.\n
 *     Index 20 is for Softcell if the Cloaked CA Agent is configured to support Unified Client.\n 
 *     Index 51 and 61 are used to store IFCP data if IFCP is supported.\n
 *     New indexes may be used for other features in future.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_RESOURCE_NOT_FOUND
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_PS_Delete(uc_uint32 index);

/**
 * Writes data to an existing resource.  
 *
 * This function is called to write a block of data to a resource identified by an index.  The data length and offset
 * within the resource to begin writing is also specified.
 *
 * If the resource did not exist before the call, it should be created. 
 *
 * A very simple implementation of this may just write the data to a file corresponding to the index of the resource.
 *
 * @param[in] index Index of the resource to write into. Different indexes correspond to independent resources.
 *     For example, resources for index 0 and 1 should be stored in two separate files.\n 
 *     Index 0 is used to store CA data.\n
 *     Index 41~47 are also used to store CA data for multiple secure cores feature.\n
 *     Index 1~8 and 31~38 are used to store Cloakded CA Agent Package if FlexiFlash is enabled. In most cases, only part of these indexes 
 *     need to be supported. For example if only one Cloaked CA Agent Package is deployed, only index 1 and 31 are needed.\n
 *     Index 9 is used to store the configuration file which is provided by Irdeto. The file must be embedded by manufacturer.\n
 *     Index 10, 11 and 12 are used for OTP(one time program) storage on the device. 
 *     Currently, only devices with Pre-enablement feature need to access these OTP areas. 
 *     Each area can store only one Pre-enablement product. If only one is supported, please bind it to index 10. 
 *     If two are supported, bind them to index 10 and 11. Please refer to integration document for Pre-enablement for details.
 *     Devices shall ingore these 3 indexes if not support Pre-enablement feature.
 *     Secure Pre-enablement feature does not need the above 3 indexes either.\n
 *     Index 20 is for Softcell if the Cloaked CA Agent is configured to support Unified Client.\n 
 *     Index 51 and 61 are used to store IFCP data if IFCP is supported.\n
 *     New indexes may be used for other features in future.
 *
 * @param[in] offset Offset within the resource to begin writing. Data before this offset must not be affected.
 * @param[in] pData Buffer containing data to write to the resource. The uc_buffer_st::bytes member 
 *     points to the start of the buffer, and the uc_buffer_st::length member contains the number of bytes in the buffer
 *     to write.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_RESOURCE_NOT_FOUND
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_PS_Write(uc_uint32 index, uc_uint32 offset, const uc_buffer_st *pData);

/**
 * Reads data from an existing resource.
 *
 * This function is called to read a block of data from a resource identified by an index. The data length and offset
 * within the resource to begin reading is also specified.
 *
 * If the resource did not exist before the call, the function should return ::UC_ERROR_RESOURCE_NOT_FOUND.
 *
 * A very simple implementation of this may just read the data from a file corresponding to the index of the resource.
 * 
 * \note If the device declares to support N bytes for this index, it must be able to be read by Cloaked CA Agent in any position 
 * from offset 0 to N-1 at anytime, even before any data is actually written to it. 
 *
 * @param[in] index Index of the resource to read from. Different indexes correspond to independent resources.
 *     For example, resources for index 0 and 1 should be stored in two separate files.\n 
 *     Index 0 is used to store CA data.\n
 *     Index 41~47 are also used to store CA data for multiple secure cores feature.\n
 *     Index 1~8 and 31~38 are used to store Cloakded CA Agent Package if FlexiFlash is enabled. In most cases, only part of these indexes 
 *     need to be supported. For example if only one Cloaked CA Agent Package is deployed, only index 1 and 31 are needed.\n
 *     Index 9 is used to store the configuration file which is provided by Irdeto. The file must be embedded by manufacturer.\n
 *     Index 10, 11 and 12 are used for OTP(one time program) storage on the device. 
 *     Currently, only devices with Pre-enablement feature need to access these OTP areas. 
 *     During startup, Cloaked CA Agent tries to read index 10, 11 and 12 to detect how many OTP areas are actually supported by the device by calling
 *     this SPI. All return values other than ::UC_ERROR_SUCCESS and ::UC_ERROR_IO_DATA_CORRUPTED are regarded as this index is not supported.
 *     Each area can store only one Pre-enablement product. If only one is supported, please bind it to index 10. 
 *     If two are supported, bind them to index 10 and 11. ::UC_ERROR_IO_DATA_CORRUPTED is regarded as the OTP data is corrupted, and Pre-enablement
 *     will be blocked. Please refer to integration document for Pre-enablement for details.
 *     Devices shall ingore these 3 indexes if not support Pre-enablement feature.
 *     Secure Pre-enablement feature does not need the above 3 indexes either.\n
 *     Index 20 is for Softcell if the Cloaked CA Agent is configured to support Unified Client.\n
 *     Index 51 and 61 are used to store IFCP data if IFCP is supported.\n
 *     New indexes may be used for other features in future.
 *
 * @param[in] offset Offset within the resource to begin reading. Data before this offset must not be affected.
 * @param[in,out] pData Buffer to read resource data into. 
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member 
 *     to the length of the buffer, in bytes.  The function will read uc_buffer_st::length bytes from the resource
 *     and store them in uc_buffer_st::bytes. 
 *     
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_RESOURCE_NOT_FOUND
 * @retval
 *     ::UC_ERROR_IO_DATA_CORRUPTED
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_PS_Read(uc_uint32 index, uc_uint32 offset, uc_buffer_st *pData);

/**
 * Get the property of a resource.
 *
 * This function is called to get the property of a resource identified by an index.
 * The property includes the reserved size of this resource.
 * 
 * The SPI is introduced from Cloaked CA Agent 4.6.0.
 * This SPI must be implemented for any new device integrations.
 * \note The property must be provided even before any data is actually written to it.
 *
 * @param[in] index Index of the resource to get the property. Different indexes correspond to independent resources.
 *     However, different resources may have the same properties.
 *     For example, Index 1 and 31 are used to store the Secure Core Package and have the same property.\n
 *
 * @param[out] pProperty returns the property of the resource.
 *     
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_RESOURCE_NOT_FOUND
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_PS_GetProperty(uc_uint32 index, uc_ps_property *pProperty);

/**
 * Performs any initialization of the persistent storage layer
 *
 * This function is called during to allow any initialization of the persistent storage layer to occur
 * before any calls to ::UniversalClientSPI_PS_Read, ::UniversalClientSPI_PS_Write, ::UniversalClientSPI_PS_GetProperty, or ::UniversalClientSPI_PS_Delete are made.
 * 
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_PS_Initialize(void);

/**
 * Performs any termination of the persistent storage layer
 *
 * This function is called during to allow any clean up of the persistent storage layer done by ::UniversalClientSPI_PS_Initialize to occur.
 * No calls to ::UniversalClientSPI_PS_Read, ::UniversalClientSPI_PS_Write, ::UniversalClientSPI_PS_GetProperty, or ::UniversalClientSPI_PS_Delete will be made after this function
 * returns. 
 * 
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_PS_Terminate(void);


/** @} */ /* End of ps */

/** @defgroup devicespi Cloaked CA Agent Device SPIs
 *  Cloaked CA Agent Device SPIs
 *
 *  These SPIs are responsible for retrieving values that are provisioned onto a device at manufacture time,
 *  and for setting any hardware specific parameters in the device.
 *  
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *  @{
 */

/**
 * Gets the client device ID
 *
 * This function is called to retrieve the ID associated with the device. 
 *
 * \note For Cloaked CA Agent for Secure Chipset based STBs: Currently, the device ID is treated as the CSSN of the device. bytes in pData points to an 
 * array , the data type of the array is HEX, e.g. ::uc_byte DeviceID[] = {0x01, 0x00, 0x00, 0x02}.
 *
 * \note For Cloaked CA Agent for IP only STBs: Currently, the device ID is treated as the MAC address of the device. bytes in pData points to an 
 * array , the data type of the array is HEX, e.g. ::uc_byte DeviceID[] = {0x43,0x0b, 0xa9, 0x92, 0x11, 0xce}.
 *
 * @param[in] pData Buffer to read device ID data into. 
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to include
 *     the device ID, and set the uc_buffer_st::length member to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire data, the uc_buffer_st::length member
 *     should be set to the minimum required buffer size, and the function should return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 * \note This SPI is for PULL EMM. \n
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_GetDeviceID(uc_buffer_st * pData);

/**
 * Gets the Secure Chipset Serial Number
 *
 * This function is called to retrieve the secure chipset serial number associated with the device.
 * This serial number is used to deliver EMMs addressed to this particular instance of a client.
 *
 * \note For Cloaked CA Agent for Secure Chipset based STBs: The device must be responsible for data validation. 
 * If the data is invalid, this function returns ::UC_ERROR_NULL_PARAM.
 *
 * \note For Cloaked CA Agent for IP only STBs: Currently CSSN is obtained from the client registration reponse  
 * message and is personalized into the device persistently through ::UniversalClientSPI_Device_SetCSSN.
 * If the CSSN is not yet set by ::UniversalClientSPI_Device_SetCSSN, this function returns 
 * ::UC_ERROR_NULL_PARAM and the length of pData is 0.
 *
 * @param[in] pData Buffer to read serial number data into. 
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  The function modifies the contents of the buffer to include
 *     the secure chipset serial number, and sets the uc_buffer_st::length member 
 *     to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire data, the uc_buffer_st::length member
 *     must be set to the minimum required buffer size, and the function returns ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_GetCSSN(uc_buffer_st * pData);

/**
 * Gets the device private data
 *
 * This returns the personalized device specific private data.  This is an optional SPI, the device may not implement it
 * if no such private data is available. This function is only used by Cloaked CA Agent for Secure Chipset based STBs.
 *
 * \note Currently, device private data is 16 bytes long. These data are provided by manufacturer and need to be personalized into device persistently.
 * Devices that are for demo purpose or for test purpose may NOT have private data at the first beginning, in such case, this SPI implementation
 * can just set the pData->length to ZERO and return UC_ERROR_SUCCESS directly.
 *
 * @param[in] pData Buffer to read private data into. 
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to include
 *     the private data, and set the uc_buffer_st::length member to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire data, the uc_buffer_st::length member
 *     should be set to the minimum required buffer size, and the function should return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_GetPrivateData(uc_buffer_st * pData);

/**
 * Sets the Secure Chipset Session Key
 *
 *
 * This function is called to provide a secure chipset with a secure session key.  This session key is
 * used to protect descrambler keys and is encrypted with another key that only this device knows about. 
 * This SPI may be called several times with duplicated data, device application could check if the actual CSSK is changed before using it.
 * This function is only used by Cloaked CA Agent for Secure Chipset based STBs.
 *
 * \note For new integration, please use ::UniversalClientSPI_Device_SetExtendedCSSK instead of this SPI. This SPI is only reserved for compatibility.
 *
 * \n
 *
 * \note Some secure chipsets require swapping the CSSK or/and CW. SoftCell3 will perform the swap internally so that the output meets the secure chipset's requirements.
 *    Cloaked CA Agent solution does not support internal swapping. In order to run the Cloaked CA Agent successfully, the swap function MUST be disabled.
 *    In this case, the client device manufacturers must contact the chipset vendor to find out how to disable the swap function.
 *    If the descramble status of the Cloaked CA Agent is D029, but there is no audio/video, it is possible that the chipset needs to be configured to disable the swap function.
 *    The client device manufacturers may consult with Irdeto if necessary.
 *
 * \n
 *
 * \note This SPI ONLY supports 2-TDES key ladder, i.e. CSSK is encrypted by chipset unique key and control word is encrypted by CSSK and the
 *    Encryption algorithm is TDES. See ::uc_dk_protection for list of the types of protection that can be provided by a platform. This key ladder is only used for 
 *    normal service descrambling, i.e. the CW key ladder.
 *
 * \n
 *
 * \note Whether the chipset needs to use secure control word mode or clear control word mode is not decided by CSSK. 
 *    The driver must judge current working mode using the control word type from ::UniversalClientSPI_Stream_SetDescramblingKey.
 *    In order to support 911 mode, client device with secure chipset must support to switch the working mode dynamically.
 *
 * \n
 *
 * \note This SPI is used to set CSSK for standard \ref glossary_msr "MSR" key ladder. 
 *    If \ref glossary_ifcp "IFCP" is supported and the device is required to use IFCP key ladder, this SPI will not be called.
 *
 * \n
 *
 * @param[in] pKeyMaterial CSSK key material to provision into the device. This is normally protected by a secure chipset unique key
 *     (CSUK) that resides within the secure chipset such that an attacker gains no advantage by intercepting this method.
 *     The uc_buffer_st::bytes member points to a buffer containing the raw key material, and the uc_buffer_st::length member contains 
 *     the number of bytes in the buffer.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_SetCSSK(const uc_buffer_st * pKeyMaterial);

/**
 * Sets the Secure Chipset Session Key With key information that supports different key ladders.
 *
 * This is the extension of ::UniversalClientSPI_Device_SetCSSK.
 * Additional key protection information is provided to support different key ladders.
 * See ::uc_dk_protection for list of the types of protection that can be provided by a platform.
 * See ::uc_dk_ladder for list of the key ladder types that can be provided by a platform. 
 * This SPI may be called several times with duplicated data, device application could check if the actual CSSK is changed before using it.
 * This function is only used by Cloaked CA Agent for Secure Chipset based STBs.
 *
 * \note For new integration, this SPI must be used instead of ::UniversalClientSPI_Device_SetCSSK.
 * 
 * \n
 *
 * \note Some secure chipsets require to swap CSSK or/and CW. SoftCell3 will perform the swap internally so that the output meets the secure chipset's requirement.
 *    Cloaked CA Agent solution does not support the ability to perform the swap internally. In order to run Cloaked CA Agent successfully, the swap function MUST be 
 *    disabled. The client device manufacturers shall contact chipset vendor to figure out how to disable swap function in such case.
 *    If the descramble status of Cloaked CA Agent is D029, but no audio/video, it is possible that the chipset needs to be configured to disable swap function.
 *    The client device manufacturers may consult with Irdeto if necessary.
 *
 * \n
 *
 * \note This SPI is used to set CSSK for standard \ref glossary_msr "MSR" key ladder. 
 *    If \ref glossary_ifcp "IFCP" is supported and the device is required to use an IFCP key ladder, this SPI will not be called.
 *
 * \n
 *
 * @param[in] pCSSKInfo Information about the session key for the secure chipset. The usage of the CSSK is different for different key ladder types, see ::uc_cssk_info.
 * For example, if it is for CW key ladder, the session key is normally enctypted by the CSUK and used to encrypt the control words.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_SetExtendedCSSK(const uc_cssk_info * pCSSKInfo);


/**
 * Retrieves device's security state
 *
 * This function is called to retrieve security state information from a device.
 * Cloaked CA Agent will combine the information from this method to generate a \ref glossary_tsc "TSC" CHIP record. 
 * This function is only used by Cloaked CA Agent for Secure Chipset based STBs.
 *
 * @param[in,out] pDeviceSecurityState Secure chipset working information to be filled in by the platform.
 *
 * See ::uc_device_security_state for the detail information needed.
 * 
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_GetSecurityState(uc_device_security_state * pDeviceSecurityState);

/**
 * Retrieves the PVR key ladder security state in secure chipset
 *
 * This function is called to retrieve security state information of a PVR key ladder.
 * \note This SPI is needed only if the device supports PVR key ladder. If the device does not support PVR key ladder,
 * no need to support this SPI. This method is only used by Cloaked CA Agent for Secure Chipset based STBs.
 * Cloaked CA Agent will combine the information from this method to generate a \ref glossary_tsc "TSC" CHIP record. 
 *
 * \n
 *
 * \note If \ref glossary_ifcp "IFCP" is supported, the PVR key ladder level and algorithm must be the same
 * for \ref glossary_msr "MSR" and \ref glossary_ifcp "IFCP".
 *
 * @param[in,out] pPVRSecurityState Device application must fill this structure with the PVR key ladder working information in secure chipset.
 *
 * See ::uc_pvr_security_state for the detail information needed.
 * 
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_GetPVRSecurityState(uc_pvr_security_state * pPVRSecurityState);

/**
 * Generate the Chip Configuration Checking.
 *
 * This function is called to ensure the configuration data integrity and authenticity. 
 * Secure chips must be able to generate a MAC using the standard CBC-MAC scheme with AES-128 block cipher.
 * \note If the Chip Configuration Checking is not supported by secure chip (check with chip vendor), UC_ERROR_CCC_NOT_SUPPORT must be returned immediately.
 * This function is only used by Cloaked CA Agent for Secure Chipset based STBs.
 *
 * @param [in] chipConfigurationRequest Chip Configuration Check Request
 * @param [out] pChipConfigurationResponse Chip Configuration Check Response.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_CCC_NOT_SUPPORT
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */

extern uc_result UniversalClientSPI_Device_GetChipConfigurationCheck(
                    uc_chip_configuration_request chipConfigurationRequest, 
                    uc_chip_configuration_response *pChipConfigurationResponse);

/**
 * Gets the device platform identifiers
 *
 * This function is called to retrieve relevant identifiers from a device.
 *
 * @param[in,out] pDevicePlatformIdentifiers Device's identifiers to be filled in by the platform.
 *
 * See ::uc_device_platform_identifiers for the detail information needed.
 * 
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_GetPlatformIdentifiers(uc_device_platform_identifiers * pDevicePlatformIdentifiers);

/**
 * Gets the device personalized data
 *
 * This function is called to retrieve personalized data in a device. 
 * The personalized data is written to the device on the production line.
 * This function is only used by Cloaked CA Agent for Secure Chipset based STBs.
 * Currently, this SPI is used for the Pre-enablement feature or Secure Pre-enablement feature, which requires personalized data to work.
 * \note The personalized data does NOT have a fixed length. Specifically, the PESK (PreEnablement Session Key) does not have a fixed length.
 *
 * @param[in,out] pData The device's personalized data.
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to include
 *     the personalized data, and set the uc_buffer_st::length member to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire data, the uc_buffer_st::length member
 *     should be set to the minimum required buffer size, and the function should return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_GetPersonalizedData(uc_buffer_st* pData );

/**
 * Sets the Secure Chipset Serial Number
 *
 * This function is called to set the secure chipset serial number associated with the device. This function is only used by 
 * Cloaked CA Agent for IP only STBs. This serial number is used to deliver EMMs addressed to this particular instance of a client.
 * The Cloaked CA Agent obtains the Secure Chipset Serial Number from the Client registration response message.
 * 
 * \note Only after the CSSN is set via ::UniversalClientSPI_Device_SetCSSN, can the Cloaked CA Agent get the valid CSSN 
 * via ::UniversalClientSPI_Device_GetCSSN.
 *
 * @param[in] pData Buffer of serial number data. 
 *     On input, the uc_buffer_st::bytes member points to a buffer stored the serial number, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_SetCSSN(const uc_buffer_st * pData);

/**
 * Sets the Multi-2 parameters for Multi-2 descrambling
 *
 * This function is called when service is scrambled by Multi-2 algorithm. The parameters will be set before CWs are output.
 *
 * Refer to \ref baf_arib "Basic Application Flow - ARIB" to get an overview.
 *
 * @param[in] streamHandle indicates the associated stream.
 * @param[in] pMulti2Parameter multi-2 parameters.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_SetMulti2Parameter(uc_stream_handle streamHandle, uc_device_multi2_parameter *pMulti2Parameter);

/**
 * Gets the device PIN code
 *
 * This function is called to obtain the PIN code associated with the device. This function is only used by Cloaked CA Agent for IP only STBs.  
 *
 * \note Currently, the length of the PIN code is 8 bytes, and bytes in pData points to a null-terminated char string.
 * e.g. bytes in pData points to "12345678", and length in pData equals to strlen("12345678").
 *
 * @param[in] pData Buffer to read PIN code data info. 
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes. The function modifies the contents of the buffer to include
 *     the PIN code, and sets the uc_buffer_st::length member to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire data, the uc_buffer_st::length member
 *     must be set to the minimum required buffer size, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_GetPINCode(uc_buffer_st * pData);

/**
 * Gets the Client's IP Address
 *
 * This function is called to retrieve Client's IP Address, bytes in pData points to a null-terminated char string, 
 * e.g. bytes in pData points to "192.xx.xx.01", length in pData equals to strlen("192.xx.xx.01").
 *
 * @param[in] pData Buffer to read Client's IP Address. 
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to include
 *     the data string, and set the uc_buffer_st::length member 
 *     to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire data, the uc_buffer_st::length member
 *     should be set to the minimum required buffer size, and the function should return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 * \note This SPI is for PULL EMM. \n
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */

extern uc_result UniversalClientSPI_Device_GetIPAddress(uc_buffer_st * pData);

/** @} */ /* End of devicespi */

/** @defgroup pvrspi Cloaked CA Agent PVR SPIs
 *  Cloaked CA Agent PVR SPIs
 *
 *  Cloaked CA Agent will output PVR relevant info via these SPIs.
 *  To access PVR functionality, the application should also invoke \ref pvrapis "Cloaked CA Agent PVR APIs"
 *  to cooperate with these SPIs.
 *
 *  
 *  @{
 */

/**
 * Set PVR session key
 *
 * This method is only used for devices without PVR key ladder. If the device has PVR key ladder, please use the SPI
 * ::UniversalClientSPI_PVR_SetExtendedSessionKey.
 *
 * This function is called to set a session key in response to the following requests:
 * -# Request for a PVR record via ::UniversalClient_ConfigService or ::UniversalClient_ConfigConnection
 * -# Request for a PVR playback via ::UniversalClient_SubmitPVRSessionMetadata
 *
 * At a PVR record session, SPI implementation should use this PVR Session Key to encrypt the descrambled content before saving it to storage.
 * At a PVR plackback session, SPI implementation should use this PVR Session Key to decrypt the encrypted content before viewing the content.
 *
 * @param[in] streamHandle The handle of the stream returned by ::UniversalClientSPI_Stream_Open for PVR record.
 * @param[in] pPVRSessionKey Buffer containing a PVR Session Key. 
 *     The uc_buffer_st::bytes member points to a buffer containing the PVR Session Key, and
 *     the uc_buffer_st::length member is set to the length of the session key.Session key length currently is 16 bytes long.
 *     
 * 
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */ 
extern uc_result UniversalClientSPI_PVR_SetSessionKey(
                        uc_stream_handle streamHandle,
                        const uc_buffer_st *pPVRSessionKey);

/**
 * Set PVR session key with key protection informatioin
 * 
 * This method is used for devices with PVR key ladder. If the device does not have PVR key ladder, please use SPI: 
 * ::UniversalClientSPI_PVR_SetSessionKey.
 * This key is used to record or playback programs.
 * This method is only used by Cloaked CA Agent for Secure Chipset based STBs.
 *
 * This is the extension of ::UniversalClientSPI_PVR_SetSessionKey function with additional key protection information provided, 
 * This function is called to set a session key in response to the following requests:
 * -# Request for a PVR record via ::UniversalClient_ConfigService or ::UniversalClient_ConfigConnection
 * -# Request for a PVR playback via ::UniversalClient_SubmitPVRSessionMetadata
 *
 * At a PVR record session, SPI implementation should use this PVR Session Key to encrypt the descrambled content before saving the content to storage.
 * At a PVR plackback session, SPI implementation should use this PVR Session Key to decrypt the encrypted content before viewing the content.
 * When PVR key ladder is used, and if this key protection field indicates the key is encrypted, it needs to be routed to PVR key ladder and the above encryption and decryption work 
 * is done inside secure chipset.
 * If the key protection indicates the key is not protected, it can be used directly to record or playback without PVR key ladder. This is to be compatible with already recorded content
 * when there was no PVR key ladder.
 * Device application can decide whether and how the key is encrypted through uc_pvrsk_info::KeyProtection.
 *
 * \note Device only needs to support one SPI, either ::UniversalClientSPI_PVR_SetExtendedSessionKey or ::UniversalClientSPI_PVR_SetSessionKey.
 *
 * \n
 *
 * \note If the PVR session key is protected, this SPI is used to set the protected PVR session key for standard \ref glossary_msr "MSR" PVR key ladder. 
 *    If \ref glossary_ifcp "IFCP" is supported and the device is required to use IFCP PVR key ladder, ::UniversalClientSPI_IFCP_Communicate will be called instead.
 *
 * @param[in] streamHandle The handle of the stream returned by ::UniversalClientSPI_Stream_Open for PVR record.
 * @param[in] pPVRSKInfo The key used to enctypt or decrypt programs.    
 * 
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */ 
extern uc_result UniversalClientSPI_PVR_SetExtendedSessionKey(
                        uc_stream_handle streamHandle,
                        const uc_pvrsk_info * pPVRSKInfo);


/** @} */ /* End of pvrspi */



/** @defgroup streamspi Cloaked CA Agent Stream SPIs
 * Cloaked CA Agent Stream SPIs
 *  
 * These SPIs implement the functionality of a 'stream'. A stream is an object that the client 
 * can connect to in order to set up filters and receive CA messages, like ECMs and EMMs, and receive
 * keys that can be used to descramble content. 
 *
 * Streams are created by the client when a method that requires a stream instance is
 * called. 
 *
 * \note Cloaked CA Agent does not have the concept of descrambler, please refer to \ref Note12 "When and how to open descrambler"
 * for more information on descrambler handling.
 *
 * A stream implementation is responsible for the following:
 *
 * - Managing lifetime of an instance of a stream, via ::UniversalClientSPI_Stream_Open and ::UniversalClientSPI_Stream_Close methods.
 * - Connecting to the CA processing aspect of the client via ::UniversalClientSPI_Stream_Connect and ::UniversalClientSPI_Stream_Disconnect.
 * - Handling filters via the ::UniversalClientSPI_Stream_OpenFilter and ::UniversalClientSPI_Stream_CloseFilter methods.
 * - Providing sections data via the ::uc_notify_callback method.
 * - Forwarding control words received via ::UniversalClientSPI_Stream_SetDescramblingKey to the corresponding descrambler(s).
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *
 * For more information on how streams are used, see \ref stream_interface_call_sequence "Stream Interface Call Sequence". 
 *  @{
 */

/**
 * Starts all filters for this stream.
 *
 * This function is called to start all filters that were allocated and set for this stream
 * using ::UniversalClientSPI_Stream_OpenFilter and ::UniversalClientSPI_Stream_SetFilter. 
 * 
 * Once this method is called, a stream implementation is free to call the ::uc_notify_callback 
 * function originally passed to ::UniversalClientSPI_Stream_Connect for this stream instance.
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
*/
uc_result UniversalClientSPI_Stream_Start(uc_stream_handle streamHandle);

/**
 * Stops all filters for this stream.
 *
 * This function is called to stop all filters that were allocated and set for this stream
 * using ::UniversalClientSPI_Stream_OpenFilter and ::UniversalClientSPI_Stream_SetFilter. 
 * 
 * Once this method is called, then no more data should be passed to ::uc_notify_callback function 
 * originally passed to ::UniversalClientSPI_Stream_Connect for this stream instance.
 *
 * 
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_Stop(uc_stream_handle streamHandle);

/**
 * Stream method for opening a filter.
 *
 * A stream is controlled by the adding 
 * and removal of 'filters'. Each filter describes
 * what kinds of sections are expected to be received from the stream. The stream implementation
 * is only required to provide sections that match one of the currently active filters. 
 *
 * This method is called to add a filter to an existing stream. Before this callback is 
 * called, no data should be provided via the notification function. 
 *
 * After an EMM stream is opened and connected, this method is called several times to set up different filters.
 * Each EMM stream will need 7 filters.
 * 
 * For an ECM stream, only a single filter is used. 
 *
 * After this method is called, ::UniversalClientSPI_Stream_SetFilter is called to provide or change the filter
 * rules on the filter handle. After all filters are initially set up, ::UniversalClientSPI_Stream_Start is called.
 *
 * #UC_INVALID_HANDLE_VALUE is not a valid filter handle. If the function returns success, it must not
 * set *pFilterHandle to #UC_INVALID_HANDLE_VALUE. 
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[out] pFilterHandle Receives an implementation defined filter handle that can be used to 
 *      identify the filter in a subsequent call to ::UniversalClientSPI_Stream_CloseFilter.
 *      
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_OpenFilter(
    uc_stream_handle streamHandle,
    uc_filter_handle *pFilterHandle);

/**
 * Stream method for setting filter information.
 * 
 * This method is called after ::UniversalClientSPI_Stream_OpenFilter for each filter.  This method is also
 * called when a message is received that requires changing of filter rules. 
 *
 * \note Mask/match/filterDepth used by UniversalClientSPI_Stream_SetFilter includes 2 bytes after table_id, i.e. section_syntax_indicator and section_length.
 * If filterDepth is equal to 2 or above and client device driver does not support to set filter conditions to these 2 bytes, Client device shall disregard these 2 bytes.
 *
 *  - For EMM streams, filter rules change when a new group unique address is assigned to the client. 
 *    In a typical Irdeto CA system, a group unique address can be reassigned as necessary to 
 *    minimize bandwidth.  
 *  - For ECM streams, filter rules change when a new ECM arrives with a different table ID than the 
 *    previous ECM. In a typical Irdeto CA system, old ECMs are played out until the descrambling key
 *    cycles. After that, the new ECM is delivered with an alternate table IDs, and repeated with that
 *    table ID until the next key cycle. This filter is used to prevent the resubmission of duplicate 
 *    ECMs to the client.
 *
 * @param[in] filterHandle Filter handle previously allocated by ::UniversalClientSPI_Stream_OpenFilter.
 * @param[in] pFilterRules The rule set of the filter to create.  If this is NULL, the implementation
 *      should return all data that arrives, regardless of pattern i.e. NULL means data is unfiltered. 
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_SetFilter(
    uc_filter_handle filterHandle,
    const uc_filter *pFilterRules);


/**
 * Stream method for closing a filter
 *
 * This function is called to close an existing filter from a stream that was previously
 * added by a call to ::UniversalClientSPI_Stream_OpenFilter.  
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in,out] pFilterHandle On input, points to a filter handle previously allocated by 
 *      ::UniversalClientSPI_Stream_OpenFilter.  Any private data 
 *      associated with filterHandle should be cleaned up by this method. When the method returns,
 *      it must set *pFilterHandle to #UC_INVALID_HANDLE_VALUE to indicate it has freed the handle. 
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_NOT_IMPLEMENTED
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_CloseFilter(
    uc_stream_handle streamHandle,
    uc_filter_handle *pFilterHandle);

/**
 * Method to connect to a stream.
 *
 * After a stream is opened with a call to ::UniversalClientSPI_Stream_Open, internally the client will call 
 * this stream interface method in order to complete the connection setup. 
 *
 * Part of the connection process involves informing the stream of a notification callback and connection handle
 * that are used to deliver new ECM and EMM sections to the client.
 *
 * After connecting to a stream, the client may set up filters by calling ::UniversalClientSPI_Stream_OpenFilter.
 * After this is done, the stream may begin sending sections to the notification callback, specifying the 
 * connection handle passed into this call to \a Connect. 
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] connectionHandle Connection handle to associate with this stream. This must also be 
 *     passed to the \a notifyCallback function when sections arrive. 
 * @param[in] notifyCallback Function to be called when sections arrive.Implementations that receive one section at a time can safely pass the data directly
 *     (possibly in parallel, as this function is reentrant), whereas implementations that receive
 *      blocks of sections can also safely pass the data directly without having to parse the sections
 *      and break it into individual section.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_Connect(
    uc_stream_handle streamHandle,
    uc_connection_handle connectionHandle,
    uc_notify_callback notifyCallback);

/**
 * Method to connect to a stream with extended callback for data(ECM sections or web service response or proximity detection message) delivery with additional information. 
 *
 * After a stream is opened with a call to ::UniversalClientSPI_Stream_Open, internally the client will call 
 * this stream interface method in order to complete the connection setup. 
 *
 * Part of the connection process involves informing the stream of a notification callback and connection handle
 * that are used to deliver data(ECM sections or web service response or proximity detection message) to the client.
 *
 * After connecting to a stream, the client may set up filters by calling ::UniversalClientSPI_Stream_OpenFilter. 
 * After this is done, the stream may begin sending data(ECM sections or web service response or proximity detection message) to the notification callback, specifying the 
 * connection handle passed into this call to \a Connect .
 *
 * See also \ref vod_playback_flow "VOD Playback Flow - Out-Of-Band ECM"
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] connectionHandle Connection handle to associate with this stream. This must also be 
 *     passed to the \a notifyCallback function when data(ECM sections or web service response or proximity detection message) arrives. 
 * @param[in] notifyCallback Function to be called when data(ECM sections or web service response or proximity detection message) arrives. 
 *      For ECM sections delivery, implementations that receive one section at a time can safely pass the data directly
 *      (possibly in parallel, as this function is reentrant), whereas implementations that receive
 *      blocks of sections can also safely pass the data directly without having to parse the sections
 *      and break it into individual section.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_Extended_Connect(
    uc_stream_handle streamHandle,
    uc_connection_handle connectionHandle,
    uc_notify_callback_extended notifyCallback);

/**
 * Stream method to disconnect a stream. 
 *
 * This method is provided to give the stream an opportunity to clean up resources related to a connection previously
 * made by a call to ::UniversalClientSPI_Stream_Connect.  
 *
 * A stream that is disconnected should not continue to call ::uc_notify_callback 
 * after this call returns. 
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] connectionHandle Connection handle originally passed to ::UniversalClientSPI_Stream_Connect.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_Disconnect(
    uc_stream_handle streamHandle,
    uc_connection_handle connectionHandle);

/**
 * Stream method to pass a key to the descrambler
 *
 * For ECM streams only: this method is provided to send descrambling keys extracted from ECMs on
 * this stream to the descrambler for each active component previously added by ::UniversalClientSPI_Stream_AddComponent.
 *
 * For other streams, this method will not be called.
 *
 * \note Currently, Cloaked CA Agent will output clear CW when the service is in 911 mode or the client is anchored with security ID.
 *    For platform with secure chipset anchor, it is highly recommended that the platform supports dual mode, i.e. the platform shall be able to disable or enable secure chipset function dynamically, 
 *    so that the secure chipset can descramble the content with clear CW. In case the platform can NOT support dual mode, the platform will NOT be able to descramble the service in 911 mode.
 *
 *    \n
 *
 * \note Some secure chipsets require to swap CSSK or/and CW. SoftCell3 will perform the swap internally so that the output meets the secure chipset's requirement.
 *    Cloaked CA Agent solution does not support the ability to perform the swap internally. In order to run Cloaked CA Agent successfully, the swap function MUST be disabled.
 *    The client device manufacturers shall contact chipset vendor to figure out how to disable swap function in such case.
 *    If the descramble status of Cloaked CA Agent is D029, but no audio/video, it is possible that the chipset needs to be configured to disable swap function.
 *    The client device manufacturers may consult with Irdeto if necessary.
 *
 *    \n
 *
 * \note Cloaked CA Agent ONLY supports 2-TDES key ladder currently, i.e. CSSK is encrypted by chipset unique key and control word is encrypted by CSSK.
 *    Encryption algorithm is TDES.
 *
 *    \n
 *
 * \note For Hybrid client, if the stream handle binds to HLS service, the descrabmling key MUST be set to PVR key ladder,
 *    refer to ::UniversalClientSPI_Device_SetExtendedCSSK.
 *
 *    \n
 *
 * \note If the descrambling key is protected, the SPI is used to set the protected descramling key for standard \ref glossary_msr "MSR" key ladder. 
 *    If \ref glossary_ifcp "IFCP" is supported and the device is required to use IFCP key ladder, ::UniversalClientSPI_IFCP_Communicate will be called instead.
 *
 *  \n
 *
 * \note If descramblingKeyProtection in the ::uc_key_info is UC_DK_PROTECTION_CLEAR,  
 * that means this stream needs a Non Secure Media Pipeline, if the pipeline currently is configured as an Secure Media Pipeline, 
 * the SPI implementation needs to return ::UC_ERROR_DRIVER_FAILURE.
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] pKeyInfo Information about the key being used to descramble content. This contains key algorithm, protection,
 *     cryptography context, and the key material.
 * @param[in] keyVersion Key version that this descrambling key is associated with. 
 *     Different key versions refer to potentially different keys. The descrambler 
 *     should maintain separate descrambling keys for each key version. The key version
 *     typically cycles between 1 and N, where N is the number of different key 
 *     versions supported by the scrambler. This value is used to 
 *     allow for smooth transition from one key to the next when the descrambling key 
 *     changes. At a minimum, implementations should be able to support at least 2 
 *     key identifiers, 0 and 1.
 *     \note For DVB, 0 indicates Even Key and 1 indicates Odd Key. For HLS, Even key and Odd Key are identical, Application can ignored this parameter, 
 *     and uses either  Even key or Odd Key.
 *
 *     \n
 *
 *     \note The uc_key_info::descramblingKeyAlgorithm should be ignored when playing MPEG DASH streams as the type of scrambling algorithm is applied by the OTT packager and might be unknown to Irdeto.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
  * @retval 
 *    ::UC_ERROR_DRIVER_FAILURE
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_SetDescramblingKey(
    uc_stream_handle streamHandle,
    const uc_key_info *pKeyInfo,
    uc_uint32 keyVersion);

/**
 * Stream method to clean the descrambler keys stored in client device.
 * 
 * This SPI has been obsolete since Cloaked CA Agent 2.9.0. The manufacturer must find a way to blank the screen instead of rely on this function call while an error code appears.
 *
 * Cloaked CA Agent will invoke this SPI immediately when the client is not entitled to view the content. 
 * Client device shall make descramblers stop working by setting random keys(both even and odd) to descramblers or by stopping descramblers.
 * 
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 *
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_CleanDescramblingKey(
    uc_stream_handle streamHandle);    

/**
 * Stream method to stop and cleanup a stream instance
 *
 * This method is called when the stream is being closed by UniversalClient_Stream_Close and is 
 * intended to give the stream implementation a chance to clean up any leftover resources set 
 * up during a call to open.
 *
 * @param[in,out] pStreamHandle On input, points to a stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 *      On output, the function must set this to #UC_INVALID_HANDLE_VALUE.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_Close(
    uc_stream_handle *pStreamHandle);

/**
 * Stream method to send data to the CCIS stream or another device for Proximity Detection.
 *
 * If the stream is for web service, this method is used to send data to a CCIS stream.
 * If the stream is for proximity detection service, this method is used to send data to another device.
 * 'pBytes' contained the information associated with the CCIS stream or device. 
 *
 *
 * \note This SPI is for PULL EMM or Proximity Detection. \n
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] pBytes Data structure that identifies the payload for the send message, also refer to ::uc_stream_send_payload.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_Send(
     uc_stream_handle streamHandle,
     const uc_stream_send_payload *pBytes);


/** @} */ /* End of streamspi */

/** @defgroup copycontrolspi Cloaked CA Agent Copy Control SPIs
 *  Cloaked CA Agent Copy Control SPIs
 *
 *  Cloaked CA Agent will output copy control information via these SPIs.
 *  To access copy control functionality, the application should also invoke \ref copycontrolapis "Cloaked CA Agent Copy Control APIs"
 *  to cooperate with these SPIs.
 *
 *  
 *  @{
 */

/**
 * Sets the macrovision configuration data
 *
 * This method is called during the initialization of Cloaked CA Agent, or when the Cloaked CA Agent received Macrovision CAM emm.
 *
 * @param[in] mac_config_data macrovsion configuration data structure. this contains MacrovisionVersion and 
    Macrovision configuration data.
 *
 * See ::uc_macrovision_config for the detail information needed.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result  UniversalClientSPI_CopyControl_Macrovision_SetConfig(uc_macrovision_config mac_config_data);


/**
 * CopyControl method to set the Macrovision mode
 *
 * This method is called when the received stream containes Macrovision mode info or submitted Session Metadata containes Macrovision mode.
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] mac_mode The mode of macrovision to be set
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result  UniversalClientSPI_CopyControl_Macrovision_SetMode(uc_stream_handle streamHandle,uc_macrovision_mode mac_mode);
/**
 * CopyControl and CI Plus method to set the CCI bits info [uc_copy_control_info] to client
 *
 * This method is called when the received ECM containes the CopyControl/CI Plus info or the submitted Session Metadata containes the CopyControl/CI Plus info.
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] pCopyControlInfo points to the structure of detail copy control information.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result  UniversalClientSPI_CopyControl_SetCCI(uc_stream_handle streamHandle, uc_copy_control_info* pCopyControlInfo);

/** @} */ /* End of copycontrolspi */

/** @defgroup smartcardspi Cloaked CA Agent Smart Card SPIs
 *  Cloaked CA Agent Smart Card SPIs
 *
 *  Cloaked CA Agent will communicate with Smart Card via these SPIs.
 *  \note Currently, if Cloaked CA Agent is configured without Unified Client support, the Smart Card SPIs are used by Smart Card slot verification only. 
 *  They are NOT used by Cloaked CA Agent. If Unified Client is used, these SPIs must be supported.
 *  Smart Card slot verification will use these SPIs to communicate with Smart Card, so that the Smart Card slot can be verified.
 *  
 *  @{
 */

/**
 * Smart Card method to initialize the smart card device.
 *
 * This method is called during the initialization of Cloaked CA Agent.
 *
 * @param[out] pSmartcardID identity for the smartcard device, Cloaked CA Agent uses the
 *            resource ID to map to a smartcard reader slot.  
 * @param[in] pSCOpenData pointer of data transmitted to smartcard driver, the pointer 
 *            points to structure ::uc_sc_open_parameter.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Smartcard_Open(uc_uint32 *pSmartcardID, uc_sc_open_parameter *pSCOpenData);


/**
 * Smart Card method to close the smartcard device.
 *
 * This method is called when closing the Cloaked CA Agent.
 *
 * @param[in] smartcardID identity for the smartcard device, Cloaked CA Agent uses the
 *            resource ID to map to a smartcard reader slot.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Smartcard_Close(uc_uint32 smartcardID);

/**
 * Smart Card method to reset the smartcard device.
 *
 * This method is called when Cloaked CA Agent wants to reset the smartcard device.
 *
 * @param[in] smartcardID identity for the smartcard device, Cloaked CA Agent uses the
 *            resource ID to map to a smartcard reader slot.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
 uc_result UniversalClientSPI_Smartcard_Reset(uc_uint32 smartcardID);

/**
 * Smart Card method to communicate with the smartcard device
 *
 * This method is called when Cloaked CA Agent wants to read/write data from/to the smartcard device
 *
 * @param[in] smartcardID identity for the smartcard device, Cloaked CA Agent uses this ID
 *            to map to a smartcard reader slot.
 * @param[in] headerLen header length of the APDU.
 * @param[in] payloadLen payload length of the APDU.
 * @param[in] pSendBuffer buffer for the  APDU.
 * @param[out] pRecvDataLen received buffer length.
 * @param[out] pRecvBuffer received buffer.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Smartcard_Communicate(uc_uint32 smartcardID, uc_uint32 headerLen, uc_uint32 payloadLen,uc_byte *pSendBuffer, uc_uint32 *pRecvDataLen, uc_byte *pRecvBuffer );

/** @} */ /* End of smartcardspi */

/** @defgroup datetimespi Cloaked CA Agent Date Time SPIs
 *  Cloaked CA Agent Date Time SPIs
 *
 *  Cloaked CA Agent will retrieve date time info via these SPIs.
 *  
 *  @{
 */


/**
 * Method to get current date time.
 *
 * This method is called to retrieve second and millisecond of current time info.
 * In Linux-like system, gettimeofday can be used to get the result (but the result needs to be converted to millisecond).
 * In MS Windows-like system, GetLocalTime and GetSystemTime can be used to get the result.
 *
 * \note Cloaked CA Agent uses the time info for features where timing control is needed. 
 * The time can be time elapsed since reboot, or it can also be the real time.
 *
 *    \n
 *
 * \note For PULL EMM, the time info is used to calculate the time of receiving Timestamp EMM. So the time must be time elapsed since reboot.
 *
 *    \n
 *
 * \note The time info must be with the precision of 1 millisecond.
 *
 * @param[out] pCurrentTime To save current time info.  
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_DateTime_GetTimeOfDay(uc_time *pCurrentTime);

/** @} */ /* End of datetimespi */

/** @defgroup manufacturercryptospi Cloaked CA Agent Crypto SPIs
 *  Cloaked CA Agent Crypto SPIs
 *
 *  Cloaked CA Agent will do the verification step for digital signature via these SPIs.
 *  
 *  @{
 */


/**
 * Method to do the verification for digital signature.
 *
 * This SPI is optional currently.
 *
 * These functional modules are implemented by both Cloaked CA Agent and STB manufacturer.
 * Generally speaking, the SPI form is faster than the Cloaked CA Agent's implementation.
 * Cloaked CA Agent will decide which implementation should be used in digital signature process.
 *
 * @param[in] pCryptoInfo send relative info indicating the deciphering algorithm type and the parameter related to it.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Crypto_Verify(uc_crypto_info *pCryptoInfo);

/** @} */ /* End of manufacturercryptospi */

/** @defgroup timerspi Cloaked CA Agent Timer SPIs
 *  Cloaked CA Agent Timer SPIs
 *
 *  Cloaked CA Agent will do timing via these SPIs.
 *  
 *  @{
 */

/**
 * Open a timer. 
 *
 * @param[out] pTimerId identity for the timer, Cloaked CA Agent uses this to map to a timer.  
 * @param[in] pTimerInfo pointer of data transmitted to timer driver, pTimerInfo 
 *            points to structure ::uc_timer_info.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Timer_Open(uc_uint32* pTimerId,uc_timer_info* pTimerInfo);

/**
 * Close the timerId
 *
 * @param[in] timerId identity for the timer, Cloaked CA Agent uses this to map to a timer.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Timer_Close(uc_uint32 timerId);

/**
 * Stops the timer.
 *
 * This method is called when Cloaked CA Agent wants to stop a timer.
 * Cloaked CA Agent may reuse the timer indicated by \b timerId later on.
 *
 * @param[in] timerId identity for the timer, Cloaked CA Agent uses this to map to a timer.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Timer_Stop(uc_uint32 timerId);


/**
 * Start the timer. 
 *
 * Cloaked CA Agent calls this to restart the timer which stopped is by ::UniversalClientSPI_Timer_Stop before.
 *
 * @param[in] timerId identity for the timer, Cloaked CA Agent uses this to map to a timer.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Timer_Start(uc_uint32 timerId);

/** @} */ /* End of timerspi */


/** @defgroup ifcpspi Cloaked CA Agent IFCP SPIs
 *  Cloaked CA Agent IFCP SPIs
 *
 *  Cloaked CA Agent will perform \ref glossary_ifcp "IFCP" related operations via these SPIs.
 *  
 *  @{
 */

/**
 * Send IFCP Command to the secure chipset.
 *
 * Cloaked CA Agent calls this to send an IFCP Command to the secure chipset and get response if any.
 * When a device is required to work in IFCP mode by the headend, the Cloaked CA Agent will call this SPI
 * to set the descrambling key, to set the secure PVR session key, etc.
 * This is the uniform IFCP command communication interface.
 *
 * \note This SPI provides IFCP command information to STB Application, and STB Application constructs the IFCP Command according to chip's specification. 
 *
 * \n
 * 
 * \note The IFCP command does not support clear descrambling key or clear PVR session key delivery.
 *       If the descrambling key is clear, it will be set via ::UniversalClientSPI_Stream_SetDescramblingKey.
 *       If the PVR session key is clear, it will be set via ::UniversalClientSPI_PVR_SetExtendedSessionKey.
 *
 * \n
 *
 * \note When this SPI is used to set the descrambling key or set the Secure PVR session key, it is a stream related SPI, refer to \ref streamspi "Cloaked CA Agent Stream SPIs".
 *    The stream handle is indicated in ::uc_IFCP_input::additionalInfo.
 *
 * \n
 *
 * \note If the ED_Ctrl in the ::uc_IFCP_input::uc_IFCP_info::uc_KLC_info is equal to 4, that means that CUR data is filled in the extraData buffer, 
 * that also means this stream needs an Secure Media Pipeline, if the pipeline currently is configured as a Non Secure Media Pipeline, 
 * the SPI implementation needs to return ::UC_ERROR_DRIVER_FAILURE.
 *
 * @param[in] pInput includes IFCP command data and some additional information.
 * @param[out] pOutput output data from IFCP module, refer to ::_uc_IFCP_output for more details.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval 
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval 
 *    ::UC_ERROR_DRIVER_FAILURE
  * @retval 
 *    ::UC_ERROR_IFCP_TDC_LOAD_FAILED
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_IFCP_Communicate(uc_IFCP_input *pInput, uc_IFCP_output *pOutput);

/**
 * Load IFCP image into the secure chipset.
 *
 * Cloaked CA Agent calls this to load an IFCP image into the secure chipset.
 *
 * There are several triggers to make the Cloaked CA Agent to call this SPI, as below,
 *
 *  - First, when a secure chipset is required to work in IFCP mode by the headend, which is triggered by an IFCP Pairing EMM, 
 * the Cloaked CA Agent will call this SPI to load the IFCP image (if it exists).
 *  - Second, if it has switched to IFCP mode, the Cloaked CA Agent will also call this SPI when the STB boots up.
 *  - Third, when processing SPE ECM, even Agent has not received the IFCP Pairing EMM (not initialized device), 
 * the Cloaked CA Agent will also read the IFCP image from PS and call this SPI to load IFCP image.
 *  - Fourth, when the Cloaked CA Agent downloads an IFCP image with a higher version than the current one on the device, 
 * or download an IFCP image to a device that does not have a valid IFCP image,  
 * and the Cloaked CA Agent already has received IFCP Pairing EMM before (the device works in IFCP mode), 
 * the Cloaked CA Agent will also read the latest IFCP image from PS and call this SPI to load IFCP image.
 *
 * @param[in] pImage image data, including activation message (AM) and RAM image.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_IFCP_LoadImage(uc_IFCP_image *pImage);

/** @} */ /* End of ifcpspi */


/** @defgroup trickmodecontrolspi Cloaked CA Agent Trick Mode Control SPIs
 *  Cloaked CA Agent Trick Mode Control SPIs
 *
 *  Cloaked CA Agent will do trick mode control via these SPIs.
 *  
 *  @{
 */

/**
 * Set extra trick mode control information.
 *
 * Cloaked CA Agent calls this to set extra trick mode control information on a client.
 *
 * This SPI is currently optional.
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] pExtraTrickModeControl points to the structure of extra trick mode control.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */

uc_result UniversalClientSPI_SetExtraTrickModeControl(uc_stream_handle streamHandle,uc_extra_trick_mode_control *pExtraTrickModeControl);

/** @} */ /* End of trickmodecontrolspi */

/** @defgroup hgpcspi Cloaked CA Agent Home Gateway Proximity Control SPIs
 *  Cloaked CA Agent Home Gateway Proximity Control (HGPC) SPIs
 *
 *  Cloaked CA Agent will do Home Gateway Proximity Control via these SPIs.
 *  
 *  @{
 */

/**
 * Send HNA Message
 *
 * Cloaked CA Agent calls this function to send HNA message to Middleware. The message size is ::UC_HGPC_MESSAGE_SIZE bytes.
 *
 * @param[in] pHNAMessage Buffer contains HNA message to be sent. Middleware need not free this message buffer.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_HGPC_SendHNAMessage(uc_byte* pHNAMessage);

/** @} */ /* End of hgpcspi */


/** @defgroup scotspi Cloaked CA Agent SCOT SPIs
 *  Cloaked CA Agent Transformation Data SPIs
 *
 *  Cloaked CA Agent will set transformation data and encrypted TASK via these SPIs.
 *  
 *  @{
 */

/**
 * Set the Transformation Data Containers (TDC) and Transformation Algorithm Session Key (TASK) to the secure chipset.
 *
 * Cloaked CA Agent calls this function to send the Transformation Data Containers and Transformation Algorithm Session Key  to the secure chipset.
 * When a device is required to work in transformation mode by the headend (in MSR mode), the Cloaked CA Agent will call this SPI
 * to set the Transformation Data Containers and Transformation Algorithm Session Key.
 *
 * \note This SPI provides Transformation Data Containers and Transformation Algorithm Session Key to secure chipset,
 * and secure chipset uses these data to inverse transform the descrambler keys.
 * \n
 * 
 * @param[in] pEncryptedTask Encrypted TASK to be provisioned into the device. This is normally protected by a Transformation Algorithm Unique Key (TAUK) that resides within the secure chipset such that an attacker gains no advantage by intercepting this method.
 * The uc_buffer_st::bytes member points to a buffer containing the raw key material, and the uc_buffer_st::length member contains the number of bytes in the buffer.
 * @param[in] tdcCount The size of the array of pTdcData.
 * @param[in] pTdcData Points to an array of ::uc_tdc_data_for_spi structure. And every member of the array contains a TDC that is encrypted by the TASK and the length of the encrypted TDC. The array capacity is indicated by "tdcCount".
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval 
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the result "Result Code List". 
 */
uc_result UniversalClientSPI_SCOT_LoadTransformationData(const uc_buffer_st *pEncryptedTask, uc_uint32 tdcCount, uc_tdc_data_for_spi *pTdcData);
/** @} */ /* End of scotspi */


/** @defgroup message_callback Cloaked CA Agent Message Callback SPI
 *  Cloaked CA Agent message_callback SPI
 *
 *  Cloaked CA Agent will register one global notification function to SPI layer to get the message from SPI layer.
 *  
 *  @{
 */
/**
 *
 * Cloaked CA Agent calls this function to register one global callback function to be called, when SPI layer need to notify the Cloaked CA Agent of various events.
 * Cloaked CA Agent provides the following events called by SPI layer: \n
 * ::UC_SMP_HDCP_STATUS_CHANGED : notify the Cloaked CA Agent that the HDCP status changes, as received from the HDCP subsystem of the SoC (system on the chip). 
 * When receiveing this event, the Cloaked CA Agent will loop all the SMP valid services to restart ECM page search and resubmit the CUR to SVPTA.
 * 
 * @param[in] messageCallback the point of global notification function
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval 
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the result "Result Code List". 
 */

uc_result UniversalClientSPI_Message_CallBack(uc_message_callback messageCallback);
/** @} */ /* End of message_callback */



/** @} */ /* End of Cloaked CA Agent SPIs*/
extern	void GL_DESCR_Init(void);
extern uc_stream_handle Current_StreamHandle;


#ifdef __cplusplus
}
#endif

#endif /* !UNIVERSALCLIENT_SPI_H__INCLUDED__ */


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
 * @file UniversalClient_API.h
 * Cloaked CA Agent Base API. This file contains the common informational methods necessary 
 * for any integration with the Cloaked CA Agent.
 */
#ifndef UNIVERSALCLIENT_API_H__INCLUDED__
#define UNIVERSALCLIENT_API_H__INCLUDED__

#include "UniversalClient_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
/**
 * Simple NULL typedef. 
 */
#define NULL 0
#endif

/**
 * Generic size for ::uc_buffer_st used in APIs.
 *
 * If no special comments, the application can allocate memory with ::UC_BUFFER_GENERIC_SIZE bytes long
 * for uc_buffer_st::bytes used in some API.
 * For example, the application can allocate memory with ::UC_BUFFER_GENERIC_SIZE bytes for uc_buffer_st::bytes used in ::UniversalClient_GetVersion
 * and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE.
 */
#define UC_BUFFER_GENERIC_SIZE 384

/**
 * Generic size for ::uc_buffer_st used in APIs.
 *
 * ::UC_BYTES_GENERIC_SIZE is kept here in order to keep the compatibility.
 */

#define UC_BYTES_GENERIC_SIZE UC_BUFFER_GENERIC_SIZE

/**
 * Size for product id.
 *
 * Product id is used to uniquely identify a product.
 */
#define UC_SIZE_PRODUCT_ID 2

/**
 * Max length of smartcard serial number buffer.
 */
#define UC_MAX_SMARTCARD_SERIAL_NUMBER_LENGTH 20

/**
 * Size of status string
 */
#define UC_STATUS_LENGTH 9

/**
 * Maximum length of version string
 */
#define UC_MAX_VERSION_STRING_LENGTH        128

/**
 * maximu operator count
 * The maximum operator count supported.
 */
#define UC_MAX_OPERATOR_COUNT               8

/**
 * maximum operator name length
 */
#define UC_MAX_OPERATOR_NAME_LENGTH         20

/**
 * size of unique address
 */
#define UC_UNIQUE_ADDRESS_SIZE              4

/**
 * maximum length of information string
 */
#define UC_MAX_ITEM_LENGH                   100

/**
 * size of nationality
 */
#define UC_NATIONALITY_SIZE                 3

/**
 * size of homing channel data
 */
#define UC_HOMING_CHANNEL_USER_DATA_SIZE    32

/**
 * size of middleware user data
 */
#define UC_MW_DATA_USER_DATA_SIZE           128

/**
 * size of homing channel data
 */
#define UC_EMM_HOMING_CHANNEL_USER_DATA_SIZE    128

/**
 * maximum sectors supported by each operator
 */
#define MAX_ACTIVE_SECTOR_COUNT_PER_OPERATOR   8

/**
 * maximum sectors count of each smartcard
 */
#define UC_MAX_SECTOR_COUNT_PER_SMARTCARD  8

/**
 * Size of sector address
 */
#define UC_SECTOR_ADDRESS_LENGTH 3

/**
 * Size of user data
 */
#define UC_TMS_USER_DATA_SIZE 32

/**
 * Maximum size of Enhanced Copy Protection
 */
#define UC_MAX_SERVICE_DATA_FOR_ENHANCED_COPY_PROTECTION     32

/**
 * Maximum transformation TDC size
 */
#define UC_MAX_TRANSFORMATION_TDC_SIZE            416

/** @defgroup commonapi Cloaked CA Agent Common APIs
 * APIs related to service management and client management functionality
 *
 * These API methods provide the necessary functionality for managing ECM and EMM services.
 * 
 *  @{
 */
/**
 * IFCP Error Code Response for SMP
 */
#define UC_SIZE_ERRORCODE_FOR_SMP    2

/** @} */ /* End of groups */


/** @defgroup hgpcapi Cloaked CA Agent Home Gateway Proximity Control APIs
 *  Cloaked CA Agent Home Gateway Proximity Control APIs
 *
 *  @{
 */

/**
 * Maximum number of secondary clients for HGPC network
 */
#define UC_MAX_HGPC_SECONDARY      4
/**
 * Maximum size for serial number in HGPC Primary & Secondary Activation EMM opcodes
 */
#define UC_SERIAL_NUMBER_SIZE   5
/**
 * HGPC HNA message size
 */
#define UC_HGPC_MESSAGE_SIZE    18

/** @} */ /* End of groups */

/** @defgroup result Result Code List
 *  List of all possible values of ::uc_result codes. 
 *
 *  Except where noted, applications and implementations should 
 *  not expect particular failure codes to always be returned from functions, other than ::UC_ERROR_SUCCESS.
 *  This is because over time and across different implementations the error codes returned from different error situations
 *  may change.
 *  @{
 */

/**
 * Result code enumerator type. 
 */
typedef enum {
    /** (0x00000000) The operation was successful. */
    UC_ERROR_SUCCESS                        = 0x00000000,

    /** (0x00000001) One or more required parameters were NULL. */
    UC_ERROR_NULL_PARAM                     = 0x00000001, 

    /** (0x00000002) Memory could not be allocated. */
    UC_ERROR_OUT_OF_MEMORY                  = 0x00000002, 

    /** (0x00000003) Unable to create requested resource. */
    UC_ERROR_UNABLE_TO_CREATE               = 0x00000003,

    /** (0x00000004) Generic OS failure. */
    UC_ERROR_OS_FAILURE                     = 0x00000004,

    /** (0x00000005) The timeout expired before the object was ready. */
    UC_ERROR_WAIT_ABORTED                   = 0x00000005, 

    /** (0x00000006) The buffer passed in is too small. */
    UC_ERROR_INSUFFICIENT_BUFFER            = 0x00000006, 

    /** (0x00000007) The specified resource could not be found. */
    UC_ERROR_RESOURCE_NOT_FOUND             = 0x00000007,

    /** (0x00000008) The resource name specified is invalid. */
    UC_ERROR_BAD_RESOURCE_NAME              = 0x00000008, 

    /** (0x00000009) The requested feature is not implemented. */
    UC_ERROR_NOT_IMPLEMENTED                = 0x00000009, 

    /** (0x0000000A) A connection is still open; stream cannot be closed. */
    UC_ERROR_CONNECTION_STILL_OPEN          = 0x0000000A, 

    /** (0x0000000B) The handle is not valid. */
    UC_ERROR_INVALID_HANDLE                 = 0x0000000B,

    /** (0x0000000C) The handle is valid but is not the correct type. */
    UC_ERROR_WRONG_HANDLE_TYPE              = 0x0000000C, 

    /** (0x0000000D) Too many handles are opened already. */
    UC_ERROR_TOO_MANY_HANDLES               = 0x0000000D,

    /** (0x0000000E) An attempt to shut down was made while a handle is still open. */
    UC_ERROR_HANDLE_STILL_OPEN              = 0x0000000E,

    /** (0x0000000F) This operation cannot be completed because the Cloaked CA Agent was already started. */
    UC_ERROR_ALREADY_STARTED                = 0x0000000F,

    /** (0x00000010) This operation cannot be completed because the Cloaked CA Agent was already stopped. */
    UC_ERROR_ALREADY_STOPPED                = 0x00000010,

    /** (0x00000011) An internal message queue is full and cannot accept more messages. */
    UC_ERROR_QUEUE_FULL                     = 0x00000011,

    /** (0x00000012) A required implementation of an SPI method is missing. */
    UC_ERROR_MISSING_SPI_METHOD             = 0x00000012,

    /** (0x00000013) The version of the SPI implementation is not compatible with the client. */
    UC_ERROR_INCOMPATIBLE_SPI_VERSION       = 0x00000013,

    /** (0x00000014) An invalid message was received. */
    UC_ERROR_MESSAGE_INVALID                = 0x00000014,

    /** (0x00000015) The specified length was not valid. */
    UC_ERROR_INVALID_LENGTH                 = 0x00000015,

    /** (0x00000016) Some required internal data is missing. */
    UC_ERROR_MISSING_DATA                   = 0x00000016,

    /** (0x00000017) Page is not what we want. */
    UC_ERROR_NOT_WANTED_ECMPAGE             = 0x00000017,

    /** (0x00000018) Generic driver failure or mismatched pipe selection. On the Cloaked CA Agent 4.10.1, this error will be returned by ::UniversalClientSPI_IFCP_Communicate or ::UniversalClientSPI_Stream_SetDescramblingKey when setting key on unexpected pipe*/
    UC_ERROR_DRIVER_FAILURE                 = 0x00000018,

    /** (0x00000019) Not entitled. */
    UC_ERROR_NOT_ENTITLED                   = 0x00000019,

    /** (0x0000001A) Invalid PK. */
    UC_ERROR_INVALID_PK                     = 0x0000001A,

    /** (0x0000001B) Invalid GK. */
    UC_ERROR_INVALID_GK                     = 0x0000001B,

    /** (0x0000001C) Invalid TG. */
    UC_ERROR_INVALID_TG                     = 0x0000001C,

    /** (0x0000001D) Missing CWDK. */
    UC_ERROR_INVALID_CWDK                   = 0x0000001D,

    /** (0x0000001E) No CA regional info. */
    UC_ERROR_NO_CA_REGIONAL_INFO            = 0x0000001E,

    /** (0x0000001F) Regional blocking. */
    UC_ERROR_REGIONAL_BLOCKING              = 0x0000001F,

    /** (0x00000020) Restricted opcode. */
    UC_ERROR_RESTRICTED_OPCODE              = 0x00000020,

    /** (0x00000021) Timestamp filter failed. */
    UC_ERROR_TIMESTAMP_FILTER               = 0x00000021,

    /** (0x00000022) Type filter failed. */
    UC_ERROR_TYPE_FILTER                    = 0x00000022,

    /** (0x00000023) Signature verification failed. */
    UC_ERROR_SIGNATURE_VERIFICATION         = 0x00000023,

    /** (0x00000024) TMS failed. */
    UC_ERROR_TMS_FAILED                     = 0x00000024,

    /** (0x00000025) Not PVR entitled. */
    UC_ERROR_NOT_PVR_ENTITLED               = 0x00000025,

    /** (0x00000026) This operation cannot be completed because PMT is not notified. */
    UC_ERROR_UNABLE_TO_CONFIG_PVR_RECORD    = 0x00000026,

    /** (0x00000027) This operation cannot be completed while a request for PVR record  is still being processed. */
    UC_ERROR_CONFIG_PVR_RECORD_STILL_OPEN   = 0x00000027,

    /** (0x00000028) This operation cannot be completed because PVR MSK is missing. */
    UC_ERROR_MISSING_PVR_MSK                = 0x00000028,

    /** (0x00000029) The recorded content expired */
    UC_ERROR_PVR_CONTENT_EXPIRED            = 0x00000029,

    /** (0x0000002A) Failed to generate PVR session key */
    UC_ERROR_FAILED_TO_GENERATE_SESSION_KEY = 0x0000002A,

    /** (0x0000002B) The PVR metadata is invalid */
    UC_ERROR_INVALID_PVR_METADATA          = 0x0000002B,

    /** (0x0000002C) The client type is wrong, a feature only supports a certain client type
    (For example, PVR works only on clients with secure chipset). 
    */
    UC_ERROR_CLIENT_TYPE_ERROR              = 0x0000002C,   

    /** (0x0000002D) Invalid sector. */
    UC_ERROR_INVALID_SECTOR                 = 0x0000002D,
    
    /** (0x0000002E) Client type error, PPV VOD feature should base on Secure Chipset mode. */
    UC_ERROR_VOD_CLIENT_TYPE_ERROR          = 0x0000002E,
    
    /** (0x0000002F) No serial number. */
    UC_ERROR_VOD_NO_SERIAL_NUMBER           = 0x0000002F,
    
    /** (0x00000030) Invalid nonce. */
    UC_ERROR_VOD_INVALID_NONCE              = 0x00000030,

    /** (0x00000031) Pkey Hash mismatch */
    UC_ERROR_PKEY_HASH_MISMATCH             = 0x00000031,

    /** (0x00000032) The variant of the device identifiers is not compatible with the client. */
    UC_ERROR_INCOMPATIBLE_VARIANT           = 0x00000032,

    /** (0x00000033) No Nationality. */
    UC_ERROR_NO_NATIONALITY                 = 0x00000033,

    /** (0x00000034) The uniqueaddress is invalid with the client. */
    UC_ERROR_INVALID_UNIQUE_ADDRESS         = 0x00000034,

    /** (0x00000035) This result code is not used now. */
    UC_ERROR_DIGITAL_COPY_NOMORE            = 0x00000035,
    
    /** (0x00000036) Black out. */
    UC_ERROR_BLACK_OUT                      = 0x00000036,

    /** (0x00000037) Homing Channel Failed. */
    UC_ERROR_HOMING_CHANNEL_FAILED          = 0x00000037,

    /** (0x00000038) FlexiFlash Failed. */
    UC_ERROR_FLEXIFLASH_FAILED          = 0x00000038,

    /** (0x00000039) Middleware User Data Failed */
    UC_ERROR_MIDDLEWARE_USER_DATA_FAILED      = 0x00000039,

    /** (0x0000003A) Pre-Enable product expired */
    UC_ERROR_PRE_ENABLE_PRODUCT_EXPIRED     = 0x0000003A,

    /** (0x0000003B) Missing Pre-Enable Session Key */
    UC_ERROR_MISSING_PESK           = 0x0000003B,

    /** (0x0000003C) The OTP data is invalid */
    UC_ERROR_INVALID_OTP_DATA           = 0x0000003C,

    /** (0x0000003D) The personalized data is invalid */
    UC_ERROR_INVALID_PERSONALIZED_DATA           = 0x0000003D,
    
    /** (0x0000003E) New CG for FSU product received */
    UC_ERROR_NEW_CG_FOR_FSU_RECEIVED           = 0x0000003E,

    /** (0x0000003F) New CG for Push VOD product received */
    UC_ERROR_NEW_CG_FOR_PUSHVOD_RECEIVED           = 0x0000003F,

    /** (0x00000040) CG Mismatch */
    UC_ERROR_CG_MISMATCH           = 0x00000040,

    /** (0x00000041) SG Mismatch */
    UC_ERROR_SG_MISMATCH           = 0x00000041,

    /** (0x00000042) Invalid PK Index */
    UC_ERROR_INVALID_PK_INDEX           = 0x00000042,

    /** (0x00000043) Invalid GK Index */
    UC_ERROR_INVALID_GK_INDEX           = 0x00000043,
    
    /** (0x00000044) Macrovision Failed. */
    UC_ERROR_MACROVISION_FAILED           = 0x00000044,

    /** (0x00000045) SN RANGE Failed. */
    UC_ERROR_SN_RANGE_FAILED           = 0x00000045,

    /** (0x00000046) Unified Client Error. */
    UC_ERROR_CONVERGENT_CLIENT_GENERIC_ERROR      = 0x00000046,

    /** (0x00000047) Smart Card is out. */
    UC_ERROR_SMARTCARD_OUT      = 0x00000047,

    /** (0x00000048) Unknown Card (non-Irdeto), or the smart card is upside down. */
    UC_ERROR_SMARTCARD_UNKNOWN    = 0x00000048,

    /** (0x00000049) Card Error, communications with Smart Card have failed. */
    UC_ERROR_SMARTCARD_ERROR      = 0x00000049,

    /** (0x0000004A) Corrupted Data, the data is corrupted */
    UC_ERROR_IO_DATA_CORRUPTED      = 0x0000004A,    

    /** (0x0000004B) Invalid change verion. */
    UC_ERROR_INVALID_CHANGE_VERSION      = 0x0000004B,

    /** (0x0000004C) This Block has been downloaded. */
    UC_ERROR_FLEXIFLASH_BLOCK_DOWNLOAD_DUPLICATION      = 0x0000004C,

    /** (0x0000004D) The Global SC EMM has been processed by Client. */
    UC_ERROR_GLOBAL_SC_EMM_DUPLICATION      = 0x0000004D,

    /** (0x0000004E) Stop EMM processing. */
    UC_ERROR_STOP_EMM_PROCESSING      = 0x0000004E,
    
    /** (0x0000004F) Load Cloaked CA package failed. */
    UC_ERROR_LOAD_CCAPACKAGE_FAILED      = 0x0000004F,

    /** (0x00000050) Invalid CFG data. */
    UC_ERROR_INVALID_CFG_DATA      = 0x00000050, 

    /** (0x00000051) Package data is invalid. */
    UC_ERROR_INVALID_PACKAGE_DATA      = 0x00000051,

    /** (0x00000052) The VM failed. */
    UC_ERROR_VM_FAILURE      = 0x00000052,  
    
    /** (0x00000053) The securecore is not loaded. */
    UC_ERROR_SECURECORE_NOT_LOADED      = 0x00000053,

    /** (0x00000054) Invalid area index. */
    UC_ERROR_INVALID_AREA_INDEX      = 0x00000054,

    /** (0x00000055) An character is not expected when parse a CFG file */
    UC_ERROR_UNEXPECTED_CHARACTER = 0x00000055,

    /** (0x00000056) Get section Data without parse a CFG file. */
    UC_ERROR_NO_CFG_PARSED = 0x00000056,

    /** (0x00000057) Parsing a CFG file before release the former parsing result. */
    UC_ERROR_CFG_PARSED_ALREADY = 0x00000057,

    /** (0x00000058) No specified section found in the CFG file. */
    UC_ERROR_SECTION_NOT_FOUND = 0x00000058,

    /** (0x00000059) The VM with special id has already defined. */
    UC_ERROR_VM_ALREADY_DEFINED = 0x00000059,

    /** (0x0000005A) Error happen during create the "vm config" object. */
    UC_ERROR_VM_CREATE_VMCONFIG = 0x0000005A,

    /** (0x0000005B) Error happen during create the vm instance. */
    UC_ERROR_VM_CREATE = 0x0000005B,

    /** (0x0000005C) Too many vm created, vm number reach top limit. */
    UC_ERROR_VM_TOP_LIMIT =  0x0000005C,

    /** (0x0000005D) Error ocurr during loading bytecode image to vm. */
    UC_ERROR_VM_LOAD_IMAGE = 0x0000005D,

    /** (0x0000005E) Error ocurr during memory map from native memory space to vm memory space. */
    UC_ERROR_VM_MEMMAP = 0x0000005E,

    /** (0x0000005F) Error ocurr during execute vm. */
    UC_ERROR_VM_EXECUTE = 0x0000005F,

    /** (0x00000060) Error ocurr VM IO. */
    UC_ERROR_VM_IO = 0x00000060,

    /** (0x00000061) Error ocurr VM RESET. */
    UC_ERROR_VM_RESET = 0x00000061,

    /** (0x00000062) The root key hash check failed. */
    UC_ERROR_ROOT_KEY_HASH_CHECK_FAILED = 0x00000062,

    /** (0x00000063) Unsupported package compress algorithm. */
    UC_ERROR_COMPRESS_ALGORITHM_NOT_SUPPORT = 0x00000063,
    
    /** (0x00000064) The SYS ID is invalid. */
    UC_ERROR_INVALID_SYS_ID = 0x00000064,

    /** (0x00000065) The version of the client is too low to support Cloaked CA package download. */
    UC_ERROR_LOW_CLIENT_VERSION = 0x00000065,

    /** (0x00000066) The CA System ID is invalid */
    UC_ERROR_INVALID_CA_SYSTEM_ID = 0x00000066,

    /** (0x00000067) Anchor Failed(illegal device). */
    UC_ERROR_DEVICE_INVALID                 = 0x00000067,

    /** (0x00000068) Request entitlementkeys failed. */
    UC_ERROR_REQUEST_ENTITLEMENT_FAILED     = 0x00000068,

    /** (0x00000069) No response for request for a certain time interval. */
    UC_ERROR_REQUEST_ENTITLEMENT_RESPONSE_TIME_OUT   = 0x00000069,

    /** (0x0000006A) CCIS internal error. */
    UC_ERROR_CCIS_INTERNAL_ERROR            = 0x0000006A,

    /** (0x0000006B) Anchor failed (DeviceID/UA mismatch: for IP only STBs, the application should register again). */
    UC_ERROR_DEVICEID_UA_MISMATCH           = 0x0000006B,

    /** (0x0000006C) VOD not entitled. */
    UC_ERROR_VOD_NOT_ENTITLED               = 0x0000006C,

    /** (0x0000006D) Missing Secure PVR CPSK. */
    UC_ERROR_MISSING_CPSK               = 0x0000006D,

    /** (0x0000006E) Asset ID mismatch. */
    UC_ERROR_ASSET_ID_MISMATCH = 0x0000006E,

    /** (x00000006F) PVR Recording is prohibited due to copy control setting */
    UC_ERROR_PVR_COPY_CONTROL_PROHIBITED = 0x0000006F,


    /** (x000000070) PVR sharing is prohibited due to is shareable setting */
    UC_ERROR_HN_PVR_NOT_SHAREABLE = 0x00000070,

    /** (x000000071) PVR sharing is prohibited due to domain id mismatch */
    UC_ERROR_HN_PVR_NOT_IN_DOMAIN = 0x00000071,

    /** (0x00000072) Fail to process Shared PVRMSK EMM. */
    UC_ERROR_SHARED_PVRMSK_PROCESS_FAIL = 0x00000072,

    /** (0x00000073) White Box Algorithm is invalid. */
    UC_ERROR_INVALID_WB_ALGORITHM = 0x00000073,

    /** (0x00000074) White Box Algorithm is invalid for Pairing EMM. */
    UC_ERROR_INVALID_WB_ALGORITHM_FOR_PAIRING_EMM = 0x00000074,

    /** (0x00000075) White Box Algorithm is invalid for Product Overwrite EMM. */
    UC_ERROR_INVALID_WB_ALGORITHM_FOR_PO_EMM = 0x00000075,

    /** (0x00000076) White Box Algorithm is invalid for Advertisement EMM. */
    UC_ERROR_INVALID_WB_ALGORITHM_FOR_AD_EMM = 0x00000076,

    /** (0x00000077) The chip configuration check is not supported. */
    UC_ERROR_CCC_NOT_SUPPORT = 0x00000077,

    /** (0x00000078) Client registry Failed. */
    UC_ERROR_INITIALIZATION_FAILED = 0x00000078,

    /** (0x00000079) no response for request for a certain time interval. */
    UC_ERROR_CLIENT_REGISTRATION_RESPONSE_TIME_OUT = 0x00000079,

    /** (0x0000007A) PIN CODE Limitation Failed. */
    UC_ERROR_PIN_CODE_LIMIT_FAILED = 0x0000007A,

    /** (0x0000007B) Cannot find the current CG's secret private data. */
    UC_ERROR_CG_PRIVATE_DATA_NOT_PAIRING = 0x0000007B,

    /** (0x0000007C) Proximity Detection is disabled by the Head-end. */
    UC_ERROR_PROXIMITY_DETECTION_DISABLED = 0x0000007C,

    /** (0x0000007D) The data provided to be encrypted/decrypted is invalid. */
    UC_ERROR_PROXIMITY_INVALID_DATA = 0x0000007D,

    /** (0x0000007E) The peer ID does not represent a valid peer or session.  */
    UC_ERROR_PROXIMITY_PEER_UNKNOWN = 0x0000007E,

    /** (0x0000007F) The key being used to encrypt/decrypt data is not ready.  */
    UC_ERROR_PROXIMITY_KEY_NOT_READY = 0x0000007F,

    /** (0x00000080) The proximity service is not configured yet.  */
    UC_ERROR_PROXIMITY_SERVICE_NOT_CONFIGURED = 0x00000080,

    /** (0x00000081) The PVR key ladder level in Pairing EMM does not match with that in secure chipset. */
    UC_ERROR_PVR_KEY_LADDER_LEVEL_NOT_MATCH = 0x00000081,

    /** (0x00000082) Pairing EMM is rejected as the secure chipset does not support IFCP mode. */
    UC_ERROR_PAIRING_EMM_REJECTED_IFCP_NOT_SUPPORTED = 0x00000082,

    /** (0x00000083) The IFCP RAM image is not loaded to the secure chipset successfully. */
    UC_ERROR_IFCP_IMAGE_NOT_LOADED      = 0x00000083,

    /** (0x00000084) SKE failed (This is not used on Cloaked CA Agent 4.2.0). */
    UC_ERROR_SKE_FAILED = 0x00000084,

    /** (0x00000085) The Secure Pre-enablement feature is not supported in IFCP. */
    UC_ERROR_PE_NOT_SUPPORTED      = 0x00000085,

    /** (0x00000086) The arbitrary value does not match feature bitmap set.*/
    UC_ERROR_IFCP_INVALID_ARBITRARY_LENGTH      = 0x00000086,

    /** (0x00000087) The arbitrary value check failed in IFCP.*/
    UC_ERROR_IFCP_AUTHENTICATION_FAILED      = 0x00000087, 

    /** (0x00000088) ECM is rejected as the secure chipset does not support IFCP mode. */
    UC_ERROR_ECM_REJECTED_IFCP_NOT_SUPPORTED = 0x00000088,

    /** (0x00000089) Unknown App Response from IFCP. */
    UC_ERROR_IFCP_UNKNOWN_APP_RESPONSE = 0x00000089,
    
    /** (0x0000008A) Fail when compute HASH value. */
    UC_ERROR_HASH_COMPUTE = 0x0000008A,
    
    /** (0x0000008B) Signal Announcement EMM is rejected as the FSU stream has been received successfully. */
    UC_ERROR_SIGNAL_ANNOUNCEMENT_EMM_NOT_NEEDED = 0x0000008B,

    /** (0x0000008C) Field Trial ECM is rejected as FSU stream receiving failed. */
    UC_ERROR_ECM_REJECTED_FSU_STREAM_FAILED = 0x0000008C,

    /** (0x0000008D) FSU stream was not successfully received. */
    UC_ERROR_FSU_STREAM_FAILED = 0x0000008D,

    /** (0x0000008E) The FSU stream CA System ID was not found in CAT. */
    UC_ERROR_MISSING_FSU_STREAM_PID_IN_CAT = 0x0000008E,

    /** (0x0000008F) The CAT was not notified to Cloaked CA Agent for FSU. */
    UC_ERROR_NO_CAT_NOTIFIED_FOR_FSU = 0x0000008F,

    /** (0x00000090) Not enough heap memory to load the new secure core package during FSU. */
    UC_ERROR_NO_ENOUGH_MEMORY_TO_LOAD_NEW_SECURE_CORE_DURING_FSU = 0x00000090,

    /** (0x00000091) The operation cannot be cancelled by user. */
    UC_ERROR_OPERATION_CANNOT_BE_CANCELED = 0x00000091,    

    /** (0x00000092) This operation cannot be completed because PVR MSK and CPSK are missing. */
    UC_ERROR_MISSING_PVR_MSK_CPSK = 0x00000092,

    /** (0x00000093) Invalid HGPC timestamp */
    UC_ERROR_HGPC_INVALID_TIMESTAMP = 0x00000093,

    /** (0x00000094) Invalid HGPC secure client */
    UC_ERROR_HGPC_INVALID_CLIENT = 0x00000094,

    /** (0x00000095) HGPC HNA message timeout */
    UC_ERROR_HGPC_HNA_MSG_TIMEOUT = 0x00000095,

    /** (0x00000096) TT not support in MSR mode */
    UC_ERROR_TT_NOT_SUPPORT_FOR_MSR = 0x00000096,

    /** (0x00000097) TT application data version mismatch */
    UC_ERROR_TT_APP_DATA_VERSION_MISMATCH = 0x00000097,

    /** (0x00000098) Traitor Tracing feature is not supported in IFCP */
    UC_ERROR_TT_NOT_SUPPORT_IN_IFCP = 0x00000098,

    /** (0x00000099) Incorrect number of CWs in the Traitor Tracing application data */
    UC_ERROR_TT_INCORRECT_CW_NUMBER = 0x00000099,

    /** (0x0000009A) Transformation IFCP TDC load failed */
    UC_ERROR_IFCP_TDC_LOAD_FAILED = 0x0000009A,

    /** (0x0000009B) Transformation IFCP TDC load unfinished */
    UC_ERROR_IFCP_TDC_LOAD_UNFINISHED = 0x0000009B,

    /** (0x0000009C) The secure core does not match the transformation mode */
    UC_ERROR_SCOT_SECURECORE_NOT_MATCHED = 0x0000009C,

    /** (0x0000009D) Either the TDC SPI was not implemented or an error occurred when Cloaked CA loaded TDC via this SPI */
    UC_ERROR_SCOT_TDC_NOT_LOAD = 0x0000009D,

    /** (0x0000009E)  the AD mode in the pairing EMM mismatchs what in the ECM */
    UC_ERROR_IFCP_AD_MODE_MISMATCH = 0x0000009E,

    /** (0x000000A0) General IFCP Auth Ctrl Error, means there is an unexpected Auth Ctrl value received */
    UC_ERROR_IFCP_AUTH_CTRL_ERR = 0x000000A0,

    /** (0x000000A1) SMP General CUR Failure from IFCP, may include Display and Record Errors */
    UC_ERROR_SMP_CUR_NOT_MEET = 0x000000A1,

    /** (0x000000A2) There is a Gereral App Response issue in the IFCP , can be about CUR mismatch or Auth Ctrl check */
    UC_ERROR_SMP_APP_RESPONSE_FAILURE = 0x000000A2,

    /** (0x000000A3) SMP not support in MSR mode */
    UC_ERROR_SMP_NOT_SUPPORTED_FOR_MSR = 0x000000A3

} uc_result;

/** @} */ /* End of results */


/** @defgroup smp_error_code SMP Error Code List
 *  List of all possible values of ::uc_smp_error_code codes. 
 *
 *  @{
 */
/**
 * SMP Error Code enumerator type. 
 */
typedef enum {
    /** (0x0401) The SMP feature is not supported */
    UC_ERROR_SMP_NOT_SUPPORTED = 0x0401,
    
    /** (0x0402) A general IFCP Auth Ctrl Error, means there is an unexpected Auth Ctrl value received */
    UC_ERROR_SMP_AUTH_CTRL_ERROR = 0x0402,
    
    /** (0x0403) The AD CUR and ED CUR do not match during the Application Data verification. */
    UC_ERROR_SMP_CUR_CHECK_ERROR = 0x0403,
    
    /** (0x0603) The recording is not allowed. */
    UC_ERROR_SMP_RECORDING_NOTALLOWED = 0x0603,
    
    /** (0x0604) The intention data is not consistent with the session used. */
    UC_ERROR_SMP_WRONG_INTENT = 0x0604,

    /** (0x0605) The parameters are wrong. */
    UC_ERROR_SMP_WRONG_PARAMETERS = 0x0605,

    /** (0x0606) The CUR dose not match between recording key command and control word command. */
    UC_ERROR_SMP_CUR_MISMATCH = 0x0606,

    /** (0x0607) The CUR enforces HDCP 2.2, but the connectivity between the TV and STB dose not support it or the HDMI connectivity is lost. */
    UC_ERROR_SMP_DISPLAY_NEED_HDCP2_2 = 0x0607,

    /** (0x0608) The CUR enforces HDCP 1.4, but the connectivity between the TV and STB dose not support it or the HDMI connectivity is lost. */
    UC_ERROR_SMP_DISPLAY_NEED_HDCP1_4 = 0x0608

    /** (others) There are some unknown errors happened. */

}uc_smp_error_code;

/** @} */ /* End of smp_error_code */


/** @defgroup status_messages Status Messages
 *  List of all possible status message strings. These status message strings can be used
 *  for technical support purposes or for debugging problems with a client integration.
 *  \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 *  The Application can ignore Ixxx-0 in the error banner.     
 *  @{
 */

/**
 * Length of connection status string
 *
 * It is recommended to be not used anymore. 
 */
#define UC_CONNECTION_STATUS_STRING_LENGTH (UC_STATUS_LENGTH-1)

/**
 * Size of connection status string
 *
 * Used by ::UniversalClient_GetServiceStatus.
 */
#define UC_CONNECTION_STATUS_STRING_SIZE (UC_STATUS_LENGTH)

/**
 * Service Status Screen Text: Service is currently descrambled.
 *
 * Being Descrambled. This message is sent  when an ECM has successfully been processed by the client,
 * and a descrambling key has already been returned to the stream implementation via ::UniversalClientSPI_Stream_SetDescramblingKey.
 */
#define ERR_MSG_D029        "D029-0  "

/**
 * Service Status Screen Text: EMM service OK or FTA service.
 *
 * EMM Accepted or FTA Service. For EMM service, this message means that an EMM has successfully been processed by the client.
 * For ECM service, this message means that current service is a clear service, i.e, no CA information is found in PMT.
 */
#define ERR_MSG_D100        "D100-0  "

/**
 * Service Status Screen Text: No matching CA System ID in CAT.
 *
 * No Matched CA System ID. This message is sent when no matched CA System ID found in CAT.
 */
#define ERR_MSG_D101        "D101-0  "

/**
 * Service Status Screen Text: PVR Record Request OK.
 *
 * PVR Record Request Accepted. This message is sent when a PVR record request has successfully been processed by the client. This also means that 
 * PVR Session Key has already been returned to the SPI via ::UniversalClientSPI_PVR_SetSessionKey or ::UniversalClientSPI_PVR_SetExtendedSessionKey and PVR Session Metadata has already been notified
 * to the application via message uc_service_message_type::UC_SERVICE_PVR_SESSION_METADATA_REPLY.
 */
#define ERR_MSG_D126        "D126-0  "

/**
 * Service Status Screen Text: PVR Playback Request OK.
 *
 * PVR Playback Request Accepted. This message is sent when a PVR playback request has successfully been processed by the client. This also means that 
 * PVR Session Key has already been returned to the SPI implementation 
 * via ::UniversalClientSPI_PVR_SetSessionKey or ::UniversalClientSPI_PVR_SetExtendedSessionKey.
 */
#define ERR_MSG_D127        "D127-0  "

/**
 * Service Status Screen Text: Cloaked CA package successfully loaded.
 *
 * This message is sent when the Cloaked CA package has been loaded successfully.
 */
#define ERR_MSG_D500         "D500-0  "

/**
 * Service Status Screen Text: IFCP image successfully loaded to the secure chipset.
 *
 * This message is sent when the IFCP image is loaded into the secure chipset successfully.
 */
#define ERR_MSG_D550         "D550-0  "

/**
 * Service Status Screen Text: IFCP image successfully read from PS but not loaded to the secure chipset.
 *
 * This message is sent when the IFCP image is read from PS successfully,
 * but the image has not been loaded into the secure chipset yet, as IFCP mode is not activated. 
 */
#define ERR_MSG_D551         "D551-0  "

/**
 * Service Status Screen Text: Incomplete Definition.
 *
 * CAT or PMT Absent. This message is sent when the service has been opened but no PMT or CAT is defined..
 * I101-0 is the default service status once the service has been opened.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner.     
 */
#define ERR_MSG_I101        "I101-0  "

/**
 * Service Status Screen Text: No ECM/EMM data received.
 *
 * No ECM/EMM Received.
 *
 * For EMM service, this message is sent when the service has been opened and CAT has been defined,
 * but NO EMM has been received.
 *
 * For ECM service, this message is sent when the service has been opened and PMT has been defined,
 * but NO ECM has been received.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner.     
 */
#define ERR_MSG_I102        "I102-0  "

/**
 * Service Status Screen Text: New CG for FSU product has been received.
 *
 * The compounds related new CG for FSU product has been received. This message is sent
 * when the Product Overwrite EMM with new CG for FSU product has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner.      
 */
#define ERR_MSG_I300        "I300-0  "

/**
 * Service Status Screen Text: New CG for Push VOD product has been received.
 *
 * The compounds related new CG for Push VOD product has been received. This message is sent
 * when the Product Overwrite EMM with new CG for Push VOD product has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner.     
 */
#define ERR_MSG_I301        "I301-0  "

/**
 * Service Status Screen Text: CG Mismatch.
 *
 * Incorrect CG. This message is sent when an EMM is rejected by the client because the client does not have the
 * right CG the EMM refers to. 
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I302        "I302-0  "

/**
 * Service Status Screen Text: SG Mismatch.
 *
 * Incorrect SG. This message is sent when an EMM is rejected by the client because the client does not have the
 * right SG the EMM refers to. 
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I303        "I303-0  "

/**
 * Service Status Screen Text: Invalid P-Key Index.
 *
 * Incorrect P-Key Index. This message is sent when an EMM is rejected by the client because the client does not support
 * the P-Key index the EMM refers to. 
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I304        "I304-0  "

/**
 * Service Status Screen Text: Invalid G-Key Index.
 *
 * Incorrect G-Key Index. This message is sent when an EMM is rejected by the client because the client does not support
 * the G-Key index the EMM refers to. 
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I305        "I305-0  "

/**
 * Service Status Screen Text: Invalid Timestamp received.
 *
 * Invalid Timestamp. This message is sent when an EMM is rejected by the client because the timestamp the EMM refers to is not valid. 
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I306        "I306-0  "

/**
 * Service Status Screen Text: Variant Mismatch.
 *
 * Incorrect Variant. This message is sent when an EMM is rejected by the client because the client does not have the
 * right Variant the EMM refers to. 
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I307        "I307-0  "

/**
 * Service Status Screen Text: P-Key Hash Mismatch.
 *
 * Incorrect P-Key Hash. This message is sent when an EMM is rejected by the client because the client does not have the
 * right P-Key Hash the EMM refers to. 
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I308        "I308-0  "

/**
 * Service Status Screen Text: Opcode Restricted.
 *
 * Incorrect Opcode. This message is sent when an EMM is rejected by the client because the client does not support
 * the Opcode the EMM refers to. 
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I309        "I309-0  "

/**
 * Service Status Screen Text: Invalid VOD Nonce.
 *
 * Incorrect VOD Nonce. This message is sent when an EMM is rejected by the client because the client does not have the
 * right VOD Nonce the EMM refers to. 
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I310        "I310-0  "

/**
 * Service Status Screen Text: TMS Failed.
 *
 * TMS Failed. This message is sent when an EMM related to TMS has been handled unsuccessfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I311        "I311-0  "

/**
 * Service Status Screen Text: Homing Channel Failed.
 *
 * Homing Channel Failed. This message is sent when an EMM related to Homing Channel has been handled unsuccessfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I312        "I312-0  "

/**
 * Service Status Screen Text: Invalid Middleware User Data.
 *
 * Incorrect Middleware User Data. This message is sent when an EMM related to Middleware User Data has been handled unsuccessfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I313        "I313-0  "

/**
 * Service Status Screen Text: Flexi Flash Failed.
 *
 * FlexiFlash Failed. This message is sent when an EMM related to Flexi Flash has been handled unsuccessfully or rejected by the client.
 * Note that if there are multiple packages/images sharing one FlexiFlash EMM filter, client will report I314-0 when it receives EMMs 
 * of packages/images that aren't needed by this client. 
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I314        "I314-0  "

/**
 * Service Status Screen Text: SN Mapping EMM handled successfully.
 *
 * SN Mapping EMM handled successfully. This message is sent when SN Mapping EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I315        "I315-0  "

/**
 * Service Status Screen Text: No valid entitlement found.
 *
 * Not Entitled. This message is sent when an EMM is rejected by the client because the client is not entitled
 * to use the product the EMM refers to. 
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I316        "I316-0  "

/**
 * Service Status Screen Text: No valid sector found.
 *
 * Incorrect Sector. This message is sent when an EMM is rejected by the client because the client does not have the
 * right sector the EMM refers to. 
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I317        "I317-0  "

/**
 * Service Status Screen Text: Sector Overwrite EMM handled successfully.
 *
 * Sector Overwrite EMM handled successfully. This message is sent when Sector Overwrite EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I318        "I318-0  "

/**
 * Service Status Screen Text: Pairing EMM handled successfully.
 *
 * Pairing EMM handled successfully. This message is sent when Pairing EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I319        "I319-0  "

/**
 * Service Status Screen Text: Product Key EMM handled successfully.
 *
 * Product Key EMM handled successfully. This message is sent when Product Key EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I320        "I320-0  "

/**
 * Service Status Screen Text: Product Overwrite EMM handled successfully.
 *
 * Product Overwrite EMM handled successfully. This message is sent when Product Overwrite EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I321        "I321-0  "

/**
 * Service Status Screen Text: Region Control EMM handled successfully.
 *
 * Region Control EMM handled successfully. This message is sent when Region Control EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I322        "I322-0  "

/**
 * Service Status Screen Text: PVR MSK EMM handled successfully.
 *
 * PVR MSK EMM handled successfully. This message is sent when PVR MSK EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I323        "I323-0  "

/**
 * Service Status Screen Text: VOD Product Overwrite EMM handled successfully.
 *
 * VOD Product Overwrite EMM handled successfully. This message is sent when VOD Product Overwrite EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I324        "I324-0  "

/**
 * Service Status Screen Text: VOD Asset ID EMM handled successfully.
 *
 * VOD Asset ID EMM handled successfully. This message is sent when VOD Asset ID EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I325        "I325-0  "

/**
 * Service Status Screen Text: Nationality EMM handled successfully.
 *
 * Nationality EMM handled successfully. This message is sent when Nationality EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I326        "I326-0  "

/**
 * Service Status Screen Text: Product Delete EMM handled successfully.
 *
 * Product Delete EMM handled successfully. This message is sent when Product Delete EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I327        "I327-0  "

/**
 * Service Status Screen Text: Entitlement Property EMM handled successfully.
 *
 * Entitlement Property EMM handled successfully. This message is sent when Entitlement Property EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I328        "I328-0  "

/**
 * Service Status Screen Text: Timestamp EMM handled successfully.
 *
 * Timestamp EMM handled successfully. This message is sent when Timestamp EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I329        "I329-0  "


/**
 * Service Status Screen Text: P-Key mismatch.
 *
 * Incorrect PKey. This message is sent when an EMM is rejected by the client because the client does not have the
 * right P-key the EMM refers to. 
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I330        "I330-0  "

/**
 * Service Status Screen Text: User Area EMM handled successfully.
 *
 * User Area EMM handled successfully. This message is sent when an User Area EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I331        "I331-0  "

/**
 * Service Status Screen Text: Application Data EMM handled successfully.
 *
 * Application Data EMM handled successfully. This message is sent when an Application Data EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I332        "I332-0  "

/**
 * Service Status Screen Text: Filter Criteria EMM handled successfully.
 *
 * Filter Criteria EMM handled successfully. This message is sent when a Filter Criteria EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I333        "I333-0  "

/**
 * Service Status Screen Text: Package EMM handled successfully.
 *
 * Package EMM handled successfully. This message is sent when a Package EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I334        "I334-0  "

/**
 * Service Status Screen Text: Block Download EMM handled successfully.
 *
 * Block Download EMM handled successfully. This message is sent when a Block Download EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I335        "I335-0  "

/**
 * Service Status Screen Text: IRD EMM handled successfully.
 *
 * IRD EMM handled successfully. This message is sent when an IRD EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I336        "I336-0  "

/**
 * Service Status Screen Text: Unique SN Mapping EMM handled successfully.
 *
 * Unique SN Mapping EMM handled successfully. This message is sent when an Unique SN Mapping EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I337        "I337-0  "
 
/**
 * Service Status Screen Text: Signed CCP CAM EMM handled successfully.
 *
 * Signed CCP CAM EMM handled successfully. This message is sent when a Signed CCP CAM EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I338        "I338-0  "

/**
 * Service Status Screen Text: TM Message EMM handled successfully.
 *
 * TM Message EMM handled successfully. This message is sent when a TM Message EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I339        "I339-0  "
   
/**
 * Service Status Screen Text: Macrovision Configuration EMM handled successfully.
 *
 * Macrovision Configuration EMM handled successfully. This message is sent when a Macrovision Configuration EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I340        "I340-0  "
 
/**
 * Service Status Screen Text: Extended TMS Message EMM handled successfully.
 *
 * Extended TMS Message EMM handled successfully. This message is sent when an Extended TMS Message EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I341        "I341-0  "

/**
 * Service Status Screen Text: Reset To Factory State EMM handled successfully.
 *
 * Reset To Factory State EMM handled successfully. This message is sent when a Reset To Factory State EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I342        "I342-0  "

/**
 * Service Status Screen Text: Invalid change version.
 *
 * Invalid change version. This message is sent when an EMM related to Adverstisement has been handled unsuccessfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I343        "I343-0  "

/**
 * Service Status Screen Text: Block Download EMM duplication.
 *
 * This Block Download EMM has been downloaded. This message is sent when an EMM related to Block Download has been download by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I344        "I344-0  "

/**
 * Service Status Screen Text: No Secure Core is loaded.
 *
 * This message is sent when an EMM is rejected because no matching secure core has been loaded.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I345        "I345-0  "

/**
 * Service Status Screen Text: Secure PVR MSK EMM handled successfully.
 *
 * Secure PVR MSK EMM handled successfully. This message is sent when a Secure PVR MSK EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I346        "I346-0  "

/**
 * Service Status Screen Text: Shared PVR MSK EMM handled incorrectly.
 *
 * Shared PVR MSK EMM handled incorrectly. This message is sent when errors occur during processing Shared PVR MSK EMM by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I347        "I347-0  "

/**
 * Service Status Screen Text: Shared PVR MSK EMM handled successfully.
 *
 * Shared PVR MSK EMM handled successfully. This message is sent when a Shared PVR MSK EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I348        "I348-0  "

/**
 * Service Status Screen Text: ARP+ Configuration EMM handled successfully.
 *
 * ARP+ Configuration EMM handled successfully. This message is sent when a ARP+ Configuration EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I349        "I349-0  "

/**
 * Service Status Screen Text: Invalid Secure CW Mode.
 *
 * This message is sent when Pairing EMM is rejected by the client because secure cw mode is not supported.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I350        "I350-0  "

/**
 * Service Status Screen Text: Invalid cipher mode.
 *
 * This message is sent when Product Overwrite EMM is rejected by the client because cipher mode is not supported.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I351        "I351-0  "

/**
 * Service Status Screen Text: Invalid White Box cipher mode.
 *
 * This message is sent when Advertisement EMM is rejected by the client because White Box cipher mode is not supported.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I352        "I352-0  "

/**
 * Service Status Screen Text: Asset VOD Entitlement Delete EMM handled successfully.
 *
 * Asset VOD Entitlement Delete EMM handled successfully. This message is sent when Asset VOD Entitlement Delete EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I353        "I353-0  "

/**
 * Service Status Screen Text: Proximity detection setting EMM handled successfully.
 *
 * Proximity detection setting EMM handled successfully. This message is sent when proximity detection setting EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I354        "I354-0  "

/**
 * Service Status Screen Text: PVR key ladder level not match.
 *
 * This message is sent when the PVR key ladder level in Pairing EMM does not match with that in secure chipset.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I355        "I355-0  "

/**
 * Service Status Screen Text: Pairing EMM is rejected due to IFCP incompatibility.
 *
 * This message is sent if the Pairing EMM has not been successfully handled by the client when the Pairing mode is IFCP.
 * For example, the secure chipset does not support IFCP mode.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I356        "I356-0  "

/**
 * Service Status Screen Text: IFCP Image Download EMM handled successfully.
 *
 * IFCP Image Download EMM handled successfully. This message is sent when the IFCP image download EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I357        "I357-0  "

/**
 * Service Status Screen Text: IFCP Image Advertisement EMM handled successfully.
 *
 * IFCP Image Advertisement EMM handled successfully. This message is sent when the IFCP image advertisement EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I358        "I358-0  "

/**
 * Service Status Screen Text: FSU Signal Announcement EMM handled successfully.
 *
 * FSU Signal Announcement EMM handled successfully. This message is sent when the FSU Signal Announcement EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I359        "I359-0  "

/**
 * Service Status Screen Text: FSU Signal Announcement EMM is dropped due to the FSU data has already been received.
 *
 * This message is sent when the client receives FSU Signal Announcement EMM, but the FSU data has already been received, so the EMM is useless and dropped.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I360        "I360-0  "

/**
 * Service Status Screen Text: HGPC Primary Secure Client Activation EMM has already been received.
 *
 * This message is sent when the HGPC Primary Secure Client Activation EMM has been handled successfully.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I361        "I361-0  "

/**
 * Service Status Screen Text: HGPC Primary or Secondary Secure Client Activation EMM has an old or invalid timestamp.
 *
 * This message is sent when the HGPC Activation EMM has an old or invalid timestamp.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I362        "I362-0  "

/**
 * Service Status Screen Text: HGPC Primary or Secondary Secure Client Activation EMM does not match the current HGPC client.
 *
 * This message is sent when the HGPC Activation EMM does not match the current HGPC client. e.g. Currently the client is HGPC primary, but the HGPC Secondary
 * Activation is received.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I363        "I363-0  "

/**
 * Service Status Screen Text: TDC_task EMM handled successfully.
 *
 * TDC_task EMM handled successfully. This message is sent when the TDC_task EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I364        "I364-0  "

/**
 * Service Status Screen Text: Extended Pairing (with SCOT-ext) EMM handled successfully.
 *
 * Extended Pairing (with SCOT-ext) EMM handled successfully. This message is sent when the Extended Pairing (with SCOT-ext) EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I365        "I365-0  "

/**
 * Service Status Screen Text: Chip ID Mapping EMM handled successfully.
 *
 * Chip ID Mapping EMM handled successfully. This message is sent when the Chip ID Mapping EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error statuses are interim statuses. They should not be displayed in the error banner.           
 * The Application can ignore Ixxx-0 in the error banner. 
 */
#define ERR_MSG_I366        "I366-0  "

/**
 * Service Status Screen Text: No valid entitlement found.
 *
 * Not Entitled. This message is sent when an ECM is rejected by the client because the client is not entitled to use the VOD product the ECM refers to.
 */
#define ERR_MSG_E015        "E015-0  "

/**
 * Service Status Screen Text: No valid entitlement found.
 *
 * Not Entitled. This message is sent when an ECM is rejected by the client because the client is not entitled
 * to use the product the ECM refers to. 
 *
 * For Cloaked CA Agent for IP only STBs, the CA client will not pull EMM through request entitlement to resolve this problem automatically.
 * After receiving this error code from the CA Client, the application should remind the user with specific information and let the user  
 * determine whether or not to request the entitlement. The request action can be done via ::UniversalClient_IPTV_RetryRequest.
 */
#define ERR_MSG_E016        "E016-0  "

/**
 * Service Status Screen Text: No valid sector found.
 *
 * Incorrect Sector. This message is sent when an ECM is rejected by the client because the client does not have the
 * right sector the ECM refers to. 
 */
#define ERR_MSG_E017        "E017-0  "

/**
 * Service Status Screen Text: Product blackout.
 *
 * Black out. This message is sent when an ECM is rejected by the client due to a product blackout filter imposed by the Headend. 
 */
#define ERR_MSG_E018        "E018-0  "

/**
 * Service Status Screen Text: P-Key mismatch.
 *
 * Incorrect PKey. This message is sent when an ECM is rejected by the client because the client does not have the
 * right Pkey the ECM refers to. 
 */
#define ERR_MSG_E030        "E030-0  "

/**
 * Service Status Screen Text: G-Key mismatch.
 *
 * Incorrect GKey. When an ECM is rejected by the client because the client does not have the
 * right Gkey the ECM refers to. This message is not used currently.
 */
#define ERR_MSG_E031        "E031-0  "

/**
 * Service Status Screen Text: TG mismatch.
 *
 * Incorrect TG. This message is sent when an ECM is rejected by the client because the client does not have the
 * right TG the ECM refers to. 
 */
#define ERR_MSG_E032        "E032-0  "


/**
 * Service Status Screen Text: CWDK mismatch.
 *
 * Incorrect CWDK. This message is sent when an ECM is rejected by the client because the client does not have the
 * right CWDK the ECM refers to. 
 */
#define ERR_MSG_E033        "E033-0  "

/**
 * Service Status Screen Text:  Macrovision blocking enforced.
 *
 * The watching service is a Macrovision service,
 * but the client device does not support Macrovision currently.
 * If the application want the client device to support Macrovision, 
 * the application must call ::UniversalClient_EnableMacrovision. 
 * E055 will NOT be used from Cloaked CA Agent v2.1.0. It is still here only for compatibility consideration.
 */
#define ERR_MSG_E055        "E055-0  "


/**
 * Service Status Screen Text: Invalid PVR metadata.
 *
 * Invalid PVR Metadata. This message is sent when PVR playback is rejected by the client because the submitted PVR metadata cannot be processed.
 */
#define ERR_MSG_E094        "E094-0  "
 
/**
 * Service Status Screen Text: Incomplete Definition.
 *
 * CAT or PMT Absent. This message is sent when the service has been opened but no PMT or CAT is defined..
 * E101-0 is the default service status once the service has been opened.
 *
 * \note E101-0 and E102-0 have been replaced by I101-0 and I102-0 from Cloaked CA Agent v2.0.2. They are still here for compatibility consideration only.
 * E101-0 and E102-0 will NOT be used from Cloaked CA Agent v2.0.2.
 */
#define ERR_MSG_E101        "E101-0  "

/**
 * Service Status Screen Text: No ECM/EMM data received.
 *
 * No ECM/EMM Received.
 * 
 * For EMM service of Secure Chipset based STBs, this message is sent when the service has been opened and CAT has been defined,
 * but NO EMM has been received.
 *
 * For EMM service of IP only STBs, this message is sent when the service has been opened, 
 * but NO EMM has been received after a sucessful client registration.
 *
 * For ECM service, this message is sent when the service has been opened and PMT is defined,
 * but NO ECM is received.
 *
 * \note E101-0 and E102-0 have been replaced by I101-0 and I102-0 from Cloaked CA Agent v2.0.2. They are still here for compatibility consideration only.
 * E101-0 and E102-0 will NOT be used from Cloaked CA Agent v2.0.2.
 */
#define ERR_MSG_E102        "E102-0  "

/**
 * Service Status Screen Text: No valid ECM found or mismatched pipe selection.
 *
 * ECM Rejected. This message is sent when an ECM is rejected by the client for a generic reason or mismatched pipe selection. 
 * This means that a descrambling key will not be sent to the stream implementation for this ECM. 
 */
#define ERR_MSG_E103        "E103-0  "

/**
 * Service Status Screen Text: Invalid EMM received.
 *
 * EMM Rejected. This message is sent when an EMM is rejected by the client. This means that if the client was depending
 * on this EMM in order to decode an ECM, that decoding will not occur. 
 */
#define ERR_MSG_E104        "E104-0  "

/**
 * Service Status Screen Text: No matching CA System ID in PMT.
 *
 * Incorrect CA System ID. This message is sent when no matched CA System ID found in PMT.
 */
#define ERR_MSG_E106        "E106-0  "

/**
 * Service Status Screen Text: No PVR Session Key to perform PVR operation.
 *
 * No PVR Session Key. This message is sent when a PVR record/playback request is rejected by the client because the client cannot get the PVR Session Key to perform PVR record/playback. 
 */
#define ERR_MSG_E111        "E111-0  "

/**
 * Service Status Screen Text: IFCP returns an error when there is a CUR enforcement failure
 *
 * This message is sent when IFCP cannot finalize the setting of recording keys during recording or playback for 4K content.
 * For more details, check the ::UC_SERVICE_PVR_RECORD_STATUS_REPLY or ::UC_SERVICE_PVR_PLAYBACK_STATUS_REPLY.
 */
#define ERR_MSG_E113           "E113-0  "

/**
 * Service Status Screen Text: Not entitled to perform PVR operation.
 *
 * PVR Not Entitled. This message is sent when a PVR Record/Playback Request is rejected by the client because the client does not have
 * the entitlement to perform PVR record/playback. 
 */
#define ERR_MSG_E128        "E128-0  "

/**
 * Service Status Screen Text: The PVR content is expired.
 *
 * The PVR content is expired, unable to view. This message is sent when a PVR playback request is rejected by the client because the content is expired.
 */
#define ERR_MSG_E129        "E129-0  "

/**
 * Service Status Screen Text: PVR Record is not allowed.
 *
 * PVR Not Allowed. This message is sent when a PVR Record Request is rejected by the client because PVR is not allowed 
 * for this service. 
 * \note This is not used after Cloaked CA Agent Rabbit 2.1.0.
 *
 */
#define ERR_MSG_E130        "E130-0  "

/**
 * Service Status Screen Text: No PVR Master Session Key to perform PVR operation.
 *
 * No PVR Master Session Key. This message is sent when a PVR record/playback request is rejected by the client because the client does not have
 * PVR Master Session Key to perform PVR record/playback. 
 */
#define ERR_MSG_E131        "E131-0  "

/**
 * Service Status Screen Text: No PVR CPSK Key to perform PVR operation.
 *
 * No PVR CPSK Key. This message is sent when a PVR record/playback request is rejected by the client because the client does not have
 * the Key to perform PVR record/playback. 
 */
#define ERR_MSG_E132        "E132-0  "

/**
 * Service Status Screen Text: Asset ID mismatch when converting Asset VOD content to PVR.
 *
 * Asset ID mismatch. This message is sent in case that the asset ID in the ECM is not consistent with the asset ID in the Metadata, 
 * which was submit when middleware resumed recording the VOD content.
 *
 */
#define ERR_MSG_E133        "E133-0  "

/**
 * Service Status Screen Text: PVR Recording is prohibited due to copy control setting.
 *
 * PVR Copying prohibited. This message is sent in case that the copy control setting is set to copy no more or Copy Never , 
 * That means no further copying is permitted or copying is prohibited, unless for trick-modes.
 *
 */
#define ERR_MSG_E134        "E134-0  "

/**
 * Service Status Screen Text: PVR Sharing is prohibited due to isShareable setting.
 *
 * PVR sharing prohibited. This message is sent in case that the isShareable flag is set to 0x00 , 
 * That means the content is available for copying but disallow for sharing in home network. This error code is only available for HN PVR.
 *
 */
#define ERR_MSG_E135        "E135-0  "

/**
 * Service Status Screen Text: PVR sharing is prohibited due to domain id mismatch.
 *
 * PVR Domain mismatch. This message is sent in case that the Cloaked CA does not have a valid domain, or the domain ID mismatch, or the Cloaked CA cannot find a correct MSK. 
 * That means sharing playback is prohibited, this error code is only available for HN PVR.
 *
 */
#define ERR_MSG_E138        "E138-0  "

/**
 * Service Status Screen Text: No valid CA Regional Information found.
 *
 * CA Regional Information Absent. This message is sent when an ECM is rejected by the client because the client does not have any
 * Regional Filtering Descriptor but Regional Filtering is enabled via ::UniversalClient_EnableRegionalFiltering.
 */
#define ERR_MSG_E139        "E139-0  "

/**
 * Service Status Screen Text: Region or Sub-Region mismatch.
 *
 * Regional Blocking. This message is sent when an ECM is rejected by the client because Regional Filtering is active and 
 * either region or sub-region does not match. 
 */
#define ERR_MSG_E140        "E140-0  "

/**
 * Service Status Screen Text: The Pre-enablement product is expired.
 *
 * The Pre-enablement product is expired. This message is sent when an ECM, that just for the Pre-enablement service, is rejected by the client because the Pre enablement product is expired.
 */
#define ERR_MSG_E150        "E150-0  "
 
/**
 * Service Status Screen Text: PESK mismatch.
 *
 * Incorrect PESK. This message is sent when an ECM, that just for the Pre-enablement service, is rejected by the client because of the incorrect PESK.
 * The required PESK cannot be found in the provisioned PEF due to index mismatch, sector mismatch or key protection algorithm mismatch.
 */
#define ERR_MSG_E151        "E151-0  "

/**
 * Service Status Screen Text: The Secure Pre-enablement is not supported.
 *
 * This message is sent if the Secure Pre-enablement feature is not supported in IFCP.
 */
#define ERR_MSG_E157        "E157-0  "

/**
 * Service Status Screen Text: White Box algorithm mismatch.
 *
 * Incorrect White Box algorithm. This message is sent when an ECM is rejected by the client because the White Box algorithm is not supported.
 */
#define ERR_MSG_E160        "E160-0  "

/**
 * Service Status Screen Text: Failed to load the required Cloaked CA package. Device is trying to download it.
 *
 * Failed to load the required Cloaked CA package from Persistent Storage, so the service cannot be descrambled at present.
 * The client will download the required Cloaked CA package from the live stream if possible. 
 */
#define ERR_MSG_E161        "E161-0  "

/**
 * Service Status Screen Text: Failed to load IFCP image.
 *
 * Failed to load the IFCP image into the secure chipset. This message is sent when the service cannot be descrambled 
 * or the PVR operation fails because the IFCP image has not been loaded into the secure chipset.
 * This is caused when the ::UniversalClientSPI_IFCP_LoadImage fails, or there is not IFCP image in the device.
 */
#define ERR_MSG_E162        "E162-0  "

/**
 * Service Status Screen Text: Invalid arbitrary value length.
 *
 * This message is sent when arbitrary value length in IFCP application payload
 * does not match the feature bitmap set.
 * This error may occur when ::UniversalClientSPI_IFCP_Communicate fails.
 */
#define ERR_MSG_E163        "E163-0  "

/**
 * Service Status Screen Text: IFCP authentication failure.
 *
 * This message is sent when arbitrary value authentication checking failed in IFCP.
 * This error may occur when ::UniversalClientSPI_IFCP_Communicate fails.
 */
#define ERR_MSG_E164        "E164-0  "

/**
 * Service Status Screen Text: Device does not support IFCP.
 *
 * Device does not support IFCP. This message is sent when an ECM is rejected by the client because IFCP is not supported.
 */
#define ERR_MSG_E165        "E165-0  "

/**
 * Service Status Screen Text: Failed to receive CA data for security update process.
 *
 * This message is sent when an ECM of the test service is rejected by the client because of FSU EMM reception failure.
 */
#define ERR_MSG_E166        "E166-0  "

/**
 * Service Status Screen Text: No enough memory to load the new secure core package during FSU.
 *
 * This message is sent when an ECM of the test service is rejected by the client because there is not enough memory to load the new secure core package to process the ECM during FSU .
 */
#define ERR_MSG_E167        "E167-0  "

/**
 * Service Status Screen Text: Either the TDC SPI was not implemented or an error occurred when Cloaked CA loaded TDC via this SPI.
 *
 * This message is sent when an ECM is rejected by the client because the transformation client doesn't load TDC successfully.
 */
#define ERR_MSG_E169        "E169-0  "

/**
 * Service Status Screen Text: The secure core does not match the transformation mode.
 *
 * This message is sent when an ECM is rejected by the client because the secure core does not match the transformation mode.
 */
#define ERR_MSG_E170        "E170-0  "

/**
 * Service Status Screen Text: Traitor Tracing is not supported in MSR mode.
 *
 * This message is sent when a Traitor Tracing ECM is received by an agent that is not switched to IFCP mode yet.
 */
#define ERR_MSG_E172        "E172-0  "

/**
 * Service Status Screen Text: Traitor Tracing application data version mismatch.
 *
 * This message is sent when the Traitor Tracing application data version does not match the version of the IFCP RAM APP.
 * This error may occur when ::UniversalClientSPI_IFCP_Communicate fails.
 */
#define ERR_MSG_E173        "E173-0  "

/**
 * Service Status Screen Text: Traitor Tracing is not supported.
 *
 * This message is sent if the Traitor Tracing feature is not supported in IFCP.
 * This error may occur when ::UniversalClientSPI_IFCP_Communicate fails.
 */
#define ERR_MSG_E174        "E174-0  "

/**
 * Service Status Screen Text: Incorrect number of CWs in the Traitor Tracing application data.
 *
 * This message is sent when an ECM of Traitor Tracing is rejected by the client because the number of CWs is incorrect.
 * This error may occur when ::UniversalClientSPI_IFCP_Communicate fails.
 */
#define ERR_MSG_E175        "E175-0  "

/**
 * Service Status Screen Text: There is a mismatch for existing Auth Ctrl value.
 *
 * This message is sent when an ECM is rejected by the IFCP RamApp according to wrong value of Auth Ctrl parameter.
 */
#define ERR_MSG_E176        "E176-0  "

/**
 * Service Status Screen Text: FlexiFlash not initialized.
 *
 * This message is sent if the FlexiFlash is not initialized.
 */
#define ERR_MSG_E501        "E501-0  "

/**
 * Service Status Screen Text: Invalid CFG data.
 *
 * Invalid CFG data.This message is sent if the CFG data is invalid.
 */
#define ERR_MSG_E502        "E502-0  "

/**
 * Service Status Screen Text: Invalid Cloaked CA package data.
 *
 * This message is sent when an invalid Cloaked CA package is rejected to be loaded.
 */
#define ERR_MSG_E503        "E503-0  "

/**
 * Service Status Screen Text: Cloaked CA package not loaded.
 *
 * This message is sent when a required Cloaked CA package has not been loaded.
 */
#define ERR_MSG_E504        "E504-0  "

/**
 * Service Status Screen Text: Invalid Cloaked CA package index.
 *
 * This message is sent when an invalid Cloaked CA package index is found in EMMs. 
 */
#define ERR_MSG_E505        "E505-0  "

/**
 * Service Status Screen Text: Duplicated VM ID.
 *
 * This message is sent when the VM instance is not created because the VM ID has already been used.
 */
#define ERR_MSG_E506        "E506-0  "

/**
 * Service Status Screen Text: Create VM context failed.
 *
 * This message is sent when the VM context failed to be created.
 */
#define ERR_MSG_E507        "E507-0  "

/**
 * Service Status Screen Text: Create VM failed.
 *
 * This message is sent when a VM instance failed to be created.
 */
#define ERR_MSG_E508        "E508-0  "

/**
 * Service Status Screen Text: Too many vm created.
 *
 * This message is sent when a VM instance is not created due to too many instances have been created.
 */
#define ERR_MSG_E509        "E509-0  "

/**
 * Service Status Screen Text: VM load image failed.
 *
 * This message is sent when the VM failed to load the Cloaked CA package.
 */
#define ERR_MSG_E510        "E510-0  "

/**
 * Service Status Screen Text: VM memmap failed.
 *
 * This message is sent when the VM failed to take the memory map operation.
 */
#define ERR_MSG_E511        "E511-0  "

/**
 * Service Status Screen Text: VM execution failed.
 *
 * This message is sent when the VM failed to execute.
 */
#define ERR_MSG_E512        "E512-0  "

/**
 * Service Status Screen Text: VM IO failed.
 *
 *  This message is sent when the VM failed to take IO operations.
 */
#define ERR_MSG_E513        "E513-0  "

/**
 * Service Status Screen Text: VM reset failed.
 *
 *  VM reset failed.This message is sent when VM not runing because the VM reset failed.
 */
#define ERR_MSG_E514        "E514-0  "

/**
 * Service Status Screen Text: Root key check failed.
 *
 *  The root key hash check failed when loading Cloaked CA package.
 */
#define ERR_MSG_E515        "E515-0  "

/**
 * Service Status Screen Text: Invalid package signature.
 *
 *  The package signature is invalid.
 */
#define ERR_MSG_E516        "E516-0  "

/**
 * Service Status Screen Text: Unsupported compress algorithm.
 *
 *  The compress algorithm is not supported.
 */
#define ERR_MSG_E517        "E517-0  "

/**
 * Service Status Screen Text: Invalid SYS ID.
 *
 *  The SYS ID is invalid.
 */
#define ERR_MSG_E518        "E518-0  "

/**
 * Service Status Screen Text: Client type mismatch.
 *
 *  The client type mismatch.
 */
#define ERR_MSG_E519        "E519-0  "

/**
 * Service Status Screen Text: The client version too low to support FlexiFlash download.
 *
 *  The client version too low.
 */
#define ERR_MSG_E520        "E520-0  "

/**
 * Service Status Screen Text: Invalid Variant.
 *
 *  The variant is invalid.
 */
#define ERR_MSG_E521        "E521-0  "

/**
 * Service Status Screen Text: Invalid White Box algorithm.
 *
 * The White Box algorithm is invalid.
 */
#define ERR_MSG_E522        "E522-0  "

/**
 * Service Status Screen Text: No valid IFCP image on PS.
 *
 * There is not valid IFCP image in the PS. It may be a virgin box or the PS is corrupt.
 */
#define ERR_MSG_E552        "E552-0  "

/**
 * Service Status Screen Text: Fail to load IFCP image to the secure chipset.
 *
 * Cloaked CA called ::UniversalClientSPI_IFCP_LoadImage but it returned failure, or the SPI is not implemented.
 */
#define ERR_MSG_E553        "E553-0  "

/**
 * Service Status Screen Text: Initialization has not been performed yet.
 *
 * Client Not Initialized. This message is sent when an ECM is rejected by the client because the client has NOT been initialized, 
 * i.e. the client currently does not have a valid unique address.
 */
#define ERR_MSG_E600        "E600-0  "


/**
 * Service Status Screen Text: Card In.
 *
 * Card In, card is a valid Irdeto Access Smartcard.
 */
#define ERR_MSG_D000_4         "D000-4  "

/**
 * Service Status Screen Text: Service is currently descrambled.
 *
 * The currently watched service is being descrambled successfully. 
 */
#define ERR_MSG_D029_4         "D029-4  "

/**
 * Service Status Screen Text: DVB EMM Service Ok.
 *
 * Service Ok.
 */
#define ERR_MSG_D100_5        "D100-5  "

/**
 * Service Status Screen Text: Descrambler Service Ok.
 *
 * Service Ok.
 */
#define ERR_MSG_D100_8         "D100-8  "

/**
 * Service Status Screen Text: No Matched Vendor ID.
 *
 * No EMM PID could be found matching the Smartcard's vendor ID. No EMMs will be filtered.
 */
#define ERR_MSG_D101_6        "D101-6  "

/**
 * Service Status Screen Text: No Vendor ID.
 *
 * There is no vendor ID (card is not in and verified).
 */
#define ERR_MSG_D102_9         "D102-9  "

/**
 * Service Status Screen Text: FTA Service.
 *
 * This service is deemed to be free-to-air because there are no CA Descriptors in the PMT.
 */
#define ERR_MSG_D105_9         "D105-9  "

/**
 * Service Status Screen Text: Please insert the correct smartcard.
 *
 * This stream is scrambled by an alternate vendor with non-matching CA System ID.
 */
#define ERR_MSG_D106_9         "D106-9  "

/**
 * Service Status Screen Text: Checking Smartcard.
 *
 * Card verifying.
 */
#define ERR_MSG_I007_4         "I007-4  "

/**
 * Service Status Screen Text: Validating subscription, please wait.
 *
 * The scrambling status was not determined yet.
 */
#define ERR_MSG_I034_9         "I034-9  "

/**
 * Service Status Screen Text: Subscription update in progress.
 *
 * All products expired, still can continue descrambling until grace period ends.
 */
#define ERR_MSG_I055_4         "I055-4  "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * The Smart Card returned a status that is not handled by SoftCell.
 */
#define ERR_MSG_I102_4         "I102-4  "

/**
  * Service Status Screen Text: Validating subscription, please wait.
  *
  * Waiting for Regional Filtering information.
  */
#define ERR_MSG_I139_4         "I139-4  "

/**
 * Service Status Screen Text: Service not scrambled.
 *
 * The currently watched service is not scrambled. No message will appear on the screen, the status will only be shown on the CA Status screen.
 */
#define ERR_MSG_E000_32        "E000-32 "

/**
 * Service Status Screen Text: Please insert Smartcard.
 *
 * Card Out, Please insert Smartcard.
 */
#define ERR_MSG_E004_4         "E004-4  "

/**
 * Service Status Screen Text: Unknown Smartcard.
 *
 * Unknown Card (non-Irdeto Access).
 */
#define ERR_MSG_E005_4         "E005-4  "

/**
 * Service Status Screen Text: Smart Card Failure.
 *
 * Card Error, communications with Smart Card have failed.
 */
#define ERR_MSG_E006_4         "E006-4  "

/**
 * Service Status Screen Text: No permission to view this channel.
 *
 * The viewer is not entitled to watch this service because the product in the ECM playout is not on the Smartcard.
 */
#define ERR_MSG_E016_4         "E016-4  "

/**
 * Service Status Screen Text: Service is currently scrambled.
 *
 * The viewer is not entitled to watch this service because the Smart Card is rejected by the Smart Card marriage function.
 */
#define ERR_MSG_E017_13        "E017-13 "

/**
 * Service Status Screen Text: This channel is not available in this location.
 *
 * The viewer is not entitled to watch this service due to a product blackout filter imposed by the Headend.
 */
#define ERR_MSG_E018_4        "E018-4  "

/**
 * Service Status Screen Text: The subscription to this channel has expired.
 *
 * The viewer is not entitled to watch this service because the product entitlement on the Smart Card has expired.
 */
#define ERR_MSG_E019_4        "E019-4  "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * An authentication failure occurred during ECM submission by SoftCell to the Smartcard. This is due to a key mismatch between the playout and the Smartcard.
 */
#define ERR_MSG_E030_4         "E030-4  "

/**
 * Service Status Screen Text: Viewing is temporarily blocked, please stay tuned.
 *
 * The viewer is not entitled to watch this service because surf locking is active.
 */
#define ERR_MSG_E032_4         "E032-4  "

/**
 * Service Status Screen Text: Invalid Smartcard.
 *
 * Ci-Layer failure.
 */
#define ERR_MSG_E033_4         "E033-4  "

/**
 * Service Status Screen Text: Service Unknown.
 *
 * There is no entry for the required service ID in the PAT on the transport stream that is currently tuned to.
 * There is no PAT on the transport stream that is currently tuned to.
 */
#define ERR_MSG_E037_32         "E037-32 "

/**
 * Service Status Screen Text: Service is not currently running.
 *
 * There is no PMT for the service.
 */
#define ERR_MSG_E038_32         "E038-32 "

/**
 * Service Status Screen Text: Locating Service.
 *
 * Occurs when the service being watched is moved to a different transport stream. This could be deduced by any one of the following:
 * Transport stream ID for the relevant SDT has changed.
 * Transport stream ID of the PAT has changed.
 * Service list descriptor containing the service is attached to a transport stream other than the transport stream currently tuned to. This would be indicated in the NIT or BAT for a specific operator.
 */
#define ERR_MSG_E039_32         "E039-32 "

/**
 * Service Status Screen Text: Decoder Memory Full.
 *
 * Decoder Memory Full.
 */
#define ERR_MSG_E040_32         "E040-32 "

/**
 * Service Status Screen Text: Service is not currently available.
 *
 * May occur when the IRD retrieves the PAT or PMT and the current event of the currently watched service is not allowed in the country, as per the definition of E043 (Not Allowed in this Country)..
 */
#define ERR_MSG_E041_32         "E041-32 "

/**
 * Service Status Screen Text: Lock and/or Ask for Parental PIN.
 *
 * When the parental rating of the event is greater than the IRD setting that has been made via the Parental Control menu.
 */
#define ERR_MSG_E042_32        "E042-32 "

/**
 * Service Status Screen Text: Not allowed in this country.
 *
 * The currently watched service is not allowed in the country indicated by the Cloaked CA Agent.
 */
#define ERR_MSG_E043_32        "E043-32 "

/**
 * Service Status Screen Text: No Event Information.
 *
 * An EIT for the current event of the current service has not been received.
 */
#define ERR_MSG_E044_32        "E044-32 "

/**
 * Service Status Screen Text: Service not allowed.
 *
 * An attempt is made to tune to a service on a transport stream, which is blocked by the mux verifier.
 */
#define ERR_MSG_E045_32        "E045-32 "

/**
 * Service Status Screen Text: Reading Satellite Information.
 *
 * Reading Satellite Information, searching SI.
 */
#define ERR_MSG_E046_32        "E046-32 "

/**
 * Service Status Screen Text: Bouquet Block.
 *
 * Bouquet Blocking.
 */
#define ERR_MSG_E047_32        "E047-32 "

/**
 * Service Status Screen Text: No Signal or Searching for signal.
 *
 * When the signal has been lost for longer than 30 seconds, the E052 banner should appear instead of E048.
 */
#define ERR_MSG_E048_32        "E048-32 "

/**
 * Service Status Screen Text: LNB Overload.
 *
 * Occurs when the current drawn out of the front-end (to power the LNB) is excessive.
 */
#define ERR_MSG_E049_32        "E049-32 "

/**
 * Service Status Screen Text: No Services available.
 *
 * An attempt is made to change to a service list of a particular type (e.g. TV, Audio) and there are no services of that type available in the bouquet.
 */
#define ERR_MSG_E050_32        "E050-32 "

/**
 * Service Status Screen Text: Searching for signal.
 *
 * Occurs when attempting to tune to a service on a transport stream that cannot be found (e.g. because the service has moved to a new frequency), or if the signal drops.
 */
#define ERR_MSG_E052_32        "E052-32 "

/**
 * Service Status Screen Text: Invalid tuning parameters.
 *
 * Occurs when there is an attempt to tune the front-end to a frequency outside the capability of the IRD.
 */
#define ERR_MSG_E064_32        "E064-32 "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * No de-Tweak key received for Custom CA Yet.
 */
#define ERR_MSG_E080_35        "E080-35 "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * De-Tweak key mismatch.
 */
#define ERR_MSG_E081_35        "E081-35 "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * The viewer is not entitled to watch this service because the date code on the card has gone backwards.
 */
#define ERR_MSG_E100_4         "E100-4  "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * The viewer is not entitled to watch this service because the sector number in the ECM playout is not on the Smartcard.
 */
#define ERR_MSG_E101_4         "E101-4  "

/**
 * Service Status Screen Text: Please insert the correct smartcard.
 *
 * This stream is scrambled by an alternate vendor with non-matching CA System ID.
 */
#define ERR_MSG_E106_9         "E106-9  "

/**
 * Service Status Screen Text: The smartcard is not fully authorized.
 *
 * The Smart Card and STB are not matched.
 */
#define ERR_MSG_E107_4         "E107-4  "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * An authentication failure occurred during ECM submission by SoftCell to the Smartcard. This is due to Patch Filter fails.
 */
#define ERR_MSG_E108_4         "E108-4  "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * No ECMs found that match the smartcard, with correct CCPversion. Or ECM rejected by the Smartcard, for incorrect CCPversion.
 */
#define ERR_MSG_E109_4         "E109-4  "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * Ci-layer type indicator in ECM header is higher than current built up Ci-layer.
 */
#define ERR_MSG_E118_4         "E118-4  "

/**
 * Service Status Screen Text: Please wait, while the card is being synchronized.
 *
 * The card is synchronizing the timestamp with the system.
 */
#define ERR_MSG_E120_4         "E120-4  "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * The viewer is not entitled to watch this service because the ECM timestamp expired.
 */
#define ERR_MSG_E133_4         "E133-4  "

/**
 * Service Status Screen Text: This channel is age restricted, insert the parental pin to view.
 *
 * The current event is Maturity Rating blocked. PIN for user profile of sufficient age needed to unblock.
 */
#define ERR_MSG_E136_4         "E136-4  "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * The current event is Maturity Rating blocked. The MR profile ID on the card is not in the ECMs.
 */
#define ERR_MSG_E137_4         "E137-4  "

/**
  * Service Status Screen Text: This channel is not available in this location.
  *
  * Regional Blocking.
  */
#define ERR_MSG_E140_4         "E140-4  "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * Home Gateway Proximity Control disallows the smartcard to be used for descrambling in the decoder.
 */
#define ERR_MSG_E141_4         "E141-4  "

/**
 * Service Status Screen Text: Insert the secondary card into the primary decoder to continue viewing with the secondary decoder.
 *
 * Home Gateway Proximity Control HNR timer expired on the smartcard. No descrambling.
 */
#define ERR_MSG_E142_4         "E142-4  "

/**
 * Service Status Screen Text: Wrong Home Network.
 *
 * This error banner is added in HGPC feature.
 */
#define ERR_MSG_E144_4         "E144-4  "

/**
 * Service Status Screen Text: Preview time period time out.
 *
 * The preview time reach the maximum limitation of this epoch.
 */
#define ERR_MSG_E152_4         "E152-4  "

/**
 * Service Status Screen Text: The device is invalid.
 *
 * Client Invalid. This message is sent when client registration or pull emm response is rejected by the client because the client is invalid, 
 * i.e. the client currently is not a legal one.
 *
 * For IP only STBs, the application should remind the user to contact with the operator to resolve this problem.
 * The application should ignore this error message if the status of the currently playing elementary streams is ::ERR_MSG_D029, or the 
 * currently playing elementary streams are clear ones.
 * The application should remove this error message if it was already displayed on the screen when the status of the currently playing elementary 
 * streams turned to ::ERR_MSG_D029, or the application switched to clear elementary streams. 
 */
#define ERR_MSG_E116        "E116-0  "

/**
 * Service Status Screen Text: Device initialization has failed.
 *
 * Device initialization failed. This message is sent when the client registration response is rejected by the client, 
 * i.e. the response message does not pass the validation check. The application will remind the user to try the request again in order to resolve 
 * this problem. The re-try action can be done via ::UniversalClient_IPTV_RetryRequest.
 * The application will ignore this error message if the status of the currently playing elementary streams is ::ERR_MSG_D029, or the 
 * currently playing elementary streams are clear ones.
 * The application will remove this error message if it was already displayed on the screen when the status of the currently playing elementary 
 * streams turned to ::ERR_MSG_D029, or the application switched to clear elementary streams. 
 */
#define ERR_MSG_E200        "E200-0  "

/**
 * Service Status Screen Text: Timeout when waiting to receive the client registration response.
 *
 * Timeout when waiting to receive the client registration response. This message is sent when the client cannot receive the expected client
 * registration response with in a set duration, i.e. the client cannot receive the response within 10 seconds.
 * The application will remind the user to try the request again in order to resolve this problem. The re-try action can be done 
 * via the ::UniversalClient_IPTV_RetryRequest.
 * The application will ignore this error message if the status of the currently playing elementary streams is ::ERR_MSG_D029, or the 
 * currently playing elementary streams are clear ones.
 * The application will remove this error message if it was already displayed on the screen when the status of the currently playing elementary 
 * streams changed to ::ERR_MSG_D029, or the application switched to clear elementary streams. 
 */
#define ERR_MSG_E201        "E201-0  "

/**
 * Service Status Screen Text: Timeout to receive the request entitlement response.
 *
 * Timeout to receive the request entitlement response. This message is sent when the client cannot receive the expected request
 * entitlement response at a certain time interval.i.e. the client cannot receive the response within 10 seconds.
 *
 * For IP only STBs, the application should remind the user to try the request again in order to resolve this problem.
 * The re-try action can be done via ::UniversalClient_IPTV_RetryRequest.
 * The application should ignore this error message if the status of the currently playing elementary streams is ::ERR_MSG_D029, or the 
 * currently playing elementary streams are clear ones.
 * The application should remove this error message if it was already displayed on the screen when the status of the currently playing elementary 
 * streams turned to ::ERR_MSG_D029, or the application switched to clear elementary streams. 
 */
#define ERR_MSG_E202        "E202-0  "

/**
 * Service Status Screen Text: Request entitlement has failed.
 *
 * Request entitlement has failed. This message is sent when the client request entitlement response is rejected by the client, 
 * i.e. the response message is not correct.
 *
 * For IP only STBs, the application should remind the user to try the request again in order to resolve this problem.
 * The re-try action can be done via ::UniversalClient_IPTV_RetryRequest.
 * The application should ignore this error message if the status of the currently playing elementary streams is ::ERR_MSG_D029, or the 
 * currently playing elementary streams are clear ones.
 * The application should remove this error message if it was already displayed on the screen when the status of the currently playing elementary 
 * streams turned to ::ERR_MSG_D029, or the application switched to clear elementary streams. 
 * \note This error code will not be used after Cloaked CA for IP only STBs 3.0.5.
 */
#define ERR_MSG_E203        "E203-0  "

/**
 * Service Status Screen Text: CCIS internal error has happened.
 *
 * Request has failed. This message is sent when the client request entitlement reponse or client registration reponse is rejected by the client, 
 * because internal errors occured within CCIS.
 *
 * For IP only STBs, the application should remind the user to try the request again in order to resolve this problem.
 * The re-try action can be done via ::UniversalClient_IPTV_RetryRequest.
 * The application should ignore this error message if the status of the currently playing elementary streams is ::ERR_MSG_D029, or the 
 * currently playing elementary streams are clear ones.
 * The application should remove this error message if it was already displayed on the screen when the status of the currently playing elementary 
 * streams turned to ::ERR_MSG_D029, or the application switched to clear elementary streams. 
 */
#define ERR_MSG_E204        "E204-0  "

/**
 * Service Status Screen Text: DeviceID/UniqueAddress pair mismatches.
 *
 * DeviceID/UniqueAddress pair mismatches.This message is sent when pull emm response is rejected by the client because
 * DeviceID/UniqueAddress pair of the client does NOT match the one in the head-end.
 *
 * For IP only STBs, the application should remind the user to try to register again in order to resolve this problem.
 * The re-try action can be done via ::UniversalClient_IPTV_RetryRequest.
 * The application should ignore this error message if the status of the currently playing elementary stream is ::ERR_MSG_D029, or the
 * currently playing elementary stream is clear.
 * The application should remove this error message if it was already displayed on the screen when the status of the currently playing elementary 
 * streams turned to ::ERR_MSG_D029, or the application switched to clear elementary streams. 
 */
#define ERR_MSG_E205        "E205-0  "

/**
 * Service Status Screen Text: the IFCP pairing AD mode mismatches with the ECM AD mode
 *
 * This message is sent when an ECM is rejected by the client because the AD mode of ECM mismatches with IFCP pairing AD mode.
 */
#define ERR_MSG_E208           "E208-0  "

/**
 * Service Status Screen Text: IFCP returns an error when there is an internal issue and it could not complete the broadcast descrambling process properly
 *
 * This message is sent when IFCP cannot finalize a broadcast descrambling service for 4K content. 
 * For more details, check the ::uc_service_status_st.errorCodeForSMP or  ::uc_extended_service_status.errorCodeForSMP.
 */
#define ERR_MSG_E209           "E209-0  "

/**
 * Service Status Screen Text: IFCP returns an error when there is a CUR enforcement failure
 *
 * This message is sent when IFCP decides the CUR is not up-to-date or as expected. 
 * For more details, check the ::uc_service_status_st.errorCodeForSMP or  ::uc_extended_service_status.errorCodeForSMP.
 */
#define ERR_MSG_E210           "E210-0  "

/**
 * Service Status Screen Text: Secure Media Pipeline is not supported in MSR mode.
 *
 * This message is sent when a Secure Media Pipeline ECM is received by an agent that is not switched to IFCP mode yet. 
 */
#define ERR_MSG_E211           "E211-0  "

/**
 * Service Status Screen Text: Proximity Detection Session Invalid.
 *
 * Proximity detection session is now invalid.
 */
#define ERR_MSG_E210_32        "E210-32 "

/**
 * Service Status Screen Text: Proximity Detection Communication Error.
 *
 * A communication error appears when performing proximity detection.
 */
#define ERR_MSG_E211_32        "E211-32 "

/**
 * Service Status Screen Text: Proximity Detection Message Timeout.
 *
 * Message response timeout when performing proximity detection.
 */
#define ERR_MSG_E212_32        "E212-32 "



/**
 * Service Status Screen Text: PIN code has already been used.
 *
 * PIN code already has been used. This message is sent when client register response is rejected by the CCIS because
 * the PIN code has been used already. It is introduced for anti-cloning to limit the PIN code, so that it can only be used once. The application will remind the user to 
 * contact with the operator to resolve the problem.
 * The application will ignore this error message if the status of the currently playing elementary streams is ::ERR_MSG_D029, or the 
 * currently playing elementary streams are clear ones.
 * The application will remove this error message if it was already displayed on the screen when the status of the currently playing elementary 
 * streams turned to ::ERR_MSG_D029, or the application switched to clear elementary streams. 
 */
#define ERR_MSG_E212        "E212-0  "

/**
 * Service Status Screen Text: CG-Private Data not paired.
 *
 * CG-Private Data not paired. This message is sent when a client cannot find the related private data for the current CG.
 * The application will remind the user to try to do the client registration again to resolve this problem.
 * The application will ignore this error message if the status of the currently playing elementary streams is ::ERR_MSG_D029, or the 
 * currently playing elementary streams are clear ones.
 * The application will remove this error message if it was already displayed on the screen when the status of the currently playing elementary 
 * streams turned to ::ERR_MSG_D029, or the application switched to clear elementary streams. 
 */
#define ERR_MSG_E213        "E213-0  "

/**
 * Service Status Screen Text: Proximity Detection Round Trip Timeout.
 *
 * The round trip time expired, as configured at the headend.
 */
#define ERR_MSG_E213_32        "E213-32 "

/**
 * Service Status Screen Text: Proximity Detection Session Expired.
 *
 * The proximity detection session expired - a new session is required.
 */
#define ERR_MSG_E214_32        "E214-32 "

/**
 * Service Status Screen Text: Proximity Detection Invalid Response.
 *
 * The proximity session failed due to an invalid response.
 */
#define ERR_MSG_E215_32        "E215-32 "

/**
 * Service Status Screen Text: Proximity Detection Root Key mismatch.
 *
 * The proximity session failed due to a root key mismatch.
 */
#define ERR_MSG_E216_32        "E216-32 "

/**
 * Service Status Screen Text: Proximity Detection TTL Check Failed.
 *
 * The proximity session failed due to a TTL check error.
 */
#define ERR_MSG_E217_32        "E217-32 "

/**
 * Service Status Screen Text: Please wait. HNA timer expired.
 *
 * Home Gateway Proximity Control HNA timer expired. No descrambling.
 */
#define ERR_MSG_E143             "E143-0  "


/* !! TODO new messages go here */

/** @} */ /* End of status_messages */

/** @defgroup servicetlvtags Cloaked CA Agent TLV Tags for Service
 *  List of all possible TLV (Tag, Length, Value) values to config service.
 *  These tags are used to carry specific parameters.
 *  Each tag is with different payload format and has its own meaning.
 *  TLV parameter is used for some advanced features.
 *  No need to call this API unless the application is using these advanced features.
 *  All TLV parameter have 1 byte tag, 2 bytes length, and N bytes value (payload).
 *  
 *  \note Data fields in TLV parameter are all in big-endian (network) order.
 *  @{
 */


/**
 * Tag: PVR record.
 *
 * This tag is used to send a PVR Record request to Cloaked CA Agent.
 * When the application is trying to make a new recording on a program, it must make a request to Cloaked CA Agent via this TLV.
 * If this TLV is processed successfully, Cloaked CA Agent will output a PVR session key via ::UniversalClientSPI_PVR_SetSessionKey or ::UniversalClientSPI_PVR_SetExtendedSessionKey,
 * together with one or more service messages of a PVR Session Metadata. The message type is uc_service_message_type::UC_SERVICE_PVR_SESSION_METADATA_REPLY. 
 * The PVR session key should be used to encrypt the descrambled content. No matter success or not, Cloaked CA Agent will send out the status via 
 * uc_service_message_type::UC_SERVICE_PVR_RECORD_STATUS_REPLY.
 * The TLV is in the following format:
 \code

 TLV_PVR_Record()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TLV_TAG_FOR_PVR_RECORD
     Length             2  bytes            0
     Data               0  byte            
 }
 \endcode

 \note Refer to \subpage pvrrecord and \subpage basicpvr_record. \n
 \note Refer to \subpage basicpvr_record_smp \n\n

 \note Sometimes, the device application wants to resume a previously stopped recording to form a single recording session.
 That means the newly started recording should keep to use the same recording key with the previous recordings. In this case, the device application must
 provide the first previously received metadata to Cloaked CA Agent using this TLV and must make sure the new recording is on the 
 same program as before. For example if it is VOD program, the two recordings must belong to the same VOD asset, if not, recording is
 prohibited. If this TLV is processed successfully, Cloaked CA Agent will output the PVR session key via ::UniversalClientSPI_PVR_SetSessionKey or ::UniversalClientSPI_PVR_SetExtendedSessionKey.
 This recording key is the same as the previous recordings. New PVR Meta data may be sent to device using uc_service_message_type::UC_SERVICE_PVR_SESSION_METADATA_REPLY. 
 No matter success or not, Cloaked CA Agent will send out the status via uc_service_message_type::UC_SERVICE_PVR_RECORD_STATUS_REPLY.
 In this case, the TLV format should be:
 \code

 TLV_PVR_Record()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TLV_TAG_FOR_PVR_RECORD
     Length             2  bytes            X
     Data               X  bytes            The first metadata of the first recording.
 }
 \endcode

 */
#define UC_TLV_TAG_FOR_PVR_RECORD    (0x00)


/**
 * Tag: Stop PVR record.
 *
 * This tag is used to send a stop PVR record request to Cloaked CA Agent.
 * When the application is trying to stop a recording on a program, it must make a request to Cloaked CA Agent via this TLV.
 * If this TLV is processed successfully, Cloaked CA Agent will stop the recording.
 * The TLV is in the following format:
 \code

 TLV_PVR_Stop_Record()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TLV_TAG_FOR_STOP_PVR_RECORD
     Length             2  bytes            0
 }
 \endcode

 \note Refer to \subpage pvrplayback and \subpage basicpvr_playback.
 
 */
#define UC_TLV_TAG_FOR_STOP_PVR_RECORD    (0x01)


/**
 * Tag: ECM connection handle list for PVR record
 *
 * \note This tag is rarely used.
 * This tag is used to send the ECM connection handles with in the service to Cloaked CA Agent.
 * Cloaked CA Agent will use these handles to judge the entitlement of the PVR record.
 \code

 TLV_ECM_Connection_Handle_List()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TLV_TAG_FOR_ECM_CONNECTION_HANDLE_LIST
     Length             2  bytes            n*4
     Connection Handles n*4 bytes           n ECM Connection Handles
 }
 \endcode
 */
#define UC_TLV_TAG_FOR_ECM_CONNECTION_HANDLE_LIST  (0x02)


/**
 * Tag: Monitor Switch.
 *
 * This tag is used to send a request to Cloaked CA Agent to enable or disable the monitor 
 * messages ::UC_SERVICE_ECM_MONITOR_STATUS, ::UC_SERVICE_EMM_MONITOR_STATUS,
 * ::UC_SERVICE_EMM_MONITOR_STATUS_FOR_CONVERGENT_CLIENT and ::UC_SERVICE_ECM_MONITOR_STATUS_FOR_CONVERGENT_CLIENT.
 * Monitor must be enabled upon displaying the Status Screen and disabled upon the exit of Status Screen.
 * The TLV is in the following format:
 \code

 TLV_Monitor_Switch()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TLV_TAG_FOR_MONITOR_SWITCH
     Length             2  bytes            0x01
     Switch Flag        1  byte             0x00/0x01 (0x00: Disable Monitor; 0x01: Enable Monitor)
 }
 \endcode
 */
#define UC_TLV_TAG_FOR_MONITOR_SWITCH (0x03)

/**
 * Tag: Trigger ECM reception
 *
 * This tag is used to send a request to Irdeto Cloaked CA Agent to trigger 
 * an ECM section reception immediately for a service 
 * to avoid the unnecessary waiting till next crypto period. 
 * Each time after a jump operation (for example, step-forward while playing VOD content ), 
 * the application should configure the service immediately 
 * by making a request to Unified Client via this tag. 
 * If this TLV is processed successfully, Irdeto Cloaked CA Agent will process 
 * the first coming ECM section anyway and do not care 
 * whether it is duplicated or new one compared with the previous ECM section.
 * The TLV is in the following format:
 \code

 TLV_TRIGGER_ECM_RECEPTION()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TLV_TAG_FOR_TRIGGER_ECM_RECEPTION
     Length             2  bytes            0
 }
 \endcode
 */
#define UC_TLV_TAG_FOR_TRIGGER_ECM_RECEPTION (0x04)

/**
 * Tag: ECM Filter Skip.
 *
 * For Cloaked CA Agent for Secure Chipset based STBs, this tag is used to request the Cloaked CA Agent to process all ECM sections and skip the ECM filters for a service. 
 * Skip the filter means every ECM notified to Cloaked CA Agent is processed.
 *
 * For Cloaked CA Agent for IP only STBs, this tag is used to send a request to Cloaked CA Agent to skip the ECM filters for a service.
 * When the application wants Cloaked CA Agent to skip the ECM filter and parse every input ECM section, it must make a request to Cloaked CA Agent via this TLV.
 * If this TLV is processed successfully, Cloaked CA Agent will skip the ECM filter for this service.
 *
 * The TLV is in the following format:
 \code

 TLV_ECM_FILTER_SKIP()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TLV_TAG_FOR_ECM_FILTER_SKIP
     Length             2  bytes            0
 }
 \endcode

 *  \note Refer to \subpage basic_emm_pulling and \subpage basic_ott.

 */
#define UC_TLV_TAG_FOR_ECM_FILTER_SKIP (0x05)


/**
 * Tag: Trick Mode Switch.
 *
 * This tag is used to set whether trick mode is enabled on a service.
 * If a service is trick mode enabled, the service can be recorded using PVR logic regardless of the copy control settings.
 * By default trick mode is disabled. 
 * The TLV is in the following format:
 \code

 TLV_Trick_Mode()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TLV_TAG_FOR_TRICK_MODE
     Length             2  bytes            1
     Trick_Mode         1  bytes            0/1 (0: Trick mode is disabled. 1: Trick mode is enabled)
 }
 \endcode

 \note If device needs to record a service using PVR in trick mode, this TLV must be submitted to Cloaked CA Agent together with the TLV UC_TLV_TAG_FOR_PVR_RECORD to
 trigger the recording. Otherwise, the recording may be failed due to the copy control restrictions.
 
 */
#define UC_TLV_TAG_FOR_TRICK_MODE    (0x06)


/**
 * Tag: Set the service to Proximity Detection Service.
 *
 * This tag is used to set the service to Proximity Detection Service.
 * If the application uses Proximity Detection, it shall open a service and set the service with this TLV.
 * The TLV is in the following format:
 \code

 TLV_Proximity_Detection()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TLV_TAG_FOR_PROXIMITY_DETECTION
     Length             2  bytes            0
 }
 \endcode

 \note Refer to \subpage proximity_detection_flow.
 
 */
#define UC_TLV_TAG_FOR_PROXIMITY_DETECTION    (0x07)


/* !! TODO new tags for service on here */

/** @} */ /* End of servicetlvtags */


/** @defgroup vodtlvtags Cloaked CA Agent TLV Tags for Pull VOD
 *  List of all possible TLV (Tag, Length, Value) values for Pull VOD feature.
 *  These tags are used to carry specific parameters.
 *  Each tag is with different payload format and has its own meaning.
 *  TLV parameter is used for some advanced features.
 *  No need to call this API unless the application is using these advanced features.
 *  All TLV parameter have 1 byte tag, 2 bytes length, and N bytes value (payload).
 *  Data fields in TLV parameter are all in big-endian (network) order.
 *  @{
 */

/**
 * Tag: VOD Nonce.
 *
 * This tag is used to request a client transaction data for \ref glossary_ppv_vod "PPV VOD" feature from Cloaked CA Agent.
 * When the application is trying to request a VOD session via ::UniversalClient_PPV_VOD_PrepareRequest, 
 * it must get a client transaction data include this TLV.
 * The TLV is in the following format:
 \code

 TLV_VOD_Nonce()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TAG_TYPE_VOD_NONCE
     Length             1  byte             16
     VODNonce           16 bytes            The 16 bytes hex data for the VOD nonce
 }
 \endcode
 */
#define UC_TAG_TYPE_VOD_NONCE           (0x00)

/**
 * Tag: Unique Address.
 *
 * This tag is used to request a client transaction data for \ref glossary_ppv_vod "PPV VOD" feature from Cloaked CA Agent.
 * When the application is trying to request a VOD session via ::UniversalClient_PPV_VOD_PrepareRequest, 
 * it must get a client transaction data include this TLV.
 * The TLV is in the following format:

 \code

 TLV_Unique_Address()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TAG_TYPE_UNIQUE_ADDRESS
     Length             1  byte             4
     Unique Address     4  bytes            The 4 bytes hex data for Unique Address.
 }
 \endcode
 */
#define UC_TAG_TYPE_UNIQUE_ADDRESS      (0x01)

/**
 * Tag: Client Version.
 *
 * This tag is used to request a client transaction data for \ref glossary_ppv_vod "PPV VOD" feature from Cloaked CA Agent.
 * When the application is trying to request a VOD session via ::UniversalClient_PPV_VOD_PrepareRequest, 
 * it must get a client transaction data include this TLV.
 * The TLV is in the following format:

 \code

 TLV_Client_Version()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TAG_TYPE_CLIENT_VERSION
     Length             1  byte             4
     ClientVersion      4  bytes            The 4 bytes hex data for client version, 
                                            for more information, see following Client_Version_Format().
 }
 \endcode
 

 * The Client Version is in the following format: 
 \code

 Client_Version_Format()
 {
     Field Name         Field Length        Field Description

     MAJOR (M)          4  bits             Major version number
     MINOR (m)          4  bits             Minor version number
     
     Release(R)         8  bits             Release number
     
     reserved(r)        4  bits             reserved 4 bits
     variant (v)        4  bits             Spatial Variant. Valid number is 1~10
     
     CG                 4  bits             Compound Generation
     SG                 4  bits             Stream Generation
 }
 \endcode 
 
 In general, 
 - \b MAJOR different major number have major functionality differences.
 - \b MINOR different minor number have minor functionality differences.
 - \b Release different release number include small bug-fixes.
 - \b reserved 4 bits for reserved.
 - \b variant different variant number have the same level of security but different implementations (EMMs are incompatible).
 - \b CG different CG have different security levels (ECMs and possibly also EMMs are incompatible).
 - \b SG different SG have different security levels (ECMs and possibly also EMMs are incompatible).
 */
#define UC_TAG_TYPE_CLIENT_VERSION      (0x02)

/* !! TODO new tags for VOD on here */

/** @} */ /* End of vodtlvtags */


/**
 * Base structure of component.
 *
 * This is the base structure of a component definition that is 
 * passed to ::UniversalClientSPI_Stream_AddComponent and ::UniversalClientSPI_Stream_RemoveComponent.
 *
 * 'Components' are used to identify recipients of descrambler keys in a later call to ::UniversalClientSPI_Stream_SetDescramblingKey.
 * When this structure is actually used, it will be as part of another structure with additional information,
 * with the 'size' parameter indicating the total size of the larger structure. 
 * 
 * \note Structures based on this structure must not contain pointers to non-static memory, since the structure
 *     is copied and used asynchronously. 
 * 
 * For example:
 * \code
 * // structure that 'inherits' from uc_component
 * typedef struct _uc_mydescrambler_component
 * {
 *   uc_component base;
 *   int extra_field;
 *   int another_field;
 * } uc_mydescrambler_component;
 * \endcode
 *
 */
typedef struct _uc_component
{
    /**
     * size of structure based on uc_component.
     *
     * This is the size, in bytes, of the structure that 'inherits' from this structure.
     * This should be set to (sizeof(uc_component) + additional data).  
     */
    uc_uint32 size;
} uc_component;

/**
 * Global message type enumeration.
 *
 * This type is used by the ::uc_global_message_proc callback provided by an application to
 * ::UniversalClient_StartCaClient. This indicates the type of message being sent to
 * the application. The application may choose to ignore messages, or 
 * it may perform additional processing based on the type of the message.
 */
typedef enum
{
    /** (0x00000000) Debug message type for ::uc_global_message_proc callback.
     * 
     * Debug messages are internal informational messages given to the application
     * in order to identify problems. They are provided in debug builds only-- a release
     * build will have all debug messages removed.
     *
     * For messages with this type, the \a lpVoid argument is a pointer to 
     * the NULL-terminated text of the debug message. 
     */
    UC_GLOBAL_DEBUG                         = 0x00000000, 

     /**
     * (0x00000001) Raw IRD message received.
     *
     * This message is replaced by ::UC_GLOBAL_EXTENDED_RAW_IRD_MESSAGE from
     * Cloaked CA Agent 2.9.0. It is reserved for drop-in integartion for devices which
     * have integrated 2.8.x and lower.
     *
     * This command means that a raw IRD message was delivered to the 
     * client that it did not understand. The actual execution of this command is
     * implementation-specific. This command may be used to send custom
     * commands to the client, like, text/mail/decoder control messages.
     *
     * Please refer to 705410 CCP IRD Messages for more detail information.
     *
     * For messages with this type, the \a lpVoid argument points to a ::uc_buffer_st
     * structure. uc_buffer_st::bytes points to a buffer containing the raw message,
     * uc_buffer_st::length is the length of the message.
     */
    UC_GLOBAL_RAW_IRD_MESSAGE               = 0x00000001,

    /**
     * (0x00000002) Serial Number Changed.
     *
     * This command means that a CAM EMM message was delivered to the client that 
     * cause the client to update the device Unique Address (Serial Number).
     *
     * For messages with this type, the \a lpVoid argument points to a ::uc_buffer_st
     * structure. uc_buffer_st::bytes points to a buffer containing new serial number,
     * uc_buffer_st::length is the length of the serial number (currently defined as 4 bytes).
     */
    UC_GLOBAL_SERIAL_NUMBER_CHANGED         = 0x00000002,
    
    /**
     * (0x00000003) Status in reply to an OOB EMM.
     *
     * This message indicates the result of OOB EMM processing.
     *
     * For messages with this type, the \a lpVoid argument points to 
     * a buffer containing an error code status string.
     * See \ref status_messages "Status Messages" for a list of 
     * possible values and what they mean.
     */
    UC_GLOBAL_OOB_EMM_STATUS_REPLY          = 0x00000003,

    /**
     * (0x00000004) Entitlement is changed.
     *
     * This message indicates that the entitlement is changed.
     *
     * For messages with this type, the \a lpVoid argument is NULL.
     * The application should use ::UniversalClient_GetProductList to check the changes.
     *
     */
    UC_GLOBAL_NOTIFY_ENTITLEMENT_CHANGE     = 0x00000004,

    /**
     * (0x00000005) ECM/EMM counter changed.
     *
     * This message indicates count of ECM sections or EMM sections processed by Cloaked CA Agent.
     *
     * For messages with this type, the \a lpVoid argument points to ::uc_ca_section_count structure.                                                                                   
     * The format of ::uc_ca_section_count is as follows:
     *
     \code
      typedef struct _uc_ca_section_count
      {
        uc_uint32 ecm_count;
        uc_uint32 emm_count; 
      }uc_ca_section_count;
     \endcode

        Explanation of the fields:
        - \b ecm_count: Count of ECM sections that have been processed.
        - \b emm_count: Count of EMM sections that have been processed.
     *
     * \note In order to save the system cost, the message will NOT be notified to the Application any more since Cloaked CA Agent v2.2.0.
     * The application can still use ::UniversalClient_GetEcmEmmCount to get the latest ECM/EMM section count.
     */
    UC_GLOBAL_NOTIFY_CASECTION_COUNT        = 0x00000005,

    /**
     * (0x00000006) Region information changed.
     *
     * This message notifies the latest region and subregion for a specific sector.
     *
     * For messages with this type, the \a lpVoid argument points to ::uc_region_information structure.                                                                                   
     *
     */
    UC_GLOBAL_NOTIFY_REGION_SUBREGION_INFO  = 0x00000006,

    /**
     * (0x00000007) TMS data changed.
     *
     * This message indicates that the TMS data is changed.
     *
     * For messages with this type, the \a lpVoid argument is NULL.
     * The application should use ::UniversalClient_GetTmsData to get the new
     * TMS data.
     *
     */
    UC_GLOBAL_NOTIFY_TMS_DATA_CHANGED       = 0x00000007,

    /**
     * (0x00000008) Nationality is changed.
     *
     * This message indicates that the nationality is changed.
     *
     * For messages with this type, the \a lpVoid argument is NULL
     * The application should use ::UniversalClient_GetNationality to check the change.
     *
     */
    UC_GLOBAL_NOTIFY_NATIONALITY_CHANGED     = 0x00000008,

    /**
     * (0x00000009) Homing Channel data is changed.
     *
     * This message indicates that the Homing Channel data is changed.
     *
     * For messages with this type, the \a lpVoid argument is NULL
     * The application should use ::UniversalClient_GetHomingChannelData to check the change.
     *
     */
    UC_GLOBAL_NOTIFY_HOMING_CHANNEL_DATA_CHANGED     = 0x00000009,

    /**
     * (0x0000000A) Middleware user data is changed.
     *
     * This message indicates that the Middleware user data is changed.
     * 
     * For messages with this type, the \a lpVoid argument is NULL
     * The application should use ::UniversalClient_GetMiddlewareUserData to check the change.
     */
    UC_GLOBAL_NOTIFY_MIDDLEWARE_USER_DATA_CHANGED     = 0x0000000A,

    /**
     * (0x0000000B) Reset to factory state.
     *
     * This message notifies the device application to reset the device to the factory state.
     * When Cloaked CA Agent receives the reset to factory EMM from head-end, the CA data in
     * persistent storage is cleared, then this message is sent to device application. The device
     * application shall erase necessary data to restore the device to factory state.
     * 
     * For messages with this type, the \a lpVoid argument point to uc_uint32 caSystemID.
     * \note For Cloaked CA Agent version 2.8.x and lower, this \a lpVoid is NULL.
     *
     * \note ::UniversalClient_ResetCAData shall not be called, as the CA data has already been cleared. 
     */
    UC_GLOBAL_NOTIFY_RESET_TO_FACTORY     = 0x0000000B,

    /**
     * (0x0000000C) CA data is cleared.
     *
     * This message notifies device application that the CA data has already been cleared by
     * Cloaked CA Agent. 
     * This message is just a notification and it is sent in two cases: When Cloaked CA Agent 
     * receives a reset to factory EMM from the headend or ::UniversalClient_ResetCAData is called 
     * by the device application.
     * 
     * For messages with this type, the \a lpVoid argument is NULL
     * \note Device application does not need to perform any actions on this message.
     */
    UC_GLOBAL_NOTIFY_CA_DATA_CLEARED     = 0x0000000C,

    /**
     * (0x0000000D) The Smart Card Status changed.
     *
     * This message notifies the change of the smart card status if smartcard has been activated.
     *
     * For messages with this type, the \a lpVoid argument points to ::uc_smartcard_status structure.
     */
    UC_GLOBAL_NOTIFY_SMARTCARD_STATUS    = 0x0000000D,

    /**
     * (0x0000000E) The smart card nationality changed.
     * This message notifies the change of the smartcard nationality.
     *
     * For messages with this type, the \a lpVoid argument points to NULL.
     * The application should use ::UniversalClient_GetSmartcardNationality to check the change.
     */
    UC_GLOBAL_NOTIFY_SMARTCARD_NATIONALITY_CHANGED = 0x0000000E,

     /**
     * (0x0000000F) The valid client changed.
     *
     * This message notifies that the valid client changed when the CAT updated or the card inserted or pulled out.
     *
     * For messages with this type, the \a lpVoid argument points to ::uc_valid_client_type value.
     */
    UC_GLOBAL_NOTIFY_VALID_CLIENT_CHANGED     = 0x0000000F,

     /**
     * (0x00000010) Smart Card Raw IRD message received.
     *
     * This command means that a smart card raw IRD message was delivered to the 
     * client that it did not understand. The actual execution of this command is
     * implementation-specific. This command may be used to send custom
     * commands to the client, like, text/mail/decoder control messages.
     *
     * Please refer to 705410 CCP IRD Messages for more detail information.
     *
     * For messages with this type, the \a lpVoid argument points to a ::uc_buffer_st
     * structure. uc_buffer_st::bytes points to a buffer containing the raw message,
     * uc_buffer_st::length is the length of the message.
     */
    UC_GLOBAL_SMARTCARD_RAW_IRD_MESSAGE     = 0x00000010,

    /**
     * (0x00000011) FlexiFlash information changed.
     *
     * This message indicates that FlexiFlash information has changed. The information contains the status of FlexiFlash
     * and the progress of Cloaked CA package downloaded, if either of these has changed, then this message type is triggered by the Agent.
     *
     * For messages with this type, the \a lpVoid argument points to ::uc_flexiflash_msg value.
     */
    UC_GLOBAL_NOTIFY_FLEXIFLASH_MESSAGE        = 0x00000011,
    
    /**
    * (0x00000012) Extended Raw IRD message received.
    *
    * This message is added from Cloaked CA Agent 2.9.0. It is a replace version of
    * ::UC_GLOBAL_RAW_IRD_MESSAGE. For devices integrated with version 2.9.0 or higher,
    * ::UC_GLOBAL_EXTENDED_RAW_IRD_MESSAGE shall be used.
    *
    * This command means that a raw IRD message from specifc operator was delivered to the 
    * client that it did not understand. The actual execution of this command is
    * implementation-specific. This command may be used to send custom
    * commands to the client, like, text/mail/decoder control messages.
    *
    * Please refer to 705410 CCP IRD Messages for more detail information.
    *
    * For messages with this type, the \a lpVoid argument points to a ::uc_raw_ird_msg
    * structure. uc_raw_ird_msg::caSystemID identifies which operator sends the message,
    * uc_raw_ird_msg::rawIrdMsg points to a buffer containing the raw message,
    * uc_raw_ird_msg::length is the length of the message.
    */
    UC_GLOBAL_EXTENDED_RAW_IRD_MESSAGE         = 0x00000012,

    /**
     * (0x00000013) The entitled status of any product entitlement changed.
     *
     * This message indicates that the entitled status of product entitlement changed, from FALSE to TRUE, or from TRUE to FALSE.
     *
     * For messages with this type, the \a lpVoid argument is uc_product_status.
     *
     */
    UC_GLOBAL_NOTIFY_ENTITLED_STATUS_CHANGE     = 0x00000013,

    /**
     * (0x00000014) Device Domain ID is changed.
     *
     * This message indicates that the Device Domain ID is changed.
     *
     * For messages with this type, the \a lpVoid argument is NULL
     * The application should use ::UniversalClient_GetHomeNetworkInfo to check the change.
     */
    UC_GLOBAL_NOTIFY_DEVICE_DOMAIN_ID_CHANGE    = 0x00000014,

    /**
     * (0x00000015) The number of Smart Card Sectors is changed.
     *
     * This message indicates that the number of smartcard sectors is changed.
     *
     * For messages with this type, the \a lpVoid argument is NULL
     * The application will use ::UniversalClient_GetSmartcardSectorsAddress to check the change.
     */
    UC_GLOBAL_NOTIFY_NUMBER_OF_SECTORS_CHANGE    = 0x00000015,

    /**
     * (0x00000016) CA Client requires manufacturer to display a message on screen.
     *
     * This message requires manufacturer to display a message on the screen. 
     * This message has no relationship with the Service Status Message. They can co-exist on the screen and do not impact each other. 
     *
     * For messages with this type, the \a lpVoid argument is a pointer to a ::uc_display_message. The manufacturer must follow the descriptions in ::uc_display_message to display the message.
     * 
     */
    UC_GLOBAL_NOTIFY_DISPLAY_MESSAGE_ON_SCREEN    = 0x00000016,

    /**
     * (0x00000017) SKE data changed.
     *
     * This message indicates that SKE data is changed.
     *
     * For messages with this type, the \a lpVoid argument is NULL
     * \note This is not used in current version of Cloaked CA Agent.
     */
    UC_GLOBAL_NOTIFY_SKELP_DATA_CHANGE    = 0x00000017,

    /**
     * (0x00000018) IFCP Image information changed.
     *
     * This message indicates that the IFCP image information changed. The information contains the status of IFCP Image
     * and the progress of IFCP Image downloaded, if either of these is changed, this message type will be triggered by the Agent.
     *
     * For messages with this type, the \a lpVoid argument points to ::uc_IFCP_image_msg value.
     */
    UC_GLOBAL_NOTIFY_IFCP_IMAGE_MESSAGE          = 0x00000018,

    /**
     * (0x00000019) All APIs are now available and can be called by the device application.
     *
     * This message indicates that the internal thread in Cloaked CA Agent has been initialized successfully and all APIs are now ready to be called.
     *
     * This message will be sent out immediately once the internal thread has finished the initialization. 
     *
     * Cloaked CA Agent has an internal thread which will do some initialization, 
     * for example, read the data from persistent storage and parse it.
     * When ::UniversalClient_StartCaClient returns, it does not mean that all APIs are ready to be called, 

     * especially the persistent storage related APIs, for example ::UniversalClient_GetProductList.
     * Takes the API ::UniversalClient_GetProductList for example, 
     * if the device application wants to get the product list right after ::UniversalClient_StartCaClient is called, 
     * the application must not call the API until it receives this notification message.
     *     
     * For messages with this type, the \a lpVoid argument points to NULL.
     */
    UC_GLOBAL_NOTIFY_API_AVAILABLE              = 0x00000019,

    /**
     * (0x0000001A) CA Client requires STB application to perform a specific action related to \ref glossary_fsu "FSU" as indicated in the payload data.
     *
     * After receiving this message, STB application must perform the specific action as indicated in the payload data. 
     * Refer to ::uc_fsu_action_type for more details about the actions.
     *
     * For messages with this type, the \a lpVoid argument is a pointer to a ::uc_fsu_action_param. 
     */
    UC_GLOBAL_NOTIFY_FOR_FSU_ACTION    = 0x0000001A,

    /**
     * (0x0000001B) The HGPC mode or parameters changes.
     *
     * This message indicates that the HGPC mode or parameters have changed. When this message is notified, Middleware can call
     * ::UniversalClient_GetHGPCInfo to get detail HGPC information.
     *
     * This message will be sent out immediately once the mode or parameters have changed. 
     *
     * For messages with this type, the \a lpVoid argument points to NULL.
     */
    UC_GLOBAL_NOTIFY_HGPC_UPDATED             = 0x0000001B,

    /**
     * (0x0000001C) The reminder to indicate how long the HNA message is to be expired.
     *
     * This message indicates that when the HNA message is to be expired on the HGPC secondary secure client.
     *
     * This message will be sent out each minute, once the application switches this notification on by
     * ::UniversalClient_SwitchHGPCReminder(uc_bool bSwitchOn)
     *
     * For messages with this type, the \a lpVoid argument points to uc_uint32. This uc_uint32 variable indicates the time left in seconds for a HNA message.
     * Middleware need not free this pointer.
     *
     * \note This message can be only enabled on HGPC secondary secure client.
     */
    UC_GLOBAL_NOTIFY_HGPC_REMINDER            = 0x0000001C,

    /**
     * (0x0000001D) The HGPC HNA message is created.
     *
     * This message indicates that the HNA message is refreshed and sent to the Middleware.
     *
     * This message will be sent out once the HNA message is refreshed and sent to the Middleware.
     *
     * For messages with this type, the \a lpVoid argument points to the ::uc_hgpc_status value. This uc_hgpc_status value indicates the result of the creation of an HNA message.
     * Middleware need not free this pointer.
     *
     * \note This message can be only sent out to the HGPC primary secure client.
     */
    UC_GLOBAL_NOTIFY_HGPC_CREATION             = 0x0000001D,

    /**
     * (0x0000001E) The HGPC HNA message is sent.
     *
     * This message indicates that the HNA message is sent repeatedly.
     *
     * This message will be sent out if the HNA message is repeatedly sent to the Middleware.
     *
     * For messages with this type, the \a lpVoid argument points to the ::uc_hgpc_status value. This uc_hgpc_status value indicates the result of the sending of an HNA message.
     * Middleware need not free this pointer.
     *
     * \note This message can be only sent out to the HGPC primary secure client.
     */
    UC_GLOBAL_NOTIFY_HGPC_SENDING              = 0x0000001E,

    /**
     * (0x0000001F) The HGPC HNA message is received.
     *
     * This message indicates that the HNA message is received.
     *
     * This message will be sent out once the HNA message is received by the Middleware.
     *
     * For messages with this type, the \a lpVoid argument points to the ::uc_hgpc_status value. This uc_hgpc_status value indicates the result of the reception of an HNA message.
     * Middleware need not free this pointer.
     *
     * \note This message can be only sent out to the HGPC secondary secure client.
     */
    UC_GLOBAL_NOTIFY_HGPC_RECEPTION            = 0x0000001F,

    /**
     * (0x00000020) The HGPC HNA message is expired.
     *
     * This message indicates that the HNA message is expired. 
     *
     * This message will be sent out when the HNA message is expired or the HNA message is not received after the secondary secure client is active. 
     *
     * The application must switch this notification on using ::UniversalClient_SwitchHGPCReminder(uc_bool bSwitchOn).
     *
     * For messages with this type, the \a lpVoid argument points to NULL.
     *
     * \note This message can be only enabled on the HGPC secondary secure client.
     */
    UC_GLOBAL_NOTIFY_HGPC_EXPIRY               = 0x00000020
    
} uc_global_message_type;

/**
 * Byte buffer structure
 *
 * This structure is used for passing arbitrary blocks of memory to the Cloaked CA Agent API.
 * 
 * The caller sets the \a bytes member to point to a 
 * buffer, and sets the \a length member to be the length of the buffer.
 * 
 * When used as an output parameter, the function being called will copy data up to
 * the length of the buffer into the memory specified by \a bytes. When used as an
 * input parameter, the function leaves the contents of the buffer alone. 
 */
typedef struct _uc_buffer_st
{
    /**
     * Pointer to a valid region of memory. Depending on the operation, this
     * may be a block of memory to write into or to read from.
     */
    uc_byte *bytes;

    /**
     * Length of the buffer. Depending on the operation, the number of bytes to 
     * read to or write from the \a bytes member.
     */
    uc_uint32 length;
} uc_buffer_st;

/**
* ::uc_bytes now is redefined to ::uc_buffer_st in order to remove the confusion between ::uc_bytes and ::uc_byte.
* The existing integration shall not be affected. 
*/
#define uc_bytes uc_buffer_st

/**
 * simple boolean type.
 */
#define uc_bool int

/**
 * The false value of ::uc_bool
 */
#define UC_FALSE 0

/**
 * The true value of ::uc_bool
 */
#define UC_TRUE 1

/**
 * Proximity status type enumeration.
 *
 * This type is used ...
 */
typedef enum
{
    /** (0x00000000) Proximity session is active and valid. */
    UC_PROXIMITY_STATUS_ACTIVE              = 0x00000000,

    /** (0x00000001) Proximity session is now invalid. */
    UC_PROXIMITY_STATUS_INVALID             = 0x00000001,

    /** (0x00000002) Proximity session is in the middle of being negotiated. */
    UC_PROXIMITY_STATUS_NEGOTIATING         = 0x00000002,

    /** (0x00000003) A communication error prevented the proximity session from being established. */
    UC_PROXIMITY_STATUS_COMM_ERROR          = 0x00000003,

    /** (0x00000004) A RTT timeout error prevented the proximity session from being established. */
    UC_PROXIMITY_STATUS_RTT_TIMEOUT         = 0x00000004,

    /** (0x00000005) The maximum message timeout was reached when waiting for a response. */
    UC_PROXIMITY_STATUS_MESSAGE_TIMEOUT     = 0x00000005,

    /** (0x00000006) The client was providing invalid challenge responses. */
    UC_PROXIMITY_STATUS_INVALID_RESPONSE    = 0x00000006,

    /** (0x00000007) The root key version is not compatible (different version numbers). */
    UC_PROXIMITY_STATUS_NOT_COMPATIBLE_ROOT = 0x00000007,

    /** (0x00000008) Proximity session is expired. */
    UC_PROXIMITY_STATUS_EXPIRED             = 0x00000008,

    /** (0x00000009) The TTL check failed, which prevented the proximity session from being established. */
    UC_PROXIMITY_STATUS_TTL_CHECK_FAILED    = 0x00000009

} uc_proximity_status_code;

/**
 * Peer device structure
 *
 * This structure is used to identify a peer device for proximity detection.
 */
typedef struct _uc_proximity_peer
{
    /**
     * indicates the peer ID of the peer device.
     * peer ID is assigned by the application.
     */
    uc_uint32 peerID;

} uc_proximity_peer;

/** 
 * Handle to a connection object 
 *
 * A 'connection' is a logical construct that represents the point at which CA
 * functionality is connected to a stream implementation. The connection object
 * interacts with a stream implementation to set filters (::uc_filter_handle) 
 * and receive ECMs and EMMs. After processing ECMs, keys are passed
 * to the stream implementation for descrambling the content. 
 * 
 * A connection handle is passed to an instance of a stream via ::UniversalClientSPI_Stream_Connect. 
 *
 */
typedef uc_uintptr uc_connection_handle;   

/**
 * Notify callback additional information structure
 *
 * This structure is used for passing the additional information to Cloaked CA when the notify callback function is invoked. 
 */
typedef struct _uc_notify_callback_info
{
    /**
     * Related request ID from ::UniversalClientSPI_Stream_Send method.
     *
     * It is used for web service response for Hybrid Client or IP only STBs, when connection stream type is ::UC_CONNECTION_STREAM_TYPE_IP.
     *
     * It is useless if ::UniversalClientSPI_Stream_Send method is not invoked.
     */
    uc_uint32 requestId;

    /**
     * Pointer to the associate ID buffer
     *
     * The associate ID buffer is created and managed by SPI layer. Cloaked CA Agent only passes it through after the ECM is processed, 
     * and does not manipulate the memory of this pointer.
     *
     * See also \ref vod_playback_flow "VOD Playback Flow - Out-Of-Band ECM"
     *
     * It is only used by Cloaked CA Agent for Secure Chipset based STBs.
     */
    void *pAssociateID;

    /**
     * Indicates a peer device
     * 
     * It is used for Proximity Detection, when the connection stream type is ::UC_CONNECTION_STREAM_TYPE_PD.
     */
    uc_proximity_peer peer;

    /**
     * Indicates the TTL value in the IP packet.
     *
     * It is used for Proximity Detection, when the connection stream type is ::UC_CONNECTION_STREAM_TYPE_PD.
     * This field must be the same as the TTL field in the IP packet of PD message received from a peer.
     */
    uc_uint32 ttl;

} uc_notify_callback_info;

/**
 * Function pointer type used to notify the client of new ECM or EMM sections.
 *
 * A function pointer of this type is passed to the implementation of a stream via the ::UniversalClientSPI_Stream_Connect
 * method. The stream implementation is expected to deliver new ECM or EMM sections that match the currently active set
 * of filters (see ::UniversalClientSPI_Stream_OpenFilter) to this function, 
 * after a call to ::UniversalClientSPI_Stream_Start and until a call to ::UniversalClientSPI_Stream_Disconnect occurs. 
 *
 * \note Implementations that receive one section at a time can safely pass the data directly, whereas implementations that receive blocks of sections 
 *     can also safely pass the data directly without having to parse the sections and break it into individual section.
 *
 * @param[in] connectionHandle The value of the connection handle previously passed to the stream implementation's 
 *     ::UniversalClientSPI_Stream_Connect method. 
 * @param[in] pSections Containing the MPEG-2 sections to process.
 *     The uc_buffer_st::bytes member must point to a buffer containing the raw data of 
 *     the sections, and the uc_buffer_st::length member must be set to the number of bytes
 *     in the sections.
 */
typedef void (*uc_notify_callback)(
    uc_connection_handle connectionHandle,
    const uc_buffer_st * pSections);

/**
 * Function pointer type used to notify the client of data(ECM sections or web service response or proximity detection message) with additional information. 
 * 
 * A function pointer of this type is passed to the implementation of a stream via the ::UniversalClientSPI_Stream_Extended_Connect 
 * method. The stream implementation is expected to deliver data(ECM sections or web service response or proximity detection message) that match the currently active set 
 * of filters (see ::UniversalClientSPI_Stream_OpenFilter) to this function, 
 * after a call to ::UniversalClientSPI_Stream_Start and until a call to ::UniversalClientSPI_Stream_Disconnect occurs.
 * 
 * \note For ECM sections which are contained in the index files, it is required that all ECM sections in one crypto period 
 * are delivered in one call. For ECM sections which are contained in the transport streams, the ECM sections can be sent 
 * via this interface one by one.
 * 
 * See also \ref vod_playback_flow "VOD Playback Flow - Out-Of-Band ECM"
 *
 * @param[in] connectionHandle The value of the connection handle previously passed to the stream implementation's 
 *     ::UniversalClientSPI_Stream_Connect method. 
 * @param[in] pData Containing data(ECM sections or web service response or proximity detection message) to process.
 *     The uc_buffer_st::bytes member must point to a buffer containing the raw data of 
 *     ECM sections or web service response or proximity detection message, and the uc_buffer_st::length member must be set to the number of bytes
 *     in ECM sections or web service response or proximity detection message.
 * @param[in] pNotifyCallbackInfo Containing the association information and request ID of the delivered pData. For ECM sections delivery, 
 *     the pAssociateID data field of the structure is passed out in the ECM status notification after the ECM sections 
 *     are processed by Cloaked CA Agent. For web service response, the request ID is from ::UniversalClientSPI_Stream_Send method.
 *     For proximity detection message, the peer is used to indicate the peer device that sends the message.
 */
typedef void(* uc_notify_callback_extended)(
    uc_connection_handle connectionHandle, 
    const uc_buffer_st *pData,
    uc_notify_callback_info *pNotifyCallbackInfo);

/**
 * Handle to a filter object 
 * 
 * A filter handle is an implementation specific value that is allocated by the implementation
 * of a stream object when the ::UniversalClientSPI_Stream_OpenFilter method is called on a connected stream.
 * While the filter is active, the stream implementation is expected to deliver ECM or EMM sections 
 * that arrive on the stream that match the filter criteria. 
 */
typedef uc_uintptr uc_filter_handle;       

/**
 * Filter definition
 *
 * This structure contains the definition of a filter for ECM and EMM sections
 * that are sent to the client for processing. This structure is passed to
 * ::UniversalClientSPI_Stream_OpenFilter to notify a stream implementation of the pattern
 * of sections it should deliver on the connection. 
 *
 * When a section arrives, the driver should match the first filterDepth 
 * bytes of the section against the filter pattern. 
 * The filter pattern is defined as a 'mask', which specifies which bits are relevant
 * for the comparison, and a 'match', which specifies the value the corresponding bits
 * should have in order to have successfully matched the pattern. 
 *
 * Here is some sample code for performing the comparison to illustrate how it is used: 
 *
 * \code
 *
 * uc_filter *pFilter = ...; // filter definition previously passed to ::UniversalClientSPI_Stream_OpenFilter. 
 * uc_byte *pSection = ...; // contents of MPEG-2 section read from hardware
 *
 * uc_bool bMatch = UC_TRUE;
 *
 * for (i=0; bMatch && i<pFilter->filterDepth; ++i)
 * {
 *     if ((pSection[i] & pFilter->mask[i]) != pFilter->match[i])
 *     {
 *         // section does NOT match  
 *         bMatch = UC_FALSE; 
 *     }
 * }
 * 
 * // ... at this point, use bMatch to determine if section should be passed to the 
 * // ::uc_notify_callback function. 
 * \endcode
 *
 * \note Integrators should be aware that filtering is an optional step intended for performance enhancement
 *     of the client and improved battery life for the device. Implementations that do not have access to 
 *     hardware-level filtering can safely ignore filters and pass on all sections received, and the client
 *     will still function correctly. 
 */
typedef struct _uc_filter {
    /**
     * Specifies which bits are important for the pattern match. 
     *
     * This points to a buffer whose length is determined by the \a filterDepth member.
     */
    uc_byte * mask;       

    /**
     * Specifies which bits to compare the masked bits against. 
     *
     * This points to a buffer whose length is determined by the \a filterDepth member.
     */
    uc_byte * match;

    /**
     * Specifies the number of bytes in the \a mask and \a match buffers
     *
     * A filterDepth of 0 means "match all sections". 
     */
    uc_uint32 filterDepth;
} uc_filter;


/**
 * Handle to a stream object 
 *
 * A stream handle is an implementation-specific type that is returned from
 * a call to ::UniversalClientSPI_Stream_Open. 
 * A stream object is a logical construct that represents a source of ECMs or EMMs,
 * and a destination for the corresponding descrambling keys. 
 */
typedef uc_uintptr uc_stream_handle;

/**
 * Connection or SPI stream type
 *
 * This type is used to indicate which type stream that will be opened when ::UniversalClientSPI_Stream_Open is called. 
 * The following are the stream types:\n
 * ::UC_CONNECTION_STREAM_TYPE_EMM \n
 * ::UC_CONNECTION_STREAM_TYPE_ECM \n
 * ::UC_CONNECTION_STREAM_TYPE_PVR_RECORD \n
 * ::UC_CONNECTION_STREAM_TYPE_PVR_PLAYBACK \n
 * ::UC_CONNECTION_STREAM_TYPE_FTA \n
 * ::UC_CONNECTION_STREAM_TYPE_IP \n
 * ::UC_CONNECTION_STREAM_TYPE_PD \n
 * ::UC_CONNECTION_STREAM_TYPE_RESERVED 
 */
typedef uc_sint32 uc_connection_stream_type;


/** @defgroup streamtype Connection and SPI Stream Type
 *  The SPI stream type is defined, and all of the types are listed. The stream type is
 *  also used as the Connection Type inside Cloaked CA Agent.
 *  @{
 */

/**
 * EMM connection or SPI stream type
 */
#define UC_CONNECTION_STREAM_TYPE_EMM 0

/**
 * ECM connection or SPI stream type
 */
#define UC_CONNECTION_STREAM_TYPE_ECM 1

/**
 * PVR record connection or SPI stream type
 */
#define UC_CONNECTION_STREAM_TYPE_PVR_RECORD 2

/**
 * PVR playback connection or SPI stream type
 */
#define UC_CONNECTION_STREAM_TYPE_PVR_PLAYBACK 3

/**
 * FTA connection or SPI stream type
 */
#define UC_CONNECTION_STREAM_TYPE_FTA 4

/**
 * IP connection or stream type
 */
#define UC_CONNECTION_STREAM_TYPE_IP 5

/**
 * Proximity Detection connection or stream type
 */
#define UC_CONNECTION_STREAM_TYPE_PD 6

/**
 * Reserved connection or stream type
 */
#define UC_CONNECTION_STREAM_TYPE_RESERVED 7



/** @} */ /* End of stream types */


/**
 * Source type.
 *
 * It is used to indicate that something is from Cloaked CA or Smart Card CA (SoftCell).
 * It is available for Unified Client only, which includes both Cloaked CA and Smart Card CA.
 */
typedef enum
{
    /** (0x00000000) Cloaked CA. 
      */
    UC_SOURCE_TYPE_CCA             = 0x00000000, 

     /**
      * (0x00000001) Smart Card CA.
      */
    UC_SOURCE_TYPE_SCA             = 0x00000001
    
} uc_global_source_type;

/**
 * Product type enumeration.
 *
 * This type is used by the ::UniversalClient_GetProductList to obtain the detailed product type.
 */
typedef enum
{
    /** (0x00000000) Normal Product. 
      */
    UC_PRODUCT_TYPE_NORMAL          = 0x00000000, 

     /**
      * (0x00000001) PVR Product.
      */
    UC_PRODUCT_TYPE_PVR             = 0x00000001,

    /**
      * (0x00000002) Push VOD Product.
      */
    UC_PRODUCT_TYPE_PUSH_VOD        = 0x00000002,

    /**
      * (0x00000003) Pre Enablement Product.
      */
    UC_PRODUCT_TYPE_PRE_ENABLEMENT  = 0x00000003,

    /**
      * (0x00000004) Pull VOD Product.
      */
    UC_PRODUCT_TYPE_PULL_VOD        = 0x00000004
    
} uc_global_product_type;

/**
 * Product status structure
 * 
 * This is the structure of each element in the list returned by a call to ::UniversalClient_GetProductList.
 * A 'product' is a unit of entitlement that gives the client the ability to descramble a service.
 * A service may be associated with one or more products. The head end is aware of this association
 * when it scrambles a service, but communicating this association to the device is outside of the scope of the
 * Cloaked CA Agent, so it us up to the application layer to determine the service that is associated
 * with a given product, using the product ID from inside this structure. 
 * 
 */
typedef struct _uc_product_status
{
    /**
     * Sector number that contains this product
     *
     * Some Cloaked CA Agents may support separate stores for client entitlements called 'sectors'. 
     * The use of sectors is operator-dependent. 
     */
    uc_byte sector_number;

    /**
     * Entitlement state of the product
     *
     * This value indicates whether the client is allowed to use this product or not. 
     * If this value is true, the client is entitled and can descramble any services that are associated
     * with this product. If this value is false, the client is not entitled and cannot descramble these
     * services.
     */
    uc_bool entitled; 

    /**
     * Unique product identifier
     *
     * This value uniquely identifies a product and is the big-endian representation 
     * of the 'ID' field for the corresponding product configuration in the CA server.
     * For example, if the product ID in the head end is 1000 decimal,
     * the product ID in this structure will be {0x03, 0xe8}.
     * See ::UC_SIZE_PRODUCT_ID for its size.
     */
    uc_byte product_id[UC_SIZE_PRODUCT_ID];
 
    /**
     * The date from which the entitlement is valid.
     *
     * The date is a sequential counter that indicates the number of days elapsed since 1-1-2000
     * and is increased by 1 date code tick every midnight UTC.
     * startingDate must be converted to have a year/month/day to show on the screen.
     * \note Please see \ref dateconversionfunction "Date Conversion Function" for sample function to convert starting date for display purpose.
     */
    uc_uint16 startingDate;

    /**
     * How long the product entitlement is valid.
     *
     * The unit of duration is DAY.
     */
    uc_byte duration;

    /**
     * The time from which the entitlement is valid.
     *
     * The time is a sequential counter that indicates the number of seconds elapsed since 1-1-2000, 00:00:00 UTC
     * startingTimeInSeconds must be converted to have a year/month/day/hour/minute/second to show on the screen.
     * \note Please see \ref dateconversionfunction "Date Conversion Function" for sample function to convert starting time for display purpose.
     * Currently, it is only applicable for VOD or Push VOD product.
     */
    uc_uint32 startingTimeInSeconds;

    /**
     * How long the product entitlement is valid.
     *
     * The unit of durationInSeconds is second.
     * durationInSeconds must be converted to day/hour/minute/second to show on the screen.
     * Currently, it is only applicable for VOD or Push VOD product.
     */
    uc_uint32 durationInSeconds;

    /**
     * Product Type.
     */
    uc_global_product_type productType;

    /**
     * CA System ID
     */
    uc_uint32 caSystemID;

    /**
     * Purchase to own flag of the product.
     * 
     * Indicate if the VOD content scrambled by this product can be converted to PVR content.
     * Currently, it is only applicable for Push VOD product, and not mandatory to be shown on the screen.
     * 
     */
    uc_bool isPurchaseToOwn;

    /**
     * The ID of the VOD asset that is scrambled by this product.
     * 
     * Indicate the ID of the VOD asset that is scrambled by this product.
     * Currently, it is only applicable for Pull VOD product, when 2 or more VOD assets scrambled by the same product.
     * It is not mandatory to be shown on the screen.
     * 
     */
    uc_uint32 assetId;

    /**
     * The source type of the product.
     * 
     * Indicate the product is from Cloaked CA or Smart Card CA. It is only available for Unified Client.
     * 
     */
    uc_global_source_type sourceType;
    
}uc_product_status;

/**
 * Product identifier structure
 *
 * This structure is used to uniquely identify a product for ::UniversalClient_QueryAnyProductEntitled 
 */
typedef struct _uc_product_identifier
{
    /**
     * The ID of CA System that the product belongs to.
     */
    uc_uint32 caSystemID;

    /**
     * The product id.
     */
    uc_byte productID[UC_SIZE_PRODUCT_ID];

}uc_product_identifier;

/**
 * ECM/EMM Counter structure
 * 
 * This is the structure containing count of CA sections that have been processed by Cloaked CA Agent.
 * This structure is used by ::UC_GLOBAL_NOTIFY_CASECTION_COUNT and ::UniversalClient_GetEcmEmmCount.
 * 
 */
typedef struct _uc_ca_section_count
{
    /**
     * Count of ECM sections that have been processed
     *
     */
    uc_uint32 ecm_count;

    /**
     * Count of EMM sections that have been processed
     *
     */
    uc_uint32 emm_count; 

}uc_ca_section_count;

/**
 * Region information structure
 * 
 * This is the structure containing the latest region information for a specific sector. 
 * This structure is used by message ::UC_GLOBAL_NOTIFY_REGION_SUBREGION_INFO and ::UniversalClient_GetRegionInfoList.
 * 
 */

typedef struct _uc_region_information
{
    /**
     * Sector number containing this region information.
     * 
     */
    uc_byte sector_number;

    /**
     * Region information for this sector.
     *
     */
    uc_byte region; 

    /**
     * Subregion information for this sector.
     *
     */
    uc_byte subregion;

    /**
     * Ca system ID for this sector.
     *
     */
    uc_uint32 caSystemID;

}uc_region_information;

/**
 * Descrambling key algorithm type
 *
 * This type is part of the ::uc_key_info structure and is passed to ::UniversalClientSPI_Stream_SetDescramblingKey
 * after an ECM arrives on a connected stream and is processed by the client. 
 * It refers to the algorithm that the descrambler should use to descrambler the content using the descrambling key.
 *
 * \note If the scrambling descriptor (tag value = 0x65) is present in the PMT, the stream implementation uses the scrambling
 *       algorithm indicated in the scrambling descriptor; Otherwise, the stream implementation uses the scrambling algorithm
 *       indicated by the Cloaked CA.
 *       Refer to the ETSI EN 300468 for more details about the scrambling descriptor and scrambling mode definition. 
 */
typedef enum
{
    /** 
     * (0) Unknown or proprietary algorithm.
     * 
     * A descrambling key may arrive with this type if a proprietary scrambling solution is being used
     * that the client does not know about. In this case, it is the responsibility of the stream implementation
     * to select what descrambling algorithm to use.
     */
    UC_DK_ALGORITHM_UNKNOWN =0, 

    /**
     * (1) DVB Common Scrambling Algorithm 
     *
     * A descrambling key may arrive with this type when the scrambling solution is known to be
     * DVB-CSA1/2. The stream implementation may use this information as it sees fit. 
     */
    UC_DK_ALGORITHM_DVB_CSA  =1,

    /**
     * (2) Advanced Encryption Standard in Reverse Cipher Block Chaining mode, 128 bit key.
     *
     * A descrambling key may arrive with this type when the scrambling solution is known to be
     * AES-128 RCBC or DVB-CSA3. The stream implementation may use this information as it sees fit.
     */
    UC_DK_ALGORITHM_AES_128_RCBC = 2,

    /**
     * (3) Advanced Encryption Standard in Cipher Block Chaining mode, 128 bit key.
     *
     * A descrambling key may arrive with this type when the scrambling solution is known to be
     * AES-128 CBC or DVB-CISSA. The stream implementation may use this information as it sees fit.
     */
    UC_DK_ALGORITHM_AES_128_CBC = 3,

    /**
     * (4) Multi-2 Scrambling mode, 64 bit key.
     */
    UC_DK_ALGORITHM_MULTI2 = 4

} uc_dk_algorithm;

/**
 * Up to Cloaked CA Agent release 2.2.1, the enum is only used to indicate descrambling key protection mechanism.
 * This type is part of the ::uc_key_info structure and is passed to ::UniversalClientSPI_Stream_SetDescramblingKey,
 * after an ECM arrives on a connected stream and is processed by the client. 
 * It refers to the method used to protect the descrambling key in transit.
 * 
 * After 2.2.1 is released, Cloaked CA Agent expects to support different key ladder providing additional key protection
 * choices, especially the AES algorithm. 
 * So, the enum is not only used in descrambling key protection but also in the protection of CSSK and PVRSK.
 *
 * For CSSK protection, this type is part of the ::uc_cssk_info structure and is passed to ::UniversalClientSPI_Device_SetExtendedCSSK
 * after a Pairing EMM arrives on a connected stream and is processed by the client.
 * The type is also passed to the client through UniversalClient_SetExtendedTestingKey as a part of ::uc_cssk_info structure,
 * when the client is in Testing mode.
 * It refers to the method used to protect the CSSK in transit.
 *
 * For PVRSK protection, this type is part of the ::uc_pvrsk_info structure and is passed to ::UniversalClientSPI_PVR_SetExtendedSessionKey,
 * It refers to the method used to protect the PVRSK in transit.
 *
 * For descrambling key protection, the method of using the enum has not changed
 */
typedef enum
{
    /**
     * (0) Unknown or proprietary protection algorithm
     *
     * Up to Cloaked CA Agent release 2.2.1, this protection type means that the descrambling key is being sent to the 
     * descrambler using a method that the client has no knowledge of.
     * In this case, it is the responsibility of the stream implementation to select which protection
     * algorithm to use.
     *
     * After 2.2.1 was released, this protection type means the key is protected by a unknown algorithm.
     */
    UC_DK_PROTECTION_UNKNOWN =0,

    /**
     * (1) Triple-DES encrypted 
     *
     * Up to Cloaked CA Agent release 2.2.1, this protection type means that the descrambling key is triple-DES encrypted
     * with the hardware or descrambler-specific session key. Descrambling keys are typically delivered with this type 
     * to ensure that they cannot be shared with another instance of a descrambler. 
     * This is intended to protect against a 'control word sharing' attack on the CA system.
     *
     * After 2.2.1 was released, this protection type means key is protected by triple-DES algorithm.
     */
    UC_DK_PROTECTION_TDES =1,

    /**
     * (2) Not encrypted
     *
     * Up to Cloaked CA Agent release 2.2.1, this protection type means that the descrambling key is not encrypted and can be
     * directly used to descramble content. Descrambling keys are delivered with this
     * type when a service is set in \b 911 \b mode or the current client is has a Security ID anchor.
     * \b 911 \b mode is a special mode that a service
     * can be placed in to bypass the regular CA system during emergencies or 
     * other appropriate situations. 
     *
     * After 2.2.1 was released, this protection type means the key is not encrypted.
     */ 
     UC_DK_PROTECTION_CLEAR =2,

    /**
     * (3) AES encrypted
     *
     * The enum value is added after 2.2.1 released, this protection type means key is protected by AES algorithm.
     * This is necessary for the AES key ladder supporting.
     */ 
     UC_DK_PROTECTION_AES =3,

     /**
     * (4) AES encrypted and transformed
     *
     * The enum value is added after 4.8.0 released. This protection type means the key is protected by the AES algorithm and transformed.
     * This is necessary for the AES key ladder when supporting transformation.
     */ 
     UC_DK_PROTECTION_AES_TRANSFORM = 4

} uc_dk_protection;

/**
 * Secure chipset pairing mode
 *
 * This type is part of the ::uc_testing_key_info structure and passed to ::UniversalClient_SetTestingKeyForMultipleMode
 * It indicates which pairing mode the secure chipset works in.
 * MSR (Minimal Security Requirement) is the standard key ladder that is well-known by STB manufacturers, supported by all Irdeto secure chipsets.
 * IFCP (Irdeto FlexiCore Processor) is introduced and supported by some Irdeto secure chipset models. 
 * And IFCP mode is supported from Cloaked CA Agent 4.2.0.
 */
typedef enum
{
    /**
     * Secure chipset pairing mode MSR.
     */
    UC_SECURECHIP_PAIRING_MODE_MSR = 0,

    /**
     * Secure chipset pairing mode IFCP. 
     */
    UC_SECURECHIP_PAIRING_MODE_IFCP = 1

} uc_securechip_pairing_mode;

/**
 * Key info structure
 *
 * This is a structure that is passed to a stream implementation from 
 * a call to ::UniversalClientSPI_Stream_SetDescramblingKey. It contains information about
 * a particular descrambling key. 
 */
typedef struct _uc_key_info
{
    /**
     * The scrambling algorithm that this key will be used for.
     *
     * Typically a descrambler will only support one type of algorithm, so this data
     * is really used for validation purposes in practice. However, it can also be
     * used to select which algorithm to use when multiple algorithms are available.
     */
    uc_dk_algorithm descramblingKeyAlgorithm;

    /**
     * The protection method used to secure this key.
     *
     * For example, this may be used to tie a descrambling key to a unique instance
     * of a descrambler hardware to prevent the key from being useful in another
     * descrambling environment.
     * If descramblingKeyProtection is UC_DK_PROTECTION_CLEAR, whatever the secure chipset mode
     * is, using the pDescramblingKey.
     */ 
    uc_dk_protection descramblingKeyProtection;

    /**
     * The key material of the descrambling key.
     *
     * This contains the raw data of the key. How this key is protected is determined by the 
     * uc_key_info::descramblingKeyProtection member, and how the key will be used by the 
     * descrambler is determined by the uc_key_info::descramblingKeyAlgorithm member.
     * The uc_buffer_st::bytes member points to the raw data of the key, and 
     * the uc_buffer_st::length member is the number of bytes in the key.
     */
    uc_buffer_st *pDescramblingKey;
    
} uc_key_info;

/**
 * Indicates the type of key ladder.
 *
 * This type is used by ::uc_cssk_info to indicate the destination key ladder that CSSK should be set to. 
 */
typedef enum _uc_dk_ladder
{
    /**
     * The CW key ladder.
     */
    UC_DK_LADDER_CW = 0,
    /**
     * The PVR key ladder
     */
    UC_DK_LADDER_PVR = 1,
    /**
     * The PVR key ladder level 2
     * This is used only when 3 level key ladder is used. And the first level key ladder is identified by UC_DK_LADDER_PVR.
     * \note For 3 level PVR key ladder, two session keys will be set to device separately using UC_DK_LADDER_PVR and UC_DK_LADDER_PVR_L2.
     */
    UC_DK_LADDER_PVR_L2 = 2
    
} uc_dk_ladder;

/**
 * 
 * CSSK Key info structure 
 * 
 * The structure is added after release 2.2.1.
 * The introduction of this structure is to support different key ladder (AES key ladder, TDES key ladder; CW key ladder, PVR key ladder).
 * It contains: 1, Information about the method to protect CSSK; 2, Destination where CSSK should be set to.
 * The structure is used in these place:
 * 1, Pased to ::UniversalClientSPI_Device_SetExtendedCSSK.
 * 2, When client is in testing mode, pased to client through API ::UniversalClient_SetExtendedTestingKey.
 */
typedef struct _uc_cssk_info
{
    /**
     * Indicates whether the stream handle is valid
     */
     uc_bool    isStreamHandleValid;

    /**
     * The stream handle associated with this session (descrambing, PVR and etc). 
     */
    uc_stream_handle streamHandle;

    /**
     * The protection method used to secure CSSK.
     */ 
    uc_dk_protection KeyProtection;

    /**
     * The destination key ladder associated with the following CSSK.
     */
    uc_dk_ladder keyLadder;

    /**
     * The key material of the CSSK
     */ 
    uc_buffer_st *pCSSK;
    
} uc_cssk_info;


/**
 * 
 * PVRSK Key info structure 
 * 
 * The structure is added after release 2.2.1.
 * The introduction of this structure is to support different key ladders (AES key ladder, TDES key ladder).
 * It contains information about the method to protect the PVRSK. 
 * The structure is passed to ::UniversalClientSPI_PVR_SetExtendedSessionKey, and indicates the PVRSK protection method.
 */
typedef struct _uc_pvrsk_info
{
    /**
     * The protection method used to secure PVRSK.
     */
    uc_dk_protection KeyProtection;

    /**
     * The key material of the PVRSK.
     */ 
    uc_buffer_st *pPVRSK;

    /**
     * The recommended algorithm used to encrypt the recorded content.
     * If the content was recorded using different algorithms, the device application 
     * must make sure the content can be decoded correctly especially for those old recorded
     * content without PVR key ladder support.
     */
    uc_dk_algorithm algorithm;

} uc_pvrsk_info;

/**
 * Indicates the type of key ladder for ::UniversalClient_SetTestingKeyForMultipleMode.
 *
 * This type is used by ::uc_testing_key_info to indicate the destination key ladder that CSSK should be set to. 
 */
typedef enum _uc_testing_key_ladder_type
{
    /**
     * The CW key ladder.
     */
    UC_TESTING_KEY_LADDER_CW = 0,
    /**
     * The OTT key ladder.
     */
    UC_TESTING_KEY_LADDER_OTT = 1

} uc_testing_key_ladder_type;

/**
 * 
 * Testing Key info structure 
 * 
 * The structure is passed to ::UniversalClient_SetTestingKeyForMultipleMode to set testing keys.
 *
 */
typedef struct _uc_testing_key_info
{
    /**
     * The scrambling algorithm that testing key will be used for.
     */
    uc_dk_algorithm descramblingKeyAlgorithm;

    /**
     * The secure chip pairing mode, indicating which key ladder will be tested.
     */
    uc_securechip_pairing_mode pairingMode;

    /**
     * The protection method used to secure testing key.
     */ 
    uc_dk_protection descramblingKeyProtection;

    /**
     * The CW or OTT key ladder level.
     * Currently, MSR supports 2 level key ladder, while IFCP supports 3 level key ladder.
     */ 
    uc_byte keyLadderLevelForDescramblingKey;

    /**
     * The key material of the odd descrambling key.
     * For CW key ladder, it is &lt;CW_ODD&gt; or the first part of &lt;CW&gt; in STB file.\n
     * For OTT key ladder, it is &lt;OTTK0&gt; in STB file.
     */
    uc_buffer_st *pOddDescramblingKey;

    /**
     * The key material of the even descrambling key.
     * For CW key ladder, it is &lt;CW_EVEN&gt; or the second part of &lt;CW&gt; in STB file.\n
     * For OTT key ladder, it is &lt;OTTK0&gt; in STB file.
     */
    uc_buffer_st *pEvenDescramblingKey;

    /**
     * The key material of the session key level-1.
     * For IFCP, it is &lt;K2&gt; in STB file.\n
     * For MSR, it is &lt;CK&gt; in STB file.
     */ 
    uc_buffer_st *pSessionKey_L1;

    /**
     * The key material of the session key level-2.
     * For IFCP:\n
     * &nbsp;&nbsp;- For CW key ladder, it is &lt;K1&gt; in STB file.\n
     * &nbsp;&nbsp;- For OTT key ladder, it is &lt;OTTK1&gt; in STB file.\n
     * For MSR, since the key ladder level is 2, this parameter will be ignored.
     */ 
    uc_buffer_st *pSessionKey_L2;

    /**
     * The key ladder type (CW or OTT).
     */ 
    uc_testing_key_ladder_type keyLadderType;

} uc_testing_key_info;

/**
 * Smart Card status structure
 *
 * This is the structure containing smartcard status information. 
 * This structure is used by ::UniversalClient_GetSmartcardStatus.
 */
typedef struct _uc_smartcard_status
{
    /** 
    * Smart Card id.
    * This is not used.
    */
    uc_uint32 smartcardId;

    /**
    * Status string.
    */
    uc_char status[UC_STATUS_LENGTH];
    
} uc_smartcard_status;


/**
 * Smart Card sector address per sector
 */
typedef struct _uc_smartcard_sector_address_per_sector
{
    /**
    * Sector Number.
    */
    uc_byte sectorNumber;

    /**
    * Sector active status.
    */
    uc_bool sectorActive;

    /**
    * Sector Address.
    */
    uc_byte sectorAddress[UC_SECTOR_ADDRESS_LENGTH];    
} uc_smartcard_sector_address_per_sector;


/**
 * Smart Card sectors address
 *
 * This structure is used by ::UniversalClient_GetSmartcardSectorsAddress.
 */
typedef struct _uc_smartcard_sectors_address
{
    /** 
    * Smart Card id.
    * This is not used.
    */
    uc_uint32 smartcardId;

    /**
    * Sectors count.
    */
    uc_byte sectorsCount;

    /**
    * Sectors address.
    * The count of sectors is identified by sectorsCount.
    */
    uc_smartcard_sector_address_per_sector sectorsAddress[UC_MAX_SECTOR_COUNT_PER_SMARTCARD];    
} uc_smartcard_sectors_address;

    
/**
 * Smart Card info structure
 *
 * This is the structure containing smartcard information. 
 * This structure is used by ::UniversalClient_GetSmartcardInfo.
 */
typedef struct _uc_smartcard_info
{
    /** 
    * Smart Card id. This is not used.
    */
    uc_uint32 smartcardId;
    
    /** 
    * Smart Card serial number string.
    */
    uc_char serialNumber[UC_MAX_SMARTCARD_SERIAL_NUMBER_LENGTH]; 
    
    /** 
    * Smart Card type.
    */
    uc_uint16 smartcardType;
    
    /** 
    * Major version.
    */
    uc_byte majorVersion;
    
    /** 
    * Minor version.
    */
    uc_byte minorVersion;
    
    /** 
    * Build infomation.
    */
    uc_byte build;
    
    /** 
    * Variant.
    */
    uc_byte variant;
    
    /** 
    * Patch level.
    */
    uc_uint16 patchLevel;
    
    /** 
    * Owner id.
    */
    uc_uint16 ownerId;
} uc_smartcard_info;

/**
 * DVB tuned information structure
 *
 * This is the structure containing the latest tuned information for current playing program. 
 * This structure is used by ::UniversalClient_SetTunedInfoList.
 */
typedef struct _uc_dvb_tuned_info
{
    /**
     * Service ID.
     */
    uc_uint16 serviceID;

    /**
     * Transport ID.
     */
    uc_uint16 transportID;

    /**
     * Network ID.
     */
    uc_uint16 networkID;
}uc_dvb_tuned_info;

/**
 * DVB tuned information structure
 *
 * This is the structure containing the TMS attribute information. 
 * This structure is used by ::UniversalClient_SetTMSAttributes.
 */
typedef struct _uc_tms_attribute_item_info
{
    /**
     * start position of the attribute in 32-bytes attributes bitmap.
     */
    uc_uint8 startbit;

    /**
     * length of current attribute length.
     */
    uc_uint8 length;

    /**
     * attribute value.
     */
    uc_uint32 value;
}uc_tms_attribute_item_info;

/**
 * Indicate the valid CA client.
 *
 * When Unified Client is integrated, the valid CA client may be changed because of the CAT update
 * or the card in/out. This type is used by ::UC_GLOBAL_NOTIFY_VALID_CLIENT_CHANGED to indicate the active CA client. 
 *
 */
typedef enum _uc_valid_client_type
{
    /**
     * No valid Irdeto CAS
     *
     */
    UC_CLIENT_UNDEFINED = 0,

    /**
     * Smart Card CA Client valid
     *
     */
    UC_SOFTCELL_VALID = 1,
    
    /**
     * Cloaked CA Client valid
     *
     */
    UC_CLOAKEDCA_VALID = 2,

    /**
     * Both Cloaked CA Client and Smart Card CA Client are valid
     *
     */
    UC_BOTH_VALID = 3
        
}uc_valid_client_type;

/**
 * FlexiFlash message structure
 *
 * This is the structure containing the status of the secure core and the progress of Cloaked CA package download.
 * This structure is used by the message ::UC_GLOBAL_NOTIFY_FLEXIFLASH_MESSAGE when FlexiFlash is enabled.
 */
typedef struct _uc_flexiflash_msg
{
    /**
     * Status of the secure core
     *
     * This is a string that indicates status of secure core.
     * The status will be defined as below: (if Secure Core version is 5.2.x or greater)\n
     * "(I%02d) %s M%02d m%02d mode%02d N%02d C%02x S%02x V%02x T%02x "\n
     * The status will be defined as below: (if Secure Core version is not equal or greater than 5.2.x)\n
     * "(I%02d) %s M%02d m%02d N%02d C%02x S%02x V%02x T%02x "\n
     * The following is an example:\n
     * "(I01) D500-0   M02 m06 mode01 N00 C01 S00 V01 T00 "\n
     * When a secure core was loaded unsuccessfully, Cloaked CA Agent will cannot obtain version information of the secure core, 
     * so will use "xx" to substitute the version information.\n
     * The following is an example:\n
     * "(I01) D500-0   Mxx mxx Nxx Cxx Sxx Vxx Txx "\n
     *
     * \note
     *      I: Area Index\n
     *      %s: string of error code, 8-byte length such as "D500-0  ", "E504-0  "\n
     *      M: Major\n
     *      m: Minor\n
     *      mode: Mode (Depends on if Secure Core version is 5.2.x or greater)\n
     *      N: Build Number\n
     *      C: CG\n
     *      S: SG\n
     *      V: Variant\n
     *      T: Anchor Type\n
     *
     */
    uc_string secureCoreListStatus;

    /**
     * Status of the Cloaked CA package downloaded
     *
     * When a Cloaked CA package downloading is triggered, this string indicates the progress of the downloading Cloaked CA package.
     * The status will be defined as below: (if Secure Core version is 5.2.x or greater)\n
     * "(I%02d) %3d%% M%02d m%02d mode%02d N%02d C%02x S%02x "\n 
     * The status will be defined as below: (if Secure Core version is not equal or greater than 5.2.x)\n
     * "(I%02d) %3d%% M%02d m%02d N%02d C%02x S%02x "\n
     * The following is an example:\n
     * "(I31)  80% M02 m06 mode01 N00 C01 S00 "\n
     * When no Cloaked CA package download is triggered, this string is empty.
     *
     * \note
     *      I: Area Index\n
     *      %3d: percentage of downloaded progress\n
     *      M: Major\n
     *      m: Minor\n
     *      mode : Mode (Depends on if Secure Core version is 5.2.x or greater)\n
     *      N: Build Number\n
     *      C: CG\n
     *      S: SG\n
     *
     */
    uc_string packagesDownloadProgressInfo;
}uc_flexiflash_msg;

/**
 * IFCP Image message structure
 *
 * This is the structure containing the status of the IFCP Image and the progress of IFCP Image download.
 * This structure is used by the ::UC_GLOBAL_NOTIFY_IFCP_IMAGE_MESSAGE message when FlexiFlash is enabled.
 */
typedef struct _uc_IFCP_image_msg
{
    /**
     * Status of the IFCP Image
     *
     * This is a string that indicates the status of the IFCP Image.
     * The status is defined as below:\n
     * "(I%02d) %s V%4d "\n
     * The following is an example:\n
     * "(I01) D550-0   V   1 "\n
     * When an IFCP Image is loaded unsuccessfully, Cloaked CA Agent will cannot obtain the version information of the IFCP Image, 
     * so it uses "xxxx" to substitute the version information.\n
     * The following is an example:\n
     * "(I01) E551-0   Vxxxx "\n
     *
     * \note
     *      I: Index, currently it is always 1\n
     *      %s: string of error code, 8-byte length such as "D550-0  ", "E551-0  "\n
     *      V: Version of image in decimal\n
     *
     */
    uc_string imageStatus;

    /**
     * Status of the IFCP Image downloaded
     *
     * When an IFCP Image download is triggered, this string indicates the progress of the IFCP Image download.
     * The status is defined as below:\n
     * "(I%02d) %3d%% V%4d "\n 
     * The following is an example:\n
     * "(I01) 80% V   2 "\n
     * When no IFCP Image download is triggered, this string is empty.
     *
     * \note
     *      I: Index, currently it is always 1\n
     *      %3d: percentage of downloaded progress\n
     *      V: Version of image in decimal\n
     *
     */
    uc_string packagesDownloadProgressInfo;
}uc_IFCP_image_msg;

/**
 * \ref glossary_fsu "FSU" action type enumeration.
 *
 * This stucture is used for ::uc_fsu_action_param to indicate what action is to be performed.
 * Also refer to \ref fsu_flow "FSU EMM Reception Flow" when implementing the FSU actions.
 */
typedef enum
{
    /**
     * The payload data of this action is ::uc_fsu_action_inform_user.
     *
     * STB application displays a message box according to the parameters in ::uc_fsu_action_inform_user.
     *
     * If ::uc_fsu_action_inform_user::forcedUpdate is UC_TRUE, there is one "OK" button on the message box,
     * and the message is as below:
     * "You will lose the current service after \b T seconds to receive CA data for security update process. 
     *  It will take about \b D seconds to receive the CA data.
     *  If you miss the data update this time, you will have to wait \b H:M:S for the next time to receive the CA data."
     *
     * If ::uc_fsu_action_inform_user::forcedUpdate is UC_FALSE, there are "OK" and "Cancel" buttons on the message box,
     * and the message is as below:
     * "You will lose the current service after \b T seconds to receive CA data for security update process. 
     *  It will take about \b D seconds to receive the CA data. 
     *  If you miss the data update this time, you will have to wait \b H:M:S for the next time to receive the CA data."
     *
     * In the message, \b T is ::uc_fsu_action_inform_user::durationBeforeTuningToFsuTransponderInSeconds,
     * and \b D is ::uc_fsu_action_inform_user::durationStayOnFsuTransponderInSeconds,
     * and \b H:M:S is converted from ::uc_fsu_action_inform_user::repetitionRateInSeconds.
     *
     * The message box shall be closed after displayed for 10 seconds, or when any button is clicked,
     * or when the message with action type = ::UC_FSU_DISPLAY_MESSAGE_AND_TUNE is received.
     *
     * \note If the device has multiple tuners and uses a free tuner to receive FSU EMMs, it shall not display this message box.
     */
     UC_FSU_INFORM_USER_TO_RECEIVE_FSU_DATA = 0,

    /**
     * The payload data of this action is ::uc_dvb_tuned_info.
     *
     * STB application displays a \b transparent message box with below message:
     *
     * "Device is receiving CA data for security update process. 
     * Please do not power off or perform any remote operations. 
     * Thanks for your cooperation."
     *
     * After displaying the message box, the STB Application must switch to the service on the transponder 
     * that is specified by the payload data in ::uc_dvb_tuned_info, and notify CAT by ::UniversalClient_DVB_NotifyCAT after tuning.
     *
     * There is no button on the message box. Any remote operations must be blocked until the message box is closed. 
     * The message box shall be closed when the message with action type = ::UC_FSU_DISPLAY_MESSAGE_AND_TUNE is received.
     *
     * \note If the current locked transponder happens to be the specified transponder, 
     *       it shall not switch service.
     *
     * \note If the device has multiple tuners and uses a free tuner to receive FSU EMMs, 
     *       it shall not switch service.
     */
     UC_FSU_DISPLAY_MESSAGE_AND_TUNE = 1,

    /**
     * The payload data of this action is ::uc_result.
     *
     * STB application displays a message box with below message according to the result provided in the payload data.
     *
     * If the result is ::UC_ERROR_SUCCESS, it displays "The CA data was successfully received!".
     * Otherwise, it displays "The CA data reception failed! Error Code = XXXXXXXX", where the error code is the result value.
     * The possible results are listed as below:
     *     ::UC_ERROR_SUCCESS, means the FSU EMMs were successfully received.
     *     ::UC_ERROR_FSU_STREAM_FAILED (0000008D), means the FSU EMMs were not completely received.
     *     ::UC_ERROR_MISSING_FSU_STREAM_PID_IN_CAT (0000008E), means the FSU CA System ID was not found in CAT.
     *     ::UC_ERROR_NO_CAT_NOTIFIED_FOR_FSU (0000008F), means the STB application did not notify CAT after tuning.
     *
     * There is only "OK" button on the message box. The message box shall be closed after displayed for 5 seconds,
     * or when button is clicked. After the message box is closed, STB application must switch back to the previous service, 
     * and notify CAT by ::UniversalClient_DVB_NotifyCAT after tuning back.
     *
     * \note If the service was not switched in action ::UC_FSU_DISPLAY_MESSAGE_AND_TUNE, then no switch back.
     */
     UC_FSU_REPORT_RESULT_AND_TUNE_BACK = 2
} uc_fsu_action_type;

/**
 * The data structure of \ref glossary_fsu "FSU" action to inform user.
 *
 * This stucture is used for ::UC_FSU_INFORM_USER_TO_RECEIVE_FSU_DATA.
 * The parameters are needed when STB application displays the message box to inform user about the CA data update (receiving FSU EMMs).
 */
typedef struct _uc_fsu_action_inform_user
{
    /**
     * Indicates whether the CA data update is forced.
     *
     * If forcedUpdate is ::UC_TRUE, there is only "OK" button on the message box, so user cannot cancel the CA data update.
     * If forcedUpdate is ::UC_FALSE, there are both "OK" and "Cancel" buttons on the message box, and user can cancel the CA data update.
     * If user clicks "Cancel" button, STB application must call ::UniversalClient_ConfigClient to notify Cloaked CA Agent.
     */
    uc_bool forcedUpdate;

    /**
     * Indicates the device will switch to FSU transponder after how much time (in seconds).
     * Refer to ::UC_FSU_INFORM_USER_TO_RECEIVE_FSU_DATA to know how to display this parameter.
     */
    uc_uint32 durationBeforeTuningToFsuTransponderInSeconds;

    /**
     * Indicates how long (in seconds) the device will stay on the FSU transponder to receive FSU EMMs.
     * Refer to ::UC_FSU_INFORM_USER_TO_RECEIVE_FSU_DATA to know how to display this parameter.
     */
    uc_uint32 durationStayOnFsuTransponderInSeconds;

    /**
     * Indicates the broadcasting period of the FSU EMMs.
     * If the user misses the CA data update once, the user has to wait for repetitionRateInMinutes for the CA data update again.
     * Refer to ::UC_FSU_INFORM_USER_TO_RECEIVE_FSU_DATA to know how to display this parameter.
     */
    uc_uint32 repetitionRateInSeconds;
} uc_fsu_action_inform_user;

/**
 * The data structure of \ref glossary_fsu "FSU" action.
 * 
 * This stucture is used for ::UC_GLOBAL_NOTIFY_FOR_FSU_ACTION
 */
typedef struct _uc_fsu_action_param
{
    /**
     * Indicates the operator who performs the FSU by the CA System ID.
     */
    uc_uint32 caSystemID;

    /**
     * Indicates what action the STB application shall perform.
     */
    uc_fsu_action_type actionType;

    /**
     * Parameters that are needed to perform the action.
     * If action type = ::UC_FSU_INFORM_USER_TO_RECEIVE_FSU_DATA, the pParamForAction points to ::uc_fsu_action_inform_user.
     * If action type = ::UC_FSU_DISPLAY_MESSAGE_AND_TUNE, the pParamForAction points to ::uc_dvb_tuned_info.
     * If action type = ::UC_FSU_REPORT_RESULT_AND_TUNE_BACK, the pParamForAction points to ::uc_result.
     */
    void *pParamForAction;
} uc_fsu_action_param;

/**
 * TMS data per operator.
 */
typedef struct _uc_tms_data_per_operator
{
    /**
     * identify the operator by its CA System ID
     */
    uc_uint32 caSystemID;

    /**
     * detail tms data for the operator which is identified by caSystemID
     */
    uc_byte tmsData[UC_TMS_USER_DATA_SIZE]; 
} uc_tms_data_per_operator;

/**
 * TMS data for Mutil-Secure core.
 *
 * This structure is passed to ::UniversalClient_Extended_GetTmsData
 */
typedef struct _uc_tms_data
{
    /**
     * the number of current valid operators
     */
    uc_uint32 validOperatorCount;
    
    /**
     * tms data for specific operator
     * The number of operators is identified by validOperatorCount.
     */
    uc_tms_data_per_operator tms[UC_MAX_OPERATOR_COUNT];
} uc_tms_data;

/**
 * regional info list per operator
 */
typedef struct _uc_regional_info_list_per_operator
{
    /**
     * the number of valid sectors
     */
    uc_uint32 validSectorCount;

    /**
     * the region and subregion info for the sector.
     * The number of sector is identified by validSectorCount.
     */
    uc_region_information regionInfoListPerOperator[MAX_ACTIVE_SECTOR_COUNT_PER_OPERATOR];
} uc_regional_info_list_per_operator;

/**
 * regional info list for multiple operators
 *
 * This structure is passed to ::UniversalClient_Extended_GetRegionInfoList
 */
typedef struct _uc_regional_info_list
{
    /**
     * the number of current valid operators
     */
    uc_uint32 validOperatorCount;
    
    /**
     * region and subregion information for specific operator
     * The number of operators is identified by validOperatorCount.
     */
    uc_regional_info_list_per_operator regionalInfoList[UC_MAX_OPERATOR_COUNT];
} uc_regional_info_list;

/**
 * version string for each secure core. The secure core is for one valid operator.
 */
typedef struct _uc_single_securecore_version
{
    /**
     * identify the operator by its CA System ID
     */
    uc_uint32 caSystemID;

    /**
     * detail version string for the secure core which is identified by the caSystemID.
     */
    uc_byte versionString[UC_MAX_VERSION_STRING_LENGTH];
} uc_single_securecore_version;

/**
 * secure core version string for multiple operators
 *
 * This structure is passed to ::UniversalClient_Extended_GetSecureCoreVersion
 */
typedef struct _uc_securecore_version
{
    /**
     * the number of current valid operators
     */
    uc_uint32 validOperatorCount;
    
    /**
     * securecore version for specific operator
     * The number of operators is identified by validOperatorCount.
     */
    uc_single_securecore_version version[UC_MAX_OPERATOR_COUNT];
} uc_securecore_version;

/**
 * serial number per operator
 */
typedef struct _uc_serial_number_per_operator
{
    /**
     * identify the operator by its CA System ID
     */
    uc_uint32 caSystemID;

    /**
     * detail serial number for the operator which is identified by the caSystemID.
     */
    uc_byte serialNumberBytes[UC_UNIQUE_ADDRESS_SIZE];
} uc_serial_number_per_operator;

/**
 * serial number information for multiple operators
 * This structure is passed to ::UniversalClient_Extended_GetSerialNumber
 */
typedef struct _uc_serial_number
{
    /**
     * the number of current valid operators
     */
    uc_uint32 validOperatorCount;
    
    /**
     * serial number for specific operator
     * The number of operators is identified by validOperatorCount
     */
    uc_serial_number_per_operator serialNumber[UC_MAX_OPERATOR_COUNT];
} uc_serial_number;

/**
 * The home network info of an operator
 */
typedef struct _uc_home_network_info_per_operator
{
    /** The operator ID.
     */
    uc_uint32 operatorID;

    /** Ca system ID
     */
    uc_uint32 caSystemID;

    /** The Device Domain ID.
     */
    uc_uint32 deviceDomainID;
    
} uc_home_network_info_per_operator;
 
/**
 * The current home network info
 */
typedef struct _uc_home_network_info
{
    /**
     * the number of current valid operators.
     * For each valid operator, its home network info is in networkInfo.
     * If validOperatorCount equals 0, there is no home network info.
     */
    uc_uint32 validOperatorCount;

    /** The network info of all operators.
     */
    uc_home_network_info_per_operator networkInfo[UC_MAX_OPERATOR_COUNT];
    
} uc_home_network_info;

/**
 * client ID per operator
 */
typedef struct _uc_client_id_per_operator
{
    /**
     * identify the operator by its CA System ID
     */
    uc_uint32 caSystemID;

    /**
     * detail client ID string for the operator which is identified by the caSystemID
     */
    uc_byte clientIDString[UC_MAX_ITEM_LENGH];
} uc_client_id_per_operator;

/**
 * client ID for multiple operators
 *
 * This strucure is passed to ::UniversalClient_Extended_GetClientIDString
 */
typedef struct _uc_client_id
{
    /**
     * the number of current valid operators
     */
    uc_uint32 validOperatorCount;
    
    /**
     * client ID for specific operator
     * The number of operators is identified by validOperatorCount.
     */
    uc_client_id_per_operator clientID[UC_MAX_OPERATOR_COUNT];
} uc_client_id;

/**
 * CA System ID for multiple operators
 *
 * This structure is passed to ::UniversalClient_Extended_ResetCAData
 */
typedef struct _uc_ca_system_id
{
    /**
     * the number of current valid operators
     *
     */
    uc_uint32 validOperatorCount;
    
    /**
     * the CA System IDs for valid operators.
     * The number of CA System ID is identified by validOperatorCount.
     *
     */
    uc_uint32 caSystemID[UC_MAX_OPERATOR_COUNT];
} uc_ca_system_id;

/**
 * operator information for single operator
 */
typedef struct _uc_single_operator
{
    /**
     * if the operator exists in real network
     */
    uc_bool active;

    /**
     * the valid caSystemID matches real network
     */
    uc_uint32 activeCaSystemID;

    /**
     * start value of the operator range
     */
    uc_uint32 caSystemIDStart;

    /**
     * end value of the operator range
     */
    uc_uint32 caSystemIDEnd;

    /**
     * operator name. A NULL-termined string.
     */
    uc_byte operatorName[UC_MAX_OPERATOR_NAME_LENGTH];
} uc_single_operator;

/**
 * operator information for multiple operators
 *
 * This structure is passed to ::UniversalClient_Extended_GetOperatorInfo
 */
typedef struct _uc_operator
{
    /**
     * the number of available operators defined in cfg file
     */
    uc_uint32 validOperatorCount;
    
    /**
     * operator information for specific operator
     * The number of supported operators is identified by validOperatorCount.
     */
    uc_single_operator operatorInfo[UC_MAX_OPERATOR_COUNT];
} uc_operator;

/**
 * nationality per operator
 */
typedef struct _uc_nationality_per_operator
{
    /**
     * identify the operator by its CA System ID
     */
    uc_uint32 caSystemID;

    /**
     * detail nationality for the operator which is identified by the caSystemID
     */
    uc_byte nationalityData[UC_NATIONALITY_SIZE];
} uc_nationality_per_operator;

/**
 * nationality for multiple operators
 *
 * This structure is passed to ::UniversalClient_Extended_GetNationality
 */
typedef struct _uc_nationality
{
    /**
     * the number of current valid operators
     */
    uc_uint32 validOperatorCount;
    
    /**
     * nationality for specific operator
     * The number of operators is identified by validOperatorCount.
     */
    uc_nationality_per_operator nationality[UC_MAX_OPERATOR_COUNT];
} uc_nationality;

/**
 * homing channel data per operator
 */
typedef struct _uc_homing_channel_per_operator
{
    /**
     * identify the operator by its CA System ID
     */
    uc_uint32 caSystemID;

    /**
     * detail homing channel data for the operator which is identified by the caSystemID
     */
    uc_byte homingChannelData[UC_HOMING_CHANNEL_USER_DATA_SIZE];
} uc_homing_channel_per_operator;

/**
 * homing channel data for multiple operators
 *
 * This structure is passed to ::UniversalClient_Extended_GetHomingChannelData
 */
typedef struct _uc_homing_channel
{
    /**
     * the number of current valid operators
     */
    uc_uint32 validOperatorCount;
    
    /**
     * homing channel information for specific operator
     * The number of operators is identified by validOperatorCount.
     */
    uc_homing_channel_per_operator homingChannel[UC_MAX_OPERATOR_COUNT];
} uc_homing_channel;

/**
 * middleware user data per operator
 */
typedef struct _uc_middleware_user_data_per_operator
{
    /**
     * identify the operator by its CA System ID
     */
    uc_uint32 caSystemID;

    /**
     * detail middleware user data for the operator which is identified by the caSystemID
     */
    uc_byte middlewareUserData[UC_MW_DATA_USER_DATA_SIZE];
} uc_middleware_user_data_per_operator;

/**
 * middleware user data for multiple operators
 *
 * This structure is passed to ::UniversalClient_Extended_GetMiddlewareUserData
 */
typedef struct _uc_middleware_user_data
{
    /**
     * the number of current valid operators
     */
    uc_uint32 validOperatorCount;
    
    /**
     * middleware user data for specific operator
     * The number of operators is identified by validOperatorCount.
     */
    uc_middleware_user_data_per_operator middlewareUserData[UC_MAX_OPERATOR_COUNT];
} uc_middleware_user_data;

/**
 * ecm & emm count per operator
 */
typedef struct _uc_ca_section_count_per_operator
{
    /**
     * identify the operator by its CA System ID
     */
    uc_uint32 caSystemID;

    /**
     * ecm count for the operator which is identified by caSystemID
     */
    uc_uint32 ecm_count;

    /**
     * emm count for the operator which is identified by caSystemID
     */
    uc_uint32 emm_count;
} uc_ca_section_count_per_operator;

/**
 * ecm and emm count for multiple operators
 *
 * This structure is passed to ::UniversalClient_Extended_GetEcmEmmCount
 */
typedef struct _uc_ca_extended_section_count
{
    /**
     * the number of current valid operators
     */
    uc_uint32 validOperatorCount;
    
    /**
     * section count for specific operator
     * The number of operators is identified by validOperatorCount
     */
    uc_ca_section_count_per_operator caSectionCount[UC_MAX_OPERATOR_COUNT];
} uc_ca_extended_section_count;

/**
 * Extended raw ird message structure
 *
 * This structure is extended for supporting multiple operators by identifing ca system id
 * for the operator who sends the raw ird message.
 *
 * This structure is used by global message UC_GLOBAL_EXTENDED_RAW_IRD_MESSAGE. 
 */
typedef struct _uc_raw_ird_msg
{
    /**
     * identify the operator by its CA System ID
     */
    uc_uint32 caSystemID;

    /**
     * the length of raw IRD message
     */
    uc_uint32 length;

    /**
     * raw IRD message
     */
    uc_byte *rawIrdMsg;
} uc_raw_ird_msg;

/**
 * Indicates the data format.
 *
 * This type is used by ::uc_oob_emm_data to indicate the data format of the OOB EMM data. 
 */
typedef enum _uc_data_format
{
    /**
     * Default format. The original binary data without any transformation or encryption.
     */
    UC_DATA_FORMAT_DEFAULT,

    /**
     * BASE64 encoding format.
     */
    UC_DATA_FORMAT_BASE64
    
} uc_data_format;

/**
 * OOB (Out of Band) EMM data structure
 *
 * This structure is used to deliver OOB EMM data for ::UniversalClient_Extended_SendOobEmmSection.
 */
typedef struct _uc_oob_emm_data
{
    /**
     * indicate the format of the following OOB EMM data
     */
    uc_data_format format;

    /**
     * OOB EMM data
     */
    uc_buffer_st data;
} uc_oob_emm_data;

/**
 * Display message type enumeration.
 *
 * This type is used for ::uc_display_message.
 * It indicates what message shall be displayed on the screen.
 * This message has no relationship with Service Status Message. They can co-exist on screen and do not impact each other.
 * The recommended message content is provided in English. Manufacturer can re-write the message as long as keeping the same meaning, and shall translate them into other languages if necessary.
 */
typedef enum
{
    /** (0x00000000) Stop displaying the message if any.
    * 
    * This value requires manufacturer to erase the message displayed on screen, if any.
    * Note that it does not affect the service status message display (e.g. E016), but only affect the message display defined in this enumeration.
    */
    UC_DISPLAY_MESSAGE_STOP                            = 0x00000000,

    /** (0x00000001) Secure core is being flashed.
    * 
    * Recommended OSD Message: "Critical data is being flashed. Please don't power off."
    * This value requires manufacturer to display the above message to prevent the device from being powered off by the user. 
    * It occurs when secure core package is being written to persistent storage.
    */
    UC_DISPLAY_MESSAGE_FLASHING_SECURE_CORE            = 0x00000001

} uc_display_message_type;

/**
 * Display message data structure
 *
 * This structure is used for the payload of global message ::UC_GLOBAL_NOTIFY_DISPLAY_MESSAGE_ON_SCREEN.
 */
typedef struct _uc_display_message
{
    /**
     * indicate the type of the displayed message
     */
    uc_display_message_type type;

} uc_display_message;

/**
 * Secure pre-enablement remaining time status structure
 *
 * This structure is used by ::uc_service_status_st to deliver the secure pre-enablement remaining timestatus.
 */
typedef struct _uc_spe_remaining_time_status
{
    /**
     * Unique product identifier
     *
     * This value uniquely identifies a product and is the big-endian representation 
     * of the 'ID' field for the corresponding product configuration in the CA server.
     * For example, if the product ID in the head end is 1000 decimal,
     * the product ID in this structure will be {0x03, 0xe8}.
     * See ::UC_SIZE_PRODUCT_ID for its size.
     */
    uc_byte product_id[UC_SIZE_PRODUCT_ID];

    /**
     * How long the product entitlement is remaining.
     *
     * The unit of remaining time is DAY.
     */
    uc_uint32 remainingTime;
} uc_spe_remaining_time_status;

/**
 * Authentication data type
 *
 * This type is used for authentication in HAAS.
 */
typedef enum {
    /**
     * basic type for authentication
     */
    UC_BASIC_AUTHENTICATION = 0x00000000,
    /**
     * this type is not supported yet!
     */
    UC_DIGEST_ACCESS_AUTHENTICATION = 0x00000001
} uc_authentication_type;

/**
 * Home Gateway Proximity Notification Status
 */
typedef enum
{
    /** 
     * (0x00000000) The notification status was successful. 
     */
    UC_HGPC_NOTIFY_STATUS_SUCCESS                = 0x00000000, 

     /**
      * (0x00000001) The notification status failed.
      */
    UC_HGPC_NOTIFY_STATUS_OTHER_ERROR            = 0x00000001,
    
     /**
      * (0x00000002) The UniversalClientSPI_HGPC_SendHNAMessage implementation returned an error.
      */
    UC_HGPC_NOTIFY_STATUS_HNA_SEND_FAILED        = 0x00000002,

     /**
      * (0x00000003) The HNA message received by a secondary secure client was invalid or repetitive.
      */
    UC_HGPC_NOTIFY_STATUS_HNA_MESSAGE_INVALID    = 0x00000003
} uc_hgpc_status;

/**
 * Home Gateway Proximity Control mode.
 *
 * It is used to indicate that the Cloacked CA client is HGPC primary, secondary or non-HGPC client.
 */
typedef enum
{
    /** (0x00000000) Non-HGPC client. 
      */
    UC_HGPC_MODE_NONE             = 0x00000000, 

     /**
      * (0x00000001) HGPC Primary Secure Client.
      */
    UC_HGPC_MODE_PRIMARY        = 0x00000001,
    
     /**
      * (0x00000002) HGPC Secondary Secure Client.
      */
    UC_HGPC_MODE_SECONDARY    = 0x00000002
} uc_hgpc_mode;

/**
 * Home Gateway Proximity Control Primary Secure Client Information
 */
typedef struct _uc_hgpc_primary_info
{
    /**
     * The value in seconds which the timer has to be set to refresh the HNA message in the primary secure client.
     */
    uc_uint32       hnaRefreshT;
    
    /**
     * The value in seconds which the primary secure client uses to repeat the HNA message.
     */
    uc_uint32       hnaRepeat;

    /**
     * The number of secondary secure clients in the HGPC network. Maximum is ::UC_MAX_HGPC_SECONDARY.
     */
    uc_byte         nrOfSecondaries;

    /**
     * The serial numbers of secondary secure clients in the HGPC network.
     */
    uc_byte         secondarySerialNr[UC_MAX_HGPC_SECONDARY][UC_SERIAL_NUMBER_SIZE];
} uc_hgpc_primary_info;

/**
 * Home Gateway Proximity Control Secondary Secure Client Information
 */
typedef struct _uc_hgpc_secondary_info
{
    /**
     * The value in seconds which the timer has to be set to get a new HNA message in the secondary secure client.
     * If 0, don't use the timer.
     */
    uc_uint32       hnaT;

    /**
     * The serial number of primary secure client in the HGPC network.
     */
    uc_byte         primarySerialNr[UC_SERIAL_NUMBER_SIZE];
} uc_hgpc_secondary_info;

/**
 * Home Gateway Proximity Control Secure Client Information
 */
typedef struct _uc_hgpc_info
{
    /**
     * Home Gateway Proximity Control working mode.
     */
    uc_hgpc_mode    hgpcMode;

    /**
     * Provides the HGPC setting information when the secure client is primary client.
     * The data will be all 0 when it is non-primary client.
     */
    uc_hgpc_primary_info        pimaryInfo;

    /**
     * Provides the HGPC setting information when the secure client is secondary client.
     * The data will be all 0 when it is non-secondary client.
     */
    uc_hgpc_secondary_info      secondaryInfo;
} uc_hgpc_info;

/**
 * Callback called to notify the application of a global event. 
 *
 * An application can implement a callback function of this type and pass it to
 * ::UniversalClient_StartCaClient.  It will then receive notifications from the client
 * when global events occur that are not associated with a particular object.
 *
 * Because this callback may occur in the context of a client thread, 
 * an application should take care to queue this message for later processing and return immediately
 * from the function. In particular, attempting to call back into any UniversalClient_XXX methods
 * from the same thread as the callback may result in undefined behavior. 
 * 
 * When processing, the application can do anything it wants with the message. Examples: log it to file,
 * display it onscreen, display a web page, or ignore it. 
 * However, the callback implementation must be sure to make copies of any of data referenced by the pointers 
 * passed into it that it wants to use outside of the context of this function, as the pointers become invalid
 * as soon as this function returns.
 *
 * @param[in] message Message ID for this message. See the definition of ::uc_global_message_type for a description
 *     of the different types. 
 * @param[in] lpVoid Message defined pointer. The value of this depends on the message type. See ::uc_global_message_type
 *     for details.
 *
 * <b>Example:</b>
 * \code
void MyGlobalMessageProc(
    uc_global_message_type message, 
    void* lpVoid)
{
    switch(message)
    {
        case UC_GLOBAL_DEBUG:
        {
            uc_char *pText = (uc_char *)lpVoid;
            printf("Debug output: %s\n", pText);
            break;
        }
        default:
        {
            printf("Unknown message type.\n");
            break;
        }
    }
}

void main()
{
    UniversalClient_StartCaClient(MyGlobalMessageProc);
}

 * \endcode
 */
typedef void (*uc_global_message_proc)(
    uc_global_message_type message, 
    void* lpVoid);

/** @defgroup universalclient_api Cloaked CA Agent APIs

 *  Cloaked CA Agent APIs
 *
 *  These API methods provide the necessary interfaces for the Application in client device.
 *
 *  @{
 */


/** @defgroup universalclient Cloaked CA Agent Base APIs
 *  Cloaked CA Agent Base APIs
 *
 *  These API methods provide the necessary basic functionality for querying information about
 *  the client, gathering product information, and controlling regional filtering. 
 *
 *  @{
 */

/**
 * Obtain the current version of the Cloaked CA Agent
 *
 * This function can be called to obtain the current version of the Cloaked CA Agent. This version can be used by applications to
 * check for supported features and application compatibility.  An application might also display a status screen with
 * version information on it that can be used for technical support purposes. 
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pVersion Receives the version string associated with the client. 
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the version
 *     string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire version string, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application 
 *     can then allocate memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and the length includes 1 byte NULL terminator.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetVersion(
        uc_buffer_st *pVersion);

/**
 * Obtain the build information of the Cloaked CA Agent
 *
 * This function can be called to obtain the build information of the Cloaked CA Agent. 
 * The Application shall display the build info in the client status screen.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pBuildInformation Receives the build string associated with the client. 
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the build
 *     string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire build string, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application 
 *     can then allocate memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and the length includes 1 byte NULL terminator.
 *
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetBuildInformation(
        uc_buffer_st *pBuildInformation);

/**
 * Obtain a description for functionalities provided by Cloaked CA Agent
 *
 * This function can to called to obtain a description for the functionalities provided by Cloaked CA Agent.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pCapabilities Receives the string of functionalities description associated with the client. 
 *     On output, the uc_buffer_st::bytes member will be filled with a NULL-terminated string describing the functionalities.
 *
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the capabilities
 *     string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire string of functionalities description, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application 
 *     can then allocate memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and the length includes 1 byte NULL terminator.
 *
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetCapabilities(
        uc_buffer_st *pCapabilities);

/**
 * Obtain the secure type of Cloaked CA Agent
 *
 * This function can be called to obtain the secure type of Cloaked CA Agent.
 * If the output value is 0, the secure type is "Secure Chipset".
 * If the output value is 1, the secure type is "Security ID for Secure Chipset based STBs".
 * If the output value is 2, the secure type is "Security ID for IP only STBs".
 * If the output value is 3, the secure type is "Active Node Lock".
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pSecureType Gets the current secure type of Cloaked CA Agent. 
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetSecureType(
        uc_byte *pSecureType);  

/**
 * Obtain the current LOCK ID in Cloaked CA Agent
 *
 * This function can be called to obtain the LOCK ID embeded in the Cloaked CA Agent.
 * A LOCK ID is a value assigned to a manufacturer.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pLockId Receives the current LOCK ID in Cloaked CA Agent. 
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetLockId(
        uc_uint16 *pLockId);         

/**
 * Obtain the internal serial number (unique address) of the CA client. 
 *
 * This API is obsolete from Cloaked CA Agent 2.9.0. To get the internal serial number, please refer to ::UniversalClient_Extended_GetSerialNumber.
 *
 * This function can be used to obtain the internal serial number of the Cloaked CA Agent. 
 * This value is assigned by the headend system, and is used to uniquely address a particular device when sending EMMs to it.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * \note This serial number MAY BE DIFFERENT from the Chipset Serial Number (CSSN) that is returned
 * from ::UniversalClientSPI_Device_GetCSSN. 
 *
 * @param[in,out] pSN Receives the internal serial number assigned to the client. 
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length 
 *     member to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer 
 *     to contain the serial number in binary big-endian format, without any terminating character, and set the 
 *     uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large 
 *     enough to hold the entire serial number, the uc_buffer_st::length member will be set to the minimum required 
 *     amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application can then allocate 
 *     memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
 *
 *     \n
 *     \note     
 *     Suppose the internal serial number is 2864434397 i.e. 0xAABBCCDD, uc_buffer_st::bytes will have the following values: \n
 *     uc_buffer_st::bytes[0] = 0xAA, \n
 *     uc_buffer_st::bytes[1] = 0xBB, \n
 *     uc_buffer_st::bytes[2] = 0xCC, \n
 *     uc_buffer_st::bytes[3] = 0xDD. \n
 *     and uc_buffer_st::length is set to 4.
 *     \n
 * 
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetSerialNumber(
        uc_buffer_st *pSN);


/**
 * Obtain the current Secure Chipset Serial Number
 *
 * This function can be called to obtain the secure chipset serial number. This secure chipset 
 * serial number is associated with the device.
 *
 * @param[in,out] pCSSN Receives the CSSN assigned to the client. 
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length 
 *     member to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer 
 *     to contain the CSSN in binary big-endian format, without any terminating character, and set the 
 *     uc_buffer_st::length member to the actual length of data returned. If the uc_buffer_st::length member is not large 
 *     enough to hold the entire CSSN, the uc_buffer_st::length member will be set to the minimum required 
 *     amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application can then allocate 
 *     memory of a sufficient size and attempt to call the function again.
 * 
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
 *     \n
 *     \note     
 *     Suppose the CSSN is 4293844428 i.e. 0xFFEEDDCC, uc_buffer_st::bytes will have the following values: \n
 *     uc_buffer_st::bytes[0] = 0xFF, \n
 *     uc_buffer_st::bytes[1] = 0xEE, \n
 *     uc_buffer_st::bytes[2] = 0xDD, \n
 *     uc_buffer_st::bytes[3] = 0xCC. \n
 *     and uc_buffer_st::length is set to 4.
 *     \n
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
  * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetCSSN(
        uc_buffer_st *pCSSN);

/**  
 * Obtain CA Client ID (the external CA serial number for operational usage) . \n
 * This API is obsolete from Cloaked CA Agent 2.9.0. To get the CA Client ID, please refer to ::UniversalClient_Extended_GetClientIDString. 
 *
 * This function can be called to obtain the client ID string which is supposed to be shown in the error banner, so that the viewer can read the identifiers directly from the error banner for operational purposes.
 *
 * If current client has been initialized (i.e. sector created for the client), the string  format is "xxxxxxxxxx(y)". xxxxxxxxxx will be the client ID number in decimal format. y is the check digit for xxxxxxxxxx.  
 *
 * (i) For a client device that applies instant CA serial number allocation scheme (i.e. manufacturers calculates CDSN=CSSN+0x1,0000,0000, and print CDSN label on the device at production time),  the client ID is the same as the CDSN value, which has the value range from 4,294,967,296 to 8,589,934,591.  \n
 * An example string:4594336592(9) 
 *
 * (ii) For a client device that does NOT apply the instant CA serial number allocation scheme,  this client ID is the same as the internal serial number (the value retrieved via UniversalClient_Extended_GetSerialNumber), which has the value range from 2,001,053,505 to 2,999,999,999.  \n
 * An example string:2005270529(8)
 *
 * If current client is a virgin one, the string  format is "xxxxxxxxxx(y)-z". xxxxxxxxxx  is the chipset serial number (CSSN) in decimal format, y is the check digit for xxxxxxxxxx and z  is the Lock ID in decimal format.  \n
 * An example string:0805502977(4)-0
 *
 * \note The information from this method must be displayed in the error banner dialog.
 *
 * @param[in,out] pClientIDString Receives the Client ID string assigned to the client. 
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length 
 *     member to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer 
 *     to contain the Client ID string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire Client ID string, the uc_buffer_st::length member will be set to the minimum required 
 *     amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application can then allocate 
 *     memory of a sufficient size and attempt to call the function again.
 * 
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and this length include 1 byte terminator.
 *
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetClientIDString(uc_buffer_st *pClientIDString);


/**
 * Obtain the current CA System ID
 *
 * This API is obsolete from Cloaked CA Agent 2.9.0. To get operator infomation such as CA System ID, 
 * please refer to ::UniversalClient_Extended_GetOperatorInfo.
 *
 * This method can be called to obtain the current CA System ID. This value is typically used to 
 * select the correct Irdeto CA stream when multiple streams exist from different CA vendors. 
 *
 * \note The information from this method must be displayed in a GUI dialog. 
 *
 * @param[out] pCASystemID Receives the current CA System ID. When Cloaked CA Agent support a range 
 * of CA System IDs, the output of this method may vary: If Cloaked CA Agent has chosen a valid ID in
 * the range as its active ID, it returns the chosen ID, otherwise, it returns the start ID in its range. 
 * If Cloaked CA Agent only supports a single ID, the result is the single one. See ::UniversalClient_GetCASystemIDRange
 * for getting the range of CA System IDs.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetCASystemID(
        uc_uint32 * pCASystemID);

/**
 * Obtain the CA System ID range
 *
 * This API is obsolete from Cloaked CA Agent 2.9.0. To get operator infomation such as CA System ID range, 
 * please refer to ::UniversalClient_Extended_GetOperatorInfo.
 *
 * This method can be called to obtain the valid CA System ID range if Cloaked CA Agent supports 
 * a range of CA System IDs. A valid CA System ID should be: (start ID <= valid ID <= end ID). If only 
 * a single CA System ID is supported, this can be omitted.
 *
 *
 * @param[out] pCASystemIDStart Receives the start value of the CA System ID range. 
 * @param[out] pCASystemIDEnd Receives the end value of the CA System ID range. 
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetCASystemIDRange(
    uc_uint32 *pCASystemIDStart,
    uc_uint32 *pCASystemIDEnd);

/**
 * Obtain the Nationality
 *
 * This API is obsolete from Cloaked CA Agent 2.9.0. To get nationality, 
 * please refer to ::UniversalClient_Extended_GetNationality.
 *
 * This function can be called to obtain the nationality of the Cloaked CA Agent. 
 * The nationality is described by three ISO 3166-1 Alpha-3 identifier (e.g. 'CHN', 'GBR', 'USA'). 
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pNationality Receives the nationality associated with the client. 
 *     On output, the uc_buffer_st::bytes member will be filled with a NULL-terminated string.  
 *
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the nationality 
 *     , including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire nationality, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_NO_NATIONALITY
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetNationality(
        uc_buffer_st *pNationality);


/**
* Obtain Homing Channel data
*
* This API is obsolete from Cloaked CA Agent 2.9.0. To get homing channel data, 
* please refer to ::UniversalClient_Extended_GetHomingChannelData.
*
* This function can be called to obtain Homing Channel data. 
*
*
* @param[in,out] pHomingChannelData Receives Homing Channel data.
*     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member 
*     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the Homing Channel data,
*     in binary big-endian format, without any terminating character and set the uc_buffer_st::length member to the actual length of data returned. 
*     If the uc_buffer_st::length member is not large enough to hold the entire Homing Channel data, the uc_buffer_st::length member
*     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application 
*     can then allocate memory of a sufficient size and attempt to call the function again.
*
*     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
*       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
*     
*     @code
*     Format of the data(The bit order is bslbf):
*     
*     Field Name      Length(bits)     
*     DTH_IRD         1            
*     ProfDec_IRD     2            
*     Spare           4            
*     Monitoring_IRD  1            
*     Homing_Channel  96                   
*     Spare           152           
*
*     In the above strucure, Homing_Channel is defined as:
*
*     Field Name            Length(bits)  
*     Frequency             32            
*     Orbital_Position      16            
*     Symbol_Rate           28            
*     Reserved              4             
*     MuxVerifier_Mode      4             
*     FEC_Inner             4             
*     West_East_Flag        1             
*     Polarisation          2
*     Roll off              2
*     Modulation System     1
*     Modulation Type       2
*
*     The detailed bit format of the above fields in Home_Channel structure is:
*
*     Field Name          Value                     Description
*     Frequency           DD D.D DD DD              where D = BCD digit
*     Orbital Pos         DD D.D                    where D = BCD digit
*     Symbol Rate         DD D.D DD D               where D = BCD digit
*     Mux Verifier        0                         Mux No Allowed
*                         1                         Mux Early
*                         2                         Mux Late
*                         3                         Mux Middle
*     FEC lnner           1                         1/2
*                         2                         2/3
*                         3                         3/4
*                         4                         5/6
*                         5                         7/8
*     West/East           0                         West
*                         1                         East
*     Polarisation        0                         Lin Horizontal
*                         1                         Lin Vertical
*                         2                         Cir Left
*                         3                         Cir Right
*     Modulation system   0                         DVB-S
*                         1                         DVB-S2
*     Roll off            0                         a=0.35
*                         1                         a=0.25
*                         2                         a=0.20
*                         3                         Reserved
*     Modulation type 
*     (DVB-S)             0                         Reserved
*                         1                         QPSK
*                         2                         8PSK
*                         3                         16-QAM
*     Modulation type 
*     (DVB-S2)            0                         Reserved
*                         1                         QPSK
*                         2                         8PSK
*                         3                         Reserved
*
*     Below is an example of a 32 Byte Homeing Channel Data:
*
*     DTH IRD with PAS4 settings
*     Byte             Value            Description    
*     [0]              0x80             DTH IRD
*     [1]              0x01             Frequency = 12.5175
*     [2]              0x25
*     [3]              0x17
*     [4]              0x50
*     [5]              0x06             Orbital Pos = 68.5 deg
*     [6]              0x85
*     [7]              0x02             Symbol rate = 21.850
*     [8]              0x18
*     [9]              0x50
*     [10]             0x00
*     [11]             0x04             FEC 5/6
*     [12]             0x95             East, DVB-S2, a=0.20  Horizontal, QPSK
*     [13]-[31]        0x00             Spare
*     @endcode
*
* @retval 
*    ::UC_ERROR_SUCCESS
* @retval
*    ::UC_ERROR_NULL_PARAM
* @retval
*    ::UC_ERROR_INSUFFICIENT_BUFFER
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List". 
*
*/
uc_result UniversalClient_GetHomingChannelData(uc_buffer_st* pHomingChannelData);


/**
* Obtain Middleware user data
*
* This API is obsolete from Cloaked CA Agent 2.9.0. To get middleware user data, 
* please refer to ::UniversalClient_Extended_GetMiddlewareUserData.
*
* This function can be called to obtain Middleware user data. 
*
* @param[in,out] pMiddlewareUserData Receives Middleware user data.
*     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member 
*     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the Middleware user data,
*     in binary big-endian format, without any terminating character and set the uc_buffer_st::length member to the actual length of data returned. 
*     If the uc_buffer_st::length member is not large enough to hold the entire Middleware user data, the uc_buffer_st::length member
*     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application 
*     can then allocate memory of a sufficient size and attempt to call the function again.
*
*     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
*       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
*
*
* @retval 
*    ::UC_ERROR_SUCCESS
* @retval
*    ::UC_ERROR_NULL_PARAM
* @retval
*    ::UC_ERROR_INSUFFICIENT_BUFFER
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List". 
*
*/

uc_result UniversalClient_GetMiddlewareUserData(uc_buffer_st * pMiddlewareUserData);


/**
 * Obtains the list of current products. 
 *
 * This function can be used to obtain a list of products.  Each product consists of an ID and
 * an entitlement state.  This function could be used for example to display a different image or 
 * color in a program guide for programs associated with these products. 
 *
 * \note The information from this method must be displayed in a GUI dialog.
 * \note For Unified Client, product information from smartcard will also be returned.
 *       But the function will be blocked for several seconds when getting the product list from smartcard.
 * \note For Unified Client, the PVR product from smartcard does not have time information,
 *       so the startDate and duration of PVR product from smartcard will be zeros and are not applicable for display.
 *
 * @param[out] pProductCount If the function succeeds, this receives the number of 
 *     elements returned in the ppProductList array.
 * @param[out] ppProductList If the function succeeds, this receives a pointer to a function-allocated
 *     array of ::uc_product_status structures.  When the caller no longer needs the information in the
 *     array, it must call ::UniversalClient_FreeProductList to free the array.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".  
 */
uc_result UniversalClient_GetProductList(uc_uint32 *pProductCount, uc_product_status **ppProductList);

/**
 * Free resources allocated in ::UniversalClient_GetProductList.
 *
 * This function will free the memory allocated by a previous call to ::UniversalClient_GetProductList.
 *
 * @param[in,out] ppProductList On input, this is the address of the pointer received in a previous call to ::UniversalClient_GetProductList.
 *       On output, the function sets this to NULL. 
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".  
 */
uc_result UniversalClient_FreeProductList(uc_product_status **ppProductList);

/**
 * Check if there is any product in the given product list is entitled. 
 *
 * This function can be used to check the overall entitlement status of a product list, in order to facilitate entitlement checking in PUSH VOD.
 * Middleware is required to provide the product information to be checked. If entitled, the PUSH VOD content can be played immediately; 
 * otherwise, user is required to purchase PPV VOD product or Subscription VOD product to view the VOD content.
 *
 * @param[in] productCount indicate how many products is given in the product list.
 * @param[in] pProductList indicate a list of products information to be checked.
 *
 * @retval 
 *    ::UC_TRUE at least one product in the given list is entitled
 * @retval
 *    ::UC_FALSE none of the products are entitled or present
 */
uc_bool UniversalClient_QueryAnyProductEntitled(uc_uint32 productCount, const uc_product_identifier *pProductList);

/**
 * Obtain the count of ECM and EMM sections processed by Cloaked CA Agent.
 *
 * This API is obsolete from Cloaked CA Agent 2.9.0. To get ECM and EMM count, 
 * please refer to ::UniversalClient_Extended_GetEcmEmmCount.
 *
 * This function can be called to obtain count of CA sections that have been processed by Cloaked CA Agent.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pCaSectionCount If the function succeeds, this receives the structure of
 *     ::uc_ca_section_count structures contained the count of ECM and EMM processed by Cloaked CA Agent.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetEcmEmmCount(uc_ca_section_count *pCaSectionCount);

/**
 * Notify OOB EMM sections to the Client
 *
 * This function is intended to notify the client with oob emm sections. 
 *
  *
  * @param[in] pOobEmmSection Buffer containing the raw content of at least one oob emm sections.
  *     The uc_buffer_st::bytes member must point to a buffer containing the section data, and
  *     the uc_buffer_st::length must be set to the number of bytes in the buffer.
  *
  * @retval 
  *    ::UC_ERROR_SUCCESS
  * @retval 
  *    ::UC_ERROR_OUT_OF_MEMORY
  * @retval
  *    ::UC_ERROR_NULL_PARAM
  * @retval
  *     Other The method may return any other errors listed in \ref result "Result Code List". 
  */
uc_result UniversalClient_SendOobEmmSection(const uc_buffer_st * pOobEmmSection);

/**
 * Notify OOB EMM sections to the Client
 *
 * This function is extended from ::UniversalClient_SendOobEmmSection to support OOB EMM section that has been transformed in other format. 
 *
  *
  * @param[in] pOobEmmSection OOB EMM data to be delivered to Cloaked CA Agent.
  *
  * @retval 
  *    ::UC_ERROR_SUCCESS
  * @retval 
  *    ::UC_ERROR_OUT_OF_MEMORY
  * @retval
  *    ::UC_ERROR_NULL_PARAM
  * @retval
  *     Other The method may return any other errors listed in \ref result "Result Code List". 
  */
uc_result UniversalClient_Extended_SendOobEmmSection(const uc_oob_emm_data * pOobEmmSection);

/**
 * This function will free the generic memory allocated by Cloaked CA Agent.
 *
 * @param[in,out] pMemoryData On input, the uc_buffer_st::bytes member is 
 *       the address holding a buffer, and the uc_buffer_st::length member is the number of bytes in the buffer.
 *       On output, the function frees the memeory, sets uc_buffer_st::bytes member to NULL, 
 *       and sets uc_buffer_st::bytes member to 0. 
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */

uc_result UniversalClient_FreeBuffer(
    uc_buffer_st *pMemoryData );  


/**
 * Clear the CA data on Persistent Storage.
 *
 * This API is obsolete from Cloaked CA Agent 2.9.0. To reset CA data, 
 * please refer to ::UniversalClient_Extended_ResetCAData.
 *
 * This function will clear all CA data on Persistent Storage. All CA data will be lost.
 * This function shall be called when the user wants to reset its device to factory state.
 * After the data is cleared, ::UC_GLOBAL_NOTIFY_CA_DATA_CLEARED will be sent to device application
 * as a notification.
 *
 * \note This function is dangerous! Device application or the middleware shall make sure that it's
 * never triggered by accidental user actions. 
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_ALREADY_STOPPED
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result  UniversalClient_ResetCAData(void);


/**
 * Obtain the current version of Secure Core.
 *
 * This API is obsolete from Cloaked CA Agent 2.9.0. To get secure core version, 
 * please refer to ::UniversalClient_Extended_GetSecureCoreVersion.
 *
 * This function can be called to obtain the current version of Secure Core. This version can be used by applications to
 * check for supported features and application compatibility.  An application might also display a status screen with
 * version information on it that can be used for technical support purposes. 
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pVersion Receives the version string associated with Secure Core. 
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the version
 *     string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire version string, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application 
 *     can then allocate memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and the length includes 1 byte NULL terminator.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetSecureCoreVersion( uc_buffer_st *pVersion);

/**
 * Obtain the home network info of this client.
 *
 * This API is used to get the current home network info of the Cloaked CA Agent.
 *
 * @param[in,out] pInfo A structure provided by application and filled by Cloaked CA Agent.
 *
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetHomeNetworkInfo( uc_home_network_info *pInfo );


/* Followings are extended APIs for multiple operators */

/**
 * Obtain the internal serial number (unique address) of the CA client for multiple operators. 
 *
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use ::UniversalClient_GetSerialNumber.
 *
 * This function can be used to obtain the internal serial number of the Cloaked CA Agent. 
 * This value is assigned by the headend system, and is used to uniquely address a particular device when sending EMMs to it.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * \note This serial number MAY BE DIFFERENT from the Chipset Serial Number (CSSN) that is returned
 * from ::UniversalClientSPI_Device_GetCSSN. 
 *
 * @param[out] pSN contains the internal serial number assigned to the client. 
 *     The uc_serial_number::validOperatorCount indicates the number of current valid operator.
 *     The uc_serial_number::serialNumber indicates the internal serial number value for specific operator.
 *     The uc_serial_number_per_operator::caSystemID indicates the operator by its CA System ID.
 *     The uc_serial_number_per_operator::serialNumberBytes will be filled with 4 bytes without a NULL-terminated string.
 *
 *     \n
 *     \note     
 *     Suppose the internal serial number is 2864434397 i.e. 0xAABBCCDD, uc_serial_number_per_operator::serialNumberBytes will have
 *     the following values: \n
 *     uc_serial_number_per_operator::serialNumberBytes[0] = 0xAA, \n
 *     uc_serial_number_per_operator::serialNumberBytes[1] = 0xBB, \n
 *     uc_serial_number_per_operator::serialNumberBytes[2] = 0xCC, \n
 *     uc_serial_number_per_operator::serialNumberBytes[3] = 0xDD. \n
 *     \n
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_Extended_GetSerialNumber(uc_serial_number *pSN);

/**  
 * Obtain CA  Client ID (the external CA serial number for operational usage) for multiple operators. \n
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use ::UniversalClient_GetClientIDString.
 *
 * This function can be called to obtain the client ID string which is supposed to be shown in the error banner, so that the viewer can read the identifiers directly from the error banner for operational purposes.
 *
 * If current client has been initialized (i.e. sector created for the client), the string  format is "xxxxxxxxxx(y)". xxxxxxxxxx will be the client ID number in decimal format. y is the check digit for xxxxxxxxxx.  
 *
 * (i) For a client device that applies instant CA serial number allocation scheme (i.e. manufacturers calculates CDSN=CSSN+0x1,0000,0000, and print CDSN label on the device at production time),  the client ID is the same as the CDSN value, which has the value range from 4,294,967,296 to 8,589,934,591.  \n
 * An example string:4594336592(9) 
 *
 * (ii) For a client device that does NOT apply the instant CA serial number allocation scheme,  this client ID is the same as the internal serial number (the value retrieved via UniversalClient_Extended_GetSerialNumber), which has the value range from 2,001,053,505 to 2,999,999,999.  \n
 * An example string:2005270529(8)
 *
 * If current client is a virgin one, the string  format is "xxxxxxxxxx(y)-z". xxxxxxxxxx  is the chipset serial number (CSSN) in decimal format, y is the check digit for xxxxxxxxxx and z  is the Lock ID in decimal format.  \n
 * An example string:0805502977(4)-0
 *
 * \note The information from this method must be displayed in the error banner dialog.
 *
 * @param[out] pClientIDString Receives the Client ID string assigned to the client.
 *     The uc_client_id::validOperatorCount indicates the number of current valid operator.
 *     The uc_client_id::clientID indicates the client ID string for specific operator.
 *     The uc_client_id_per_operator::caSystemID indicates the operator by its CA System ID.
 *     The uc_client_id_per_operator::clientIDString will be filled a NULL-terminated string.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_Extended_GetClientIDString(uc_client_id *pClientIDString);

/**
 * Obtain the operator information for multiple operators
 *
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
 * ::UniversalClient_GetCASytemID and ::UniversalClient_GetCASystemIDRange to get
 * operator's CA System ID and range.
 *
 * This function can be called to obtain the operator information supported.
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pOperatorInfo contains the operator information supported by the device. 
 *     The uc_operator::validOperatorCount indicates the number of current valid operator.
 *     The uc_operator::operatorInfo describes the information for specific operator.
 *     The uc_single_operator::active indicates if the operator is active. If UC_TRUE, the operator is active. Or UC_FALSE will be returned.
 *     The uc_single_operator::activeCaSystemID indicates the CA System ID which matches the real network for the operator.
 *     The uc_single_operator::caSystemIDStart indicates the start value of the range for the operator.
 *     The uc_single_oeprator::caSystemIDEnd indicates the end value of the range for the operator.
 *     The uc_single_oeprator::operatorName will be filled with a NULL-terminated string.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_NO_NATIONALITY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_Extended_GetOperatorInfo(uc_operator *pOperatorInfo);

/**
 * Obtain the nationality for multiple operators
 *
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
 * ::UniversalClient_GetNationality.
 *
 * This function can be called to obtain the nationality of the Cloaked CA Agent. 
 * The nationality is described by three ISO 3166-1 Alpha-3 identifier (e.g. 'CHN', 'GBR', 'USA'). 
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pNationality contains the nationality associated with the client. 
 *     The uc_nationality::validOperatorCount indicates the number of current valid operator.
 *     The uc_nationality::nationality describes the nationality data for specific operator.
 *     The uc_nationality_per_operator::caSystemID indicates the operator by its CA System ID.
 *     The uc_nationality_per_operator::nationalityData member will be filled with 3 bytes data without a NULL-terminated.  
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_NO_NATIONALITY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_Extended_GetNationality(uc_nationality *pNationality);

/**
 * Obtain the homing channel for multiple operators
 *
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
 * ::UniversalClient_GetHomingChannelData.
 *
 * This function can be called to obtain Homing Channel data. 
 *
 *
 * @param[out] pHomingChannelData contains Homing Channel data.
 *     The uc_homing_channel::validOperatorCount indicates the number of current valid operator.
 *     The uc_homing_channel::homingChannel describes the homing channel data for specific operator.
 *     The uc_homing_channel_per_operator::caSystemID indicates the operator by its CA System ID.
 *     The uc_homing_channel_per_operator::homingChannelData contains detail homing channel data.
 *
 *     @code
 *     Format of the data(The bit order is bslbf):
 *     
 *     Field Name      Length(bits)     
 *     DTH_IRD         1             
 *     ProfDec_IRD     2             
 *     Spare           4             
 *     Monitoring_IRD  1             
 *     Homing_Channel  96                   
 *     Spare           152           
 *
 *     In the above strucure, Homing_Channel is defined as:
 *
 *     Field Name            Length(bits)  
 *     Frequency             32            
 *     Orbital_Position      16            
 *     Symbol_Rate           28            
 *     Reserved              4             
 *     MuxVerifier_Mode      4             
 *     FEC_Inner             4             
 *     West_East_Flag        1             
 *     Polarisation          2
 *     Roll off              2
 *     Modulation System     1
 *     Modulation Type       2
 *
 *     The detailed bit format of the above fields in Home_Channel structure is:
 *
 *     Field Name          Value                     Description
 *     Frequency           DD D.D DD DD              where D = BCD digit
 *     Orbital Pos         DD D.D                    where D = BCD digit
 *     Symbol Rate         DD D.D DD D               where D = BCD digit
 *     Mux Verifier        0                         Mux No Allowed
 *                         1                         Mux Early
 *                         2                         Mux Late
 *                         3                         Mux Middle
 *     FEC lnner           1                         1/2
 *                         2                         2/3
 *                         3                         3/4
 *                         4                         5/6
 *                         5                         7/8
 *     West/East           0                         West
 *                         1                         East
 *     Polarisation        0                         Lin Horizontal
 *                         1                         Lin Vertical
 *                         2                         Cir Left
 *                         3                         Cir Right
 *     Modulation system   0                         DVB-S
 *                         1                         DVB-S2
 *     Roll off            0                         a=0.35
 *                         1                         a=0.25
 *                         2                         a=0.20
 *                         3                         Reserved
 *     Modulation type 
 *     (DVB-S)             0                         Reserved
 *                         1                         QPSK
 *                         2                         8PSK
 *                         3                         16-QAM
 *     Modulation type 
 *     (DVB-S2)            0                         Reserved
 *                         1                         QPSK
 *                         2                         8PSK
 *                         3                         Reserved
 *
 *     Below is an example of a 32 Byte Homeing Channel Data:
 *
 *     DTH IRD with PAS4 settings
 *     Byte             Value            Description    
 *     [0]              0x80             DTH IRD
 *     [1]              0x01             Frequency = 12.5175
 *     [2]              0x25
 *     [3]              0x17
 *     [4]              0x50
 *     [5]              0x06             Orbital Pos = 68.5 deg
 *     [6]              0x85
 *     [7]              0x02             Symbol rate = 21.850
 *     [8]              0x18
 *     [9]              0x50
 *     [10]             0x00
 *     [11]             0x04             FEC 5/6
 *     [12]             0x95             East, DVB-S2, a=0.20  Horizontal, QPSK
 *     [13]-[31]        0x00             Spare
 *     @endcode
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 *
 */
uc_result UniversalClient_Extended_GetHomingChannelData(uc_homing_channel *pHomingChannelData);

/**
 * Obtain the middleware user data for multiple operators
 *
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
 * ::UniversalClient_GetMiddlewareUserData.
 *
 * This function can be called to obtain Middleware user data for multiple operators. 
 *
 * @param[out] pMiddlewareUserData Receives Middleware user data.
 *     The uc_middleware_user_data::validOperatorCount indicates the number of current valid operator.
 *     The uc_middleware_user_data::middlewareUserData describes the middleware data for specific operator.
 *     The uc_middleware_user_data_per_operator::caSystemID indicates the operator by its CA System ID.
 *     The uc_middleware_user_data_per_operator::middlewareUserData contains detail middleware user data.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_Extended_GetMiddlewareUserData(uc_middleware_user_data *pMiddlewareUserData);

/**
 * Obtain the ecm and emm count for multiple operators
 *
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
 * ::UniversalClient_GetEcmEmmCount.
 *
 * This function can be called to obtain count of CA sections that have been processed by Cloaked CA Agent.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pCaSectionCount If the function succeeds, this receives the structure of
 *     ::uc_ca_extended_section_count structures contained the count of ECM and EMM processed by Cloaked CA Agent.
 *     The uc_ca_extended_section_count::validOperatorCount indicates the number of current valid operator.
 *     The uc_ca_extended_section_count::caSectionCount describes the CA section count for specific operator.
 *     The uc_ca_section_count_per_operator::caSystemID indicates the operator by its CA System ID.
 *     The uc_ca_section_count_per_operator::ecm_count contains the ecm section count.
 *     The uc_ca_section_count_per_operator::emm_count contains the emm section count.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_Extended_GetEcmEmmCount(uc_ca_extended_section_count *pCaSectionCount);

/**
 * Clear the CA data on Persistent Storage for multiple operators.
 *
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
 * ::UniversalClient_ResetCAData.
 *
 * This function will clear CA data on Persistent Storage for specific operators. The CA data will be lost.
 * This function shall be called when the user wants to reset its device to factory state.
 * After the data is cleared, ::UC_GLOBAL_NOTIFY_CA_DATA_CLEARED will be sent to device application
 * as a notification.
 *
 * \note This function is dangerous! Device application or the middleware shall make sure that it's
 * never triggered by accidental user actions. 
 *
 * @param[in] pCASystemID indicates the operator whose CA data will be cleared. 
 *     The uc_ca_system_id::validOperatorCount indicates the number of operators. If the validOperatorCount is 0, all operators' CA
 *     data will be cleared. If the validOperatorCount is greater than 0, the CA data for the operator who is identifies by CA System ID in 
 *     uc_ca_system_id::caSystemID will be cleared.
 *     The uc_ca_system_id::caSystemID indicates the operator by its CA System ID. The CA System ID can be gotten by 
 *     UniversalClient_Extended_GetOperatorInfo().
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_ALREADY_STOPPED
 * @retval
 *    ::UC_ERROR_INVALID_CA_SYSTEM_ID
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_Extended_ResetCAData(const uc_ca_system_id *pCASystemID);

/**
 * Obtain the secure core version for multiple operators
 *
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
 * ::UniversalClient_GetSecureCoreVersion.
 *
 * This function can be called to obtain the current versions of Multi Secure Cores. These version can be used by applications to
 * check for supported features and application compatibility.  An application might also display a status screen with
 * version information on it that can be used for technical support purposes. 
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pVersion Receives the version string associated with Multi Secure Cores. 
 *     The uc_securecore_version::validOperatorCount indicates the number of current valid operators, and the uc_securecore_version::version 
 *     describes the securecore version information for specific operator.
 *     The uc_single_securecore_version::caSystemID indicates the operator by its CA System ID, and the uc_single_securecore_version::versionString
 *     contains the securecore information.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_Extended_GetSecureCoreVersion(uc_securecore_version *pVersion);

/** @} */ /* End of groups */


/** @defgroup tmsapi Cloaked CA Agent TMS APIs
 *  Cloaked CA Agent TMS APIs
 *
 *  These API methods provide interfaces needed by Targeted Messaging System (TMS). 
 *  TMS handles text and fingerprint messages to target specific groups of subscribers according to attributes, 
 *  which can be dynamically generated and set. These specific groups are identified by attribute settings, set by CA server. 
 *
 *  @{
 */


/**
* Set current service ID, transport ID and network ID for TMS
*
* This method is designed to set current tuned service information for TMS. The tuned service information
* is represented by the uc_dvb_tuned_info structure that contains serviceID, transportID and networkID. See ::uc_dvb_tuned_info.
*
* If TMS is activated, it must be called when any one of service ID, transport ID and networkID is changed.
* ServiceID/TransportID/NetworkID is one of the ways to set TMS data.
*
* \note If the client device application supports ONLY 1 descramble service, there is no need to invoke ::UniversalClient_SetTunedInfoList.
* ::UniversalClient_SetTunedInfo is designed to support 1 descramble service ONLY, and ::UniversalClient_SetTunedInfoList is designed to support multiple descramble services. 
* Each time this function is called, Univesal Client will clean up the existing tuned service info set via ::UniversalClient_SetTunedInfo or ::UniversalClient_SetTunedInfoList.
*
* @param[in] serviceID current service ID
*
* @param[in] transportID current tansport ID
*
* @param[in] networkID current network ID
*
* @retval 
*    ::UC_ERROR_SUCCESS
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List". 
*
*/
uc_result UniversalClient_SetTunedInfo(uc_uint16 serviceID, uc_uint16 transportID, uc_uint16 networkID);

/**
* Set a list of tuned services for TMS to Cloaked CA Agent.
*
* This method is designed to set a list of tuned services for TMS.
* If TMS is activated, it must be called when the tuned information is changed.
*
* \note This function is similar to ::UniversalClient_SetTunedInfo, but this function can set a list of tuned services to Cloaked CA Agent.
* Each time this function is called, Univesal Client will clean up the existing tuned service info set via ::UniversalClient_SetTunedInfo or ::UniversalClient_SetTunedInfoList.
*
* @param[in] tunedInfoCount The number of elements in the pTunedInfoList. If the value is 0, Cloaked CA Agent will clean up the existing tuned information and return directly. 
* When the value is 1, this function acts like ::UniversalClient_SetTunedInfo.
*
* @param[in] pTunedInfoList Pointer to a ::uc_dvb_tuned_info array containing tunedInfoCount items.
*
* @retval 
*    ::UC_ERROR_SUCCESS
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List". 
*
*/
uc_result UniversalClient_SetTunedInfoList(uc_uint32 tunedInfoCount, uc_dvb_tuned_info* pTunedInfoList);


/**
* Obtain TMS data
*
* This API is obsolete from Cloaked CA Agent 2.9.0. To get TMS data, 
* please refer to ::UniversalClient_Extended_GetTmsData.
*
* This function can be called to obtain TMS data. 
*
* \note The information from this method must be displayed in a GUI dialog. TMS data should be displayed in a hexadecimal format.
* TMS data from this method is for display purpose only.
*
*
* @param[in,out] pTmsData Receives TMS data.
*     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member 
*     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the TMS data,
*     in binary big-endian format, without any terminating character and set the uc_buffer_st::length member to the actual length of data returned. 
*     If the uc_buffer_st::length member is not large enough to hold the entire TMS data, the uc_buffer_st::length member
*     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application 
*     can then allocate memory of a sufficient size and attempt to call the function again.
*
*     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
*       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
*
*
* @retval 
*    ::UC_ERROR_SUCCESS
* @retval
*    ::UC_ERROR_NULL_PARAM
* @retval
*    ::UC_ERROR_INSUFFICIENT_BUFFER
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List". 
*
*/
uc_result UniversalClient_GetTmsData(uc_buffer_st* pTmsData);

/**
* Obtain TMS data for multiple operators.
*
* This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
* ::UniversalClient_GetTmsData.
*
* This function can be called to obtain TMS data for multiple operators.
*
* \note The information from this method must be displayed in a GUI dialog. TMS data should be displayed in a hexadecimal format.
* TMS data from this method is for display purpose only.
*
*
* @param[out] pTmsData Receives TMS data.
*     The uc_tms_data::validOperatorCount indicates the number of current valid operator.
*     The uc_tms_data::tms indicates the TMS data for specific operator.
*     The uc_tms_data_per_operator::caSystemID indicates the operator by its CA System ID.
*     The uc_tms_data_per_operator::tmsData contains detail TMS data.
*
*
* @retval 
*    ::UC_ERROR_SUCCESS
* @retval
*    ::UC_ERROR_NULL_PARAM
* @retval
*    ::UC_ERROR_INSUFFICIENT_BUFFER
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List". 
*
*/
uc_result UniversalClient_Extended_GetTmsData(uc_tms_data *pTmsData);

/**
* Set TMS attributes.
*
* The function shall be called when middleware wants to set client-based tms attributes.
*
*
* @param[in] attributeCount Indicate the count of client-based TMS attributes in pTmsAttributesItemInfoList.
*
* @param[in] pTmsAttributesItemInfoList Pointer to an array of ::uc_tms_attribute_item_info items.
*     The device application is responsible for maintaining this buffer.
*
*
* @retval 
*    ::UC_ERROR_SUCCESS
* @retval
*    ::UC_ERROR_NULL_PARAM
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List". 
*
*/
uc_result UniversalClient_SetTMSAttributes(uc_uint16 attributeCount,  uc_tms_attribute_item_info* pTmsAttributesItemInfoList);


/** @} */ /* End of groups */


/** @defgroup copycontrolapis Cloaked CA Agent Copy Control APIs
 *  Cloaked CA Agent Copy Control APIs
 *
 *  These APIs are for copy control functionality.
 *  In order to achieve copy control functionality, \ref copycontrolspi "Cloaked CA Agent Copy Control SPIs" must be implemented.
 *
 *  @{
 */

/**
  * Enable/Disable Macrovision
  *
  * \note This API is abandoned. Cloaked CA Agent will check the SPI interfaces to decide whether the client device supports Macrovision.
  * If the Macrovision SPI interfaces in \ref copycontrolspi "Cloaked CA Agent Copy Control SPIs" are implemented, Cloaked CA Agent will treat the client device 
  * as Macrovision supported.
  *
  * @param[in] enable Set to ::UC_TRUE if the platform supports Macrovision, else ::UC_FALSE.
  *    
  * @retval 
  *    ::UC_ERROR_SUCCESS
  * @retval
  *     Other The method may return any other errors listed in \ref result "Result Code List". 
  */
uc_result UniversalClient_EnableMacrovision(uc_bool enable);

/** @} */ /* End of groups */


/** @defgroup regionalfilteringapis Cloaked CA Agent Regional Filtering APIs
 *  Cloaked CA Agent Regional Filtering APIs
 *
 *  Regional Filtering is not a mandatory feature, but if the operator requires the client devices to implement it, then it is a must.
 *
 *  These APIs are for Regional Filtering.
 *  Regional Filtering is a feature to lock the secure client in a specific operational region.
 *  The region info is played out in certain service information tables, and also downloaded into the secure client.
 *  Cloaked CA Agent checks before descrambling whether the region info of the service and that for the secure client match, 
 *  unless Cloaked CA Agent or secure client is configured to ignore the checking.
 *
 *  @{
 */

/**
 * Enable or disable regional filtering
 *
 * This function can be used to enable or disable regional filtering from the application level.
 * If regional filtering is enabled, devices are prevented from descrambling services in
 * regions where they are not blocked. 
 *
 * By default, regional filtering is disabled.  The client does not remember the state
 * of regional filtering between restarts, so this function must be called at each initialization time.
 *
 * @param[in] enable Indicates whether to enable or disable regional filtering. If this value is ::UC_TRUE,
 *        Regional Filtering will be enabled. Otherwise, it will be disabled.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_EnableRegionalFiltering(uc_bool enable);


/**
 * Notify the client of a new regional filtering descriptor
 *
 * This function is intended to notify a client of a new IA regional filtering descriptor. 
 * A 'IA regional filtering descriptor' is a block of data that is intended to notify a device
 * of which geographical region it is in. If regional filtering is enabled, this information 
 * is later used to determine if a client is entitled to descramble services in this region.
 * A client is entitled to descramble services in a region if it has received an appropriate
 * 'regional filtering EMM' via an open EMM service.
 * 
 * In a typical \ref glossary_dvb "DVB" system, regional filtering descriptors are included 
 * as part of the 'CA descriptor' field within the \ref glossary_cat "CAT". In unusual configurations
 * or in other transports, the regional filtering may take different forms. 
 *
 * \note Regional filtering does not apply until it is enabled by calling ::UniversalClient_EnableRegionalFiltering.
 *
 * The format of the regional filtering descriptor is as follows
 *
 \code    
    Regional_Filtering_Descriptor()
    {
        descriptor_tag = 0xA0                       (8 bits)
        descriptor_length                           (8 bits)
     
        IA_Regional_Filtering_Descriptor()
        {
            descriptor_tag = 0xA0                   (8 bits)
            descriptor_version                      (8 bits)
            sector_count                            (8 bits)
            for(i=0;i<sector_count;I++)
            {
                sector_index                        (8 bits)
                region_info                         (8 bits)
                sub_region_info                     (8 bits)
            }
        }
    }
 \endcode
 *
 * \note IA_Regional_Filtering_Descriptor() is different from Regional_Filtering_Descriptor().
 *
    Explanation of the fields:
    - \b descriptor_tag: The 8-bit tag value for this descriptor will always be set to 0xA0. 
    - \b descriptor_length:  The length of the descriptor in bytes.
    - \b descriptor_tag: The 8-bit tag value for this descriptor will always be set to 0xA0. 
    - \b descriptor_version:  The 8-bit version value should be different for different descriptor. 
    - \b sector_count: The 8-bit count value should be the total sector count in this RF descriptor.
    - \b sector_index:  The 8-bit sector index value should be the index num of the sector.
    - \b region_info: The 8-bit region value should be the region of the sector.
    - \b sub_region_info: The 8-bit sub region value should be the sub region of the sector.
  *
  * @param[in] pDescriptor Buffer containing the raw contents of the IA_Regional_Filtering_Descriptor.
  *     The uc_buffer_st::bytes member must point to a buffer containing the descriptor, and
  *     the uc_buffer_st::length must be set to the number of bytes in the buffer.
  *
  * @retval 
  *    ::UC_ERROR_SUCCESS
  * @retval 
  *    ::UC_ERROR_OUT_OF_MEMORY
  * @retval
  *    ::UC_ERROR_NULL_PARAM
  * @retval
  *     Other The method may return any other errors listed in \ref result "Result Code List". 
  */
uc_result UniversalClient_NotifyRegionalFilteringDescriptor(
     const uc_buffer_st *pDescriptor);

/**
 * Obtains the list of Region Information
 *
 * This API is obsolete from Cloaked CA Agent 2.9.0. To get regional information, 
 * please refer to ::UniversalClient_Extended_GetRegionInfoList.
 *
 * This function can be called to obtain the list of region and subregion indexed by sector number. 
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pRegioninfoCount If the function succeeds, this receives the number of 
 *     elements returned in the ppRegioninfoList array.
 * @param[out] ppRegioninfoList If the function succeeds, this receives a pointer to a function-allocated
 *     array of ::uc_region_information structures.  When the caller no longer needs the information in the
 *     array, it must call ::UniversalClient_FreeRegionInfoList to free the array.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_MISSING_DATA
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetRegionInfoList(uc_uint32 *pRegioninfoCount, uc_region_information **ppRegioninfoList);

/**
 * Free resources allocated in ::UniversalClient_GetRegionInfoList.
 *
 * This function will free the memory allocated by a previous call to ::UniversalClient_GetRegionInfoList.
 *
 * @param[in,out] ppRegioninfoList On input, this is the address of the pointer received in a previous call to ::UniversalClient_GetRegionInfoList.
 *       On output, the function sets this to NULL. 
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".  
 */
uc_result UniversalClient_FreeRegionInfoList(uc_region_information **ppRegioninfoList);

/**
 * Obtains the list of Region Information for multiple operators.
 *
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
 * ::UniversalClient_GetRegionInfoList.
 *
 * This function can be called to obtain the list of region and subregion indexed by sector number. 
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pRegionalInfoList If the function succeeds, this receives a pointer to a function-allocated
 *     array of ::uc_regional_info_list structures. 
 *     The uc_regional_info_list::validOperatorCount indicates the number of current valid operator.
 *     The uc_regional_info_list::regionalInfoList indicates the region and subregion for specific operator.
 *     The uc_regional_info_list_per_operator::validSectorCount indicates the number of valid sector.
 *     The uc_regional_info_list_per_operator::regionInfoListPerOperator indicates the region and subregion information for specific sector.
 *     The uc_region_information::sector_number indicates the sector number which contains followed region information.
 *     The uc_region_information::region indicates the region information for the sector.
 *     The uc_region_information::subregion indicates the subregion information for the sector.
 *     The uc_region_information::caSystemID indicates the operator for the sector by its CA System ID.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_MISSING_DATA
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_Extended_GetRegionInfoList(uc_regional_info_list *pRegionalInfoList);


/** @} */ /* End of groups */



/** @defgroup testingapi Cloaked CA Agent Testing APIs
 *  Cloaked CA Agent Testing APIs
 *
 *  These API methods provide the necessary basic functionality to test secure chip with STB file from SCPC,
 *  and generate \ref glossary_tsc "TSC" CHIP record needed by SCPC to create upload files for this device.
 *
 *  Testing APIs are provided to support the creation of the TSC files on STB production line.
 *
 *  Please refer to \ref usestestingkeys "Use device testing APIs" on how to use these APIs.
 *
 *  @{
 */

/**
 * Max length of a \ref glossary_tsc "TSC" CHIP record.
 * Used by ::UniversalClient_CreateTscChipRecord
 */
#define SIZE_MAX_TSC_CHIP_RECORD (4096)


/**
 * Set testing CSSK and CW.
 *
 * This method is designed to test the secure chip key ladder.It must ONLY be called during the verification phase.
 * When this method is called, the Cloaked CA Agent will enter Test Mode. This method is only used by the application of Secure Chipset based STBs.
 * In Test Mode, any incoming ECM section will trigger a call to ::UniversalClientSPI_Device_SetCSSK or ::UniversalClientSPI_Device_SetExtendedCSSK 
 * and ::UniversalClientSPI_Stream_SetDescramblingKey with the CSSK and CW from this method.
 *
 * Irdeto will provide CW and CSSK together with a corresponding bitstream. 
 *
 * This method caches the CSSK and CW in a internal buffer and then returns.
 *
 * @param[in] pCssk CSSK key material to the device. This is normally protected by a secure chipset unique key
 *     (CSUK) that resides within the secure chipset such that an attacker gains no advantage by intercepting this method.
 *     The uc_buffer_st::bytes member points to a buffer containing the raw key material, and the uc_buffer_st::length member contains 
 *     the number of bytes in the buffer.
 * @param[in] pEvenDescramblingKeyInfo It's the information about the even key being used to descramble content. This contains the key algorithm, protection,
 *     cryptography context, and the key material. See ::uc_key_info.
 * @param[in] pOddDescramblingKeyInfo It's the information about the odd key being used to descramble content. This contains the key algorithm, protection,
 *     cryptography context, and the key material. See ::uc_key_info.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INVALID_HANDLE
 * @retval
 *    ::UC_ERROR_WRONG_HANDLE_TYPE
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_SetTestingKey(
    const uc_buffer_st *pCssk,
    const uc_key_info *pEvenDescramblingKeyInfo,
    const uc_key_info *pOddDescramblingKeyInfo
    );
    
/**
 * Set testing CSSK and CW with key infomation to support multi key ladder.
 *
 * This is the extension of the ::UniversalClient_SetTestingKey function with additional key protection infomation provided, 
 * in order to support different key ladders.It must ONLY be called during the verification phase.
 * This method is only used by the application of Secure Chipset based STBs.
 * When this method is called, the Cloaked CA Agent will enter Test Mode. Any incoming ECM section will trigger the call to 
 * ::UniversalClientSPI_Device_SetCSSK or ::UniversalClientSPI_Device_SetExtendedCSSK and ::UniversalClientSPI_Stream_SetDescramblingKey.
 *
 * Irdeto will provide the CW, CSSK, and their key protection info together with a corresponding bitstream. 
 *
 * This method caches the CSSK, CW, the related key protection info in a internal buffer and then returns.
 *
 *
 * @param[in] pCSSKInfo Information about the CSSK being used to protect the descrambling key. The CSSK is also protected by the CSUK.
 *     This contains CSSK protection and CSSK material.See ::uc_cssk_info.
 * @param[in] pEvenDescramblingKeyInfo It's information about the even key being used to descramble content. This contains the key algorithm, protection,
 *     cryptography context, and the key material. See ::uc_key_info.
 * @param[in] pOddDescramblingKeyInfo It's information about the odd key being used to descramble content. This contains the key algorithm, protection,
 *     cryptography context, and the key material. See ::uc_key_info.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INVALID_HANDLE
 * @retval
 *    ::UC_ERROR_WRONG_HANDLE_TYPE
 * @retval
 *     Others: The method may return any other errors listed in the \ref result "Result Code List". 
 */
uc_result UniversalClient_SetExtendedTestingKey(
    const uc_cssk_info * pCSSKInfo,
    const uc_key_info *pEvenDescramblingKeyInfo,
    const uc_key_info *pOddDescramblingKeyInfo
    );

/**
 * Set testing keys information that supports multiple mode, including IFCP mode.
 *
 * This method is designed to test the secure chip key ladder. It must ONLY be called during the verification phase.
 * When this method is called, the Cloaked CA Agent will enter Test Mode. This method is only used by the application of Secure Chipset based STBs.
 * In Test Mode, any incoming ECM section will trigger calls to SPI to set the testing keys from this method.
 *
 * @param[in] pTestingKeyInfo Includes all the information needed to perform the verification.
 *
 * If ::UC_SECURECHIP_PAIRING_MODE_MSR is set, ::UniversalClientSPI_Device_SetCSSK or ::UniversalClientSPI_Device_SetExtendedCSSK 
 * and ::UniversalClientSPI_Stream_SetDescramblingKey will be called.
 * If ::UC_SECURECHIP_PAIRING_MODE_IFCP is set, ::UniversalClientSPI_IFCP_Communicate will be called.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INVALID_HANDLE
 * @retval
 *    ::UC_ERROR_WRONG_HANDLE_TYPE
 * @retval
 *     Others: The method may return any other errors listed in the \ref result "Result Code List". 
 */
uc_result UniversalClient_SetTestingKeyForMultipleMode(
    const uc_testing_key_info * pTestingKeyInfo
    );
    
/**
 * Requests the Cloaked CA Agent to reload personalized data from Persistent Storage.
 *
 * This method can be used to cause the Cloaked CA Agent to reload the personalized data by calling
 * ::UniversalClientSPI_Device_GetPersonalizedData. 
 * 
 * \note This API is not used anymore.
 * 
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_ReloadPersonalizedData(void);


/**
 * Create a TSC CHIP record to describe the current device.
 *
 * This method generates a \ref glossary_tsc "TSC" CHIP record. This method is only used by the application of Secure Chipset based STBs.
 * The Irdeto SCPC must receive the \ref glossary_tsc "TSC" CHIP record in order to generate an upload file for this device.
 * A call to this method invokes the following SPIs.
 *     -# ::UniversalClientSPI_Device_GetCSSN
 *     -# ::UniversalClientSPI_Device_GetSecurityState
 *     -# ::UniversalClientSPI_Device_GetPlatformIdentifiers
 *     -# ::UniversalClientSPI_Device_GetPrivateData
 *     -# ::UniversalClientSPI_Device_GetChipConfigurationCheck
 *
 *
 * @param[in,out] tscChipRecord On input, Cloaked CA Agent will ZERO the incoming buffer.
 *     On output, it is filled with a NULL-ended ASCII string with a max length ::SIZE_MAX_TSC_CHIP_RECORD,
 *     suitable for being inserted into the relevant CHIP entry of the \ref glossary_tsc "TSC File".
 *     
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INVALID_HANDLE
 * @retval
 *    ::UC_ERROR_WRONG_HANDLE_TYPE
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_CreateTscChipRecord(
    uc_char tscChipRecord[SIZE_MAX_TSC_CHIP_RECORD]
    );

/** @} */ /* End of groups */


/** @defgroup ppvvodapis Cloaked CA Agent Pull VOD APIs
 *  Cloaked CA Agent Pull VOD APIs for transactional streaming mode.
 *
 *  These API methods provide the necessary functionality for \ref glossary_ppv_vod "PPV VOD".
 *
 *  In this solution, Cloaked CA Agent will generate the Client Transaction Data at the start of VOD session via ::UniversalClient_PPV_VOD_PrepareRequest.
 *  The application should submit the Client Transaction Data to the CA server and send the response from CA server to Cloaked CA Agent via ::UniversalClient_Extended_SendOobEmmSection.
 *  VOD content is regarded as a normal scrambled service. If the client is entitled to view the VOD content, control words will be output to the SPI via ::UniversalClientSPI_Stream_SetDescramblingKey
 *  as normal.
 *
 *  To cancel a VOD request, the client device application can call ::UniversalClient_PPV_VOD_CancelRequest with the nonce returned from ::UniversalClient_PPV_VOD_PrepareRequest.
 *
 *  Refer to \ref ppv_vod_client_communication "Pull VOD Flow - Transactional streaming mode" to get an 
 *  overview on how to play a \ref glossary_ppv_vod "PPV VOD" service.
 *
 *  For more details for integration code, please refer to \ref playppvvodservie "User plays Pull VOD service in transactional streaming mode".
 *
 *  \note Cloaked CA Agent ONLY provides \ref glossary_ppv_vod "PPV VOD" feature for platform with secure chipset support.
 *
 *  @{
 */

/**
 * Request the Client Transaction Data at the start of \ref glossary_ppv_vod "PPV VOD" session.
 * 
 * The client device application can call this method to get the Client Transaction Data from Cloaked CA Agent.
 * The Client Transaction Data is generated by Cloaked CA Agent in response to a VOD request.
 *
 * Refer to \ref ppv_vod_client_communication "Pull VOD Flow - Transactional streaming mode" to get an 
 * overview on how to play a \ref glossary_ppv_vod "PPV VOD" service.
 *
 * The Client Transaction Data is organized with the following TLV format.
 * The application can parse the TLV format to get related information, including the VOD nonce,
 * the client Unique Address, client version, etc.
 * 
 \code
Client_Transaction_Data()
{        
    Field Name                      Field Length     Field Value
    
    tag_number                      8   bits         The total number of the tags
    length                          16  bits         The total byte length of the tags
    for(i=0;i<tag_number;i++)                        Loop tag_number times for the tags
    {
        tag                         8   bits         Tag. This can be UC_TAG_TYPE_VOD_NONCE, UC_TAG_TYPE_UNIQUE_ADDRESS and UC_TAG_TYPE_CLIENT_VERSION currently.
        length(N)                   8   bits         Tag length
        value                       8*N bits         Tag value
    }
    HMAC                            64  bits         The HMAC value of the whole parameters.
                                                     The application do not need to care about it.  
}
 \endcode

 * \note For above tags, please refer to \ref vodtlvtags "Cloaked CA Agent TLV Tags for Pull VOD" for a list of TLV tags and its values.
 *
 * @param[out] pTransactionData Buffer containing the client transaction data. 
 *     If the function succeeds, the uc_buffer_st::bytes member points to a function-allocated buffer containing a Client_Transaction_Data(). 
 *     The uc_buffer_st::length member must be set to the number of bytes in the buffer.
 * 
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_VOD_CLIENT_TYPE_ERROR
 * @retval
 *    ::UC_ERROR_VOD_NO_SERIAL_NUMBER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 *
 * \note When the caller no longer needs the information, it must call ::UniversalClient_FreeBuffer 
         to free the uc_buffer_st::bytes member.
 *  
 */
uc_result UniversalClient_PPV_VOD_PrepareRequest(
    uc_buffer_st *pTransactionData );

/**
 * Cancel a \ref glossary_ppv_vod "PPV VOD" request to the client.
 *
 * The client device application can call this method to cancel a request to the client with a VOD nonce data.
 *
 * Refer to \ref ppv_vod_client_communication "Pull VOD Flow - Transactional streaming mode" to get an 
 * overview on how to play a \ref glossary_ppv_vod "PPV VOD" service.
 *
 * @param[in] pNonceData Buffer containing the nonce data.
 *     The uc_buffer_st::bytes member must point to a buffer containing the nonce data, 
 *     which is the VOD nonce previously retrieved by a call to ::UniversalClient_PPV_VOD_PrepareRequest. 
 *     The uc_buffer_st::length member must be set to the number of bytes in the buffer.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval 
 *    ::UC_ERROR_VOD_INVALID_NONCE
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_PPV_VOD_CancelRequest(
    const uc_buffer_st *pNonceData );    


/** @} */ /* End of groups */


/** @defgroup hashapis Cloaked CA Agent HAAS APIs
 *  Cloaked CA Agent HbbTV Authentication Application Service APIs.
 *
 *  @{
 */
/**
 * Query the hash code of the device.
 * @param[in] authType Authentication type, only support ::UC_BASIC_AUTHENTICATION now.
 * @param[in] timestamp (4 bytes) Seconds elapsed since Epoch (1st January 1970, UTC 00:00), should equal or be later than 1st January 2000, UTC 00:00.
 * @param[in, out] pHashCode
 * On input, pHashCode pHashCode->bytes point to actual memory to store hash code, pHashCode->length indicates max available bytes count. It is recommended to allocate 256 or more bytes for pHashCode.
 * On output, if the function is successfully, pHashCode will be updated with actual content and length of code. Middleware should use base64 encoder for the content and actual length returned rather than a string ending with 0x00. If the return value is UC_ERROR_INSUFFICIENT_BUFFER, the pHashCode->length will be updated with recommended value.
 * 
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *    ::UC_ERROR_TIMESTAMP_FILTER
 * @retval
 *    ::UC_ERROR_HASH_COMPUTE
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 *  
 */
uc_result UniversalClient_HAAS_GetHashCode(
    uc_authentication_type authType,
    uc_uint32 timestamp, uc_buffer_st* pHashCode );

/** @} */ /* End of groups */


/** @defgroup proximitydetectionapis Cloaked CA Agent Proximity Detection APIs
 *  Cloaked CA Agent Proximity Detection APIs.
 *
 *  @{
 */

/**
 * Session Start Parameters structure
 *
 * This structure is used for input parameters of ::UniversalClient_Proximity_StartSession.
 */
typedef struct _uc_proximity_start_parameters
{
    /**
     * indicates the peer device.
     */
    uc_proximity_peer peer;

} uc_proximity_start_parameters;

/**
 * Proximity property structure
 *
 * This structure is used for ::UniversalClient_Proximity_GetPropertyForSource.
 */
typedef struct _uc_proximity_property
{
    /**
     * indicates the status of the proximity session with a sink device.
     */
    uc_proximity_status_code statusCode;

} uc_proximity_property;

/**
 * Start a proximity session with another device.
 * It is called when a device is going to request data (e.g. PVR content) from another device.
 *
 * @param[in] pStartParam indicates the parameters used to start the session. 
 * 
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_PROXIMITY_DETECTION_DISABLED
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 *  
 */
uc_result UniversalClient_Proximity_StartSession(
    uc_proximity_start_parameters *pStartParam );

/**
 * Stop a proximity session if it will not be used any more.
 *
 * @param[in] peer indicates the session by peer. 
 * 
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_PROXIMITY_DETECTION_DISABLED
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 *  
 */
uc_result UniversalClient_Proximity_StopSession(
    uc_proximity_peer peer );

/**
 * Query the proximity session status when device works as source.
 * It is called before a device is going to share data to another device.
 *
 * @param[in] peer indicates the proximity session by peer. 
 * @param[out] pParam if the function is successfully, it will return the proximity detection property. 
 * 
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_PROXIMITY_DETECTION_DISABLED
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 *  
 */
uc_result UniversalClient_Proximity_GetPropertyForSource(
    uc_proximity_peer peer, uc_proximity_property *pParam );

/**
 * Encrypt the data that will be transferred to sink device.
 * It is called when a source device wants to protect the data that will be transferred to sink device.
 * The data input to this API must be a multiple of 16 bytes.
 *
 * @param[in] peer indicates the peer sink device who requested the data. 
 * @param[in,out] pData the data to be encrypted, must be a multiple of 16 bytes. After running the function, it will be the encrypted data on success.
 * 
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_PROXIMITY_DETECTION_DISABLED
 * @retval 
 *    ::UC_ERROR_PROXIMITY_INVALID_DATA
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 *  
 */
uc_result UniversalClient_Proximity_EncryptData(
    uc_proximity_peer peer, uc_buffer_st *pData );

/**
 * Decrypt the data received from source device.
 * It is called when a sink device receives encrypted data from source device.
 * The data input to this API must be a multiple of 16 bytes.
 *
 * @param[in] peer indicates the peer source device who encrypted the data. 
 * @param[in,out] pData the data to be decrypted, must be a multiple of 16 bytes. After running the function, it will be the decrypted data on success.
 * 
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_PROXIMITY_DETECTION_DISABLED
 * @retval 
 *    ::UC_ERROR_PROXIMITY_INVALID_DATA
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 *  
 */
uc_result UniversalClient_Proximity_DecryptData(
    uc_proximity_peer peer, uc_buffer_st *pData );


/** @} */ /* End of groups */

/** @defgroup hgpcapi Cloaked CA Agent Home Gateway Proximity Control APIs
 *  Cloaked CA Agent Home Gateway Proximity Control APIs
 *
 *  These API methods provide interfaces needed by Home Gateway Proximity Control (HGPC). 
 *
 *  @{
 */

/**
 * Obtain the Home Gateway Proximity Control information.
 *
 * This method can be called to obtain the HGPC configuration from the secure client.
 * Generally it is called when ::UC_GLOBAL_NOTIFY_API_AVAILABLE is notified after Cloaked CA Agent resets or ::UC_GLOBAL_NOTIFY_HGPC_UPDATED 
 * is notified when HGPC information updates.
 *
 * @param[out] pHgpcInfo points to the buffer to store the HGPC configuration data. Middleware handles this buffer's alloc and free.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetHGPCInfo(uc_hgpc_info* pHgpcInfo);

/**
 * Middleware calls this API to notify received HNA message to Cloaked CA Agent.
 *
 * This method can be called to notify Cloaked CA Agent of the HNA message. The message size is ::UC_HGPC_MESSAGE_SIZE bytes.
 *
 * @param[in] pHNAMessage points to the buffer which stores the received HNA message data. MW handles this buffer's allocation and free.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_NotifyHNAMessage(const uc_byte* pHNAMessage);

/**
 * Middleware calls this API to enable the ::UC_GLOBAL_NOTIFY_HGPC_REMINDER and ::UC_GLOBAL_NOTIFY_HGPC_EXPIRY notification.
 *
 * This method can be called to enable HGPC reminder and expiry notification.
 *
 * @param[in] bSwitchOn switches the HGPC reminder notification on or off. The reminder can only be switched on the HGPC secondary 
 * secure client. If UC_TRUE is set, the reminder is switched on and ::UC_GLOBAL_NOTIFY_HGPC_REMINDER is sent every 60 seconds 
 * and ::UC_GLOBAL_NOTIFY_HGPC_EXPIRY is sent when the HNA message expires or the HNA message is not received after the secondary secure client is active.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_HGPC_INVALID_CLIENT
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_SwitchHGPCReminder(uc_bool bSwitchOn);

/** @} */ /* End of groups */

/** @} */ /* End of Univeral Client APIs */


/** @defgroup convergentclient_api Unified Client Specific APIs

 *  Unified Client APIs
 *
 *  These API methods provide the necessary interfaces for the Unified Client which includes the functionality
 *  of Cloaked CA Agent and Softcell. Some of the following APIs are used to get information of Softcell or smartcard,
 *  such as ::UniversalClient_GetSoftcellVersion, ::UniversalClient_GetSmartcardStatus.
 *  Some of the APIs are used to replace the corresponding Unified Client APIs that are only for Cloaked CA Agent,
 *  such as ::UniversalClient_GetConvergentServiceStatus.
 
 *  However, not all APIs have its Unified Client specific version. Some of the Cloaked CA Agent APIs can be used 
 *  no matter it is Cloaked CA Agent or Unified Client. And these APIs can be divided into 2 types. One is APIs without 
 *  Cloaked CA or Softcell concept, such as ::UniversalClient_GetCSSN, ::UniversalClient_OpenService. The other 
 *  is APIs including ::uc_global_source_type in their input or output parameters, such as ::UniversalClient_GetProductList.
 *  @{
 */


/**
 * Obtain the current version of Softcell.
 *
 * This function can be called to obtain the current version of Softcell. This version can be used by applications to
 * check for supported features and application compatibility.  An application might also display a status screen with
 * version information on it that can be used for technical support purposes. 
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pVersion Receives the version string associated with Softcell. 
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the version
 *     string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire version string, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application 
 *     can then allocate memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and the length includes 1 byte NULL terminator.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetSoftcellVersion( uc_buffer_st *pVersion);


/**
 * Obtain the build information of Softcell
 *
 * This function can be called to obtain the build information of Softcell. 
 * The application shall display the build info in the client status screen.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pBuildInfo Receives the build string associated with the client. 
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the build
 *     string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire build string, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application 
 *     can then allocate memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and the length includes 1 byte NULL terminator.
 *
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetSoftcellBuildInfo(
    uc_buffer_st *pBuildInfo);

    
/**
 * Obtain the status of smartcard.
 *
 * This function can be called to obtain smartcard status. 
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in] smartcardID smartcard id. This parameter is not used.
 * @param[out] pSmartcardStatus If the function succeeds, the smartcard status information is copied in it.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".  
 */
uc_result UniversalClient_GetSmartcardStatus( 
    uc_uint32 smartcardID, 
    uc_smartcard_status* pSmartcardStatus );


/**
 * Obtain the smartcard infomation.
 *
 * This function can be called to obtain smartcard infomation. 
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in] smartcardID smartcard id. This parameter is not used.
 * @param[out] pSmartcardInfo If the function succeeds, the smartcard info is copied in it.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".  
 */
uc_result UniversalClient_GetSmartcardInfo( 
    uc_uint32 smartcardID, 
    uc_smartcard_info* pSmartcardInfo );

    
/**
 * Obtain the CA System ID from smartcard.
 *
 * This method can be called to obtain the active CA sytem ID from the smartcard.
 *
 * @param[in] smartcardID smartcard id. This parameter is not used.
 * @param[out] pCaSystemID If the function succeeds, the active CA system ID in smartcard is copied in it.
 *
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetSmartcardCASystemID(
    uc_uint32 smartcardID,
    uc_uint32 *pCaSystemID);


/**
 * Obtain the nationality of smartcard.
 *
 * This function can be called to obtain the nationality of Smartcard. 
 * The nationality is described by three ISO 3166-1 Alpha-3 identifier (e.g. 'CHN', 'GBR', 'USA'). 
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in] smartcardID smartcard id. This parameter is not used.
 * @param[in,out] pNationality Receives the nationality associated with the client. 
 *     On output, the uc_buffer_st::bytes member will be filled with a NULL-terminated string.  
 *
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the nationality 
 *     , including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire nationality, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_NO_NATIONALITY
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetSmartcardNationality(
        uc_uint32 smartcardID,
        uc_buffer_st *pNationality);

/**
 * Obtain the TMS data of smartcard.
 *
 * This function can be called to obtain TMS data of Smartcard. 
 *
 * @param[in] smartcardID smartcard id. This parameter is not used.
 * @param[in,out] pTMSData Receives the TMS data of smartcard.
 *
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the nationality 
 *     , including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire nationality, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
 *
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *    ::UC_ERROR_ALREADY_STOPPED
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetSmartcardTMSData(
    uc_uint32 smartcardID,    
    uc_buffer_st *pTMSData);        

/**
 * Obtain the unique address of smartcard.
 *
 * This function can be called to obtain smartcard unique address. 
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in] smartcardID smartcard id. This parameter is not used.
 * @param[out] pSmartcardUA receives the smartcard emm filter number and the smartcard Unique Address.
 *      The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length
 *      member to the maximum length of the buffer, in bytes. The function will modify the contents of the buffer
 *      to contain the emm filter number and the Smart Card Unique Address in binary big-endian format, without any terminating character, and set the 
 *      uc_buffer_st::length member to the actual length of data returned. If the uc_buffer_st::length member is not large enough
 *      to hold the entire Smart Card Unique Address, the uc_buffer_st::length member will be set to the minimum required value,
 *      and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application can then allocate memory of a sufficient size
 *      and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
 *     \n
 *     \note     
 *     Suppose the emm filter number is 24(= 0x18) and the Smart Card Unique Address is 12345678(=0xBC614E), 
 *     uc_buffer_st::bytes will have the following values: \n
 *      uc_buffer_st::bytes[0] = 0x18, \n
 *      uc_buffer_st::bytes[1] = 0xBC, \n
 *      uc_buffer_st::bytes[2] = 0x61, \n
 *      uc_buffer_st::bytes[3] = 0x4E, \n
 *     and uc_buffer_st::length is set to 4.
 *     \n
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
  * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".  
 */
uc_result UniversalClient_GetSmartcardUniqueAddress( 
    uc_uint32 smartcardID, 
    uc_buffer_st* pSmartcardUA );

/**
 * Obtain the sectors address from smartcard.
 *
 * This method can be called to obtain the sectors address from the smartcard.
 *
 * @param[in] smartcardID smartcard id. This parameter is not used.
 * @param[out] pSmartcardSectorsAddress If the function succeeds, the sectors address in smartcard is copied in it.
 *
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_GetSmartcardSectorsAddress ( 
    uc_uint32 smartcardID, 
    uc_smartcard_sectors_address * pSmartcardSectorsAddress );

/** @} */ /* End of Unified Client APIs */


#ifdef __cplusplus
}
#endif

#endif /* !UNIVERSALCLIENT_API_H__INCLUDED__ */

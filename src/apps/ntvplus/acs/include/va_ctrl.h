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


#ifndef _VA_CTRL_H_
#define _VA_CTRL_H_

#include "va_types.h"
#include "va_errors.h"
#include "va_setup.h"


/**
    @addtogroup ctrlAPI
    @brief This section describes the VA_CTRL API
    @{
*/

/**
    @defgroup ctrlConst VA_CTRL Constants
    @brief The VA_CTRL constants
     
    In the va_errors.h header file:
    - @ref kVA_OK
    - @ref kVA_DATA_NOT_AVAILABLE
    - @ref kVA_INVALID_PARAMETER
    - @ref kVA_ALREADY_DONE
    - @ref kVA_OPEN_ACS_FIRST
    - @ref kVA_SWITCH_ON_PROGRAM_FIRST
    - @ref kVA_ILLEGAL_HANDLE
    - @ref kVA_TRANSITION_NOT_ALLOWED
    - @ref kVA_NOT_IMPLEMENTED

    In the va_setup.h header file:
    - @ref kVA_SETUP_NBMAX_ACS This constant indicates the maximum number of ACS instances permitted by Viaccess-Orca.
    - @ref kVA_SETUP_NBMAX_SC This constant indicates the maximum number of smartcard slots permitted by Viaccess-Orca. 
 @{
*/

/** Size of serial number in bytes */
#define kVA_SERIAL_NUMBER_SIZE  12

/** @} */


/**
 @defgroup ctrlEnum VA_CTRL Enumerations
 @brief The VA_CTRL enumerations
 @{
*/

/**
    The STB calls the @ref VA_CTRL_OpenAcsInstance function to open a given ACS instance in a specific operating
    mode or the @ref VA_CTRL_ChangeAcsInstanceMode function to dynamically change the operating mode of
    that ACS instance. The type tVA_CTRL_AcsMode enumerates the possible ACS operating modes:
    @note The "multiple" operating modes (i.e. from @ref eLIVE_AND_PERSISTENT_RECORD to @ref eTIMESHIFT_PLAYBACK_AND_PERSISTENT_RECORD)
    are used to ensure that only one ACS instance is used for each program source. The program source can be the live, the persistent 
    mass storage, or the non-persistent mass storage.
    @n The STB manufacturer must adapt the use of these modes according to STB hardware and software features.
 */
typedef enum
{
    eLIVE = 0,                                          /**< Mode used to descramble a live DVB program */
    ePERSISTENT_RECORD,                                 /**< Mode used to persistently record an incoming scrambled DVB program to a mass storage device (for example, a hard drive) */
    ePERSISTENT_PLAYBACK,                               /**< Mode used to descramble and play back a DVB program that was previously persistently recorded */
    eTIMESHIFT_RECORD,                                  /**< Mode used to record an incoming scrambled DVB program to a mass storage device. This recording is meant to be non-persistent and must be used for timeshifting purposes only */
    eTIMESHIFT_PLAYBACK,                                /**< Mode used to descramble and play back a DVB program that was previously non-persistently recorded */
    eLIVE_AND_PERSISTENT_RECORD,                        /**< Mode used to descramble a live DVB program and at the same time persistently record this scrambled DVB program to a mass storage device */
    eLIVE_AND_TIMESHIFT_RECORD,                         /**< Mode used to descramble a live DVB program and at the sametime non-persistently record this scrambled DVB program to a mass storage device for timeshifting purposes only */
    eLIVE_AND_TIMESHIFT_RECORD_AND_PERSISTENT_RECORD,   /**< Mode used to descramble a live DVB program and at the same to both persistently and non-persistently record this scrambled DVB program to a mass storage device */
    eTIMESHIFT_RECORD_AND_PERSISTENT_RECORD,            /**< Mode used to both persistently and nonpersistently record an incoming scrambled DVB program to a mass storage device */
    eTIMESHIFT_PLAYBACK_AND_PERSISTENT_RECORD           /**< Mode used to persistently record a scrambled DVB program that was previously non-persistently recorded. This mode must therefore not be used to actually play back the scrambled DVB program (no descrambling) */
} tVA_CTRL_AcsMode;

/**
    Viaccess-Orca uses the pfVaStreamNotificationCallback function to notify the STB about the descrambling events
    occurring on a given elementary stream.
 */
typedef enum
{
    eCLEAR = 0,                 /**< The stream contains no associated CA_Descriptors in the PMT. The stream is therefore considered unscrambled. */
    eVIACCESS_NOT_APPLICABLE,   /**< The stream contains associated CA_Descriptors in the PMT, however it contains no Viaccess-Orca CA_Descriptors */
    eWAITING_FOR_VIACCESS,      /**< The stream contains associated Viaccess-Orca CA_Descriptors in the PMT. Viaccess-Orca therefore proceeds to descramble the stream. Depending on the access conditions available in the security module and in the ECMs, Viaccess-Orca then sends either a eVIACCESS_DESCRAMBLING or eVIACCESS_NOT_DESCRAMBLING message. */
    eVIACCESS_NOT_DESCRAMBLING, /**< Viaccess-Orca will not descramble the stream because either no security module is available, the access conditions required by the stream are not fulfilled by the security module, or a stream error occurred. */
    eVIACCESS_DESCRAMBLING      /**< Viaccess-Orca is descrambling the stream */
} tVA_CTRL_StreamDescramblingEvent;

/**
    The STB can use the VA_CTRL_QueryStream function in order to check whether Viaccess-Orca can descramble or
    not a given elementary stream. The tVA_CTRL_StreamCaEnableEvent enumerations roughly correspond to
    the CA_enable field from the DVB CA_PMT_Reply object, but a closer match will be provided in future
    versions.
 */
typedef enum
{
    eQUERY_NO,              /**< This is a response to a VA_CTRL_QueryStream request. Viaccess-Orca cannot descramble the stream */
    eQUERY_MAYBE,           /**< This is a response to a VA_CTRL_QueryStream request. The Viaccess-Orca system found a Viaccess-Orca CA_Descriptor associated with the stream. However, this does not guarantee that the ECMs are actually broadcast or that the security module has the appropriate rights to decrypt the stream */
    eQUERY_YES              /**< This response to a VA_CTRL_QueryStream request means that Viaccess-Orca can play the stream */
} tVA_CTRL_StreamCaEnableEvent;

/**
    Viaccess-Orca uses the pfVaStreamNotificationCallback function to inform the STB whether a given
    elementary stream can be recorded or not for each type of recording included in the current ACS mode
   (persistent or non-persistent). The tVA_CTRL_StreamRecordPolicy enumeration provides information on
    the existing recording policies.
 */
typedef enum 
{ 
    eRECORD_SCRAMBLED,      /**< The stream must be persistently recorded scrambled. In this case, Viaccess-Orca will not provide the control words to the associated stream descrambler. */
    eRECORD_FORBIDDEN,      /**< The persistent recording of the stream is strictly forbidden */
    eTIMESHIFT_SCRAMBLED,   /**< The stream must be non-persistently recorded scrambled for timeshift. In that case, Viaccess-Orca will not provide the control words to the associated stream descrambler */
    eTIMESHIFT_FORBIDDEN    /**< The non-persistent recording of the stream for timeshift is strictly forbidden */
} tVA_CTRL_StreamRecordPolicy;

/** 
    Viaccess-Orca uses the pfVaStreamNotificationCallback function to send to the STB different notifications
    about each added elementary stream. The STB must use this enumeration in order to correctly identify the
    nature of the received notification. The tVA_CTRL_StreamNotificationType enumeration specifies the
    available notification types.
 */
typedef enum
{
    eSTREAM_DESCRAMBLING_NOTIFICATION,  /**< The received notification is a tVA_CTRL_StreamDescramblingEvent */
    eRECORD_POLICY_NOTIFICATION,        /**< The received notification is a tVA_CTRL_StreamRecordPolicy */
    eMATURITY_RATING_NOTIFICATION       /**< The received notification contains the current maturity rating of the elementary stream */
} tVA_CTRL_StreamNotificationType;

/**
    The STB can use the @ref VA_CTRL_FileTransferRegister function in order to register to a file transfer for a file transfer type.
 */

typedef enum {
      eFT_OPERATOR_FILE_TYPE_01 = 0xFF, /**< File transfer type number 01 */
      eFT_OPERATOR_FILE_TYPE_02 = 0xFE, /**< File transfer type number 02 */
      eFT_OPERATOR_FILE_TYPE_03 = 0xFD, /**< File transfer type number 03 */
      eFT_OPERATOR_FILE_TYPE_04 = 0xFC, /**< File transfer type number 04 */
      eFT_OPERATOR_FILE_TYPE_05 = 0xFB, /**< File transfer type number 05 */
      eFT_OPERATOR_FILE_TYPE_06 = 0xFA, /**< File transfer type number 06 */
      eFT_OPERATOR_FILE_TYPE_07 = 0xF9, /**< File transfer type number 07 */
      eFT_OPERATOR_FILE_TYPE_08 = 0xF8, /**< File transfer type number 08 */
      eFT_OPERATOR_FILE_TYPE_09 = 0xF7, /**< File transfer type number 09 */
      eFT_OPERATOR_FILE_TYPE_10 = 0xF6, /**< File transfer type number 10 */
      eFT_OPERATOR_FILE_TYPE_11 = 0xF5, /**< File transfer type number 11 */
      eFT_OPERATOR_FILE_TYPE_12 = 0xF4, /**< File transfer type number 12 */
      eFT_OPERATOR_FILE_TYPE_13 = 0xF3, /**< File transfer type number 13 */
      eFT_OPERATOR_FILE_TYPE_14 = 0xF2, /**< File transfer type number 14 */
      eFT_OPERATOR_FILE_TYPE_15 = 0xF1, /**< File transfer type number 15 */
      eFT_OPERATOR_FILE_TYPE_16 = 0xF0  /**< File transfer type number 16 */

} tVA_CTRL_FileType;

/** @} */


/**
 @defgroup ctrlAliases VA_CTRL Type Aliases
 @brief The VA_CTRL type aliases
 @{
 */

/**
    This type defines the STB serial number. The serial number must be coded in BCD.
    If the box serial number is 000000000000000000291186, the STB manufacturer must set the following attributes:
    @n `pserialNumber[0] = 0x00 ;`
    @n `pserialNumber[1] = 0x00 ;`
    @n `pserialNumber[2] = 0x00 ;`
    @n `pserialNumber[3] = 0x00 ;`
    @n `pserialNumber[4] = 0x00 ;`
    @n `pserialNumber[5] = 0x00 ;`
    @n `pserialNumber[6] = 0x00 ;`
    @n `pserialNumber[7] = 0x00 ;`
    @n `pserialNumber[8] = 0x00 ;`
    @n `pserialNumber[9] = 0x29 ;`
    @n `pserialNumber[10] = 0x11 ;`
    @n `pserialNumber[11] = 0x86 ;`

    @note The serial number must have the same value as the one provided to the BlackBox application.
 */
typedef BYTE tVA_CTRL_StbSerialNb[kVA_SERIAL_NUMBER_SIZE];

/**
    SOID are numbers that are used in order to identify the service operator
    @note The most significant byte of the `tVA_CTRL_Soid` data is not used.
         `tVA_CTRL_Soid` values therefore range from 0x000000 to 0xFFFFFF
 */
typedef UINT32 tVA_CTRL_Soid;

 /** @} */

/**
 @defgroup ctrlStruct VA_CTRL Structures
 @brief The VA_CTRL structures
 @{
 */

/**
    The tVA_CTRL_StbInformation structure contains information on the hardware and software used in the STB.
 */
typedef struct
{
    tVA_CTRL_StbSerialNb stBoxIdentity;             /**< The STB serial number information structure is described in @ref tVA_CTRL_StbSerialNb */
    WORD                 wConstructorIdentifier;    /**< STB manufacturer identifier. This value is provided by Viaccess-Orca */
    WORD                 wModelIdentifier;          /**< Model version identifier. This value is provided by Viaccess-Orca */
    BYTE                 byHardwareType;            /**< Specifies the hardware type of the decoder */
    UINT8                uiHardwareVersion;         /**< Specifies the hardware version of the decoder */
    BYTE                 bySoftwareType;            /**< Specifies the software type of the decoder */
    UINT8                uiSoftwareVersion;         /**< Specifies the software version of the decoder */
} tVA_CTRL_StbInformation;

/**
    The tVA_CTRL_Demux structure gives basic information on the VA_DMX driver
 */
typedef struct
{
    UINT16 uiMaxNumberOfFilters;    /**< The maximum number of filters that can be associated with one ACS */
    UINT16 uiMaxSizeOfFilter;       /**< The maximum size of a filter in bytes */
} tVA_CTRL_Demux;

/**
    The tVA_CTRL_Descrambler structure contains information on the VA_DSCR driver
 */
typedef struct
{
    UINT16 uiMaxNumberOfChannels;   /**< Maximum number of channels that the descrambler hardware is able to handle at the same time.
                                         A channel corresponds to a PID */
} tVA_CTRL_Descrambler;

/**
    The tVA_CTRL_AcsResources structure represents the VA_DMX and VA_DSCR drivers that are each implicitly associated with an ACS.
 */
typedef struct
{
    tVA_CTRL_Demux       stDemux;       /**< Gives information on the current VA_DMX driver */
    tVA_CTRL_Descrambler stDescrambler; /**< Gives information on the current VA_DSCR driver */
}  tVA_CTRL_AcsResources;

/**
    The tVA_CTRL_ConfigurationParameters structure sets the STB parameters and features.
    It is the only argument of the @ref VA_CTRL_Init function
 */
typedef struct
{
    UINT32                  uiNbAcs;                    /**< Number of ACS required to manage the STB hardware.
                                                             The value of uiNbAcs must always be lower or equal to @ref kVA_SETUP_NBMAX_ACS and greater than 0. */
    UINT32                  uiNbSmartCard;              /**< Used to set the number of smartcard slots. 
                                                             The value of uiNbSmartCard must always be lower or equal to @ref kVA_SETUP_NBMAX_SC and greater than 0 */
    tVA_CTRL_AcsResources   aAcs[kVA_SETUP_NBMAX_ACS];  /**< The aAcs parameter is an array of the @ref tVA_CTRL_AcsResources structures.
                                                             Each structure gives the description of one ACS object */
    tVA_CTRL_StbInformation stStbInformation;           /**< The stStbInformation parameter sets the details regarding the STB manufacturer.
                                                             For more information refer to @ref tVA_CTRL_StbInformation */
} tVA_CTRL_ConfigurationParameters;

/**
    This structure contains the current network and program information. Viaccess-Orca retrieves this data using the
    @ref VA_CTRL_GetProgramInformation function
 */
typedef struct
{
    WORD  wNetworkId;           /**< The network identifier carried in the NIT table */
    WORD  wTransportStreamId;   /**< Value of the current transport stream identifier. This value is contained in the SDT and the NIT tables */
    WORD  wOriginalNetworkId;   /**< Value that identifies the network of the originating delivery system */
    WORD  wServiceId;           /**< The service identifier labels the current program switched on.
                                     This value is contained in the SDT and the PMT table, under Program Number */
} tVA_CTRL_ProgramInformation;

/** @} */


/**
 @defgroup ctrlUnion VA_CTRL Unions
 @brief The VA_CTRL unions
 @{
 */

/**
    This union encapsulates the value of the notification sent by Viaccess-Orca to the STB via the
    pfVaStreamNotificationCallback function. The STB must interpret the actual value of the union using
    the tVA_CTRL_StreamNotificationType enumeration
 */
typedef union
{
    tVA_CTRL_StreamDescramblingEvent    eStreamDescramblingEvent;   /**< The notification information is one of the values defined by the tVA_CTRL_StreamDescramblingEvent enumeration */
    tVA_CTRL_StreamRecordPolicy         eStreamRecordPolicy;        /**< The notification information is one of the values defined by the tVA_CTRL_StreamRecordPolicy enumeration */
    UINT8                               uiStreamMaturityRating;     /**< The notification information is the current maturity rating of the elementary stream */
} tVA_CTRL_StreamNotificationInfo;

/** @} */


/**
 @defgroup ctrlCbFunctions VA_CTRL Callback Functions
 @brief The VA_CTRL API uses two callback functions to send notifications to the STB.
 @{
 */

/**
    This function is used by Viaccess-Orca to send to the STB different notifications about an added elementary stream.
    The STB passes this callback as a parameter of function VA_CTRL_AddStream.
    
    @param [in] dwStbStreamHandle       The handle of the elementary stream (as provided in VA_CTRL_AddStream) that triggers the notification
    @param [in] eStreamNotificationType The actual notification type as defined by the tVA_CTRL_StreamNotificationType
    @param [in] uStreamNotificationInfo The actual notification value. The STB must use the appropriate field of the 
                                 tVA_CTRL_StreamNotificationInfo union based on the notification type specified by the eStreamNotificationType parameter
    @retval kVA_OK      If the notification is successful
    @retval kVA_ERROR   For any other reason
 */
typedef INT (*tVA_CTRL_StreamNotificationCallback)(DWORD dwStbStreamHandle,
                                                   tVA_CTRL_StreamNotificationType eStreamNotificationType,
                                                   tVA_CTRL_StreamNotificationInfo uStreamNotificationInfo);

/**
    This function is used by Viaccess-Orca to send a reply to the STB after a call of the VA_CTRL_QueryStream function.
    @param [in] dwStbStreamHandle       The handle of the elementary stream (as provided in VA_CTRL_AddStream) for which the reply is sent
    @param [in] eStreamCaEnableEvent    The actual notification value as defined by the tVA_CTRL_StreamCaEnableEvent enumeration
    
    @retval kVA_OK      If the notification is successful
    @retval kVA_ERROR   For any other reason
 */
typedef INT (*tVA_CTRL_QueryStreamNotificationCallback)(DWORD dwStbStreamHandle,
                                                        tVA_CTRL_StreamCaEnableEvent eStreamCaEnableEvent);

/**
    This function is used by Viaccess-Orca to send a reply to the STB after a file transfer ended.
    @param [in] dwFileTransferHandle   The handle of the file transfer (as provided in @ref VA_CTRL_FileTransferRegister) for which the reply is sent
    @param [in] eFileType              The file type value as defined by the @ref tVA_CTRL_FileType enumeration
    @param [in] uiBufferLength         The length of the transferred file buffer
    @param [in] pFileBuffer            Pointer to the transferred file buffer

*/
typedef void (*tVA_CTRL_FileTransferCallback) (DWORD dwFileTransferHandle, tVA_CTRL_FileType eFileType, UINT32 uiBufferLength, BYTE* pFileBuffer);

/** @} */



/**
 @defgroup viaCtrlFunc VA_CTRL Viaccess-Orca Functions
 @brief The VA_CTRL API functions provided by Viaccess-Orca.

 @{
  All pointers in parameters of the following functions are allocated and freed by STB manufacturer.
  They can be freed as soon as the function returns. 
*/

/**
    This function is used to configure and initialize the Viaccess-Orca process. This configuration then informs the ACS
    about the decoder features. The initialization of the Viaccess-Orca process creates all the internal objects that are
    used by the ACS system.
    
    @param [in] pParameters Specifies all of the parameters that the ACS requires for configuration and initialization.
                        The structure is allocated and filled by the STB manufacturer
    @retval kVA_OK                  If both the configuration and initialization are successful
    @retval kVA_ALREADY_DONE        If the ACS is already initialized
    @retval kVA_INVALID_PARAMETER   If one of the parameters of the tVA_CTRL_ConfigurationParameters structure does not fulfill the conditions it must respect

    @b Comments
    @n The STB can manage several input sources, such as for example, two tuners, a hard disk, and a fast
    communication bus. Each input corresponds to a hardware resource which, in turn, corresponds to an ACS
    object in the Viaccess-Orca ACS. The VA_CTRL_Init() function informs Viaccess-Orca how many ACS objects it must create.
    @n The following graphic shows an example of the hardware configuration:
    @image html  HWConfig.png "Example of hardware configuration"
    @image latex HWConfig.png "Example of hardware configuration"
    @n Viaccess-Orca creates the number of ACS objects set by the uiNbAcs parameter. These ACS objects are identified by
    their index (from 0 to uiNbAcs-1) in the array aAcs[] as specified in the `tVA_CTRL_ConfigurationParameters` structure.
    The other drivers use this mechanism to determine which ACS they have to address.
    @n One ACS object is always associated with a set of three drivers: VA_DMX, VA_DSCR and VA_CTRL.
    
    For more information refer to @ref tVA_CTRL_ConfigurationParameters.
*/
INT   VA_CTRL_Init(const tVA_CTRL_ConfigurationParameters *pParameters);

/**
    The VA_CTRL_Start function starts the Viaccess-Orca ACS thread.

    @b Comments
    @n It is this function that starts the ACS thread once the ACS system has been configured and initialized.
    @n Once this function is called, the calling thread is blocked until another thread calls the @ref VA_CTRL_Stop
    function and the ACS system is fully stopped.
*/
void  VA_CTRL_Start(void);

/**
    The VA_CTRL_Stop function stops the Viaccess-Orca ACS thread.
     
    @retval kVA_OK                  If the request is correctly accepted
    @retval kVA_ALREADY_DONE        If the ACS is already stopped
     
    @b Comments
    @n The ACS system is considered fully stopped when:
    - The return value of the call is kVA_OK.
    - The VA_CTRL_Start function returns.
    @n Once the ACS system is stopped the @ref VA_CTRL_Init() method must be called before starting the ACS again.
*/
INT   VA_CTRL_Stop(void);


/**
    The STB calls the VA_CTRL_OpenAcsInstance function to open an ACS instance in a given operating mode.
    This function must be called before any other operation on this ACS instance.

    @param [in] dwAcsId     The value of this argument lies between 0 and uiNbAcs-1, with uiNbAcs the number of ACS instances
                            required by the STB at initialization. The dwAcsId parameter specifies the ACS instance to be opened.
    @param [in] eAcsMode    The parameter specifies the operating mode of the ACS to be opened.
                            This mode is a value of the tVA_CTRL_AcsMode enumeration

    @retval kVA_OK                  If the request is successfully opened
    @retval kVA_INVALID_PARAMETER   Either if the parameter dwAcsId is out of range or if eAcsMode is not a value of the @ref tVA_CTRL_AcsMode enumeration.
    @retval kVA_ALREADY_DONE        If the ACS is already opened.
*/
INT   VA_CTRL_OpenAcsInstance(DWORD dwAcsId, tVA_CTRL_AcsMode eAcsMode);


/**
    The STB calls the VA_CTRL_ChangeAcsInstanceMode function to dynamically change an operating mode
    for a given ACS instance. This function must be called after the VA_CTRL_OpenAcsInstance() function and before
    the VA_CTRL_CloseAcsInstance() function.

    @param [in] dwAcsId     The value of this argument lies between 0 and uiNbAcs-1, with uiNbAcs the number of ACS instances
                        required by the STB at initialization. The dwAcsId parameter specifies the ACS instance to be changed.
    @param [in] eNewAcsMode The parameter specifies the new operating mode for the ACS instance; it is a value of the @ref tVA_CTRL_AcsMode enumeration.

    @retval kVA_OK                  If the request is successfully changed.
    @retval kVA_NOT_IMPLEMENTED     If the PVR feature is not implemented only the Live operating mode is possible, in which case the
                                    VA_CTRL_ChangeAcsInstanceMode() function should not be called.
    @retval kVA_INVALID_PARAMETER   Either if the parameter dwAcsId is out of range or if eNewAcsMode is not a value of the @ref tVA_CTRL_AcsMode enumeration.
    @retval kVA_OPEN_ACS_FIRST      If the ACS identified by the dwAcsId parameter is closed.
    @retval kVA_TRANSITION_NOT_ALLOWED  If the eNewAcsMode parameter does not correspond to an accepted transition.

    @b Comments
    @n The goal of the VA_CTRL_ChangeAcsInstanceMode function is to dynamically change the operating mode
    for the current DVB program. This means that only the mode transitions with the same program source will be
    accepted.
    @n For example, the mode can be changed from @ref eLIVE to @ref eLIVE_AND_PERSISTENT_RECORD if a persistent
    recording of the current live DVB program is required.
    @n The following table shows the transitions that are accepted or rejected for the DVB program that is currently
    switched on (OK = transition accepted):
    @image html  ACSTransitions2.png
    @image latex ACSTransitions2.png
*/
INT   VA_CTRL_ChangeAcsInstanceMode(DWORD dwAcsId, tVA_CTRL_AcsMode eNewAcsMode);

/**
    The STB uses this function to close a currently opened ACS.

    @param [in] dwAcsId  The value of this argument lies between 0 and uiNbAcs-1, with uiNbAcs the number of ACS required by
                    the STB at initialization. The dwAcsId parameter specifies the ACS instance to be closed.

    @retval kVA_OK                       If the ACS is successfully closed.
    @retval kVA_INVALID_PARAMETER        If the parameter dwAcsId is out of range.
    @retval kVA_SWITCH_OFF_PROGRAM_FIRST If the program is not switched off.
    @retval kVA_ALREADY_DONE             If the ACS is already closed.
*/
INT   VA_CTRL_CloseAcsInstance(DWORD dwAcsId);

/**
    The STB calls the VA_CTRL_TsChanged function whenever the original_network_id and tsid couple
    changes. This is carried out after a VA_CTRL_SwitchOffProgram and before the next VA_CTRL_SwitchOnProgram
    functions.
    @param [in] dwAcsId The value of this argument lies between 0 and uiNbAcs-1, with uiNbAcs the number of ACS required by
                        the STB at initialization. The dwAcsId parameter specifies the ACS instance on which the
                        original_network_id and tsid couple has changed.
    @retval kVA_OK                       If the original_network_id and tsid couple changes notification was successfully taken into account.
    @retval kVA_INVALID_PARAMETER        If the parameter dwAcsId is out of range.
    @retval kVA_OPEN_ACS_FIRST           If the ACS identified by the dwAcsId parameter is closed.
    @retval kVA_SWITCH_OFF_PROGRAM_FIRST If a program is currently switched on.

    @b Comments @n
    In the context of DVB Services delivered over SPTS IP-based networks, the VA_CTRL_TsChanged function
    must be called at every program zapping even if the tsid does not change.
*/
INT   VA_CTRL_TsChanged(DWORD dwAcsId);

/**
    The STB informs Viaccess-Orca that the CAT table has changed. The STB calls the @ref VA_CTRL_CatUpdated function
    whenever one of the following conditions is fulfilled:
    - the version number of the current CAT table changes
    - the transponder changes and the CAT table for this transponder is available
    If the CAT table is segmented into several sections, the STB must retrieve all these sections and concatenate
    them before calling Viaccess-Orca. Though correct order is not mandatory in the concatenation, Viaccess-Orca requires
    the STB to retrieve all CAT table sections to avoid missing the Viaccess-dedicated CA_descriptors.

    @param [in] dwAcsId The value of this argument lies between 0 and uiNbAcs-1, with uiNbAcs the number of ACS required by
                        the STB at initialization. The dwAcsId parameter specifies the ACS instance with which the CAT table is
                        updated.
    @param [in] uiCatLength This value gives the length -in bytes- of the buffer that contains the CAT
                            (as a concatenation of all of its sections).
    @param [in] pCat Pointer to the CAT. This buffer contains one or several sections, each of them starting with the byte
                     Table_Id and ending with the four bytes of CRC32. The size of each section must be equal to
                     Section_Length + 3.

    @retval kVA_OK                If the CAT update notification is successful.
    @retval kVA_INVALID_PARAMETER If the dwAcsId parameter is out of range. If uiCatLength is 0 or pCat is NULL
    @retval kVA_OPEN_ACS_FIRST    If the ACS identified by the dwAcsId parameter is closed.

    @b Comments
    @n When a transponder changes, it is mandatory to first call the function VA_CTRL_TsChanged before the
    function VA_CTRL_CatUpdated.

    For related information, see @ref VA_CTRL_TsChanged.
*/
INT   VA_CTRL_CatUpdated(DWORD dwAcsId, UINT32 uiCatLength, BYTE * pCat);


/**
    This function is used to switch on a new program. The STB must not switch on more than one program per ACS
    instance. The STB can switch on a new program if the following two conditions are fulfilled:
    - the previous program was switched off
    - the PMT of the new program is available

    To change the current program, the STB calls the VA_CTRL_SwitchOffProgram function followed by
    VA_CTRL_SwitchOnProgram. This procedure is carried out whenever the DVB-triplet changes (original
    network ID, Transport Stream ID and Service ID). For systems embarking CAM modules the ACS recognizes
    automatically whether the PMT or the CA_PMT should be used.

    @param [in] dwAcsId The value of this argument lies between 0 and uiNbAcs-1, with uiNbAcs the number of ACS required by
                        the STB at initialization. The dwAcsId parameter specifies the ACS instance on which the program is
                        switched on.
    @param [in] uiPmtLength This value gives the length in bytes of the buffer that contains the PMT or the CA_PMT.
    @param [in] pPmt Pointer to the PMT or the CA_PMT:
                     - For the PMT: this buffer contains the whole section, starting with the byte Table_Id. The buffer size is usually
                                    equal to Section_Length + 3 (the buffer includes the CRC_32).
                     - For the CA_PMT: this buffer contains the whole CA_PMT object, starting with ca_pmt_tag (as defined in EN 50221).

    @retval kVA_OK                If the program is successfully switched on.
    @retval kVA_INVALID_PARAMETER If the dwAcsId parameter is out of range, if uiPmtLength is 0 or pPmt is NULL.
    @retval kVA_OPEN_ACS_FIRST    If the ACS identified by the dwAcsId parameter is closed.
    @retval kVA_ALREADY_DONE      If a program is already switched on.

    @b Comments
    @n In case of error, the new program is not switched on and the current program remains unchanged.
    
    For related information, refer to @ref VA_CTRL_SwitchOffProgram(), @ref VA_CTRL_GetProgramInformation().
*/
INT   VA_CTRL_SwitchOnProgram(DWORD dwAcsId, UINT32 uiPmtLength, BYTE * pPmt);

/**
    The STB uses this function to switch off the current program of a given ACS. The @ref VA_CTRL_SwitchOffProgram function
    is called as soon as the program changes, see also @ref VA_CTRL_SwitchOnProgram().

    @param [in] dwAcsId The value of this argument lies between 0 and uiNbAcs-1, with uiNbAcs the number of ACS required by
                        the STB at initialization. The dwAcsId parameter specifies the ACS instance on which the program is
                        switched off.

    @retval kVA_OK                If the program is successfully switched off.
    @retval kVA_INVALID_PARAMETER If the dwAcsId parameter is out of range.
    @retval kVA_OPEN_ACS_FIRST    If the ACS identified by the dwAcsId parameter is closed.
    @retval kVA_ALREADY_DONE      If the program is already switched off.

    @b Comments
    @n In case of error, the program is not switched off.

    For related information refer to @ref VA_CTRL_SwitchOnProgram(), @ref VA_CTRL_GetProgramInformation()
*/
INT   VA_CTRL_SwitchOffProgram(DWORD dwAcsId);

/**
    The STB informs Viaccess-Orca that the PMT table has changed. The STB calls the @ref VA_CTRL_PmtUpdated function
    after a call to @ref VA_CTRL_SwitchOnProgram whenever the version number of the PMT table changes. For systems
    embarking CAM modules the ACS recognizes automatically whether the PMT or the CA_PMT should be used.
    After it calls the @ref VA_CTRL_PmtUpdated function, the STB must:
    - call the @ref VA_CTRL_UpdateStream function for all streams still in use
    - call the @ref VA_CTRL_AddStream function for all new streams
    - call the @ref VA_CTRL_RemoveStream function for all streams no longer in use.

    @param [in] dwAcsId The value of this argument lies between 0 and uiNbAcs-1, with uiNbAcs the number of ACS required by
                        the STB at initialization. The dwAcsId parameter specifies the ACS with which the PMT table is updated.
    @param [in] uiPmtLength This value gives the length in bytes of the buffer that contains the PMT or the CA_PMT.
    @param [in] pPmt Pointer to the PMT or the CA_PMT.
                     - For the PMT: this buffer contains the whole section, starting with the byte Table_Id.
                     The buffer size is usually equal to Section_Length + 3 (the buffer includes the CRC_32).
                     - For the CA_PMT: this buffer contains the whole CA_PMT object, starting with ca_pmt_tag (as defined in EN 50221).

    @retval kVA_OK If the PMT update notification was successful.
    @retval kVA_INVALID_PARAMETER       If the dwAcsId parameter is out of range, 
                                        If uiPmtLength equals 0,
                                        If pPmt is NULL or if the PMT is not correctly formated (TableId != 2, Wrong length or CRC32).
    @retval kVA_OPEN_ACS_FIRST          If the ACS identified by the dwAcsId parameter is closed.
    @retval kVA_SWITCH_ON_PROGRAM_FIRST If the current program is switched off.

    @b Comments
    @n After a call to the VA_CTRL_PmtUpdated function and before it returns, Viaccess-Orca must be able to access the
    PMT data without the corresponding accessor returning kVA_DATA_NOT_AVAILABLE. (The accessor is not
    called in the VA_CTRL_PmtUpdated code. For more information refer to @ref important_information "Important information — Calls to the API")
    
    For related information, refer to @ref VA_CTRL_UpdateStream, @ref VA_CTRL_AddStream, @ref VA_CTRL_RemoveStream
*/
INT   VA_CTRL_PmtUpdated(DWORD dwAcsId, UINT32 uiPmtLength, BYTE * pPmt);


/**
    This function informs Viaccess-Orca that a new elementary stream is to be played. The request is automatically
    rejected if one of the following two conditions occurs:
    - The ACS identifier is out of range.
    - The program is switched off.
    Otherwise, Viaccess-Orca accepts the request and notifies the STB through the
    `pfVaStreamNotificationCallback` callback function. For more information refer to @ref adding_elementary_streams.

    @param [in] dwAcsId The value of this argument lies between 0 and uiNbAcs-1, with uiNbAcs the number of ACS required by
                        the STB at initialization. The dwAcsId parameter specifies the ACS instance to which the elementary
                        stream is to be added.
    @param [in] dwStbStreamHandle The STB manufacturer chooses the stream handle value. It uniquely identifies an elementary stream,
                                  independently of the ACS instance. Viaccess-Orca uses the dwStbStreamHandle to communicate information
                                  on the stream to the STB.
    @param [in] dwPid This is the added stream pid.
    @param [in] pfVaStreamNotificationCallback The callback function used by Viaccess-Orca to send notifications to the STB.
                                               If it is NULL, no notifications are available for the current elementary stream.

                @note
                For a switched-on program Viaccess-Orca recommends to provide a notification callback for at least one elementary stream
                (e.g. for the video stream). This allows the STB to receive notifications about the descrambling status or about the
                stream maturity rating.
                Furthermore, for the recording modes, the STB should provide a notification callback for each added elementary stream,
                in order to properly receive notifications about the recording policy.

    @retval kVA_INVALID_PARAMETER       If the dwAcsId parameter is out of range.
                                        If the pid is not valid (>0x1FFF).
                                        If dwStbStreamHandle is already in use or not valid (kVA_ILLEGAL_HANDLE).
    @retval kVA_OPEN_ACS_FIRST          If the ACS identifier does not correspond to an opened ACS
    @retval kVA_SWITCH_ON_PROGRAM_FIRST If the current program is switched off.
    In these three cases, no elementary stream is added and the value of dwStbStreamHandle is free to be used by another call.
    @retval kVA_OK                      If the stream has been added successfully

    For related information, refer to @ref VA_CTRL_UpdateStream, @ref VA_CTRL_RemoveStream
*/
INT   VA_CTRL_AddStream(DWORD dwAcsId,
                        DWORD dwStbStreamHandle,
                        DWORD dwPid,
                        tVA_CTRL_StreamNotificationCallback pfVaStreamNotificationCallback);


/**
    This function verifies whether Viaccess-Orca can descramble a given elementary stream. This request is
    automatically rejected if one of the following three conditions occurs:
    - The ACS identifier is out of range.
    - The ACS is not opened.
    - The program is switched off.
    Otherwise, Viaccess-Orca accepts the request and answers with the pfVaQueryStreamNotificationCallback
    function. The notification is either eQUERY_NO, eQUERY_MAYBE or eQUERY_YES. 

    @param [in] dwAcsId The value of this argument lies between 0 and uiNbAcs-1, with uiNbAcs the number of ACS required by
                        the STB at initialization. The dwAcsId parameter specifies the ACS instance on which the stream is to be
                        queried.
    @param [in] dwStbStreamHandle The STB manufacturer determines this stream handle. It uniquely identifies an elementary stream,
                                  independently of the ACS instance. Viaccess-Orca uses the dwStbStreamHandle handle to reply to the query.
    @param [in] dwPid This is the queried stream pid.
    @param [in] pfVaQueryStreamNotificationCallback Callback function used by Viaccess-Orca to send the reply to a query stream. It must not be NULL.

    @retval kVA_OK                      If the query is successful.
    @retval kVA_INVALID_PARAMETER       If the dwAcsId parameter is out of range. If the pid is not valid (>0x1FFF)
                                        If pfVaQueryStreamNotificationCallback is NULL.
    @retval kVA_OPEN_ACS_FIRST          If the ACS identified by the dwAcsId parameter is closed.
    @retval kVA_SWITCH_ON_PROGRAM_FIRST If the current program is switched off.

    @b Comments
    @n In case of error, the elementary stream is not queried and the dwStbStreamHandle value is free to be used
    again. The dwStbStreamHandle value can be re-used as soon as Viaccess-Orca notifies the STB.

    For related information, refer to @ref tVA_CTRL_QueryStreamNotificationCallback.
    For more information refer to @ref Querying_elementary_streams "Querying elementary streams".
*/
INT   VA_CTRL_QueryStream(DWORD dwAcsId,
                          DWORD dwStbStreamHandle,
                          DWORD dwPid,
                          tVA_CTRL_QueryStreamNotificationCallback pfVaQueryStreamNotificationCallback);

/**
   This function updates the PMT information of a given elementary stream. The request is automatically
   rejected if the Viaccess-Orca stream handle does not correspond to an added stream.
   Otherwise, Viaccess-Orca accepts the request and notifies the STB with the pfVaStreamNotificationCallback
   function initially used when the stream has been added. 

   @param [in] dwStbStreamHandle As provided in VA_CTRL_AddStream.
   @param [in] dwPid This is the updated stream pid.

   @retval kVA_OK                If the update request is successful.
   @retval kVA_INVALID_PARAMETER If the dwStbStreamHandle parameter does not correspond to an added stream. If the pid is not valid (>0x1FFF).
   
   For related information, refer to @ref VA_CTRL_AddStream
*/
INT   VA_CTRL_UpdateStream(DWORD dwStbStreamHandle, DWORD dwPid);

/**
   The VA_CTRL_RemoveStream function removes a previously added elementary stream. The request is
   automatically rejected if the Viaccess-Orca stream handle does not correspond to an added stream. Otherwise,
   Viaccess-Orca accepts the request.

   @param [in] dwStbStreamHandle As provided in VA_CTRL_AddStream.

   @retval kVA_OK                If the remove request is successful.
   @retval kVA_INVALID_PARAMETER If the dwStbStreamHandle parameter does not correspond to an added stream.

   @b Comments
   @n As soon as the VA_CTRL_RemoveStream function returns, the STB stream handle dwStbStreamHandle is
   free to be used again. The demux and descrambler resources that were used by the removed stream are free.
*/
INT   VA_CTRL_RemoveStream(DWORD dwStbStreamHandle);

/**
    This function is used to set the SOID black-list required by the universe partitioning feature. Calling this
    function has a meaning only for the STB manufacturers that need to handle several simulcrypt operators, each
    of them offering its own universe. The STB manufacturer provides the SOID black-list each time users switch
    universe. This list is used by the ACS instance to discard all ECM channels of the next selected program(s), for
    which the SOID values match those in the list. 

    Note that it is up to the STB manufacturer to manage the association between universes and black-listed SOIDs.
    An ACS instance handles the SOID black-list obeying the following rules:
    - by default, there is no SOID black-list,
    - an SOID black-list set via the VA_CTRL_SetSoidBlackList function remains active until the ACS instance is closed,
    - the SOID black-list can be changed during the ACS instance life-time by calling the VA_CTRL_SetSoidBlackList function,
    - the SOID black-list can be reset (return to the initial default state, with no SOID black-list) by calling the
      VA_CTRL_SetSoidBlackList function with an empty SOID black-list (i.e. uiSoidCount set to 0 and
      pSoidBlackList set to NULL),
    - once the SOID black-list is set, the ACS instance will take it into account starting from the next PMT
      parsing. That means that typically the STB manufacturer should call VA_CTRL_SetSoidBlackList
      before calling VA_CTRL_SwitchOnProgram. If the ACS instance is already open the STB manufacturer
      should call first VA_CTRL_SwitchOffProgram, then VA_CTRL_SetSoidBlackList and finally VA_CTRL_SwitchOnProgram.

    @param [in] dwAcsId The value of this argument lies between 0 and uiNbAcs-1, with uiNbAcs the number of ACS required by the
                        STB at initialization. The dwAcsId parameter specifies the ACS instance for which the SOID black-list is set.
    @param [in] uiSoidCount This value gives the count of the tVA_CTRL_Soid elements present in the array pointed by
                            pSoidBlackList. If uiSoidCount is 0 (and pSoidBlackList is NULL), the SOID black-list is reset.
    @param [in] pSoidBlackList Pointer to an array containing uiSoidCount black-listed SOIDs. If pSoidBlackList is NULL (and
                               uiSoidCount is 0), the SOID black-list is reset.

    @retval kVA_OK                       If the SOID black-list is successfully set (or reset). The filtering of the black-listed SOID applies from next
                                         PMT parsing.
    @retval kVA_INVALID_PARAMETER        If the dwAcsId parameter is out of range, if uiSoidCount is 0 but pSoidBlackList is not NULL, or if
                                         uiSoidCount is not 0 but pSoidBlackList is NULL.
    @retval kVA_SWITCH_OFF_PROGRAM_FIRST If a program is currently switched on.

    @b Comments
    @n In case of error, the SOID black-list is not set; if there is a previous SOID black-list, it remains unchanged.
*/
INT   VA_CTRL_SetSoidBlackList(DWORD dwAcsId, UINT32 uiSoidCount, tVA_CTRL_Soid * pSoidBlackList);

/**
    This function is used to register to a file transfer for a specific file type.

    @param [in] eFileType               The file type value (as defined by the @ref tVA_CTRL_FileType enumeration) for which this registration is done
    @param [in] pfFileTransferCallback  Pointer to the callback function called after a file transfer ended.

    @retval dwFileTransferHandle        The handle of the file transfer registering.
    @retval kVA_ILLEGAL_HANDLE          If registering to a file transfer failed
                                        (e.g. if the registering is already done for this file type, or if the ACS is not initialized).
*/
DWORD  VA_CTRL_FileTransferRegister(tVA_CTRL_FileType eFileType, tVA_CTRL_FileTransferCallback pfFileTransferCallback);


/**
    This function is used to unregister from a file transfer.

    @param [in] dwFileTransferHandle     The handle of the file transfer as provided in @ref VA_CTRL_FileTransferRegister.

    @retval kVA_OK                       If the unregister is successfully done.
    @retval kVA_ERROR                    If the ACS is not initialized.
    @retval kVA_INVALID_PARAMETER        If the dwFileTransferHandle does not correspond to a file transfer registering.
*/
INT   VA_CTRL_FileTransferUnregister(DWORD dwFileTransferHandle);

/**@}*/


/* Functions implemented by STB Manufacturer */

/**
 @defgroup manCtrlFunc VA_CTRL STB Manufacturer Functions
 @brief The VA_CTRL functions to be implemented by the STB manufacturer.
 @{
*/

/**
    Viaccess-Orca uses this function to retrieve the current network and program information.
    @param [in] dwAcsId The value of this argument lies between 0 and uiNbAcs-1,
                         with uiNbAcs the number of ACS required by the STB at initialization.
                         The dwAcsId parameter specifies the ACS instance of the program information to be retrieved.
    @param [in] pProgramInfo Pointer to the address of a tVA_CTRL_ProgramInformation structure.
                          Viaccess-Orca allocates and frees the memory, but the structure fields are filled by the STB manufacturer.
    
    @retval kVA_OK                 If the required information is available and was returned to Viaccess-Orca.
    @retval kVA_DATA_NOT_AVAILABLE If the required information is not available yet.
                                   This return code can be used if the NIT table has not yet arrived for example.
    @retval kVA_ERROR              For any other reason.

    @b Comments
    @n The VA_CTRL_GetProgramInformation function must return kVA_OK and set the wNetworkId field of the
    @ref tVA_CTRL_ProgramInformation structure to 0x0 for an IP stream.

    @note The information of the NIT/SDT stream for NetworkId should be returned only when it is available. Otherwise, the
    return value is @ref kVA_DATA_NOT_AVAILABLE.
*/
INT   VA_CTRL_GetProgramInformation(DWORD dwAcsId, tVA_CTRL_ProgramInformation *pProgramInfo);

/** @} */


/** @} */

/** @} */ /* end group : ctrlAPI */


#endif /* _VA_CTRL_H_ */


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


#ifndef _VA_PVR_H_
#define _VA_PVR_H_

#include "va_types.h"
#include "va_os.h"


/**
 @ingroup std_drivers
 */

/**
 @defgroup pvrDriver VA_PVR API
 @brief 
 This section describes the interface with a generic STB manufacturer Personal Video Recorder driver used
 to record and play back elementary streams either persistently or in time-shift mode. Only scrambled streams are impacted by this API.
 @{
 */

/**
    @defgroup pvrConst VA_PVR Constants
    @brief The VA_PVR driver uses a list of constants. These are:
    
    In the `va_pvr.h` header file:
    - @ref kVA_PVR_ECM_MAX_SIZE
    - @ref kVA_PVR_METADATA_MAX_SIZE
    
    In the `va_errors.h` header file:
    - @ref kVA_OK
    - @ref kVA_DATA_NOT_AVAILABLE
    - @ref kVA_ERROR
    - @ref kVA_INVALID_PARAMETER
    - @ref kVA_ILLEGAL_HANDLE
    - @ref kVA_RESOURCE_BUSY
    - @ref kVA_UNEXPECTED_CALLBACK
 @{
*/

/** The PVR metadata maximum size */
#define kVA_PVR_METADATA_MAX_SIZE   64
/** The PVR ECM maximum size */
#define kVA_PVR_ECM_MAX_SIZE        1024

/** @} */

/**
 @defgroup pvrEnum VA_PVR Enumerations
 @brief The VA_PVR enumerations
 @{
 */

/**
 Viaccess-Orca calls the VA_PVR_OpenEcmChannel function to open an ECM channel corresponding to one ECM 
 PID. This structure enumerates the possible record types:
 @note In this document, "timeshift" and "non-persistent" denote the same thing.
 */
typedef enum
{
    ePERSISTENT, /**< The ePERSISTENT type is used to specify that the ECM channel is open for 
                 persistent recording. */
    eTIMESHIFT   /**< The eTIMESHIFT type is used to specify that the ECM channel is open for 
                 timeshift (i.e. non-persistent) recording. */

} tVA_PVR_RecordType ;

/** @} */


/**
 @defgroup manPvrFunc VA_PVR STB Manufacturer Functions
 @brief The VA_PVR functions to be implemented by the STB manufacturer.
 
 @{
 The following section gives detailed information on the different VA_PVR mechanisms already listed 
 above.@n
 All of these functions are coded by the STB manufacturer.@n
 The functions are defined in:@n
 `va_pvr.h`
 */

/**
    This function opens one ECM channel corresponding to one ECM PID.

    @param[in] dwAcsId          The value of this parameter lies between 0 and @e uiNbAcs-1, with 
                                @e uiNbAcs the number of ACS instances required by the STB at the 
                                initialization stage. 
                                For more information refer to @ref ctrlAPI "VA_CTRL API".
                                @n The dwAcsId parameter specifies the ACS instance to which the 
                                ECM channel is associated. This ACS instance must be a record one, 
                                opened with a persistent or timeshift recording operating mode, or 
                                both.
    @param[in] eRecordType      Recording type of the ECM channel to be recorded. This type is a 
                                value of the @ref tVA_PVR_RecordType enumeration.
    @param[in] wEcmChannelPid   PID value of the ECM channel to be recorded.

    @retval kVA_ILLEGAL_HANDLE  If the opening of the ECM channel failed for any reason; in 
                                particular:
                                - if @e dwAcsId does not correspond to a record instance
                                - if @e eRecordType is not a value of the @ref tVA_PVR_RecordType 
                                enumeration
                                - if @e eRecordType is not coherent with the ACS operating mode
    @retval "The STB ECM channel handle"    If the opening of the ECM channel is successful, an 
                                            @e StbEcmChannelHandle is returned; this handle is used 
                                            by the @ref VA_PVR_RecordEcm function.
    @note The @e eRecordType value is always filled; however it is particularly important when the 
    operating mode is @ref eTIMESHIFT_RECORD_AND_PERSISTENT_RECORD or
    @ref eLIVE_AND_TIMESHIFT_RECORD_AND_PERSISTENT_RECORD.
*/
DWORD VA_PVR_OpenEcmChannel ( DWORD              dwAcsId,
                              tVA_PVR_RecordType eRecordType,
                              WORD               wEcmChannelPid );

/**
    This function closes an ECM channel previously opened by @ref VA_PVR_OpenEcmChannel.

    @param[in] dwStbEcmChannelHandle    The ECM channel handle returned by @ref VA_PVR_OpenEcmChannel.

    @retval kVA_OK                  If the ECM channel is successfully closed.
    @retval kVA_INVALID_PARAMETER   If the handle does not correspond to a valid opened ECM channel.
*/
INT VA_PVR_CloseEcmChannel ( DWORD   dwStbEcmChannelHandle );

/**
    The VA_PVR_RecordEcm function asks the STB to record one ECM associated with a previously opened ECM
    channel.

    @param[in] dwStbEcmChannelHandle    The ECM channel handle that was returned by 
                                        @ref VA_PVR_OpenEcmChannel, when successful.
    @param[in] uiEcmLength              Indicates the size in bytes of the buffer pointed to by 
                                        pEcm. This parameter must be greater than zero but not 
                                        greater than @ref kVA_PVR_ECM_MAX_SIZE.
    @param[in] pEcm                     The pointer to the ECM buffer to be recorded. pEcm must not 
                                        be NULL.
    @param[in] pUserData                The pointer to STB manufacturer data. The address is set by 
                                        the STB manufacturer and is given to Viaccess-Orca through the 
                                        @ref tVA_DMX_NotifyFilteredSection function.
                                        @n For further information, refer to @ref demuxDriver "VA_DMX API".

    @retval kVA_OK                  If the ECM record request is successful.
    @retval kVA_INVALID_PARAMETER   If the handle does not exist.
                                    @n If uiEcmLength is out of range.
                                    @n If pEcm is NULL.
    @retval kVA_RESOURCE_BUSY       If the STB is already processing a recording on this ECM channel.

    @b Comments
    @n When @ref VA_PVR_RecordEcm returns, Viaccess-Orca can free the memory block pointed to by pEcm. 
    Consequently, the STB has to copy the content of the buffer before the request returns.
    
    If an error occurred (@ref kVA_INVALID_PARAMETER or @ref kVA_RESOURCE_BUSY), the STB ignores the
    @ref VA_PVR_RecordEcm request.
*/
INT VA_PVR_RecordEcm (  DWORD        dwStbEcmChannelHandle,
                        UINT32       uiEcmLength,
                        BYTE *       pEcm,
                        void *       pUserData );

/**
    This function writes a metadata file that contains Viaccess-Orca private information.

    @param[in] dwAcsId              The value of this parameter lies between 0 and @e uiNbAcs-1, 
                                    with @e uiNbAcs the number of ACS instances required by the STB 
                                    at the initialization stage. For more information refer to @ref ctrlAPI.
                                    @n The @e dwAcsId parameter specifies the ACS instance that manages 
                                    the metadata.
    @param[in] uiMetadataLength     Indicates the size in bytes of the buffer pointed to by 
                                    @e pMetadata.
                                    @n This parameter must be greater than zero but not greater than 
                                    @ref kVA_PVR_METADATA_MAX_SIZE.
    @param[in] pMetadata            The pointer to the metadata buffer. This pointer must not be 
                                    NULL.

    @retval kVA_OK                  If the metadata write request is successful.
    @retval kVA_INVALID_PARAMETER   If a parameter is out of range or if the ACS operating mode is 
                                    @ref eLIVE.
    @retval kVA_ERROR               If metadata writing fails for any reason.

    @b Comments
    @n Viaccess-Orca allocates, fills, and frees the memory block pointed to by @e pMetadata.
*/
INT VA_PVR_WriteMetadata (  DWORD    dwAcsId,
                            UINT32   uiMetadataLength,
                            BYTE *   pMetadata );

/**
    This function reads a metadata file that contains Viaccess-Orca private information.

    @param[in] dwAcsId          The value of this parameter lies between 0 and @e uiNbAcs-1, with 
                                @e uiNbAcs the number of ACS required by the STB at the 
                                initialization stage. The @e dwAcsId parameter specifies the ACS 
                                instance that manages the metadata. For more information refer to @ref ctrlAPI.
    @param[in] uiMetadataLength Indicates the size in bytes of the buffer pointed to by @e pMetadata. 
                                It also indicates the number of bytes to read.
                                @n This parameter must be greater than zero but not greater than 
                                @ref kVA_PVR_METADATA_MAX_SIZE.
    @param[in] pMetadata        Points to the metadata buffer to be read. This pointer must not be 
                                NULL.

    @retval kVA_OK                 If the metadata read request is successful. @e pMetadata points 
                                   to the Metadata.
    @retval kVA_INVALID_PARAMETER  If a parameter is out of range or if the ACS operating mode is 
                                   @ref eLIVE.
    @retval kVA_ERROR              If metadata reading fails for any reason.

    @b Comments
    @n Viaccess-Orca allocates and frees the memory block pointed to by @e pMetadata.
*/
INT VA_PVR_ReadMetadata (   DWORD    dwAcsId,
                            UINT32   uiMetadataLength,
                            BYTE *   pMetadata );

/** @} */

/** @} */

#endif /* _VA_PVR_H_ */




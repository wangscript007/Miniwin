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

#ifndef _VA_DMX_H_
#define _VA_DMX_H_

#include "va_types.h"
#include "va_errors.h"

/**
 @ingroup std_drivers
 */

/**
 @defgroup demuxDriver VA_DMX API
 @brief This section describes the interface for demux resources.

 Viaccess-Orca uses these resources to set filters for EMM and ECM monitoring.
 @{
*/

/**
 @defgroup demuxConst VA_DMX Constants
 @brief The VA_DMX driver uses a list of constants.
     
    In the `va_errors.h` header file:
    - @ref kVA_OK
    - @ref kVA_INVALID_PARAMETER
    - @ref kVA_STOP_FILTER_FIRST
    - @ref kVA_SET_FILTER_PARAMETERS_FIRST
    - @ref kVA_ILLEGAL_HANDLE

    In the `va_setup.h` header file:
    - @ref kVA_SETUP_NBMAX_SECTIONFILTERS
    - @ref kVA_SETUP_NBMAX_PID_CHANNELS
    - @ref kVA_SETUP_NBMAX_FILTERS_PER_CHANNEL

    In the `va_dmx.h` header file:
    - @ref kVA_DMX_MAX_FILTER_SIZE
    @{
*/

/** Maximum size of the filter */
#define kVA_DMX_MAX_FILTER_SIZE    8

/** @} */

/**
 @defgroup demuxEnum VA_DMX Enumerations
 @brief The VA_DMX driver uses one enumeration.
 @{
*/

/**
    According to the provided notification mode, the STB must notify Viaccess-Orca about a matching 
    section only once (for one-shot notifications) or continuously (as long as the filter criteria 
    match the section and the filter is not stopped).
*/
typedef enum
{
    eVA_DMX_OneShot,    /**< The STB must notify Viaccess-Orca about a matching section only once. */
    eVA_DMX_Continuous  /**< The STB must notify Viaccess-Orca as long as the filter criteria match
                             the section and the filter is not stopped. */
} tVA_DMX_NotificationMode;

/** @} */

/**
 @defgroup demuxCbFunctions VA_DMX Callback Functions
 @brief The VA_DMX driver uses one callback function.
 @{
*/

/**
    The STB calls this callback function type to notify Viaccess-Orca that a section matches the filter 
    criteria. The buffer that contains the section is specified in the `pBuffer` parameter. The STB 
    only notifies Viaccess-Orca about the sections that are error-free and pass the Cyclic Redundancy 
    Codes (CRC) check, if available.

    @param [in] dwVaFilterHandle    The Viaccess-Orca filter handle that the STB is provided with when 
                                    the filter is allocated. Refer to @ref VA_DMX_AllocateSectionFilter for details.
    @param [in] uiBufferLength      This value gives the length in bytes of the buffer that 
                                    contains the filtered section.
    @param [in] pBuffer             The address of the section buffer. The first byte must be the 
                                    table identifier of the section. It may be freed when this 
                                    function returns.
    @param [in] pUserData           Pointer to a STB driver user data which is set by the STB 
                                    manufacturer. The value pointed to by this parameter is not 
                                    used by the Viaccess-Orca ACS. Viaccess-Orca sends this address back as 
                                    a parameter of the @ref VA_DSCR_SetKeys function.
    
    @retval kVA_OK                  If the notification is taken into account.
    @retval kVA_INVALID_PARAMETER   If the `dwVaFilterHandle` parameter does not refer to an 
                                    allocated filter.

    The STB manufacturer can use the value that the `pUserData` points to in order to tag the 
    sections with a timestamp. Viaccess-Orca returns this timestamp with the @ref VA_DSCR_SetKeys 
    function. In this way, the STB manufacturer knows the timestamp of the control words to be 
    set. This can help handle trick modes. A timestamp can also be associated with an ECM and 
    stored on a hard drive.
    
    For related information refer to @ref VA_DMX_AllocateSectionFilter.
*/
typedef INT (*tVA_DMX_NotifyFilteredSection)(DWORD dwVaFilterHandle , 
                                             UINT32 uiBufferLength  , 
                                             BYTE *pBuffer          ,
                                             void *pUserData        );

/** @} */

/**
 @defgroup manDemuxFunc VA_DMX STB Manufacturer Functions
 @brief The following section details the functions that are coded by the STB manufacturer.
 @{ 
*/

/**
    This function allocates a filter to a PID.

    @param [in] dwAcsId                        The value of this parameter is between 0 and `uiNbAcs`-1. 
                                               The `uiNbAcs` value is the number of ACS required by the 
                                               STB at initialization stage. For information on 
                                               initialization, refer to @ref ctrlAPI.
                                               @n The `dwAcsId` parameter indicates the ACS (demux) 
                                               instance the filter is allocated to.
    @param [in] dwVaFilterHandle               The Viaccess-Orca ACS uses this value to identify a filter 
                                               instance. It corresponds on a one to one basis to the 
                                               `dwStbFilterHandle` value returned by this function. 
                                               The STB manufacturer must store and keep the 
                                               `dwVaFilterHandle` value whilst the filter is allocated. 
                                               The STB uses this value to notify the Viaccess-Orca ACS that a 
                                               section matches that given filter criteria.
    @param [in] wPid                           This value gives the PID of the demux channel to which the 
                                               filter is allocated. Viaccess-Orca may allocate several filters 
                                               with the same PID.
    @param [in] pfVA_DMX_NotifyFilteredSection This is a pointer of callback function. The STB calls this 
                                               function to notify Viaccess-Orca that a section matches the 
                                               filter criteria. The buffer that contains the section is 
                                               specified in the `pBuffer` parameter. The STB only notifies 
                                               Viaccess-Orca about the sections that are error-free and pass the 
                                               Cyclic Redundancy Codes (CRC) check, if available. Note also 
                                               that for a matching section the STB calls this notification 
                                               function according to the specified notification mode (the 
                                               @ref tVA_DMX_NotificationMode parameter passed in the function 
                                               @ref VA_DMX_SetSectionFilterParameters): only once or continuously.
    
    @retval kVA_ILLEGAL_HANDLE                 If the section filters allocation failed.
    @retval dwStbFilterHandle                  It must be unique. This means that two filters can never have the 
                                               same handle value even if they belong to two different ACS instances.
 
    The Viaccess-Orca ACS requires a @ref kVA_SETUP_NBMAX_SECTIONFILTERS` filter per ACS instance.
    
    For more information refer to @ref VA_DMX_FreeSectionFilter.
*/
DWORD VA_DMX_AllocateSectionFilter(
    DWORD dwAcsId, DWORD dwVaFilterHandle,
    WORD  wPid, 
    tVA_DMX_NotifyFilteredSection pfVA_DMX_NotifyFilteredSection);

/**
    This function frees a previously allocated filter.

    @param [in] dwStbFilterHandle   STB manufacturer filter handle that is returned by the 
                                    @ref VA_DMX_AllocateSectionFilter function. It uniquely 
                                    identifies the filter to be freed.
    
    @retval kVA_OK                  If the filter is successfully freed.
    @retval kVA_INVALID_PARAMETER   If the `dwStbFilterHandle` value does not correspond to an 
                                    allocated filter.
    @retval kVA_STOP_FILTER_FIRST   If the filter is not stopped. Before the Viaccess-Orca ACS frees a 
                                    filter, it always stops the filter with the 
                                    @ref VA_DMX_StopSectionFilter function.
    
    For related information, refer to: @ref VA_DMX_AllocateSectionFilter and @ref VA_DMX_StopSectionFilter.
*/
INT VA_DMX_FreeSectionFilter( DWORD dwStbFilterHandle );

/**
    This function sets the value and the mask of a given filter.

    @param [in] dwStbFilterHandle  STB filter handle that is returned by the 
                                   @ref VA_DMX_AllocateSectionFilter function. It identifies the 
                                   filter to be set.
    @param [in] uiLength           Indicates the size of the filter value and filter mask in bytes. 
                                   This value ranges from 0 to @ref kVA_DMX_MAX_FILTER_SIZE` (the 
                                   default maximum value is 8 bytes, unless otherwise specified 
                                   by Viaccess). If `uiLength` is set to 0, the filtering is only 
                                   carried out on the PID of the section.
    @param [in] pValue             This address contains the first byte of the filter value. If the 
                                   `uiLength` value is greater than 0, the `pValue` cannot be NULL. 
                                   If `uiLength` is set to 0, `pValue` does not have to be set.
    @param [in] pMask              This address contains the first byte of the filter mask. If the 
                                   `uiLength` value is greater than 0, `pMask` cannot be NULL. If 
                                   `uiLength` is set to 0, `pMask` does not have to be set.
    @param [in] eNotificationMode  This parameter specifies the notification mode for a matching 
                                   section: if `eVA_DMX_OneShot`, the STB has to call the 
                                   notification function only once; if `eVA_DMX_Continuous`, the 
                                   STB has to call the notification callback as long as the filter 
                                   criteria match the section and the filter is not stopped.
    
    @retval kVA_OK                 If the filter is successfully set.
    @retval kVA_INVALID_PARAMETER  If the `dwStbFilterHandle` value does not correspond to an 
                                   allocated filter or if the `uiLength`, `pValue` or `pMask` does 
                                   not match the conditions detailed above.
    
    @note The `VA_DMX_SetSectionFilterParameters` function does not automatically start the filtering 
    process. If the filter is already running, the STB must handle this dynamic situation without 
    interruption.
    @n Viaccess-Orca allocates and frees the memory blocks pointed to by `pValue` and `pMask`.

    For related information refer to: @ref VA_DMX_StartSectionFilter and @ref VA_DMX_StopSectionFilter.
*/
INT VA_DMX_SetSectionFilterParameters(
    DWORD dwStbFilterHandle, 
    UINT32 uiLength, BYTE *pValue, BYTE *pMask,
    tVA_DMX_NotificationMode eNotificationMode);

/**
    This function starts a given filtering.

    @param [in] dwStbFilterHandle           The STB filter handle that is returned by the 
                                            @ref VA_DMX_AllocateSectionFilter function. It 
                                            identifies the filter to be started.

    @retval kVA_OK                          If the filter is successfully started.
    @retval kVA_INVALID_PARAMETER           If the `dwStbFilterHandle` does not correspond to an 
                                            allocated filter.
    @retval kVA_SET_FILTER_PARAMETERS_FIRST If the filter parameters have not been set since the 
                                            filter allocation. In this case, Viaccess-Orca must call the 
                                            @ref VA_DMX_SetSectionFilterParameters function at least once.
    
    @note As soon as a filter is started, the STB must notify the Viaccess-Orca ACS about any section 
    that matches the filter criteria using the callback function type 
    @ref tVA_DMX_NotifyFilteredSection. The STB calls this notification function according to the 
    specified `eNotificationMode` (in the function @ref VA_DMX_SetSectionFilterParameters): only 
    once or continuously.
    @n If the filter is already started the function returns the @ref kVA_OK` value.

    For related information refer to: @ref VA_DMX_StopSectionFilter, @ref tVA_DMX_NotifyFilteredSection 
    and @ref VA_DMX_SetSectionFilterParameters.
*/
INT VA_DMX_StartSectionFilter(DWORD  dwStbFilterHandle);

/**
    This function stops a given filtering.

    @param [in] dwStbFilterHandle           The STB filter handle that is returned by the 
                                            @ref VA_DMX_AllocateSectionFilter function. It 
                                            identifies the filter to be stopped.

    @retval kVA_OK                          If the filter is successfully stopped.
    @retval kVA_INVALID_PARAMETER           If the `dwStbFilterHandle` does not correspond to an 
                                            allocated filter.
    
    @note If the filter is already stopped, the function just returns the @ref kVA_OK` value.

    For related information refer to @ref VA_DMX_StartSectionFilter.
*/
INT VA_DMX_StopSectionFilter(DWORD  dwStbFilterHandle);

/** @} */
/** @} */

#endif /* _VA_DMX_H_ */


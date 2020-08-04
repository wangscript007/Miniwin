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


#ifndef _VA_XNVM_H_
#define _VA_XNVM_H_

#include "va_types.h"
#include "va_errors.h"


/**
 @ingroup std_drivers
 */

/**
 @defgroup xnvmDriver VA_XNVM API
 @brief This section describes the interface with the Viaccess-Orca XNVM (eXtended NVM) driver.

 It details how Viaccess-Orca expects to write data to and read data from a storage device for non volatile memory.
 @{
 */

/**
 @defgroup xnvmConst VA_XNVM Constants
 @brief The VA_XNVM constants

    The VA_XNVM driver uses @ref kVA_SETUP_ACS_DATA_SEGMENT_SIZE.@n
    This constant defines the sizes in byte that are made available by the STB manufacturer for the ACS_DATA segments.
    Minimum sizes required by Viaccess-Orca are defined in the Requirements document.

    @note The @ref kVA_SETUP_ACS_DATA_SEGMENT_SIZE constant defines the size for each of the two
    ACS_DATA_1 and ACS_DATA_2 segments.

    The VA_XNVM driver uses constants defined in va_errors.h header file:
    - @ref kVA_OK
    - @ref kVA_INVALID_PARAMETER
    - @ref kVA_RESOURCE_BUSY
    - @ref kVA_UNEXPECTED_CALLBACK
    - @ref kVA_ERROR
    - @ref kVA_ILLEGAL_HANDLE
*/

/**
 @defgroup xnvmEnum VA_XNVM Enumerations
 @brief The VA_XNVM enumerations
 @{
*/

/** VA_XNVM Segments */
typedef enum
{
    eSEGMENT_SOFTWARE = 0, /**< This value defines the segment where the downloaded software is stored by the loader. This segment is optional. */
    eSEGMENT_ACS_DATA_1,   /**< This value defines the first segment where the ACS stores its own data. This segment is mandatory. */
    eSEGMENT_ACS_DATA_2,   /**< This value defines the second segment where the ACS stores its own data. This segment is mandatory unless the option `VAOPT_DISABLE_NVM_MIRRORING` is activated. */
    eSEGMENT_LOADER_DATA,  /**< This value defines the segment where TRIGGER and LOADER share data. This segment is optional. */

    eSEGMENT_LAST          /**< Not used */
    
} tVA_XNVM_Segment;

/** @} */

/**
 @defgroup xnvmFunc VA_XNVM STB Manufacturer Functions
 @brief The following section details the functions that are coded by the STB manufacturer.
 @{

*/

/**
    The VA_XNVM_Open function opens a session for reading and/or writing a segment.

    @param [in] dwVaXnvmHandle      This value is the handle provided by Viaccess-Orca. It should be used by the STB STB Manufacturer to call any Viaccess-Orca
                                    functions.
                                   
    @param [in] eSegment            This value is the segment that will be read or written by Viaccess-Orca.

    @retval kVA_ILLEGAL_HANDLE      If the VA_XNVM segment is already opened, if dwVaXnvmHandle has already been used or if the opening failed for any other reason.
    @retval dwStbXnvmHandle         This is the STB manufacturer handle. The dwStbXnvmHandle is used by Viaccess-Orca to call any STB
                                    manufacturer functions.
*/
DWORD   VA_XNVM_Open(DWORD dwVaXnvmHandle, tVA_XNVM_Segment eSegment);

/**
   The VA_XNVM_Close function closes a session for reading or writing a segment.

   @param [in] dwStbXnvmHandle    This value is the handle returned by VA_XNVM_Open.                                     

   @retval kVA_OK                  If the close request is successful.
   @retval kVA_INVALID_PARAMETER   If the dwStbXnvmHandle value does not correspond to an open segment.
   @retval kVA_ERROR               If the closing failed for any other reason.
                                   
*/
INT     VA_XNVM_Close(DWORD dwStbXnvmHandle);

/**
   The VA_XNVM_Read function requests that data be read from a segment.

   @param [in]  dwStbXnvmHandle    This value is the handle returned by VA_XNVM_Open.
   @param [in]  uiOffset           This value is the offset in bytes, from the beginning of the segment.
   @param [in]  uiSize             This value provides the number of bytes to read from the previous offset.
   @param [out] pReadData          This is the address where the read data is to be stored. It is allocated and freed by Viaccess-Orca.

   @retval kVA_OK                  If the read request is successful.
   @retval kVA_INVALID_PARAMETER   @n
                                   - If uiSize + uiOffset > size of the corresponding VA_XNVM segment (see @ref kVA_SETUP_ACS_DATA_SEGMENT_SIZE)
                                   - If uiSize = 0
                                   - If pReadData is NULL
                                   - If the dwStbXnvmHandle value does not correspond to an open segment.
   @retval kVA_RESOURCE_BUSY       If the VA_XNVM driver is already processing a previous Viaccess-Orca read or write request on the segment associated
                                   with dwStbXnvmHandle.

   @b Comments @n
   If the return value is kVA_OK, Viaccess-Orca waits for one of the two callback functions VA_XNVM_RequestDone or
   VA_XNVM_RequestFailed before freeing the allocated memory. In all other cases the request is rejected, no
   callback function is expected and the memory is freed immediately by Viaccess-Orca.

*/
INT     VA_XNVM_Read (DWORD dwStbXnvmHandle, UINT32 uiOffset, UINT32 uiSize, BYTE* pReadData);

/**
   This function requests the STB to write data to a segment.

   @param [in] dwStbXnvmHandle     This value is the handle returned by VA_XNVM_Open.
   @param [in] uiOffset            This value is the offset in bytes, from the beginning of the segment.
   @param [in] uiSize              This value provides the number of bytes to write from the previous offset.
   @param [in] pWriteData          The parameter identifies the address of the buffer that contains the data to be written. It is allocated and
                                   freed by Viaccess-Orca.

   @retval kVA_OK                  If the write request is successful.
   @retval kVA_INVALID_PARAMETER   @n
                                   - If uiSize + uiOffset > size of the corresponding VA_XNVM segment (see @ref kVA_SETUP_ACS_DATA_SEGMENT_SIZE)
                                   - If uiSize = 0
                                   - If pWriteData is NULL
                                   - If the dwStbXnvmHandle value does not correspond to an open segment.
   @retval kVA_RESOURCE_BUSY       If the VA_XNVM driver is already processing a previous Viaccess-Orca read or write request on the segment associated
                                   with dwStbXnvmHandle.

   @b Comments @n
   If the return value is kVA_OK, Viaccess-Orca waits for one of the two callback functions VA_XNVM_RequestDone or
   VA_XNVM_RequestFailed before freeing the allocated memory. In all other cases the request is rejected, no
   callback function is expected and the memory is freed immediately by Viaccess-Orca.

*/
INT     VA_XNVM_Write(DWORD dwStbXnvmHandle, UINT32 uiOffset, UINT32 uiSize, BYTE* pWriteData);

/** @} */


/**
 @defgroup xnvmCbFunctions VA_XNVM Viaccess-Orca Functions
 @brief The following section gives detailed information on the different VA_XNVM driver functions.
 All of these functions are provided by Viaccess-Orca.
 @{
 */

/**
   This function is called as soon as the read or write request is successfully completed.

   @param [in] dwVaXnvmHandle      This value is the handle provided by Viaccess-Orca as first parameter in the VA_XNVM_Open function.

   @retval kVA_OK                  If the callback is correctly accepted.
   @retval kVA_INVALID_PARAMETER   If the dwVaXnvmHandle parameter does not refer to an open segment.
   @retval kVA_UNEXPECTED_CALLBACK If Viaccess-Orca did not request a read or write operation or if the last read or write operation did not return
                                   kVA_OK.
                                   
*/
INT     VA_XNVM_RequestDone(DWORD dwVaXnvmHandle);

/**
   This function is called when the read or write request fails.

   @param [in] dwVaXnvmHandle      This value is the handle provided by Viaccess-Orca as first parameter in the VA_XNVM_Open function.

   @retval kVA_OK                  If the callback is correctly accepted.
   @retval kVA_INVALID_PARAMETER   If the dwVaXnvmHandle parameter does not refer to an open segment.
   @retval kVA_UNEXPECTED_CALLBACK If Viaccess-Orca did not request a read or write operation or if the last read or write operation did not return
                                   kVA_OK.
                                   
*/
INT     VA_XNVM_RequestFailed(DWORD dwVaXnvmHandle);

/** @} */

/** @} */

#endif /*_VA_XNVM_H*/

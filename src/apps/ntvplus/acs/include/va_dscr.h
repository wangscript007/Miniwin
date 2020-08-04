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

#ifndef _VA_DSCR_H_
#define _VA_DSCR_H_

#include "va_types.h"
#include "va_errors.h"

/**
 @ingroup std_drivers
 */

/**
 @defgroup descrDriver VA_DSCR API
 @brief This section details the descrambler API for the Viaccess-Orca CAS.
 @{
*/

/**
 @defgroup descrConst VA_DSCR Constants
 @brief The VA_DSCR driver uses a list of constants.
     
    In the `va_errors.h` header file:
    - @ref kVA_OK
    - @ref kVA_INVALID_PARAMETER
    - @ref kVA_ILLEGAL_HANDLE
    - @ref kVA_NOT_IMPLEMENTED
*/

/**
 @defgroup descrEnum VA_DSCR Enumerations
 @brief The VA_DSCR driver uses the following enumeration
 @{
*/

/**
    The algorithms that are currently supported by Viaccess-Orca for content scrambling/descrambling are 
    DVB-CSA, AES (ATIS 0800006) and the three DVB-CSA3 modes. The descrambling channel can be 
    configured to use either one of these algorithms by specifying the appropriate enumeration, as
    defined in the `va_dscr.h` header file.
*/
typedef enum
{
    eSCRAMBLING_ALGO_DVB_CSA,     /**< DVB Common Scrambling Algorithm (DVB-CSA). */
    eSCRAMBLING_ALGO_AES_128_CBC, /**< AES-128, CBC mode Algorithm. The Initialization Vector IV 
                                       consists of 16 bytes (or 128 bits) and the Residual Block 
                                       Management method is the Residual Block Termination and 
                                       the Short Block Handling (also called XOR). */
    eSCRAMBLING_ALGO_DVB_CSA3_STANDARD_MODE,           /**< DVB-CSA3 in standard mode. */
    eSCRAMBLING_ALGO_DVB_CSA3_MINIMALLY_ENHANCED_MODE, /**< DVB-CSA3 in minimally enhanced mode. */
    eSCRAMBLING_ALGO_DVB_CSA3_FULLY_ENHANCED_MODE,     /**< DVB-CSA3 in fully enhanced mode. */

    eSCRAMBLING_ALGO_ENUM_LAST    /**< Number of algorithms available in the @ref tVA_DSCR_ScramblingAlgo enumeration. */

} tVA_DSCR_ScramblingAlgo;

/** @} */

/**
 @defgroup descrStruct VA_DSCR Structures
 @brief The VA_DSCR driver uses the following structure.
 @{
 */

/**
    This structure contains parameters needed for the descrambling channel initialization,
    mainly the algorithm to be used to descramble the content.
*/
typedef struct
{
    tVA_DSCR_ScramblingAlgo eContentScramblingAlgo; /**< The algorithm to be used by the descrambler. */
    UINT32 uiIVLength; /**< The length (in bytes) of the `pIV` is 16 for `eSCRAMBLING_ALGO_AES_128_CBC`
                            or 0 for `eSCRAMBLING_ALGO_DVB_CSA` and for all three 
                            `eSCRAMBLING_ALGO_DVB_CSA3` algorithms. */
    const BYTE *pIV;   /**< The Initialization Vector IV for the descrambling algorithm. This buffer is 
                            16-bytes long for `eSCRAMBLING_ALGO_AES_128_CBC` and it is set to NULL for 
                            `eSCRAMBLING_ALGO_DVB_CSA` and for all three `eSCRAMBLING_ALGO_DVB_CSA3`
                            algorithms. */
} tVA_DSCR_ConfigurationParameters;

/** @} */

/**
 @defgroup manDescrFunc VA_DSCR STB Manufacturer Functions
 @brief The following section details the functions that are coded by the STB manufacturer.
 @{ 
*/

/**
    This function allocates a descrambling channel to a given elementary stream.

    @param [in] dwStbStreamHandle Identifies the elementary stream to which the descrambling 
                                  channel must be linked.
                                  @n This value is set by the STB manufacturer. The Viaccess-Orca ACS 
                                  receives this value when the stream is added. For more 
                                  information refer to @ref VA_CTRL_AddStream.
    
    @retval kVA_ILLEGAL_HANDLE    If the opening of the descrambler failed.
    @retval dwStbDescrHandle      The STB manufacturer descrambler handle. This handle must be a 
                                  unique value. This means that two descramblers cannot have the 
                                  same handle values, even if they belong to two different ACS instances.
*/
DWORD VA_DSCR_Open( DWORD dwStbStreamHandle );

/**
    This function sets specific parameters for the current descrambling channel, mainly the 
    algorithm to use to descramble the associated elementary stream. Currently, only the DVB-CSA, 
    the AES-128 (CBC mode) and the three-modes DVB-CSA3 algorithms are supported for content 
    descrambling, but future evolutions should be considered.

    Note that calling this function is not mandatory and the default descrambling algorithm is 
    DVB-CSA. However, this function can be called several times during the descrambling channel 
    lifetime, in order to dynamically notify potential updates of the descrambling parameters 
    (algorithm) associated with the elementary stream.

    @param [in] dwStbDescrHandle  STB manufacturer descrambler handle that is returned by the 
                                  @ref VA_DSCR_Open function.
    @param [in] pstParameters     This parameter contains initialization data for the descrambler 
                                  and it is a pointer to a @ref tVA_DSCR_ConfigurationParameters 
                                  structure. The memory is allocated by Viaccess-Orca and it is freed 
                                  after the function's return. This parameter cannot be NULL.
                                  @n The field `eContentScramblingAlgo` is one of the 
                                  `tVA_DSCR_ScramblingAlgo` enumerations. If the 
                                  `eSCRAMBLING_ALGO_AES_128_CBC` algorithm is specified, the pointer 
                                  `pIV` is not NULL and the pointed buffer is 16-bytes long (as 
                                  indicated by `uiIVLength`). Otherwise the pointer `pIV` is NULL 
                                  and `uiIVLength` is set to 0.
    
    @retval kVA_OK                If the parameters for the descrambler channel were successfully set.
    @retval kVA_INVALID_PARAMETER If either the descrambler handle did not correspond to a valid 
                                  opened descrambler, or the `pstParameters` is set to NULL. It is 
                                  also returned if the `pIV` or the `uiIVLength` are not correct 
                                  according to the specified algorithm `tVA_DSCR_ScramblingAlgo`.
    @retval kVA_NOT_IMPLEMENTED   If the STB does not currently support the specified scrambling algorithm.

    @note If there is no call to the `VA_DSCR_SetParameters` function, after opening the descrambling 
    channel, it is assumed that the default descrambling algorithm to be used is DVB-CSA.
*/
INT VA_DSCR_SetParameters(
    DWORD dwStbDescrHandle,
    const tVA_DSCR_ConfigurationParameters *pstParameters );

/**
    This function closes a descrambling channel previously opened using the @ref VA_DSCR_Open function.

    @param [in] dwStbDescrHandle    STB manufacturer descrambler handle that is returned by the 
                                    @ref VA_DSCR_Open function.

    @retval kVA_OK                  If the descrambler channel was successfully closed.
    @retval kVA_INVALID_PARAMETER   If the descrambler handle does not correspond to a valid 
                                    opened descrambler.
*/
INT VA_DSCR_Close( DWORD dwStbDescrHandle );

/**
    This function provides a previously opened descrambler with the control words (the odd and/or 
    even keys) that enable it to descramble the associated elementary stream. The descrambling 
    algorithm to be used is set via the @ref VA_DSCR_SetParameters function; otherwise, in the 
    case where this function is not previously called, the default descrambling algorithm is DVB-CSA.

    @param [in] dwStbDescrHandle    STB manufacturer descrambler handle that is returned by the 
                                    @ref VA_DSCR_Open function.
    @param [in] uiOddKeyLength      Gives the length of the odd key in bytes.
    @param [in] pOddKey             Pointer to the odd key buffer or NULL if the odd key is not 
                                    available yet.
    @param [in] uiEvenKeyLength     Gives the length of the even key in bytes.
    @param [in] pEvenKey            Pointer to the even key buffer or NULL if the even key is not 
                                    available yet.
    @param [in] pUserData           Pointer to STB manufacturer data. The address is set by the 
                                    STB manufacturer and is given to Viaccess-Orca through the 
                                    @ref tVA_DMX_NotifyFilteredSection function.
    
    @retval kVA_OK                  If the set keys request was successful.
    @retval kVA_INVALID_PARAMETER   If either the descrambler handle did not correspond to a valid 
                                    opened descrambler, or the key lengths are not supported. It is 
                                    also returned if the `pOddKey` and `pEvenKey` pointers are both 
                                    set equal to NULL.
    
    @b Comments
    @n The `VA_DSCR_SetKeys` function is only called after the STB is notified that Viaccess-Orca can 
    descramble the stream. For more information refer to @ref adding_elementary_streams "Adding elementary streams".

    @note The key lengths are usually constants and equal to 8 (for DVB-CSA) or 16 (for AES 
    and all DVB-CSA3 modes).@n
    Viaccess-Orca allocates, fills, and frees the memory blocks pointed by `pOddKey` and `pEvenKey`. 
    These memory blocks are freed as soon as @ref VA_DSCR_SetKeys returns. The STB manufacturer 
    value that the `pUserData` parameter points to can be used as a timestamp. For further 
    information refer to @ref demuxDriver.
*/
INT VA_DSCR_SetKeys(
    DWORD dwStbDescrHandle,
    UINT16 uiOddKeyLength, const BYTE  *pOddKey,
    UINT16 uiEvenKeyLength, const BYTE  *pEvenKey,
    void *pUserData );

/** @} */
/** @} */

#endif /* _VA_DSCR_H_ */

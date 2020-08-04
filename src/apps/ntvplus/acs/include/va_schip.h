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

/*
    $Revision: 5833 $
    $Date: 2007-05-10 17:56:04 +0200 (jeu., 10 mai 2007) $
*/


#ifndef _VA_SCHIP_H_
#define _VA_SCHIP_H_

#include "va_types.h"
#include "va_errors.h"


/**
 @ingroup sec_drivers
 */

/**
 @defgroup schipDriver VA_SCHIP API
 @brief This section describes the interface with a generic secure chipset driver.
 It is designed for STB manufacturers who will map this interface on to the functions made available by the hardware secure chipset.
 @{
 */

/**
 * @defgroup schipEnum VA_SCHIP Enumerations
 * @brief The VA_SCHIP enumerations

 * @{ */

/**
The tVA_SCHIP_CipheringMode enumeration defines the possible modes of activation for the control word protection.
*/
typedef enum
{
	/**
	The chipset only sets the CW on the descrambler.
	*/
	eINACTIVE,
	/**
	The CW is deciphered by the chipset and set on the descrambler. This chipset-working mode is reversible.
	*/
	eSESSION,
	/**
	The CW is deciphered by the chipset and set on the descrambler. This chipset-working mode is not reversible.
	*/
	eLOCKED

} tVA_SCHIP_CipheringMode;
/**@}*/


/**
 * @defgroup schipFunc VA_SCHIP STB Manufacturer Functions
 * @brief The VA_SCHIP functions to be implemented by the STB manufacturers.

* @{
*/

/*---------------------------------------------------------------------------*/
/**
    This function returns the ciphering activation mode of the chipset.
    @param  pChipsetMode    Identifies the activation mode. Please refer to the @ref schipEnum description for details.
    @retval kVA_OK                  If the secure chipset has successfully returned the mode.
    @retval kVA_NOT_IMPLEMENTED     If the secure chipset is not present.
*/
INT VA_SCHIP_GetChipsetMode ( tVA_SCHIP_CipheringMode* pChipsetMode );

/*---------------------------------------------------------------------------*/
/**
    This function gives the chipset its ciphering activation mode. The CW that is received after this call will then be deciphered or not deciphered.
    @param  eChipsetMode    Identifies the activation mode. Please refer to the @ref schipEnum description for details.
    @retval kVA_OK                  The chipset has successfully applied the mode.
    @retval kVA_NOT_IMPLEMENTED     The mode cannot be applied because the chipset is locked, or because the requested mode is not supported by the chipset or because no secure chipset is present.
    @retval kVA_INVALID_PARAMETER   The mode doesn't belong to the @ref tVA_SCHIP_CipheringMode enum.

	@b Comments @n When calling this function with `eLOCKED`, the chipset must be locked by blowing the dedicated hardware fuse.
*/
INT VA_SCHIP_SetChipsetMode ( tVA_SCHIP_CipheringMode eChipsetMode );


/**
	This function provides the chipset with a session key that will be used later to decipher the received CW.
	@param  uiSessionKeyLength      Size in bytes of the session key.
	@param  pSessionKey             Buffer of bytes containing the session key.
	@retval kVA_OK                  If the chipset has successfully received the session key.
    @retval kVA_NOT_IMPLEMENTED     If the secure chipset is not present. 
    @retval kVA_INVALID_PARAMETER   If the key pointer is NULL.@n
                                    If the key length is not compatible with the secure chipset cryptographic protocol (e.g. the 3DES protocol usually uses a 16-bytes key).
*/
INT VA_SCHIP_SetSessionKey ( UINT32 uiSessionKeyLength, const BYTE *pSessionKey );

/*---------------------------------------------------------------------------*/
/**
    This function returns the unique chipset identifier that is written at the manufacturing site by Viaccess-Orca equipment; it is publicly accessible.
    @retval kVA_ILLEGAL_HANDLE If the chipset identifier can not be retrieved.@n
            Otherwise, the chipset identifier.
*/
DWORD VA_SCHIP_GetChipsetId ( void );

/**@}*/
/**@}*/

#endif /* _VA_SCHIP_H_ */


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

#ifndef _VA_STDBY_H_
#define _VA_STDBY_H_

#include "va_types.h"
#include "va_errors.h"
#include "va_os.h"

/**
 @ingroup std_drivers
 */

/**
 @defgroup stdbyDriver VA_STDBY API (deprecated)
 @brief This section describes the interface with a generic STB manufacturer Standby driver.

 @warning The VA_STDBY is deprecated and must not be part of the integration.

 @{
 This driver is used to notify Viaccess-Orca when the STB enters into or leaves the standby mode. It also enables Viaccess-Orca
 to set an alarm for a given date.
 */
 
 
/**
 @defgroup manStdbyFunc VA_STDBY STB Manufacturer Functions
 @{
 The following section gives detailed information on the different Standby functions already listed above.@n
 All of these functions are coded by the STB manufacturer.@n
 The functions are defined in:@n
 `va_stdby.h`
*/

/**
    Viaccess-Orca calls this function to notify the STB manufacturer that all of the standby phase processes have been run.

	@note This function must be called by Viaccess-Orca in response to a @ref VA_STDBY_EnterStandby function call.
 */
void VA_STDBY_StandbyProcessDone(void);

/**
    Viaccess-Orca calls this function to set the next wake up alarm.
	
	@param [in] pTime The address that contains the date and time values of the alarm.
	                  @n The type used (@ref tVA_OS_Tm) is defined in the @ref osDriver.
					  @n If `pTime` is set to NULL, the currently stored wake up alarm must be canceled.
	
	@retval kVA_OK                If the wake up timer was correctly set.
	@retval kVA_INVALID_PARAMETER If the date fields `pTime` points to does not meet the restrictions described in the @ref osDriver.

	@note Only one wake up alarm can be set. A new date and time setup will overwrite the previous one.
 */
INT  VA_STDBY_SetAlarm(tVA_OS_Tm * const pTime);

/**
    Viaccess-Orca calls this function to notify the STB manufacturer that all alarm processes are done.

	@note Viaccess-Orca calls this function in response to a call to the @ref VA_STDBY_NotifyAlarm function.
 */
void VA_STDBY_AlarmProcessDone  (void);

/* @} */

/**
 @defgroup viaStdbyFunc VA_STDBY Viaccess-Orca Functions
 @brief The VA_STDBY functions to be implemented by Viaccess-Orca.
 @{
*/

/**
    This function is called when the STB enters a Functional Standby phase. Some of the processes Viaccess
    executes may require a certain amount of time. Viaccess-Orca therefore calls the
    @ref VA_STDBY_StandbyProcessDone function to notify the driver that the process is done.
	The STB can then switch to Electrical Standby.
	
	@note The STB can cancel the Viaccess-Orca standby processes with the @ref VA_STDBY_ExitStandby function.
 */
void VA_STDBY_EnterStandby(void);


/**
    The VA_STDBY driver calls this function when the alarm set by Viaccess-Orca ends.
	When the associated process is finished, Viaccess-Orca calls the function @ref VA_STDBY_AlarmProcessDone.
 */
void VA_STDBY_NotifyAlarm (void);


/**
    The VA_STDBY driver calls this function when an FS --> ON transition occurs.
	Viaccess-Orca cancels the current processes; it does not call the @ref VA_STDBY_StandbyProcessDone function
	or the @ref VA_STDBY_AlarmProcessDone function. The STB can then switch to ON.
 */
void VA_STDBY_ExitStandby (void);

/* @} */
 
/* @} */

#endif /* _VA_STDBY_H_ */



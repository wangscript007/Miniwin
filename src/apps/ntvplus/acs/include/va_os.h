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



#ifndef _VA_OS_H_
#define _VA_OS_H_

#include "va_types.h"
#include "va_errors.h"
#include "va_setup.h"


/**
 @ingroup std_drivers
 */

/**
 @defgroup osDriver VA_OS API
 @brief The functions described in this section provide mechanisms for the Viaccess-Orca ACS to manage
 memory, synchronize access to a resource between threads, process data from and to a communication
 port, and retrieve the UTC time and the processor runtime.
* @{
*/

/**
* @defgroup osConst VA_OS Constants
  @brief The VA_OS driver uses the following constants:

    In the `va_errors.h` header file:
    - @ref kVA_OK
    - @ref kVA_INVALID_PARAMETER
    - @ref kVA_ILLEGAL_HANDLE
    - @ref kVA_TIMEOUT

    In the va_os.h header file:
    - @ref kVA_OS_WAIT_FOREVER
    - @ref kVA_OS_NO_WAIT
    - @ref kVA_OS_NBMAX_PERIODIC_CALLS

    In the `va_setup.h` header file:
    - @ref kVA_SETUP_SEMAPHORE_SIZE_IN_DWORD
    - @ref kVA_SETUP_MUTEX_SIZE_IN_DWORD
* @{
*/

#define kVA_OS_NBMAX_PERIODIC_CALLS            1            /**< Nb max periodic calls */
#define kVA_OS_WAIT_FOREVER                    0xFFFFFFFF   /**< Wait forever */
#define kVA_OS_NO_WAIT                         0            /**< No wait */

/** @} */

/**
* @defgroup osStruct VA_OS Structures
* @brief The VA_OS driver contains the following data structures.
* @{
*/

/**
Semaphores are used as synchronization objects to manage access to shared system resources.
@note
Viaccess-Orca uses the @ref kVA_SETUP_SEMAPHORE_SIZE_IN_DWORD constant to allocate memory to each OS 
semaphore it requires. This value is equal to or greater than the actual size of the STB 
manufacturer OS semaphore.
*/
typedef DWORD tVA_OS_Semaphore[kVA_SETUP_SEMAPHORE_SIZE_IN_DWORD];

/**
A mutex is a synchronization object that is created so that multiple program threads can take 
turns in sharing the same resource. Mutexes are useful for protecting shared data structures from 
concurrent modifications.
@note
Viaccess-Orca uses the @ref kVA_SETUP_MUTEX_SIZE_IN_DWORD constant to allocate memory to each OS mutex it 
requires. This value is equal to or greater than the actual size of the STB manufacturer OS mutex.
Mutexes are recursive, which means that Viaccess-Orca can lock a given mutex several times within the 
same thread, without generating an error. This thread unlocks the mutex the same number of times 
in order to make it available for other threads.
*/
typedef DWORD tVA_OS_Mutex[kVA_SETUP_MUTEX_SIZE_IN_DWORD] ;

/**
This structure gives the date as the number of seconds elapsed since 00:00:00 
on January 1, 1970 (UTC).
*/
typedef UINT32 tVA_OS_Time ;

/**
This structure specifies the various properties of the calendar time.
*/
typedef struct
{
    UINT32 uiSec;        /**<  Gives seconds after the minute [0, 59].     */
    UINT32 uiMin;        /**<  Gives minutes after the hour [0, 59].       */
    UINT32 uiHour;       /**<  Gives the hour of the day [0, 23].          */
    UINT32 uiMonthDay;   /**<  Gives the day of the month [1, 31].         */
    UINT32 uiMonth;      /**<  Gives the month of the year [0, 11].        */
    UINT32 uiYear;       /**<  Gives the years since 1900.                 */
    UINT32 uiWeekDay;    /**<  Gives days from Sunday [0,6].               */
    UINT32 uiYearDay;    /**<  Gives the days since first January [0,365]. */

} tVA_OS_Tm;


/**
This structure gives information on the processor runtime.
*/
typedef struct
{
    UINT32 uiMilliSec;  /**< Indicates the processor runtime, in milliseconds.   */
    UINT32 uiMicroSec;  /**< Indicates the remaining microseconds, if available. */
} tVA_OS_RunTime ;


/** @} */

/**
* @defgroup osCbFunctions VA_OS Callback Functions
* @brief The VA_OSdriver uses one callback function.
* @{
*/

/**
The tVA_OS_PeriodicCallFct is the periodic callback function. This callback function is a 
parameter of the @ref VA_OS_StartPeriodicCall function.
@param  dwViaccessData
        Given by Viaccess-Orca in the @ref VA_OS_StartPeriodicCall request. 
        The STB returns it with the periodic callback function.
@retval kVA_OK                If the call is successful.
@retval kVA_INVALID_PARAMETER If the dwViaccessData parameter does not correspond to a 
                              requested periodic call.
@see VA_OS_StartPeriodicCall
*/
typedef INT (*tVA_OS_PeriodicCallFct)(DWORD dwViaccessData);


/** @} */

/**
* @defgroup manOsFunc VA_OS STB Manufacturer Functions
* @brief The following section details the functions that are coded by the STB manufacturer
* @{
*/


/**
This function allocates memory blocks.
@param uiSize Size of allocated memory in bytes.
@return a pointer to the allocated space or NULL if the available memory is insufficient.

@b Comments @n
    This function must be thread-safe. This means that it can be called by two threads 
    at the same time without generating an error.
    This function must be mapped to the standard libc equivalent function.
@see VA_OS_Free
*/
void *VA_OS_Alloc ( UINT32 uiSize);

/**
The VA_OS_Free function frees the memory block previously allocated by VA_OS_Alloc.
@param ptr Indicates the address of the memory block to free.
@b Comments @n This function must be thread-safe. This means that it can be called by two threads 
      at the same time without generating an error.
      If ptr is NULL the function must return immediately without any processing.
      This function must be mapped to the standard libc equivalent function.
@see VA_OS_Alloc
*/
void VA_OS_Free( void *ptr );

/**
Viaccess-Orca uses this function to print formatted output to a unique external communication port 
(preferably a serial port).
@param pFormat This is the format of the string to be printed. It has the same syntax as printf.
@return the number of printed characters.
@b Comments @n This function must be thread-safe.
*/
INT VA_OS_Printf(const char *pFormat, ...);


/**
This function reads the characters from a unique external communication port 
(preferably a serial port), if available.
@param uiDuration 
       This parameter has two possible values:
       - If uiDuration equals @ref kVA_OS_NO_WAIT, the function tries to get the character but does 
         not wait if the character is not available.
       - If uiDuration equals @ref kVA_OS_WAIT_FOREVER, the function waits indefinitely, and exits 
         only when another thread releases the character.
@return character found on the communication port input (as an ASCII value), or 
@retval kVA_TIMEOUT if no incoming character is available. This last return value is relevant 
                    only when the parameter uiDuration is set to @ref kVA_OS_NO_WAIT.
@b Comments @n The serial input must be stored in a buffer. The buffer size must be at least 16 characters 
      in length. The buffer works as a FIFO queue.
*/
INT VA_OS_Getc(UINT32 uiDuration);

/**
This function initializes a semaphore. Semaphores are commonly used to count and limit the number 
of threads that access a shared resource.

@param pSemaphore
       Points to the semaphore to initialize.

       @param uiValue
       Indicates the initial value of the semaphore's counter. It is greater than or equal to 
       zero. It gives the initial number of available resources. These resources are acquired and 
       released using the respective functions @ref VA_OS_AcquireSemaphore and
       @ref VA_OS_ReleaseSemaphore.

@retval kVA_OK                 If the semaphore is successfully initialized.
@retval kVA_INVALID_PARAMETER  If pSemaphore is set to NULL.
@b Comments @n This function does not allocate memory for the semaphore's data structure. 
      It only initializes the structure's fields. 
@see VA_OS_DeleteSemaphore VA_OS_AcquireSemaphore VA_OS_ReleaseSemaphore
*/
INT VA_OS_InitializeSemaphore(tVA_OS_Semaphore * const pSemaphore, UINT32 uiValue);

/**
This function deletes a semaphore that was previously initialized by 
@ref VA_OS_InitializeSemaphore.

@param pSemaphore Points to the semaphore to be deleted.

@retval kVA_OK                If the semaphore was successfully deleted.
@retval kVA_INVALID_PARAMETER If pSemaphore does not correspond to an initialized semaphore.

@b Comments @n This function does not free the memory addressed by pSemaphore. For related information, 

@see VA_OS_InitializeSemaphore VA_OS_AcquireSemaphore VA_OS_ReleaseSemaphore
*/
INT VA_OS_DeleteSemaphore(tVA_OS_Semaphore * const pSemaphore);

/**
A thread calls the function VA_OS_AcquireSemaphore to acquire a semaphore previously initialized 
by @ref VA_OS_InitializeSemaphore. If no resource is available, the thread is suspended. As soon as a
resource is made available, the waiting thread is released and continues its execution.

@param pSemaphore
       Pointer to the semaphore to acquire.
@param uiDuration
       The value of this parameter is @ref kVA_OS_WAIT_FOREVER. The function waits indefinitely 
       and exits only when another thread releases the semaphore.

@retval kVA_OK
        If the semaphore was successfully acquired.
@retval kVA_INVALID_PARAMETER
        Either if pSemaphore does not correspond to an initialized semaphore or if `uiDuration` does
        not fulfill the constraints detailed above.
@see @ref VA_OS_InitializeSemaphore @ref VA_OS_DeleteSemaphore @ref VA_OS_ReleaseSemaphore
*/
INT VA_OS_AcquireSemaphore(tVA_OS_Semaphore * const pSemaphore, UINT32 uiDuration);


/**
This function releases the semaphore initialized by @ref VA_OS_InitializeSemaphore. 
This mechanism enables a thread, waiting for the semaphore to be resumed.
@param pSemaphore Points to the semaphore to be released.
@retval kVA_OK
        If the semaphore is successfully released.
@retval kVA_INVALID_PARAMETER
        If pSemaphore does not correspond to an initialized semaphore.
@see VA_OS_InitializeSemaphore VA_OS_DeleteSemaphore VA_OS_AcquireSemaphore
*/
INT VA_OS_ReleaseSemaphore(tVA_OS_Semaphore * const pSemaphore);

/**
This function initializes a mutex. A mutex is a synchronization object that is created so that 
multiple program threads can take turns in sharing the same resource. It ensures that only one 
thread at a time can modify data or have access to a resource .

@param pMutex Points to the mutex to initialize.

@retval kVA_OK                If the initialization of the mutex is successful.
@retval kVA_INVALID_PARAMETER If pMutex is set to NULL.

@b Comments @n this function does not allocate memory for the mutex's data structure. 
      It only initializes the structure's fields.
@see VA_OS_DeleteMutex VA_OS_LockMutex VA_OS_UnlockMutex
*/
INT VA_OS_InitializeMutex(tVA_OS_Mutex * const pMutex);


/**
This function deletes a mutex that was previously initialized by the 
@ref VA_OS_InitializeMutex function.
@param pMutex Points to the mutex to delete.
@retval kVA_OK
        If mutex is successfully deleted.
@retval kVA_INVALID_PARAMETER
        If pMutex does not correspond to an initialized mutex.
@b Comments @n This function does not free the memory addressed by pMutex.
@see VA_OS_InitializeMutex VA_OS_LockMutex VA_OS_UnlockMutex
*/
INT VA_OS_DeleteMutex (tVA_OS_Mutex * const pMutex);


/**
This function locks a mutex. If the mutex is currently locked by another thread, the caller is 
blocked until the owner unlocks it. The thread owns the mutex when the @ref VA_OS_LockMutex call
is successful. At that point only this thread is permitted to access the shared resource.
Mutexes are recursive. The OS records the number of times one thread locks the mutex. 
The same number of @ref VA_OS_UnlockMutex operations must be performed before the mutex is 
unlocked.
@param pMutex Points to the mutex to lock.
@retval kVA_OK                If the mutex is successfully locked.
@retval kVA_INVALID_PARAMETER If pMutex does not correspond to an initialized mutex.
@see VA_OS_InitializeMutex VA_OS_DeleteMutex VA_OS_UnlockMutex
*/
INT VA_OS_LockMutex (tVA_OS_Mutex * const pMutex);


/**
This function unlocks a mutex if the calling thread is the owner. If the @ref VA_OS_LockMutex 
function was called several times by the thread, the @ref VA_OS_UnlockMutex function must be 
called the same number of times to free the mutex for other threads.
@param pMutex Points to the mutex to unlock.
@retval kVA_OK                If the mutex was successfully unlocked.
@retval kVA_INVALID_PARAMETER If pMutex does not correspond to an initialized mutex.
@see VA_OS_InitializeMutex VA_OS_DeleteMutex VA_OS_LockMutex
*/
INT VA_OS_UnlockMutex (tVA_OS_Mutex * const pMutex);

/**
This function suspends the execution of the current thread for a specified amount 
of time.
@param uiDuration This value specifies the duration of the suspended execution in ms. 
                  If uiDuration is equal to 0, the calling thread is moved to the end of 
                  the queue by the OS scheduler and a new thread gets to run.
*/
void VA_OS_Sleep( UINT32 uiDuration );


/**
This function requests the driver to periodically call the Viaccess-Orca callback function given as 
a parameter.
@param uiDurationPeriod
       This value indicates the time interval between two consecutive calls. 
       The duration unit is in ms.
@param pfPeriodicCallFunction
       This pointer contains the address of the callback function that is periodically called 
       by the STB manufacturer OS.
       The prototype of the callback function is @ref tVA_OS_PeriodicCallFct. 
@param dwViaccessData
       This Viaccess-Orca parameter is returned in the callback function.
@return This function returns the STB handle of the periodic call process or @ref kVA_ILLEGAL_HANDLE
        - If the request fails
        - If the `uiDurationPeriod` parameter is equal to 0,
        - If the `uiDurationPeriod` parameter is lower than 100 ms.

This handle is used later on by Viaccess-Orca to stop the periodic call with @ref VA_OS_StopPeriodicCall.

@b Comments @n      The minimum value between two consecutive calls is 100 ms. 
      The required precision is +/- 20 ms.
      The Viaccess-Orca callback function performs limited processing and returns rapidly. 
      Viaccess-Orca requires at most @ref kVA_OS_NBMAX_PERIODIC_CALLS instances of the periodic call 
      process.
@see tVA_OS_PeriodicCallFct VA_OS_StopPeriodicCall
*/
DWORD VA_OS_StartPeriodicCall( UINT32                 uiDurationPeriod,
                               tVA_OS_PeriodicCallFct pfPeriodicCallFunction,
                               DWORD                  dwViaccessData);


/**
This function stops the periodic calling function process started by @ref VA_OS_StartPeriodicCall.
@param dwHandle  This handle is returned by the @ref VA_OS_StartPeriodicCall request.
@retval kVA_OK                If the request is successful.
@retval kVA_INVALID_PARAMETER If dwHandle does not correspond to a started periodic call process.
@see VA_OS_StartPeriodicCall
*/
INT VA_OS_StopPeriodicCall( DWORD dwHandle );


/**
This function retrieves the UTC time in the @ref tVA_OS_Time format.
The date-time returned by @ref VA_OS_GetTime must be a secure, tamperproof time and it cannot be 
changed by the STB user. This absolute date-time should be retrieved by the STB either using
information in the broadcast signal (DVB- Time and Data Table or TDT) or using information 
exchanged via the return channel (Network Time Protocol or NTP), for example.
@param pTime  Indicates the address where the UTC time value must be stored in 
              the @ref tVA_OS_Time format.
@b Comments @n The memory pointed by pTime is allocated and freed by Viaccess-Orca.
      If pTime is NULL the function must return immediately without any processing.
@see tVA_OS_Time VA_OS_TimeToTm VA_OS_TmToTime
*/
void VA_OS_GetTime(tVA_OS_Time *pTime);


/**
This function translates a time value from the @ref tVA_OS_Time format 
to the @ref tVA_OS_Tm format.
@param pTime
       Address of the variable that contains the time value in the @ref tVA_OS_Time format.
@param pTm
       Address of the new time value in the @ref tVA_OS_Tm format.
@b Comments @n The memory pointed by pTime and by pTm is allocated and freed by Viaccess-Orca.
      If pTime or pTm is NULL the function must return immediately without any processing.
@see tVA_OS_Time tVA_OS_Tm VA_OS_TimeToTm
*/
void VA_OS_TimeToTm( const tVA_OS_Time *pTime, tVA_OS_Tm *pTm );


/**
This function translates a time value from the @ref tVA_OS_Tm format to 
the @ref tVA_OS_Time format.
@param pTm
       Address of the variable that contains the time value in the @ref tVA_OS_Tm format.
@param pTime
       Address of the new time value in the @ref tVA_OS_Time format.

@b Comments @n The memory pointed by pTm and by pTime is allocated and freed by Viaccess-Orca.
      If pTm or pTime is NULL the function must return immediately without any processing.
      The @ref VA_OS_TmToTime function ignores the specified contents of the uiWeekDay and 
      uiYearDay members of the @ref tVA_OS_Tm structure. It uses the values of the other
      components to determine the calendar time ; these components must not have unnormalized 
      values outside their normal ranges (for example : uiMonth = 13).
      In this case @ref tVA_OS_Time is set to (tVA_OS_Time)(-1).
@see tVA_OS_Time tVA_OS_Tm VA_OS_TimeToTm
*/
void VA_OS_TmToTime( const tVA_OS_Tm *pTm, tVA_OS_Time *pTime);


/**
This function indicates the processor runtime (in @ref tVA_OS_RunTime format) starting from the 
moment the STB is switched on. For more details refer to @ref tVA_OS_RunTime.
@param pRunTime  
       Indicates the address where the runtime value must be stored in the @ref tVA_OS_RunTime 
       format.
@b Comments @n The memory pointed by pRunTime is allocated and freed by Viaccess-Orca.
      If pRunTime is NULL the function must return immediately without any processing.
      If the STB manufacturer cannot reach a precision of less than a millisecond, 
      the uiMicroSec field of the variable pointed by pRunTime must be set to 0.
*/
void VA_OS_GetRunTime( tVA_OS_RunTime *pRunTime);

/**@}*/

/**@}*/

#endif /* _VA_OS_H_ */


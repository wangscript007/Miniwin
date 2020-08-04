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


#ifndef _VA_ERRORS_H_
#define _VA_ERRORS_H_

/**
 @ingroup common
 */

/**
 @defgroup errors va_errors.h
 @brief Provides the definitions of VO error codes
 @{
 */

/* Generic error codes */
#define kVA_ILLEGAL_HANDLE               0xFFFFFFFF /**< Illegal handle */
#define kVA_ERROR                        -1         /**< Error code by default */
#define kVA_INVALID_PARAMETER            -2         /**< Invalid parameter */
#define kVA_ALREADY_DONE                 -3         /**< Already done */
#define kVA_UNEXPECTED_CALLBACK          -4         /**< Unexpected callback */
#define kVA_RESOURCE_BUSY                -5         /**< Resource busy */
#define kVA_RESOURCE_NOT_LOCKED          -6         /**< Resource not locked */
/* #define kVA_RESOURCE_STILL_LOCKED     -7              Deprecated */
#define kVA_RESOURCE_ALREADY_LOCKED      -8         /**< Resource already locked */
#define kVA_CONNECTION_LOST              -9         /**< Connection lost */
#define kVA_INVALID_OPERATION            -10        /**< Invalid operation*/

/* Specific error codes */
#define kVA_STOP_FILTER_FIRST            -100       /**< Stop filter first */
#define kVA_SET_FILTER_PARAMETERS_FIRST  -101       /**< Set filter parameters first */
#define kVA_SWITCH_ON_PROGRAM_FIRST      -102       /**< Switch on program first   */
#define kVA_SWITCH_OFF_PROGRAM_FIRST     -103       /**< Switch off program first  */
#define kVA_INSERT_CARD_FIRST            -104       /**< Insert card first         */
#define kVA_RESET_CARD_FIRST             -105       /**< Reset card first          */
#define kVA_OPEN_ACS_FIRST               -106       /**< Open ACS first            */
#define kVA_TRANSITION_NOT_ALLOWED       -107       /**< Transition not allowed    */
#define kVA_EOF                          -108       /**< End of file               */
#define kVA_FILE_IS_READ_ONLY            -109       /**< File is read-only         */
#define kVA_FILE_NOT_FOUND               -110       /**< File not found            */

#define kVA_OK                            0         /**< Successful */

/* Warning */
#define kVA_SECTION_REJECTED              1         /**< Section rejected     */
#define kVA_NO_LOCKED_BUFFER              2         /**< No locked buffer     */
#define kVA_TIMEOUT                       3         /**< Timeout              */
#define kVA_DESCRIPTOR_CORRUPTED          4         /**< Descriptor corrupted */
#define kVA_NEXT_DESCRIPTOR               5         /**< Next descriptor      */
#define kVA_NO_MORE_DESCRIPTOR            6         /**< No more descriptor   */
#define kVA_DATA_NOT_AVAILABLE            7         /**< Data not available   */
#define kVA_NOT_IMPLEMENTED               8         /**< Not implemented      */
/* #define kVA_RELEASE_RESOURCES          9 Deprecated */

/* Common definitions */
#define kVA_WAIT_FOREVER                  0xFFFFFFFF /**< Wait forever */

/** @} */

#endif /* _VA_ERRORS_H_ */


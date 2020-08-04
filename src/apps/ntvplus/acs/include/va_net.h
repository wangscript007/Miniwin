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


#ifndef _VA_NET_H_
#define _VA_NET_H_

#include "va_types.h"
#include "va_errors.h"


/**
 @ingroup std_drivers
 */

/**
 @defgroup netDriver VA_NET API
 @brief This section describes the interface with a generic STB manufacturer VA_NET driver, used 
for providing two-way communications between the STB and the head-end via an IP network.
 @{
 */

/**
    @defgroup netConst VA_NET Constants
    @brief The VA_NET constants

    The VA_NET driver uses the following constants:
    
    In the `va_errors.h` header file:
    - @ref kVA_OK
    - @ref kVA_ILLEGAL_HANDLE
    - @ref kVA_INVALID_PARAMETER
    - @ref kVA_RESOURCE_BUSY
    - @ref kVA_CONNECTION_LOST
    - @ref kVA_WAIT_FOREVER

 @{
*/


/** @} */

/**
 @defgroup netEnum VA_NET Enumerations
 @brief The VA_NET enumerations
 @{
 */

/** These are the possible values for the VA_NET driver status */
typedef enum 
{
    eVA_NOT_CONNECTED,  /**< The STB is not connected. */
    eVA_OPEN_DONE,      /**< The request connection has been opened successfully. */
    eVA_OPEN_FAILED,    /**< The request open connection has failed. */
    eVA_RECEIVE_DONE,   /**< The information has been received successfully. */
    eVA_RECEIVE_FAILED, /**< The reception of information has failed. */
    eVA_SEND_DONE,      /**< The information has been sent successfully. */
    eVA_SEND_FAILED     /**< The send of the information has failed. */

} tVA_NET_Status;

/** @} */


/**
 @defgroup netCallback VA_NET Callback Functions
 @brief The VA_NET callbacks functions
 @{
 */

/**
    This is a notify callback function which notifies the situation after the requests open, 
    receive and send, in Unicast connection. It is a parameter of the 
    @ref VA_NET_UnicastOpen function.
    
    The following table shows the arguments required when this function is called.
    @image html  va_net_callback_arguments.png
    @image latex va_net_callback_arguments.png

    @param[in] dwVaNetHandle    A Viaccess-Orca handle is used to identify this connection.
                                The STB must pass this value back to Viaccess-Orca when calling notify 
                                callback functions.
    @param[in] eStatus          An enumeration to indicate the different status for unicast connection.
    @param[in] uiDataLength     Only present in the following cases:
                                 - @ref eVA_RECEIVE_DONE : The size of the returned data in bytes.
                                 - @ref eVA_SEND_FAILED : The number of bytes successfully sent 
                                 before the send failed/timed out.
    @param[out] pData           The data returned by the request if successful. The STB can release 
                                it when the function returns. Only present when `eStatus` is @ref eVA_RECEIVE_DONE.
    @retval kVA_OK                  If Viaccess-Orca was waiting for this function to be called.
    @retval kVA_INVALID_PARAMETER   If `dwVaNetHandle` does not refer to a valid, allocated Viaccess-Orca handle.
    @retval kVA_UNEXPECTED_CALLBACK If the function was called unexpectedly for this handle.
*/
typedef INT (*tVA_NET_Callback)(DWORD dwVaNetHandle, tVA_NET_Status eStatus, 
                                UINT32 uiDataLength ,BYTE *pData);

/** @} */

/**
 @defgroup manNetFunc VA_NET STB Manufacturer Functions
 @brief The VA_NET functions to be implemented by the STB manufacturer.

 @{
 The following section gives detailed information on the different network mechanisms already 
 listed in above.@n
 All of these functions are coded by the STB manufacturer.@n
 The functions are defined in:@n `va_net.h`
 */


/**
    This function initializes and attempts to open a standard TCP/IP connection between the STB and 
    a head-end server. Once the connection is successful, the STB will subsequently call the 
    function pointer @e pfVaUnicastCallback with the second parameter set to @ref eVA_OPEN_DONE. 
    This function must not block.
    
    If the attempt to open a connection fails asynchronously, the STB calls the function pointer
    @e pfVaUnicastCallback with the second parameter set to @ref eVA_OPEN_FAILED.

    @param[in] dwVaNetHandle        A Viaccess-Orca handle used to identify this connection. The STB must
                                    pass this value back to Viaccess-Orca when calling callback functions
                                    For more information refer to @ref tVA_NET_Callback.
    @param[in] pIPAddress           A NULL-terminated string describing the IP address of the 
                                    head-end server. For more information refer to @ref IP_Address_format "IP Address format".
    @param[in] uiPort               The port of the server to connect to. 
                                    For more information refer to @ref IP_Address_format "IP Address format".
    @param[in] uiTimeout            The timeout for an attempt to connect, in milliseconds. A value 
                                    of @ref kVA_WAIT_FOREVER indicates that the request never times 
                                    out.
    @param[in] pfVaUnicastCallback  The function pointer which returns the variable corresponding 
                                    to the type of the operation: open, receive or send.
    @retval kVA_ILLEGAL_HANDLE      If an invalid IP Address or port is specified. The only invalid 
                                    port is port 0.
    @retval "Any other value"       A valid STB net handle. Viaccess-Orca will pass this parameter to 
                                    the STB as dwStbNetHandle for any request that it makes for 
                                    this connection. See also @ref VA_NET_UnicastClose.
*/
DWORD VA_NET_UnicastOpen( DWORD dwVaNetHandle, 
                         const char *pIPAddress, UINT16 uiPort, UINT32 uiTimeout,
                         tVA_NET_Callback pfVaUnicastCallback);

/**
    This function requests the Unicast connection to fill a buffer. As soon as data has been 
    received from the network (the amount of data can be lower or equal to the @e uiMaximumLength),
    the STB passes it to Viaccess-Orca by calling the function pointer @e pfVaUnicastCallback with the 
    second parameter set to @ref eVA_RECEIVE_DONE.
    This function must not block.

    If the STB has received more data than that transmitted to the callback, the data is stored in 
    a queue until the next receive request is made.

    If the request times out before receiving any data, @e pfVaUnicastCallback with the second 
    parameter set to @ref eVA_RECEIVE_DONE is called, with the number of bytes equal to 0.
    
    If the connection is lost, due to a network failure, the receive request is cancelled, and the 
    STB calls the function pointer @e pfVaUnicastCallback with the second parameter set to 
    @ref eVA_RECEIVE_FAILED.

    Only one valid read request is possible at a time for any given handle.

    @param[in] dwStbNetHandle   A handle as returned by VA_NET_UnicastOpen.
    @param[in] uiMaximumLength  The maximum number of bytes that the STB is to retrieve from the 
                                connection.
    @param[in] uiTimeout        The timeout value of this request, in milliseconds. The 
                                @ref kVA_WAIT_FOREVER value indicates that the request never times out.

    @retval kVA_OK                  If the request is correctly formatted.
    @retval kVA_INVALID_PARAMETER   If @e dwStbNetHandle is not a valid handle to an open 
                                    connection, if @e uiMaximumLength equals 0.
    @retval kVA_RESOURCE_BUSY       If the requested connection is already processing another read 
                                    or open request.
    @retval kVA_CONNECTION_LOST     If the connection to the head-end server has been lost due to a 
                                    network failure.

    @note Keep in mind that as soon as the callback function is called, Viaccess-Orca may call 
          @ref VA_NET_UnicastReceive once more.
*/
INT VA_NET_UnicastReceive( DWORD dwStbNetHandle, UINT32 uiMaximumLength ,UINT32 uiTimeout );

/**
    This function sends a variable size block of data to the head-end server via the connection. If 
    the connection already has a send in progress, the request is rejected. This function should 
    return immediately, i.e. it does not block. Viaccess-Orca guarantees that the data held in @e pData
    remains valid until it is notified that the send request is complete by @e pfVaUnicastCallback
    with the second parameter set to @ref eVA_SEND_DONE or @e pfVaUnicastCallback with the second 
    parameter set to @ref eVA_SEND_FAILED.
    
    If the request times out, @e pfVaUnicastCallback with the second parameter set to 
    @ref eVA_SEND_FAILED is called.

    @param[in] dwStbNetHandle   A handle as returned by @ref VA_NET_UnicastOpen.
    @param[in] uiDataLength     The number of bytes that the STB is to send to the head-end server.
    @param[in] pData            A pointer to a contiguous block of memory of length @e uiDataLength.
    @param[in] uiTimeout        The timeout value of this request, in milliseconds.
                                The @ref kVA_WAIT_FOREVER value indicates that the request never times out.

    @retval kVA_OK                  If the request is correctly formatted.
    @retval kVA_INVALID_PARAMETER   If @e dwStbNetHandle is not a valid handle to an open 
                                    connection, if @e pData equals NULL or @e uiDataLength equals 0.
    @retval kVA_RESOURCE_BUSY       If the specified handle is already processing another send or 
                                    open request.
    @retval kVA_CONNECTION_LOST     If the connection to the head-end server has been lost due to a 
                                    network failure.
*/
INT VA_NET_UnicastSend ( DWORD dwStbNetHandle, 
                        UINT32 uiDataLength ,BYTE* pData, UINT32 uiTimeout ) ;


/**
    This function closes a Unicast connection and frees any resources allocated to the connection. 
    Once successfully called, the @e dwStbNetHandle becomes invalid, and any subsequent attempt to 
    perform an operation using this handle returns @ref kVA_INVALID_PARAMETER.
    
    If a receive is in progress when this function is called, @e pfVaUnicastCallback with the 
    second parameter set to @ref eVA_RECEIVE_FAILED is called, with any data already received being 
    discarded.
    
    If a send is in progress when this function is called, @e pfVaUnicastCallback with the second 
    parameter set to @ref eVA_SEND_FAILED is called.

    @param[in]  dwStbNetHandle      A handle as returned by @ref VA_NET_UnicastOpen.

    @retval kVA_OK                  If the connection was successfully closed.
    @retval kVA_INVALID_PARAMETER   If @e dwStbNetHandle is not a valid handle to an open connection.
*/
INT VA_NET_UnicastClose( DWORD dwStbNetHandle ) ;


/** @cond multicast */

/**
    This function initializes and opens a standard UDP communications port on the STB.

    @warning The VA_NET for multicast is deprecated and must not be part of the integration.

    @note Multicast connections can only be used to receive, not to send data.

    Only one handle can be allocated to any given IP pair (multicast/broadcast address:port). 
    Any attempt to allocate a second handle to an allocated IP pair results in a call to 
    @e pfVaMulticastCallback with the second parameter set to @ref eVA_OPEN_FAILED.

    Once the port has been opened, the STB automatically listens to any incoming data, and then 
    notifies Viaccess-Orca by calling @e pfVaMulticastCallback with the second parameter set to 
    @ref eVA_RECEIVE_DONE.

    In the case of a Multicast address, the STB is also to perform any necessary operations to join 
    the Multicast group as part of the open operation.

    This function does not block. If the attempt to make a Multicast connection fails asynchronously 
    (due to the inability to resolve the address name, or because the address:port pair already 
    has a connection made to it), the STB calls @e pfVaMulticastCallback with the second 
    parameter set to @ref eVA_OPEN_FAILED.

    @param[in] dwVaNetHandle          A Viaccess-Orca handle used to identify this connection. The STB 
                                      must pass this value back to Viaccess-Orca when calling callback 
                                      functions.
                                      For more information refer to @ref tVA_NET_Callback.
    @param[in] pIPAddress             A NULL-terminated string defining the IP Multicast address to 
                                      listen to.
                                      For more information refer to @ref IP_Address_format "IP Address format".
    @param[in] uiPort                 The port on the Multicast server that the STB listens to. 
                                      For more information refer to @ref IP_Address_format "IP Address format".
    @param[in] uiBufferSize           The minimum amount of data that the STB must be able to 
                                      receive before starting to reject data, i.e. the size of the 
                                      reception buffer.
    @param[in] pfVaMulticastCallback  The function pointer which returns the variable corresponding 
                                      to the type of the operation: open or receive.

    @retval    kVA_ILLEGAL_HANDLE     If the STB was unable to open a valid port. The only invalid 
                                      port is port 0.
    @retval    "Any other value"      A valid STB net handle. Viaccess-Orca will pass this parameter to 
                                      the STB as @e dwStbNetHandle for any request that it makes 
                                      for this connection. See also @ref VA_NET_MulticastClose.
*/
DWORD VA_NET_MulticastOpen( DWORD dwVaNetHandle, 
                           const char *pIPAddress, UINT16 uiPort, UINT32 uiBufferSize,
                           tVA_NET_Callback  pfVaMulticastCallback ) ;

/**
    This function closes a Multicast connection, and frees any resources allocated to the connection. 
    Once successfully called, the @e dwStbNetHandle becomes invalid, and any subsequent attempt 
    to perform an operation using this handle returns @ref kVA_INVALID_PARAMETER. Once called, any 
    data waiting to be passed to Viaccess-Orca by @e pfVaMulticastCallback with the second parameter 
    set to @ref eVA_RECEIVE_DONE is deleted immediately.

    @warning The VA_NET for multicast is deprecated and must not be part of the integration.

    @param[in]   dwStbNetHandle  A handle as returned by @ref VA_NET_MulticastOpen.

    @retval kVA_OK                  If the connection was successfully closed.
    @retval kVA_INVALID_PARAMETER   If @e dwStbNetHandle is not a valid handle to an open 
                                    Multicast communications port.
*/
INT VA_NET_MulticastClose( DWORD dwStbNetHandle ) ;

/** @endcond multicast */

/** @} */

/** @} */

#endif /* _VA_NET_H_ */


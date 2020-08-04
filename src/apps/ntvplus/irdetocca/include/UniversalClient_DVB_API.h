/*
 * Copyright 2009-2016 Irdeto B.V.
 *
 * This file and the information contained herein are the subject of copyright
 * and intellectual property rights under international convention. All rights
 * reserved. No part of this file may be reproduced, stored in a retrieval
 * system or transmitted in any form by any means, electronic, mechanical or
 * optical, in whole or in part, without the prior written permission of Irdeto
 * B.V.
 */

 /**
 * @file UniversalClient_DVB_API.h
 *
 * Cloaked CA Agent DVB API. This file has methods for 
 * passing Program Map Tables (PMTs) and Conditional Access Tables (CATs) that
 * are specific to the DVB transport. This API is an extension to
 * the common service processing layer located in UniversalClient_Common_API.h. 
 *
 */
#ifndef UNIVERSALCLIENT_DVB_API_H__INCLUDED__
#define UNIVERSALCLIENT_DVB_API_H__INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

#include "UniversalClient_Common_API.h"

/** @defgroup universalclient_api Cloaked CA Agent APIs

 *  Cloaked CA Agent APIs
 *
 *  These API methods provide the necessary interfaces for the Application in client device.
 *
 *  @{
 */

/** @defgroup dvbapi Cloaked CA Agent DVB APIs
 *  All APIs related to DVB-specific functionality
 *
 * These API methods provide the necessary functionality for configuring ECM services associated with 
 * programs defined in PMTs and EMM services associated with transports defined in CATs. 
 * 
 * An integration can use these methods to add CA to a device that supports DVB. 
 *
 *
 * See \ref commonapi "Cloaked CA Agent Common APIs" for fundamental methods for managing services and starting
 * and stopping the Cloaked CA Agent.
 *
 * See also \ref baf_dvb "Basic Application Flow for DVB"
 * for an overview of how the DVB API is used. 
  *  @{
 */


/**
 * Updates a service with a new PMT associated with a program.
 *
 * This function updates a service handle that was previously opened using ::UniversalClient_OpenService
 * by passing it a new PMT.  A single service handle may only be associated with one PMT. Multiple
 * service handles may be opened to deal with multiple programs at once.
 *
 * If the PMT has not changed since the service was updated, this function
 * does nothing. If it is new or has changed, it propagates the changes through the system, 
 * which may result in changes to active components for stream instances, 
 * and the opening and closing of streams. 
 *
 * When an application tunes to a DVB program, it should pass the PMT to this function
 * to listen for ECMs from all ECM streams in the PMT, process them, and send control
 * words back to the streams.
 *
 * This function performs the following:
 *
 * - The PMT is parsed to obtain a list of ECM stream PIDs, and for each ECM stream,
 *   the PIDs of the elementary streams that are scrambled by the ECMs.
 * - The streams in the list is compared against the streams in a previous call to this function.
 * - Each of the new streams in the list is opened (see ::UniversalClientSPI_Stream_Open),
 *   and the client processing thread begins communicating with each stream instance.
 * - Each of the streams removed from the list are closed
 * - Any streams associated with ECM PIDs whose elementary stream PIDs have changed will result
 *   in components on those streams changing (See ::UniversalClientSPI_Stream_AddComponent 
 *   and ::UniversalClientSPI_Stream_RemoveComponent).
 *
 * @param[in] serviceHandle Service handle previously opened by a call to ::UniversalClient_OpenService.
 * @param[in] pPmt Buffer containing the raw contents of the PMT table associated with a
 *     program. The uc_buffer_st::bytes member must point to a buffer containing the PMT, and
 *     the uc_buffer_st::length must be set to the number of bytes in the buffer.

 * 
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_DVB_NotifyPMT(
    uc_service_handle serviceHandle,
    const uc_buffer_st *pPmt);


/**
 * Updates a service with a new CAT associated with a transport
 *
 * This function updates a service handle that was previously opened using ::UniversalClient_OpenService
 * by passing it a new CAT.   
 * Typically, there is one CAT that refers to one EMM stream associated with a transport.
 * However, over time, the CAT may change. 
 *
 * A single service handle may only be associated with one CAT. 
 *
 * If the CAT has not changed since the service was updated, this function
 * does nothing. If it is new or has changed, it propagates the changes through the system, which may result in 
 * the closing and opening of streams. 
 * 
 * This function performs the following:
 *
 * - The CAT is parsed to obtain the EMM stream PID.
 * - If the PID is different from the previous CAT passed to this service, the previous stream
 *   will be closed and a new stream opened. (See ::UniversalClientSPI_Stream_Close and ::UniversalClientSPI_Stream_Open)
 * - A service handle is allocated.
 * - A stream is opened for the EMM PID, and associated with the service handle.
 *
 * @param[in] serviceHandle Service handle previously opened by a call to ::UniversalClient_OpenService.
 * @param[in] pCat Buffer containing the raw contents of the CAT table associated with a
 *     transport. The uc_buffer_st::bytes member must point to a buffer containing the CAT, and
 *     the uc_buffer_st::length must be set to the number of bytes in the buffer.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClient_DVB_NotifyCAT(
    uc_service_handle serviceHandle,
    const uc_buffer_st *pCat);

/** @} */ /* End of groups */

/**
 * @page stream_interface_call_sequence Stream Interface Call Sequence
 * 
 * The general sequence in which the methods of the stream interface methods 
 * defined in \ref streamspi "Cloaked CA Agent Stream SPIs" 
 * is called by the client during normal operation is provided in this section. Integrators 
 * should be aware of this sequence and plan accordingly when implementing the stream interface methods. 
 * ECM streams:
 * - Application opens a service using ::UniversalClient_OpenService
 * - Application notifies service of a new PMT of a program to be decrypted by passing it to ::UniversalClient_DVB_NotifyPMT.
 * - ::UniversalClientSPI_Stream_Open is called from the same thread to create a new stream instance, for each of the
 *   ECM PIDs in the PMT. 
 * - ::UniversalClientSPI_Stream_Connect is called from the client processing thread to associate each stream 
 *   with a connection.
 * - ::UniversalClientSPI_Stream_AddComponent is called for each elementary stream PID associated with the ECM PID. 
 * - ::UniversalClientSPI_Stream_OpenFilter and ::UniversalClientSPI_Stream_SetFilter is called from the client processing 
 *   thread to set up filters on the stream.
 * - ::UniversalClientSPI_Stream_Start is called, and the stream implementation begins listening for sections that match the filter. 
 * - When sections arrive on a stream, stream implementation calls ::uc_notify_callback.
 * - ::UniversalClientSPI_Stream_SetFilter is called from the client processing thread to update the filter.
 *   This gives the stream implementation an 
 *   opportunity to filter out repeated ECMs. This process is repeated whenever a new non-duplicate 
 *   ECM arrives. 
 * - ::UniversalClientSPI_Stream_SetDescramblingKey is called from the client processing thread to notify the stream of a successfully processed descrambling key. 
 * - Eventually, the application closes the service using ::UniversalClient_CloseService.
 * - ::UniversalClientSPI_Stream_Stop is called to stop all filters. 
 * - ::UniversalClientSPI_Stream_CloseFilter is called from the client processing thread to clean up any existing filters.
 * - ::UniversalClientSPI_Stream_RemoveComponent is called for each elementary stream PID associated with the ECM PID.
 * - ::UniversalClientSPI_Stream_Disconnect is called from the client processing thread to disassociate the stream from the connection.
 * - ::UniversalClient_CloseService waits for client processing thread to completely disconnect.
 * - ::UniversalClientSPI_Stream_Close is called from the same thread as ::UniversalClient_CloseService to free the stream instance. 
 *
 * DVB EMM streams:
 * - Application opens a service using ::UniversalClient_OpenService
 * - Application notifies service of a new CAT by passing it to ::UniversalClient_DVB_NotifyCAT.
 * - ::UniversalClientSPI_Stream_Open is called from the same thread to create a new stream instance for the EMM PID extracted from the CAT.
 * - ::UniversalClientSPI_Stream_Connect is called from the client processing thread to associate the stream 
 *   with a connection.
 * - ::UniversalClientSPI_Stream_OpenFilter and ::UniversalClientSPI_Stream_SetFilter is called from the client processing thread to set up filters on the stream.
 * - ::UniversalClientSPI_Stream_Start is called, and the stream implementation begins listening for sections that match the filter. 
 * - When sections arrive, stream implementation calls ::uc_notify_callback.
 * - If device information changes as a result of the EMM, ::UniversalClientSPI_Stream_SetFilter is called 
 *   to update the filter. This gives the implementation an opportunity to continue receiving EMMs addressed to
 *   it when device information changes.
 * - Eventually, the application closes the service using ::UniversalClient_CloseService.
 * - ::UniversalClientSPI_Stream_Stop is called to stop all filters. 
 * - ::UniversalClientSPI_Stream_CloseFilter is called from the client processing thread to clean up any existing filters.
 * - ::UniversalClientSPI_Stream_Disconnect is called from the client processing thread to disassociate the stream from the connection.
 * - ::UniversalClient_CloseService waits for client processing thread to completely disconnect.
 * - ::UniversalClientSPI_Stream_Close is called from the same thread as ::UniversalClient_CloseService to free the stream instance. 
 *
 * Pull EMM streams:
 * - Application opens a service using ::UniversalClient_OpenService.
 * - Application notifies service of CCIS address by passing it to ::UniversalClient_IPTV_SubmitServerAddress.
 * - ::UniversalClientSPI_Stream_Open is called from the same thread to create a new stream instance.
 * - ::UniversalClientSPI_Stream_Connect is called from the client processing thread to associate the stream 
 *   with the new connection.
 * - ::UniversalClientSPI_Stream_Start is called, and the stream implementation begins listening for the response from CCIS. 
 * - ::UniversalClientSPI_Stream_Send is called, and the stream implementation send the Pull EMM request message to CCIS . 
 * - When sections arrive, stream implementation calls ::uc_notify_callback.
 * - Eventually, the application closes the service using ::UniversalClient_CloseService.
 * - ::UniversalClientSPI_Stream_Stop is called.
 * - ::UniversalClientSPI_Stream_Disconnect is called from the client processing thread to disassociate the stream from the connection.
 * - ::UniversalClient_CloseService waits for client processing thread to completely disconnect.
 * - ::UniversalClientSPI_Stream_Close is called from the same thread as ::UniversalClient_CloseService to free the stream instance. 
 */



/** @} */ /* End of Univeral Client APIs */


#ifdef __cplusplus
}
#endif

#endif /* !UNIVERSALCLIENT_DVB_API_H__INCLUDED__ */

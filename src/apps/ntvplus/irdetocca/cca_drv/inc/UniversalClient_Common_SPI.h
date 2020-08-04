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
 * @file UniversalClient_Common_SPI.h
 * Header for Service Provider Interfaces (SPIs) for the common services layer. This file contains the methods 
 * for the transport-specific Service Provider Interfaces that must be implemented on a platform
 * for the Cloaked CA Agent API.
 */
#ifndef UNIVERSALCLIENT_COMMON_SPI_H__INCLUDED__
#define UNIVERSALCLIENT_COMMON_SPI_H__INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

#include "UniversalClient_SPI.h"
#include "UniversalClient_Common_API.h"

/** @defgroup universalclient_spi Cloaked CA Agent SPIs

 *  Cloaked CA Agent SPIs
 *
 *  Service Provider Interfaces(SPIs) are driver-level APIs called by Cloaked CA Agent.
 *  Client device platform must implement these SPIs in order to run Cloaked CA Agent.
 *
 *  @{
 */
 

/** @defgroup streamspi Cloaked CA Agent Stream SPIs
 *  @{
 */

/**
 * Stream method for opening a new instance of a DVB stream.
 *
 * This method is called to create a new instance of a DVB stream.
 * This is intended to process the input data in \a pStreamOpenParams
 * and set up any implementation-specific resources and allocate a stream handle, 
 * which is then passed to the other stream interface methods.  
 *
 * When this method is called, the stream implementation should process it 
 * and return ::UC_ERROR_SUCCESS. Any other result will be propagated through the 
 * Cloaked CA Agent API. 
 *
 * \note ::UniversalClientSPI_Stream_Open is a special SPI. 
 * 
 * \note When this SPI is assigned to ::uc_spi_implementation_st::Stream_Open in ::UniversalClientSPI_GetImplementation, and there is a compile warning,
 * please feel free to make a type convertion to remove the warning.
 * 
 * \note For DVB client, the correct prototype for this SPI is \n
 * \code
 * uc_result UniversalClientSPI_Stream_Open( 
 *     uc_connection_stream_type  streamType, 
 *     const uc_stream_open_params *pStreamOpenParams, //-----------Here is the difference!
 *     uc_stream_handle  *pStreamHandle); 
 * \endcode
 * but not \n
 * \code
 * uc_result UniversalClientSPI_Stream_Open(
 *     uc_connection_stream_type streamType,
 *     void *pStreamOpenParams,                       //-----------Here is the difference!
 *     uc_stream_handle *pStreamHandle);
 * \endcode
 *
 * @param[in] streamType indicating type of stream.  Different methods will be called on different type streams.
 *     The following stream methods will only be called if \a streamType is ::UC_CONNECTION_STREAM_TYPE_ECM:
 *     - ::UniversalClientSPI_Stream_SetDescramblingKey. \n
 *     The following stream methods will only be called if \a streamType is ::UC_CONNECTION_STREAM_TYPE_ECM or \n
 *     ::UC_CONNECTION_STREAM_TYPE_PVR_RECORD:
 *     - ::UniversalClientSPI_Stream_AddComponent.
 *     - ::UniversalClientSPI_Stream_RemoveComponent.
 * @param[in] pStreamOpenParams Pointer to structure that describes
 *      the parameters needed to open the stream.
 * @param[out] pStreamHandle Receives an implementation-specific handle to the stream.
 *      This stream handle will be passed to other stream interface methods
 *      in order to identify the instance of the stream, and finally in a call to 
 *      ::UniversalClientSPI_Stream_Close in order to free the handle. 
 *      NULL is not a valid stream handle. If the function returns success, it must not
 *      set *pStreamHandle to NULL. 
 * \note When streamType is ::UC_CONNECTION_STREAM_TYPE_PVR_RECORD or ::UC_CONNECTION_STREAM_TYPE_PVR_PLAYBACK 
 *       or ::UC_CONNECTION_STREAM_TYPE_PD, the ::uc_stream_open_params::caStream member in pStreamOpenParams 
 *       should be ignored.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_Open(
    uc_connection_stream_type streamType,
    const uc_stream_open_params *pStreamOpenParams,
    uc_stream_handle *pStreamHandle);

/**
 * Stream method to add a descrambling component to the stream.
 *
 * This method is used to add a component to a stream.
 * A 'component' is the information 
 * associated with an audio or video stream. 
 *
 * The implementation of this method should manage a list of active components,
 * such that when ::UniversalClientSPI_Stream_SetDescramblingKey is called, it can forward the
 * key to all PIDs that require it using the descrambler API. 
 *
 * \note ::UniversalClientSPI_Stream_AddComponent is a special SPI. 
 *
 * \note When this SPI is assigned to ::uc_spi_implementation_st::Stream_AddComponent in ::UniversalClientSPI_GetImplementation, and there is a compile warning,
 * please feel free to make a type convertion to remove the warning.
 *
 * \note For DVB client, the correct prototype for this SPI is \n
 * \code
 * uc_result UniversalClientSPI_Stream_AddComponent(
 *   uc_stream_handle streamHandle,
 *   const uc_elementary_component *pComponent //-----------Here is the difference!
 *   );
 * \endcode
 * but not \n
 * \code
 * uc_result UniversalClientSPI_Stream_AddComponent( 
 *   uc_stream_handle streamHandle,
 *   const uc_component *pComponent     //-----------Here is the difference!
 *   );
 * \endcode
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] pComponent Component structure that identifies the elementary stream for this component.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_AddComponent(
    uc_stream_handle streamHandle,
    const uc_elementary_component *pComponent);

/**
 * Stream method to remove a descrambling component from the stream.
 *
 * This method is used to remove a component from a stream, given a component definition.
 * A 'component' is the information
 * associated with an audio or video stream. 
 * 
 * The implementation of this method should search for a component with a matching definition
 * in the list of active components, and remove it if it is found. A component 'matches' if it
 * has the same PID as an active component.
 *
 * \note ::UniversalClientSPI_Stream_RemoveComponent is a special SPI. 
 *
 * \note When this SPI is assigned to ::uc_spi_implementation_st::Stream_RemoveComponent in ::UniversalClientSPI_GetImplementation, and there is a compile warning,
 * please feel free to make a type convertion to remove the warning.
 *
 * \note For DVB client, the correct prototype for this SPI is \n
 * \code
 * uc_result UniversalClientSPI_Stream_RemoveComponent(
 *   uc_stream_handle streamHandle,
 *   const uc_elementary_component *pComponent //-----------Here is the difference!
 *   );
 * \endcode
 * but not \n
 * \code
 * uc_result UniversalClientSPI_Stream_RemoveComponent(
 *   uc_stream_handle streamHandle,
 *   const uc_component *pComponent //-----------Here is the difference!
 *   ); 
 * \endcode
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] pComponent Component structure that identifies the elementary stream for this component.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_RemoveComponent(
    uc_stream_handle streamHandle,
    const uc_elementary_component *pComponent);


/** @} */ /* End of streamspi */


/** @} */ /* End of Cloaked CA Agent SPIs*/

#ifdef __cplusplus
}
#endif

#endif /* !UNIVERSALCLIENT_COMMON_SPI_H__INCLUDED__ */


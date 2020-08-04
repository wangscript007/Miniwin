/**
 * @file UniversalClient_Core_SPI.c
 */

/** @defgroup universalclient_spi Cloaked CA Agent SPIs

 *  Cloaked CA Agent SPIs
 *
 *  Service Provider Interfaces(SPIs) are driver-level APIs called by Cloaked CA Agent.
 *  Client device platform must implement these SPIs in order to run Cloaked CA Agent.
 *
 *  @{
 */

//////////////////////////////////////////////////////////////////////////////////////////
//	include header file
//////////////////////////////////////////////////////////////////////////////////////////
#include "stdio.h"
#include "UniversalClient.h"
#include "UniversalClient_SPI.h"
#include "UniversalClient_Common_SPI.h"

#include "dvbhal/gxdescrambler_hal.h"
#define CA_CCA_PVR_SUP 0	//MX
#define UC_SPI_DEBUG
/********************************************************************************/
/*                                 Macro                                        */
/********************************************************************************/
#ifdef UC_SPI_DEBUG
#define UC_SPI_DBG(fmt, arg...)   {printf("\033[0;35m[UC_SPI][%s](%d): ", __FUNCTION__, __LINE__); printf((char *)fmt, ##arg); printf("\033[m");}
#define UC_SPI_ERR(fmt, arg...)   {printf("\033[0;35m[UC_SPI][%s](%d): ", __FUNCTION__, __LINE__); printf((char *)fmt, ##arg); printf("\033[m");}
#else
#define UC_SPI_DBG(fmt, arg...)
#define UC_SPI_ERR(fmt, arg...)   {printf("\033[0;35m[UC_SPI][%s](%d): ", __FUNCTION__, __LINE__); printf((char *)fmt, ##arg); printf("\033[m");}
#endif

//////////////////////////////////////////////////////////////////////////////////////////
//	global variable
//////////////////////////////////////////////////////////////////////////////////////////
//handle_t cas_v = -1; 
//handle_t cas_a = -1; 


//////////////////////////////////////////////////////////////////////////////////////////
//	external function
//////////////////////////////////////////////////////////////////////////////////////////
/** @defgroup streamspi Cloaked CA Agent Stream SPIs
 *  @{
 */

/**
 * Stream method for opening a new instance of a generic stream.
 *
 * This method is called to create a new instance of the stream.
 * This is intended to process the implementation-specific input data in \a pStreamOpenParams
 * and set up any implementation-specific resources and allocate a stream handle,
 * which is then passed to the other stream interface methods.
 *
 * When this method is called, the stream implementation should process it
 * and return ::UC_ERROR_SUCCESS. Any other result will be propagated through the
 * Cloaked CA Agent API.
 *
 * \note This method is always called from the context of the thread that called
 * ::UniversalClient_OpenConnection.
 *
 * @param[in] streamType Indicating type of stream. Different methods can be called on
 *     different streams. For example, the following stream methods will only be called
 *     if \a streamType is ::UC_CONNECTION_STREAM_TYPE_ECM:
 *     - ::UniversalClientSPI_Stream_AddComponent
 *     - ::UniversalClientSPI_Stream_RemoveComponent
 *     - ::UniversalClientSPI_Stream_SetDescramblingKey
 * @param[in] pStreamOpenParams Value of \a pStreamOpenParams parameter
 *      that was originally passed to the ::UniversalClient_OpenConnection.
 *      Higher level APIs built on the Cloaked CA Agent Core API
 *      may also define specific values to be passed to this parameter.
 *
 * @param[out] pStreamHandle Receives an implementation-specific handle to the stream
 *      This stream handle will be passed to other stream interface methods
 *      in order to identify the instance of the stream, and finally in a call to
 *      ::UniversalClientSPI_Stream_Close in order to free the handle.
 *      UC_INVALID_HANDLE_VALUE is not a valid stream handle. If the function returns
 *      success, it must not set *pStreamHandle to UC_INVALID_HANDLE_VALUE.
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
//	void *pStreamOpenParams,
    const uc_stream_open_params *pStreamOpenParams,
	uc_stream_handle *pStreamHandle)
{
    const uc_stream_open_params *pStrOpenPara = pStreamOpenParams;
    uc_result retVal = UC_ERROR_SUCCESS;
    UC_SPI_DBG("streamType: %d streamHandle: %x pid: %d\n", streamType, *pStreamHandle, pStrOpenPara->caStream.pid);

    if(pStrOpenPara == NULL)
    {
        *pStreamHandle = UC_INVALID_HANDLE_VALUE;
        retVal = UC_ERROR_NULL_PARAM;
        return retVal;
    }

    if(streamType == UC_CONNECTION_STREAM_TYPE_EMM)
    {

        uc_uint32 idx = 0;
        for(idx = 0; idx < MAX_EMM_STREAM_NUM; idx++)
        {
            if(gEMMStreamInfo.streamHandle[idx] == UC_INVALID_HANDLE_VALUE)
            {
                //found resource
                break;
            }
        }

        if(idx >= MAX_EMM_STREAM_NUM)
        {
            *pStreamHandle = UC_INVALID_HANDLE_VALUE;
            retVal = UC_ERROR_TOO_MANY_HANDLES;
        }
        else
        {
            if(pStrOpenPara->caStream.protocolType == UC_STREAM_DVB)
            {
                gEMMStreamInfo.pid[idx] = pStrOpenPara->caStream.pid;
            }
            else
            {
                gEMMStreamInfo.pid[idx] = (uc_uint16)CCA_INVALID_PID;
            }
            *pStreamHandle = gEMMStreamInfo.streamHandle[idx] = (uc_stream_handle)((uc_stream_handle)EMM_STREAM_HANDLE + idx);
        }

    }
    else if(streamType == UC_CONNECTION_STREAM_TYPE_ECM)
    {
        int i = 0;
        for(i = 0; i < MAX_ECM_STREAM_NUM; i++)
        {
            if(gECMStreamInfo.streamHandle[i] == UC_INVALID_HANDLE_VALUE)
            {
                //found resource
                break;
            }
        }

        if(i >= MAX_ECM_STREAM_NUM)
        {
            *pStreamHandle = UC_INVALID_HANDLE_VALUE;
            retVal = UC_ERROR_TOO_MANY_HANDLES;
        }
        else
        {
            if(pStrOpenPara->caStream.protocolType == UC_STREAM_DVB)
            {
                gECMStreamInfo.pid[i] = pStrOpenPara->caStream.pid;
            }
            else
            {
                gECMStreamInfo.pid[i] = (uc_uint16)CCA_INVALID_PID;
            }
            *pStreamHandle = gECMStreamInfo.streamHandle[i] = (uc_stream_handle)((uc_stream_handle)ECM_STREAM_HANDLE + i);
        }
    }
    else if(streamType == UC_CONNECTION_STREAM_TYPE_PVR_RECORD)
    {
        #if(CA_CCA_PVR_SUP == 1)
        if(pStrOpenPara->caStream.protocolType == UC_STREAM_DVB)
        {
            gPVRStreamInfo.pvrRecordPid = pStrOpenPara->caStream.pid;
        }
        else
        {
            gPVRStreamInfo.pvrRecordPid = (uc_uint16)CCA_INVALID_PID;
        }
        *pStreamHandle = gPVRStreamInfo.pvrRecordStreamHandle = (uc_stream_handle)((uc_stream_handle)PVR_RECORD_STREAM_HANDLE);
        #else
        *pStreamHandle = (uc_stream_handle)PVR_RECORD_STREAM_HANDLE;
        #endif
    }
    else if(streamType == UC_CONNECTION_STREAM_TYPE_PVR_PLAYBACK)
    {
        #if(CA_CCA_PVR_SUP == 1)
        if(pStrOpenPara->caStream.protocolType == UC_STREAM_DVB)
        {
            gPVRStreamInfo.pvrPlaybackPid= pStrOpenPara->caStream.pid;
        }
        else
        {
            gPVRStreamInfo.pvrPlaybackPid = (uc_uint16)CCA_INVALID_PID;
        }
        *pStreamHandle = gPVRStreamInfo.pvrPlaybackStreamHandle= (uc_stream_handle)((uc_stream_handle)PVR_PLAYBACK_STREAM_HANDLE);
        #else
        *pStreamHandle = (uc_stream_handle)PVR_PLAYBACK_STREAM_HANDLE;
        #endif
    }
    else
    {
        *pStreamHandle = UC_INVALID_HANDLE_VALUE;
        retVal = UC_ERROR_NOT_IMPLEMENTED;
    }

    UC_SPI_DBG("ret: %d streamHandle: %x pid: %d\n", retVal, *pStreamHandle, pStrOpenPara->caStream.pid);

	return retVal;
}

/**
 * Stream method to add a descrambling component to the stream.
 *
 * This method is used to add a transport-defined component to a stream.
 * A 'component' is an audio, video, or other content stream identifier
 * that indicates a destination for descrambling keys.
 *
 * The implementation of this method can manage a list of active components,
 * such that when ::UniversalClientSPI_Stream_SetDescramblingKey is called, it can forward
 * the descrambling key to each of the active components.
 *
 * This method is called as the result of calling ::UniversalClient_AddComponent on an
 * open connection. The implementation may ignore this method if there is only one component
 * per stream or the concept of components does not apply.
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] pComponent Copy of component structure originally passed to ::UniversalClient_AddComponent.
 *     This can point to ::uc_component, or any structure based on ::uc_component. The size of the structure is indicated by
 *     the uc_component::size member. The type of this structure is application-specific.
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
	//const uc_component *pComponent)
	const uc_elementary_component *pComponent)
{
    uc_result retVal = UC_ERROR_SUCCESS;
    //const uc_elementary_component *pstreamComponent = pComponent;


	printf("YJC %s %d\n",__func__,__LINE__);

	if(pComponent == NULL)
	{
		retVal = UC_ERROR_NULL_PARAM;
	}
#if(CA_CCA_PVR_SUP == 1)
	else if(streamHandle == gPVRStreamInfo.pvrRecordStreamHandle)
	{
		UC_SPI_DBG("[%s %d] Stream_AddComponent for PVR RECORD\n", __FUNCTION__, __LINE__);
	}
	else if(streamHandle == gPVRStreamInfo.pvrPlaybackStreamHandle)
	{
		UC_SPI_DBG("[%s %d] Stream_AddComponent for PVR PLAYBACK\n", __FUNCTION__, __LINE__);
	}
#endif
	else
	{
		if(streamHandle == UC_INVALID_HANDLE_VALUE)
		{
			retVal = UC_ERROR_INVALID_HANDLE;
		}
		else if(((uc_stream_handle)streamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)ECM_STREAM_HANDLE&STREAM_TYPE_MASK))
		{
			//ECM stream
			int i = 0, j = 0;
			for(i = 0; i < MAX_ECM_STREAM_NUM; i++)
			{
				if(streamHandle == gECMStreamInfo.streamHandle[i])
				{
					//found stream handle
					break;
				}
			}
			if(i >= MAX_ECM_STREAM_NUM)
			{
				retVal = UC_ERROR_RESOURCE_NOT_FOUND;
			}
			else
			{
				if(pComponent->componentStream.protocolType == UC_STREAM_DVB)
				{
					uc_bool balreadyExist = FALSE;
					for(j = 0; j < MAX_ECM_COMPONENT_NUM; j++)
					{
						if(gECMStreamInfo.componentPid[i][j] == pComponent->componentStream.pid)
						{
							//already exist.
							balreadyExist = TRUE;
							break;
						}
					}
					
					printf("YJC %s %d\n",__func__,__LINE__);

					if(balreadyExist != TRUE)
					{
						 for(j = 0; j < MAX_ECM_COMPONENT_NUM; j++)
                        {
                            if(gECMStreamInfo.componentPid[i][j] == CCA_INVALID_PID)
                            {
								gECMStreamInfo.componentPid[i][j] = pComponent->componentStream.pid;
							#if 0
								gECMStreamInfo.dscmbHandle[i][j] = DESCR_Allocate(pComponent->componentStream.pid);
								if(gECMStreamInfo.dscmbHandle[i][j]<0)
									printf("\n %s %d DESCR_Allocate failed! 0x%x\n",__FUNCTION__,__LINE__,pComponent->componentStream.pid);
							#endif
								//gECMStreamInfo.dscmbHandle[i][j] = (uc_uint32)u32descmbler;
								break;
                            }
						 }	
					}
				}	
			}
		}
	}
    UC_SPI_DBG("ret: %d streamHandle: %x componentPid: %d\n", retVal, streamHandle, pComponent->componentStream.pid);

	return retVal;
}

/**
 * Stream method to remove a descrambling component from the stream.
 *
 * This method is used to remove a transport-defined component from a stream, given a component definition.
 * A 'component' is an audio, video, or other content stream identifier
 * that indicates a destination for descrambling keys.
 *
 * The implementation of this method should search for a component with a matching definition
 * in the list of active components, and remove it if it is found.  The mechanism
 * by which it matches the component is implementation-specific.
 *
 * This method is called as the result of calling ::UniversalClient_RemoveComponent on an
 * open connection. The implementation may ignore this method if there is only one component
 * per stream or the concept of components does not apply.
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] pComponent Copy of component structure originally passed to ::UniversalClient_AddComponent.
 *     This can point to ::uc_component, or any structure based on ::uc_component. The size of the structure is indicated by
 *     the uc_component::size member. The type of this structure is application-specific.
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
    //const uc_component *pComponent)
      const uc_elementary_component *pComponent)
{
    uc_result retVal = UC_ERROR_SUCCESS;
    const uc_elementary_component *pstreamComponent = NULL;
	
	UC_SPI_DBG("streamHandle: %x componentPid: 0x%x \n", streamHandle, pComponent->componentStream.pid);
#if 1
	if(pComponent == NULL)
	{
		retVal = UC_ERROR_NULL_PARAM;
	}
#if(CA_CCA_PVR_SUP == 1)
    else if(streamHandle == gPVRStreamInfo.pvrRecordStreamHandle)
	{
		UC_SPI_DBG("[%s %d] Stream_RemoveComponent for PVR RECORD\n", __FUNCTION__, __LINE__);
	}
	else if(streamHandle == gPVRStreamInfo.pvrPlaybackStreamHandle)
	{
		UC_SPI_DBG("[%s %d] Stream_RemoveComponent for PVR PLAYBACK\n", __FUNCTION__, __LINE__);
	}
#endif
    else
    {
        if(streamHandle == UC_INVALID_HANDLE_VALUE)
        {
            retVal = UC_ERROR_INVALID_HANDLE;
        }
        else if((streamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)ECM_STREAM_HANDLE&STREAM_TYPE_MASK))
        {
            //ECM stream
            int i = 0, j = 0;
            for(i = 0; i < MAX_ECM_STREAM_NUM; i++)
            {
                if(streamHandle == gECMStreamInfo.streamHandle[i])
                {
                    //found stream handle
                    break;
                }
            }
            if(i >= MAX_ECM_STREAM_NUM)
            {
                retVal = UC_ERROR_RESOURCE_NOT_FOUND;
            }
            else
            {
                pstreamComponent = pComponent;
                if(pstreamComponent->componentStream.protocolType == UC_STREAM_DVB)
                {
                    for(j = 0; j < MAX_ECM_COMPONENT_NUM; j++)
                    {
                        if(gECMStreamInfo.componentPid[i][j] == pstreamComponent->componentStream.pid)
                        {
                            //found
                            break;
                        }
                    }

                    if(j >= MAX_ECM_COMPONENT_NUM)
                    {
                        retVal = UC_ERROR_RESOURCE_NOT_FOUND;
                    }
                    else
                    {
                    	
                        uc_result retDscmb = UC_ERROR_SUCCESS;
						#if 0
                        retDscmb = MDrv_DSCMB_FltDisconnectPid((MS_U32)gECMStreamInfo.dscmbHandle[i][j], (MS_U32)gECMStreamInfo.componentPid[i][j]);
                        if(retDscmb == FALSE)
                        {
                            UC_SPI_ERR("MDrv_DSCMB_FltDisconnectPid err\n");
                        }

                        retDscmb = MDrv_DSCMB_FltFree((MS_U32)gECMStreamInfo.dscmbHandle[i][j]);
						#endif
						retDscmb = DESCR_Free(gECMStreamInfo.dscmbHandle[i][j]);
                        if(retDscmb != UC_ERROR_SUCCESS)
				        {
                            retVal = UC_ERROR_DRIVER_FAILURE;
				        }
                        else
                        {
                        	UC_SPI_DBG("streamHandle: %x componentPid: %d dscmbHandle %d\n", gECMStreamInfo.streamHandle[i], gECMStreamInfo.componentPid[i][j], gECMStreamInfo.dscmbHandle[i][j]);
                            gECMStreamInfo.dscmbHandle[i][j] = (uc_uint32)DRV_DSCMB_FLT_NULL;
                            gECMStreamInfo.componentPid[i][j] = (uc_uint16)CCA_INVALID_PID;
							//gECMStreamInfo.key_status[i][j].Odd_Key_Set_Status = 0;
							//gECMStreamInfo.key_status[i][j].Even_Key_Set_Status = 0;
                        }
                    }
                }
                else
                {
                    retVal = UC_ERROR_NOT_IMPLEMENTED;
                }
            }
        }
        else
        {
            retVal = UC_ERROR_NOT_IMPLEMENTED;
        }
    }
#endif
    UC_SPI_DBG("ret: %d streamHandle: %x componentPid: %d\n", retVal, streamHandle, pComponent->componentStream.pid);
	return retVal;
}


/** @} */ /* End of streamspi */

/** @} */ /* End of Cloaked CA Agent SPIs*/

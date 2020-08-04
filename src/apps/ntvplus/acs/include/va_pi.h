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
    $Revision: 23678 $
    $Date: 2012-05-15 17:05:45 +0200 (mar., 15 mai 2012) $
*/

#ifndef _VA_PI_H_
#define _VA_PI_H_

#include "va_types.h"
#include "va_errors.h"


#if defined VAOPT_DISABLE_VA_CTRL

typedef enum
{ 
    eLIVE = 0 ,
    ePERSISTENT_RECORD ,
    ePERSISTENT_PLAYBACK ,
    eTIMESHIFT_RECORD ,
    eTIMESHIFT_PLAYBACK ,
    eLIVE_AND_PERSISTENT_RECORD ,
    eLIVE_AND_TIMESHIFT_RECORD ,
    eLIVE_AND_TIMESHIFT_RECORD_AND_PERSISTENT_RECORD ,
    eTIMESHIFT_RECORD_AND_PERSISTENT_RECORD ,
    eTIMESHIFT_PLAYBACK_AND_PERSISTENT_RECORD

} tVA_PI_AcsMode;


typedef enum
{
    eCLEAR = 0,
    eVIACCESS_NOT_APPLICABLE,
    eWAITING_FOR_VIACCESS, 
    eVIACCESS_NOT_DESCRAMBLING,
    eVIACCESS_DESCRAMBLING,

    eQUERY_NO,
    eQUERY_MAYBE,
    eQUERY_YES

} tVA_PI_StreamEvent;

typedef struct
{
    WORD  wNetworkId;
    WORD  wTransportStreamId;
    WORD  wOriginalNetworkId;
    WORD  wServiceId;

} tVA_PI_ProgramInformation ;

typedef struct
{
    WORD  wPid ;
    BYTE  byType ;
 
} tVA_PI_StreamInformation ;

typedef enum 
{ 
    eRECORD_SCRAMBLED ,
    eRECORD_FORBIDDEN ,
    eTIMESHIFT_SCRAMBLED ,
    eTIMESHIFT_FORBIDDEN 

} tVA_PI_StreamRecordPolicy ;


INT VA_PI_OpenAcs( DWORD dwAcsId, tVA_PI_AcsMode eAcsMode );

INT VA_PI_ChangeAcsMode( DWORD dwAcsId, tVA_PI_AcsMode eNewAcsMode );

INT VA_PI_CloseAcs( DWORD dwAcsId );

INT VA_PI_TsChanged( DWORD dwAcsId );

INT VA_PI_CatUpdated( DWORD dwAcsId );

INT VA_PI_ParseCatDescriptors ( DWORD dwAcsId );

INT VA_PI_CatDescParsingFct(DWORD dwAcsId, BYTE *pDescriptor );

INT VA_PI_SwitchOnProgram( DWORD dwAcsId ) ; 

INT VA_PI_SwitchOffProgram( DWORD dwAcsId  );

INT VA_PI_PmtUpdated( DWORD dwAcsId );

INT VA_PI_ParsePmtProgramDescriptors( DWORD dwAcsId );

INT VA_PI_PmtProgramDescParsingFct( DWORD dwAcsId, BYTE *pDescriptor );

INT VA_PI_GetProgramInformation( DWORD dwAcsId, 
                                 tVA_PI_ProgramInformation *pProgramInfo );

DWORD VA_PI_AddStream( DWORD dwAcsId, DWORD dwStbStreamHandle );

INT VA_PI_QueryStream( DWORD dwAcsId, DWORD dwStbStreamHandle );

INT VA_PI_UpdateStream ( DWORD dwVaStreamHandle );

INT VA_PI_RemoveStream ( DWORD dwVaStreamHandle );

INT VA_PI_NotifyStreamEvent(DWORD dwStbStreamHandler, 
                            tVA_PI_StreamEvent eStreamEvent);

INT VA_PI_NotifyStreamRecordPolicy (DWORD dwStbStreamHandle, 
                                   tVA_PI_StreamRecordPolicy eStreamRecordPolicy);

INT VA_PI_ParsePmtStreamDescriptors( DWORD dwStbStreamHandle );

INT VA_PI_PmtStreamDescParsingFct( DWORD dwVaStreamHandle, BYTE *pDescriptor);

INT VA_PI_GetStreamInformation( DWORD dwStbStreamHandle, 
                               tVA_PI_StreamInformation *pStreamInfo);


#endif /* VAOPT_DISABLE_VA_CTRL */

#endif /* _VA_PI_H_ */




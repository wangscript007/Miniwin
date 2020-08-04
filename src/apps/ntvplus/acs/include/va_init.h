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

#ifndef _VA_INIT_H_
#define _VA_INIT_H_

#include "va_types.h"
#include "va_errors.h"
#include "va_setup.h"


typedef struct
{
    UINT8 uiSerialNumberSize;
    BYTE  *pSerialNumber;

} tVA_INIT_StbSerialNb;


typedef struct
{
    tVA_INIT_StbSerialNb stBoxIdentity ;
    WORD                 wConstructorIdentifier;
    WORD                 wModelIdentifier;
    BYTE                 byHardwareType;
    UINT8                uiHardwareVersion;
    BYTE                 bySoftwareType;
    UINT8                uiSoftwareVersion;

} tVA_INIT_StbInformation;



typedef struct
{
    UINT16 uiMaxNumberOfFilters ;
    UINT16 uiMaxSizeOfFilter;

} tVA_INIT_Demux;

typedef struct
{
    UINT16 uiMaxNumberOfChannels;

} tVA_INIT_Descrambler;

typedef struct
{
    tVA_INIT_Demux       stDemux;
    tVA_INIT_Descrambler stDescrambler;

}  tVA_INIT_AcsResources ;



typedef struct
{
    UINT32                  uiNbAcs ;
    UINT32                  uiNbSmartCard ;
    tVA_INIT_AcsResources   aAcs[kVA_SETUP_NBMAX_ACS] ;
    tVA_INIT_StbInformation stStbInformation ;

} tVA_INIT_ConfigurationParameters ;


INT VA_INIT_InitializeAcs(
    const tVA_INIT_ConfigurationParameters *pParameters) ;

void VA_INIT_StartAcs(void);

INT VA_INIT_StopAcs(void);


#endif /* _VA_INIT_H_ */


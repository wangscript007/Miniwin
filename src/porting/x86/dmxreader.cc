#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ngl_types.h>
#include <ngl_os.h>
#include <ngl_log.h>

#include <dmxreader.h>
#include <iostream>
#include "rtpsession.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include "rtplibraryversion.h"
#include "rtppacket.h"

NGL_MODULE(DMXREADER);

#define RBUFF_SIZE (188*100)
#define TSPACK_SIZE (188*7) 
#define ABS(a) ((a)>0?(a):-(a))

using namespace jrtplib;

void checkerror(int rtperr){
    if (rtperr < 0){
	std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
	exit(-1);
    }
}

static void TSRcvProc(void*p){
    void**pp=(void**)p;
    ON_TS_RECEIVED tscbk=(ON_TS_RECEIVED)pp[0];
    void*userdata=pp[1];
    bool done =false;
    int status;
    RTPSession sess;
    uint16_t portbase = 6664;
    pp[0]=NULL; 
    RTPUDPv4TransmissionParams transparams;
    RTPSessionParams sessparams;
    sessparams.SetOwnTimestampUnit(1.0/10.0);		
    sessparams.SetAcceptOwnPackets(true);
 
    transparams.SetPortbase(portbase);

    status = sess.Create(sessparams,&transparams);	
    NGLOG_ERROR_IF(status,"ERROR %d :%s",status, RTPGetErrorString(status).c_str());
 
    sess.BeginDataAccess();
    RTPTime delay(0.020);
    RTPTime starttime = RTPTime::CurrentTime();
    while (!done){
       status = sess.Poll();
       NGLOG_ERROR_IF(status,"ERROR %d :%s",status, RTPGetErrorString(status).c_str());
	
       if (sess.GotoFirstSourceWithData()){
           do{
		RTPPacket *pack;		
		while ((pack = sess.GetNextPacket()) != NULL){
                    size_t len=pack->GetPacketLength()/188;len*=188;
                    NGLOG_DEBUG_IF(len!=1316||pack->GetPayloadType()!=96,"type=%d len=%d",pack->GetPayloadType(),pack->GetPacketLength());
                    tscbk(pack->GetPayloadData(), len,userdata);
                    sess.DeletePacket(pack);
		}
           }while (sess.GotoNextSourceWithData());
       }
       RTPTime::Wait(delay);
    }
    NGLOG_INFO("TSReceiver Exit!!");	
    sess.EndDataAccess();
    delay = RTPTime(10.0);
    sess.BYEDestroy(delay,0,0);
}

void StartTSReceiver(ON_TS_RECEIVED cbk,void*data){
    HANDLE tid;
    void*params[2];
    params[0]=(void*)cbk;
    params[1]=data;
    nglCreateThread(&tid,0,0,TSRcvProc,params);
    while(params[0]!=NULL){
       usleep(10);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////

int get_ts_pid(BYTE*ts){
    return ((ts[1]&0x1F)<<8)|ts[2];
}

int get_ts_unit_start(BYTE*ts){
    return (ts[1]&0x40);
}

int get_ts_continue_count(BYTE*ts){
    return ts[3]&0x0F;
}

int get_ts_payload(BYTE*ts,BYTE*data,int issection){
    BYTE*pd=ts+4;
    int len=0;
    if(ts[3]&0x20)//adapt_field
        pd+=(*pd)+1;//(issection?1:0);
    if(get_ts_unit_start(ts)&&issection)
        pd+=(*pd)+1;
    if(ts[3]&0x10){
       len=188-(pd-ts);
       memcpy(data,pd,len);
    }
    return len;
}
int get_section_length(BYTE*sec){
    return (sec[1]&0x0F)<<8|sec[2];
}

#ifndef __SI_TABLE_H__
#define __SI_TABLE_H__
#include <ngl_types.h>
#include <ngl_os.h>
#include <string.h>
#include <stdlib.h>
#include <descriptors.h>
#include <ngl_tuner.h>
#include <vector>
enum{
   PID_PAT=0,
   PID_CAT=1,
   PID_TSDT=2,
   PID_NIT=0x10,
   PID_SDT=0x11,
   PID_BAT=0x11,
   PID_EIT=0x12,
   PID_RST=0x13,
   PID_TDT=0x14,
   PID_TOT=0x14,
   PID_INVALID=0x1FFF
};

enum{
   TBID_PAT=0x00,
   TBID_CAT=0x01,
   TBID_PMT=0x02,
   TBID_TSDT=0x03,
   TBID_DSI =0x3B,
   TBID_DDB =0x3C,
   TBID_SDT=0x42,
   TBID_SDT_OTHER=0x46,
   TBID_NIT=0x40,
   TBID_NIT_OTHER=0x41,
   TBID_BAT=0x4A,

   TBID_EITPF=0x4E,
   TBID_EITPF_OTHER=0x4F,
   TBID_EITS=0x50,
   TBID_EITS_LAST=0x5F,
   TBID_EITS_OTHER=0x60,
   TBID_EITS_OTHER_LAST=0x6F,

   TBID_TDT=0x70,
   TBID_RST=0x71,
   TBID_TOT=0x73,
   TBID_AIT=0x74,
   TBID_MPE=0x78,//MPE-FEC Section ETSI EN 301 192
// TBID_ECM=0x80,0x81
// TBID_EMM=0x82-0x8F
};
enum{//300468 page 77
   SVC_VIDEO=1,
   SVC_AUDIO=2,
   SVC_TELETEXT=3,
   SVC_NVOD_REF=4,
   SVC_NVOD_SHIFT=5,
   SVC_MOSAIC=6,
   SVC_FMRADIO=7,
   SVC_SRM=8,
   //9 reserved
   SVC_ADVANCE_CODEC_RADIO=0x0A,
   SVC_H264_AVC_MOSAIC=0x0B,
   SVC_DATA_BROADCAST=0x0C,
   //0x0d reserved
   SVC_RCS_MAP=0x0E,
   SVC_RCS_FLS=0x0F,
   SVC_MHP=0x10,
   SVC_MPEG2HD=0x11,
   //0x12-15 reserved
   SVC_H264_AVC_SD=0x16,
   SVC_H264_AVC_SD_REF=0x17,
   SVC_H264_AVC_SD_SHIFT=0x18,
   SVC_H264_AVC_HD=0x19,
   SVC_H264_AVC_HD_REF=0x1A,
   SVC_H264_AVC_HD_SHIFT=0x1B,
   SVC_HEVC_VIDEO=0x1F,
};

#define ISVIDEO(t) ((t==SVC_VIDEO)||(t==SVC_MPEG2HD)||(t==SVC_H264_AVC_SD)||(t==SVC_H264_AVC_HD)||(t==SVC_HEVC_VIDEO))
#define ISHDVIDEO(t) ((t==SVC_MPEG2HD)||(t==SVC_H264_AVC_HD))
#define ISAUDIO(t) ((t==SVC_AUDIO)||(t==SVC_FMRADIO)||(t==SVC_ADVANCE_CODEC_RADIO))

typedef std::vector<class PSITable>SECTIONLIST;
typedef class StreamDB_{
public:
    unsigned short netid;//orignetworkid for DVB/C/T or TPID for S/S2
    unsigned short tsid;
    NGLTunerParam tune;
    SECTIONLIST pat;
    SECTIONLIST pmt;
    SECTIONLIST sdt;
    StreamDB_(){
      netid=tsid=0;
    }
    void reset(){
        netid=tsid=0;
        memset(&tune,0,sizeof(NGLTunerParam));;
        pat.clear();
        pmt.clear();
        sdt.clear();
    }
}STREAMDB;

typedef std::vector<STREAMDB>STREAMLIST;

class PSITable{
protected:
    BYTE*data;
    bool owndata;
public:
    PSITable(const BYTE*buf,bool deepcopy=true);
    PSITable(const PSITable&b,bool deepcopy=true);
    ~PSITable();
    unsigned char tableId()const {return data[0];}
    unsigned char version()const {return (data[5]&0x3E)>>1;}
    size_t sectionLength()const {return ((data[1]&0x0F)<<8)|data[2]; }
    unsigned short extTableId()const { return (data[3]<<8)|data[4]; }
    unsigned char sectionNo()const {return data[6];}
    unsigned char lastSectionNo()const {return data[7];}
    unsigned int crc32()const{
        BYTE*p=data+sectionLength()-1;
        return p[0]<<24|p[1]<<16|p[2]<<8|p[3];
    }
    bool crcOK()const;
    bool operator!=(const PSITable & b)const;
    bool operator>=(const PSITable & b)const;
    bool operator<=(const PSITable & b)const;
    bool operator>(const PSITable & b)const;
    bool operator<(const PSITable & b)const;
    bool operator==(const PSITable & b)const;
    PSITable&operator=(const PSITable &b);
    operator const BYTE*()const{return data;}
};

typedef struct{
    USHORT program_number;
    USHORT pmt_pid;
}MpegProgramMap,PROGRAMMAP;

enum {
    ST_VIDEO,
    ST_AUDIO,
    ST_TELETEXT,
    ST_SUBTITLE,
    ST_DATA
}STREAMCATEGEORY;

class PAT:public PSITable{
public:
     PAT(const BYTE*buf,bool deepcopy=true):PSITable(buf,deepcopy){}
     PAT(const PSITable&b,bool deepcopy=true):PSITable(b,deepcopy){}
     USHORT getStreamId()const{return extTableId();}
     int getPrograms(PROGRAMMAP*maps,bool nit=false)const;
     int getPMTPID(USHORT program_number)const;
};

typedef class MpegElement:public Descriptors{
public:
    BYTE stream_type; 
    char iso639lan[4];
    USHORT pid;
    USHORT caid;
    USHORT ecmpid;
    INT getCategory()const;
    INT getType()const;//convert to porting video/audio type
    BOOL getCAECM(USHORT*caid,USHORT*ecmpid)const;
}ELEMENTSTREAM;

class PMT:public PSITable{
public:
     BYTE*getProgramDescriptors(INT&len)const{
          len=((data[10]&0x0F)<<8)|data[11];
          return data+12;
     }
public:
     PMT(const BYTE*buf,bool deepcopy=true):PSITable(buf,deepcopy){}
     PMT(const PSITable&b,bool deepcopy=true):PSITable(b,deepcopy){}
     INT getCAECM(USHORT*caid,USHORT*ecmpid)const;
     USHORT pcrPid()const{return ((data[8]&0x1F)<<8)|data[9];}
     int getElements(ELEMENTSTREAM*es,bool own=true)const;
     int getTeletexts(TeletextItem*items);
     int getSubtitles(SubtitleItem*items);
     USHORT getProgramNumber()const{return extTableId();}
};

class CAT:public PSITable{
public:
     CAT(const BYTE*buf,bool deepcopy=true):PSITable(buf,deepcopy){}
     CAT(const PSITable&b,bool deepcopy=true):PSITable(b,deepcopy){}
     BYTE*  getDescriptors(int&len)const;
     USHORT getEMMPID()const;//find in CA_Descriptor
};

typedef class DVBService:public Descriptors{
public:
     USHORT netid;
     USHORT tsid;
     USHORT service_id;
     BYTE eitS;
     BYTE eitPF;
     BYTE runStatus;
     BYTE freeCAMode;
     BYTE serviceType;
     DVBService();
     DVBService(const DVBService&b);
     DVBService(const BYTE*buf,INT len,bool deep=true);
     int getServiceName(char*name,char*provider=nullptr)const;
}SERVICE;

class SDT :public PSITable{
public:
     SDT(const BYTE*buf,bool deepcopy=true):PSITable(buf,deepcopy){}
     SDT(const PSITable&b,bool deepcopy=true):PSITable(b,deepcopy){}
     USHORT getStreamId()const{return extTableId();}
     USHORT getNetId()const{return (data[8]<<8)|data[9];}
     int getServices(DVBService*services,bool own=true)const;
};

typedef class DVBEvent:public Descriptors{
public:
   UINT event_id;
   DWORD start_time;
   DWORD duration;
   BYTE nibble;
   BYTE rating;
   DVBEvent();
   DVBEvent(const DVBEvent&);
   int getShortName(char*name,char*des)const;
   int getNibble(BYTE*nb1,BYTE*nb2)const;
   int getExtend(char*text,const char*lan=nullptr)const;
   int getRating(const char*lan=NULL)const;
}EVENT;

class EIT:public PSITable{
public:
    EIT(const BYTE*dt,bool deepcopy=true):PSITable(dt,deepcopy){}
    EIT(const PSITable&b,bool deepcopy=true):PSITable(b,deepcopy){}
    USHORT getStreamId()const{return (data[8]<<8)|data[9];}
    USHORT getNetId()const{return (data[10]<<8)|data[11];}
    USHORT getServiceId()const{return extTableId();}
    BYTE getSegmentLastSecNo()const{return data[12];}
    BYTE getLastTableId()const{return data[13];}
    int getEvents(EVENT*events,bool own=true)const;
    operator const SERVICELOCATOR()const;
};

typedef class DVBStream:public Descriptors{
public:
   USHORT tsid;
   USHORT netid;
   bool getDelivery(NGLTunerParam*tp)const;
   int getServices(SERVICELOCATOR*svc)const;
   int getLCN(SERVICELOCATOR*svc,USHORT*lcn)const;
}STREAM;

class NIT:public PSITable{
public:
   NIT(const BYTE*dt,bool deepcopy=true):PSITable(dt,deepcopy){}
   NIT(const PSITable&b,bool deepcopy=true):PSITable(b,deepcopy){}
   BYTE*getNetworkDescriptors(int&len)const{
       len=((data[8]&0x0F)<<8)|data[9];
       return data+10;
   }
   virtual int getName(char*name,const char*lan)const;
   int getStreams(STREAM*ts,bool own)const;
   int getLCN(SERVICELOCATOR*svc,USHORT*lcn)const;//get lcn from first descriptor loop
   int matchServices(SERVICELOCATOR*svc,int size)const;//only used for LCNV1 in first descriptor
};

class BAT:public NIT{
public:
   BAT(const BYTE*dt,bool deepcopy=true):NIT(dt,deepcopy){}
   BAT(const PSITable&b,bool deepcopy=true):NIT(b,deepcopy){}
   BYTE*getBouquetDescriptors(int&len)const{return getNetworkDescriptors(len);} 
   virtual int getName(char*name,const char*lan)const override;
};

class TDT:public PSITable{
public:
   TDT(const BYTE*dt,bool deepcopy=true):PSITable(dt,deepcopy){}
   void getTime(INT&mjd,INT&utc)const{
      mjd=(data[3]<<8)|data[4];
      utc=(data[5]<<16)|(data[6]<<8)|data[7];
   }   
};

typedef struct {
    BYTE country_code[4];
    BYTE region_id;
    BYTE polarity;
    USHORT local_time_offset;
    ULONG time_of_change;
    USHORT next_time_offset;
}TimeOffset;

class TOT:public TDT{
public:
   TOT(const BYTE*dt,bool deepcopy=true):TDT(dt,deepcopy){}
   BYTE*getDescriptors(int&len)const;
   int getTimeOffset(TimeOffset*tms,int size)const;
};

#endif

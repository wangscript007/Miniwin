#ifndef __SATELLITE_H__
#define __SATELLITE_H__
#include <ngl_types.h>
#include <ngl_tuner.h>

NGL_BEGIN_DECLS
typedef NGLTunerParam TRANSPONDER;
#define MAX_SATELLITE_NAME_LEN 32
typedef struct{
   USHORT position;
   USHORT antennaId;//Antenna ID bind to this satellite
   USHORT direction;//0--west 1--east
   USHORT lnb;//0--OFF 1--HORIZONTAL 2-VERTICAL
   USHORT k22;//0-OFF 1--ON
   USHORT diseqc;//0-none 1-diseqc-A ,2-B 3-C 4-D
   CHAR name[MAX_SATELLITE_NAME_LEN]; 
}SATELLITE;

/////////////////////////////////////////////////////////////////////////
int LoadSatelliteFromDB(const char*fname);
int SaveSatellite2DB(const char*fname);
int GetSatelliteCount();
int GetSatelliteInfo(int idx,SATELLITE*sat);
int GetSatelliteTp(int satid,TRANSPONDER*tps,int tp_count);
int AddSatellite(SATELLITE*sat);
int RemoveSatellite(int id);
int AddTp2Satellite(int satid,const TRANSPONDER*tp);
int GetParamsByTP(int tpid,TRANSPONDER*tp,SATELLITE*sat);
int RemoveTransponder(int tpid);
int RemoveTpFromSatellite(int satid,int tpid);
int ConfigureTransponder(const TRANSPONDER*tp);
int ConfigureTransponderById(int tpid);//configure 22k polarity ...
/////////////////////////////////////////////////////////////////////////

NGL_END_DECLS

#endif

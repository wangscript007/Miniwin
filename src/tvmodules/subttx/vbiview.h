#ifndef __VBI_VIEW_H__
#define __VBI_VIEW_H__
#include <view.h>
#include <ngl_types.h>
#include <ngl_dmx.h>
#include <ngl_log.h>
#include <ngl_os.h>
extern "C"{
#include <vbi.h>
#include <exp-gfx.h>
#include <exp-txt.h>
#include <hamm.h>
#include <dvb_demux.h>
#include <cc608_decoder.h>
#include <sliced.h>
}

namespace nglui{

#define MAX_SLICES 32
class VBIView:public View{
protected:

   HANDLE hfilter;
   vbi_decoder   *vbi_dec;
   vbi_dvb_demux *vbi_dmx;
   _vbi_cc608_decoder*cc608;
   vbi_sliced sliced[MAX_SLICES];
   vbi_page  page;
   vbi_pgno pgno;
   vbi_subno subno;
   vbi_pgno pgno_req;
   vbi_subno subno_req;
   int vbi_events;
   struct{
      vbi_pgno pgno;
      vbi_subno subno;
   }nav_link[6];
   RefPtr<ImageSurface>imagesurface;
   static void DATA_CBK(HANDLE dwVaFilterHandle,const BYTE *pBuffer,UINT uiBufferLength, void *pUserData);
   static BOOL VBI_CBK(vbi_dvb_demux *dx,void *data,const vbi_sliced *sliced,UINT sliced_lines,int64_t pts);
   static void VBI_EVENT_CBK(vbi_event *ev, void *data);

   virtual void onPageReceived(vbi_event&evt)=0;
public:
   VBIView(int w,int h);
   ~VBIView();
   virtual void start(int pid,BOOL tsmod=TRUE); 
   virtual void requestPage(int page,int sub=VBI_ANY_SUBNO)=0;
   void stop();
};

}
#endif

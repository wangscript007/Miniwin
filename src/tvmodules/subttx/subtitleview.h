#ifndef __SUBTITLE_VIEW_H__
#define __SUBTITLE_VIEW_H__
#include <view.h>
#include <mutex>

typedef struct decoder_s decoder_t;
typedef struct dvbsub_color_s dvbsub_color_t;
namespace nglui{

class SubtitleView:public View{
private:
   class Page{
   protected:
     std::vector<std::shared_ptr< class SubtitleRegion >>regions;
   public:
     void add_region(std::shared_ptr<SubtitleRegion>rgn){
          regions.push_back(rgn);
     }
     void draw(GraphContext&canvas);
     void reset();
     int getSubtitleRegionCount(){return regions.size();}
   };
protected:
   std::mutex page_lock;
   USHORT pid;
   USHORT composition_page_id;
   USHORT ancillary_page_id;
   void* filter;
   decoder_t* sub_dec;
   std::shared_ptr<Page>page;
   static void PES_CBK(HANDLE dwVaFilterHandle,const BYTE *pBuffer,UINT uiBufferLength, void *pUserData);
   static int SUBTITLE_RENDER(decoder_t*dec,void*data);
   SubtitleRegion*create_region(const BYTE*pixels,int w,int h,dvbsub_color_t*pals,int npal);
public:
   SubtitleView(int w,int h);
   ~SubtitleView();
   void onDraw(GraphContext&canvas)override;
   void decode(USHORT pid,USHORT composition_page,USHORT ancillary_page);
};

}
#endif

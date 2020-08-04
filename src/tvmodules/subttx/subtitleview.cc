#include <subtitleview.h>
#include <ngl_dmx.h>
#include <ngl_log.h>
#include <dvbsub.h>

NGL_MODULE(SubtitleView);

namespace nglui{

class SubtitleRegion{
public:
  int region_id;
  int x,y;
  int width,height;
  BYTE*pixels;
  std::string text;
  SubtitleRegion(const char*txt,int tx,int ty){
     pixels=nullptr;
     text=txt;
     width=height=0;
     set_pos(tx,ty);
  }
  SubtitleRegion(int w,int h){
      width=w,   height=h;
      x = y= 0;
      region_id=0;
      pixels=(BYTE*)malloc(w*h*4);
  }
  void set_pos(int x,int y){
      this->x=x;this->y=y;
  }
  ~SubtitleRegion(){
      if(pixels)free(pixels);
  }
};

SubtitleView::SubtitleView(int w,int h):View(w,h){
    sub_dec=(decoder_t*)malloc(sizeof(decoder_t));
    setBgColor(0);
    filter=nullptr;
    page=std::make_shared<Page>();
}

SubtitleView::~SubtitleView(){
   dvbsub_close(sub_dec);
   free(sub_dec);
   nglFreeSectionFilter(filter);
}

void SubtitleView::onDraw(GraphContext&canvas){
   View::onDraw(canvas);
   std::lock_guard<std::mutex> lock(page_lock);
   page->draw(canvas);
#ifdef DEBUG
   if(page->getSubtitleRegionCount()){
      char fname[128];
      sprintf(fname,"page_%d.png",page->getSubtitleRegionCount());
      canvas.get_target()->write_to_png(fname);
      NGLOG_DEBUG("write png %s",fname);
   }
#endif
}

void SubtitleView::decode(USHORT pid,USHORT composition_page,USHORT ancillary_page){
   if( (this->pid==pid)||(composition_page_id==composition_page)
       ||(ancillary_page_id==ancillary_page))
       return ;
   this->pid=pid;
   composition_page_id=composition_page;
   ancillary_page_id=ancillary_page;
   dvbsub_open(sub_dec,composition_page_id,ancillary_page_id);
   dvbsub_setrender(sub_dec,SubtitleView::SUBTITLE_RENDER,this);
   filter=nglAllocateSectionFilter(0,pid,PES_CBK,this,DMX_PES);
   nglStartSectionFilter(filter);
   NGLOG_DEBUG("dec=%p pid=%d page=%d,%d",sub_dec,pid,composition_page_id,ancillary_page_id);
}

void SubtitleView::PES_CBK(HANDLE dwVaFilterHandle,const BYTE *pBuffer,UINT uiBufferLength, void *pUserData){
    SubtitleView*thiz=(SubtitleView*)pUserData;
    block_t block;
    block.p_buffer=(BYTE*)pBuffer;
    block.i_buffer=uiBufferLength;
    block.i_size=uiBufferLength;
    block.i_flags=0;
    block.i_pts=0;
    int rc=decode_time_stamp(&block.i_pts,pBuffer+9,0x21);
    NGLOG_VERBOSE_IF(uiBufferLength>184,"dec=%p buf:%p/%d pts:%lld/%d",thiz->sub_dec,pBuffer,uiBufferLength,block.i_pts/9000,rc);
    block.p_buffer+=14;
    block.i_buffer-=14;
    dvbsub_decode(thiz->sub_dec,&block);
}

SubtitleRegion*SubtitleView::create_region(const BYTE*pixels,int w,int h,dvbsub_color_t*pals,int npal){
    uint32_t colors[256];
    nglui::SubtitleRegion*rgn=new nglui::SubtitleRegion(w,h);
    
    RefPtr<ImageSurface>img=ImageSurface::create(Surface::Format::ARGB32,w,h);
    NGLOG_VERBOSE("%dx%d pal=%p npal=%d imsg.data=%p",w,h,pals,npal,img->get_data());
    for(int i=0;i<npal;i++){
       dvbsub_color_t c=pals[i];
       colors[i]=dvbsub_ycbcr2argb(&c);
       NGLOG_VERBOSE("PAL[%d],%02x,%02x,%02x,%02x-->%08x",i, c.Y,c.Cb,c.Cr,c.T,colors[i]);
    }
    BYTE*dst=rgn->pixels;
    for(int y=0;y<h;y++){
        UINT *pd=(UINT*)dst;
        for(int x=0;x<w;x++)
            pd[x]=colors[pixels[x]];
        pixels+=w;
        dst+=img->get_stride();
    }
    return rgn;
}

void SubtitleView::Page::draw(GraphContext&canvas){
    for(auto r:regions){
        canvas.set_source_rgb(0,0,0);
        if(r->pixels){
            canvas.rectangle(r->x,r->y,r->width,r->height);
            canvas.fill();
            RefPtr<ImageSurface>img=ImageSurface::create(r->pixels,Surface::Format::ARGB32,r->width,r->height,r->width*4);
            canvas.draw_image(img,r->x,r->y);
        }else if(!r->text.empty()){
            canvas.rectangle(r->x,r->y,1280,50);
            canvas.fill();
            canvas.set_font_size(40);
            canvas.draw_text(r->x,r->y,r->text);
        }
        NGLOG_VERBOSE("draw region[%d] to %d,%d img.size=%dx%d",r->region_id,r->x,r->y,r->width,r->height);
    }
}

void SubtitleView::Page::reset(){
    regions.clear();
}

int SubtitleView::SUBTITLE_RENDER(decoder_t*p_dec,void*data){
    decoder_sys_t *p_sys = p_dec->p_sys;
    subpicture_t *p_spu=NULL;
    SubtitleView*thiz=(SubtitleView*)data;
    int i, j;
    int i_base_x=p_sys->i_spu_x;
    int i_base_y=p_sys->i_spu_y;

    /* Allocate the subpicture internal data. */
    if( p_sys->display.b_windowed ) {
        /* From en_300743v01 - */
        /* the DDS is there to indicate intended size/position of text */
        /* the intended video area is ->i_width/height */
        /* the window is within this... SO... we should leave i_original_picture_width etc. as is */
        /* and ONLY change i_base_x.  effectively i_max_x/y are only there to limit memory requirements*/
        /* we COULD use the upper limits to limit rendering to within these? */

        /* see notes on DDS at the top of the file */
        i_base_x += p_sys->display.i_x;
        i_base_y += p_sys->display.i_y;
    }
    std::lock_guard<std::mutex> lock(thiz->page_lock);
    thiz->page->reset();
    /* Loop on region definitions */
    NGLOG_DEBUG("basepos=%d,%d windowed=%d",i_base_x,i_base_y,p_sys->display.b_windowed);
    NGLOG_DEBUG_IF(p_sys->p_page,"rendering %i regions", p_sys->p_page->i_region_defs);
    for( i = 0; p_sys->p_page && ( i < p_sys->p_page->i_region_defs ); i++ ) {
        dvbsub_region_t     *prgn;
        dvbsub_regiondef_t  *prgndef;
        dvbsub_clut_t       *p_clut;
        dvbsub_color_t      *p_color;

        prgndef = &p_sys->p_page->p_region_defs[i];

        /* Find associated region */
        for( prgn = p_sys->p_regions; prgn != NULL;prgn = prgn->p_next ) {
            if( prgndef->i_id == prgn->i_id ) break;
        }

        /* if a region exists, then print it's size */
        NGLOG_DEBUG_IF(prgn,"rendering region %i (%i,%i) to (%i,%i)", i,prgndef->i_x, prgndef->i_y,
                     prgndef->i_x + prgn->i_width, prgndef->i_y + prgn->i_height );
        NGLOG_DEBUG_IF(!prgn,"rendering region %i (%i,%i) (no region matched to render)",i,prgndef->i_x,prgndef->i_y);

        if( !prgn ) {
            NGLOG_DEBUG("region %i not found", prgndef->i_id );
            continue;
        }

        /* Find associated CLUT */
        for( p_clut = p_sys->p_cluts; p_clut != NULL; p_clut = p_clut->p_next ) {
            if( prgn->i_clut == p_clut->i_id ) break;
        }
        if( !p_clut ) {
            NGLOG_DEBUG("clut %i not found", prgn->i_clut );
            continue;
        }
        /* FIXME: don't create a subpicture region with VLC CODEC YUVP
         * when it actually is a TEXT region */

        int i_entries= (prgn->i_depth==1)?4:( (prgn->i_depth==2)?16:256);
        p_color = (prgn->i_depth==1) ? p_clut->c_2b : ( (prgn->i_depth==2) ? p_clut->c_4b : p_clut->c_8b );

        NGLOG_DEBUG("create_region[%d]:%dx%d",prgn->i_id,prgn->i_width,prgn->i_height);
        std::shared_ptr<SubtitleRegion> rgn(thiz->create_region(prgn->p_pixbuf,prgn->i_width,prgn->i_height,p_color,i_entries));
        rgn->set_pos(i_base_x+prgndef->i_x,prgndef->i_y+i_base_y);
        rgn->region_id=prgn->i_id;
        thiz->page->add_region(rgn);
        /* Check subtitles encoded as strings of characters  * (since there are not rendered in the pixbuffer) */
        for( j = 0; j < prgn->i_object_defs; j++ ) {
            dvbsub_objectdef_t *p_object_def = &prgn->p_object_defs[j];

            if( ( p_object_def->i_type != 1 ) || !p_object_def->psz_text ) continue;
            std::shared_ptr<SubtitleRegion> rgnt(new nglui::SubtitleRegion(p_object_def->psz_text,
                  i_base_x + prgndef->i_x + p_object_def->i_x,i_base_y + prgndef->i_y + p_object_def->i_y));
            rgnt->region_id=p_object_def->i_id;
            NGLOG_DEBUG("create_region[%d]%s",p_object_def->i_id,p_object_def->psz_text);
            thiz->page->add_region(rgnt);
        }
        thiz->invalidate(nullptr);
    }
}
}

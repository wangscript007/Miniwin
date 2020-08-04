#include <vbiview.h>
#include <ngl_log.h>

NGL_MODULE(VBIView);

extern "C" vbi_dvb_demux *_vbi_dvb_ts_demux_new(vbi_dvb_demux_cb*,void*,unsigned int);
extern "C" void vbi_teletext_channel_switched(vbi_decoder*);
extern "C" void vbi_caption_channel_switched(vbi_decoder*);

namespace nglui{

VBIView::VBIView(int w,int h)
  :View(w,h){
    vbi_dec=nullptr;
    hfilter=nullptr;
    vbi_dmx=nullptr;
    cc608=nullptr;
    vbi_events=VBI_EVENT_CLOSE|VBI_EVENT_LOCAL_TIME|VBI_EVENT_PROG_INFO
         |VBI_EVENT_PROG_ID|VBI_EVENT_ASPECT|VBI_EVENT_CAPTION|VBI_EVENT_TTX_PAGE|_VBI_EVENT_CC608;

    vbi_dec=vbi_decoder_new();
    cc608=_vbi_cc608_decoder_new();

    memset(&page,0,sizeof(page));
}

void VBIView::start(int pid,BOOL tsmod){
   if(hfilter)
       nglFreeSectionFilter(hfilter);
   if(vbi_dmx)
       vbi_dvb_demux_delete(vbi_dmx);

   if(tsmod)
       vbi_dmx=_vbi_dvb_ts_demux_new(VBI_CBK,this,pid);
   else
       vbi_dmx=vbi_dvb_pes_demux_new(NULL,this);//VBI_CBK,this);

   vbi_teletext_channel_switched(vbi_dec);
   vbi_caption_channel_switched(vbi_dec);

   vbi_event_handler_add(vbi_dec,vbi_events,VBI_EVENT_CBK,this);
   _vbi_cc608_decoder_add_event_handler(cc608, _VBI_EVENT_CC608,VBI_EVENT_CBK,this);

   hfilter=nglAllocateSectionFilter(0,pid,DATA_CBK,this,tsmod?DMX_TS:DMX_PES);
   nglStartSectionFilter(hfilter);
   NGLOG_DEBUG("filter= %p pid=%d vbi=%p cc608_dec=%p",hfilter,pid,vbi_dec,cc608);
}

void VBIView::stop(){
   nglFreeSectionFilter(hfilter);
   vbi_dvb_demux_delete(vbi_dmx);

   hfilter=nullptr;
   vbi_dmx=nullptr;
   cc608=nullptr;
}

VBIView::~VBIView(){
    vbi_dvb_demux_delete(vbi_dmx);
    nglFreeSectionFilter(hfilter);
    vbi_decoder_delete(vbi_dec);
    _vbi_cc608_decoder_delete(cc608);
}

typedef struct block_s{
    BYTE*p_buffer; /**< Payload start */
    size_t i_buffer; /**< Payload length */
    size_t i_size; /**< Buffer total size */
    UINT i_flags;
    int64_t i_pts;
    int64_t i_dts;
}block_t;

void VBIView::DATA_CBK(HANDLE dwVaFilterHandle,const BYTE *pBuffer,UINT uiBufferLength, void *pUserData){
    VBIView*ccvbi=(VBIView*)pUserData;
    block_t block;
    block.p_buffer=(BYTE*)pBuffer;
    block.i_buffer=uiBufferLength;
    block.i_size=uiBufferLength;
    block.i_flags=0;
    block.i_pts=0;
    block.p_buffer+=45;
    block.i_buffer-=45;
    //NGLOG_DUMP("PES",pBuffer,64);
    //NGLOG_DUMP("BLOCK",block.p_buffer,64);
    if( block.i_buffer > 0 && ( ( block.p_buffer[0] >= 0x10 && block.p_buffer[0] <= 0x1f ) ||
          ( block.p_buffer[0] >= 0x99 && block.p_buffer[0] <= 0x9b ) ) ) {
        vbi_sliced   *p_sliced = ccvbi->sliced;
        UINT i_lines = 0;

        block.i_buffer--;
        block.p_buffer++;
        while( block.i_buffer >= 2 ){
            INT     i_id   = block.p_buffer[0];
            UINT i_size = block.p_buffer[1];

            if( 2 + i_size > block.i_buffer ) break;

            if( ( i_id == 0x02 || i_id == 0x03 ) && i_size >= 44 && i_lines < MAX_SLICES ){
                if(block.p_buffer[3] == 0xE4 ){    /* framing_code */
                    UINT line_offset  = block.p_buffer[2] & 0x1f;
                    UINT field_parity = block.p_buffer[2] & 0x20;

                    p_sliced[i_lines].id = VBI_SLICED_TELETEXT_B;
                    if( line_offset > 0 )
                        p_sliced[i_lines].line = line_offset + (field_parity ? 0 : 313);
                    else
                        p_sliced[i_lines].line = 0;
                    for( int i = 0; i < 42; i++ )
                        p_sliced[i_lines].data[i] = vbi_rev8( block.p_buffer[4 + i] );
                    i_lines++;
                }
            }
            block.i_buffer -= 2 + i_size;
            block.p_buffer += 2 + i_size;
        }
        if( i_lines > 0 )
            vbi_decode(ccvbi->vbi_dec, p_sliced, i_lines, 0 );
        NGLOG_VERBOSE("vbi_decode %d lines",i_lines);
    }
    ccvbi->requestPage(ccvbi->pgno);
}

BOOL VBIView::VBI_CBK(vbi_dvb_demux *dx,void *user_data,const vbi_sliced *sliced,UINT sliced_lines,int64_t pts){
    double ts;
    VBIView*ccvbi=(VBIView*)user_data;
    vbi_decode(ccvbi->vbi_dec,(vbi_sliced*)sliced,sliced_lines,ts);
    NGLOG_VERBOSE("%d lines",sliced_lines);
    return true;
}

void VBIView::VBI_EVENT_CBK(vbi_event *ev, void *data){
    switch(ev->type){
    case VBI_EVENT_LOCAL_TIME:{
             const vbi_local_time *lt= ev->ev.local_time;
             NGLOG_VERBOSE("TIME:%s",asctime(localtime(&lt->time)));
          }break;
    case VBI_EVENT_ASPECT:{
             const vbi_aspect_ratio *p=&ev->ev.aspect;
             NGLOG_DEBUG("VBI_EVENT_ASPECT:ratio=%f filmmode=%d subtitle=%d",
                p->ratio,p->film_mode,p->open_subtitles);
          }break;
    case VBI_EVENT_PROG_INFO:{
             const vbi_program_info*pg= ev->ev.prog_info;
             NGLOG_DEBUG("VBI_EVENT_PROG_INFO:[%s]",pg->title);
          }break;
    case VBI_EVENT_PROG_ID:NGLOG_DEBUG("VBI_EVENT_PROG_ID");break;
    case VBI_EVENT_NETWORK:{
             const vbi_network*n=&ev->ev.network;
             NGLOG_DEBUG("VBI_EVENT_NETWORK:[%s]call[%s]",n->name,n->call);
          }break;
    case _VBI_EVENT_CC608:
          NGLOG_VERBOSE("VBI_EVENT_CC608");break;
    case VBI_EVENT_CAPTION:
          NGLOG_VERBOSE("VBI_EVENT_CAPTION");
          ((VBIView*)data)->onPageReceived(*ev);
          break;
    case VBI_EVENT_TTX_PAGE:
          NGLOG_VERBOSE("VBI_EVENT_TTX");
          ((VBIView*)data)->onPageReceived(*ev);
          break; 
    case VBI_EVENT_CLOSE:
       break;
    }
}


}//namespace

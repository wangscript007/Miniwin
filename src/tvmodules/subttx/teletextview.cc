#include <teletextview.h>
#include <assert.h>

NGL_MODULE(TeletextView);

#define CW 12
#define CH 10

namespace nglui{

TeletextView::TeletextView(int w,int h)
  :INHERITED(w,h){
    setFlag(FOCUSABLE);
    setFlag(ENABLED);
    selected_link=-1;
    pgno_req=pgno=0x100;//0x100 is default homepage
    subno_req=subno=VBI_ANY_SUBNO;
    vbi_events|=VBI_EVENT_TTX_PAGE;
    timerstarted=0;

    imagesurface=ImageSurface::create(Surface::Format::ARGB32,41*12,25*10);
    RefPtr<Context>simg=Context::create(imagesurface);
    simg->set_source_rgb(1,1,1);
    simg->rectangle(0,0,41*12,25*10);
    simg->fill();
}


TeletextView::~TeletextView(){
}

void TeletextView::start(int pid,BOOL ts){
   VBIView::start(pid,ts);
   if(!timerstarted){
      sendMessage(WM_TIMER,0,0,1000);
      timerstarted++;
   }
}

void TeletextView::requestPage(int pg,int sub){
    if( (pgno==pg&&subno==sub)||(pg==pgno_req&&sub==subno_req))
        return;
    int rc=vbi_is_cached(vbi_dec,pg,sub);
    pgno_req=pg;
    subno_req=sub;
    NGLOG_DEBUG("requet page %x,%x cached=%d",pg,sub,rc);
    if(rc){
        vbi_fetch_vt_page(vbi_dec,&this->page,pg,sub,VBI_WST_LEVEL_3p5,25, 1);
        build_links();
        this->pgno=pg;
        this->subno=sub;
        invalidate(nullptr);
    }
}

int TeletextView::build_links(){
    vbi_link_ext ll;//last_link
    vbi_link*pll=(vbi_link*)&ll;
    vbi_char*pc=page.text;
    int flashed=0;
    links.clear();
    memset(pll,0,sizeof(vbi_link_ext));
    pll->pgno=-1;pll->subno=-1;
    for(int i=0;i<page.rows;i++){
        for(int j=0;j<page.columns;j++){
            flashed+=pc[j].flash;
            if(pc[j].link){
                vbi_link l;
                vbi_resolve_link(&page,j,i,&l);
                if(l.pgno!=pll->pgno||l.subno!=pll->subno||strcmp((const char*)l.url,(const char*)pll->url)){
                    vbi_size sz=(vbi_size)pc[j].size;
                    if(pll->pgno!=-1){
                       links.push_back(ll);
                       NGLOG_VERBOSE("[%d,%d]->page[%x,%x]%s linksize=%dx%d",i,j,l.pgno,l.subno,l.url,ll.width,ll.height);
                    }
                    ll.col=j;ll.row=i;
                    ll.color=page.color_map[pc[j].foreground];
                    ll.height=(sz==VBI_DOUBLE_WIDTH||sz==VBI_DOUBLE_SIZE)?2:1;
                    ll.width=(sz==VBI_DOUBLE_HEIGHT||sz==VBI_DOUBLE_SIZE)?2:1;
                    ll.link=l;
                }else{//samelink as last char
                    ll.width++;
                }
            }else if(ll.link.pgno!=-1){
                NGLOG_VERBOSE("lastlink->page[%x,%x] linksize=%dx%d",ll.link.pgno,ll.link.subno,ll.width,ll.height);
                links.push_back(ll);ll.link.pgno=-1;
            }
        }pc+=page.columns;
    }NGLOG_DEBUG("%d flashed char pagesize=%dx%d",flashed,page.columns,page.rows);
    for(int i=0;i<6;i++)
       NGLOG_DEBUG("%d-->%x,%x",i,page.nav_link[i].pgno,page.nav_link[i].subno);
    selected_link=links.size()?0:-1;
    return links.size();
}


void TeletextView::getLinkRect(vbi_link_ext&l,RECT&rect){
    RECT rc=getClientRect();
    float sx=(float)rc.width/(41*12);
    float sy=(float)rc.height/(25*10);
    vbi_link*pl=(vbi_link*)&l;
    rect.x=l.col*12.*sx;
    rect.y=l.row*10.*sy;
    rect.width=l.width*12.*sx;
    rect.height=l.height*10.*sy;
}

void TeletextView::onDraw(GraphContext&canvas){
    RECT rc=getClientRect();

    vbi_draw_vt_page(&page, VBI_PIXFMT_RGBA32_LE,imagesurface->get_data(),FALSE,FALSE);
    canvas.draw_image(imagesurface,&rc,NULL,ST_FIT_XY);

    if(selected_link>=0&&selected_link<links.size()){
        RECT rect;
        getLinkRect(links[selected_link],rect);
        rect.inflate(5,2);
        canvas.set_line_width(2);
        canvas.set_color(links[selected_link].color);
        canvas.rectangle(rect.x,rect.y,rect.width,rect.height);
        canvas.stroke();
    }
    if(!pgentered.empty()){
        canvas.set_source_rgb(0,1,0);
        canvas.set_font_size(84);
        canvas.draw_text(rc,pgentered,DT_CENTER|DT_VCENTER);
    }
    vbi_unref_page(&page);
}

bool TeletextView::onMessage(DWORD msgid,DWORD wParam,ULONG lParam){
    switch(msgid){
    case WM_LOAD_TTXPAGE:
         return true;
    case WM_TIMER:
         if(entering){
             entering=0;
         }else if(!pgentered.empty()){
             int pg=strtoul(pgentered.c_str(),nullptr,16);
             requestPage(pg);
             pgentered="";
             invalidate(nullptr);
         }
         sendMessage(msgid,wParam,lParam,1000);
         return true; 
    default:return VBIView::onMessage(msgid,wParam,lParam);
    }
}

void TeletextView::onPageReceived(vbi_event&ev){
    int pgno_rcved=ev.ev.ttx_page.pgno;
    int subno_rcved=ev.ev.ttx_page.subno;
    NGLOG_VERBOSE("rcvd:%x,%x show:%x,%x req:%x,%x",pgno_rcved,subno_rcved,pgno,subno,pgno_req,subno_req);
    if( (pgno_rcved==pgno) && (VBI_ANY_SUBNO==subno||subno_rcved==subno)){
        NGLOG_DEBUG("update header:%d clock:%d roll_head:%d pnoffset:%d",ev.ev.ttx_page.header_update,
             ev.ev.ttx_page.clock_update,ev.ev.ttx_page.roll_header,ev.ev.ttx_page.pn_offset);
    }
    if( (pgno_rcved==pgno_req) && (VBI_ANY_SUBNO==subno_req||subno_rcved==subno_req)){
        pgno=pgno_req;
        subno=subno_req;
        vbi_fetch_vt_page(vbi_dec, &page,pgno,subno,VBI_WST_LEVEL_3p5,25, 1);
        build_links();
        invalidate(nullptr);
    }
}

bool TeletextView::onKeyUp(KeyEvent&event){
    if(links.size()==0)
        return INHERITED::onKeyUp(event);
    switch(event.getKeyCode()){
    case KEY_LEFT:
    case KEY_UP:
          selected_link=(selected_link-1+links.size())%links.size();
          invalidate(nullptr);
          NGLOG_DEBUG("focus=%d",selected_link);
          return true;
    case KEY_RIGHT:
    case KEY_DOWN:
          selected_link=(selected_link+1)%links.size();
          invalidate(nullptr);
          NGLOG_DEBUG("focus=%d",selected_link);
          return true;
    case KEY_0:
          pgentered.append(1,'0');
          entering++;
          invalidate(nullptr);
          return true;
    case KEY_1...KEY_9:
          entering++;
          pgentered.append(1,('1'+event.getKeyCode()-KEY_1));
          invalidate(nullptr);
          return true;
    case KEY_ENTER:{
          vbi_link_ext l=links[selected_link];
          requestPage(l.link.pgno,l.link.subno);
          }return true;
    default:return false;
    } 
}

BOOL TeletextView::vbi_page_has_flash(const vbi_page *pg){
   const vbi_char *cp;
   const vbi_char *end;
   UINT attr;
   end = pg->text + pg->rows * pg->columns;
   attr = 0;
   for (cp = pg->text; cp < end; ++cp)
       if(cp->flash)return TRUE;
   return FALSE;
}


}//namespace

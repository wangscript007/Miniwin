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
    switch(event.getKeyCode()){
    case KEY_LEFT:
    case KEY_UP:
          if(links.size()==0)
             return INHERITED::onKeyUp(event);
          selected_link=(selected_link-1+links.size())%links.size();
          invalidate(nullptr);
          NGLOG_DEBUG("focus=%d",selected_link);
          return true;
    case KEY_RIGHT:
    case KEY_DOWN:
          if(links.size()==0)
             return INHERITED::onKeyUp(event);
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
void TeletextView::destroy_patch(struct ttx_patch *p){
    assert (NULL != p);
    if (p->scaled_on)
       pixman_image_unref (p->scaled_on);
    if (p->scaled_off)
       pixman_image_unref (p->scaled_off);
    if (p->unscaled_on)
       pixman_image_unref (p->unscaled_on);
    if (p->unscaled_off)
       pixman_image_unref (p->unscaled_off);
    memset(p,0,sizeof(ttx_patch));
}
void TeletextView::delete_patches(){
    struct ttx_patch *p;
    struct ttx_patch *end;

    end = patches + n_patches;
    for (p = patches; p < end; ++p)
       destroy_patch (p);

    free (patches);
    patches = NULL;
    n_patches = 0;
}
void TeletextView::add_patch(UINT column,UINT row,UINT columns,vbi_size size, BOOL flash)
{
    struct ttx_patch *p;
    struct ttx_patch *end;
    INT  pw, ph;
    INT ux, uy;
    UINT endcol;

    assert (NULL != unscaled_on);
    assert (NULL != unscaled_off);

    end = patches + n_patches;
    endcol = column + columns;

    for (p = patches; p < end; ++p)
       if (p->row == row && p->column < endcol
          && (p->column + p->columns) > column){
	  /* Patches overlap, we replace the old one. */
  	  destroy_patch (p);
	  break;
       }

      if (p >= end){
         UINT size;

         size = (n_patches + 1) * sizeof (*patches); 
         patches = (ttx_patch*)realloc(patches,size);

         p = patches + n_patches;
         ++n_patches;
      }

      p->column		= column;
      p->row		= row;
      p->scaled_on	= NULL;
      p->scaled_off	= NULL;
      p->unscaled_off	= NULL;
      p->columns	= columns;
      p->phase		= 0;
      p->flash		= flash;
      p->dirty		= TRUE;

      switch (size){
      case VBI_DOUBLE_WIDTH:
         p->width = 2;    p->height = 1;
         break;
      case VBI_DOUBLE_HEIGHT:
         p->width = 1;    p->height = 2;
         break;
      case VBI_DOUBLE_SIZE:
         p->width = 2;    p->height = 2;
         break;
      default:
         p->width = 1;    p->height = 1;
         break;
     }

     ux = (0 == p->column) ? 0 : p->column * CW - 5;
     uy = (0 == p->row) ? 0 : p->row * CH - 5;

     pw = p->width * p->columns * CW + 10;
     ph = p->height * CH + 10;

     p->unscaled_on = pixman_image_create_bits(PIXMAN_a8r8g8b8,pw, ph,NULL,0);
     assert (NULL != p->unscaled_on);
     //z_pixbuf_copy_area(/*src*/unscaled_on,ux,uy,pw,ph,/*dst*/p->unscaled_on,0,0);

     if (flash){
         p->unscaled_off = pixman_image_create_bits (PIXMAN_a8r8g8b8,pw, ph,NULL,0);
         assert (p->unscaled_off != NULL);
         //z_pixbuf_copy_area (/*src*/unscaled_off, ux, uy, pw, ph,/*dst*/p->unscaled_off,0,0);
     }

     if (scaled_on){
         UINT sw, sh;
         UINT uw, uh;

         sw = pixman_image_get_width (scaled_on);
         sh = pixman_image_get_height(scaled_on);

         uw = pixman_image_get_width (unscaled_on);
         uh = pixman_image_get_height(unscaled_on);

         scale_patch (p, sw, sh, uw, uh);
     }
}
void TeletextView::scale_patch(struct ttx_patch *p, UINT sw, UINT sh, UINT uw, UINT uh)
{
    UINT srcw;
    UINT srch;
    UINT dstw;
    UINT dsth;
    INT n;
 
    assert (NULL != p);

    if (p->scaled_on){
       pixman_image_unref (p->scaled_on);
       p->scaled_on = NULL;
    }

    if (p->scaled_off){
       pixman_image_unref(p->scaled_off);
       p->scaled_off = NULL;
    }

    srch = p->height * CH + 10;
    dsth = (sh * srch + (uh >> 1)) / uh;
    n = (0 == p->row) ? 0 : 5;
    p->sy = dsth * n / srch;
    p->sh = ceil (dsth * (n + p->height * CH) / (double) srch) - p->sy;
    p->dy = p->sy + lrint (floor (sh * p->row * CH / (double) uh - dsth * n / (double) srch + .5));
 
    srcw = p->width * p->columns * CW + 10;
    dstw = (sw * srcw + (uw >> 1)) / uw;
    n = (0 == p->column) ? 0 : 5;
    p->sx = dstw * n / srcw;
    p->sw = ceil (dstw * (n + p->width * p->columns * CW)/ (double) srcw) - p->sx;
    p->dx = p->sx + lrint (floor (sw * p->column * CW / (double) uw - dstw * n / (double) srcw + .5));

    /*if (dstw > 0 && dsth > 0){
       p->scaled_on =z_pixbuf_scale_simple (p->unscaled_on,(int) dstw, (int) dsth, interp_type);

       if (p->flash)
           p->scaled_off =  z_pixbuf_scale_simple (p->unscaled_off,(int) dstw, (int) dsth,interp_type);

       p->dirty = TRUE;
    }*/
}

void  TeletextView::build_patches(){
    vbi_char *cp;
    UINT row;

    delete_patches ();

    //if (!page)return;

    cp = page.text;

    for (row = 0; row < page.rows; ++row){
       UINT col = 0;
       while (col < page.columns){
	  if ((cp[col].flash) && cp[col].size <= VBI_DOUBLE_SIZE){
	      UINT n;
	      for (n = 1; col + n < page.columns; ++n)
		 if (!(cp[col + n].flash)
		     || cp[col].size != cp[col + n].size)
	 	   break;
	      add_patch (col, row, n, (vbi_size)cp[col].size, /* flash */ TRUE);
	      col += n;
	  }else{
	      ++col;
	  }
       }
       cp += page.columns;
    }
}


}//namespace

#include <ccview.h>
#include <ngl_log.h>
#define ABS(x) ((x)>0?(x):-(x))

NGL_MODULE(CCVIEW);

namespace nglui{

CCView::CCView(int w,int h):VBIView(w,h){
    vbi_events|=VBI_EVENT_TTX_PAGE;
    imagesurface=ImageSurface::create(Surface::Format::ARGB32,41*12,25*10);
    RefPtr<Context>simg=Context::create(imagesurface);
    simg->set_source_rgb(1,1,1);
    simg->rectangle(0,0,41*12,25*10);
    simg->fill();
    pgno=1;//VBI_CAPTION_CC1;
}

void CCView::onPageReceived(vbi_event&ev){
    switch(ev.type){
    case VBI_EVENT_CAPTION:   onCaption(ev);break;
    case _VBI_EVENT_CC608 :   onCC(ev);break;
    }
}

void CCView::requestPage(int pg,int sub){
    if( (pgno==pg&&subno==sub)||(pg==pgno_req&&sub==subno_req))
        return;
    bool cached=vbi_fetch_cc_page(vbi_dec,&page,pg,TRUE);
    NGLOG_DEBUG("page %x cached=%d",pg,cached);
    if(cached)invalidate(nullptr);
}

void CCView::onCaption(vbi_event&ev){
    if(pgno!=ev.ev.caption.pgno)
        return;
    vbi_fetch_cc_page (vbi_dec, &page,ev.ev.caption.pgno,/*reset dirty flags*/TRUE);
    if(ABS(page.dirty.roll) > page.rows) {
         //old_clear_display ();
         //update_display = TRUE;
    } else if (page.dirty.roll == -1) {
         //old_roll_up (page.dirty.y0, page.dirty.y1);
         /*if (smooth_rolling) {
             vert_offset = CELL_HEIGHT - 2; // field lines 
         }
         update_display = TRUE;*/
    } else {
         //old_draw_page (&page);
         //update_display = TRUE;
    }
}

void CCView::onCC(vbi_event&ev){
    //if(ev.ev._cc608->channel!=pgno)return;
    //TODO
}

static int get_first_visible_row( vbi_char *p_text, int rows, int columns){
    for ( int i = 0; i < rows * columns; i++ ){
        if ( p_text[i].opacity != VBI_TRANSPARENT_SPACE )
            return i / columns;
    }
    return -1;
}

static int get_last_visible_row( vbi_char *p_text, int rows, int columns){
    for ( int i = rows * columns - 1; i >= 0; i-- ) {
        if (p_text[i].opacity != VBI_TRANSPARENT_SPACE)
            return i / columns;
    }
    return -1;
}
void CCView::onDraw(GraphContext&canvas){
    int i_first_row = get_first_visible_row( page.text,page.rows,page.columns );
    int i_num_rows=0;
    if ( i_first_row < 0 ) {
        i_first_row =page.rows - 1;
        i_num_rows = 0;
    } else {
        i_num_rows = get_last_visible_row(page.text,page.rows,page.columns ) - i_first_row + 1;
    }
    View::onDraw(canvas);
#if 0 
    char text[1024]={0};
    i_first_row=0;i_num_rows=15;
    vbi_print_page_region( &page,text,sizeof(text),"UTF-8", 0, 0, 0, i_first_row,page.columns,i_num_rows);
    NGLOG_DEBUG("pagesize=%dx%d first=%d num_rows=%d %s",page.columns,page.rows,i_first_row,i_num_rows,text);
#else
    i_first_row=0;i_num_rows=15;
    vbi_draw_vt_page_region(&page,VBI_PIXFMT_RGBA32_LE,imagesurface->get_data(), -1,
                          0, i_first_row,page.columns, i_num_rows, 1, 1);
    NGLOG_DEBUG("i_first_row=%d columns=%d i_num_rows=%d page.size=%dx%d",i_first_row,page.columns,i_num_rows,page.columns,page.rows);
#endif
}

}//namespace

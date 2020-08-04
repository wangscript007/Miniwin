#include<channelepgview.h>
#include<ngl_timer.h>
#include <ngl_log.h>

NGL_MODULE(CHANNELEPG);
namespace nglui{

ChannelBar::ChannelBar(const std::string& txt):ListView::ListItem(txt){
     index=-1;
}
void ChannelBar::addEvent(const TVEvent&evt){
     events.push_back(evt);
}

void ChannelBar::addEvent(const std::string name,ULONG starttm,ULONG dur){
     TVEvent e;
     e.name=name;
     e.start=starttm;
     e.duration=dur;
     addEvent(e);
}

void ChannelBar::clearEvents(){
     events.clear();
}

int ChannelBar::selectEvent(LONG etm){
    int idx=0;
    for(auto e:events){
        if((e.start<etm)&&((e.start+e.duration)>etm))
            return idx;
        idx++;
    }
    return -1;
}

void ChannelEpgView::DefaultPainter(AbsListView&lv,const AbsListView::ListItem&itm,int state,GraphContext&canvas){
     int idx=0;
     float pixelmin =((ChannelEpgView&)lv).getPixelPerMinute();
     int nameWidth=((ChannelEpgView&)lv).getChannelNameWidth();
     ChannelBar&bar=(ChannelBar&)itm;
     ULONG starttime=((ChannelEpgView&)lv).getStartTime();
     ChannelEpgView&chv=(ChannelEpgView&)lv;
     canvas.set_font_size(20);
     time_t now=time(NULL);
     int lastisnow=0;
     if(state){
         canvas.set_color(chv.getColor(CHANNEL_FOCUSED));
         canvas.rectangle(bar.rect);
         canvas.fill();
     }
     canvas.save();
     RECT chr=bar.rect;chr.x+=nameWidth;
     canvas.rectangle(chr);
     canvas.clip();
     for(auto e:bar.events){
         RECT r=bar.rect;
         r.x=nameWidth+(pixelmin*(e.start-starttime)/60);
         r.width=e.duration*pixelmin/60;
         int color=chv.getColor(EVENT_FEATURE);
         if(idx++==bar.index){
             color=chv.getColor(EVENT_FOCUSED);
         }else  if(e.start<now)
             color=chv.getColor(EVENT_POST);
         else if(e.start<now&&e.start+e.duration>now){
             color=chv.getColor(EVENT_NOW);
             lastisnow=1;
         }else if(e.start>now){
             color=chv.getColor(lastisnow?EVENT_NEXT:EVENT_FEATURE);
             lastisnow=0;
         }
         if(e.start+e.duration<starttime)continue;
         if(r.right()<=bar.rect.x)continue;
         if(r.x>bar.rect.right())break;
         canvas.set_color(color);
         canvas.rectangle(r);
         canvas.fill();
         canvas.set_color(chv.getColor(BORDER));
         canvas.rectangle(r);
         canvas.stroke();
         canvas.set_color(lv.getFgColor());
         canvas.draw_text(r,e.name);
         struct tm ts;
         gmtime_r((time_t*)&e.start,&ts);
         NGLOG_VERBOSE("%5d time:%d/%d %02d:%02d-%3d  %d-%d %s",e.id,ts.tm_mon+1,ts.tm_mday,ts.tm_hour,ts.tm_min,e.duration/60,r.x,r.width,e.name.c_str()); 
     }
     canvas.restore();
     RECT rb=bar.rect;
     rb.width=nameWidth-1;
     rb.height-=1;
     canvas.set_color(chv.getColor(CHANNEL_FG)); 
     canvas.draw_text(rb,bar.getText());
     canvas.move_to(0,bar.rect.y+bar.rect.height-1);
     canvas.line_to(lv.getWidth(),bar.rect.y+bar.rect.height-1);
     canvas.stroke();
}


////////////////////////////////////////////////////////////////////////////////////////////////

ChannelEpgView::ChannelEpgView(int w,int h):ListView(w,h){
     pixelMinute=1.0;
     nglGetTime((NGL_TIME*)&starttime);
     setStartTime(starttime);
     nameWidth=120;
     setBgColor(0xFF101010);
     item_painter_=ChannelEpgView::DefaultPainter;
     onEventSelect=nullptr;
     onTimeChange=nullptr;
     colors[CHANNEL_FG] =0xFFFFFFFF;
     colors[CHANNEL_FOCUSED]=0x80FF0000;
     colors[BORDER]=0xFF222222;
     colors[EVENT_POST] =0xFF333333;
     colors[EVENT_NOW ] =0xFF444444;
     colors[EVENT_NEXT] =0xFF666666;
     colors[EVENT_FEATURE]=0xFF888888;
     colors[EVENT_FOCUSED]=0x8000FF00;
}

void ChannelEpgView::setColor(int idx,UINT color){
    colors[idx]=color;
}

UINT ChannelEpgView::getColor(int idx){
   return colors[idx];
}

void ChannelEpgView::setStartTime(LONG t){
     if(starttime!=t-(t%3600)){
         starttime=t-(t%3600);
         if(onTimeChange)onTimeChange(*this);
         invalidate(nullptr);
     }
}

LONG ChannelEpgView::getStartTime(){
     return starttime;
}

void ChannelEpgView::setPixelPerMinute(float p){
     pixelMinute=p;
}

void ChannelEpgView::setChannelNameWidth(int w){
     nameWidth=w;
}

int ChannelEpgView::getChannelNameWidth(){
     return nameWidth;
}

float ChannelEpgView::getPixelPerMinute(){
     return pixelMinute;
}

void ChannelEpgView::setIndex(int idx){
    ChannelBar*ch=(ChannelBar*)getItem(index_);
    ChannelBar*nch=(ChannelBar*)getItem(idx);
    INHERITED::setIndex(idx);
    if(ch&&ch->index>=0&&ch->index<ch->events.size()&&nch&&nch->index<0){
         TVEvent&e=ch->events[ch->index];
         nch->selectEvent(e.start);
    }
}

LONG ChannelEpgView::getEndTime(){
    return starttime+(getWidth()-nameWidth)*60/pixelMinute;
}

bool ChannelEpgView::onKeyUp(KeyEvent&key){
    LONG tt=getStartTime();
    ChannelBar*ch=(ChannelBar*)getItem(index_);
    NGLOG_DEBUG("ch=%p events=%d",ch,(ch?ch->events.size():0));

    switch(key.getKeyCode()){
    case KEY_LEFT: 
         if(ch&&ch->events.size()){
             ch->index=(ch->index+ch->events.size()-1)%ch->events.size();
             TVEvent&e=ch->events[ch->index];
             if(onEventSelect)onEventSelect(*this,e);
             tt=e.start-e.start%3600;
             NGLOG_DEBUG_IF(e.start<this->starttime,"setStartTime(%ld)%s",tt,ctime(&tt)); 
             if(e.start<this->starttime)
                setStartTime(e.start);
             invalidate(nullptr);
             return true;
         }break;
    case KEY_RIGHT:
         if(ch&&ch->events.size()){
             ch->index=(ch->index+1)%ch->events.size();
             TVEvent&e=ch->events[ch->index];
             if(onEventSelect)onEventSelect(*this,e);
             tt=this->starttime+(e.duration+3600);tt-=tt%3600; 
             NGLOG_DEBUG_IF(e.start+e.duration>=this->getEndTime(),"setStartTime(%ld)%s",tt,ctime(&tt));
             if(e.start+e.duration>=this->getEndTime())
                setStartTime(this->starttime+(e.duration+3600));
             invalidate(nullptr);
             return true; 
         }break;
    default:return ListView::onKeyUp(key);
    }
    return false;
}

void ChannelEpgView::onDraw(GraphContext&canvas){
    ListView::onDraw(canvas);

    RECT r=getClientRect();
    canvas.set_font_size(getFontSize());
    int item_height,idx=top_;
    getItemSize(nullptr,&item_height);
    r.inflate(-2,0);
    r.height=item_height;
    for(auto itm=list_.begin()+idx;itm<list_.end();itm++,idx++){
        (*itm)->rect=r;
        item_painter_(*this,*(*itm),idx==index_,canvas);
        r.offset(0,item_height);
        if(r.y>=getHeight())break;
    }
    SCROLLINFO sb;
    sb.fMask=SIF_PAGE|SIF_RANGE|SIF_POS;
    sb.nPage=idx-top_;
    sb.nMin=0;
    sb.nMax=list_.size();
    sb.nPos=top_;
}

}//end namespace

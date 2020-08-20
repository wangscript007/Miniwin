#include <view.h>
#include <ngl_log.h>
#include <string.h>
#include <algorithm>

NGL_MODULE(View);

namespace nglui{
enum{
   PFLAG_PRESSED=1,
};
View::View(int w,int h)
  : mId(0),  mParent(nullptr),
    mFgColor(0xFFFFFFFF),
    mScrollX(0),mScrollY(0),
    mViewFlags(VISIBLE),
    mFontSize(20){
    mLayout.reset(nullptr);
    mBound.set(0,0,w,h);
    mPreferSize.set(w,h);
    mInvalid=Region::create();
    invalidate(nullptr);
    onClick_=nullptr;
    onMessage_=nullptr;
    memset(&mScrollInfos,0,sizeof(SCROLLINFO)*2);
    setBgColor(0xFF000000);
}

View::~View(){
    for(auto c:mChildren)
       delete c;
    mChildren.clear();
}

View*View::findViewById(int id){
    for(auto c=mChildren.begin();c!=mChildren.end();c++){
         if((*c)->mChildren.size()){
            auto cc=(*c)->findViewById(id);
            if(cc)return cc;
         }
         if((*c)->mId==id)
            return (*c);
    }
    return nullptr;
}

bool View::pointInView(int x,int y)const{
    return((x>=mBound.x)&&(x<mBound.x+mBound.width)&&(y>=mBound.y)&&(y<mBound.y+mBound.height));
}

void View::setScrollInfo(int bar,const SCROLLINFO*info,bool redraw){
    if(info==nullptr||bar<0||bar>1)return;
    if(info->fMask&SIF_RANGE){
        mScrollInfos[bar].nMin=info->nMin;
        mScrollInfos[bar].nMax=info->nMax;
    }if(info->fMask&SIF_PAGE){
        mScrollInfos[bar].nPage=info->nPage;
    }if(info->fMask&SIF_POS){
        mScrollInfos[bar].nPos=info->nPos;
    }if(info->fMask&SIF_TRACKPOS){
        mScrollInfos[bar].nTrackPos=info->nTrackPos;
    }
}

void View::scrollTo(int x,int y){
    if( (mScrollX!=x)|| (mScrollY!=y) ){
       mScrollX=x;
       mScrollY=y;
       invalidate(nullptr);
    }
}

void View::scrollBy(int dx,int dy){
    mScrollX+=dx;
    mScrollY+=dy;
    if(dx||dy)invalidate(nullptr);
}

int View::getScrollX()const{
    return mScrollX;
}

int View::getScrollY()const{
    return mScrollY;
}

void View::setScrollX(int x){
    scrollTo(x,mScrollY);    
}

void View::setScrollY(int y){
    scrollTo(mScrollX,y);
}

void View::drawScrollBar(GraphContext&canvas,int bar){
    RECT rc=getClientRect();
    SCROLLINFO sb=mScrollInfos[bar];
    rc.inflate(-1,-1);
    rc.offset(mScrollX,mScrollY);
    if(sb.nMax<sb.nPage||sb.nMax<=sb.nMin)
        return;
    int barsize=(bar==SB_VERT)?rc.height:rc.width;
    int barpos=barsize*(sb.nPos)/(sb.nMax-sb.nMin);
    barsize=barsize*sb.nPage/(sb.nMax-sb.nMin);

    NGLOG_VERBOSE("BARINFO range=%d,%d pos=%d page=%d",sb.nMin,sb.nMax,sb.nPos,sb.nPage);
    NGLOG_VERBOSE("barsize=%d pos=%d view.size=%d,%d ",barsize,barpos,rc.width,rc.height);
    if(bar==SB_VERT){
        rc.set(rc.width-8,rc.y,8,rc.height);
        RECT rb=rc;rb.inflate(-3,0);
        canvas.set_color(0xFF222222);
        canvas.rectangle(rb);
        canvas.fill();
        canvas.set_color(0xFF888888);
        rc.height=barsize;
        rc.y=barpos+mScrollY;
        canvas.rectangle(rc);
        canvas.fill();
    }else{
        rc.set(rc.x,rc.height-10,rc.width,10);
        canvas.set_color(0xFF888888);
        canvas.rectangle(rc);
        canvas.fill();
        canvas.set_color(0xFFFF0000);
        rc.width=barsize;
        rc.x=barpos+mScrollX;
        canvas.rectangle(rc);
        canvas.fill();
    }
}

void View::setClickListener(ClickListener ls){
    onClick_=ls;
}

void View::setMessageListener(MessageListener ls){
    onMessage_=ls;
}

void View::clip(GraphContext&canvas){
    canvas.reset_clip();
    if(hasFlag(CLIPCHILDREN)){//clip children
        for(auto c:mChildren){
            RECT rc=c->getBound();
            if(c->isVisible()==false)continue;
            mInvalid->subtract((RectangleInt&)rc);;
        }
    }
    if(mParent){//clip sliblings
        BOOL afterthis=FALSE;
        RECT rect=getBound();
        for(auto s:mParent->mChildren){
            RECT rc,r=s->getBound();
            if( afterthis==FALSE ){
                afterthis=(s==this);
                continue;
            }
            if(s->hasFlag(VISIBLE)==false)continue;
            if(false==rc.intersect(rect,r))continue;
            rc.offset(-rect.x,-rect.y);
            mInvalid->subtract((RectangleInt&)rc);
        }
        rect=mParent->getClientRect();
        mInvalid->intersect((const RectangleInt&)rect);//clip the area outof parent's bounds
    }

    int num=mInvalid->get_num_rectangles();
    for(int i=0;i<num;i++){
        RectangleInt r=mInvalid->get_rectangle(i);
        canvas.rectangle(r.x,r.y,r.width,r.height);
    }
    canvas.clip();
}

void View::resetClip(){
    mInvalid->subtract(mInvalid);
}

GraphContext*View::getCanvas(){
    View*p=this;
    POINT pt={0,0};
    while(p->mParent){
        pt.x+=p->getX();
        pt.y+=p->getY();
        p=p->mParent;
    }
    GraphContext*canvas=p->getCanvas();
    if(canvas==nullptr)return nullptr;
    GraphContext*c=canvas->subContext(pt.x,pt.y,getWidth(),getHeight());
    c->set_font_face(canvas->get_font_face());
    return c;
}

void View::draw(){
     RefPtr<GraphContext>canvas(getCanvas());
     if((canvas==nullptr)||(hasFlag(VISIBLE)==false))return;
     canvas->set_antialias(ANTIALIAS_GRAY);
     canvas->set_font_size(getFontSize());
     canvas->set_source(mBgPattern);
     canvas->reset_clip();
     clip(*canvas);
     canvas->translate(-mScrollX,-mScrollY);
     if( (false==mInvalid->empty()) && ( (vis_rgn==nullptr) 
            || (vis_rgn->contains_rectangle(mInvalid->get_extents())!=Region::Overlap::OUT) )){
         onDraw(*canvas);
         drawChildren();
         canvas->invalidate(mInvalid);
     }
     resetClip();
}

void View::drawChildren(){
    if(hasFlag(CLIPCHILDREN))return;
    for(auto child : mChildren){
        RECT rc=child->getBound();
        if(mInvalid->contains_rectangle((RectangleInt&)rc)==Region::Overlap::OUT)continue;
        RefPtr<Region>inv=mInvalid->copy();
        inv->intersect((const RectangleInt&)rc);//child->getBound());
        inv->translate(-child->getX(),-child->getY());
        child->mInvalid->do_union(inv);
        child->draw();
    }
}

void View::onDraw(GraphContext&canvas){
    RECT rc=getClientRect();
    canvas.save();
    canvas.set_source(mBgPattern);
    canvas.rectangle(rc.x,rc.y,rc.width,rc.height);
    canvas.fill();
    canvas.restore();
    if(hasFlag(BORDER)){
         canvas.set_color(255,0,0);
         canvas.rectangle(0,0,getWidth(),getHeight());
         canvas.stroke();
    }
#ifdef __DEBUG/*for invalidate debug*/
    int num=mInvalid->get_num_rectangles();
    canvas.set_color(0xFF00FF00);
    for(int i=0;i<num;i++){
        RectangleInt r=mInvalid->get_rectangle(i);
        canvas.rectangle(r.x,r.y,r.width,r.height);
    }
    if(num)canvas.stroke();
#endif

}

const RECT View::getBound()const{
    return mBound;
}

void View::getFocusRect(RECT&r)const{
    r=mBound;
}

View& View::setId(int id){
    mId=id;
    return *this;
}

int View::getId() const{
    return mId;
}

int View::getWidth()const{
    return mBound.width;
}

int View::getHeight()const{
    return mBound.height;
}

int View::getX()const{
    return mBound.x;
}

int View::getY()const{
    return mBound.y;
}

View& View::setText(const std::string&txt){
    mText=txt;
    invalidate(nullptr);
    return *this;
}

const std::string& View::getText()const{
    return mText;
}

View& View::setBound(const RECT&b){
    if(mParent){
       mParent->invalidate(&mBound);
       mParent->invalidate(&b);
    }
    mBound=b;
    invalidate(nullptr);
    return *this;
}

View& View::setBound(int x,int y,int w,int h){
    RECT rect;
    rect.set(x,y,w,h);
    setBound(rect);
    return *this;
}

View& View::setPos(int x,int y){
    setBound(x,y,mBound.width,mBound.height);
    return *this;
}

View& View::setSize(int w,int h){
    setBound(mBound.x,mBound.y,w,h);
    return *this;
}

const RECT View::getClientRect()const{
    RECT r=mBound;
    r.x=r.y=0;
    return r;
}

View& View::setBgPattern(const RefPtr<const Pattern>& source){
    mBgPattern=source; 
    return *this;
}

RefPtr<const Pattern>View::getBgPattern(){
    return mBgPattern;
}

View& View::setFgColor(UINT color){
    mFgColor=color;
    invalidate(nullptr);
    return *this;
}

UINT View::getFgColor() const{
    return mFgColor;
}

View& View::setBgColor(UINT color){
    mBgPattern=SolidPattern::create_rgba(((color>>16)&0xFF)/255.,((color>>8)&0xFF)/255.,(color&0xFF)/255.,(color>>24)/255.);
    invalidate(nullptr);
    return *this;
}

UINT View::getBgColor() const{
#define F2D(f) ((BYTE)(255*(f)))
    if(mBgPattern->get_type()==Pattern::Type::SOLID){
        double r,g,b,a;
        cairo_pattern_get_rgba((cairo_pattern_t*)mBgPattern->cobj(),&r,&g,&b,&a);
        return F2D(a)<<24|F2D(r)<<16|F2D(g)<<8|F2D(b);
    }
    return 0;
}

const SIZE&View::getPreferSize(){
    mPreferSize.set(getWidth(),getHeight());
    return mPreferSize;
}

void View::onFocusChanged(bool){
}

View& View::setFlag(int flag) {
    mViewFlags |= flag;
    if((INT)flag&FOCUSED)
       onFocusChanged(TRUE);
    invalidate(nullptr);
    return *this;
}

View& View::clearFlag(int flag) {
    mViewFlags&=(~flag);
    if(flag&FOCUSED)
       onFocusChanged(FALSE);
    invalidate(nullptr);
    return *this;
}

bool View::hasFlag(int flag) const {
    return (mViewFlags&flag)==flag;
}

bool View::isFocused()const {
   return hasFlag(FOCUSED);
}

View& View::setVisible(bool visable) {
   if(visable)
        setFlag(VISIBLE);
   else 
        clearFlag(VISIBLE);
   return *this;
}

bool View::isVisible() const {
   return hasFlag(VISIBLE);
}

View& View::setEnable(bool enable) {
    if(enable)
        setFlag(ENABLED);
    else 
        clearFlag(ENABLED);
    return *this;
}

bool View::isEnable() const {
    return hasFlag(ENABLED);
}

int View::getFontSize() const{
    return mFontSize;
}

View& View::setFontSize(int sz){
    mFontSize=sz;
    invalidate(nullptr);
    return *this;
}

void View::invalidate(const RECT*rect){
    RECT rc=getClientRect();
    if(rect){
        rc.intersect(*rect);
        if(rc.empty())return;
    }
    if(mParent&&(mParent->hasFlag(CLIPCHILDREN)==false)){
        rc.offset(getX(),getY());
        rc.intersect(mParent->getClientRect());
        if(!rc.empty()){
            mParent->mInvalid->do_union((RectangleInt&)rc);
            mParent->sendMessage(WM_INVALIDATE,0,0);
        }
    }else{
        mInvalid->do_union((RectangleInt&)rc);
        sendMessage(WM_INVALIDATE,0,0);
    }
}

void View::invalidate(const RefPtr<Region>crgn){
    if(mParent&&(mParent->hasFlag(CLIPCHILDREN)==false)){
        RefPtr<Region>rgn=crgn->copy();
        RECT rc=mParent->getClientRect();
        rgn->translate(getX(),getY());
        rgn->intersect((const RectangleInt&)rc);
        if(!rgn->empty()){
            mParent->mInvalid->do_union(rgn);
            mParent->sendMessage(WM_INVALIDATE,0,0);
        }
    }else{
        mInvalid->do_union(crgn);
        sendMessage(WM_INVALIDATE,0,0);
    }
}
// Layout
Layout* View::setLayout(Layout* layout){
    mLayout.reset(layout);
    if(mChildren.size())
        onLayout();
    return layout;
}


Layout* View::getLayout() const{
     return mLayout.get();
}

void View::onLayout(){
    if (isVisible() && (mLayout.get() != nullptr)) {
        mLayout->onLayout(this);
    }
}

View*View::getParent(){
    return mParent;
}

View& View::setParent(View*p){
    mParent=p;
    onAttached();
    return *this;
}

int View::getViewOrder(View*v){
    int idx=0;
    for(auto c:mChildren){
        if(c==v)return idx;
        idx++;
    }
    return -1;
}

View* View::getChildView(size_t idx){
    if (idx >= mChildren.size()) {
        return nullptr;
    }
    return mChildren[idx];
}

View* View::addChildView(View* view){
    if(nullptr==view)
         return nullptr;
    view->setParent(this);
    mChildren.push_back(view);
    onLayout();
    view->invalidate(nullptr);
    return view;
}


void View::removeChildView(View* view){
    auto itr=std::find(mChildren.begin(),mChildren.end(),view);
    if(itr!=mChildren.end()){
        mChildren.erase(itr);
        onLayout();
    }
}

void View::removeChildren(){
    mChildren.clear();
    onLayout();
}
void View::removeChildView(size_t idx){
    View* pView = getChildView(idx);
    removeChildView(pView);     
}

size_t View::getChildrenCount() const{
    return mChildren.size();
}

void View::onMeasure(int widthMeasureSpec, int heightMeasureSpec){
}

void View::onResize(UINT cx,UINT cy){
}

bool View::onKeyDown(KeyEvent& evt){
    return false;
}

bool View::onKeyUp(KeyEvent& evt){
    if(evt.getKeyCode()==KEY_ENTER && onClick_){
        onClick_(*this);
        return true;
    }
    return false;
}

bool View::dispatchTouchEvent(MotionEvent&event){
    return onTouchEvent(event);
}

bool View::onInterceptTouchEvent(MotionEvent&event){
    return false;
}

bool View::onTouchEvent(MotionEvent& mt){
    switch(mt.getAction()){
    case MotionEvent::ACTION_UP:
        if(mPrivateFlags&PFLAG_PRESSED);
        break; 
    case MotionEvent::ACTION_DOWN:
        if(onClick_&&mt.getActionButton()==MotionEvent::BUTTON_PRIMARY)
            onClick_(*this);
        break;
    case MotionEvent::ACTION_MOVE:
        break;
    case MotionEvent::ACTION_CANCEL:break;
    }
    return false;
}

void View::sendMessage(View*v,DWORD msgid,DWORD wParam,ULONG lParam,DWORD delayedtime){
    if(mParent){
        mParent->sendMessage(v,msgid,wParam,lParam,delayedtime);
    }
}

void View::sendMessage(DWORD msgid,DWORD wParam,ULONG lParam,DWORD delayedtime){
    sendMessage(this,msgid,wParam,lParam,delayedtime);
}

bool View::onMessage(DWORD msgid,DWORD wParam,ULONG lParam){
    RECT r;
    switch(msgid){
    default:
         if(onMessage_)return onMessage_(*this,msgid,wParam,lParam);
         return false;   
    }
}

}//endof namespace

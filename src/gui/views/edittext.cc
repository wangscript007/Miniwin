#include <edittext.h>
#include <keyboard.h>
#include <ngl_log.h>
#include <regex>
#include <math.h>

NGL_MODULE(EditText);

namespace nglui{

EditText::EditText(int w,int h):EditText(std::string(),w,h){
}

EditText::EditText(const std::string&txt,int w,int h):Widget(txt,w,h){
    setFlag(FOCUSABLE|ENABLED);
    mTextAlignment=DT_LEFT|DT_VCENTER;
    mLabelAlignment=DT_LEFT|DT_VCENTER;
    mEditMode=0;
    mLabelWidth=0;
    mCaretPos=0;
    mCaretOn=false;
    mBlinkOn=true;
    mLabelBkColor=0;
    afterChanged=nullptr;
    mCaretRect.set(0,0,0,0);
}

void EditText::setTextWatcher(AfterTextChanged ls){
    afterChanged=ls;
}

void EditText::setCaretPos(int idx){
    mCaretPos=idx; 
    invalidate(nullptr);
}

void EditText::setPattern(const std::string&pattern){
    mInputPattern=pattern;
}

bool EditText::match(){
    if(mInputPattern.empty())
       return true;
    try{
        std::regex reg(mInputPattern);
        return std::regex_match(mText,reg);
    }catch(std::exception &ex){
        NGLOG_ERROR("Error:%s",ex.what());
        return false;
    }
}

const std::string& EditText::replace(size_t start,size_t len,const std::string&txt){
    mText.replace(start,len,txt);
    invalidate(nullptr);
    return mText;
}

const std::string& EditText::replace(size_t start,size_t len,const char*txt,size_t size){
    mText.replace(start,len,txt,size);
    invalidate(nullptr);
    return mText;
}

void EditText::setEditMode(EDITMODE mode){
    mEditMode=mode;
}

int EditText::getCaretPos(){
    return mCaretPos;
}

void EditText::setLabelColor(int c){
    mLabelBkColor=c;
    invalidate(nullptr);
}

void EditText::setLabelWidth(int w){
    mLabelWidth=w;
}

void EditText::setLabel(const std::string&txt){
    mLabel=txt;
    invalidate(nullptr);
}

void EditText::setLabelAlignment(int align){
    mLabelAlignment=align;
    invalidate(nullptr);
}

const std::string&EditText::getLabel(){
    return mLabel;
}

void EditText::setHint(const std::string&txt){
    mHint=txt;
    if(mText.empty())invalidate(nullptr);
}

const std::string& EditText::getHint(){
    return mHint;
}

int EditText::Unicode2UTF8(UINT uni,std::string&str){
   if(uni<=0x7F){
       str+=uni;
       return 1;
   }else if(uni>=0x80&&uni<=0x7FF){
       str+=0xC0|((uni>6)&0x1F);
       str+=0x80|(uni&0x3F);
       return 2; 
   }else if(uni>=0x800&&uni<=0xFFFF){
       str+=0xE0|(uni>>12);
       str+=0x80|((uni>>6)&0x3F);
       str+=0x80|(uni&0x3F);
       return 3;
   }else if(uni>=0x10000){
       str+=0xF0|((uni>>18)&0x07);
       str+=0x80|((uni>>12)&0x3F);
       str+=0x80|((uni>>6)&0x3F);
       str+=0x80|(uni&0x3F);
       return 4;
   }
   return 0;
}

void EditText::setCaretBlink(bool blink){
   mBlinkOn=blink;
   if(mBlinkOn && hasFlag(FOCUSED))
       sendMessage(WM_TIMER,0,0,1000);
   else{
       mCaretOn=true;
       invalidate(&mCaretRect);
   }
}

void EditText::onFocusChanged(bool focus){
   if( focus && mBlinkOn )sendMessage(WM_TIMER,0,0,1000);
}

bool EditText::onMessage(DWORD msg,DWORD wp,ULONG lp){
    switch(msg){
    case WM_CHAR:{
        if ( !isEnable() )
            return Widget::onMessage(msg,wp,lp);
        std::string txt;
        Unicode2UTF8(wp,txt);
        NGLOG_VERBOSE("mCaretPos=%d text:[%s]",mCaretPos,txt.c_str());
        if(mEditMode==0||mCaretPos>=mText.size()){
            mText.insert(mCaretPos,txt);
            mCaretPos++;
        }else{
            mText.replace(mCaretPos,1,txt);
        }
        if(nullptr!=afterChanged)
            afterChanged(*this);
        invalidate(nullptr);
        return true;
      }
   case WM_TIMER:
      if(hasFlag(FOCUSED)){
          mCaretOn=!mCaretOn;
          invalidate(&mCaretRect);
          if(mBlinkOn)sendMessage(WM_TIMER,0,0,1000);
      }
      break;
   default:return Widget::onMessage(msg,wp,lp);
   }
}

bool EditText::onKeyUp(KeyEvent&evt){
    char ch;
    bool ret=false;
    int changed=0;
    std::string text_old=mText;
    INT caretpos_old=mCaretPos;
    switch(evt.getKeyCode()){
    case KEY_LEFT:
        if(mCaretPos>0){
            setCaretPos(mCaretPos-1);
            return true;
        }break;
    case KEY_RIGHT:
        if(mCaretPos<mText.size()){
            setCaretPos(mCaretPos+1);
            return true;
        }break;
    case KEY_DOWN:
        if(mCaretPos<mText.size()){
           ch=mText[mCaretPos];
           if(ch>='0'&&ch<='9'){
               if(ch>'0')ch--;
               else if(ch=='0')ch='9';
               mText[mCaretPos]=ch;
               changed++;
               ret=true;
           }
        }break;
    case KEY_UP:
        if(mCaretPos<mText.size()){
           ch=mText[mCaretPos];
           if(ch>='0'&&ch<='9'){
               if(ch<'9')ch++;
               else if(ch=='9')ch='0';
               mText[mCaretPos]=ch;
               changed++;
               ret=true;
           }
        }break;
    case KEY_BACKSPACE:
        if(mCaretPos>0){
             mText.erase(--mCaretPos,1);
             changed++;
             ret=true;
        }break;
    case KEY_DELETE:
        if(mCaretPos<mText.size()-1){
            mText.erase(mCaretPos,1);
            changed++;
            ret=true; 
        }break;
    case KEY_INSERT:
         mEditMode=!mEditMode;
         invalidate(nullptr);
         break;
    case KEY_ENTER:
    case KEY_OK:
         if(nullptr!=afterChanged)
             afterChanged(*this);
         return false;
    case KEY_SPACE:sendMessage(WM_CHAR,' ',0);return true;
    case KEY_0:sendMessage(WM_CHAR,'0',0);return true;
    case KEY_1...KEY_9:
        sendMessage(WM_CHAR,'1'+(evt.getKeyCode()- KEY_1),0);
        return true;
    case KEY_MENU:
        if(isEnable()){
            Keyboard *kb=new Keyboard(320,400,640,240);
            kb->setBuddy(this);
        }return true;
    default:break; 
    }
    if(changed){
        if(!match()){
             mText=text_old;
             mCaretPos=caretpos_old;
             return false;
        }  
        invalidate(nullptr);
        if(nullptr!=afterChanged)
            afterChanged(*this);
    }
    return ret;
}

void EditText::onDrawCaret(GraphContext&canvas,const RECT&r,EDITMODE editmode){
    if(INSERT==editmode){
        canvas.move_to(r.x,r.y);
        canvas.line_to(r.x,r.bottom());
        canvas.stroke();
    }else{
        canvas.rectangle(r);
	canvas.fill();
    }
}

void EditText::onDraw(GraphContext&canvas){
    Widget::onDraw(canvas);
    RECT r=getClientRect();
    
    canvas.set_font_size(getFontSize());
    if(mLabelWidth>0){
        r.width=mLabelWidth;
        if(!hasFlag(FOCUSED)){
            canvas.set_color(mLabelBkColor);
            canvas.rectangle(r);
            canvas.fill();
        }
        r.inflate(-4,0);
        canvas.set_color(mFgColor);
        canvas.draw_text(r,mLabel,mLabelAlignment);
        r.x=mLabelWidth;
        r.width=getWidth()-mLabelWidth;
    }
    // Text
    canvas.set_color(mText.empty()?0xFF555555:mFgColor);
    std::string txt=mText.empty()?mHint:mText;
    canvas.draw_text(r,txt,mTextAlignment);
    int caretx=mLabelWidth;
    int caretw=getFontSize()/3;

    if(mCaretPos>=0 && mText.length()>0){
        TextExtents te,te1;
        std::string ss =mText.substr(0,mCaretPos);
        std::string ss1=mText.substr(0,mCaretPos+1);
        canvas.get_text_extents(ss,te);
        canvas.get_text_extents(ss1,te1);
        caretx=mLabelWidth+te.x_advance+te.x_bearing;
        caretw=te1.x_advance-te.x_advance+1;
    }
    mCaretRect.set(caretx,r.y,caretw,r.height);
    if(mCaretOn){
        UINT caret_color=(0xFFFFFF&(~mFgColor))|0x80000000;
        canvas.set_color(caret_color);
        onDrawCaret(canvas,mCaretRect,(EDITMODE)mEditMode);
    }
}
}



#ifndef __NGLUI_EDITVIEW_H__
#define __NGLUI_EDITVIEW_H__
#include <widget.h>

namespace nglui{

class EditText:public Widget{
public:
  DECLARE_UIEVENT(void,AfterTextChanged,EditText&);
protected:
  int mEditMode;//0-->insert mode 1-->replace_mode
  int mCaretPos;
  int mLabelWidth;
  int mLabelAlignment;
  bool mCaretOn;/*for caret blink flag:false caret hide,true caret is shown*/
  bool mBlinkOn;
  RECT mCaretRect;
  std::string mLabel;
  std::string mInputPattern;
  std::string mHint;
  int mLabelBkColor;
  AfterTextChanged afterChanged;
  bool match();
  virtual void onFocusChanged(bool)override;
public:
  typedef enum{
    INSERT=0,
    REPLACE=1
  }EDITMODE;

  EditText(int w,int h);
  EditText(const std::string&txt,int w,int h);
  static int Unicode2UTF8(UINT unicode,std::string&utf8);
  void setLabelColor(int color);
  const std::string&replace(size_t start,size_t len,const std::string&txt);
  const std::string&replace(size_t start,size_t len,const char*txt,size_t size);
  void setPattern(const std::string&pattern);
  virtual void setHint(const std::string&txt);
  const std::string& getHint();
  virtual void setTextWatcher(AfterTextChanged ls);
  virtual void setLabelWidth(int w);
  virtual void setLabel(const std::string&txt);
  virtual void setLabelAlignment(int align);
  virtual const std::string&getLabel();
  virtual void setEditMode(EDITMODE mode);
  void setCaretBlink(bool blink=true);
  virtual void onDraw(GraphContext&ctx)override;
  virtual void onDrawCaret(GraphContext&ctx,const RECT&rc,EDITMODE editmode);
  virtual void setCaretPos(int idx);
  virtual int getCaretPos();
  virtual bool onKeyUp(KeyEvent&evt)override; 
  virtual bool onMessage(DWORD msg,DWORD wp,ULONG lp);
};

}//endof nglui

#endif


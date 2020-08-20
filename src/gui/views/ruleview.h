#ifndef __RULE_VIEW_H__
#define __RULE_VIEW_H__
#include <view.h>

namespace nglui{

class RuleView:public View{
public:
   DECLARE_UIEVENT(void,GetLabeListener,RuleView&,int idx,std::string&label);
protected:
   int startValue;
   int endValue;
   int interval;
   int offset;
   GetLabeListener onGetLabel;
public:
   RuleView(int w,int h);
   RuleView& setMin(int);
   RuleView& setMax(int);
   RuleView& setInterval(int);
   RuleView& setOffset(int);
   void setLabeListener(GetLabeListener listener);
   int getMin()const{return startValue;}
   int getMax()const{return endValue;}
   int getInterval()const{return interval;}
   int getOffset()const{return offset;}
   void onDraw(GraphContext&)override;
};
}
#endif

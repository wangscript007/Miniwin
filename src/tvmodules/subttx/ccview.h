#ifndef __CLOSECAPTION_VIEW_H__
#define __CLOSECAPTION_VIEW_H__
#include <vbiview.h>

namespace nglui{


class CCView:public VBIView{
private:
   void onCaption(vbi_event&ev);
   void onCC(vbi_event&ev);
public:
   CCView(int w,int h);
   void onPageReceived(vbi_event&ev); 
   void onDraw(GraphContext&canvas);
   void requestPage(int page,int sub=VBI_ANY_SUBNO)override; 
};

}//namespace

#endif

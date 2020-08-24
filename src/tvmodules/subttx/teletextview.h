#ifndef __TELETEXT_VIEW_H__
#define __TELETEXT_VIEW_H__
#include <vbiview.h>
#include <window.h>
#include <ngl_types.h>
#include <ngl_dmx.h>
#include <ngl_log.h>
#include <ngl_os.h>

namespace nglui{

typedef struct {
   vbi_link link;
   int col;
   int row;
   int width;
   int height;
   int color;
} vbi_link_ext;

struct ttx_patch {
    UINT  column;
    UINT  row;
    INT   width, height; /* geometry of the patch */
    INT   sx, sy;
    INT   sw, sh;
    INT   dx, dy;
    UINT  columns;	/* text columns covered */
    INT   phase;	/* flash phase */
    BOOL  flash;	/* flashing patch */
    BOOL  dirty;	/* image changed */
};

#define WM_LOAD_TTXPAGE 1001

class TeletextView:public VBIView{
typedef VBIView INHERITED;
protected:
   int selected_link;//current_selected_link

   struct ttx_patch *patches;	/* patches to be applied */
   UINT	n_patches;

   int entering;
   int timerstarted;
   std::string pgentered;

   std::vector<vbi_link_ext>links;
   void getLinkRect(vbi_link_ext&l,RECT&rect);
   BOOL vbi_page_has_flash(const vbi_page*pg);
   int build_links();
   virtual void onPageReceived(vbi_event&ev);
public:
   TeletextView(int w,int h);
   ~TeletextView();
   void start(int pid,BOOL tsmod=TRUE);
   virtual void onDraw(GraphContext&canvas)override;
   virtual bool onKeyUp(KeyEvent&event)override;
   void requestPage(int page,int sub=VBI_ANY_SUBNO)override;
   bool onMessage(DWORD msgid,DWORD wParam,ULONG lParam);
};


}//namespace

#endif//__TELETEXT_VIEW_H__



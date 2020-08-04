#ifndef __DOCUMENT_VIEW_H__
#define __DOCUMENT_VIEW_H__
#include<view.h>
namespace nglui{

class DocumentView:public View{
protected:
   RefPtr<Surface>doc_surface;
   std::vector<std::string>playlist_;
   int index_;
public:
   DocumentView(int width, int height);
   bool loadDocument(const std::string&path);
   virtual void onDraw(GraphContext&canvas);
   void setPlaylist(const std::vector<std::string>&pl);
   void first();
   void prev();
   void next();
   void last();
};


}//namespace
#endif //__DOCUMENT_VIEW_H__

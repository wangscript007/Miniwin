#ifndef __GRAPH_DEVICE_H__
#define __GRAPH_DEVICE_H__
#include <rect.h>
#include <cairomm/context.h>
#include <map>

using namespace Cairo;
namespace nglui{
class GraphDevice{
private:
   int width;
   int height;
   int format;
   uint64_t last_compose_time;
   class GraphContext*primaryContext;//
   DWORD compose_event;
   HANDLE primarySurface;
   static std::vector<class GraphContext*>gSurfaces;
   static GraphDevice*mInst;
   GraphDevice(int format=-1);
   void sortLayers();
#ifndef CAIRO_HAS_FC_FONT
   struct FT_LibraryRec_*ft_library;
   std::map<std::string,RefPtr<const FontFace>>fonts;
   RefPtr<const FontFace>getFont(const std::string&family=std::string());
#endif
public:
   static GraphDevice&getInstance();
   ~GraphDevice();
   void getScreenSize(int &w,int&h);
   int getScreenWidth();
   int getScreenHeight(); 
   void flip();
   void ComposeSurfaces();
   bool needCompose();
   GraphContext*createContext(int w,int h);
   GraphContext*createContext(const RECT&rect);
   GraphContext*getPrimaryContext();
   void remove(GraphContext*ctx);
   void invalidate(const RECT*);
   HANDLE getPrimarySurface(){return primarySurface;}
};
}
#endif


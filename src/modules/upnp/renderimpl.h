#ifndef __MEDIA_RENDER_IMPL_H__
#define __MEDIA_RENDER_IMPL_H__
#include <string>

namespace nglui{

class MediaRenderer{
protected:
   std::string  deviceName;
   static MediaRenderer*mInst;
   class RendererImpl *deviceimpl;
   MediaRenderer(const std::string&name);
public:
   static MediaRenderer&getInstance();
   ~MediaRenderer();
   MediaRenderer&setName(const std::string&);
   MediaRenderer&start();
   MediaRenderer&stop();
};

}//namespace
#endif

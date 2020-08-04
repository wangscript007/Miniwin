#ifndef __COMPLEX_COLOR_H__
#define __COMPLEX_COLOR_H__

namespace nglui{

class ComplexColor{
public:
   virtual bool isStateful() { return false; }
   virtual int getDefaultColor()=0;
};


}
#endif

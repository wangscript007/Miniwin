#ifndef __DVB_APPLICATION_H__
#define __DVB_APPLICATION_H__
#include<app.h>

namespace nglui{

class DVBApp:public App{
public:
    DVBApp(int argc,const char**argv);
    int exec();
};

}//namespace

#endif

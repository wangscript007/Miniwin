#include<windows.h>
#include "ELuna.h"
using namespace nglui;

void registApp(lua_State* L){
    //register a class and it's constructor. indicate all constructor's param type
    ELuna::registerClass<App>(L, "App", ELuna::constructor<App,int, const char**>);
    //register a method
    ELuna::registerMethod<App>(L, "exec", &App::exec);
    /*ELuna::registerMethod<CPPClass>(L, "cppSum", &CPPClass::cppSum);
    ELuna::registerMethod<CPPClass, CPPClass&>(L, "createRef", &CPPClass::createRef);
    ELuna::registerMethod<CPPClass>(L, "print", &CPPClass::print);*/
}

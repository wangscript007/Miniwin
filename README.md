# Miniwin
MiniWin是一套可适用于IOT,TV等嵌入式系统的UI框架。
Mini问基于C++开发，提供常用UI组件，提供资源管理打包。
提供DVB协议栈功能(本人DTV出身，顺带为TV提供点福利)
该UI框架以Cairo,pixman,freetype等开源模块搭建而成。可运行于DirectFB，FrameBuffer，OpenGL等系统之上。


porting guide:

1,A new porting must be placed to src/porting/xxx(where xxx is you chipset name)
2,implement your porting api to xxx directory
3,enter source root directory 
3,make a dir named outxxx,enter outxxx)
4,cmake -DCMAKE_TOOLCHAN_FILE="your cmake toolchain path" \  
        -DNGL_CHIPSET=xxx -DNGL_CA=irdeto 
        ..
  (NGL_CA is optional) CMAKE_TOOLCHAIN_FILE is optinal for host compile,mandatory for cross compile

5,after step 4,makefile is created in directory outxxx where you can type make to build your program.
6,you can export USE_RFB_GRAPH=ON before cmake is executed to to use remote framebuffer for test.

#include <NGRenderer.h>
#include <Console.h>
#include <FolderUtilities.h>

int main(int argc,const char*argv[]){
   App app(argc,argv);
   FolderUtilities::SetHomeFolder("./");
   std::shared_ptr<Console>console;
   console.reset(new Console);
   console->Init();
   NGRenderer*neswin=new NGRenderer(console,false);
   std::string romfile=argv[1];
   console->Initialize(romfile);
   neswin->sendMessage(1000,0,0);//RunFrame();
   app.exec();
}


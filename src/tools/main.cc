#include "filepak.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <random>
#include <time.h>
#include <dirent.h>
#include <gzstream.h>
#include <memory>

using namespace nglui;
int main(int argc,const char*argv[]){
   FilePAK pak;
   struct stat st;
   if(argc>=3){
       stat(argv[1],&st);
       if(!S_ISDIR(st.st_mode))
          cout<<argv[1]<<" is not a directory"<<endl;
       cout<<argv[0]<<" Dir "<<argv[1] << " To PAK:"<<argv[2]<<endl;
       pak.createPAK(argv[2],argv[1]);
   }
   if(argc==2){
       stat(argv[1],&st);
       if(!S_ISREG(st.st_mode))
          cout<<argv[1]<<" is not exists!"<<endl;
       pak.readPAK(argv[1]);
       std::vector<std::string>names=pak.getAllPAKEntries();
       printf("%-50s  %-8s:%-8s\r\n","ResourceName","Offset","Size"); 
       for(auto nm:names){
           FilePAK::PAKFileEntry*r=pak.getPAKEntry(nm);
           printf("%-50s  %-8d:%-8d\r\n",r->name,r->offset,r->size);//<<r->size<<"  offset:"<<r->offset<<" name:"<<r->name<<" fullname:"<<r->fullname<<std::endl;
           void*data=pak.getPAKEntryData(nm);
           FilePAK::freeEntryData(data);
       }
   }else if(argc==1){
       cout<<"1: package a dir to resource pak "<<endl;
       cout<<"    "<<argv[0]<<" pakagedir resource_pak"<<endl;
       cout<<"2: list resources of resourcepak"<<endl;
       cout<<"    "<<argv[0]<<" pak_file"<<endl;
   }
   return 0;
}

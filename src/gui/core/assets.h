#ifndef __ASSETS_H__
#define __ASSETS_H__

#include<cairomm/surface.h>
#include <map>
#include <memory>
#include <istream>
using namespace Cairo;
namespace nglui{

class Assets{
private:
  std::string osdlanguage;
  std::map<const std::string,RefPtr<ImageSurface>>images;
  std::map<const std::string,std::string>strings;
  class ZIPArchive* pak;
protected:
  void loadStrings(const std::string&lan);
public:
  Assets(const std::string&pakpath);
  ~Assets();
  static RefPtr<ImageSurface>loadImage(std::istream&is);
  RefPtr<ImageSurface>loadImage(const std::string&resname,bool cache=true);
  const std::string getString(const std::string&id,const std::string&lan="");
  std::unique_ptr<std::istream>getInputStream(const std::string&fname);
};

}//namespace
#endif

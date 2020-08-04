#ifndef __ZIP_ARCHIVE_H__
#define __ZIP_ARCHIVE_H__
#include <istream>
#include <string>
#include <vector>
#include <memory>
namespace nglui{

class ZIPArchive{
protected:
  void* zip;
  int method;
public:
  ZIPArchive(const std::string&fname);
  ~ZIPArchive();
  void remove(const std::string&fname);
  int getEntries(std::vector<std::string>&entries);
  std::unique_ptr<std::istream> getInputStream(const std::string&fname);
};

}

#endif

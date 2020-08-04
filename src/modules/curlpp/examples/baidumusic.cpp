#include <sstream>

#include <cstdlib>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>


/**
 * This example shows how to use the easy interface and obtain
 * informations about the current session.
 */
static const char*urls[]={
  "http://tingapi.ting.baidu.com/v1/restserver/ting?format=json&method=baidu.ting.billboard.billList&type=%s&size=10&offset=0",/*test ok*/
  "http://tingapi.ting.baidu.com/v1/restserver/ting?format=json&method=baidu.ting.search.catalogSug&query=%s",/*test ok*/
  "http://tingapi.ting.baidu.com/v1/restserver/ting?format=json&method=baidu.ting.song.lry&songid=%s",
  "http://tingapi.ting.baidu.com/v1/restserver/ting?format=json&method=baidu.ting.song.downWeb&songid=%s&bit=24&t=%lld",
};
int main(int argc, const char **argv) {
    // Let's declare a stream
    // Add some option to the curl_easy object.
    int idx=atoi(argv[1]);
    char url[256];
    if(idx<3)
       sprintf(url,urls[idx],argv[2]);
    else
        sprintf(url,urls[idx],argv[2],time(nullptr));
    printf("URL:%s\r\n",url);
    try {
        curlpp::Cleanup cleaner;
        curlpp::Easy request;
        // Set the writer callback to enable cURL 
        // to write result in a memory area
        request.setOpt(new curlpp::options::WriteStream(&std::cout));
        // Setting the URL to retrive.
        request.setOpt(new curlpp::options::Url(url));
        request.perform();
        return EXIT_SUCCESS;
    }catch ( curlpp::LogicError & e ) {
        std::cout << e.what() << std::endl;
    }catch ( curlpp::RuntimeError & e ) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}

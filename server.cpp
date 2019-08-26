#include<stdio.h>
#include<iostream>
#include<unistd.h>
#include<ifaddrs.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include"httplib.h"
#include<boost/filesystem.hpp>
#include<ostream>
using namespace httplib;
namespace bf = boost::filesystem;
#define SHARED_PATH "Download"
class P2PServer{
  public:
    static void PairHandler(const Request &req,Response &rsp)
    {
      rsp.status = 200;
    }
    static void ListHandler(const Request &req,Response &rsp)
    {
      bf::directory_iterator item_begin(SHARED_PATH) ;
      bf::directory_iterator item_end;
      std::stringstream body;
      for(;item_begin != item_end;++item_begin)
      {
        if(bf::is_directory(item_begin->status()))
          continue;
        bf::path path = item_begin->path();
        std::string name = path.filename().string();
        rsp.body +=name+"\n";
      }
      rsp.set_header("Content-Type","text/html");
      rsp.status = 200;
    }
    static void DownLoadHandler(const Request &req,Response &rsp)
    {
      bf::path path(req.path.c_str());
      std::string mid = SHARED_PATH;
      mid += "/";
      std::string name = mid+ path.filename().string();
      if(!(bf::exists(name)))
      {
        rsp.status = 404;
        return;
      }
      if(bf::is_directory(name))
      {
        rsp.status = 403;
        return;
      }
      std::ifstream file(name.c_str(),std::ios::binary);
      if(!file.is_open())
      {
        std::cerr <<"open file" <<name<< "failed\n";
        rsp.status = 404;
        return;
      }
      int64_t fsize = bf::file_size(name);
      rsp.body.resize(fsize);
      file.read(&rsp.body[0],fsize);
      if(!file.good())
      {
        std::cerr << "read file"<<name<<"failed\n";
        rsp.status = 500;
        return;
      }
      file.close();
      rsp.status = 200;
      rsp.set_header("Content-Type","application/ocet-stream");
    }
  private:
    Server _server;
  public:
    P2PServer(){
      if(!bf::exists(SHARED_PATH))
      {
        bf::create_directory(SHARED_PATH);
      }
    }
    bool Start(uint16_t port)
    {
      _server.Get("/hostpair",PairHandler);
      _server.Get("/list",ListHandler);
      _server.Get("/list/(.*)",DownLoadHandler);
      _server.listen("0.0.0.0",port);
      return true;
    }
};
int main(){
  P2PServer srv;
  srv.Start(22);
  return 0;
}

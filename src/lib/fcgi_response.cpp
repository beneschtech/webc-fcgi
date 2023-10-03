#include <config.h>
#ifdef HAVE_CSTRING
#include <cstring>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#include <fcgi_request_cpp.hxx>

FCGIResponse::FCGIResponse(const FCGX_Request *p)
{
  p_fcgiHandle = p;
  p_data.clear();
  p_headers.clear();
  p_cookies.clear();
  p_httpCode = 200;
}

void FCGIResponse::set_cookie(std::string name,std::string value)
{
  p_cookies[name] = value;
}

void FCGIResponse::set_header(std::string name,std::string value)
{
  p_headers[name] = value;
}

bool FCGIResponse::send()
{
  FCGX_Stream *strm = p_fcgiHandle->out;
  if (!strm)
    return false;
  std::string header = FCGI::headerLine(p_httpCode);
  if (FCGX_PutS(header.c_str(),strm) == -1)
  {
    return false;
  }
  std::string svrname = FCGI::serverName();
  if (!svrname.empty())
  {
    svrname.append("\r\n");
    svrname = "Server: "+svrname;
    if (FCGX_PutS(svrname.c_str(),strm) == -1)
    {
      return false;
    }
  }
  for (std::pair<std::string,std::string> h: p_headers)
  {
    std::string hl = h.first + ": " + h.second + "\r\n";
    if (FCGX_PutS(hl.c_str(),strm) == -1)
    {
      return false;
    }
  }
  for (std::pair<std::string,std::string> h: p_cookies)
  {
    std::string hl = "Set-Cookies: " + h.first + "=" + h.second + "\r\n";
    if (FCGX_PutS(hl.c_str(),strm) == -1)
    {
      return false;
    }
  }
  char buf[256];
  memset(buf,0,sizeof(buf));
  snprintf(buf,sizeof(buf)-1,"Content-Length: %lu\r\n\r\n",p_data.size());
  if (FCGX_PutS(buf,strm) == -1)
  {
    return false;
  }
  if (-1 == FCGX_PutStr(p_data.get(),p_data.size(),strm))
  {
    return false;
  }
  FCGX_Finish_r(const_cast<FCGX_Request *>(p_fcgiHandle));
  return true;
}

void FCGIResponse::set_c_string(const char *s)
{
  p_data.clear();
  p_data.append(s);
}

void FCGIResponse::set_string(std::string &src)
{
  p_data.clear();
  p_data.append(src);
}

void FCGIResponse::set_data(void *src,size_t sz)
{
  p_data = FCGIData((const char *)src,sz);
}

void FCGIResponse::read_local_file(std::string filename)
{
  struct stat s;
  int rc = stat(filename.c_str(),&s);
  if (rc != 0)
  {
    perror(filename.c_str());
    return;
  }
  FILE *f = fopen(filename.c_str(),"rb");
  if (!f)
  {
    perror(filename.c_str());
    return;
  }
  size_t sz = s.st_size;
  p_data.resizeTo(sz);
  if (fread(p_data.get_for_modify(),sz,1,f) != sz)
  {
    perror(filename.c_str());
    return;
  }
  fclose(f);
}

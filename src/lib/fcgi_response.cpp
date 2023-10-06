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

/**
 * @brief FCGIResponse::FCGIResponse is responsible for sending the response
 * to the browser. It takes the pointer from the request object associated
 * with it to set everything up correctly
 * @param p The raw pointer retrieved from FCGIRequest::FCGXHandle()
 */
FCGIResponse::FCGIResponse(const FCGX_Request *p)
{
  p_fcgiHandle = p;
  p_data.clear();
  p_headers.clear();
  p_cookies.clear();
  p_httpCode = 200;
}

/**
 * @brief FCGIResponse::set_cookie Adds or replaces a cookie to send via a
 * Set-Cookie header in the response. The user is responsible for setting
 * "value" to have any additional parameters, such as domains, expiry, etc..
 * *Note* This header may be sent multiple times, and is correct to do so
 * @param name The name of the cookie
 * @param value The value and any additional attributes of the cookie
 */
void FCGIResponse::set_cookie(std::string name,std::string value)
{
  p_cookies[name] = value;
}

/**
 * @brief FCGIResponse::set_header Sets or replaces a raw header value.
 * Common uses are Content-Type, Content-Disposition, or others. The only header
 * that can not be set and is overridden is Content-Length which is determined
 * by the data attached to this response
 * @param name The name of the header, ie "Content-Type"
 * @param value The value of the header, ie "text/html"
 */
void FCGIResponse::set_header(std::string name,std::string value)
{
  p_headers[name] = value;
}

/**
 * @brief FCGIResponse::send sends the message to the browser. At this point
 * the object should be considered invalid and only read operations should be
 * performed at this point.
 * @return true if sent successfully, otherwise false
 */
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
    std::string hl = "Set-Cookie: " + h.first + "=" + h.second + "\r\n";
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

/**
 * @brief FCGIResponse::set_c_string sets the response data to the c string
 * pointed to by s
 * @param s the character pointer to set the data to
 */
void FCGIResponse::set_c_string(const char *s)
{
  p_data.clear();
  p_data.append(s);
}

/**
 * @brief FCGIResponse::set_string Sets the response data to the data contained in
 * the passed std::string object, which should be binary safe.
 * @param src a reference to the std::string object to set the data to
 */
void FCGIResponse::set_string(std::string &src)
{
  p_data.clear();
  p_data.append(src);
}

/**
 * @brief FCGIResponse::set_data sets the raw data pointed to by src for the length
 * of sz as the response data. This is intended to send binary data from an in memory
 * object.
 * @param src pointer to the data to send
 * @param sz the size of the data to send
 */
void FCGIResponse::set_data(void *src,size_t sz)
{
  p_data = FCGIData((const char *)src,sz);
}

/**
 * @brief FCGIResponse::read_local_file loads the file specified in filename to the
 * data to send. This function is binary safe.
 * @param filename the filename to load as the response data
 */
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

#include <config.h>
#ifdef HAVE_IOSTREAM
#include <iostream>
#endif

#include <fcgi_request_cpp.hxx>

FCGIRequest::FCGIRequest(std::shared_ptr<FCGX_Request> r)
{
  p_fcgiHandle = r;
}

FCGIRequest::~FCGIRequest()
{
  if (p_fcgiHandle.use_count() < 2)
  {
    FCGX_Free(p_fcgiHandle.get(),1);
  }
}

bool FCGIRequest::hasHeader(std::string key)
{
  std::map<std::string,std::string>::iterator it = p_headers.find(key);
  return (it != p_headers.end());
}

std::string FCGIRequest::header(std::string key)
{
  std::map<std::string,std::string>::iterator it = p_headers.find(key);
  if (it == p_headers.end())
    return std::string();
  return it->second;
}

const std::map<std::string,std::string> *FCGIRequest::allHeaders()
{
  return &p_headers;
}

static void dump_map(std::string name,std::map<std::string,std::string> *map)
{
  std::cout << name << ": (" << map->size() << " elements)" << std::endl;
  for (std::pair<std::string,std::string> val: *map)
  {
    std::cout << "   " << val.first <<": '" << val.second << "'" << std::endl;
  }
  std::cout << std::endl;
}

static void dump_file_map(std::map<std::string,FCGIMultipartItem> *map)
{
  std::cout << "Files: (" << map->size() << " elements)" << std::endl;
  for (std::pair<std::string,FCGIMultipartItem> val: *map)
  {
    auto it = val.second.attributes.find("filename");
    std::cout << "   " << val.first <<": Filename '" << it->second << "' (" << val.second.data.size() << " bytes)" << std::endl;
  }
  std::cout << std::endl;
}

void FCGIRequest::debug_dump()
{
  std::cout << "Request: " << this << std::endl;
  std::cout << std::endl;
  std::cout << "FastCGI Handle: " << p_fcgiHandle.get() << std::endl;
  std::cout << "URI: " << p_uri << std::endl;
  std::cout << "Query String: " << p_query_string << std::endl;
  std::cout << "Post Data: '";
  for (char c: p_postdata.toStdString())
  {
    if (c == 13)
    {
      std::cout << "\\r";
    } else if (c == 10){
      std::cout << "\\n\n";
    } else if (!std::isprint(c)) {
      char b[10] = {0,};
      snprintf(b,sizeof(b),"\\x%02x",(c & 0xff));
      std::cout << b;
    } else {
      std::cout << c;
    }
  }
  std::cout << "'" << std::endl;

  dump_map("Environment",&p_envp);
  dump_map("Headers",&p_headers);
  dump_map("Query Fields",&p_queryfields);
  dump_map("Post Fields",&p_postfields);
  dump_map("Cookies",&p_cookies);
  dump_file_map(&p_files);
}

bool FCGIRequest::hasEnv(std::string key)
{
  std::map<std::string,std::string>::iterator it = p_envp.find(key);
  return (it != p_envp.end());
}

std::string FCGIRequest::getenv(std::string key)
{
  std::map<std::string,std::string>::iterator it = p_envp.find(key);
  if (it == p_envp.end())
    return std::string();
  return it->second;
}

const std::map<std::string,std::string> *FCGIRequest::allEnviron()
{
  return &p_envp;
}

bool FCGIRequest::hasCookie(std::string key)
{
  std::map<std::string,std::string>::iterator it = p_cookies.find(key);
  return (it != p_cookies.end());
}

std::string FCGIRequest::cookie(std::string key)
{
  std::map<std::string,std::string>::iterator it = p_cookies.find(key);
  if (it == p_cookies.end())
    return std::string();
  return it->second;
}

const std::map<std::string,std::string> *FCGIRequest::allCookies()
{
  return &p_cookies;
}

bool FCGIRequest::hasQueryField(std::string key)
{
  std::map<std::string,std::string>::iterator it = p_queryfields.find(key);
  return (it != p_queryfields.end());
}

std::string FCGIRequest::queryField(std::string key)
{
  std::map<std::string,std::string>::iterator it = p_queryfields.find(key);
  if (it == p_queryfields.end())
    return std::string();
  return it->second;
}

const std::map<std::string,std::string> *FCGIRequest::allQueryFields()
{
  return &p_queryfields;
}

bool FCGIRequest::hasPostField(std::string key)
{
  std::map<std::string,std::string>::iterator it = p_postfields.find(key);
  return (it != p_postfields.end());
}

std::string FCGIRequest::postField(std::string key)
{
  std::map<std::string,std::string>::iterator it = p_postfields.find(key);
  if (it == p_postfields.end())
    return std::string();
  return it->second;
}

const std::map<std::string,std::string> *FCGIRequest::allPostFields()
{
  return &p_postfields;
}


bool FCGIRequest::hasFile(std::string key)
{
  std::map<std::string,FCGIMultipartItem>::iterator it = p_files.find(key);
  return (it != p_files.end());
}

FCGIMultipartItem FCGIRequest::file(std::string key)
{
  std::map<std::string,FCGIMultipartItem>::iterator it = p_files.find(key);
  if (it == p_files.end())
    return FCGIMultipartItem();
  return it->second;
}

const std::map<std::string,FCGIMultipartItem> *FCGIRequest::allFiles()
{
  return &p_files;
}

FCGIData *FCGIRequest::postData()
{
  return &p_postdata;
}

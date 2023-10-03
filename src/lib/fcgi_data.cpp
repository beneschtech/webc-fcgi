#include <config.h>

#ifdef HAVE_CSTRING
#include <cstring>
#endif

#include <fcgi_request_cpp.hxx>

FCGIData::FCGIData()
{
  p_data.clear();
}

FCGIData::FCGIData(std::string &s)
  :FCGIData()
{
  p_data.resize(s.size());
  memset(p_data.data(),0,s.size());
  memcpy(p_data.data(),s.data(),s.size());
}

FCGIData::FCGIData(const char *p,size_t sz)
  :FCGIData()
{
  p_data.resize(sz);
  memset(p_data.data(),0,sz);
  memcpy(p_data.data(),p,sz);
}

bool FCGIData::resizeTo(size_t sz)
{
  p_data.resize(sz);
  return (p_data.size() == sz);
}

bool FCGIData::append(char c)
{
  size_t sz = p_data.size();
  p_data.push_back(c);
  return (p_data.size() == (sz+1));
}

const char FCGIData::at(size_t i)
{
  char rv = 0;
  if (p_data.size() <= i)
    return 0xff;
  rv = p_data.at(i);
  return rv;
}

bool FCGIData::append(const char *s)
{
  size_t sz = p_data.size();
  sz += strlen(s);
  char *p = const_cast<char *>(s);
  while (*p)
  {
    p_data.push_back(*p);
    p++;
  }
  return (p_data.size() == sz);
}

bool FCGIData::append(std::string &s)
{
  size_t sz = p_data.size();
  sz += s.size();
  const char *p = s.data();
  size_t i = 0;
  const size_t ssz = s.size();
  while (i < ssz)
  {
    p_data.push_back(*p);
    p++; i++;
  }
  return (p_data.size() == sz);
}

std::string FCGIData::toStdString()
{
  std::string rv;
  rv.resize(p_data.size());
  char *dest = const_cast<char *>(rv.data());
  memcpy(dest,p_data.data(),p_data.size());
  return rv;
}

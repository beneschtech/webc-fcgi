/*
 * Copyright 2023 Chris Benesch
 *
 * fcgi_request_cpp - A somewhat simple post processor for FastCGI
 * requests to put in front of your CGI/C++ based application. It's
 * a common thing to have to reinvent, and this saves that time
 *
 * Compare and inspired by the ancient ccgi package from GNU
 *
 * MIT Standard distribution license
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the “Software”),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
 * NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
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

char FCGIData::at(const size_t i)
{  
  if (p_data.size() <= i)
    return 0xff;
  const char rv = p_data.at(i);
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

#include <config.h>

#ifdef HAVE_CSTRING
#include <cstring>
#endif

#include <fcgi_request_cpp.hxx>

std::vector<struct FCGIMultipartItem> FCGIRequest::parseMultipart(std::string boundary, FCGIData data)
{
  std::vector<struct FCGIMultipartItem> rv;

  std::string dataStr;
  dataStr.resize(data.size());
  std::memcpy(dataStr.data(),data.get_for_modify(),data.size());
  std::string::size_type idx = dataStr.find(boundary,0);
  const std::string::size_type bndLen = boundary.length();

  while (idx != std::string::npos)
  {
    std::string::size_type start = idx+bndLen;
    start += 2;
    idx = dataStr.find(boundary,start);
    if (idx == std::string::npos)
      continue; // weve hit the end
    std::string::size_type dataStart = dataStr.find("\r\n\r\n",start);
    std::string nvpairs = dataStr.substr(start,dataStart-start);
    std::string::size_type rpos = nvpairs.find("\r\n");
    std::string::size_type lrpos = 0;
    std::string nvtmp;
    if (rpos == std::string::npos)
    {
      nvtmp = nvpairs;
    } else {
    while (rpos != std::string::npos)
    {
      nvtmp.append(nvpairs.substr(lrpos,rpos));
      lrpos = rpos+2;
      if (lrpos > nvpairs.size()) break;
      rpos = nvpairs.find("\r\n",lrpos);
      nvtmp.append("; ");
      if (rpos == std::string::npos)
      {
        nvtmp.append(nvpairs.substr(lrpos));
      }
    }
    }
    nvpairs = nvtmp;
    FCGIMultipartItem itm;
    itm.data.resizeTo(idx-dataStart-8);
    std::memcpy(itm.data.get_for_modify(),&data.get()[dataStart+4],itm.data.size());
    std::vector<std::string::size_type> commas;
    std::string::size_type i = 0;
    while (i != std::string::npos)
    {
      commas.push_back(i);
      i = nvpairs.find(';',i+1);
    }
    commas.push_back(i);
    std::vector<std::string> nvpairsv;
    for (size_t j = 1; j < commas.size(); j++)
    {
      if (j == 1)
      {
      nvpairsv.push_back(nvpairs.substr(commas[j-1],commas[j]-commas[j-1]));
      } else {
        nvpairsv.push_back(nvpairs.substr(commas[j-1]+1,commas[j]-1-commas[j-1]));
      }
    }
    for (std::string nvpair: nvpairsv)
    {
      std::vector<std::string> nv;
      std::string key,val;
      if (nvpair.find('=') != std::string::npos)
      {
        nv = FCGI::str_split(nvpair,'=');
      } else if (nvpair.find(':') != std::string::npos) {
        nv = FCGI::str_split(nvpair,':');
      } else {
        itm.attributes.insert({FCGI::string_trim(nvpair),""});
      }
      if (nv.size() == 2)
      {

        key = FCGI::string_trim(nv[0]);
        val = FCGI::string_trim(nv[1]);
        if (val.front() == '"')
        {
          val = val.substr(1,val.size()-2);
        }
        itm.attributes.insert({key,val});
        if (key.find("Content") != std::string::npos) // content type or disposition
        {
          if (val.find("base64") != std::string::npos)
          {
            if (itm.data.size() > 0)
            {
              std::string base64 = itm.data.toStdString();
              FCGIData d = FCGI::base64Decode(base64);
              itm.data = d;
            }
          }
        }
      }
    }
    rv.push_back(itm);
  }
  return rv;
}

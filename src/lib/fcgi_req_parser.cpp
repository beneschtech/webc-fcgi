#include <config.h>
#ifdef HAVE_CSTRING
#include <cstring>
#endif
#ifdef HAVE_IOSTREAM
#include <iostream>
#endif
#include <fcgi_request_cpp.hxx>

static std::string safe_get_map_value(std::string key,std::map<std::string,std::string> *map)
{
    std::map<std::string,std::string>::iterator it = map->find(key);
    if (it == map->end())
        return std::string();
    return it->second;
}

bool FCGIRequest::parse()
{
    char **envp = p_fcgiHandle->envp;
    while (*envp)
    {
        std::string envs(*envp);
        size_t idx = envs.find_first_of('=');
        if (idx == std::string::npos)
        {
            break;
        }
        std::string key = envs.substr(0,idx);
        std::string val = envs.substr(idx+1);
        p_envp.insert({key,val});
        if (key.substr(0,5) == "HTTP_")
        {
            p_headers.insert({key.substr(5),val});
        }
        envp++;
    }
    p_uri = safe_get_map_value("SCRIPT_NAME",&p_envp);
    p_query_string = safe_get_map_value("QUERY_STRING",&p_envp);
    std::string len= safe_get_map_value("CONTENT_LENGTH",&p_envp);
    size_t clen = 0;
    if (!len.empty())
    {
        clen = strtoul(len.c_str(),nullptr,10);
    }
    p_postdata.resizeTo(clen);
    char *pdata = p_postdata.get_for_modify();
    memset(pdata,0,clen);
    FCGX_GetStr(pdata,clen,p_fcgiHandle->in);
    p_queryfields = FCGI::query_string_parse(p_query_string);
    std::string cookiestr = safe_get_map_value("HTTP_COOKIE",&p_envp);
    if (cookiestr.length())
        p_cookies = FCGI::cookie_parse(cookiestr);

    if (!p_postdata.empty())
    {
        std::string contentType = safe_get_map_value("CONTENT_TYPE",&p_headers);
        std::string boundary;
        if (contentType.find("multipart/") != std::string::npos)
        {
            std::vector<std::string> ctarr = FCGI::str_split(contentType,';');
            for (std::string v: ctarr)
            {
                v = FCGI::string_trim(v);
                if (v.find("boundary") == 0)
                {
                    std::string::size_type i = v.find('=');
                    if (i != std::string::npos)
                    {
                        boundary = v.substr(i+1);
                        break;
                    }
                }
            }
            if (!boundary.empty())
            {
                std::vector<FCGIMultipartItem> items = parseMultipart(boundary,p_postdata);
                for (FCGIMultipartItem itm: items)
                {
                  auto nmit = itm.attributes.find("name");
                  if (nmit == itm.attributes.end())
                  {
                    continue; // I dont know how to deal with this (yet)
                  }
                  auto fnit = itm.attributes.find("filename");
                  if (fnit == itm.attributes.end())
                  {
                    // no filename, so it must be a field value
                    p_postfields[nmit->second] = itm.data.toStdString();
                  } else {
                    std::string fname = fnit->second;
                    p_files[nmit->second] = { itm };
                  }
                }
            }
        } else {
            std::string pdata = std::string(p_postdata.get(),p_postdata.size());
            p_postfields = FCGI::query_string_parse(pdata);
        }
    }
    return true;
}

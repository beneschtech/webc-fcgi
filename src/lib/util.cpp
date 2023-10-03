#include <config.h>
#include <fcgi_request_cpp.hxx>

#ifdef __linux
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif
#ifdef HAVE_CSTRING
#include <cstring>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_SYSCALL_H
#include <sys/syscall.h>
#endif
#endif

static std::string _serverName;

namespace FCGI
{

std::string serverName() { return _serverName; }
void setServerName(std::string s) { _serverName = s; }

void SetThreadName(const char* threadName)
{
    if (!*threadName)
        return;
#ifdef __linux
    if (getpid() != syscall(SYS_gettid))
    {
        // Basically strncpy into a preallocated buffer
        char namebuf[16];
        size_t sz = strlen(threadName);
        if (sz > sizeof(namebuf)-1)
            sz = sizeof(namebuf)-1;
        memset(namebuf,0,sizeof(namebuf));
        memcpy(namebuf,threadName,sz);
        namebuf[15] = 0; // Make sure its 0 terminated
        pthread_t myThread = pthread_self();
        pthread_setname_np(myThread,namebuf);
    }
#endif
}

std::vector<std::string> str_split(std::string haystack,char needle)
{
    std::vector<std::string> rv;
    std::string::size_type sz = 0;
    while (sz != std::string::npos)
    {
        std::string::size_type i = haystack.find_first_of(needle,sz);
        rv.push_back(haystack.substr(sz,i));
        if (i == std::string::npos)
        {
            sz = i;
        } else {
            sz = i+1;
        }
    }
    return rv;
}


std::map<std::string,std::string> query_string_parse(std::string l)
{
    std::map<std::string,std::string> rv;
    if (l.empty())
        return rv;
    std::vector<std::string> nvpairs = str_split(l,'&');
    for (std::string pair: nvpairs)
    {
        std::string::size_type idx = pair.find_first_of('=');
        std::string key = pair.substr(0,idx);
        std::string val = urldecode(pair.substr(idx+1));
        rv.insert({key,val});
    }
    return rv;
}

std::map<std::string,std::string> cookie_parse(std::string l)
{
    std::map<std::string,std::string> rv;
    if (l.empty())
        return rv;
    std::vector<std::string> nvpairs = str_split(l,';');
    for (std::string pair: nvpairs)
    {
        std::string::size_type idx = pair.find_first_of('=');
        std::string key = pair.substr(0,idx);
        std::string val = pair.substr(idx+1);
        rv.insert({key,val});
    }
    return rv;
}

std::string string_trim(std::string s)
{
    std::string rv = s;
    while (std::isspace(rv.front()))
    {
        rv = rv.substr(1);
    }
    while (std::isspace(rv.back()))
    {
        rv = rv.substr(0,rv.length()-1);
    }
    return rv;
}

}; // namespace FCGI

#include <config.h>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <fcgi_request_cpp.hxx>

std::string htmlEchoRequest(FCGIRequest *);

int main()
{
    FCGIListener l("/tmp/simple-echo.sock");
    if (!l.open())
    {
        std::cerr << l.error_string() << std::endl;
        return 1;
    }
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    ::chmod(l.listener_path().c_str(),mode);
    if (!l.start())
    {
        std::cerr << l.error_string() << std::endl;
        return 1;
    }
    while (1)
    {
        FCGIRequest req = l.nextRequest();
        std::string rv = htmlEchoRequest(&req);
        FCGIResponse resp(req.FCGXHandle());
        resp.set_header("Content-Type","text/html");
        resp.set_string(rv);
        resp.send();
        std::string::size_type idx = req.uri().find("stop");
        if (idx != std::string::npos)
        {
            l.stop();
            break;
        }
    }
}

std::string htmlEchoRequest(FCGIRequest *req)
{
  std::string rv;
  std::stringstream ss;
  ss << "<html>\n <head>\n  <title>Request " << req << "</title>\n </head>\n <body>";
  ss << "  <table align='center'>\n";
  ss << "   <tr><th colspan=2>Request Parameters</th></tr>\n";
  ss << "   <tr><th>URI</th><td>" << req->uri() << "</td></tr>\n";
  ss << "   <tr><th>Query String</th><td>" << req->query_string() << "</td></tr>\n";
  ss << "   <tr><th>Method</th><td>" << req->method() << "</td></tr>\n";
  ss << "  </table>\n";

  ss << "  <table align='center' border=1>\n";
  ss << "   <tr><th colspan=2>Environment</th></tr>\n";
  std::map<std::string,std::string> *envp = const_cast<std::map<std::string,std::string> *>(req->allEnviron());
  for (std::map<std::string,std::string>::iterator it = envp->begin(); it != envp->end(); it++)
  {
    ss << "   <tr><th>" << it->first << "</th><td>" << it->second << "</td></tr>\n";
  }
  ss << "  </table>\n";

  ss << " </body>\n</html>" << std::endl;
  rv = ss.str();
  return rv;
}

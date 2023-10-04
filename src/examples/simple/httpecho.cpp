#include <config.h>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <fcgi_request_cpp.hxx>

// Forward declaration for readability
std::string htmlEchoRequest(FCGIRequest *);

/**
 * This program echoes the request sent to the browser in a very
 * aesthetically pleasing table format. Although it only shows
 * a few parameters and the envp array, it does demonstrate
 * how to access and iterate over the values stored.
 */
int main()
{
  // Constructor can take the location of the listening socket
  FCGIListener l("/tmp/simple-echo.sock");
  // Always check for errors
  if (!l.open())
  {
    std::cerr << l.error_string() << std::endl;
    return 1;
  }

  // Enahcement, default to 0666 mode, allowing override
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  ::chmod(l.listener_path().c_str(),mode);

  // This actually starts in a seperate thread and parses requests as
  // they come in, adding them to a queue.  It is left up to the caller
  // as to the processing model, ie single thread, thread per, or
  // pooled.  This is similar to how microhttpd's event loop works
  if (!l.start())
  {
    std::cerr << l.error_string() << std::endl;
    return 1;
  }

  // Since this is an example program, we only use single threaded
  // processing
  while (1)
  {
    // Mutex protected queue
    FCGIRequest req = l.nextRequest();
    // Process the request and return a string
    std::string rv = htmlEchoRequest(&req);
    // Pair a response object with the request
    FCGIResponse resp(req.FCGXHandle());
    // Content type, important for specifying what will be shown
    resp.set_header("Content-Type","text/html");
    // Set return data
    resp.set_string(rv);
    // And ship it off
    resp.send();

    // If the request URI had "stop" in it, stop the server and
    // exit cleanly
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
  // Example of how to iterate over decoded parts of the request
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

#include <config.h>
#include <iostream>
#include <sys/stat.h>
#include <fcgi_request_cpp.hxx>

int main()
{
    FCGIListener l("/tmp/simple-pingpong.sock");
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
    FCGI::setServerName("pingpong/1.0");
    while (1)
    {
        FCGIRequest req = l.nextRequest();
        FCGIResponse resp(req.FCGXHandle());

        std::string::size_type stopidx = req.uri().find("stop");
        std::string::size_type pingidx = req.uri().find("/ping/");
        resp.set_header("Content-Type","text/plain");
        if (pingidx == std::string::npos)
        {
          resp.set_c_string("OK\r\n");
        } else {
          resp.set_c_string("Pong!\r\n");
        }
        resp.send();

        if (stopidx != std::string::npos)
        {
            l.stop();
            break;
        }
    }
}

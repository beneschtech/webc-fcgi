#include <config.h>
#include <exception>
#include <iostream>
#include <memory>
#include <thread>
#include <errno.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <fcgi_request_cpp.hxx>
#include <fcgiapp.h>

/**
 * @brief FCGIListener::FCGIListener default constructor
 * There should only be one of these objects in an
 * application and runs its event loop in a seperate
 * thread
 */
FCGIListener::FCGIListener()
{
    p_listenerSocketPath.clear();
    p_fcgiHandle = -1;
    p_errorString.clear();
    p_stopFlag = false;
    p_state = INVALID;
}

/**
 * @brief FCGIListener::FCGIListener c-tor which sets up
 * the passed in path
 * @param listenerPath the path to listen to as a unix
 * socket filename, or TCP listen port
 */
FCGIListener::FCGIListener(std::string listenerPath)
    :FCGIListener()
{
    p_listenerSocketPath = listenerPath;
}

FCGIListener::~FCGIListener()
{
    ::unlink(p_listenerSocketPath.c_str());
}

bool FCGIListener::open()
{    
    p_errorString.clear();
    if (p_listenerSocketPath.empty())
    {
        p_errorString = "Blank listener path";
        return false;
    }
    FCGX_Init();
    if (::unlink(p_listenerSocketPath.c_str()) != 0)
    {
        std::string err = strerror(errno);
        if (err != "No such file or directory")
        {
            p_errorString = p_listenerSocketPath;
            p_errorString.append(" (unlink): ");
            p_errorString.append(err);
            return false;
        }
    }
    FILE *f = fopen(p_listenerSocketPath.c_str(),"w");
    if (!f)
    {
        p_errorString = p_listenerSocketPath;
        p_errorString.append(": ");
        p_errorString.append(strerror(errno));
        return false;
    }
    fclose(f);
    if (::unlink(p_listenerSocketPath.c_str()) != 0)
    {
        p_errorString = p_listenerSocketPath;
        p_errorString.append(" (unlink): ");
        p_errorString.append(strerror(errno));
        return false;
    }
    p_fcgiHandle = FCGX_OpenSocket(p_listenerSocketPath.c_str(),10);
    if (p_fcgiHandle >= 0)
        p_state = ATTACHED;
    return (p_fcgiHandle != -1);
}

bool FCGIListener::start()
{
    if (p_fcgiHandle <= 0)
    {
        p_errorString = "Invalid cgi handle (call open() first)";
        return false;
    }
    if (p_state == RUNNING)
    {
        p_errorString = "Listener already running";
        return false;
    }
    p_emptyMutex.lock();
    std::thread t1(&FCGIListener::thr_listen,this);
    t1.detach();
    return true;
}

void FCGIListener::thr_listen()
{
    p_state = RUNNING;
    p_stopFlag = false;
    FCGI::SetThreadName("FCGI Listen");

    while (!p_stopFlag)
    {
        std::shared_ptr<FCGX_Request> req = std::make_shared<FCGX_Request>();
        FCGX_InitRequest(req.get(),p_fcgiHandle,0);
        if (FCGX_Accept_r(req.get()) == 0)
        {
            FCGIRequest reqst(req);
            if (reqst.parse())
            {
                req.reset();
                {
                    p_emptyMutex.try_lock();
                    p_reqQueue.push_back(reqst);
                    p_emptyMutex.unlock();
                }
            }
        } else {
            char errdesc[1024];
            if (!p_stopFlag)
                p_errorString = strerror_r(errno,errdesc,sizeof(errdesc));
            p_stopFlag = true;
        }
    }
    p_state = STOPPED;
}

void FCGIListener::stop()
{
    p_stopFlag = true;
    ::shutdown(p_fcgiHandle,SHUT_RDWR);
    ::close(p_fcgiHandle);
}

FCGIRequest FCGIListener::nextRequest()
{
    std::lock_guard<std::mutex> l(p_mutex);
    FCGIRequest rv(nullptr);
    {
      std::lock_guard<std::mutex> l2(p_emptyMutex);
      rv = p_reqQueue.front();
      p_reqQueue.erase(p_reqQueue.begin());
    }
    if (p_reqQueue.empty())
      p_emptyMutex.try_lock();
    return rv;
}

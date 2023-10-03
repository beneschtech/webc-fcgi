#ifndef FCGI_REQUEST_CPP_HXX
#define FCGI_REQUEST_CPP_HXX

#include <fcgiapp.h>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <mutex>

/**
 * @brief The FCGIData class represents a chunk of raw data
 * a std::string would suffice, but this allows seperation and
 * clear idnetification of binary data.  It is implemented as
 * a vector of chars with common operations stubbed out
 */
class FCGIData
{
public:
  FCGIData();
  FCGIData(std::string &);
  FCGIData(const char *,size_t sz);
  bool append(std::string &);
  bool append(const char *);
  bool append(char);
  char *get_for_modify() { return p_data.data(); }
  const char *get() { return p_data.data(); }
  size_t size() { return p_data.size(); }
  bool resizeTo(size_t);
  const char at(size_t);
  std::string toStdString();
  void clear() { p_data.clear(); }
  bool empty() { return p_data.size() == 0; }

private:
  std::vector<char> p_data;
};

/**
 * @brief The FCGIMultipartItem class represents an item of
 * a multipart message. If it has an indicator of being
 * a base64 encoded string, it will be automatically
 * converted
 */
struct FCGIMultipartItem
{
  std::map<std::string,std::string> attributes;
  FCGIData data;
};

// Util functions
namespace FCGI
{
void SetThreadName(const char *);
std::vector<std::string> str_split(std::string haystack,char needle);
std::string urldecode(std::string);
std::string urlencode(std::string);
std::map<std::string,std::string> query_string_parse(std::string);
std::map<std::string,std::string> cookie_parse(std::string);
std::string string_trim(std::string);
void setServerName(std::string);
std::string serverName();
std::string headerLine(unsigned short httpcode);
FCGIData base64Decode(std::string &);
std::string base64Encode(FCGIData &);
};


class FCGIResponse
{
public:
  FCGIResponse(const FCGX_Request *);
  bool send();
  void set_cookie(std::string name,std::string value);
  void set_header(std::string name,std::string value);
  FCGIData *dataPtr() { return &p_data; }
  int status() { return p_httpCode; }
  void set_status_code(int code) { p_httpCode = code; }
  void set_string(std::string &);
  void set_data(void *,size_t);
  void set_c_string(const char *);
  void read_local_file(std::string);

private:
  int p_httpCode;
  std::map<std::string,std::string> p_headers;
  std::map<std::string,std::string> p_cookies;
  FCGIData p_data;
  const FCGX_Request *p_fcgiHandle;
};

class FCGIRequest
{
public:
  FCGIRequest(std::shared_ptr<FCGX_Request>);
  ~FCGIRequest();
  bool parse();
  void debug_dump();
  const FCGX_Request *FCGXHandle() { return p_fcgiHandle.get(); }
  const std::string uri() { return p_uri; }
  const std::string query_string() { return p_query_string; }
  const std::string method() { return p_method; }

  bool hasEnv(std::string);
  std::string getenv(std::string);
  const std::map<std::string,std::string> *allEnviron();
  bool hasHeader(std::string);
  std::string header(std::string);
  const std::map<std::string,std::string> *allHeaders();
  bool hasCookie(std::string);
  std::string cookie(std::string);
  const std::map<std::string,std::string> *allCookies();
  bool hasQueryField(std::string);
  std::string queryField(std::string);
  const std::map<std::string,std::string> *allQueryFields();
  bool hasPostField(std::string);
  std::string postField(std::string);
  const std::map<std::string,std::string> *allPostFields();
  bool hasFile(std::string);
  FCGIMultipartItem file(std::string);
  const std::map<std::string,FCGIMultipartItem> *allFiles();
  FCGIData *postData();

protected:
  std::vector<struct FCGIMultipartItem> parseMultipart(std::string boundary,FCGIData data);

private:
  std::shared_ptr<FCGX_Request> p_fcgiHandle;
  std::map<std::string,std::string> p_envp;
  std::map<std::string,std::string> p_headers;
  std::map<std::string,std::string> p_cookies;
  std::map<std::string,std::string> p_postfields;
  std::map<std::string,std::string> p_queryfields;
  std::map<std::string,FCGIMultipartItem> p_files;
  FCGIData p_postdata;
  std::string p_uri;
  std::string p_query_string;
  std::string p_method;
};

class FCGIListener
{
public:
  FCGIListener();
  FCGIListener(std::string listenerPath);
  ~FCGIListener();
  bool open();
  bool start();
  void stop();

  enum State {
    INVALID,
    ATTACHED,
    RUNNING,
    STOPPED
  };

  void set_listener_path(std::string p) { p_listenerSocketPath = p; }
  const std::string listener_path() { return p_listenerSocketPath; }
  const int socket() { return p_fcgiHandle; }
  bool const has_error() { return (p_errorString.length() > 0); }
  const std::string error_string() { return p_errorString; }
  const State state() { return p_state; }
  FCGIRequest nextRequest();

protected:
  void thr_listen();

private:
  std::vector<FCGIRequest> p_reqQueue;
  std::mutex p_mutex;
  std::mutex p_emptyMutex;
  std::string p_listenerSocketPath;
  std::string p_errorString;
  int p_fcgiHandle;
  bool p_stopFlag;
  State p_state;
};

#endif // FCGI_REQUEST_CPP_HXX

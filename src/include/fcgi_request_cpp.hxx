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
  /**
   * @brief FCGIData default constructor, called by all other
   * flavors of constructors which initializes all the data
   * to a known cleared/good state, and allows the object to
   * be able to be included in a STL contianer
   */
  FCGIData();
  /**
   * @brief FCGIData c-tor which copies the values in the
   * string passed in to the data vector internally
   * @param data - a reference to a std::string to copy
   */
  FCGIData(std::string &data);
  /**
   * @brief FCGIData c-tor which copies sz bytes of the
   * passed in pointer into the internal data
   * @param sz the number of bytes to copy
   * @param data the pointer to the data to copy in
   */
  FCGIData(const char *data,size_t sz);
  /**
   * @brief append adds the data contianed in the passed
   * in string into the internal data.  This is done in
   * a binary safe manner, ie size and raw pointers
   * passed to memcpy()
   * @return true if successfull, false if not
   */
  bool append(std::string &);
  /**
   * @brief append adds the data contianed in the passed
   * in c string into the internal data.  This is not
   * binary safe, and only proceeds to the next 0 byte
   * @return true if successfull, false if not
   */
  bool append(const char *);
  /**
   * @brief append adds a single character to the data
   * @return true if successfull, false if not
   */
  bool append(char);
  /**
   * @brief get_for_modify returns a non-const pointer
   * to the internal data
   * @return the internal pointer to the data
   */
  char *get_for_modify() { return p_data.data(); }
  /**
   * @brief get_for_modify returns a const pointer
   * to the internal data, should be ususal one called
   * to directly access the data
   * @return the internal pointer to the data
   */
  const char *get() { return p_data.data(); }
  /**
   * @brief size returns the size of the internal data
   * structure.
   * @return the size of the data
   */
  size_t size() { return p_data.size(); }
  /**
   * @brief resizeTo resizes the internal data to
   * the passed in size. The data will be in an
   * undefined state, and can not be guaranteed to be
   * initialized to zero
   * @return true if successfull, false if not
   */
  bool resizeTo(size_t);
  /**
   * @brief at
   * @return returns the single byte at the position indicated
   */
  char at(const size_t);
  /**
   * @brief toStdString creates a std::String object with the
   * internal data copied to it and returns it. Binary safe
   * @return a std::string containing the data from this object
   */
  std::string toStdString();
  /**
   * @brief clear clears the internal data structure also setting
   * the size to zero.
   */
  void clear() { p_data.clear(); }
  /**
   * @brief empty checks if the size of the internal data is zero
   * @return true if empty, false if not
   */
  bool empty() { return p_data.size() == 0; }

private:
  std::vector<char> p_data;
};

/**
 * @brief The FCGIMultipartItem struct represents an item of
 * a multipart message. If it has an indicator of being
 * a base64 encoded string, it will be automatically
 * converted. This is usually used in post data for
 * fields and uploaded files.
 */
struct FCGIMultipartItem
{
  /**
   * @brief attributes the attributes passed in such as
   * content-disposition, type, etc..
   */
  std::map<std::string,std::string> attributes;
  /**
   * @brief data an FCGIData class instance of the actual
   * binary data contained in the item.
   */
  FCGIData data;
};

/**
 * @brief
 * The FCGI namespace encapsulates some utility functions that
 * are used internally and likely helpful to users of the library
 * It is in a namespace to avoid collissions with user code
 */
namespace FCGI
{
/**
 * @brief SetThreadName if supported, sets the thread name for easier
 * debugging / performance benchmarking. Currently implemented on
 * Linux, but short port to any public *nix flavor without too
 * much trouble
 */
void SetThreadName(const char *);
/**
 * @brief str_split splits a string along a character. Keep in mind
 * this function may not behave as expected. In the case of something
 * like passwd=123=-65 it will resolve passwd / 123=-65
 * @param haystack The string to split
 * @param needle The character to split on
 * @return an even numbered vector of split strings
 */
std::vector<std::string> str_split(std::string haystack,char needle);
/**
 * @brief urldecode decodes a string from urlencoding ie "percent"
 * encoding into its raw data representation. Although technically
 * this can represent binary data, it is most commonly used for
 * strings that may have puncuation or things like that so its return
 * type is simply defined as a string
 * @return The string / raw data that this represents.
 */
std::string urldecode(std::string);
/**
 * @brief urlencode convert a string (or binary data)
 * into a urlencoded string.
 * @return a urlencoded (percent) string representing the string
 * passed in.
 */
std::string urlencode(std::string);
/**
 * @brief query_string_parse parses a query string into
 * name/value pairs, and also urldecodes the values
 * @return a map of name/value pairs decoded from the
 * query string passed in
 */
std::map<std::string,std::string> query_string_parse(std::string);
/**
 * @brief cookie_parse parses the Cookie header line into
 * cookie names and urldecoded values.  It is seperated by
 * semicolons and = signs as opposed to the  query string
 * with & symbols
 * @return a map of name/value pairs decoded from the
 * cookie string passed in
 */
std::map<std::string,std::string> cookie_parse(std::string);
/**
 * @brief string_trim trims whitespace off the beginning and
 * end of a string.  Many operations with HTTP data are
 * whitespace agnostic.
 * @return the trimmed string
 */
std::string string_trim(std::string);
/**
 * @brief setServerName
 * This sets a server name to be returned to a browser
 * This is not to be confused with hostname, but something
 * like nginx/1.3 or apache/2.44.  This is stored in an
 * internal buffer and only needs to be called once
 */
void setServerName(std::string);
/**
 * @brief serverName returns the server name set by
 * setServerName, or else a blank string
 * @return
 */
std::string serverName();
/**
 * @brief headerLine returns a line suitable for a reply
 * to a webserver indicating the status code
 * Example: "HTTP/1.1 200 OK"
 * @param httpcode the code to generate a signature for
 * @return the strign to pass to the server/user
 */
std::string headerLine(unsigned short httpcode);
/**
 * @brief base64Decode decodes a base64 encoded string
 * and returns the raw data as an FCGIData object.  This
 * was largely ripped from FreeBSD
 * @param in the base64 encoded string
 * @return the binary data represented
 */
FCGIData base64Decode(std::string &);
/**
 * @brief base64Encode encodes binary data into a base64 string
 * @param dat - The binary data to encode
 * @return  The base64 encoded string
 */
std::string base64Encode(FCGIData &);
};

/**
 * @brief The FCGIResponse class is the object responsible
 * for sending the response to the browser. It is tied
 * to the request via the constructor
 */
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

/**
 * @brief The FCGIRequest class is the heart of
 * this project. The FCGIListener class creates
 * instances of these in a queue to be handed
 * out to the application.
 */
class FCGIRequest
{
public:
  /**
   *@brief Default c-tor, with the copyable object type
   * std::shared_ptr which allows usage in a STL object
   * The pointer is initialized from the FCGI accept loop
   * and is only destroyed once all copies have been
   * destroyed.
   */
  FCGIRequest(std::shared_ptr<FCGX_Request>);
  /**
   * @brief default destructor, when the number of copies
   * reaches 1, it calls the fast cgi library destroy
   * function, which facilitates its copyable ability
   * and also ensures proper cleanup.
   */
  ~FCGIRequest();
  /**
   * @brief parse Parses the request and fills all of the
   * appropriate data structures with the data retrieved.
   * @return true if parsing was successfull, flase if not
   * and not added to the request queue
   */
  bool parse();
  /**
   * @brief debug_dump for debugging of the library, shows
   * the data contained in the request after parsing.
   */
  void debug_dump();
  /**
   * @brief FCGXHandle
   * @return a handle of the internal FastCGI handle to be used
   * by a response object. This object must exist for as long
   * as the response object, or else the pointer will become
   * invalid on this objects destruction.
   */
  const FCGX_Request *FCGXHandle() { return p_fcgiHandle.get(); }
  /**
   * @brief uri
   * @return The url string of the request, ie /myapp/x/y/z
   */
  const std::string uri() { return p_uri; }
  /**
   * @brief query_string
   * @return The part of the URL that comes after the "?"
   * It is usually either parsed for name/value pairs
   * or used directly as a session id or other uses
   */
  const std::string query_string() { return p_query_string; }
  /**
   * @brief method
   * @return The method used, ie GET POST DELETE etc..
   */
  const std::string method() { return p_method; }

  /**
   * @brief hasEnv checks if "name" exists in the environment
   * variable array
   * @param name string to search
   * @return true if found, false if not
   */
  bool hasEnv(std::string name);
  /**
   * @brief getenv gets the environment variable denoted by
   * "name" similar to the libc "getenv(const char *)"
   * function. A fastcgi request uses this to represent several
   * one off and useful parameters
   * @param name string to search for
   * @return the value of the environment variable represented
   * by "name"
   */
  std::string getenv(std::string name);
  /**
   * @brief allEnviron gets the internal structure of the environment
   * variable structure for iteration or other uses outside of the
   * objects internal functions
   * @return a pointer to the map of name/value pairs
   */
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

/**
 * @brief The FCGIListener class
 * This class should be application global and provides
 * the FastCGI listening, initialization, and an event loop
 * which then creates FCGIRequest objects to hand off from
 * an internal queue for processing.
 */
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
  int socket() { return p_fcgiHandle; }
  bool has_error() { return (p_errorString.length() > 0); }
  const std::string error_string() { return p_errorString; }
  State state() { return p_state; }
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

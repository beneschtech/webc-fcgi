# webc-fcgi

A C++ wrapper and implementation to present fastcgi requests in an object oriented fashion
This library decodes CGI formatted messages from FastCGI and presents a set of objects that can be accessed

* Query string, uri, method
* ENVP variables
* Query string, both full original, and decoded urldecoded name value pairs
* Post fields - supports both urlencoded and multipart submissions
* Files - Uploaded files are recorded as well with both post fields, filenames, and if necessary base64 decoding
* Access to raw post data for JSON/RPC, etc..

# Requirements
Requires the fast cgi developer library and C++14, standard GNU build process

# Build
* autoreconf -fi
* ./configure <options>
* make
* make install

# Usage
In user code, one only need to 
`#include <fcgi_request_cpp.hxx>`
and link against the generated library, and -lfcgi
See example programs under the examples subdirectory for examples

# Documentation
Documentation can be found [here](https://www.beneschtech.com/doc/fcgi_request_cpp/)

# Targets
* Developed on Debian 12 (Bookworm) - Passing
* Tested build on FreeBSD - Passing

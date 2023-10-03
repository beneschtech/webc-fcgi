webc-fcgi - A C++ wrapper and implementation to present fastcgi requests in an object oriented fashion

This library decodes CGI formatted messages from FastCGI and presents a set of objects that can be accessed
Query string, uri, method
ENVP variables
Query string, both full original, and decoded urldecoded name value pairs
Post fields - supports both urlencoded and multipart submissions
Files - Uploaded files are recorded as well with both post fields, filenames, and if necessary base64 decoding

Access to ppost data for JSON/RPC, etc..

Requires the fast cgi developer library and C++14, standard GNU build process

* autoreconf -fi
** ./configure <options>
** make
** make install

In user code, one only need to include fcgi_request_cpp.hxx and link against the generated library, and -lfcgi

Developed on Debian 12 (Bookworm)

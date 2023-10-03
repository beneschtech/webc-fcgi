
AM_CXXFLAGS = -I${abs_top_srcdir} \
        -I${abs_top_srcdir}/src/include \
        ${FCGI_CFLAGS}
lib_LTLIBRARIES = libfcgi_request.la
libfcgi_request_la_SOURCES = fcgi_listener.cpp \
        fcgi_request.cpp \
        fcgi_data.cpp \
        fcgi_req_parser.cpp \
        fcgi_response.cpp \
        httpcodes.cpp \
        urlencode.cpp \
        base64.cpp \
        multipart.cpp \
        util.cpp

libfcgi_request_la_LIBADD = ${FCGI_LIBS}

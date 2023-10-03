AM_CXXFLAGS = -I${abs_top_srcdir} -I${abs_top_srcdir}/src/include ${FCGI_CFLAGS}
noinst_PROGRAMS=pingpong httpecho

pingpong_SOURCES=pingpong.cpp
pingpong_LDADD=${abs_top_builddir}/src/lib/libfcgi_request.la ${FCGI_LIBS}

httpecho_SOURCES=httpecho.cpp
httpecho_LDADD=${abs_top_builddir}/src/lib/libfcgi_request.la ${FCGI_LIBS}

# This encapsulates all of the results from
# autoscan into one place, which makes configure.ac
# a lot easier to read
AC_DEFUN([SCANRES],
[

AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_FUNC_STRERROR_R 
AC_TYPE_MODE_T

AC_CHECK_HEADERS([cstring])
AC_CHECK_HEADERS([string])
AC_CHECK_HEADERS([iostream])
AC_CHECK_HEADERS([memory])
AC_CHECK_HEADERS([thread])
AC_CHECK_HEADERS([exception])
AC_CHECK_HEADERS([mutex])
AC_CHECK_HEADERS([vector])
AC_CHECK_HEADERS([stdio.h])
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([errno.h])
AC_CHECK_HEADERS([pthread.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([sys/socket.h])
AC_CHECK_HEADERS([sys/syscall.h])
AC_CHECK_HEADERS([sys/types.h])
AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([strings.h])

AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([socket]) 
AC_CHECK_FUNCS([strerror])
AC_CHECK_FUNCS([strtol])
AC_CHECK_FUNCS([strtoul])

])

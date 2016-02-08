#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([sciviewer], [0.5], [dg.briseno@gmail.com])
AM_INIT_AUTOMAKE
#AC_CONFIG_SRCDIR([src/raster.c])
#AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX

# Checks for libraries.
AC_CHECK_LIB([GL], [glClear])
AC_CHECK_LIB([X11], [XCreateWindow])
AC_CHECK_LIB([dl], [dl])
AC_CHECK_LIB([pthread], [pthread_create],, [AC_MSG_ERROR([
                You are missing pthreads
              ])])
AC_CHECK_LIB([Xxf86vm], [XF86VidModeQueryExtension],, [AC_MSG_ERROR([You are missing Xf86vm required by glfw3])])
AC_CHECK_LIB([m], [pow])
AC_CHECK_LIB([Xrandr], [XRRFreeGamma],, [AC_MSG_ERROR([ Missing Xrandr])])
AC_CHECK_LIB([GLEW], [__glewEnableVertexAttribArray],, [AC_MSG_ERROR([ Missing GLEW library required for OpenGL])])
AC_CHECK_LIB([Xi], [XIQueryVersion],, [AC_MSG_ERROR([ Missing Xi])])
AC_CHECK_LIB([Xinerama], [XineramaIsActive],, [AC_MSG_ERROR([ Missing Xinerama])])
AC_CHECK_LIB([Xcursor], [XcursorImageCreate],, [AC_MSG_ERROR([ Missing Xcursor])])

#AC_CHECK_LIB([glfw3], [glfwGetWindowSize], echo, printf "\n\tmissing required library glfw3\n\n"; exit)
PKG_CHECK_MODULES([GLFW3], [glfw3],, [AC_MSG_ERROR([
                   The glfw3 library was not found, which is needed for opengl support. 
                     ])])
PKG_CHECK_MODULES([GDAL], [gdal],, [AC_MSG_ERROR([
                   missing GDAL library needed for image processing.
                     ])])
# FIXME: Replace `main' with a function in `-lrt':
#AC_CHECK_LIB([rt], [main])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC

AC_OUTPUT(Makefile src/Makefile)

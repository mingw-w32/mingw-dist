# configure.ac -*- autoconf -*- vim: filetype=config
#
# $Id$
#
# Written by Keith Marshall  <keithmarshall@users.sourceforge.net>
# Copyright (C) 2013, MinGW.org Project
#
#
# Configuration script for mingw-dist validation suite
#
#
# This is free software.  Permission is granted to copy, modify and
# redistribute this software, under the provisions of the GNU General
# Public License, Version 3, (or, at your option, any later version),
# as published by the Free Software Foundation; see the file COPYING
# for licensing details.
#
# Note, in particular, that this software is provided "as is", in the
# hope that it may prove useful, but WITHOUT WARRANTY OF ANY KIND; not
# even an implied WARRANTY OF MERCHANTABILITY, nor of FITNESS FOR ANY
# PARTICULAR PURPOSE.  Under no circumstances will the author, or the
# MinGW Project, accept liability for any damages, however caused,
# arising from the use of this software.
#
  m4_include([../VERSION.m4])
  AC_INIT([mingw-dist-tests],__VERSION__,[http://mingw.org/reporting_bugs])
#
# Building the test suite driver program requires a C++ compiler,
# (which must emit binary code to run on the build platform).
#
  AC_PROG_CXX
#
# The test suite is heavily dependent on the Xerces-C++ SDK; check, to
# ensure that this is installed.
#
  AC_CHECK_LIB([xerces-c],[main])
  AS_IF([test x"$ac_cv_lib_xerces_c_main" = xyes],dnl
  [XERCES_C_TOOLS=xerces-c-tools],[XERCES_C_TOOLS=no-xerces-c-tools])dnl
  AC_SUBST([XERCES_C_TOOLS])
#
# Configure output comprises a simple makefile.
#
  AC_CONFIG_FILES([Makefile])
  AC_OUTPUT
#
# $RCSfile$: end of file

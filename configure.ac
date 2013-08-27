# configure.ac -*- autoconf -*- vim: filetype=config
#
# $Id$
#
# Written by Keith Marshall  <keithmarshall@users.sourceforge.net>
# Copyright (C) 2010, 2011, 2013, MinGW.org Project
#
#
# Configuration script for mingw-dist
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
  AC_INIT([mingw-dist],[2.2],[http://mingw.org/reporting_bugs])

# Specify a default (invalid) user name, for authentication to FRS.
#
  : ${FRS_USERNAME="anonymous"}
  AC_ARG_VAR([FRS_USERNAME],[user name for FRS authentication [anonymous]])

# Check for required tools.
#
  m4_include([makeopts.m4])
  MINGW_AC_MAKE_NO_PRINT_DIRECTORY
  AC_PROG_LN_S

# Specify all makefiles which are to be created
#
  AC_CONFIG_FILES([Makefile])
  AC_CONFIG_FILES([Makefile.comm])
  AC_FOREACH([subdir],[common contrib mingw32 msys],
  [AC_CONFIG_FILES(subdir[/Makefile:Makefile.stub.in])dnl
   mingw_ac_subdirs=${mingw_ac_subdirs+"${mingw_ac_subdirs} "}"subdir"dnl
  ])

# Specify the subdirectories to be managed, derived from the AC_FOREACH
# list of subdirectories in which makefiles have been specified
#
  AC_SUBST([mingw_ac_subdirs])

# Create all makefiles
#
  AC_OUTPUT
#
# $RCSfile$: end of file

## makeopts.m4 -*- autoconf -*- vim: filetype=config
##
## $Id$
##
## Written by Keith Marshall <keithmarshall@users.sourceforge.net>
## Copyright (C) 2013, MinGW.org Project
##
##
## Autoconf macros to check for options supported by 'make'.
##
##
## This is free software.  Permission is hereby granted to copy
## and redistribute this software, either as is or in modified form,
## subject only to the restrictions that the original author's notice
## of copyright and disclaimers of warranty and of liability shall be
## preserved without change in EVERY copy, and that modified copies
## shall be clearly identified as such.
##
## This software is provided "as is", in the hope that it may prove
## useful, but there is NO WARRANTY OF ANY KIND; not even an implied
## WARRANTY OF MERCHANTABILITY, nor of FITNESS FOR A PARTICULAR PURPOSE.
## Under no circumstances will the author, or the MinGW Project, accept
## liability for any damages, however caused, arising from the use of
## this software.


# MINGW_AC_MAKE_OPTION_SUPPORTED( VARNAME, OPTION )
# -------------------------------------------------
# If OPTION is supported by make, set VARNAME=' OPTION', otherwise
# set VARNAME to nothing; in either case, call AC_SUBST for VARNAME.
#
AC_DEFUN([MINGW_AC_MAKE_OPTION_SUPPORTED],
[AC_REQUIRE([AC_PROG_MAKE_SET])dnl
 AC_MSG_CHECKING([whether make supports the $2 option])
 TAB='	' $1=; mkdir conftest.dir
 cat <<-EOF> conftest.dir/Makefile
	conftest:
	${TAB}@\$(MAKE) $2 conftest-recursive

	conftest-recursive:
	${TAB}@true
	EOF
 ( cd conftest.dir; make >/dev/null 2>&1 ) && ac_val=yes $1=' $2' || ac_val=no
 rm -rf conftest.dir
 AC_MSG_RESULT([$ac_val])dnl
 AC_SUBST([$1])dnl
])# MINGW_AC_MAKE_OPTION_SUPPORTED

# MINGW_AC_MAKE_NO_PRINT_DIRECTORY
# --------------------------------
# Assign NO_PRINT_DIRECTORY=" --no-print-directory", if make supports
# the --no-print-directory option, otherwise leave NO_PRINT_DIRECTORY
# unassigned; in either case, call AC_SUBST for NO_PRINT_DIRECTORY.
#
AC_DEFUN([MINGW_AC_MAKE_NO_PRINT_DIRECTORY],
[MINGW_AC_MAKE_OPTION_SUPPORTED([NO_PRINT_DIRECTORY],[--no-print-directory])dnl
])# MINGW_AC_MAKE_NO_PRINT_DIRECTORY

# $RCSfile$: end of file

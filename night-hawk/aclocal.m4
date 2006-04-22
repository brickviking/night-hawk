dnl aclocal.m4 generated automatically by aclocal 1.3

dnl Copyright (C) 1994, 1995, 1996, 1997, 1998 Free Software Foundation, Inc.
dnl This Makefile.in is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.


AC_DEFUN(ACWS_PRE_ENV,
[ifelse($#, 0, , $#, 1, , [$2="$1 $$2"])])
AC_DEFUN(ACWS_PRE_ENV_MULTI,
[ifelse($#, 0,, $#, 1,, $#, 2, [ACWS_PRE_ENV([$1], [$2])],
[ACWS_PRE_ENV([$1], [$2])
ACWS_PRE_ENV_MULTI([$1], builtin(shift, builtin(shift, $@)))])])

AC_DEFUN(ACWS_POST_ENV,
[ifelse($#, 0, , $#, 1, , [$2="$$2 $1"])])
AC_DEFUN(ACWS_POST_ENV_MULTI,
[ifelse($#, 0,, $#, 1,, $#, 2, [ACWS_POST_ENV([$1], [$2])],
[ACWS_POST_ENV([$1], [$2])
ACWS_POST_ENV_MULTI([$1], builtin(shift, builtin(shift, $@)))])])


AC_DEFUN([ACWS_ENABLE_CXX],
[AC_PROVIDE([ACWS_DISABLE_CXX])
AC_ARG_ENABLE(cxx,
[  --disable-cxx           do not check for C++ and do not use it],
[if test x"$enableval" = xyes; then :; else
  enable_cxx="no";
fi],
[enable_cxx="yes";])
])

AC_DEFUN([ACWS_DISABLE_CXX],
[AC_PROVIDE([ACWS_ENABLE_CXX])
AC_ARG_ENABLE(cxx,
[  --enable-cxx           check for C++ and use it],
[if test x"$enableval" = xyes; then :; else
  enable_cxx="no";
fi],
[enable_cxx="no";])
])


AC_DEFUN(ACWS_CHECK_CXX,
[
ifelse($1,[enabled],[AC_REQUIRE([ACWS_ENABLE_CXX])],[AC_REQUIRE([ACWS_DISABLE_CXX])])
if test x"$enable_cxx" = xyes; then
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_PROG_CXX])
  AC_SUBST(CXXINCLUDES)

  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  if test "$cross_compiling" = no; then
    AC_MSG_CHECKING([that C++ compiler can compile simple program])
  fi
  AC_TRY_RUN([int main() { return 0; }],
    AC_MSG_RESULT(yes),
    AC_MSG_RESULT(no)
    AC_MSG_WARN([a working C++ compiler is required])
    enable_cxx=no
    AC_MSG_RESULT("disabling C++"),:)
  if test x"$enable_cxx" = xyes; then
    if test "$cross_compiling" = no; then
      AC_MSG_CHECKING([that C++ static constructors and destructors are called])
    fi
    AC_TRY_RUN(
      [extern "C" { void _exit(int); }
      int i;
      struct A {
        char dummy;
        A() { i = 1; }
        ~A() { if (i == 1) _exit(0); } };
      A a;
      int main() { exit (1); }],
      AC_MSG_RESULT(yes),
      AC_MSG_RESULT(no)
      AC_MSG_WARN([a working C++ compiler is required. disabling C++])
      enable_cxx=no,:)
    # no need for other tests, if C++ disabled
    if test x"$enable_cxx" = xyes; then
      AC_CACHE_CHECK([that header files support C++],[acws_cv_headers_support_cxx],
        AC_TRY_LINK([#include <stdio.h>],
          [fopen(0, 0);],
          [acws_cv_headers_support_cxx=yes],
          [acws_cv_headers_support_cxx=no]))
      if test x"$acws_cv_headers_support_cxx" = xyes; then :; else
        AC_MSG_WARN([header files do not support C++])
        AC_MSG_WARN([(if you are using a version of gcc/g++ earlier than 2.5, you should install libg++)])
        enable_cxx=no
      fi
    fi
  fi
  AC_LANG_RESTORE
else
  AC_MSG_RESULT("skipping C++ checks")
fi])


AC_DEFUN(ACWS_CHECK_LIB,
[acws_chk_lib="$1"
AC_CHECK_LIB([$acws_chk_lib], $2, [ACWS_PRE_ENV_MULTI([-l$acws_chk_lib], $3)],, [$4])])


AC_DEFUN(ACWS_SET_XLDFLAGS,
[
dnl **************************************************
dnl temporarily add -L$x_libraries to LDFLAGS
[acws_save_LDFLAGS="$LDFLAGS"
# this is for systems (like SGI), 
# that do not need any special flags for 
# X includes and libraries!
if test notset = notset${x_libraries} ; then
  LDFLAGS="$LDFLAGS"
else
  LDFLAGS="$LDFLAGS -L$x_libraries"
  case $host_os in
  solaris*)
    if test  x"$ac_R_space" = xyes; then
      LDFLAGS="$LDFLAGS -R $x_libraries"
    else
      LDFLAGS="$LDFLAGS -R$x_libraries"
    fi;;
  esac
fi
LDFLAGS="$XM_LDFLAGS $LDFLAGS"
]])


AC_DEFUN(ACWS_RST_LDFLAGS,
[
dnl **************************************************
dnl restore LDFLAGS
[LDFLAGS="$acws_save_LDFLAGS"]])


AC_DEFUN(ACWS_CHECK_LIBS_X,
[
AC_REQUIRE([AC_PATH_XTRA])
dnl --------------------------------------------------
dnl make substitutions in makefile.in
dnl --------------------------------------------------
AC_SUBST(X_PLAIN_LIBS)dnl 
AC_SUBST(X_ALL_LIBS)dnl 
case "$host_os" in
  hpux9.05)
    if test x"$x_includes" = x/usr/local/X11R5/include; then
      if test -d /usr/include/X11R5; then
        x_includes=/usr/include/X11R5
	X_CFLAGS="-I$x_includes"
      fi
    fi
    if test x"$x_libraries" = x/usr/local/X11R5/lib; then
      if test -d /usr/lib/X11R5; then
        x_libraries=/usr/lib/X11R5
	X_LIBS="-L $x_libraries"
      fi
    fi
    ;;
  sunos4*)
    if test x$x_includes = x; then
      if test -d /usr/X11R5/include; then
        x_includes=/usr/X11R5/include
	X_CFLAGS="-I$x_includes"
      fi
    fi
    ;;
esac

dnl --------------------------------------------------
dnl Check for X libraries
dnl X_EXTRA_LIBS is set by [AC_PATH_XTRA]
dnl X_PRE_LIBS is set by [AC_PATH_XTRA]
dnl this was probably checked before
dnl --------------------------------------------------
ACWS_SET_XLDFLAGS
X_PLAIN_LIBS="$X_EXTRA_LIBS"
ACWS_PRE_ENV_MULTI([$X_PLAIN_LIBS], X_EXT_LIBS, X_XT_LIBS, X_XMU_LIBS, X_XM_LIBS, X_ALL_LIBS)
ACWS_CHECK_LIB(X11, XAllocColorCells, [X_PLAIN_LIBS, X_EXT_LIBS, X_XT_LIBS, X_XMU_LIBS, X_XM_LIBS, X_ALL_LIBS], [$X_PLAIN_LIBS])
ACWS_PRE_ENV_MULTI([$X_PRE_LIBS], X_PLAIN_LIBS, X_EXT_LIBS, X_XT_LIBS, X_XMU_LIBS, X_XM_LIBS, X_ALL_LIBS)
ACWS_RST_LDFLAGS
])


AC_DEFUN(ACWS_CHECK_LIBS_XEXT,
[
AC_REQUIRE([ACWS_CHECK_LIBS_X])
dnl **************************************************
dnl make substitutions in makefile.in
AC_SUBST(X_EXT_LIBS)dnl 
dnl **************************************************
dnl check for extended libraries
ACWS_SET_XLDFLAGS
case $host_os in # (
  irix*)
    ACWS_CHECK_LIB(Xext, XShapeCombineMask, [X_EXT_LIBS, X_XMU_LIBS, X_ALL_LIBS], [$X_PLAIN_LIBS])
    ACWS_CHECK_LIB(Xpm, XpmCreateImageFromData, [X_EXT_LIBS, X_ALL_LIBS], [$X_PLAIN_LIBS])
    ;; # (
  solaris*)
    ACWS_CHECK_LIB(Xext, XShapeCombineMask, [X_EXT_LIBS, X_XMU_LIBS, X_ALL_LIBS], [$X_PLAIN_LIBS])
    ACWS_CHECK_LIB(Xpm, XpmCreateImageFromData, [X_EXT_LIBS, X_ALL_LIBS], [$X_PLAIN_LIBS])
    ;; # (
  *)
    ACWS_CHECK_LIB(Xext, XShapeCombineMask, [X_EXT_LIBS, X_XMU_LIBS, X_XM_LIBS, X_ALL_LIBS], [$X_PLAIN_LIBS])
    ACWS_CHECK_LIB(Xpm, XpmCreateImageFromData, [X_EXT_LIBS, X_XM_LIBS, X_ALL_LIBS], [$X_PLAIN_LIBS])
esac
ACWS_RST_LDFLAGS
])


AC_DEFUN([ACWS_WITHOUT_SCORES],
[AC_PROVIDE([ACWS_WITH_SCORES])
AC_ARG_WITH(scores,
[  --with-scores=path      use path as scores file.
                          if not absolute, \$HOME is prepended.],
[if test -n "$withval"; then
    if test x"$withval" = xyes; then
       with_scores="no";
       AC_MSG_ERROR(option \`--with-scores' needs a value.)
    fi
else
  with_scores="no";
fi],
[with_scores="no";])
])

AC_DEFUN([ACWS_DISABLE_SLOWDEFS],
[AC_PROVIDE([ACWS_ENABLE_SLOWDEFS])
AC_ARG_ENABLE(slowdefs,
[  --enable-slowdefs       compile with slow defaults:
                          reduced sprites, mono, 8-bit, 11025Hz],
[if test x"$enableval" = xyes; then :; else
  enable_slowdefs="no";
fi],
[enable_slowdefs="no"])
if test x"$enable_slowdefs" = xyes; then
   enable_red_sprites=yes
   enable_mono=yes
   enable_8bit=yes
   with_sam_rate=11025
fi
])

AC_DEFUN([ACWS_DISABLE_RED_SPRITES],
[AC_PROVIDE([ACWS_ENABLE_RED_SPRITES])
AC_ARG_ENABLE(red_sprites,
[  --enable-red-sprites    compile nighthawk with reduced sprites.],
[if test x"$enableval" = xyes; then :; else
  enable_red_sprites="no";
fi],
[enable_red_sprites="no";])
])

AC_DEFUN([ACWS_DISABLE_SOUND],
[AC_PROVIDE([ACWS_ENABLE_SOUND])
AC_ARG_ENABLE(sound,
[  --enable-sound          compile with sound support.
                          automatically enabled on i?86 machines.],
[if test x"$enableval" = xyes; then :; else
  enable_sound="no";
fi],
[case "$host_cpu" in
     i?86) enable_sound=yes;;
     *) enable_sound="no";;
esac])
])

AC_DEFUN([ACWS_DISABLE_MONO],
[AC_PROVIDE([ACWS_ENABLE_MONO])
AC_ARG_ENABLE(mono,
[  --enable-mono           compile with mono sound as default.],
[if test x"$enableval" = xyes; then :; else
  enable_mono="no";
fi],
[enable_mono="no";])
])

AC_DEFUN([ACWS_DISABLE_8BIT],
[AC_PROVIDE([ACWS_ENABLE_8BIT])
AC_ARG_ENABLE(8bit,
[  --enable-8bit           use 8bit sound samples as default.],
[if test x"$enableval" = xyes; then :; else
  enable_8bit="no";
fi],
[enable_8bit="no";])
])

AC_DEFUN([ACWS_WITHOUT_SAM_RATE],
[AC_PROVIDE([ACWS_WITH_SAM_RATE])
AC_ARG_WITH(sam_rate,
[  --with-sam-rate=rate    compile with sample rate as default.
                          22050Hz, when option not given.],
[if test -n "$withval"; then
    if test x"$withval" = xyes; then
       with_sam_rate="no";
       AC_MSG_ERROR(option \`--with-sam-rate' needs a value.)
    fi
else
  with_sam_rate="no";
fi],
[with_sam_rate="no";])
])

AC_DEFUN([ACWS_WITHOUT_FX_CHANNELS],
[AC_PROVIDE([ACWS_WITH_FX_CHANNELS])
AC_ARG_WITH(fx_channels,
[  --with-fx-channels=no   compile with no. of FX channels as default.
                          8, when option not given.],
[if test -n "$withval"; then
    if test x"$withval" = xyes; then
       with_fx_channels="no";
       AC_MSG_ERROR(option \`--with-fx-channels' needs a value.)
    fi
else
  with_fx_channels="no";
fi],
[with_fx_channels="no";])
])

AC_DEFUN([ACWS_DISABLE_TOOLS],
[AC_PROVIDE([ACWS_ENABLE_TOOLS])
AC_ARG_ENABLE(tools,
[  --enable-tools          compile nighthawk tools.],
[if test x"$enableval" = xyes; then :; else
  enable_tools="no";
fi],
[enable_tools="no";])
])


# Do all the work for Automake.  This macro actually does too much --
# some checks are only needed if your package does certain things.
# But this isn't really a big deal.

# serial 1

dnl Usage:
dnl AM_INIT_AUTOMAKE(package,version, [no-define])

AC_DEFUN(AM_INIT_AUTOMAKE,
[AC_REQUIRE([AM_PROG_INSTALL])
PACKAGE=[$1]
AC_SUBST(PACKAGE)
VERSION=[$2]
AC_SUBST(VERSION)
dnl test to see if srcdir already configured
if test "`cd $srcdir && pwd`" != "`pwd`" && test -f $srcdir/config.status; then
  AC_MSG_ERROR([source directory already configured; run "make distclean" there first])
fi
ifelse([$3],,
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE")
AC_DEFINE_UNQUOTED(VERSION, "$VERSION"))
AC_REQUIRE([AM_SANITY_CHECK])
AC_REQUIRE([AC_ARG_PROGRAM])
dnl FIXME This is truly gross.
missing_dir=`cd $ac_aux_dir && pwd`
AM_MISSING_PROG(ACLOCAL, aclocal, $missing_dir)
AM_MISSING_PROG(AUTOCONF, autoconf, $missing_dir)
AM_MISSING_PROG(AUTOMAKE, automake, $missing_dir)
AM_MISSING_PROG(AUTOHEADER, autoheader, $missing_dir)
AM_MISSING_PROG(MAKEINFO, makeinfo, $missing_dir)
AC_REQUIRE([AC_PROG_MAKE_SET])])


# serial 1

AC_DEFUN(AM_PROG_INSTALL,
[AC_REQUIRE([AC_PROG_INSTALL])
test -z "$INSTALL_SCRIPT" && INSTALL_SCRIPT='${INSTALL_PROGRAM}'
AC_SUBST(INSTALL_SCRIPT)dnl
])

#
# Check to make sure that the build environment is sane.
#

AC_DEFUN(AM_SANITY_CHECK,
[AC_MSG_CHECKING([whether build environment is sane])
# Just in case
sleep 1
echo timestamp > conftestfile
# Do `set' in a subshell so we don't clobber the current shell's
# arguments.  Must try -L first in case configure is actually a
# symlink; some systems play weird games with the mod time of symlinks
# (eg FreeBSD returns the mod time of the symlink's containing
# directory).
if (
   set X `ls -Lt $srcdir/configure conftestfile 2> /dev/null`
   if test "[$]*" = "X"; then
      # -L didn't work.
      set X `ls -t $srcdir/configure conftestfile`
   fi
   if test "[$]*" != "X $srcdir/configure conftestfile" \
      && test "[$]*" != "X conftestfile $srcdir/configure"; then

      # If neither matched, then we have a broken ls.  This can happen
      # if, for instance, CONFIG_SHELL is bash and it inherits a
      # broken ls alias from the environment.  This has actually
      # happened.  Such a system could not be considered "sane".
      AC_MSG_ERROR([ls -t appears to fail.  Make sure there is not a broken
alias in your environment])
   fi

   test "[$]2" = conftestfile
   )
then
   # Ok.
   :
else
   AC_MSG_ERROR([newly created file is older than distributed files!
Check your system clock])
fi
rm -f conftest*
AC_MSG_RESULT(yes)])

dnl AM_MISSING_PROG(NAME, PROGRAM, DIRECTORY)
dnl The program must properly implement --version.
AC_DEFUN(AM_MISSING_PROG,
[AC_MSG_CHECKING(for working $2)
# Run test in a subshell; some versions of sh will print an error if
# an executable is not found, even if stderr is redirected.
# Redirect stdin to placate older versions of autoconf.  Sigh.
if ($2 --version) < /dev/null > /dev/null 2>&1; then
   $1=$2
   AC_MSG_RESULT(found)
else
   $1="$3/missing $2"
   AC_MSG_RESULT(missing)
fi
AC_SUBST($1)])

# Like AC_CONFIG_HEADER, but automatically create stamp file.

AC_DEFUN(AM_CONFIG_HEADER,
[AC_PREREQ([2.12])
AC_CONFIG_HEADER([$1])
dnl When config.status generates a header, we must update the stamp-h file.
dnl This file resides in the same directory as the config header
dnl that is generated.  We must strip everything past the first ":",
dnl and everything past the last "/".
AC_OUTPUT_COMMANDS(changequote(<<,>>)dnl
ifelse(patsubst(<<$1>>, <<[^ ]>>, <<>>), <<>>,
<<test -z "<<$>>CONFIG_HEADERS" || echo timestamp > patsubst(<<$1>>, <<^\([^:]*/\)?.*>>, <<\1>>)stamp-h<<>>dnl>>,
<<am_indx=1
for am_file in <<$1>>; do
  case " <<$>>CONFIG_HEADERS " in
  *" <<$>>am_file "*<<)>>
    echo timestamp > `echo <<$>>am_file | sed -e 's%:.*%%' -e 's%[^/]*$%%'`stamp-h$am_indx
    ;;
  esac
  am_indx=`expr "<<$>>am_indx" + 1`
done<<>>dnl>>)
changequote([,]))])


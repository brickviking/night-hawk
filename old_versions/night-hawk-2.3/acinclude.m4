## --------------------------------------------------
## macros to add something to environment variables
## --------------------------------------------------

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

## --------------------------------------------------
## CXX
## --------------------------------------------------

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

##
## usage: ACWS_CHECK_CXX
##        ACWS_CHECK_CXX(enabled)

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

## --------------------------------------------------
## check for a library and add it to environment variables
## --------------------------------------------------
##
## ACWS_CHECK_LIB(lib, libfunc, env-variables, additional-libs)
##

AC_DEFUN(ACWS_CHECK_LIB,
[acws_chk_lib="$1"
AC_CHECK_LIB([$acws_chk_lib], $2, [ACWS_PRE_ENV_MULTI([-l$acws_chk_lib], $3)],, [$4])])

## --------------------------------------------------
## ACWS_SET_XLDFLAGS
## --------------------------------------------------
##
## ACWS_SET_XLDFLAGS
## save current LDFLAGS and set them for compiling X11 programs

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

## --------------------------------------------------
## ACWS_RST_LDFLAGS
## --------------------------------------------------
##
## ACWS_RST_LDFLAGS
## restore LDFLAGS

AC_DEFUN(ACWS_RST_LDFLAGS,
[
dnl **************************************************
dnl restore LDFLAGS
[LDFLAGS="$acws_save_LDFLAGS"]])

## --------------------------------------------------
## ACWS_CHECK_LIBS_X
## --------------------------------------------------
##
## ACWS_CHECK_LIBS_X
##
## Locates all required libraries to compile plain X11 programs
## substitutes the following @var@ variables in Makefile.in:
##
## X_PLAIN_LIBS  for plain X11 programs
## X_ALL_LIBS     all X libs found.
##
## Augments the following X_???_LIBS variables:
##
## X_EXT_LIBS   X Library extensions (incl. Xpm)
## X_XT_LIBS     X Toolkit libraries
## X_XMU_LIBS    Xmu libraries
## X_XM_LIBS     X Motif libraries

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

## --------------------------------------------------
## ACWS_CHECK_LIBS_XEXT
## --------------------------------------------------
##
## ACWS_CHECK_LIBS_XEXT
##
## locates all required libraries to compile X extended programs
## substitutes the following @var@ variables in Makefile.in:
##
## the ones from ACWS_CHECK_LIBS_X plus:
##
## X_EXT_LIBS

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

##
## this is special for nighthawk
##

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


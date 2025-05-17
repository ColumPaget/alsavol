AC_INIT([alsavol],[1.0])


dnl define a function we can use to check if a compiler flag
dnl is supported by the compiler
AC_DEFUN([CHECK_COMPILER_OPT],
[
          AC_MSG_CHECKING([checking if compiler supports $1])
                OLD_CFLAGS="$CFLAGS"
                CFLAGS="$CFLAGS $1"
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM(test(){return 1;})], [COMP_OPT_SUPPORT="yes"], [COMP_OPT_SUPPORT="no"])
    AC_MSG_RESULT([$COMP_OPT_SUPPORT])
                AS_IF([test "$COMP_OPT_SUPPORT" != "yes"], [CFLAGS="$OLD_CFLAGS"])
])

AC_PROG_CC
AC_LANG([C])
AC_PROG_MAKE_SET
m4_warn([obsolete],
[The preprocessor macro `STDC_HEADERS' is obsolete.
  Except in unusual embedded environments, you can safely include all
  ISO C90 headers unconditionally.])dnl
# Autoupdate added the next two lines to ensure that your configure
# script's behavior did not change.  They are probably safe to remove.
AC_CHECK_INCLUDES_DEFAULT
AC_PROG_EGREP

AC_SYS_LARGEFILE


AC_CHECK_LIB([asound], [snd_ctl_open])


AC_ARG_WITH(libuseful, [  --with-libuseful		use system-wide libUseful (default=auto) ], cf_system_libuseful=$withval )
AC_ARG_ENABLE(harden, [  --enable-harden 		use compiler options to harden code (default=auto) ], cf_use_harden=$enableval )



AS_IF([test "$cf_use_harden" = "yes"],
[
        CHECK_COMPILER_OPT("-fstack-protector-strong")
        CHECK_COMPILER_OPT("-ftrivial-auto-var-init=zero")
        CHECK_COMPILER_OPT("-fstack-clash-protection")
        CHECK_COMPILER_OPT("-fno-strict-overflow")
        CHECK_COMPILER_OPT("-fno-strict-aliasing")
        CHECK_COMPILER_OPT("-fno-delete-null-pointer-checks")
        CHECK_COMPILER_OPT("-fcf-protection=full")
])



if test "$cf_system_libuseful" != "no"
then
	AC_CHECK_LIB([Useful-5],GetToken,[cf_have_libUseful5=yes],[cf_have_libUseful5=no])
fi

if test "$cf_have_libUseful5" = "yes" 
then
	AC_CHECK_HEADERS([libUseful-5/libUseful.h])
  echo "installed libUseful-5 found, using that instead of bundled libUseful."
  echo "if you want to use bundled libUseful, force it with --with-libuseful=no"
  AC_SUBST(LIBUSEFUL_BUNDLED,"")
  dnl Default behavior of AC_CHECK_LIB is to add the library to the LIBS variable
  dnl previous AC_CHECK_LIB don't do this, because an action is supplied for them
  AC_CHECK_LIB([Useful-5],GetToken)
else
  AC_DEFINE([USE_LIBUSEFUL_BUNDLED])
  AC_SUBST(LIBUSEFUL_BUNDLED,"libuseful-bundled/libUseful.a")
  AC_CONFIG_SUBDIRS([libuseful-bundled])
fi

dnl read Makefile.in and write Makefile
AC_CONFIG_FILES([Makefile])
AC_OUTPUT


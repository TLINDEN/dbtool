# -*-sh-*-

AC_PREREQ(2.61)

define([dbtoolversion], esyscmd([sh -c "cat VERSION|head -1"]))

# what we are compiling
AC_INIT([dbtool], [dbtoolversion], [tlinden@cpan.org])

AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(config)
AC_CONFIG_HEADER(platform.h)

AM_INIT_AUTOMAKE()
LT_INIT

# default values, dynamically linking and stripped binary
STATIC="no"


# provide flag --enable-static
AC_ARG_ENABLE([static-binary],
    AS_HELP_STRING([--enable-static-binary], [create a statically linked binary]),
    [
	if test "x$enableval" = "xyes"; then
	    STATIC="yes"
	    LDFLAGS="$LDFLAGS -static"
	fi
])

AC_ARG_ENABLE([debug],
    AS_HELP_STRING([--disable-debug], [Disable debugging]))
AS_IF([test "x$enable_debug" != "xno"], [
          CXXFLAGS="$CFLAGS -g -DDEBUG"
          enable_debug="yes"
])


AC_CANONICAL_HOST


# look for the existence of install
AC_PROG_INSTALL

# look if c++ compiler exists
AC_PROG_CXX

# look for libc
AC_HEADER_STDC

# additional headers which are required
AC_CHECK_HEADERS(unistd.h stdio.h stdlib.h)


# Check for some target-specific stuff
case "$host" in
    *aix*)
        # libm is required as well
        CXXFLAGS="$CFLAGS -D_AIX_SOURCE=1"
        LDFLAGS="$LDFLAGS -lm"
        ;;
    *-*-android*) ;;
    *-*-cygwin*) ;;
    *-*-dgux*) ;;
    *-*-darwin*) ;;
    *-*-dragonfly*) ;;
    *-*-haiku*) ;;
    *-*-hpux*) ;;
    *-*-irix5*) ;;
    *-*-irix6*) ;;
    *-*-k*bsd*-gnu | *-*-kopensolaris*-gnu) ;;
    *-*-linux*) ;;
    *-*-netbsd*) ;;
    *-*-freebsd*)
        # ports install to /usr/local by default, check
        if test -d "/usr/local/lib" -a -d "/usr/local/include"; then
            CXXFLAGS="$CFLAGS -I/usr/local/include"
            LDFLAGS="$LDFLAGS -L/usr/local/lib"
        fi
        ;;
    *-*-bsdi*) ;;
    *-next-*) ;;
    *-*-openbsd*) ;;
    *-*-solaris*) ;;
    *-*-sunos4*) ;;
    *-ncr-sysv*) ;;
    *-sni-sysv*) ;;
    *-*-sysv4.2*) ;;
    *-*-sysv5*) ;;
    *-*-sysv*) ;;
    *-*-sco*) ;;
    *-*-unicos*) ;;
    *-dec-osf*) ;;
    *-*-nto-qnx*) ;;
    *-*-ultrix*) ;;
    *-*-lynxos) ;;
esac


# look for libs
GDBM="yes"
BERKELEY="no"
PCRE="no"


# configure args
AC_ARG_WITH([berkeley],
    [AS_HELP_STRING([--with-berkeley],
	            [Use berkeley library ])])

if test "x$with_berkeley" = "xno"; then
  BERKELEY=no
  GDBM=yes
elif test "x$with_berkeley" = "xyes"; then
  BERKELEY=yes
  GDBM=no
fi


AC_ARG_WITH([gdbm],
    [AS_HELP_STRING([--with-gdbm],
	            [Use GNU gdbm library ])])

if test "x$with_gdbm" = "xno"; then
  GDBM=no
elif test "x$with_gdbm" = "xyes"; then
  GDBM=yes
fi

AC_ARG_WITH([gdbm-dir],
    [AS_HELP_STRING([--with-gdbm-dir=DIR],
	            [Use GNU gdbm library in DIR])],
    [
	AC_MSG_CHECKING([checking $with_gdbm_dir/include/gdbm.h presence])
	if test -r "$with_gdbm_dir/include/gdbm.h" ; then
	    CXXFLAGS="-I$with_gdbm_dir/include $CXXFLAGS"
	    CFLAGS="$CXXFLAGS"
	    LDFLAGS="-L$with_gdbm_dir/lib $LDFLAGS"
	    LIBS="-lgdbm"
            GDBM=yes
	    AC_MSG_RESULT([yes])
	else
            GDBM=no
	    AC_MSG_RESULT([no])
	fi
    ])

AC_ARG_WITH([pcre-dir],
    [AS_HELP_STRING([--with-pcre-dir=DIR],
	            [Use GNU pcre library in DIR])],
    [
	AC_MSG_CHECKING([checking $with_pcre_dir/include/pcre.h presence])
	if test -r "$with_pcre_dir/include/pcre.h" ; then
	    CXXFLAGS="-I$with_pcre_dir/include $CXXFLAGS"
	    CFLAGS="$CXXFLAGS"
	    LDFLAGS="-L$with_pcre_dir/lib $LDFLAGS"
	    LIBS="-lpcre"
	    AC_MSG_RESULT([yes])
	else
	    AC_MSG_RESULT([no])
	fi
    ])

AC_ARG_WITH([bdb-include-dir],
            [AS_HELP_STRING([--with-bdb-include-dir],
                            [Specify bdb include prefix])],
            [
		    GDBM="no"
		    BERKELEY="yes"
		    CXXFLAGS="-I${with_bdb_include_dir} ${CXXFLAGS}"
		    CFLAGS="$CXXFLAGS"
   	     ],
            [])

AC_ARG_WITH([bdb_lib_dir],
            [AS_HELP_STRING([--with-bdb-lib-dir],
                            [Specify bdb library prefix])],
	    [
		# dont check presence since lib file name unpredictable
		GDBM="no"
		BERKELEY="yes"
		LDFLAGS="-L${with_bdb_lib_dir} ${LDFLAGS}"
            ])


# first, check for gdbm, which is the default
if test "$GDBM" = "yes"; then
    # search for it via pkg-config
    PKG_CHECK_MODULES([GDBM], [gdbm], _gdbm=yes, _gdbm=no)
    if test "x$_gdbm" = "xyes"; then
	AC_SUBST([GDBM_CFLAGS])
	AC_SUBST([GDBM_CXXFLAGS])
	AC_SUBST([GDBM_LIBS])
	GDBM="yes"
    fi
fi

if test "$GDBM" = "yes"; then
     export LDFLAGS
     export LIBS
     export CFLAGS
     AC_CHECK_LIB(gdbm, gdbm_open)
     if test "x$ac_cv_lib_gdbm_gdbm_open" = "xno"; then
	 GDBM="no"
     fi
fi

# now, if we didn't find gdbm or it was disabled, try berkeley db
if test "$GDBM" = "no"; then
    AC_LANG(C++)
     export LDFLAGS
     export LIBS
     export CXXFLAGS
    AX_BERKELEY_DB_CXX([4.0.0],[
	    LDFLAGS="$DB_CXX_LIBS $LDFLAGS"
	    BERKELEY="yes"
	    ],[BERKELEY="no"])
fi



export CFLAGS
export LIBS
export LDFLAGS
AC_CHECK_LIB(pcre, pcre_compile)



# these functions must exist
AC_CHECK_FUNCS(getopt fdopen fgetc getenv getpass)

# complain if the functions checked for above does not exists
if test "$ac_cv_func_getopt" != yes; then
  echo
  echo "*** Warning: function getopt() not found!"
  echo "*** You will not be able to compile dbtool!"
  #exit 1
fi

if test "$ac_cv_func_fdopen" != yes; then
  echo
  echo "*** Warning: function fdopen() not found!"
  echo "*** You will not be able to compile dbtool!"
  #exit 1
fi

if test "$ac_cv_func_fgetc" != yes; then
  echo
  echo "*** Warning: function fgetc() not found!"
  echo "*** You will not be able to compile dbtool!"
  #exit 1
fi

if test "$ac_cv_func_getenv" != yes; then
  echo
  echo "*** Warning: function getenv() not found!"
  echo "*** You will not be able to compile dbtool!"
  #exit 1
fi

if test "$ac_cv_lib_pcre_pcre_compile" != yes; then
  echo
  echo "*** Warning: the PCRE library could not be found!"
  echo "*** You will not be able to compile dbtool!"
  echo "*** Please get a copy from http://www.pcre.org,"
  echo "*** install it and then retry!"
  exit 1
fi

if test "$GDBM" = "yes"; then
    USEDB="GDBM"
elif test "$BERKELEY" = "yes"; then
    USEDB="BERKELEY-DB"
    CXXFLAGS="-DHAVE_BERKELEY=1 $CXXFLAGS"
else
    echo "Neither gdbm or berkeley-db is instaled!"
    exit 1
fi


# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# the default prefix
AC_PREFIX_DEFAULT(/usr/local)

# finaly write out
AC_CONFIG_FILES([Makefile])
AC_OUTPUT


AC_MSG_RESULT([
    Build configured for $PACKAGE $VERSION:
        CXX:                    ${CXX}
        CXXFLAGS:               ${CXXFLAGS}
        LDFLAGS:                ${LDFLAGS}
        LIBS:                   ${LIBS}
        debug:                  ${enable_debug}
        static build:           ${STATIC}

        prefix:                 ${prefix}
        libdir:                 ${libdir}
        includedir:             ${includedir}

        target platform:        ${host}

	db backend:             ${USEDB}
    Type 'make' to build, 'make install' to install.
])


echo

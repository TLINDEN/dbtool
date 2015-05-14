#!/bin/sh

case $1 in
    clean)
	rm -rf .deps *~ config.cache config.status *.o \
	    dbtool Makefile Makefile.in autom4te.cache .libs depcomp install-sh libtool \
	    dbtool.1 missing mkinstalldirs platform.h stamp* \
	    config/[cdilm]* config.cache aclocal.m4 configure
	;;
    *)
	pod2man dbtool.pod \
	    | sed -e 's#\.TH DBTOOL 1 "....-..-.." "perl v.\..\.." "User Contributed Perl Documentation"#\.TH DBTOOL 1#g' > dbtool.1
	mkdir -p config
	autoreconf --install --force --verbose -I config
	;;
esac

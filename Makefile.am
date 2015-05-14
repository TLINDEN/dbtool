ACLOCAL_AMFLAGS = -I config
bin_PROGRAMS = dbtool
dbtool_SOURCES = cipher.cc  config.cc  dbtool.cc  digest.cc  engine.cc  rijndael.cc
man_MANS = dbtool.1
EXTRA_DIST = $(man_MANS)

clean:
	rm -rf autom4te.cache dbtool *.o *~ config.log config.status Makefile .deps


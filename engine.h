/*
 * 'dbtool' is a simple but powerful  commandline interface to the
 * GNU gdbm system (or, alternatively the Berkeley DB), useful for
 * shellscripts which needs a database for data storage.
 */

/*
 *
 *  This file  is part of the DBTOOL program.
 *
 *  By  accessing  this software,  DBTOOL, you  are  duly informed
 *  of and agree to be  bound  by the  conditions  described below
 *  in this notice:
 *
 *  This software product,  DBTOOL,  is developed by T.v. Dein
 *  and  copyrighted  (C)  2001-2015  by  T.v. Dein,  with all
 *  rights reserved.
 *
 *  There  is no charge for DBTOOL software.  You can redistribute
 *  it and/or modify it under the terms of the GNU  General Public
 *  License, which is incorporated by reference herein.
 *
 *  DBTOOL is distributed WITHOUT ANY WARRANTY, IMPLIED OR EXPRESS,
 *  OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE or that
 *  the use of it will not infringe on any third party's intellec-
 *  tual property rights.
 *
 *  You  should  have received a copy of the  GNU  General  Public
 *  License  along with DBTOOL.  Copies can also be obtained from:
 *
 *    http://www.gnu.org/licenses/gpl.txt
 *
 *  or by writing to:
 *
 *  Free Software Foundation, Inc.
 *  59 Temple Place, Suite 330
 *  Boston, MA 02111-1307
 *  USA
 *
 *  Or contact:
 *
 *   "T.v. Dein" <tlinden@cpan.org>
 *
 *
 */


#ifndef ENGINE_H
#define ENGINE_H

/* the default initial blocksize for the gdbm database, see gdbm(3) */
#define BLOCKSIZE 1024

/* the default filemode for file creation, will be xored to the users umask */
#define FILEMODE  0644


#include <string>

extern "C" {
#include "platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <pcre2.h>
}

#ifdef HAVE_BERKELEY

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#endif

#include <db_cxx.h>

#else

# include <gdbm.h>

#endif

#include "config.h"
#include "cipher.h"

extern Config config; /* instanced in ::main() */
extern cipher rijn;   /* instanced in ::main() */

/*
 * The Engine class will do all gdbm related stuff, i.e.
 * insert, search, create, remove and so on.
 * methods defined in engine.cc
 */
class Engine {
 private:
  void init(); /* init() will be called from every method once */
  string encode(const string& data);
  string decode(const string& data);

  /*
   * the db object has the same name for both backends
   */
#ifdef HAVE_BERKELEY
  Db *db;
#else
  GDBM_FILE db;
#endif

  string pkg;

 public:
  Engine() {};
  void insert();
  void update();
  void remove();
  void select();
  void from_input();
  void   dump();
  void regexp();
};


#endif // ENGINE_H

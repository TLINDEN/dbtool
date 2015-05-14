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

#include "platform.h"
#include "dbtool.h"
#include "config.h"
#include "engine.h"
#include "cipher.h"

Config config;
cipher rijn;

int main(int argc, char *argv[]) {
  config.args(argc, argv);
  Engine engine;
  string pkg = PACKAGE;

  if(config.parse() != 0) {
    cerr << config.usage;
    exit(1);
  }
  else {
    switch(config.command) {
    case 'i':
      /* insert */
      if(config.key == "") {
	cerr << pkg << ": key required\n";
	exit(1);
      }
      engine.insert();
      break;
    case 'd':
      /* dump */
      engine.dump();
      break;
    case 'I':
      engine.from_stdin();
      break;
    case 's':
      /* select */
      if(config.key == "") {
	cerr << pkg << ": key required\n";
	exit(1);
      }
      engine.select();
      break;
    case 'S':
      /* regexp select */
      if(config.key == "") {
	cerr << pkg << ": key (regexp) required\n";
	exit(1);
      }
      engine.regexp();
      break;
    case 'r':
      /* remove */
      if(config.key == "") {
	cerr << pkg << ": key required\n";
	exit(1);
      }
      engine.remove();
      break;
    case 'u':
      /* update */
      if(config.key == "") {
	cerr << pkg << ": key required\n";
	exit(1);
      }
      engine.update();
      break;
    case 'v':
      /* version */
      cerr << "This is " << pkg << " version " << VERSION << " by T.v. Dein.\n";
      exit(1);
    case 'h':
      /* usage/help */
      cerr << config.usage;
      exit(1);
    }
  }
  exit(0);
}



string readpass() {
  char *pass;
  char *envpass;
  envpass = getenv(PW_VARNAME);
  if(envpass != NULL) {
    string pw = envpass;
    return pw;
  }
  else {
#ifdef HAVE_GETPASS
    pass = getpass("passphrase: ");
    string password = pass;
    free(pass);
#else
    cout << "passphrase: ";
    string password;
    cin >> password;
#endif
    return password;
  }
}



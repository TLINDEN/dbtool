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




#include "dbtool.h"
#include "config.h"
#include "platform.h"


Config::Config() {
  /* konstructor */ 
  pkg = PACKAGE;
  default_db = DB_VARNAME;

  usage =
    "\nUsage: " + pkg + " -d database [DirusSfwVhtRFpP] [-k key] [-v value]\n"
    + " -d database   Specify the filename of the database\n"
    + " -i            Insert data, -k and -v is required (*)\n"
    + " -r            Remove data, -k is required\n"
    + " -u            Update data, -k and -v is required\n"
    + " -s            Search for a key, -k is required\n"
    + " -S            Search for all entrys which match a regular\n"
    + "               expression, the regexp must be specified with\n"
    + "               the -k parameter, i.e.: " + pkg + " -d file -S -k \"^[a-z]+\\d{3}.*$\"\n"
    + " -D            Dump all keys/values of the specified gdbm file\n"
    + " -w            Print out keys too, only possible with -S and -s\n"
    + " -f            Force mode (overwrite file and data!)\n"
    + " -k key        the key for an entry\n"
    + " -v value      the value for the corresponding key, -k is required\n"
    + " -F separator  field separator, optional used together with\n"
    + "               -i if no -k and -v is specified, and with -D as\n"
    + "               output separator\n"
    + " -t expression Use expression to decide which part of an input line\n"
    + "               is to be used as the key and which one as the value, i.e.:\n"
    + "               " + pkg + " -i -d file -t \"^(.*)(\\/.*)$\";\n"
    + "               only possible with -i and without -k and -v(read from stdin)\n"
    + " -R            Reverse meaning of token -t, the first match will be the\n"
    + "               value and the second one the key, if used with -s, -S or -D then\n"
    + "               first the value and then the key will be printed\n"
    + " -p            Use encrypted database. " + pkg + " will ask you for the passphrase,\n"
    + "               unless the environment variable DB_PASSPHRASE is set.\n"
    + " -P passphrase Use encrypted database. Specify the passphrase on the commandline\n"
    + " -V            Print the version of dbtool\n"
    + " -h            Print this usage message\n"
    + "\n"
    + "If you insert data and omit -k and -v then " + pkg + " will read\n"
    + "the key/value pair from STDIN, key and value will be\n"
    + "separated by the parameter of the -F option or, if -F\n"
    + "is omited, by whitespace, the record separator is the\n"
    + "newline, thus one line stands for one key/value pair.\n"
    + "\n"
    + "If no database filename is specified (-d) but the\n"
    + "environment variable DB_DATABASE exists, " + pkg + " will\n"
    + "use the file specified in DB_DATABASE.\n"
    + "\n"
    + "" + pkg + " is free software under the terms of the GPL.\n"
    + "Report bugs to T.v. Dein <tlinden@cpan.org>.\n";
}


void Config::args(int argc, char *argv[]) {
  _argc = argc;
  _argv = argv;
}

int Config::parse() {
  /* parse commandline parameters */

  int option;
  int cmd_insert = 0;

  separator = ' ';               /* default field separator, used by -S and -D and -i */
  command   = 'X';               /* this should get something else */
  with      = 0;                 /* do not print keys, used by -S and -s */
  reverse   = 0;                 /* first key, then value (not reverse is the default) */

  char *fname;
  fname = getenv(default_db);
  if(fname != NULL) {
    filename = fname;            /* will be overwritten by -d */
  }

  while (1) {
    option = getopt(_argc, _argv, "F:DSRft:wuirsd:k:v:VhpP:");
    if(option == -1) break;

    switch(option) {
    case 'f':
      force = 1;
      break;
    case 'i':
      command = 'i';
      break;
    case 'r':
      command = 'r';
      break;
    case 's':
      command = 's';
      readonly = 1;
      break;
    case 'S':
      command = 'S';
      readonly = 1;
      break;
    case 'u':
      command = 'u';
      break;
    case 'd':
      filename = optarg;
      break;
    case 'k':
      key = optarg;
      cmd_insert = 1;
      break;
    case 'v':
      value = optarg;
      cmd_insert = 1;
      break;
    case 'F':
      separator = optarg[0];
      break;
    case 'R':
      reverse = 1;
      break;
    case 'D':
      command = 'd';
      readonly = 1;
      break;
    case 't':
      token = optarg;
      break;
    case 'w':
      with = 1;
      break;
    case 'V':
      command = 'v';
      break;
    case 'h':
      command = 'h';
      break;
    case 'p':
      encrypted = 1;
      phrase = "";
      break;
    case 'P':
      encrypted = 1;
      phrase = optarg;
      break;
    case '?':
      return -1;
    }
  }

  /* check the sence of some options */
  if(token != "" && command != 'i') {
    cerr << pkg << ": -t only suitable with -i!\n";
    exit(1);
  }
  if(command == 'i' && cmd_insert != 1) {
    command = 'I'; /* read from stdin! */
  }
  else if(command == 'X') {
    command = 'h'; /* no command mode specified, so print out the usage message and exit */
  }
  if(with == 1 && command != 'S' && command != 's') {
    cerr << pkg << ": -w only suitable with -S or -s!\n";
    exit(1);
  }


  return 0;
}


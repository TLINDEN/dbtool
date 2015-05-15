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
#include "engine.h"
#include "platform.h"
/*
 * Initialize the database and get a new Db instance pointer
 */
void Engine::init() {
  pkg = PACKAGE;
  int mode;
#ifdef HAVE_BERKELEY
  int err;
#endif
  if(config.filename == "") {
    cerr << pkg << ": no database file specified!" << endl;
    exit(1);
  }
#ifdef HAVE_BERKELEY
  db = new Db(NULL,0);
  db->set_error_stream(&cerr);
  db->set_errpfx(pkg.c_str());
  if(config.readonly == 1)
    mode = DB_RDONLY;
  else
    mode = DB_CREATE;
#else
  if(config.readonly == 1)
    mode = GDBM_READER;
  else
    mode = GDBM_WRCREAT;
#endif


  if(config.force == 1) {
#ifdef HAVE_BERKELEY
    if ((err = db->open(NULL, config.filename.c_str(), NULL, DB_HASH, mode, FILEMODE)) != 0) {
      cerr << "Failed to open " + config.filename << "(" << strerror(err) << ")" << endl;
      exit(1);
    }
#else 

    db = gdbm_open(
		   (char *)config.filename.c_str(),
		   BLOCKSIZE,
		   mode,
		   FILEMODE,
		   0
		   );
#endif
  }
  else {
#ifdef HAVE_BERKELEY
    if ((err = db->open(NULL, config.filename.c_str(), NULL, DB_HASH, mode, FILEMODE)) != 0) {
      cerr << "Failed to open " + config.filename << "(" << strerror(err) << ")" << endl;
      exit(1);
    }
#else
    db = gdbm_open(
		   (char *)config.filename.c_str(),
		   BLOCKSIZE,
		   mode,
		   FILEMODE,
		   0
		   );
#endif
  }
#ifndef HAVE_BERKELEY
  if(!db) {
    cerr << pkg << ": " << gdbm_strerror(gdbm_errno) << endl;
    exit(1);
  }
#endif

  /*
   * ok, at this point everything is ok, usage is correct,
   * database is open, now check the passphrase, if encrypted
   * database requested.
   */
  if(config.encrypted) {
    if(config.phrase == "") {
      config.phrase = readpass();
    }
    rijn.init(config.phrase); /* this might fail */
    config.phrase = "                                                                       ";
  }
}


/*
 * dump out all data in db
 */
void Engine::dump() {
  init();
#ifdef HAVE_BERKELEY
  try {
    Dbc *dbcp;
    db->cursor(NULL, &dbcp, 0);
    Dbt key;
    Dbt data;
    while (dbcp->get(&key, &data, DB_NEXT) == 0) {
      /* iterate over every tuple and dump it out */
      string K((char *)key.get_data(), key.get_size());
      string V((char *)data.get_data(), data.get_size());
      if(config.reverse == 1) {
	cout << decode(V) << config.separator << K << endl;
      }
      else {
	cout << K << config.separator << decode(V) << endl;
      }
    }
    dbcp->close();
  }
  catch (DbException &dbe) {
    cerr << pkg << ": " << dbe.what() << "\n";
  }
  db->close(0);
#else
  datum key;
  datum value;
  key = gdbm_firstkey(db);
  while ( key.dptr != NULL ) {
    /* iterate over every tuple and dump it out */
    value = gdbm_fetch(db, key);   
    string K(key.dptr, key.dsize);
    string V(value.dptr, value.dsize);
    if(config.reverse == 1) {
      cout << decode(V) << config.separator << K << endl;
      }
    else {
      cout << K << config.separator << decode(V) << endl;
    }
    key = gdbm_nextkey(db,key);
  }
  gdbm_close(db);
#endif
}




/*
 * search for regexp given in config.key
 */
void Engine::regexp() {
  init();
  int num;
  pcre *p_pcre;
  pcre_extra *p_pcre_extra;
  char *err_str;
  int sub_len = 9;
  int *sub_vec;
  int erroffset;
  p_pcre_extra = NULL;
  p_pcre = pcre_compile((char *)config.key.c_str(), 0,
			(const char **)(&err_str), &erroffset, NULL);
#ifdef HAVE_BERKELEY
    Dbc *dbcp;
    db->cursor(NULL, &dbcp, 0);
    Dbt key;
    Dbt data;
    while (dbcp->get(&key, &data, DB_NEXT) == 0) {
      string K((char *)key.get_data(), key.get_size());
      string V((char *)data.get_data(), data.get_size());
#else
    datum key;
    datum value;
    key = gdbm_firstkey(db);
    while ( key.dptr != NULL ) {
      value = gdbm_fetch(db, key);
      string K(key.dptr, key.dsize);
      string V(value.dptr, value.dsize);
#endif
      sub_vec = new int(sub_len);
      num = pcre_exec(p_pcre, p_pcre_extra, (char *)K.c_str(),
		      (int)K.length(), 0, 0, (int *)sub_vec, sub_len);
      if(num == 1){
	if(config.reverse == 1) {
	  cout << decode(V);
	  if(config.with == 1) {
	    cout << config.separator << K;
	  }
	  cout << endl;
	}
	else {
	  if(config.with == 1) {
	    cout << K << config.separator;
	  }
	  cout << decode(V) << endl;
	}
      }
      K = "";
      V = "";
      delete(sub_vec);
#ifndef HAVE_BERKELEY
      key = gdbm_nextkey(db,key);
#endif
    }
    pcre_free(p_pcre);
#ifdef HAVE_BERKELEY
    dbcp->close();
    db->close(0);
#else
  gdbm_close(db);
#endif
}






/*
 * Insert data into the db
 */
void Engine::from_input() {
  init();
#ifdef HAVE_BERKELEY
  int err;
#else
  int ret;
#endif

  FILE *stream;
  stream = fdopen(0, "r");
  char c;
  string mode = "key";
  string key = "";
  string value = "";
  string line = "";
  
  int num, match;
  pcre *p_pcre;
  pcre_extra *p_pcre_extra;
  char *err_str;
  int sub_len = 9;
  int *sub_vec;
  char *v1;
  char *v2;
  int erroffset;
  p_pcre_extra = NULL;
  p_pcre = pcre_compile((char *)config.token.c_str(), 0,
			(const char **)(&err_str), &erroffset, NULL);
  while((c = fgetc(stream)) != EOF) {
    if(c == '\n') {
      // record end
      mode = "key";
      if(config.token != "") {
	v1 = new char[line.length()];
	v2 = new char[line.length()];
	sub_vec = new int[sub_len];
	num = pcre_exec(p_pcre, p_pcre_extra, (char *)line.c_str(),
			(int)line.length(), 0, 0, (int *)sub_vec, sub_len);
	if(num < 0)
	  cerr << "Token \"" << config.token << "\" did not match on \"" << line << "\"!\n";
	else if(num == 1)
	  cerr << "Token " << config.token << " did not produce sub strings!\n";
	else {
	  match = pcre_copy_substring((char *)line.c_str(), sub_vec, num, 1, v1, line.length());
	  match = pcre_copy_substring((char *)line.c_str(), sub_vec, num, 2, v2, line.length());
	  if(config.reverse) {
	    value = v1;
	    key   = v2;
	  }
	  else {
	    value = v2;
	    key   = v1;
	  }
	}
	delete(sub_vec);
	pcre_free((void *)v1);
	pcre_free((void *)v2);
	line = "";
      }
      value = encode(value);
#ifdef HAVE_BERKELEY
      Dbt d_key((char *)key.c_str(), key.length());
      Dbt d_value((char *)value.c_str(), value.length());
#else
      datum d_key = {(char *)key.c_str(), key.length()};
      datum d_value = {(char *)value.c_str(), value.length()};
#endif
      if(config.force == 1) {
#ifdef HAVE_BERKELEY
	if((err = db->put(0, &d_key, &d_value, 0)) != 0) {
	  cerr << "Database error" << "(" << strerror(err) << ")" << endl;
	  exit(1);
	}
#else
	ret = gdbm_store(db, d_key, d_value, GDBM_REPLACE);
#endif
      }
      else {
#ifdef HAVE_BERKELEY
	if((err = db->put(NULL, &d_key, &d_value, DB_NOOVERWRITE)) != 0) {
	  if(err == DB_KEYEXIST) {
	    cerr << "Key " + config.key + " already exists" << "(" << strerror(err) << ")" << endl;
	    exit(1);
	  }
	  else {
	    cerr << "Database error" << "(" << strerror(err) << ")" << endl;
	    exit(1);
	  }
	}
#else
	ret = gdbm_store(db, d_key, d_value, GDBM_INSERT);
#endif
      }
#ifndef HAVE_BERKELEY
      if(ret != 0) {
	cerr << pkg << ": " << gdbm_strerror(gdbm_errno) << endl;
	exit(1);
      }
#endif
      key = "";
      value = "";
      continue;
    }
    else if(c == config.separator && mode != "value" && config.token == "") {
      // key ready, the following stuff is the data!
      mode = "value";
      continue;
    }
    if(config.token != "") {
      /* we have a split token */
      line += char(c);
    }
    else {
      if(mode == "key")
	key += char(c);
      else
	value += char(c);
    }
  }
  pcre_free(p_pcre);
#ifdef HAVE_BERKELEY
  db->close(0);
#else
  gdbm_close(db);
#endif
}


/*
 * Insert data into the db
 */
void Engine::insert() {
  init();
  string __value;
  __value = encode(config.value);
#ifdef HAVE_BERKELEY
  int err;
  char *k;
  char *v;
  k = (char *)config.key.c_str();
  v = (char *)__value.c_str();
  Dbt key(k, strlen(k));
  Dbt value(v, strlen(v));
#else
  int ret;
  datum key   = {(char *)config.key.c_str(), config.key.length()};
  datum value = {(char *)__value.c_str(), __value.length()};
#endif

  if(config.force == 1) {
#ifdef HAVE_BERKELEY
    if((err = db->put(0, &key, &value, 0)) != 0) {
      cerr << "Database error" << "(" << strerror(err) << ")" << endl;
      exit(1);
    }
#else
    ret = gdbm_store(db, key, value, GDBM_REPLACE);
#endif
  }
  else {
#ifdef HAVE_BERKELEY
    if((err = db->put(NULL, &key, &value, DB_NOOVERWRITE)) != 0) {
      if(err == DB_KEYEXIST) {
	cerr << "Key " + config.key + " already exists" << "(" << strerror(err) << ")" << endl;
	exit(1);
      }
      else {
	cerr << "Database error" << "(" << strerror(err) << ")" << endl;
	exit(1);
      }
    }
#else
    ret = gdbm_store(db, key, value, GDBM_INSERT);
#endif
  }
#ifdef HAVE_BERKELEY
  db->close(0);
#else
  if(ret != 0) {
    cerr << pkg << ": " << gdbm_strerror(gdbm_errno) << endl;
    exit(1);
  }
  gdbm_close(db);
#endif
}



/*
 * update a database
 */
void Engine::update() {
  init();
  string __value;
  __value = encode(config.value);
#ifdef HAVE_BERKELEY
  int err;
  char *k;
  char *v;
  k = (char *)config.key.c_str();
  v = (char *)__value.c_str();
  Dbt key(k, strlen(k));
  Dbt value(v, strlen(v));
#else
  int ret;
  datum key   = {(char *)config.key.c_str(), config.key.length()};
  datum value = {(char *)__value.c_str(), __value.length()};
#endif

  if(config.force == 1) {
#ifdef HAVE_BERKELEY
    if((err = db->put(0, &key, &value, 0)) != 0) {
      cerr << "Database error" << "(" << strerror(err) << ")" << endl;
      exit(1);
    }
#else
    ret = gdbm_store(db, key, value, GDBM_REPLACE);
#endif
  }
  else {
#ifdef HAVE_BERKELEY
    Dbt val;
    err = db->get(0, &key, &val, 0);
    if(err == 0) {
      /* key exists, do the update */
      if((err = db->put(0, &key, &value, 0)) != 0) {
	cerr << "Database error" << "(" << strerror(err) << ")" << endl;
	exit(1);
      }
    }
    else if(err == DB_NOTFOUND) {
      cerr << "Key " << config.key << " does not exist\n";
      exit(1);
    }
    else {
      cerr << "Database error" << "(" << strerror(err) << ")" << endl;
      exit(1);
    }
#else
    ret = gdbm_exists(db, key);
    if(ret) {
      ret = gdbm_store(db, key, value, GDBM_REPLACE);
    }
    else {
      cerr << "Key " << config.key << " does not exist\n";
      exit(1);
    }
#endif
  }
#ifdef HAVE_BERKELEY
  db->close(0);
#else
  if(ret != 0) {
    cerr << pkg << ": " << gdbm_strerror(gdbm_errno) << endl;
    exit(1);
  }
  gdbm_close(db);
#endif
}



/*
 * remove an entry
 */
void Engine::remove() {
  init();
  int ret;
#ifdef HAVE_BERKELEY
  Dbt key((char *)config.key.c_str(), config.key.length() + 1);
  if((ret = db->del(NULL, &key, 0)) != 0) {
    cerr << "Database error" << "(" << strerror(ret) << ")" << endl;
    exit(1);
  }
  db->close(0);
#else
  datum key   = {(char *)config.key.c_str(), config.key.length()};
  ret = gdbm_delete(db, key);
  gdbm_close(db);
#endif
}



/*
 * search for specific data
 */
void Engine::select() {
  init();
#ifdef HAVE_BERKELEY
  int err;
  char *k;
  k = (char *)config.key.c_str();
  Dbt key(k, strlen(k));
  Dbt value;
  err = db->get(0, &key, &value, 0);
  if(err == 0) {
    string gotvalue((char *)value.get_data(), value.get_size());
    if(config.reverse == 1) {
      cout << decode(gotvalue);
      if(config.with == 1) {
	cout << config.separator << config.key;
      }
      cout << endl;
    }
    else {
      if(config.with == 1) {
	cout << config.key << config.separator;
      }
      cout << decode(gotvalue) << endl;
    }
  }
  else {
    cerr << "Database error" << "(" << strerror(err) << ")" << endl;
    exit(1);
  }
  db->close(0);
#else
  datum content;
  datum key   = {(char *)config.key.c_str(), config.key.length()};
  content = gdbm_fetch(db, key);
  string V(content.dptr, content.dsize);
  if(config.with == 1)
    cout << config.key << config.separator;
  cout << decode(V) << endl;
  gdbm_close(db);
#endif
}

string Engine::encode(const string& data) {
  if(config.encrypted) {
    return rijn.encrypt(data);
  }
  else
    return data;
}

string Engine::decode(const string& data) {
  if(config.encrypted) {
    return rijn.decrypt(data);
  }
  else
    return data;
}


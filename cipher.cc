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


#include "cipher.h"

void cipher::init(const string& phrase) {
  /*
   * 1st create a MD5 checksum of the passphrase and
   * use this instead of the phrase itself for encryption
   */
  char *__key;
  dig.initDigest();
  dig.putDigest( (unsigned char *)phrase.c_str(), phrase.length() );
  __key = dig.stringDigest(); // this is a 32 byte long string, as Rijndael:: expects
  
  /* convert the key to unsigned char[] */
  for(int i=0; i<32; i++) {
    key[i] = __key[i];
  }
}



string cipher::encrypt(const string& source) {
  size_t size = source.length();

  /* convert the source to unsigned char[] */
  unsigned char* plainText;
  plainText = new unsigned char[size];
  for(int a=0; a<size; a++) {
    plainText[a] = source[a];
  }

  /* initialize encryptor */
  rijn.init(Rijndael::CBC,  Rijndael::Encrypt, key, Rijndael::Key32Bytes);

  /* encrypt the source */
  unsigned char *output = (unsigned char *)malloc(size + 16);
  int res = rijn.padEncrypt(plainText, (int)size, output);

  /* convert the result back to char[] */
  char *outText = (char *)malloc(res);
  for(int y=0; y<(res); y++) {
    outText[y] = output[y];
  }

  free(output);
  
  /* return the crypted string */
  if(res >= 0) {
    string text = string(outText, res);
    free(outText);
    return text;
  }
  else {
    cerr << "Failed to encrypt: " << error(res) << "!" << endl;
    exit(1);
  }
}


string cipher::decrypt(const string& source) {
  size_t size = source.length();

    /* convert the source to unsigned char[] */
  unsigned char* cryptedText;
  cryptedText = new unsigned char[size];
  for(int a=0; a<size; a++) {
    cryptedText[a] = source[a];
  }

  /* initialize decryptor */
  rijn.init(Rijndael::CBC,  Rijndael::Decrypt, key, Rijndael::Key32Bytes);

  /* decrypt the source */
  unsigned char *output = (unsigned char *)malloc(size);
  int res = rijn.padDecrypt(cryptedText, (int)size, output);

  /* convert the result back to char[] */
  char *outText = (char *)malloc(res);
  for(int y=0; y<(res); y++) {
    outText[y] = output[y];
  }

  free(output);
  
  /* return the decrypted string */
  if (res >= 0) {
    string text = string(outText, res);
    free(outText);
    return text;
  }
  else {
    cerr << "Failed to decrypt: " << error(res) << " (passphrase invalid?) !" << endl;
    exit(1);
  }
}

const char* cipher::error(int num) {
  switch(num) {
  case 0:  return "RIJNDAEL_SUCCESS"; break;
  case -1: return "RIJNDAEL_UNSUPPORTED_MODE"; break;
  case -2: return "RIJNDAEL_UNSUPPORTED_DIRECTION"; break;
  case -3: return "RIJNDAEL_UNSUPPORTED_KEY_LENGTH"; break;
  case -4: return "RIJNDAEL_BAD_KEY"; break;
  case -5: return "RIJNDAEL_NOT_INITIALIZED"; break;
  case -6: return "RIJNDAEL_BAD_DIRECTION"; break;
  case -7: return "RIJNDAEL_CORRUPTED_DATA"; break;
  default: return "UNKNOWN"; break;
  }
}

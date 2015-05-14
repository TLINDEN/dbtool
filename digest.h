// Copyright (C) 1999-2001 Open Source Telecom Corporation.
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// As a special exception to the GNU General Public License, permission is 
// granted for additional uses of the text contained in its release 
// of Common C++.
// 
// The exception is that, if you link the Common C++ library with other
// files to produce an executable, this does not by itself cause the
// resulting executable to be covered by the GNU General Public License.
// Your use of that executable is in no way restricted on account of
// linking the Common C++ library code into it.
// 
// This exception does not however invalidate any other reasons why
// the executable file might be covered by the GNU General Public License.
// 
// This exception applies only to the code released under the 
// name Common C++.  If you copy code from other releases into a copy of
// Common C++, as the General Public License permits, the exception does
// not apply to the code that you add in this way.  To avoid misleading
// anyone as to the status of such modified files, you must delete
// this exception notice from them.
// 
// If you write modifications of your own for Common C++, it is your choice
// whether to permit this exception to apply to your modifications.
// If you do not wish that, delete this exception notice.  

#ifndef	DIGEST_H
#define	DIGEST_H

#include <cstdio>

extern "C" {
#include <string.h>
}



/**
 * The digest base class is used for implementing and deriving one way
 * hashing functions.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short base class for hashing services.
 */
class Digest {
protected:
	Digest() {};

	/**
	 * Reset the digest table to an initial default value.
	 */
	virtual void initDigest(void) = 0;

	/**
	 * Get the size of a digest in octets.
	 *
	 * @return number of bytes in digest.
	 */
	virtual unsigned getSize(void) = 0;

	/**
	 * Copy the binary digest buffer to user memory.
	 *
	 * @return number of bytes in digest.
	 * @param buffer to write into.
	 */
	virtual unsigned getDigest(unsigned char *buffer) = 0;

	/**
	 * Put data into the digest bypassing the stream subsystem.
	 *
	 * @param buffer to read from.
	 * @param length of data.
	 */
	virtual void putDigest(unsigned char *buffer, unsigned length) = 0;

};


/**
 * A md5 collection/computation accululator class.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short md5 hash accumulation.
 */
class MD5Digest : public Digest
{
private:
	unsigned long state[4];
	unsigned long count[2];
	unsigned char buf[64];
	unsigned bpos;
	unsigned char md5[16];
	bool updated;

protected:
	int overflow(int c);

	void Update(void);

	void Commit(void);

public:
	MD5Digest();

	void initDigest(void);

	inline unsigned getSize(void)
		{return 16;};

	unsigned getDigest(unsigned char *buffer);

	void putDigest(unsigned char *buffer, unsigned len);

	char* stringDigest();
};


#endif


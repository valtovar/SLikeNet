/*-
 * Parts of this code are copyright 2003-2005 Colin Percival
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions 
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
 
/*
 * This file was taken from RakNet 4.082.
 * Please see licenses/RakNet license.txt for the underlying license and related copyright.
 *
 * Modified work: Copyright (c) 2016-2017, SLikeSoft UG (haftungsbeschränkt)
 *
 * This source code was modified by SLikeSoft. Modifications are licensed under the MIT-style
 * license found in the license.txt file in the root directory of this source tree.
 * Alternatively you are permitted to license the modifications under the Simplified BSD License.
 */
 
#if 0
__FBSDID("$FreeBSD: src/usr.bin/bsdiff/bspatch/bspatch.c,v 1.1 2005/08/06 01:59:06 cperciva Exp $");
#endif

#include "MemoryCompressor.h"

#include <bzlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef _WIN32
// KevinJ - Windows compatibility
#include <err.h>
#include <unistd.h>
#else
typedef int ssize_t;
#include <wchar.h>
#include <io.h>
#include "slikenet/linux_adapter.h"
#include "slikenet/osx_adapter.h"
#define fseeko fseek
static void err(int i, ...)
{
	exit(i);
}
static void errx(int i, ...)
{
	exit(i);
}
#endif
#include <fcntl.h>

#ifndef _O_BINARY
#define _O_BINARY 0
#endif
#ifndef O_BINARY
#define O_BINARY _O_BINARY 
#endif


static off_t offtin(unsigned char *buf)
{
	off_t y;

	y=buf[7]&0x7F;
	y=y*256;y+=buf[6];
	y=y*256;y+=buf[5];
	y=y*256;y+=buf[4];
	y=y*256;y+=buf[3];
	y=y*256;y+=buf[2];
	y=y*256;y+=buf[1];
	y=y*256;y+=buf[0];

	if(buf[7]&0x80) y=-y;

	return y;
}

// This function modifies the main() function included in bspatch.c of bsdiff-4.3 found at http://www.daemonology.net/bsdiff/
// It is changed to be a standalone function, to work entirely in memory, and to use my class MemoryDecompressor as an interface to BZip
// Up to the caller to deallocate new
bool ApplyPatch(char *old, unsigned int oldsize, char **_new, unsigned int *newsize, char *patch, unsigned int patchsize )
{
//	FILE * f, * cpf, * dpf, * epf;
//	BZFILE * cpfbz2, * dpfbz2, * epfbz2;
//	int cbz2err, dbz2err, ebz2err;
//	int fd;
//	ssize_t oldsize,newsize;
	ssize_t bzctrllen,bzdatalen;
	//unsigned char header[32];
	unsigned char buf[8];
//	unsigned char *old, *_new;
	off_t oldpos,newpos;
	off_t ctrl[3];
//	off_t lenread;
	off_t i;
	MemoryDecompressor decompress;
	unsigned int coff, doff, eoff;

//	if(argc!=4) errx(1,"usage: %s oldfile newfile patchfile\n",argv[0]);

	/* Open patch file */
//	if (fopen_s(&f, argv[3], "rb") != 0)
//		err(1, "fopen(%s)", argv[3]);

	/*
	File format:
	0	8	"BSDIFF40"
	8	8	X
	16	8	Y
	24	8	sizeof(newfile)
	32	X	bzip2(control block)
	32+X	Y	bzip2(diff block)
	32+X+Y	???	bzip2(extra block)
	with control block a set of triples (x,y,z) meaning "add x bytes
	from oldfile to x bytes from the diff block; copy y bytes from the
	extra block; seek forwards in oldfile by z bytes".
	*/

	/* Read header */
//	if (fread(header, 1, 32, f) < 32) {
//		if (feof(f))
//			errx(1, "Corrupt patch\n",0);
//		err(1, "fread(%s)", argv[3]);
//	}

//	memcpy(header, patch, 32);

	/* Check for appropriate magic */
	if (memcmp(patch, "BSDIFF40", 8) != 0)
	//	errx(1, "Corrupt patch\n",0);
		return false;

	/* Read lengths from header */
	bzctrllen=offtin((unsigned char*)patch+8);
	bzdatalen=offtin((unsigned char*)patch+16);
	*newsize=offtin((unsigned char*)patch+24);
	if((bzctrllen<0) || (bzdatalen<0) || (*newsize<0))
	//	errx(1,"Corrupt patch\n",0);
		return false;

	/* Close patch file and re-open it via libbzip2 at the right places */
//	if (fclose(f))
//		err(1, "fclose(%s)", argv[3]);
//	if (fopen_s(&cpf, argv[3], "rb") != 0)
//		err(1, "fopen(%s)", argv[3]);
//	if (fseeko(cpf, 32, SEEK_SET))
//		err(1, "fseeko(%s, %lld)", argv[3],
//		(long long)32);
//	if ((cpfbz2 = BZ2_bzReadOpen(&cbz2err, cpf, 0, 0, NULL, 0)) == NULL)
//		errx(1, "BZ2_bzReadOpen, bz2err = %d", cbz2err);
//	if (fopen_s(&dpf, argv[3], "rb") != 0)
//		err(1, "fopen(%s)", argv[3]);
//	if (fseeko(dpf, 32 + bzctrllen, SEEK_SET))
//		err(1, "fseeko(%s, %lld)", argv[3],
//		(long long)(32 + bzctrllen));
//	if ((dpfbz2 = BZ2_bzReadOpen(&dbz2err, dpf, 0, 0, NULL, 0)) == NULL)
//		errx(1, "BZ2_bzReadOpen, bz2err = %d", dbz2err);
//	if (fopen_s(&epf, argv[3], "rb") != 0)
//		err(1, "fopen(%s)", argv[3]);
//	if (fseeko(epf, 32 + bzctrllen + bzdatalen, SEEK_SET))
//		err(1, "fseeko(%s, %lld)", argv[3],
//		(long long)(32 + bzctrllen + bzdatalen));
//	if ((epfbz2 = BZ2_bzReadOpen(&ebz2err, epf, 0, 0, NULL, 0)) == NULL)
//		errx(1, "BZ2_bzReadOpen, bz2err = %d", ebz2err);
	
	// decompress.Decompress((char*)patch+32, patchsize-32, true);

	coff=0;
	if (decompress.Decompress((char*)patch+32, bzctrllen, false)==false)
		return false;
	doff=decompress.GetTotalOutputSize();
	if (decompress.Decompress((char*)patch+32+bzctrllen, bzdatalen, false)==false)
		return false;
	eoff=decompress.GetTotalOutputSize();
	if (decompress.Decompress((char*)patch+32+bzctrllen+bzdatalen, patchsize-(32+bzctrllen+bzdatalen), true)==false)
		return false;

//	if(((fd=_open(argv[1],O_RDONLY | _O_BINARY,0))<0) ||
//		((oldsize=_lseek(fd,0,SEEK_END))==-1) ||
//		((old=(unsigned char*)malloc(oldsize+1))==NULL) ||
//		(_lseek(fd,0,SEEK_SET)!=0) ||
//		(_read(fd,old,oldsize)!=oldsize) ||
//		(_close(fd)==-1)) err(1,"%s",argv[1]);
//	if((_new=(unsigned char*)malloc(newsize+1))==NULL) err(1,NULL);
	*_new = new char[*newsize+1];

	oldpos=0;newpos=0;
	while(newpos<(off_t)*newsize) {
		/* Read control data */
		for(i=0;i<=2;i++) {
		//	lenread = BZ2_bzRead(&cbz2err, cpfbz2, buf, 8);
		//	if ((lenread < 8) || ((cbz2err != BZ_OK) &&
		//		(cbz2err != BZ_STREAM_END)))
		//		errx(1, "Corrupt patch\n");
			memcpy(buf, decompress.GetOutput()+coff, 8);
			coff+=8;
		
			ctrl[i]=offtin(buf);
		};

		/* Sanity-check */
		if(newpos+ctrl[0]>(off_t)*newsize)
		{
			delete [] (*_new);
			return false;
		}

		/* Read diff string */
		//lenread = BZ2_bzRead(&dbz2err, dpfbz2, _new + newpos, ctrl[0]);
		//if ((lenread < ctrl[0]) ||
		//	((dbz2err != BZ_OK) && (dbz2err != BZ_STREAM_END)))
		//	errx(1, "Corrupt patch\n");
		memcpy(*_new + newpos, decompress.GetOutput()+doff, ctrl[0]);
		doff+=ctrl[0];

		/* Add old data to diff string */
		for(i=0;i<ctrl[0];i++)
			if((oldpos+i>=0) && (oldpos+i<(off_t)oldsize))
				(*_new)[newpos+i]+=old[oldpos+i];

		/* Adjust pointers */
		newpos+=ctrl[0];
		oldpos+=ctrl[0];

		/* Sanity-check */
		if(newpos+ctrl[1]>(off_t)*newsize)
		{
			delete [] (*_new);
			return false;
		}

		/* Read extra string */
		//lenread = BZ2_bzRead(&ebz2err, epfbz2, _new + newpos, ctrl[1]);
		//if ((lenread < ctrl[1]) ||
		//	((ebz2err != BZ_OK) && (ebz2err != BZ_STREAM_END)))
		//	errx(1, "Corrupt patch\n");
		memcpy(*_new + newpos, decompress.GetOutput()+eoff, ctrl[1]);
		eoff+=ctrl[1];

		/* Adjust pointers */
		newpos+=ctrl[1];
		oldpos+=ctrl[2];
	};

	/* Clean up the bzip2 reads */
//	BZ2_bzReadClose(&cbz2err, cpfbz2);
//	BZ2_bzReadClose(&dbz2err, dpfbz2);
//	BZ2_bzReadClose(&ebz2err, epfbz2);
//	if (fclose(cpf) || fclose(dpf) || fclose(epf))
//		err(1, "fclose(%s)", argv[3]);

	/* Write the new file */
//	if(((fd=_open(argv[2],O_CREAT|O_TRUNC|O_WRONLY,0666))<0) ||
//		(_write(fd,_new,newsize)!=newsize) || (_close(fd)==-1))
//		err(1,"%s",argv[2]);

//	free(_new);
//	free(old);

	return true;
}


int TestPatchInMemory(int argc,char *argv[])
{
	FILE *patchFile, *newFile, *oldFile;
	char *patch,*_new,*old;
	unsigned patchSize, oldSize;
	unsigned int newSize;
	int res;
	if(argc!=4) errx(1,"usage: %s oldfile newfile patchfile\n",argv[0]);

	fopen_s(&patchFile, argv[3], "rb");
	fopen_s(&newFile, argv[2], "wb");
	fopen_s(&oldFile, argv[1], "rb");
	fseeko(patchFile, 0, SEEK_END);
	fseeko(oldFile, 0, SEEK_END);
	patchSize=ftell(patchFile);
	oldSize=ftell(oldFile);
	fseeko(patchFile, 0, SEEK_SET);
	fseeko(oldFile, 0, SEEK_SET);
	patch = new char [patchSize];
	old = new char [oldSize];
	fread(patch, patchSize, 1, patchFile);
	fread(old, oldSize, 1, oldFile);

	// #low Should not allocate memory in ApplyPatch and then free it in the caller again
	// also return value looks inconsistent
	res = (ApplyPatch( old, oldSize, &_new, &newSize,patch, patchSize) ? 1 : 0);
	delete[] old;
	delete[] patch;
	fwrite(_new, newSize, 1, newFile);

	if (res == 1) {
		// if res == 0, _new was already cleared in ApplyPatch
		delete[] _new;
	}

	fclose(patchFile);
	fclose(newFile);
	fclose(oldFile);

	return res;
}

int PATCH_main(int argc,char * argv[])
{
	FILE * f, * cpf, * dpf, * epf;
	BZFILE * cpfbz2, * dpfbz2, * epfbz2;
	int cbz2err, dbz2err, ebz2err;
	int fd;
	ssize_t oldsize,newsize;
	ssize_t bzctrllen,bzdatalen;
	unsigned char header[32],buf[8];
	unsigned char *old, *_new;
	off_t oldpos,newpos;
	off_t ctrl[3];
	off_t lenread;
	off_t i;

	unsigned bytesRead=0;

	if(argc!=4) errx(1,"usage: %s oldfile newfile patchfile\n",argv[0]);

	/* Open patch file */
	if (fopen_s(&f, argv[3], "rb") != 0)
		err(1, "fopen(%s)", argv[3]);

	/*
	File format:
	0	8	"BSDIFF40"
	8	8	X
	16	8	Y
	24	8	sizeof(newfile)
	32	X	bzip2(control block)
	32+X	Y	bzip2(diff block)
	32+X+Y	???	bzip2(extra block)
	with control block a set of triples (x,y,z) meaning "add x bytes
	from oldfile to x bytes from the diff block; copy y bytes from the
	extra block; seek forwards in oldfile by z bytes".
	*/

	/* Read header */
	if (fread(header, 1, 32, f) < 32) {
		if (feof(f))
			errx(1, "Corrupt patch\n");
		err(1, "fread(%s)", argv[3]);
	}

	/* Check for appropriate magic */
	if (memcmp(header, "BSDIFF40", 8) != 0)
		errx(1, "Corrupt patch\n");

	/* Read lengths from header */
	bzctrllen=offtin(header+8);
	bzdatalen=offtin(header+16);
	newsize=offtin(header+24);
	if((bzctrllen<0) || (bzdatalen<0) || (newsize<0))
		errx(1,"Corrupt patch\n");

	/* Close patch file and re-open it via libbzip2 at the right places */
	if (fclose(f))
		err(1, "fclose(%s)", argv[3]);
	if (fopen_s(&cpf, argv[3], "rb") != 0)
		err(1, "fopen(%s)", argv[3]);
	if (fseeko(cpf, 32, SEEK_SET))
		err(1, "fseeko(%s, %lld)", argv[3],
		(long long)32);
	if ((cpfbz2 = BZ2_bzReadOpen(&cbz2err, cpf, 0, 0, NULL, 0)) == NULL)
		errx(1, "BZ2_bzReadOpen, bz2err = %d", cbz2err);
	if (fopen_s(&dpf, argv[3], "rb") != 0)
		err(1, "fopen(%s)", argv[3]);
	if (fseeko(dpf, 32 + bzctrllen, SEEK_SET))
		err(1, "fseeko(%s, %lld)", argv[3],
		(long long)(32 + bzctrllen));
	if ((dpfbz2 = BZ2_bzReadOpen(&dbz2err, dpf, 0, 0, NULL, 0)) == NULL)
		errx(1, "BZ2_bzReadOpen, bz2err = %d", dbz2err);
	if (fopen_s(&epf, argv[3], "rb") != 0)
		err(1, "fopen(%s)", argv[3]);
	if (fseeko(epf, 32 + bzctrllen + bzdatalen, SEEK_SET))
		err(1, "fseeko(%s, %lld)", argv[3],
		(long long)(32 + bzctrllen + bzdatalen));
	if ((epfbz2 = BZ2_bzReadOpen(&ebz2err, epf, 0, 0, NULL, 0)) == NULL)
		errx(1, "BZ2_bzReadOpen, bz2err = %d", ebz2err);

	if(((fd=_open(argv[1],O_RDONLY|O_BINARY,0))<0) ||
		((oldsize=_lseek(fd,0,SEEK_END))==-1) ||
		((old=(unsigned char*)malloc(oldsize+1))==NULL) ||
		(_lseek(fd,0,SEEK_SET)!=0) ||
		(_read(fd,old,oldsize)!=oldsize) ||
		(_close(fd)==-1)) err(1,"%s",argv[1]);
	if((_new=(unsigned char*)malloc(newsize+1))==NULL) err(1,NULL);

	oldpos=0;newpos=0;
	while(newpos<newsize) {
		/* Read control data */
		for(i=0;i<=2;i++) {
			lenread = BZ2_bzRead(&cbz2err, cpfbz2, buf, 8);
			if ((lenread < 8) || ((cbz2err != BZ_OK) &&
				(cbz2err != BZ_STREAM_END)))
				errx(1, "Corrupt patch\n");
			bytesRead+=8;
			//printf("cbz2err cpfbz2 %i %i\n", 8, bytesRead);
			ctrl[i]=offtin(buf);
		};

		/* Sanity-check */
		if(newpos+ctrl[0]>newsize)
			errx(1,"Corrupt patch\n");

		/* Read diff string */
		lenread = BZ2_bzRead(&dbz2err, dpfbz2, _new + newpos, ctrl[0]);
		bytesRead+=8;
//		printf("dbz2err dpfbz2 %i %i\n", ctrl[0], bytesRead);
		if ((lenread < ctrl[0]) ||
			((dbz2err != BZ_OK) && (dbz2err != BZ_STREAM_END)))
			errx(1, "Corrupt patch\n");

		/* Add old data to diff string */
		for(i=0;i<ctrl[0];i++)
			if((oldpos+i>=0) && (oldpos+i<oldsize))
				_new[newpos+i]+=old[oldpos+i];

		/* Adjust pointers */
		newpos+=ctrl[0];
		oldpos+=ctrl[0];

		/* Sanity-check */
		if(newpos+ctrl[1]>newsize)
			errx(1,"Corrupt patch\n");

		/* Read extra string */
		lenread = BZ2_bzRead(&ebz2err, epfbz2, _new + newpos, ctrl[1]);
		bytesRead+=8;
//		printf("ebz2err epfbz2 %i %i\n", ctrl[1], bytesRead);
		if ((lenread < ctrl[1]) ||
			((ebz2err != BZ_OK) && (ebz2err != BZ_STREAM_END)))
			errx(1, "Corrupt patch\n");

		/* Adjust pointers */
		newpos+=ctrl[1];
		oldpos+=ctrl[2];
	};

	/* Clean up the bzip2 reads */
	BZ2_bzReadClose(&cbz2err, cpfbz2);
	BZ2_bzReadClose(&dbz2err, dpfbz2);
	BZ2_bzReadClose(&ebz2err, epfbz2);
	if (fclose(cpf) || fclose(dpf) || fclose(epf))
		err(1, "fclose(%s)", argv[3]);

	/* Write the new file */
	if(((fd=_open(argv[2],O_CREAT|O_TRUNC|O_WRONLY|O_BINARY,0666))<0) ||
		(_write(fd,_new,newsize)!=newsize) || (_close(fd)==-1))
		err(1,"%s",argv[2]);

	free(_new);
	free(old);

	return 0;
}

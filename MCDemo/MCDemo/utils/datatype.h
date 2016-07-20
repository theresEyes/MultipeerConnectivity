#ifndef DATATYPE_H
#define DATATYPE_H
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <assert.h>

#ifdef _WIN32
#ifdef FD_SETSIZE
#undef FD_SETSIZE
#endif
#define FD_SETSIZE 1024
#include <winsock2.h>
#include <windows.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <malloc.h>
#else
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <fcntl.h>
#include <alloca.h>
#include <string.h>
#include <sys/un.h>
#endif
#include <map>
#include <list>
#include <bitset>
#include <set>

#ifdef _WIN32
	#ifndef	CloseSocket2
	#define CloseSocket2(sock) closesocket(sock)
	#endif
	#include <hash_set>
	#include <hash_map>
	typedef	SOCKET				SOCKET_T;
#else
	typedef unsigned int DWORD;
	typedef unsigned char BYTE;
	typedef unsigned short WORD;
	#include <ext/hash_set>
	#include <ext/hash_map>
	#define BOOL bool
	#ifndef	CloseSocket2
	#define CloseSocket2(sock) close(sock)
	#endif
	typedef	int				SOCKET_T;
#endif

#include <string>
using namespace std;

#ifdef _WIN32
	using namespace stdext;
#else
    namespace __gnu_cxx{
        template<>
        struct hash<unsigned long long> {
            size_t operator()(const unsigned long long & key) const {
                return ((key&0xffffFFFF00000000LL) >> 32) 
                    +   (key&0xFFFFffffLL);
            }
        };
    }
	using namespace __gnu_cxx;
#endif

//#include "queue.h"

#pragma pack(push, 1)		/* Set data byte align */

#undef	BIG_ENDIAN
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN	1
#endif

#ifndef int8
	typedef char int8;
#endif
#ifndef uint8
	typedef unsigned char uint8;
#endif
#ifndef int16
	typedef short int16;
#endif
#ifndef uint16
	typedef unsigned short uint16;
#endif
#ifndef uint
	typedef unsigned int uint;
#endif
#ifndef int32
	typedef int int32;
#endif
#ifndef uint32
	typedef unsigned int uint32;
#endif
#ifndef int64
	typedef long long int64;
#endif
#ifndef uint64
	typedef unsigned long long uint64;
#endif
#ifdef _WIN32
#undef socklen_t
typedef int socklen_t;
#else
typedef long long int64;
typedef unsigned long long uint64;
#endif

#ifdef _WIN32
#define STDCALL __stdcall
#else
#define STDCALL
#endif

#ifndef TRUE
#define TRUE			1
#endif
#ifndef FALSE
#define FALSE			0
#endif

typedef uint8 tvu_guid[8];

#define BITS8	0xff
#define BITS16	0xffff
#define BITS32	0xffffffff
#define BITS48	0xffffffffffff


#define MAX(a,b)	((a)>(b)) ? (a):(b)
#define MIN(a,b)	((a)>(b)) ? (b):(a)
#define is_num(x) ((x)<='9'&&(x)>='0')
						
#ifndef GUID_DEFINED
#define GUID_DEFINED
			 
#ifndef _WIN32
struct GUID   // size is 16 bytes
{          
	uint32	Data1;
	uint16	Data2;
	uint16	Data3;
	uint8	Data4[8];
};
#endif
#endif


#ifdef __cplusplus

#ifdef LITTLE_ENDIAN	/* Intel architecture */
static inline uint64 hton64(uint64 lli)
{
	uint8 *cold, *cnew;
	uint64 llinew = 0;
	int i;
	cold = (uint8 *)&lli;
	cnew = (uint8 *)&llinew;
	for(i=0;i<8;i++)
		cnew[i] = cold[7-i];
	return llinew;
}

#define ntoh64	hton64

static inline uint32 reverse32(uint32 n)
{
	uint8 *cold, *cnew;
	uint32 nn = 0;
	int i;
	cold = (uint8 *)&n;
	cnew = (uint8 *)&nn;
	for(i=0;i<4;i++)
		cnew[i] = cold[3-i];
	return nn;
}
static inline uint16 reverse16(uint16 n)
{
	uint8 *cold, *cnew;
	uint16 nn = 0;
	int i;
	cold = (uint8 *)&n;
	cnew = (uint8 *)&nn;
	for(i=0;i<2;i++)
		cnew[i] = cold[1-i];
	return nn;
}
static inline uint64 reverse64(uint64 n)
{
	uint8 *cold, *cnew;
	uint64 nn = 0;
	int i;
	cold = (uint8 *)&n;
	cnew = (uint8 *)&nn;
	for(i=0;i<8;i++)
		cnew[i] = cold[7-i];
	return nn;
}

#elif BIG_ENDIAN	/* BIG_ENDIAN */
#define hton64
#define ntoh64
#endif	/* LITTLE_ENDIAN */

#endif /* cplusplus */

static inline uint64 _strtoull(const char* str, char** p, int len)
{
#ifdef WIN32
	return _strtoui64(str, p, len);
#else
	return strtoull(str, p, len);
#endif
}

typedef struct {
	char   ch[16];
} UUID128_T;

#pragma pack(pop)		/* Restore data byte align */
#endif	/* DATATYPE_H */


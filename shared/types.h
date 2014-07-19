#pragma once

#ifdef _WIN32
#	define INLINE __forceinline
#else
#	define INLINE inline
#endif

#if COMPILER != COMPILER_GNU
	typedef signed __int64 int64;
	typedef signed __int32 int32;
	typedef signed __int16 int16;
	typedef signed __int8 int8;
	typedef unsigned __int64 uint64;
	typedef unsigned __int32 uint32;
	typedef unsigned __int16 uint16;
	typedef unsigned __int8 uint8;
#else
	typedef int64_t int64;
	typedef int32_t int32;
	typedef int16_t int16;
	typedef int8_t int8;
	typedef uint64_t uint64;
	typedef uint32_t uint32;
	typedef uint16_t uint16;
	typedef uint8_t uint8;
#endif

/* Define this if you're using a big-endian machine */
/* NOTE: These can be optimized into assembly */
#ifdef USING_BIG_ENDIAN

#	include <machine/byte_order.h>
#	define bswap_16(x) NXSwapShort(x)
#	define bswap_32(x) NXSwapInt(x)
#	define bswap_64(x) NXSwapLongLong(x)

INLINE static void swap16(uint16* p) { *p = bswap_16((uint16)*p); }
INLINE static void swap32(uint32* p) { *p = bswap_32((uint32)*p); }
INLINE static void swap64(uint64* p) { *p = bswap_64((uint64)*p); }

INLINE static float swapfloat(float p)
{
	union { float asfloat; uint8 asbytes[4]; } u1, u2;
	u1.asfloat = p;
	/* swap! */
	u2.asbytes[0] = u1.asbytes[3];
	u2.asbytes[1] = u1.asbytes[2];
	u2.asbytes[2] = u1.asbytes[1];
	u2.asbytes[3] = u1.asbytes[0];
    
	return u2.asfloat;
}

INLINE static double swapdouble(double p)
{
	union { double asfloat; uint8 asbytes[8]; } u1, u2;
	u1.asfloat = p;
	/* swap! */
	u2.asbytes[0] = u1.asbytes[7];
	u2.asbytes[1] = u1.asbytes[6];
	u2.asbytes[2] = u1.asbytes[5];
	u2.asbytes[3] = u1.asbytes[4];
	u2.asbytes[4] = u1.asbytes[3];
	u2.asbytes[5] = u1.asbytes[2];
	u2.asbytes[6] = u1.asbytes[1];
	u2.asbytes[7] = u1.asbytes[0];

	return u2.asfloat;
}

INLINE static void swapfloat(float * p)
{
	union { float asfloat; uint8 asbytes[4]; } u1, u2;
	u1.asfloat = *p;
	/* swap! */
	u2.asbytes[0] = u1.asbytes[3];
	u2.asbytes[1] = u1.asbytes[2];
	u2.asbytes[2] = u1.asbytes[1];
	u2.asbytes[3] = u1.asbytes[0];
	*p = u2.asfloat;
}

INLINE static void swapdouble(double * p)
{
	union { double asfloat; uint8 asbytes[8]; } u1, u2;
	u1.asfloat = *p;
	/* swap! */
	u2.asbytes[0] = u1.asbytes[7];
	u2.asbytes[1] = u1.asbytes[6];
	u2.asbytes[2] = u1.asbytes[5];
	u2.asbytes[3] = u1.asbytes[4];
	u2.asbytes[4] = u1.asbytes[3];
	u2.asbytes[5] = u1.asbytes[2];
	u2.asbytes[6] = u1.asbytes[1];
	u2.asbytes[7] = u1.asbytes[0];
	*p = u2.asfloat;
}

INLINE static uint16 swap16(uint16 p) { return bswap_16((uint16_t)p); }
INLINE static uint32 swap32(uint32 p) { return bswap_32((uint32_t)p); }
INLINE static uint64 swap64(uint64 p)  { return bswap_64((uint64_t)p); }

INLINE static void swap16(int16* p) { *p = bswap_16((uint16_t)*p); }
INLINE static void swap32(int32* p) { *p = bswap_32((uint32_t)*p); }
INLINE static void swap64(int64* p) { *p = bswap_64((uint64_t)*p); }

INLINE static int16 swap16(int16 p) { return bswap_16((uint16_t)p); }
INLINE static int32 swap32(int32 p) { return bswap_32((uint32_t)p); }
INLINE static int64 swap64(int64 p)  { return bswap_64((uint64_t)p); }

#else
#	define swap16(p) p
#	define swap32(p) p
#	define swap64(p) p
#	define swapfloat(p) p
#	define swapdouble(p) p
#endif

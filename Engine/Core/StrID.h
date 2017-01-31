#pragma once

//  The reference implementation for the code is within this documentation: http://www.faqs.org/rfcs/rfc1952.html
//	The reference implementation was turned into a recursive function

/**
* To use: create a string id from a string (without quotes) at compile time with SID(string)
* Using StrID GetStrID( const char* string ) will use the same method as SID, except it will
* Store the id in a map so the string can be looked up with const char* GetStringFromStrID( StrID id )
*
*/

#define STORE_IDS 1

//	Documentation Copyright:

/*
Copyright (c) 1996 L. Peter Deutsch

Permission is granted to copy and distribute this document for any
purpose and without charge, including translations into other
languages and incorporation into compilations, provided that the
copyright notice and this notice are preserved, and that any
substantive changes or deletions from the original are clearly
marked.

A pointer to the latest version of this and related documentation in
HTML format can be found at the URL
<ftp://ftp.uu.net/graphics/png/documents/zlib/zdoc-index.html>.
*/

#include <cstring>
#include <cstdint>
#include <iostream>

#define DEBUG_USE_STRING_AS_STRID

#ifdef DEBUG_USE_STRING_AS_STRID
typedef std::string StrID;
#else
typedef unsigned int StrID; /* Added By Brandon Svochak*/
#endif

							// Generate CRC lookup table
template <unsigned c, int k = 8>
struct f : f<((c & 1) ? 0xedb88320 : 0) ^ (c >> 1), k - 1> {};
template <unsigned c> struct f<c, 0> { enum { value = c }; };

#define CRC32_A(x) CRC32_B(x) CRC32_B(x + 128)
#define CRC32_B(x) CRC32_C(x) CRC32_C(x +  64)
#define CRC32_C(x) CRC32_D(x) CRC32_D(x +  32)
#define CRC32_D(x) CRC32_E(x) CRC32_E(x +  16)
#define CRC32_E(x) CRC32_F(x) CRC32_F(x +   8)
#define CRC32_F(x) CRC32_G(x) CRC32_G(x +   4)
#define CRC32_G(x) CRC32_H(x) CRC32_H(x +   2)
#define CRC32_H(x) CRC32_I(x) CRC32_I(x +   1)
#define CRC32_I(x) f<x>::value ,

/* Code below was modified and added by Brandon Svochak */
constexpr unsigned crc_table[] = { CRC32_A( 0 ) };

// Constexpr implementation and helpers
constexpr unsigned int crc32_impl( const unsigned char* p, size_t len, unsigned int crc ) {
	return len ?
		crc32_impl( p + 1, len - 1, (crc >> 8) ^ crc_table[(crc & 0xFF) ^ *p] )
		: crc;
}

constexpr unsigned int crc32( const uint8_t* data, size_t length ) {
	return ~crc32_impl( data, length, ~0 );
}

constexpr size_t strlen_c( const char* str ) {
	return *str ? 1 + strlen_c( str + 1 ) : 0;
}

#ifdef DEBUG_USE_STRING_AS_STRID
#define WSID(str) StrID(str)
#else
constexpr StrID WSID( const char* str ) {
	return (StrID)crc32( (unsigned char*)str, strlen_c( str ) );
}
#endif

namespace Hourglass
{
	namespace StrIDUtil
	{
		StrID GetStrID(const char* string);
		const char* GetStringFromStrID(StrID id);
	}
}

#if STORE_IDS
#define SID(string) Hourglass::StrIDUtil::GetStrID( #string )
#else
#define SID(string) WSID(#string)
#endif
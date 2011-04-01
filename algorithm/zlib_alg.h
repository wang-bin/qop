#ifndef ZLIB_ALG_H
#define ZLIB_ALG_H

#include "zlib.h"

extern "C" {
	int deflate_zlib(Bytef *dest, unsigned long *destLen, const Bytef *source, unsigned long sourceLen);
	int deflate_zlib(Bytef *dest, unsigned long *destLen, const Bytef *source, unsigned long sourceLen);
}

#endif // ZLIB_ALG_H

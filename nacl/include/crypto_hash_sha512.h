#ifndef crypto_hash_sha512_h
#define crypto_hash_sha512_h
#define crypto_hash_sha512_BYTES 64
int crypto_hash_sha512(unsigned char *out,const unsigned char *in,unsigned long long inlen);
#endif

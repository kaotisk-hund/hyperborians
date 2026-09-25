#ifndef crypto_hash_h
#define crypto_hash_h
#define crypto_hash_BYTES 64
int crypto_hash(unsigned char *out,const unsigned char *in,unsigned long long inlen);
#endif

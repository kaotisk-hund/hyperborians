#ifndef crypto_hash_sha256_h
#define crypto_hash_sha256_h
#define crypto_hash_sha256_BYTES 32
int crypto_hash_sha256(unsigned char *out,const unsigned char *in,unsigned long long inlen);
#endif

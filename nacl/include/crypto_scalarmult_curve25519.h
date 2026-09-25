#ifndef crypto_scalarmult_curve25519_h
#define crypto_scalarmult_curve25519_h
#define crypto_scalarmult_curve25519_BYTES 32
#define crypto_scalarmult_curve25519_SCALARBYTES 32
int crypto_scalarmult_curve25519(unsigned char *q,const unsigned char *n,const unsigned char *p);
int crypto_scalarmult_curve25519_base(unsigned char *q,const unsigned char *n);
#endif

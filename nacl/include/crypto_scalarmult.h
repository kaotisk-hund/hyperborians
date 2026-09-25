#ifndef crypto_scalarmult_h
#define crypto_scalarmult_h
#define crypto_scalarmult_BYTES 32
#define crypto_scalarmult_SCALARBYTES 32
int crypto_scalarmult(unsigned char *q,const unsigned char *n,const unsigned char *p);
int crypto_scalarmult_base(unsigned char *q,const unsigned char *n);
#endif

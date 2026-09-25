#ifndef crypto_scalarmult_h
#define crypto_scalarmult_h
int crypto_scalarmult(unsigned char *q,const unsigned char *n,const unsigned char *p);
int crypto_scalarmult_base(unsigned char *q,const unsigned char *n);
#endif
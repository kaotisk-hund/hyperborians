#ifndef crypto_box_h
#define crypto_box_h
#define crypto_box_PUBLICKEYBYTES 32
#define crypto_box_SECRETKEYBYTES 32
#define crypto_box_BEFORENMBYTES 32
#define crypto_box_NONCEBYTES 24
#define crypto_box_ZEROBYTES 32
#define crypto_box_BOXZEROBYTES 16
int crypto_box(unsigned char *c,const unsigned char *m,unsigned long long mlen,const unsigned char *n,const unsigned char *pk,const unsigned char *sk);
int crypto_box_open(unsigned char *m,const unsigned char *c,unsigned long long clen,const unsigned char *n,const unsigned char *pk,const unsigned char *sk);
int crypto_box_beforenm(unsigned char *k,const unsigned char *pk,const unsigned char *sk);
int crypto_box_afternm(unsigned char *c,const unsigned char *m,unsigned long long mlen,const unsigned char *n,const unsigned char *k);
int crypto_box_open_afternm(unsigned char *m,const unsigned char *c,unsigned long long clen,const unsigned char *n,const unsigned char *k);
int crypto_box_keypair(unsigned char *pk,unsigned char *sk);
#endif

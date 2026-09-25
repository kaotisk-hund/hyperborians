#ifndef crypto_secretbox_h
#define crypto_secretbox_h
#define crypto_secretbox_KEYBYTES 32
#define crypto_secretbox_NONCEBYTES 24
#define crypto_secretbox_ZEROBYTES 32
#define crypto_secretbox_BOXZEROBYTES 16
int crypto_secretbox(unsigned char *c,const unsigned char *m,unsigned long long mlen,const unsigned char *n,const unsigned char *k);
int crypto_secretbox_open(unsigned char *m,const unsigned char *c,unsigned long long clen,const unsigned char *n,const unsigned char *k);
#endif

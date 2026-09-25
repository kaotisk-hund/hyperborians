#ifndef crypto_secretbox_h
#define crypto_secretbox_h
int crypto_secretbox(unsigned char *c,const unsigned char *m,unsigned long long mlen,const unsigned char *n,const unsigned char *k);
int crypto_secretbox_open(unsigned char *m,const unsigned char *c,unsigned long long clen,const unsigned char *n,const unsigned char *k);
#endif
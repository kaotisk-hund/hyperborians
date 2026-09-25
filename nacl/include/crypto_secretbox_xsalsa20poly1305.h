#ifndef crypto_secretbox_xsalsa20poly1305_h
#define crypto_secretbox_xsalsa20poly1305_h
#define crypto_secretbox_xsalsa20poly1305_KEYBYTES 32
#define crypto_secretbox_xsalsa20poly1305_NONCEBYTES 24
#define crypto_secretbox_xsalsa20poly1305_ZEROBYTES 32
#define crypto_secretbox_xsalsa20poly1305_BOXZEROBYTES 16
int crypto_secretbox_xsalsa20poly1305(unsigned char *c,const unsigned char *m,unsigned long long mlen,const unsigned char *n,const unsigned char *k);
int crypto_secretbox_xsalsa20poly1305_open(unsigned char *m,const unsigned char *c,unsigned long long clen,const unsigned char *n,const unsigned char *k);
#endif

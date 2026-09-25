#ifndef crypto_box_curve25519xsalsa20poly1305_h
#define crypto_box_curve25519xsalsa20poly1305_h
#define crypto_box_curve25519xsalsa20poly1305_PUBLICKEYBYTES 32
#define crypto_box_curve25519xsalsa20poly1305_SECRETKEYBYTES 32
#define crypto_box_curve25519xsalsa20poly1305_BEFORENMBYTES 32
#define crypto_box_curve25519xsalsa20poly1305_NONCEBYTES 24
#define crypto_box_curve25519xsalsa20poly1305_ZEROBYTES 32
#define crypto_box_curve25519xsalsa20poly1305_BOXZEROBYTES 16
int crypto_box_curve25519xsalsa20poly1305(unsigned char *c,const unsigned char *m,unsigned long long mlen,const unsigned char *n,const unsigned char *pk,const unsigned char *sk);
int crypto_box_curve25519xsalsa20poly1305_open(unsigned char *m,const unsigned char *c,unsigned long long clen,const unsigned char *n,const unsigned char *pk,const unsigned char *sk);
int crypto_box_curve25519xsalsa20poly1305_beforenm(unsigned char *k,const unsigned char *pk,const unsigned char *sk);
int crypto_box_curve25519xsalsa20poly1305_afternm(unsigned char *c,const unsigned char *m,unsigned long long mlen,const unsigned char *n,const unsigned char *k);
int crypto_box_curve25519xsalsa20poly1305_open_afternm(unsigned char *m,const unsigned char *c,unsigned long long clen,const unsigned char *n,const unsigned char *k);
int crypto_box_curve25519xsalsa20poly1305_keypair(unsigned char *pk,unsigned char *sk);
#endif

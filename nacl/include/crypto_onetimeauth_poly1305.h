#ifndef crypto_onetimeauth_poly1305_h
#define crypto_onetimeauth_poly1305_h
#define crypto_onetimeauth_poly1305_BYTES 16
#define crypto_onetimeauth_poly1305_KEYBYTES 32
int crypto_onetimeauth_poly1305(unsigned char *out,const unsigned char *in,unsigned long long inlen,const unsigned char *k);
int crypto_onetimeauth_poly1305_verify(const unsigned char *h,const unsigned char *in,unsigned long long inlen,const unsigned char *k);
#endif

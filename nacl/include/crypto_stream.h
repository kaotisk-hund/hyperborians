#ifndef crypto_stream_h
#define crypto_stream_h
#define crypto_stream_KEYBYTES 32
#define crypto_stream_NONCEBYTES 24
int crypto_stream(unsigned char *c,unsigned long long clen,const unsigned char *n,const unsigned char *k);
int crypto_stream_xor(unsigned char *c,const unsigned char *m,unsigned long long mlen,const unsigned char *n,const unsigned char *k);
#endif

#ifndef crypto_core_h
#define crypto_core_h
#define crypto_core_OUTPUTBYTES 32
#define crypto_core_INPUTBYTES 16
#define crypto_core_KEYBYTES 32
#define crypto_core_CONSTBYTES 16
int crypto_core(unsigned char *out,const unsigned char *in,const unsigned char *k,const unsigned char *c);
#endif

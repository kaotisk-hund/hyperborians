#ifndef crypto_core_salsa20_h
#define crypto_core_salsa20_h
#define crypto_core_salsa20_OUTPUTBYTES 64
#define crypto_core_salsa20_INPUTBYTES 16
#define crypto_core_salsa20_KEYBYTES 32
#define crypto_core_salsa20_CONSTBYTES 16
int crypto_core_salsa20(unsigned char *out,const unsigned char *in,const unsigned char *k,const unsigned char *c);
#endif

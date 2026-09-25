#ifndef crypto_core_hsalsa20_h
#define crypto_core_hsalsa20_h
#define crypto_core_hsalsa20_OUTPUTBYTES 32
#define crypto_core_hsalsa20_INPUTBYTES 16
#define crypto_core_hsalsa20_KEYBYTES 32
#define crypto_core_hsalsa20_CONSTBYTES 16
int crypto_core_hsalsa20(unsigned char *out,const unsigned char *in,const unsigned char *k,const unsigned char *c);
#endif

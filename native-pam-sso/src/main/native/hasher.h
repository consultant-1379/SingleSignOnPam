#ifndef HASHER_H
#define	HASHER_H

#ifdef	__cplusplus
extern "C" {
#endif
#define SHA256_DIGEST_LENGTH 32

    unsigned char * keyedHash(unsigned char* key, unsigned char* data, int data_len);
    unsigned char * keyedHash_simple(unsigned char* key, unsigned char* data);
    unsigned char * sha256(const unsigned char* text);

#ifdef	__cplusplus
}
#endif

#endif	/* HASHER_H */


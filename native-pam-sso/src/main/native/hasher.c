#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include "hasher.h"
#include "debug.h"
#include "base64.h"


unsigned char * keyedHash(unsigned char* key, unsigned char* data, int data_len) {
    DBG("start keyedHash");
    unsigned char* result;
    DBG2("keyedHashing: datalength= %02d keylength= %02d", data_len, strlen(key));
    result = HMAC(EVP_sha256(), key, strlen(key), data, data_len, NULL, NULL);
    DBG1("keyedHash = %s", hexencode(result, SHA256_DIGEST_LENGTH));
    return result;
}

unsigned char * keyedHash_simple(unsigned char* key, unsigned char* data) {
    DBG("start keyedHash_simple");
    unsigned char* result;
    result = keyedHash(key, data, strlen(data));
    return result;
}

unsigned char * sha256(const unsigned char* text) {
    DBG("start SHA256");
    unsigned char *obuf = (unsigned char *) malloc(SHA256_DIGEST_LENGTH);
    SHA256(text, strlen(text), obuf);

    DBG1("sha256 = %s", hexencode(obuf, SHA256_DIGEST_LENGTH));

    return obuf;
}

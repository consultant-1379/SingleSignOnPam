#include <string.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

#include "debug.h"

unsigned char * hexencode(unsigned char *decoded, int length) {
    DBG("start hexencode");
    unsigned char *buffer = (unsigned char *) malloc(length * 2 + 1);
    int i;
    for (i = 0; i < length; i++) {
        sprintf(buffer + (i * 2), "%02x", decoded[i]);
    }
    buffer[length * 2] = '\0';
    return buffer;
}

char *base64encode(const unsigned char *input, int length) {
    DBG("start base64encode");
    BIO *bmem, *b64;
    BUF_MEM *bptr;

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, input, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    char *buff = (char *) malloc(bptr->length);
    memcpy(buff, bptr->data, bptr->length - 1);
    buff[bptr->length - 1] = 0;

    BIO_free_all(b64);

    return buff;
}
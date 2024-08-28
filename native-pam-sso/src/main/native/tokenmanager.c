#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "tokenmanager.h"
#include "debug.h"

unsigned long extract_counter(char * token) {
    DBG("start extract counter");
    DBG1("Token = %s", token);
    unsigned long counter;
    char *seperator = strchr(token, TOKEN_SEPERATOR);
    if (seperator == NULL) {
        DBG1("Can't extract counter. token not found %c", TOKEN_SEPERATOR);
        return NULL;
    }
    int length = seperator - token;
    unsigned char *counterstring = (unsigned char *) malloc(length + 1);
    int i;
    for (i = 0; i < length; ++i) {
        counterstring[i] = token[i];
    }
    counterstring[length] = '\0';
    counter = atol(counterstring);
    return counter;
}

char * extract_HMAC(char * token) {
    DBG("start extract HMAC");
    char *seperator = strchr(token, TOKEN_SEPERATOR);
    if (seperator == NULL) {
        DBG1("Can't extract HMAC. token not found %c", TOKEN_SEPERATOR);
        return NULL;
    }
    int length = strlen(token) - (seperator - token);
    char *hmac = (char *) malloc((length + 1) * sizeof (char));
    int i;
    for (i = 0; i < length; ++i) {
        hmac[i] = *(++seperator);
    }
    hmac[length] = '\0';
    return hmac;
}

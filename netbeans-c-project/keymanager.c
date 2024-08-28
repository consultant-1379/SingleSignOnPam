
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <openssl/evp.h>
#include <openssl/sha.h>
#include "filemanager.h"
#include "keymanager.h"
#include "base64.h"
#include "debug.h"
#include "hasher.h"

unsigned char * get_master_key() {
    DBG("start get master key");
    unsigned char *key = read_first_line(MASTER_KEY_FILE);
    if (key == NULL) {
        ERR("can't get MASTER KEY");
        return NULL;
    }
    return key;
}

unsigned long get_current_counter() {
    DBG("start get current counter");
    unsigned long counter = get_number(COUNTER_FILE);
    return counter;
}

void update_counter(const unsigned long newcounter) {
    write_number(COUNTER_FILE, newcounter);
}

struct tm * get_newest_time_slot() {
    struct tm *time_start;
    time_t timer;
    timer = time(NULL);
    time_start = localtime(&timer);
    time_start->tm_sec = 0;
    mktime(time_start);
    DBG1("The new timeslot starts at %s", asctime(time_start));
    return time_start;
}

struct tm * get_oldest_time_slot() {
    struct tm *time_start;
    time_t timer;
    timer = time(NULL);
    time_start = localtime(&timer);
    time_start->tm_sec = 0;
    time_start->tm_min -= (FRAME_SIZE_MINUTES / 2);
    mktime(time_start);
    DBG1("The old timeslot starts at %s", asctime(time_start));
    return time_start;
}

unsigned char *generate_session_key(struct tm *timeslot, unsigned long counter) {
    DBG1("start generate session key: %lu", counter);

    unsigned char *data = (unsigned char *) malloc(10); // 6 bytes for the date 4 bytes for the counter

    data[0] = (unsigned char) (timeslot->tm_year / 100 + 19); //tm_year is years since 1900
    data[1] = (unsigned char) (timeslot->tm_year % 100);
    data[2] = (unsigned char) (timeslot->tm_mon + 1); // January is 0, we want it to be 1
    data[3] = (unsigned char) timeslot->tm_mday;
    data[4] = (unsigned char) timeslot->tm_hour;
    data[5] = (unsigned char) timeslot->tm_min;

    data[6] = (unsigned char) (counter >> (8 * 3));
    data[7] = (unsigned char) (counter >> (8 * 2));
    data[8] = (unsigned char) (counter >> (8 * 1));
    data[9] = (unsigned char) counter;

    DBG1("%s", hexencode(data, 10));

    unsigned char *master_key = NULL;
    master_key = get_master_key();
    if (master_key == NULL) {
        DBG1("Unable to get master_key errno = %d", errno);
        return master_key;
    }
    unsigned char *hmac = keyedHash(master_key, &data[0], 10);

    return hmac;
}

unsigned char *generate_mac(struct tm *timeslot, char *userid, unsigned long counter) {
    DBG2("start generate mac with for %s %lu", userid, counter);

    unsigned char * session_key = NULL;
    session_key = generate_session_key(timeslot, counter);
    if (session_key == NULL) {
        DBG1("Unable to get master_key errno = %d", errno);
        return session_key;
    }
    unsigned char * session_key_hex = hexencode(session_key, SHA256_DIGEST_LENGTH);
    DBG1("session_key_hex = %s", session_key_hex);
    free(session_key_hex);

    unsigned char counter_string[64];
    sprintf(counter_string, "%lu", counter);
    DBG1("counter as string = \'%s\'", counter_string);

    unsigned char * hmac_data = (unsigned char *) malloc(strlen(userid) + strlen(counter_string) + 1);
    strcpy(hmac_data, userid);
    strcat(hmac_data, counter_string);
    DBG1("hmac_data = \'%s\'", hmac_data);
    unsigned char *hmac = keyedHash_simple(session_key, hmac_data);
    free(hmac_data);
    return hmac;
}

unsigned int validate_mac(char * given_mac, char * userid, unsigned long counter) {
    DBG3("start validate MAC: %s %s %lu ", given_mac, userid, counter);

    struct tm* oldslot = get_oldest_time_slot();
    unsigned char *oldmac = generate_mac(oldslot, userid, counter);
    if (oldmac == NULL) {
        ERR("validation failed, can't generate MAC");
        return INVALID;
    }
    char *oldmacbase64 = base64encode(oldmac, SHA256_DIGEST_LENGTH);
    DBG1("old MAC: %s", oldmacbase64);
    if (strcmp(given_mac, oldmacbase64) == 0) {
        free(oldmacbase64);
        return VALID;
    }

    struct tm* newslot = get_newest_time_slot();
    unsigned char *recentmac = generate_mac(newslot, userid, counter);
    if (oldmac == NULL) {
        ERR("validation failed, can't generate MAC");
        return INVALID;
    }
    char *recentmacbase64 = base64encode(recentmac, SHA256_DIGEST_LENGTH);
    DBG1("recent MAC: %s", recentmacbase64);
    if (strcmp(given_mac, recentmacbase64) == 0) {
        free(recentmacbase64);
        return VALID;
    }
    free(oldmacbase64);
    free(recentmacbase64);
    return INVALID;
}

unsigned int validate_counter(unsigned long given_counter) {
    DBG1("start validate counter: %lu", given_counter);
    unsigned long current_counter = get_current_counter();
    if (given_counter >= (current_counter-1)) {
        return VALID;
    }
    return INVALID;
}


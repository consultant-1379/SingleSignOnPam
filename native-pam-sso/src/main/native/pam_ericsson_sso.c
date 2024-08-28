#define PAM_SM_AUTH

#include <stdio.h>
#include <stdarg.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <syslog.h>

#include "tokenmanager.h"
#include "error.h"
#include "debug.h"
#include "keymanager.h"

#include <stdlib.h>
#include <libgen.h>
#include <memory.h>
#include <errno.h>
#include <syslog.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/pkcs12.h>
#include <openssl/err.h>
#include <sys/stat.h>
#include <pwd.h>
#include <rpcsvc/ypclnt.h>
#include <rpcsvc/yp_prot.h>


#ifndef FALSE
#define FALSE (0)
#define TRUE (!(FALSE))
#endif


char * MASTER_KEY_FILE = "/var/tmp/pam_sso_key";
char * COUNTER_FILE = "/var/tmp/pam_sso_counter";
extern void set_debug_level(int level);

int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    int retval;
    const char *user; /* User Name */
    const char *old_pwd; /* Old Password */
    
    int i;
    for (i = 0; i < argc; i++) {
        if (strncmp(argv[i], "keyfile=", 8) == 0) {
            MASTER_KEY_FILE = argv[i] + 8;
        } else if (strncmp(argv[i], "cntfile=", 8) == 0) {
            COUNTER_FILE = argv[i] + 8;
        } else if (strncmp(argv[i], "debug", 5) == 0) {
            set_debug_level(1);
        }
        else {
            DBG1("Unknown parameter: %s", argv[i]);
        }
    }

    DBG("Start Authenticate.");

    retval = pam_get_user(pamh, &user, NULL);
    DBG1("username = %s", user);

    if (retval != PAM_SUCCESS) {
        DBG("Failed to get username");
        return retval;
    }
    DBG1("username = %s", user);

    retval = pam_get_item(pamh, PAM_AUTHTOK, (void**) &old_pwd);
    if (retval != PAM_SUCCESS) {
        DBG("Failed to get password token");
        return retval;
    }
    DBG1("password = %s", old_pwd);

    // 1. validate counter -> if not valid -> abort
    // 2. validate HMAC with slot 1 or slot 2
    // 3. update counter if valid.

    unsigned long counter = extract_counter(old_pwd);
    if (counter == NULL) {
        DBG("Counter couldn't be extracted from password");
        return PAM_IGNORE;
    }
    DBG1("counter given = %lu", counter);

    char *hmac = extract_HMAC(old_pwd);
    if (hmac == NULL) {
        DBG("MAC couldn't be extracted from password");
        return PAM_IGNORE;
    }
    DBG1("MAC given = %s", hmac);

    if (validate_counter(counter) == INVALID) {
        DBG1("Given counter too small: %lu", counter);
        return PAM_IGNORE;
    }

    int macvalid = validate_mac(hmac, user, counter);
    if (macvalid == INVALID) {
        DBG1("MAC doesn't validate: %s", hmac);
        return PAM_IGNORE;
    }
    
    unsigned long currentcounter = get_current_counter();
    
    if (counter > currentcounter){
    update_counter(counter);
    }
    
    DBG ("Token validated, returning PAM_SUCCESS");
    
    DBG("attempting to clear PAM_AUTHTOK");
    retval = pam_set_item(pamh, PAM_AUTHTOK, NULL);
    
    return (PAM_SUCCESS);
}

int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return (PAM_IGNORE);
}

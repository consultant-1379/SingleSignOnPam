#define PAM_SM_AUTH

#include <stdio.h>
#include <stdarg.h>

int pam_sm_authenticate(int *pamh, int flags, int argc, const char **argv) {

    return (1);
}

int pam_sm_setcred(int *pamh, int flags, int argc, const char **argv) {
    return (1);
}

/*
 * PKCS #11 PAM Login Module
 * Copyright (C) 2003 Mario Strasser <mast@gmx.net>,
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * $Id: debug.c 411 2010-04-07 13:52:14Z ludovic.rousseau $
 */

#include "debug.h"
#include <stdarg.h>
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>

/* current debug level */
static int debug_level = 0;

void set_debug_level(int level) {
    debug_level = level;
}

int get_debug_level(void) {
    return debug_level;
}

void debug_print(int level, const char *file, int line, const char *format, ...) {
    va_list ap;
    if (debug_level >= level) {
        /* is stdout is a tty */
        
            /* else we use syslog(3) */
            char buf[100];

            /* print message */
            va_start(ap, format);
            vsnprintf(buf, sizeof (buf), format, ap);
            va_end(ap);

            syslog(LOG_INFO, "%s", buf);
        }
}

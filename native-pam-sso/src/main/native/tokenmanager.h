
#ifndef TOKENMANAGER_H
#define	TOKENMANAGER_H

#ifdef	__cplusplus
extern "C" {
#endif

#define TOKEN_SEPERATOR '-'

unsigned long extract_counter( char * token);
 char * extract_HMAC( char * token);

#ifdef	__cplusplus
}
#endif

#endif	/* TOKENMANAGER_H */


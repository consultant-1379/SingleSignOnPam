/* 
 * File:   base64.h
 * Author: erenmul
 *
 * Created on 03 December 2012, 17:09
 */

#ifndef BASE64_H
#define	BASE64_H

#ifdef	__cplusplus
extern "C" {
#endif

    unsigned char * hexencode(unsigned char *decoded, int length);
    char *base64encode(const unsigned char *input, int length);
#ifdef	__cplusplus
}
#endif

#endif	/* BASE64_H */



#ifndef FILEMANAGER_H
#define	FILEMANAGER_H

#ifdef	__cplusplus
extern "C" {
#endif

    unsigned char * read_first_line(char * filename);
    
    unsigned long get_number(char * filename);

#ifdef	__cplusplus
}
#endif

#endif	/* FILEMANAGER_H */


#ifndef KEYMANAGER_H
#define	KEYMANAGER_H

#ifdef	__cplusplus
extern "C" {
#endif

#define FRAME_SIZE_MINUTES 2
#define VALID 0
#define INVALID 1
    
    extern char * MASTER_KEY_FILE;
    extern char * COUNTER_FILE;

    unsigned char * get_master_key();

    struct tm* get_newest_time_slot();
    struct tm* get_oldest_time_slot();
    unsigned long get_current_counter();
    void update_counter(const unsigned long counter);

#ifdef	__cplusplus
}
#endif

#endif	/* KEYMANAGER_H */


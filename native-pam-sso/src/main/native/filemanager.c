#include <stdio.h>
#include <string.h>

#include "filemanager.h"
#include "debug.h"

unsigned char * read_first_line(char * filename) {
    DBG("start read first line");
    FILE *input_file;
    input_file = fopen(filename, "r");
    if (input_file == NULL) {
        ERR1("unable to open file: %s", filename);
        return NULL;
    }
    unsigned char *line = (unsigned char*) malloc(128);
    fgets(line, 128, input_file);
    fclose(input_file);
    unsigned char *carriage_return = strchr(line, '\n');
    if (carriage_return != NULL) {
        carriage_return[0] = '\0';
    }
    DBG2("read %s from %s", line, filename);
    return line;
}

unsigned long get_number(char * filename) {
    DBG("start get number");
    FILE *input_file;
    unsigned long counter;

    input_file = fopen(filename, "r");
    if (input_file == NULL) {
        ERR1("unable to open file: %s", filename);
        return NULL;
    }
    fscanf(input_file, "%lu", &counter);
    DBG2("read %lu from %s", counter, filename);
    fclose(input_file);
    return counter;
}

void write_number(char * filename, unsigned long counter) {
    DBG("start write number");
    FILE *output_file;
    output_file = fopen(filename, "w");
    if (output_file == NULL) {
        ERR1("unable to open file for writing: %s", filename);
        return NULL;
    }
    fprintf(output_file, "%lu", counter);
    fclose(output_file);
}


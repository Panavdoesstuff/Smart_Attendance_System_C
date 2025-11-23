#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>

void trim(char *s);
void input_line(char *buf, int buflen);
void print_header(const char *title);
void note_filename_for_subject(const char *subject, char *out, int out_len);

#endif
#include "utils.h"

/* =============================================================
   UTILITIES
   ============================================================= */

void trim(char *s) {
    int n = (int)strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r')) s[--n] = '\0';
    while (n > 0 && s[n-1] == ' ') s[--n] = '\0';
}

/* Make note filename for a subject, e.g. notes_Coding.txt */
void note_filename_for_subject(const char *subject, char *out, int out_len) {
    snprintf(out, out_len, "notes_%s.txt", subject);
}

/* Safe fgets into buffer and trim */
void input_line(char *buf, int buflen) {
    if (!fgets(buf, buflen, stdin)) { buf[0] = '\0'; return; }
    trim(buf);
}

/* Print textbook-style header (replaces boxed title) */
void print_header(const char *title) {
    int width = 60;
    printf("\n");
    for (int i = 0; i < width; ++i) putchar('=');
    printf("\n");
    /* center title roughly */
    int padding = (width - (int)strlen(title)) / 2;
    if (padding < 0) padding = 0;
    for (int i = 0; i < padding; ++i) putchar(' ');
    printf("%s\n", title);
    for (int i = 0; i < width; ++i) putchar('=');
    printf("\n\n");
}
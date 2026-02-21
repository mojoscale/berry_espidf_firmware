#include "esp_system.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static const char *TAG = "BERRY";

// ============ PROVIDE MISSING SYMBOLS ============

// File I/O functions (using stdio which works in ESP-IDF)
void *be_fopen(const char *filename, const char *mode) {
    ESP_LOGD(TAG, "fopen: %s", filename);
    return fopen(filename, mode);
}

void be_fclose(void *file) {
    if (file) fclose((FILE*)file);
}

size_t be_fread(void *ptr, size_t size, size_t nmemb, void *file) {
    return file ? fread(ptr, size, nmemb, (FILE*)file) : 0;
}

size_t be_fwrite(const void *ptr, size_t size, size_t nmemb, void *file) {
    return file ? fwrite(ptr, size, nmemb, (FILE*)file) : 0;
}

int be_fseek(void *file, long offset, int whence) {
    return file ? fseek((FILE*)file, offset, whence) : -1;
}

long be_ftell(void *file) {
    return file ? ftell((FILE*)file) : -1;
}

long be_fsize(void *file) {
    if (!file) return -1;
    FILE *f = (FILE*)file;
    long pos = ftell(f);
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, pos, SEEK_SET);
    return size;
}

int be_fflush(void *file) {
    return file ? fflush((FILE*)file) : 0;
}

char *be_fgets(char *s, int size, void *file) {
    return file ? fgets(s, size, (FILE*)file) : NULL;
}

// Console I/O
void be_writebuffer(const char *buffer, int length) {
    for (int i = 0; i < length; i++) {
        putchar(buffer[i]);
    }
}

int be_readstring(char *buffer, int max_len) {
    if (fgets(buffer, max_len, stdin) == NULL) return 0;
    int len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n') {
        buffer[len-1] = '\0';
        len--;
    }
    return len;
}

// Module and class tables (empty by default - override in your code)
typedef struct { const char *name; void *ptr; } be_table_entry_t;

// These will be overridden by strong symbols in your main code
__attribute__((weak)) be_table_entry_t be_module_table[] = {{NULL, NULL}};
__attribute__((weak)) be_table_entry_t be_class_table[] = {{NULL, NULL}};

// ============ FIX THE COMPILER WARNING ============
// This fixes the isalnum() warning without modifying Berry code
// The warning occurs because isalnum() expects unsigned char
// We can add a compiler flag to ignore this specific warning
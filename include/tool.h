#ifndef toolH
#define toolH

#include <stdio.h>
extern char strada_error[256];
#define MAX_CONFIG_LENGTH 128
///////////////////////////////////////////////////////////////////////////////
void conv_upper(char* str);
char* get_ext(const char* fname, char* ext, size_t n);
char* get_path(const char* fname, char* target, size_t n);
void set_fname(const char* f_str, char* dst_str, size_t n, const char* f_ext);
char* getKey(char* key, size_t n, const char* s1, const char* s2);
char* trim(char* str);
int getbufInt(const char* buf, int p, int c);
float getbufFlt(const char* buf, int p, int c);
double getbufDbl(char* buf, int p, int c);
bool repstrrtn(char* buf, int c);	// replace RET(\n) with NULL
void chomp(char* buf);
int maxlinelen(FILE* fp, char* buffer, int size); // Calculate the max numbers of text file
char* float_print(char* buf, size_t n, float x);// Number of significant digits = 4 and width = 5
char* fixfloat(char* buff, double value, int width, int ndig = 7);
char* fixfloat(char* buff, float value, int width, int ndig=7);
bool getconfig(const char* fname, char* key, char* dst, size_t n);
char* strnstr(char* target, char* s, int size);

void dqconv(char* dest, size_t n, char* source);
char* csv_parser(char* input, char** array, int c, char SEP, char DIG);

#endif

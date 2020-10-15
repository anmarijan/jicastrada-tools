#ifndef toolH
#define toolH

#include <stdio.h>
extern char strada_error[256];
#define MAX_CONFIG_LENGTH 128
///////////////////////////////////////////////////////////////////////////////
void conv_upper(char* str);
char* get_ext(const char* fname, char* ext);
char* get_path(const char* fname, char* target);
void set_fname(const char* f_str, char* dst_str, const char* f_ext);
char* getKey(char* key,const char* s1, const char* s2);
char* substring(char* dst, const char* buff,int s, int e);
char* trim(char* str, int max);	//文字の前後から空白を取り除く
char* trim(char* str);
int getbufInt(char* buf, int p, int c);
float getbufFlt(char* buf, int p, int c);
double getbufDbl(char* buf, int p, int c);
bool repstrrtn(char* buf, int c);	//改行をNULLに変換する
void chomp(char* buf);
int maxlinelen(FILE* fp, char* buffer, int size); //テキストファイルの一行の最大値を計算する。
char* float_print(char* buf, float x);//有効桁数４印字幅５
char* fixfloat(char* buff, float value, int width, int ndig=7);
bool getconfig(const char* fname, char* key, char* dst);
char* strnstr(char* target, char* s, int size);

void dqconv(char* source, char* dest, int maxlen);
char* csv_parser(char* input, char** array, int c, char SEP, char DIG);

#endif

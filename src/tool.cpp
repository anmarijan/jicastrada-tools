#include <pch.h>
//------------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cmath>
#include <assert.h>
#include <ctype.h>
#include <limits.h>
//------------------------------------------------------------------------------
#include <stdexcept>
//------------------------------------------------------------------------------
#include "tool.h"
char strada_error[256];
////////////////////////////////////////////////////////////////////////////////
// alternative of strupr
////////////////////////////////////////////////////////////////////////////////
void conv_upper(char* str) {
	while( *str ) {
		*str = toupper((int)*str);
		str++;
	}
}
//------------------------------------------------------------------------------
char* get_ext(const char* fname, char* str) {

	const char* p;
	const char* q;
	p = fname;
	q = NULL;
	while( *p ) {
		if( *p == '.' ) q = p;
		p++;
	}
	if( q == NULL ) {
		str[0] = '\0';
	} else {
		strcpy_s(str, sizeof(str), q+1);
	}
	return str;
}
//------------------------------------------------------------------------------
char* get_path(const char* fname, char* target) {
    int pos = 0;
    int i = 0;
    strcpy_s(target, sizeof(target), fname);
    while (target[i] != 0 ) {
        if( target[i] == '\\' || target[i] == '/' ) pos = i;
        i++;
    }
    target[pos] = 0;
    return target;
}
//------------------------------------------------------------------------------
// change the extension
//------------------------------------------------------------------------------
void set_fname(const char* f_str, char* dst_str, const char* f_ext) {

	int len, pos;
	len = strlen(f_str);
	if (len < 3) return;
	pos = len - 1;
	while(pos>0) {
		if( f_str[pos] == '.' ) break;
		pos--;
	}
	if( pos == 0 ) sprintf_s(dst_str, sizeof(dst_str), "%s.%s", f_str, f_ext);
	else {
		strncpy_s(dst_str, sizeof(dst_str), f_str, pos + 1);
		dst_str[pos+1] = '\0';
		strcat_s(dst_str,sizeof(dst_str),f_ext);
	}
}
////////////////////////////////////////////////////////////////////////////////
char* getKey(char* key,const char* s1, const char* s2){
	sprintf_s(key,sizeof(key),"%s#AND#%s",s1,s2);
	return key;
}

char* substring(char* dst, const char* buff,int s, int e){
	strncpy_s(dst,sizeof(dst),&buff[s],e-s); dst[e-s] = '\0';
	return dst;
}
//------------------------------------------------------------------------------
// max: max size of the string
//------------------------------------------------------------------------------
char* trim(char* str, int max){
	int a = -1;
	if( max <= 0 ) {
    	str[0] = '\0';
        return str;
    }
    int len = strlen(str)+1;
    if( max > len ) max = len;

	for(int i=0; i< max-1; i++){
		if(str[i] == ' ') continue;
		else {
			a = i;
			break;
		}
	}
    if( a == -1 ) { // all spaces
        for(int i=0; i < max; i++) str[i] = '\0';
        return str;
    }
    int b = max;
    for(int i=max-1; i > 0; i--) {	//max includes NULL
        if( str[i-1] == ' ' ) continue;
        else {
            b = i;
            break;
        }
    }
	int c = 0;
	for(int i=a; i < b; c++,i++){
		str[c] = str[i];
	}
	while(c < max) str[c++] = '\0';
	return str;
}

char* trim(char* str) {
	int s , e;
	int len, i;
	len = strlen(str);

	for( s = 0; s < len; s++) {
		if( str[s] != ' ' && str[s] != '\t' ) break;
	}
	for( e = len-1; e >= s ; e-- ) {
		if( str[e] != ' ' && str[e] != '\t' ) break;
	}

	for(i=0; i < e - s + 1; i++) str[i] = str[s+i];
	str[i] = '\0';
	return str;
}
//------------------------------------------------------------------------------
//  Convert a part of a string to integer
// buf string buffer
// p   start point
// c   number of chars from the start point
//------------------------------------------------------------------------------
int getbufInt(const char* buf, int p, int c){
	char dst[20];
	char *endptr;
	long n;
	int ret;
	assert( c < 20 && c > 0 && p >= 0);
	strncpy_s(dst, sizeof(dst),&buf[p],c);
    dst[c] = '\0';
    trim(dst, 20);
	n = strtol(dst, &endptr, 0);
	if( *endptr != '\0' ) {
		fprintf(stderr, "try to convert integer from [%s]\n", endptr);
		throw std::runtime_error("Integer convert error");
	}
	if( n < INT_MIN || n > INT_MAX ) throw std::out_of_range("getbufInt");
	ret = (int) n;
	return ret;
}
//------------------------------------------------------------------------------
// Convert a part of a string to float
//------------------------------------------------------------------------------
float getbufFlt(const char* buf, int p, int c){
	char dst[20];
	char msg[40];
    char *endptr;
	float f;
	assert( c < 20 && c > 0 && p >= 0);
	strncpy_s(dst, sizeof(dst),&buf[p],c);
    dst[c] = '\0';
    trim(dst, 20);
    f = (float)strtod(dst, &endptr);
    if( *endptr != '\0' ) {
		sprintf_s(msg, sizeof(msg), "getbufFlt:%s", dst);
		throw std::runtime_error(msg);
	}
	return f;
}
//------------------------------------------------------------------------------
// Convert a part of a string to double
//------------------------------------------------------------------------------
double getbufDbl(char* buf, int p, int c){
	char dst[20];
	char msg[40];
    char *endptr;
	double f;
	assert( c < 20 && c > 0 && p >= 0);
	strncpy_s(dst, sizeof(dst),&buf[p],c);
    dst[c] = '\0';
    trim(dst, 20);
    f = strtod(dst, &endptr);
    if( *endptr != '\0' ) {
		sprintf_s(msg, sizeof(msg), "getbufDbl:%s", dst);
		throw std::runtime_error(msg);
	}
	return f;
}
//------------------------------------------------------------------------------
// replace RET to NULL (max c)
//------------------------------------------------------------------------------
bool repstrrtn(char* buf, int c){
	int i ;
	for(i=0; i < c; i++) {
		if (buf[i] == '\0') break;
		if(buf[i] == '\n'){
			buf[i] = '\0';
			break;
		}
	}
	if( i == c ) return false;
	return true;
}
//------------------------------------------------------------------------------
// change the last char to '\0'
//------------------------------------------------------------------------------
void chomp(char* buf) {
	int len = strlen(buf);
	if( len > 0 ) buf[len-1] = '\0';
}
//------------------------------------------------------------------------------
// calculate the max length of line
//------------------------------------------------------------------------------
int maxlinelen(FILE* fp, char* buffer, int size) {
	int slen;
	int maxlen = 0;
	if (buffer == NULL) return 0;
	while ( fgets(buffer, size, fp) != NULL ) {
		slen = strlen(buffer);
		maxlen = maxlen + slen;
	}

	fseek(fp, 0L, SEEK_SET);
    return maxlen;
}

char* float_print(char* buf, float x){
	char name[12];
	memset(buf, '\0', 6);
	memset(name, '\0', 12);
	if( x < 0.00005 ) sprintf_s(buf,sizeof(buf), "    0");
	else if( x < 1 ) {
		sprintf_s(name, sizeof(name), "%6.4f",x);
		name[6] = '\0';
		strcpy_s(buf, sizeof(buf), &name[1]);
	}
	else if(x > 9999) sprintf_s(buf, sizeof(buf), " 9999");
	else sprintf_s(buf, sizeof(buf), "%5.4g",x);

	return buf;
}
//------------------------------------------------------------------------------
// Float to string with fixed width
//------------------------------------------------------------------------------
char* fixfloat(char* buff, float value, int width, int ndig) {
	double d = static_cast<double>(value);
	return fixfloat(buff, d, width, ndig);
}

char* fixfloat(char* buff, double value, int width, int ndig) {
	char temp_str[128];
	char value_str[16];
	int dec, sign;
	int last = width;
	int pos = 0;
	int zn = 0;	// Number of 0 in the right of a string
	if( value == 0 ) {
		buff[width-1] = '0';
		for(int i=0; i < width-1; i++) buff[i] = ' ';
		buff[width] = '\0';
		return buff;
	}
	for(int i=0; i < 16; i++) value_str[i] = '0';

	if( width < ndig ) ndig = width;
	_ecvt_s(temp_str, sizeof(temp_str), value, ndig, &dec, &sign);
	strncpy_s(value_str, sizeof(value_str), temp_str, ndig);
	//dec: location of the decimal point 
	if(sign != 0 ) {
		buff[0] = '-';
		pos = 1;
		width--;
		if( width <= ndig ) {
			ndig = width;
			_ecvt_s(temp_str, sizeof(temp_str), value, width, &dec, &sign); //Re-calc for reduced width
			strncpy_s(value_str, sizeof(value_str), temp_str, width);
		}
	}

	if( dec > width ) {
		for(int i=0; i < width; i++) buff[pos++] = '9';
		buff[last] = '\0';
		return buff;
	} else if (dec == width ) {
		for(int i=0; i < width; i++) buff[pos+i] = value_str[i];
		buff[last] = '\0';
		return buff;
	} else if( dec > 0 ){
		for(int i=0; i < dec; i++)
			buff[pos++] = value_str[i];
		buff[pos++] = '.';
		for(int i=dec; i < width; i++)
			buff[pos++] = value_str[i];
		buff[last] = '\0';
	} else {
		buff[pos++] = '.';
		dec = -dec;
		for(int i=0; i < dec; i++) {
			buff[pos++] = '0';
			// smaller than the number of digit
			if (pos == last) {
				for(int j=0; j < last-1; j++ ) buff[j] = ' ';
				buff[last-1] = '0';
				if( sign != 0 ) buff[last-2] = '-';
				return buff;
			}
		}
		for(int i=0; i < width-dec-1; i++) 	//2 -> 1
			buff[pos++] = value_str[i];
		buff[last] = '\0';
	}
	// Count the number of 0
	for(pos=last-1; buff[pos] != '.'; pos--) {
		if( buff[pos] == '0' ) zn++;
		else break;
	}
	if( buff[pos] == '.') {
		zn++;
		pos--;
	}
	if( zn > 0 ) {
		for(int i=0; i < last-zn; i++, pos--) {
			buff[last-i-1] = buff[pos];
		}
		for(int i=0; i < zn; i++) buff[i] = ' ';
	}

	return buff;
}

//------------------------------------------------------------------------------
// Read a configuration file 
//------------------------------------------------------------------------------
bool getconfig(const char* fname, char* key, char* dst) {
	FILE* fp = NULL;
	char buff[MAX_CONFIG_LENGTH];
	char name[MAX_CONFIG_LENGTH];
    char* p;
	char* next_token = NULL;
	errno_t error = fopen_s(&fp, fname, "rt");
    if( error != 0 || fp == NULL ) return false;
	else {
		memset(buff, '\0', MAX_CONFIG_LENGTH);
		dst[0] = '\0';
		while (fgets(buff, MAX_CONFIG_LENGTH, fp) != NULL) {
			for (p = buff; *p != '\0' && *p != '\n'; p++) {
				if (*p == '#') {
					*p = '\0';
					break;
				}
			}
			if (*p == '\n') *p = '\0';
			p = strtok_s(buff, "=", &next_token);
			if (p) {
				strcpy_s(name, sizeof(name), p);
				trim(name, MAX_CONFIG_LENGTH);
				if (strcmp(name, key) == 0) {
					p = strtok_s(NULL, "\0", &next_token);
					if (p) {
						strcpy_s(dst, sizeof(dst), p);
						int length = strlen(dst);
						trim(dst, length + 1);
					}
					else dst[0] = '\0';
					//                if( length > 128 ) strcpy(dst,value);
					//                else strncpy_s(dst, sizeof(dst), value, length);
					fclose(fp);
					return true;
				}
			}
		}
		fclose(fp);
	}
	return false;
}

char* strnstr(char* target, char* s, int size) {
	int start = 0;
	int count;
	char* comp;
	char* search;
	if( *s == '\0' ) return NULL;
	while( *target != '\0' && *s != '\0') {
		if( *target == *s ) {
			search = s +1;
			comp = target+1;
			count = 0;
			while( *search != '\0' && *comp != '\0' ) {
				if( *comp != *search) break;
				comp++;
				search++;
				count++;
				if( start + count >= size ) return NULL;
			}
			if( *search == '\0' ) return target;
		}
		target++;
		start++;
	}
	return NULL;
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
char* csv_parser(char* input, char** array, int c, char SEP, char DIG) {
	int len = strlen(input);
	if(len == 0 ) return 0;
	if(input[len-1] == '\n') {
		if(len == 1 ) return 0;
		input[len-1] = 0;
	}
	char* pos = input;
	char* pre = pos;
	for (int i = 0; i < c; i++) {
		if( *pos == SEP ) {
		    *pos = 0;
			array[i] = pos;
			pos++;
			continue;
		}
		pre = pos;
        if( *pos == '"' ) {
            pos++;
            pre = pos;
            bool quate_end = false;
            while( *pos != 0 ) {
                if( *pos == '"' ) quate_end = true;
                else if ( quate_end == true && *pos == SEP ) {
                    pos--;
                    *pos = 0;
                    pos++;
                    break;
                }
                pos++;
            }
        } else {
            while( *pos != 0 ) {
                if( *pos == SEP ) break;
                pos++;
            }
        }
		*pos = 0;
		array[i] = pre;
        pos++;
        if( *pos == 0 ) break;
	}
	return pos;
}
//---------------------------------------------------------------------------
// Extract a string inside double-quotations (for csv format)
//---------------------------------------------------------------------------
void dqconv(char* source, char* dest, int maxlen) {
//	char* p;
	int i = 0;
	int s = -1;
	int e = -1;

	while( source[i] != '\0' ) {
		if ( s == -1 ) {
			if( source[i] != '\"' && source[i] != ' ' && source[i] != '\t') break;
			else if ( source[i] == '\"' ) {
				s = i;
			}
		} else if ( s > -1 && source[i] == '\"' ) {
			e = i;
		}
		i++;
	}
	if( s > -1 && e > -1 ) {
		for(int j = e + 1; j < i; j++ ) {
			if( source[j] != ' ' && source[j] != '\t' ) {
				strncpy_s(dest, sizeof(dest), source, maxlen);
				return;
			}
		}
		if( e - s -1 > maxlen) e = s + 1 + maxlen;
		for(int j = s+1; j < e; j++) dest[j-s-1] = source[j];
		dest[e-s-1] = '\0';
	} else strncpy_s(dest, sizeof(dest), source, maxlen);
}

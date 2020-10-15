//---------------------------------------------------------------------------
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>
//---------------------------------------------------------------------------
#include "tool.h"
#include "stgetopt.h"
//---------------------------------------------------------------------------
#define BUFF_SIZE 256

void usage() {
	puts("1. Listing STRADA file in a directory");
    puts("Usage: stheader { aod | int | ire } [Options]");
	puts("Options");
	puts("-H\tDisplay Header");
	puts("-w n\tSet width of file name to n\n");
	puts("2. Change comment in a STRADA file");
    puts("Usage: stheader com file_name comment");
	puts("");
}

void comment(const char* fname, const char* comment) {
	FILE* fp;
	char* buff;
	char* p;
	char header[BUFF_SIZE];
	long int fsize;
	char c;
	fp = fopen(fname, "rt");
	if( fp == NULL ) {
		fprintf(stderr, "Cannot open file %s\n", fname);
		exit(1);
	}
	fseek(fp, 0L, SEEK_END);
	fsize = ftell(fp);
	rewind(fp);
	if( (buff = (char*) malloc(fsize+BUFF_SIZE+1)) == NULL ) {
		fprintf(stderr, "Cannot allocate buffer\n");
		fclose(fp);
		return;
	}
	//buff = new char[fsize + BUFF_SIZE + 1];
	if( fgets( header, BUFF_SIZE, fp) == NULL ) {
		fclose(fp);
		free(buff);
		return;
	}
	sprintf(header+5, "%s\n", comment);
	p = buff;
	while(true) {
		c = fgetc(fp);
		if( c == EOF ) break;
		*p = c;
		p++;
	}
	fclose(fp);
	*p = '\0';
	fp = fopen(fname, "wt");
	fprintf(fp, "%s", header);
	p = buff;
	while( *p != '\0' ) {
		fputc(*p, fp);
		p++;
	}
	fclose(fp);
//	delete[] buff;
	free(buff);
}

char* shrink_fname(const char* fname, char* dest, unsigned int length) {
	strncpy(dest, fname, length);
	dest[length] = '\0';
	if( strlen(fname) > length ){
		dest[length-1] = '.';
	}
	return dest;
}

int main(int argc, char* argv[])
{
    if( argc < 2 ) {
		usage();
        exit(1);
    }


    FILE* fp;
	int type;
    const char* type_str[] = {"aod", "int","ire"};
    const char* head_str[] = {"AOD", "INT", "IRE"};
    char mask[10];
	char shname[256];
	type = -1;
	if( strcmp( argv[1] , "com" ) == 0 ) {
		if( argc < 3 ) {
			usage();
			exit(1);
		}
		comment(argv[2], argv[3]);
		return 0;
	}
	for(int i=0; i < 3; i++) {
		if(strcmp( argv[1], type_str[i] ) == 0 ) {
			type = i;
			sprintf(mask, "*.%s", type_str[i]);
			break;
		}
	}
	if( type == -1 ) {
		usage();
		exit(1);
	}

	optarg = NULL;
	optind = 2 ;
	char ch;
	int fname_width = 15;
	bool disp_header = false;

	while( optind < argc ) {
		ch = get_opt(argc, argv, "w:Hv");
		if( ch == EOF ) {
			continue;
		} else {
			switch (ch) {
			case 'w':
				fname_width = atoi(optarg);
				if( fname_width < 5 ) fname_width = 5;
				else if (fname_width > 40 ) fname_width = 40;
				break;
			case 'H':
				disp_header = true;
				break;
			default:
				;
			}
		}
	}
	if( disp_header ) {
		printf("%-*sDATE   VER ZONE MODE TP COMMENT\n", fname_width, "NAME");
	}
	char buff[BUFF_SIZE];
	char comment[BUFF_SIZE];
	char size[20];
	char ver[3];
	int data[3];

	struct _finddata_t ffblk;
    int hFile = _findfirst(mask,&ffblk);
	if( hFile == -1 ) {
		printf("Not found\n");
		return (0);
	}
    int done = 0;
	while( done == 0) {
		fp = fopen(ffblk.name, "rt");
		fgets(buff, BUFF_SIZE, fp);
		buff[strlen(buff)-1] = '\0';
		comment[0] = '\0';
		if( strlen(buff) >= 3 && strncmp(buff, head_str[type], 3) == 0 ) {
			if( buff[3] == '\0' ) strcpy(ver, "1 ");
			else {
				if( buff[3] == ' ' ) ver[0] = '1';
				else if( buff[3] == '2' ) ver[0] = '2';
				else ver[0] = 'X' ;
				if( buff[4] == '\0' ) ver[1] = ' ';
				else if(buff[4] == '*' ) ver[1] = '*';
				else if(buff[4] == ' ' ) ver[1] = ' ' ;
				else ver[1] = 'X';
				ver[2] = '\0';
				if( buff[5] != '\0' ) strcpy( comment, buff+5);
			}
			fgets(buff, BUFF_SIZE, fp);
			for(int i=0; i < 3; i++) {
				strncpy( size, buff + i*5 , 5);
				size[5] = '\0';
				data[i] = atoi(size);
			}
			time_t tw = ffblk.time_write ;
			tm* ptm = localtime(&tw);
			shrink_fname(ffblk.name, shname, fname_width);
			printf("%-*s%02hd-%02hd-%02hd %s%5d%5d%2d%s\n", fname_width, shname, 1900+ptm->tm_year, ptm->tm_mon, ptm->tm_mday, ver, data[0], data[1], data[2], comment);
		} else {
			printf("%s is not a JICA-STRADA file.\n", ffblk.name);
		}
		fclose(fp);
		done = _findnext(hFile, &ffblk);
	}
	_findclose(hFile);
    return 0;
}
//---------------------------------------------------------------------------

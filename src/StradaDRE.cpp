#include <pch.h>
//---------------------------------------------------------------------------
#include <new>
#include <stdio.h>
#include <string.h>
#include <stdexcept>

#include "tool.h"
#include "StradaDRE.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
NodeDirection::NodeDirection() {
	memset( name ,	0, 11);
	memset( s_node, 0, 11);
	memset( e_node, 0, 11);
	memset( s_link, 0, 11);
	memset( e_link, 0, 11);

	total = 0 ;
	for(int i=0; i < 10; i++) vol[i] = 0;
	sx = sy= nx= ny= ex= ey = 0;

}

int NodeDirection::Read(FILE* fp){
//	int length;
	char buf[256];
	if( fgets(buf, 256, fp) == NULL ) return -1;
//	length = strlen(buf);
//	if( length < 190 ) return -1;
	strncpy_s( s_node, 11, &buf[ 0], 10 ); trim(s_node);
	strncpy_s( s_link, 11, &buf[10], 10 ); trim(s_link);
	strncpy_s( name  , 11, &buf[20], 10 ); trim(name)  ;
	strncpy_s( e_link, 11,&buf[30], 10 ); trim(e_link);
	strncpy_s( e_node, 11,&buf[40], 10 ); trim(e_node);
	total = getbufInt(buf, 50, 7 );
	for(int i=0; i < 10; i++)
		vol[i] = getbufInt(buf, 57+7*i, 7);
	sx = getbufFlt(buf, 128, 10 );
	sy = getbufFlt(buf, 138, 10 );
	nx = getbufFlt(buf, 148, 10 );
	ny = getbufFlt(buf, 158, 10 );
	ex = getbufFlt(buf, 168, 10 );
	ey = getbufFlt(buf, 178, 10 );

	return (1);
}

void NodeDirection::Write(FILE* fp) const {
	fprintf(fp,"%10s%10s%10s%10s%10s%7d",
		s_node, s_link, name, e_link, e_node, total);
	for(int i=0; i < 10; i++)
		fprintf(fp,"%7d", vol[i] );

	fprintf(fp,"%10g%10g%10g%10g%10g%10g\n",
				sx, sy, nx, ny, ex, ey );
}

StradaDRE::StradaDRE(){
	nNode = 0;
	nMode = 0;
	coordinate = 0;
}

StradaDRE::~StradaDRE(){
}

int StradaDRE::Read(FILE* fp) {
	char buf[256];
	if( fgets(buf, 256, fp) == NULL ) return -1;
	if( strncmp( buf, "DRE2 ", 5) != 0 ) return -1;
	if( fgets(buf, 256, fp) == NULL ) return -1;
	nNode = getbufInt(buf, 0, 5);
	nMode = getbufInt(buf, 5, 5);
	coordinate = getbufInt(buf, 10, 5);
	if( nNode <= 0 ) return -1;
	if( nMode <  0 ) return -1;
	if( coordinate < 0 || coordinate > 2 ) return -1;
	int count = 0;
	while(fgets(buf, 256, fp) != NULL) {
		count++;
	}
	if( count == 0 ) return -1;

	fseek( fp, 0, SEEK_SET );
	fgets(buf, 256, fp);
	fgets(buf, 256, fp);

	NodeDirection ND;
	for (int i = 0; i < count; i++) {
		if (ND.Read(fp) == -1) {
			direc.clear();
			return -1;
		}
		direc.push_back(ND);
	}
	return 1;
}

void StradaDRE::Read(char* fname) {
	FILE* fp = NULL;
	errno_t error = fopen_s(&fp, fname, "rt");
	if( error != 0 || fp == NULL) throw std::runtime_error("DRE2");
	else {
		int ret = Read(fp);
		fclose(fp);
		if (ret == -1) throw std::runtime_error("DRE2");
	}
}

void StradaDRE::Write(FILE* fp){
	fprintf(fp,"DRE2 jWtrf2\n");
	fprintf(fp,"%5d%5d%5d\n", nNode, nMode, coordinate);
	for(const auto& d: direc) {
		d.Write(fp);
	}
}


void StradaDRE::Write(const char* fname){
	FILE* fp;
	errno_t error = fopen_s(&fp, fname, "wt");
	if( error == 0 && fp  != NULL ) {
		Write(fp);
		fclose(fp);
	}
}




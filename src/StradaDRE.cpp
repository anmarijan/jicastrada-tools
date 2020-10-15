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
	strncpy( s_node, &buf[ 0], 10 ); trim(s_node,11); s_node[10]  = '\0';
	strncpy( s_link, &buf[10], 10 ); trim(s_link,11); s_link[10]  = '\0';
	strncpy( name  , &buf[20], 10 ); trim(name,11)  ; name[10]  = '\0';
	strncpy( e_link, &buf[30], 10 ); trim(e_link,11); e_link[10]  = '\0';
	strncpy( e_node, &buf[40], 10 ); trim(e_node,11); e_node[10]  = '\0';
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

void NodeDirection::Write(FILE* fp) {
	fprintf(fp,"%10s%10s%10s%10s%10s%7d",
		s_node, s_link, name, e_link, e_node, total);
	for(int i=0; i < 10; i++)
		fprintf(fp,"%7d", vol[i] );

	fprintf(fp,"%10g%10g%10g%10g%10g%10g\n",
				sx, sy, nx, ny, ex, ey );
}

StradaDRE::StradaDRE(){
	direc = NULL;
	nNode = 0;
	nMode = 0;
	nDirection = 0;
	coordinate = 0;
}

StradaDRE::~StradaDRE(){
	delete[] direc;
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
	nDirection = count;

	fseek( fp, 0, SEEK_SET );
	fgets(buf, 256, fp);
	fgets(buf, 256, fp);

	direc = new NodeDirection[nDirection];

	for(int i=0; i < count ; i++ )
		if( direc[i].Read(fp)  == -1) {
			delete[] direc;
			nDirection = 0;
			direc = NULL;
			return -1;
		}
	return 1;
}

void StradaDRE::Read(char* fname) {
	FILE* fp;
	if((fp = fopen(fname ,"rt"))==NULL) throw std::runtime_error("DRE2");
	int ret = Read(fp);
	fclose(fp);
	if( ret == -1 ) throw std::runtime_error("DRE2");
}

void StradaDRE::Write(FILE* fp){
	fprintf(fp,"DRE2 jWtrf2\n");
	fprintf(fp,"%5d%5d%5d\n", nNode, nMode, coordinate);
	for( int i = 0; i < nDirection; i++ ) {
		direc[i].Write(fp);
	}
}


void StradaDRE::Write(const char* fname){
	FILE* fp;
	if( (fp=fopen(fname,"wt")) != NULL ) {
		Write(fp);
		fclose(fp);
	}
}




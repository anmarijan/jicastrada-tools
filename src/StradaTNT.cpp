#include <pch.h>
/*----------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <set>
/*----------------------------------------------------------------------------*/
#include "StradaTNT.h"
#include "tool.h"
/*----------------------------------------------------------------------------*/
#define MAX_BUFF   1024
#define CSV_READ   kip = strtok_s(NULL, ",", &next_token); if( kip == NULL ) return false; trim(kip);
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
TRNLine::TRNLine() {
    memset(name, 0, 11);
    mode = 1; way = 1; frequency = 1; speed = 400;
	flag = 0;
}
///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
bool TRNLine::ReadCSV(char* buff) {
	nodes.clear();
	char* next_token = NULL;
	char* kip = strtok_s(buff , ",", &next_token); if( kip == NULL ) return false;
	dqconv(kip, name, 10);
	trim(name, 11);
	CSV_READ ; mode = atoi(kip); //printf("%d:", mode);
	CSV_READ ; way = atoi(kip);//printf("%d:", way);
	CSV_READ ; frequency = atoi(kip);//printf("%d:", frequency);
	CSV_READ ; speed = atoi(kip);//printf("%d:", speed);
	CSV_READ ; nNode = atoi(kip);//printf("%d\n", nNode);

	for(int i=0; i < nNode; i++) {
		TRNNode node;
		CSV_READ ;
		node.access = kip[0];
		CSV_READ ;
		strncpy_s(node.name, sizeof(node.name), kip,10);
		nodes.push_back(node);
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
void TRNLine::Write(FILE* fp) {
	fprintf(fp, "%-10s%2d%1d%4d%3d%3d", name, mode, way, frequency, speed, nNode);
	for(const auto& node : nodes) {
		fprintf(fp, "%c%-10s",  node.access, node.name);
	}
	fprintf(fp, "\n");
}
////////////////////////////////////////////////////////////////////////
//  StradaTNT
///////////////////////////////////////////////////////////////////////////////
StradaTNT::StradaTNT() {
    nLine = nMode = 0;
}
///////////////////////////////////////////////////////////////////////////////
//  StradaTNT
//  -1: Fail to read 1st line
//  -2: Not TNT
//  -3: Fail to read 3rd line
//  -4: No of lines or modes is 0 or minus
//  -5: Fail to read the line name
//  -6: Fail to read line information
///////////////////////////////////////////////////////////////////////////////
int StradaTNT::Read(FILE* fp)
{
	int max_len;
	std::vector<char> vb(MAX_BUFF);
	char* sbuf;
	char* kip;
	char* next_token = NULL;
	int csv;

	sbuf = &vb[0];
	max_len = maxlinelen(fp, sbuf, MAX_BUFF) + 1;	//
	if( max_len > MAX_BUFF ) {
		vb.resize(max_len);
	}
	sbuf = &vb[0];

	if(fgets(sbuf,MAX_BUFF,fp)==NULL) return(-1);
	if(strncmp(sbuf, "TNT2 ",5) == 0 ) csv = 0;
	else if( strncmp(sbuf, "TNT2*", 5) == 0 ) csv = 1;
	else if( strncmp(sbuf, "TNT4*", 5) == 0 ) csv = 1;
	else return (-2);
	if(fgets(sbuf,MAX_BUFF,fp)==NULL) return(-3);
	if(csv == 0 ) {
		nLine = getbufInt(sbuf, 0, 5);
		nMode = getbufInt(sbuf, 5, 5);
	} else if (csv == 1) {
		kip = strtok_s(sbuf, ",", &next_token);
		if( kip == NULL ) return (-3);
		trim(kip);
		nLine = atoi(kip);
		kip = strtok_s(NULL, ",", &next_token);
		if( kip == NULL ) return (-3);
		trim(kip);
		nMode = atoi(kip);
	}
//	printf("%d:%d\n", nLine, nMode);
	if( nLine <= 0 || nMode <= 0 ) return(-4);

	for(int i=0; i < nLine; i++) {
		TRNLine line;
		if( csv == 0 ) {
			memset(sbuf, 0, 11);
			if( fgets(sbuf,11,fp) == NULL) return(-5);
			trim(sbuf);
			strncpy_s(line.name, sizeof(line.name), sbuf, 10); line.name[10]='\0';
			try {
	            if( fgets(sbuf,14,fp) == NULL) throw 6;
				short mode = getbufInt(sbuf, 0, 2);
				short way  = (short)(sbuf[2] - '0');
				int freq = getbufInt(sbuf, 3, 4);
	            int spd  = getbufInt(sbuf, 7, 3);
	            int num  = getbufInt(sbuf, 10,3);

	            if( mode < 0 )           throw  7;
	            if( way < 1 || way > 2 ) throw  8;
	            if( freq < 0 )           throw  9;
	            if( spd < 0 )            throw 10;
	            if( num < 0 )            throw 11;

	            line.mode = mode;
	            line.way  = way;
	            line.frequency = freq;
	            line.speed = spd;
	            line.nNode = num;
	            for(int j=0; j < line.nNode; j++) {
	                if( fgets(sbuf,12,fp) == NULL) throw 12;
					TRNNode node;
	                node.access = sbuf[0];
	                sbuf[12]='0';
	                sbuf[0] = ' ';
	                trim(sbuf, 12);
	                strncpy_s(node.name, sizeof(node.name), sbuf, 10);
					line.nodes.push_back(node);
	            }
	            //
	            if( fgets(sbuf, MAX_BUFF, fp) == NULL) throw 13;

	        } catch(int e) {
				lines.clear();
	            return ( -1000*(i+1) - e);
	        }
        } else if (csv == 1){
			if( fgets(sbuf, max_len,fp) == NULL ) return(-5);
			sbuf[strlen(sbuf)-1] = '\0';	//改行削除
			if( line.ReadCSV(sbuf) == false) return (-10000 - i);
		}
		lines.push_back(line);
    }
    return 0;
}
///////////////////////////////////////////////////////////////////////////////
//  StradaTNT
///////////////////////////////////////////////////////////////////////////////
void StradaTNT::Read(const char *file_name) {
	FILE* fp = NULL;
	errno_t error = fopen_s(&fp, file_name, "rt");
	if(error != 0 || fp==NULL) throw std::runtime_error("Cannot open TNT2 file");
	else {
		int ret = Read(fp);
		fclose(fp);
		std::string msg;
		if (ret < 0) {
			msg = "TNT2:" + std::to_string(ret);
			throw std::runtime_error(msg);
		}
	}
}
void StradaTNT::Write(char* fname) {
	FILE* fp = NULL;
	errno_t error = fopen_s(&fp, fname, "wt");
	if(error != 0 || fp == NULL) throw std::runtime_error("Cannot create TNT2 file");
	else {
		fprintf(fp, "TNT2 \n");
		fprintf(fp, "%5d%5d\n", nLine, nMode);
		for (int i = 0; i < nLine; i++) lines[i].Write(fp);
		fclose(fp);
	}
}
///////////////////////////////////////////////////////////////////////////////
//  
///////////////////////////////////////////////////////////////////////////////
void StradaTNT::Write(FILE* fp) {
	fprintf(fp, "TNT2 \n");
	fprintf(fp, "%5d%5d\n", nLine, nMode);
	for(int i=0; i < nLine; i++) lines[i].Write(fp);
}
///////////////////////////////////////////////////////////////////////////////
//  StradaTNT
///////////////////////////////////////////////////////////////////////////////
TRNLine* StradaTNT::getLine(int i) {
	return &lines[i];
}

int StradaTNT::get_route_count() {
	int c = 0;
	for(int i=0; i < nLine; i++) {
		if( lines[i].way == 2 ) c += 2;
		else c++;
	}
	return c;
}

int StradaTNT::get_node_count() {
	std::set<std::string> node_table;
	int c;
	for(const auto& line : lines) {
		for(const auto& node : line.nodes) {
			node_table.insert(node.name);
		}
	}
	c = (int)node_table.size();
	return c;
}

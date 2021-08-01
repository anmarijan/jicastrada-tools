#include <pch.h>
#include <new>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#define AOD_BAD_ALLOC 1
/*----------------------------------------------------------------------------*/
#include "StradaAOD.h"
#include "tool.h"
// #include "Matrix.h"
/*----------------------------------------------------------------------------*/
ODMatrix::ODMatrix(){
	nZone = 0;
}
ODMatrix::ODMatrix(int n){
	data.reset(new double[n*n]);
	for(int i=0; i< n*n; i++ ) data[i] = 0;
	nZone = n;
}
ODMatrix::ODMatrix(const char* od_name, int n){
	name = od_name;
	data.reset(new double[n*n]);
	for(int i=0; i< n*n; i++ ) data[i] = 0;
	nZone = n;
}
void ODMatrix::init(int n){
	data.reset(new double[n*n]);
	for(int i=0; i< n*n; i++ ) data[i] = 0;
	nZone = n;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
StradaAOD::StradaAOD() {
	version = 2;
	nTable = nZone = 0;
	type = 0;
	csv = false;
    zeroskip = true;
	colnum = 0; colwid = 0; decwid = 0; data_type = 'X';
}
StradaAOD::StradaAOD(int nTable, int nZone){
	version = 2;
	type = 0;
	allocTable(nTable, nZone);
	csv = false;
	colnum = 0;  colwid = 0; decwid = 0; data_type = 'X';
}
StradaAOD::StradaAOD(const StradaAOD & aod){

    nTable = aod.nTable;
    nZone = aod.nZone;
	try {
		allocTable(nTable,nZone);
	} catch(int e) {
		throw e;
	}
	name = aod.name;
	type = aod.type;
	version = aod.version;
    csv = aod.csv;
    zeroskip = aod.csv;
	colnum = 0;  colwid = 0; decwid = 0; data_type = 'X';

	for(int i=0; i < nTable; i++){
		for(int j=0; j < nZone * nZone; j++){
			tables[i].data[j] = aod.tables[i].data[j];
		}
	}
}
// 
void StradaAOD::allocTable(int n_tables, int n_zones){
	assert( n_tables > 0 && n_zones > 0);
	clear();
	try {
		tables.resize(n_tables);
		nTable = n_tables;
		nZone = n_zones;
	} catch( const std::bad_alloc&  ) {
		throw AOD_BAD_ALLOC;
	}
	try {
		for(int i=0; i< nTable; i++) {
			tables[i].init(nZone);
		}
	} catch( const std::bad_alloc& ) {
		tables.clear();
		nTable = nZone = 0;
		throw AOD_BAD_ALLOC;
	}
}
/////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////
double StradaAOD::getOD(int tbl,int org, int dst){
	assert((tbl < nTable)&&(org < nZone)&&(dst < nZone));
	int s = nZone * org + dst;
	return tables[tbl].data[s];
}
/////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////
double StradaAOD::getSumOD(int org, int dst) {
	assert((org < nZone) && (dst < nZone));
	double ret = 0;
	for(int i=0; i < nTable; i++)
		ret += tables[i].data[nZone * org + dst];
	return ret;
}
void StradaAOD::setOD(int tbl, int org, int dst,double number){
	assert((tbl < nTable)&&(org < nZone)&&(dst < nZone));
	tables[tbl].data[nZone * org + dst] = number;
}
/////////////////////////////////////////////////////////////////////////////
//  
/////////////////////////////////////////////////////////////////////////////
void StradaAOD::plusOD(int tbl,int org,int dst,double number) {
	assert((tbl < nTable)&&(org < nZone)&&(dst < nZone));
	tables[tbl].data[nZone * org + dst] += number;
}
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void StradaAOD::multOD(int tbl, double x) {
	assert(tbl < nTable);
	for(int i=0; i < nZone ; i++)
        for(int j=0; j < nZone; j++)
            tables[tbl].data[nZone * i + j] *= x;
}
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void StradaAOD::clearOD() {
    for(int t=0; t < nTable; t++) {
        for(int i=0; i < nZone  ; i++)
            for(int j=0; j < nZone; j++)
                tables[t].data[nZone * i + j] = 0;
    }
}
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void StradaAOD::setTableName(int tbl, const char* str){
	assert( (tbl < nTable)&&(tbl >= 0) );
	tables[tbl].name = str;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
int StradaAOD::FTFormat(char* str)
{
	int len = strlen(str);

	int counter=0;
	while( str[counter] != '(' ) {
		counter++;
		if ( counter > len - 1) return (-1);
	}
	counter++;
	int st = counter;
	while(true) {
		if( !isdigit(str[counter])) break;
		counter++;
		if ( counter > len - 1) return (-1);

	}
	if( str[counter] == 'I' ) data_type = 'I';
	else if( str[counter] == 'F') data_type = 'F';
	else data_type = 'X';
	char buf[20];
	memset(buf, 0, 20);
	colnum = getbufInt(str, st, counter - st);
	counter++;
	st = counter;

	while(true) {
		if( !isdigit(str[counter])) break;
		counter++;
		if ( counter > len - 1) return (-1);
	}

	colwid = getbufInt(str, st, counter - st);

	if( str[counter] == '.' ) {
		counter++;
		st = counter;
		if ( counter > len - 1) return (-1);
		while(true) {
			if( !isdigit(str[counter])) break;
			counter++;
			if ( counter > len - 1) return (-1);
		}
		decwid = getbufInt(str, st, counter -st);
		if( str[counter] != ')' ) return (-1);
	}
	else if( str[counter] == ')' ) decwid = 0;
	else return(-1);

	return 0;
}
////////////////////////////////////////////////////////////////////////////////
//	
////////////////////////////////////////////////////////////////////////////////
void StradaAOD::Read(const char* fname){

	std::ifstream ifs(fname, std::ios_base::in);
	if( !ifs ) {
		throw std::runtime_error("Cannot open AOD file.");
	}
	char* p;
	char dst[100];

	csv = false;

	memset(dst, 0, 100);

	std::string buff;
	try {
		if( std::getline(ifs, buff).fail() ) throw std::runtime_error("line 1");

		if( buff.compare(0,3,"AOD") != 0 && buff.compare(0,3,"IOD")!=0)	throw std::runtime_error("File type");
		if (buff[3] == ' ') version = 1;
		else if (buff[3] == '2') version = 2;
		else if (buff[3] == '3') version = 3;
		else throw std::runtime_error("Version");

		if( buff[4] == '*' ) csv = true;
		name = buff.substr(5);

		if( std::getline(ifs, buff).fail() ) throw std::runtime_error("line 2");
		if( csv ) {
			bool csv_line2 = false;
			p = &buff[0];
			while( *p != '\0' ) {
				if (*p == ',') {
					csv_line2 = true;
					break;
				}
				else p++;
			}
			if( csv_line2 ) {
				boost::tokenizer<boost::escaped_list_separator<char> > tokens(buff);
				boost::tokenizer<boost::escaped_list_separator<char> >::iterator it = tokens.begin();
				if ( it == tokens.end()) throw std::runtime_error("csv");
				nZone = atoi(it->c_str());
				++it; if (it == tokens.end()) throw std::runtime_error("csv");
				nTable = atoi(it->c_str());
				++it; if (it == tokens.end()) throw std::runtime_error("csv");
				type = atoi(it->c_str());
				++it; if (it == tokens.end()) throw std::runtime_error("csv");
				if (nZone <= 0 || nTable <= 0 || type < 0 || type > 3)
					throw std::runtime_error("Out of range");
				allocTable(nTable, nZone);
				int c = 0;
				while (it != tokens.end()) {
					tables[c++].name = (*it);
					++it;
				}
			} else {
				p = &buff[0];
				nZone  = getbufInt(p, 0,5);
				nTable = getbufInt(p, 5,5);
				type   = getbufInt(p,10,5);
				if (nZone <= 0 || nTable <= 0 || type < 0 || type > 3)
					throw std::runtime_error("Out of range");
				allocTable(nTable, nZone);
				// else (task)
			}
		} else {
			p = &buff[0];
			nZone  = getbufInt(p, 0,5);
			nTable = getbufInt(p, 5,5);
			type   = getbufInt(p,10,5);
			if(nZone <=0 || nTable <=0 || type < 0 || type > 3)
				throw std::runtime_error("Out of range");

			allocTable(nTable,nZone);
			//table name
			size_t length = strlen(p);
			length -= 15;	//Exclude the table information
			size_t name_len = ( version == 1 ) ?  20 : 10;
			length /= name_len;
			length = (length < static_cast<size_t>(nTable)) ? length : nTable;
			for(size_t i=0; i < length; i++){
				std::string str = buff.substr(15 + name_len * i, name_len);
				boost::trim(str);
				tables[i].name = str;
			}
		}
		//From line 3 

		if( version == 1 ) {
			if( std::getline(ifs, buff).fail() ) throw std::runtime_error("format");
			//return(-1);	//(10I7)
			p = &buff[0];
			if( FTFormat(p) == -1 ) throw std::runtime_error("format");
			int m = nZone / colnum;
			int n = nZone % colnum;

			for(int i=0; i < nTable; i++){
				for(int j = 0; j < nZone; j++){
					for(int k=0; k < m; k++){
						if( std::getline(ifs, buff).fail() ) throw std::runtime_error("read");
						unsigned int colsize = static_cast<unsigned int>(colnum * colwid);
						if( buff.length() < colsize ) throw std::runtime_error("read");
						for(int l = 0; l < colnum; l++){
							strncpy_s(dst,sizeof(dst),&buff[colwid*l],colwid);
							dst[colwid] = '\0';
							setOD(i, j, colnum*k+l, (double)atof(dst));
						}
					}
					if(n !=0){
						if( std::getline(ifs, buff).fail() ) throw std::runtime_error("read");
						unsigned int width = static_cast<unsigned int>(n * colwid);
						if( buff.length() < width ) throw std::runtime_error("read");
						for(int l=0;l<n;l++){
							strncpy_s(dst,sizeof(dst),&buff[colwid*l],colwid);
							dst[colwid] = '\0';
							setOD(i, j, colnum*m+l, (double)atof(dst));
						}
					}
				}//zone
			}// table
		} else if (csv) {//version 2 - CSV
			if(type == 0 ) {
				for(int t=0; t < nTable; t++) {
					for (int i = 0; i < nZone; i++) {
						if( std::getline(ifs, buff).fail() ) throw std::runtime_error("read");
						boost::tokenizer<> tokens(buff);
						boost::tokenizer<>::iterator it = tokens.begin();
						if (it == tokens.end()) throw std::runtime_error("csv01");

						for(int j=0; j < nZone ; j++) {
							setOD(t,i,j, atof(it->c_str()));
							++it;
							if(j<nZone-1 && it == tokens.end()) throw std::runtime_error("csv02");
						}
					}
				}
			} else if (type==2) {
				int origin, dest;
				double x;
				while( std::getline(ifs, buff) ) {
					boost::tokenizer<> tokens(buff);
					boost::tokenizer<>::iterator it = tokens.begin();
					if (it == tokens.end()) throw std::runtime_error("csv03");
					origin = std::stoi(*it);
					++it; if (it == tokens.end()) throw std::runtime_error("csv04");
					dest = std::stoi(*it);
					if( origin <= 0 || origin > nZone || dest <=0 || dest > nZone)
						throw std::runtime_error("range");
					for (int t=0; t < nTable; t++) {
						++it; if (it == tokens.end()) throw std::runtime_error("csv05");
						x = std::stod(*it);
						setOD(t, origin-1, dest-1, x);
					}
				}
			}
		} else {
			for(int t=0; t < nTable; t++) {
				for(int i=0; i < nZone; i++) {
					if( std::getline(ifs, buff).fail() ) throw std::runtime_error("read");
					int w = nZone * 8;
					if( buff.length() < static_cast<size_t>(w) )  throw std::runtime_error("read");
					for(int j=0; j < nZone; j++ ) {
						std::string sub = buff.substr(8*j,8);
						setOD(t, i , j, atof(sub.c_str()));
					}
				}
			}
		}

	} catch (std::runtime_error& e) {
		clear();
		fprintf(stderr, "AOD Format Error: %s\n", e.what());
	}
	printf("Zone=%d, Tables=%d, type=%d\n", nZone, nTable, type);
}
////////////////////////////////////////////////////////////////////////////////
// Ver2.0 format (8 cols x n zones)
////////////////////////////////////////////////////////////////////////////////
void StradaAOD::Write(FILE* fp){

	bool zflag;

	if( version == 1) fprintf(fp,"AOD ");
	else if (version == 2) fprintf(fp, "AOD2");
	if(csv)
		fprintf(fp, "*%s\n", name.c_str());
	else
		fprintf(fp, " %s\n", name.c_str());

	if( csv ) {
		fprintf(fp,"%d,%d,%d",nZone, nTable, type);
		for(int i=0; i < nTable; i++)
			fprintf(fp,",%s", tables[i].name.c_str());
		fprintf(fp,"\n");
	} else {
		fprintf(fp,"%5d%5d%5d",nZone, nTable, type);
		for(int i=0; i < nTable; i++)
			if (version == 1)  fprintf(fp,"%-20s", tables[i].name.c_str());
			else if( version == 2) fprintf(fp,"%-10s", tables[i].name.c_str());
		fprintf(fp,"\n");
	}

	if( version == 2) {
		if(csv) {	//CSV
			if( type == 0 ) {
	        	for(int k=0; k < nTable; k++) {
	                for(int i=0; i < nZone; i++) {
	                    for(int j=0; j < nZone-1; j++) {
							fprintf(fp, "%g,", getOD(k, i, j));
						}
						fprintf(fp, "%g\n", getOD(k,i,nZone-1));
	                }
	            }
            } else if (type == 2) {	//Not type=1
				for(int i=0; i < nZone; i++) {
					for(int j=0; j < nZone; j++) {
                        if(zeroskip) {
                            zflag = false;
                            for(int k=0; k < nTable; k++) {
                                if( getOD(k, i, j) > 0 ) {
                                    zflag = true;
                                    break;
                                }
                            }
                        } else zflag = true;
						if( zflag ) {	//Non zero data only
							fprintf(fp, "%d,%d", i+1, j+1);
							for(int k=0; k < nTable; k++) fprintf(fp, ",%g", getOD(k, i, j) );
							fprintf(fp, "\n");
						}
					}
				}
			}
		} else {
			for(int k=0; k< nTable; k++) {
				for(int i=0; i < nZone; i++){
					for(int j=0; j < nZone; j++){
						fprintf(fp,"%8d", (int)getOD(k, i, j) );
					}
					fprintf(fp,"\n");
				}
			}
		}
	} else if( version == 1) {
		fprintf(fp,"(%dI8)\n", nZone);
		for(int i=0; i < nTable; i++){
			for(int j = 0; j < nZone; j++){
				for(int k=0; k < nZone; k++){
					 fprintf(fp,"%8d",(int)getOD(i,j,k));
				}
				fprintf(fp,"\n");
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void StradaAOD::Write(const char* fname) {
	FILE* fp = NULL;
	errno_t error;
	error = fopen_s(&fp, fname, "wt");
	if(error != 0 || fp == NULL) throw std::runtime_error("AOD");
	else {
		Write(fp);
		fclose(fp);
	}
}
////////////////////////////////////////////////////////////////////////////////
//	
////////////////////////////////////////////////////////////////////////////////
void StradaAOD::clear(){
	tables.clear();
	nTable = 0;
	nZone = 0;
}
/////////////////////////////////////////////////////////////////////////////
//  
/////////////////////////////////////////////////////////////////////////////
double StradaAOD::getMaxOD(int tbl){
	double m_val = 0;
	for(int i=0; i < nZone; i++)
		for(int j=0; j < nZone; j++) {
			m_val = ( m_val > getOD(tbl,i,j) ) ? m_val : getOD(tbl,i, j) ;
		}
	return m_val;
}
/////////////////////////////////////////////////////////////////////////////
//  Return the max value of all tables
/////////////////////////////////////////////////////////////////////////////
double StradaAOD::getMaxOD(){
	double m_val = 0;
	for (int t=0; t < nTable; t++) {
		double val = getMaxOD(t);
		m_val = ( m_val > val ) ? m_val : val ;
    }
    return m_val;
}

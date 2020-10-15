#include <new>
#include <assert.h>
#include <stdio.h>
#include <string.h>
//#include <mem.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
//#include <time.h>
#define AOD_BAD_ALLOC 1
/*----------------------------------------------------------------------------*/
#include "StradaAOD.h"
#include "tool.h"
// #include "Matrix.h"
/*----------------------------------------------------------------------------*/
ODMatrix::ODMatrix(){
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
}
StradaAOD::StradaAOD(int nTable, int nZone){
	version = 2;
	type = 0;
	allocTable(nTable, nZone);
	csv = false;
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

	for(int i=0; i < nTable; i++){
		for(int j=0; j < nZone * nZone; j++){
			tables[i].data[j] = aod.tables[i].data[j];
		}
	}
}
// メモリ確保
void StradaAOD::allocTable(int nTable, int nZone){
	assert( nTable > 0 && nZone > 0);
	clear();
	try {
		tables.resize(nTable);
	} catch( const std::bad_alloc& e) {
		throw AOD_BAD_ALLOC;
	}
	try {
		for(int i=0; i< nTable; i++) {
			tables[i].init(nZone);
		}
	} catch( const std::bad_alloc& e) {
		tables.clear();
		throw AOD_BAD_ALLOC;
	}
}
/////////////////////////////////////////////////////////////////////////////
// double形式で返す
/////////////////////////////////////////////////////////////////////////////
double StradaAOD::getOD(int tbl,int org, int dst){
	assert((tbl < nTable)&&(org < nZone)&&(dst < nZone));
	return tables[tbl].data[nZone * org + dst];
}
/////////////////////////////////////////////////////////////////////////////
// 加算は小数点を含み、合計は整数に丸める
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
//  OD表に加える
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
        for(int i=0; i < nZone ; i++)
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
//	ファイルを読み込む
////////////////////////////////////////////////////////////////////////////////
void StradaAOD::Read(const char* fname){

	std::ifstream ifs(fname, std::ios_base::in);
	if( !ifs ) {
		throw std::runtime_error("Cannot open AOD file.");
	}
	char* p;
	char* kip = NULL;
	char dst[100];
	int length;
	int name_len;
	int max_len;

	csv = false;

	memset(dst, 0, 100);

	std::string buff;
	try {
		if( std::getline(ifs, buff).fail() ) throw std::runtime_error("line 1");

		if( buff.compare(0,3,"AOD") != 0 && buff.compare(0,3,"IOD")!=0)	throw std::runtime_error("File type");
		if( buff[3] == ' ' ) version = 1;
		else if( buff[3] == '2' ) version = 2;
		else throw std::runtime_error("Version");

		if( buff[4] == '*' ) csv = true;
		name = buff.substr(5);

		if( std::getline(ifs, buff).fail() ) throw std::runtime_error("line 2");
		length = buff.length();
		if( csv ) {

			//2行目はcsv形式の時もあるし、そうでない時もある！
			bool csv_line2 = false;
			for( int i=0; i < 15; i++ ) {
				if( buff[i] >= length) throw std::runtime_error("line 2");
				if( buff[i] == ',' ) {
					csv_line2 = true;
					break;
				}
			}
			p = &buff[0];
			if( csv_line2 ) {
				kip = strtok(p, ",");
				if (kip==NULL) throw std::runtime_error("csv");
				nZone = atoi(kip);
				kip = strtok(NULL,","); if (kip==NULL) throw std::runtime_error("csv");
				nTable = atoi(kip);
				kip = strtok(NULL,","); if (kip==NULL) throw std::runtime_error("csv");
				type = atoi(kip);
				kip = strtok(NULL,",");
			} else {
				nZone  = getbufInt(p, 0,5);
				nTable = getbufInt(p, 5,5);
				type   = getbufInt(p,10,5);
			}
			if(nZone <=0 || nTable <=0 || type < 0 || type > 3)
				throw std::runtime_error("Out of range");
//			printf("%5d%5d%5d\n", nZone, nTable, type);
			allocTable(nTable,nZone);
			if( csv_line2 ) {
				int c = 0;
				while (kip != NULL) {
					tables[c++].name = kip;
					kip = strtok(NULL,",");
				}
			}
			// else 文の追加

		} else {
			p = &buff[0];
			nZone  = getbufInt(p, 0,5);
			nTable = getbufInt(p, 5,5);
			type   = getbufInt(p,10,5);
			if(nZone <=0 || nTable <=0 || type < 0 || type > 3)
				throw std::runtime_error("Out of range");

			allocTable(nTable,nZone);
			//テーブル名称の取得
			length = strlen(p);
			length -= 15;	//テーブル情報の長さを除く(改行はない)
			name_len = ( version == 1 ) ?  20 : 10;
			length /= name_len;
			length = (length < nTable) ? length : nTable;
			for(int i=0; i < length; i++){
				strncpy(dst,&p[15+name_len*i],20);
				dst[name_len] = '\0';
				trim(dst,name_len);
				tables[i].name = dst;
			}
		}
		//3行目以降の読み込み
//		printf("%5d%5d%5d\n", nZone, nTable, type);

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
						if( buff.length() < colnum * colwid ) throw std::runtime_error("read");
						for(int l = 0; l < colnum; l++){
							strncpy(dst,&buff[colwid*l],colwid);
							dst[colwid] = '\0';
							setOD(i, j, colnum*k+l, (double)atof(dst));
						}
					}
					if(n !=0){
						if( std::getline(ifs, buff).fail() ) throw std::runtime_error("read");
						if( buff.length() < n * colwid ) throw std::runtime_error("read");
						for(int l=0;l<n;l++){
							strncpy(dst,&buff[colwid*l],colwid);
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
						p = &buff[0];
						kip = strtok(p, ",");
						if (kip==NULL) throw std::runtime_error("csv01");
						for(int j=0; j < nZone ; j++) {
							setOD(t,i,j,(double)atof(kip));
							kip = strtok(NULL, ",");
							if(j<nZone-1 && kip==NULL) throw std::runtime_error("csv02");
						}
					}
				}
			} else if (type==2) {
				int origin, dest;
				double x;
				while( std::getline(ifs, buff) ) {
					p = &buff[0];
					kip = strtok(p, ",");
						if (kip==NULL) throw std::runtime_error("csv03");
						origin = atoi(kip);
					kip = strtok(NULL, ",");
						if (kip==NULL) throw std::runtime_error("csv04");
						dest = atoi(kip);
					if( origin <= 0 || origin > nZone || dest <=0 || dest > nZone)
						throw std::runtime_error("range");
					for (int t=0; t < nTable; t++) {
						kip = strtok(NULL, ",");
							if (kip==NULL) throw std::runtime_error("csv05");
							x = atof(kip);
						setOD(t, origin-1, dest-1, (double)x);
					}
				}
			}
		} else {
			for(int t=0; t < nTable; t++) {
				for(int i=0; i < nZone; i++) {
					if( std::getline(ifs, buff).fail() ) throw std::runtime_error("read");
					if( buff.length() < 8 * nZone )  throw std::runtime_error("read");
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
}
////////////////////////////////////////////////////////////////////////////////
// Ver2.0形式で保存する（８カラム×ゾーン数）
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
		if(csv) {	//CSV 形式の場合
			if( type == 0 ) {
	        	for(int k=0; k < nTable; k++) {
	                for(int i=0; i < nZone; i++) {
	                    for(int j=0; j < nZone-1; j++) {
							fprintf(fp, "%g,", getOD(k, i, j));
						}
						fprintf(fp, "%g\n", getOD(k,i,nZone-1));
	                }
	            }
            } else if (type == 2) {	//type=1には未対応
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
						if( zflag ) {	//非ゼロのデータのみ
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
	FILE* fp;
	if((fp = fopen(fname ,"wt"))==NULL) throw std::runtime_error("AOD");
	Write(fp);
	fclose(fp);
}
////////////////////////////////////////////////////////////////////////////////
//	初期化する(typeは初期化しない)
////////////////////////////////////////////////////////////////////////////////
void StradaAOD::clear(){
	tables.clear();
	nTable = 0;
	nZone = 0;
}
/////////////////////////////////////////////////////////////////////////////
//  各テーブル内で最大の値
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
//  全てのテーブル内で最大の値
/////////////////////////////////////////////////////////////////////////////
double StradaAOD::getMaxOD(){
	double m_val = 0;
	for (int t=0; t < nTable; t++) {
		double val = getMaxOD(t);
		m_val = ( m_val > val ) ? m_val : val ;
    }
    return m_val;
}

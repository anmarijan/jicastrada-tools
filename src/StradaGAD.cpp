#include <string.h>
#include <stdlib.h>
#include <stdexcept>
#include <assert.h>
#include <boost/algorithm/string.hpp>  //trim
#include "StradaGAD.h"
#include "tool.h"
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
GAData::GAData(){
}
GAData::~GAData(){
}

StradaGAD::StradaGAD(){
    csv = false;
    nZone = nData = 0;
}

StradaGAD::~StradaGAD(){
}

void StradaGAD::clear(){
	GA.clear();
    nZone = nData = 0;
}
// 合計欄を設ける
void StradaGAD::init(int nZone, int nData){
	clear();
	this->nZone = nZone;
	this->nData = nData;
	GA.resize(nData);
	for(int i=0; i< nData; i++) {
		GA[i].name = std::to_string(i+1);
		GA[i].data.resize(nZone+1,0);
	}
}
int StradaGAD::get_data(int field, int zone) {
	assert( field >= 0 && field < nData && zone >= 0 && zone < nZone +1);
	return (int)GA[field].data[zone];
}
////////////////////////////////////////////////////////////////////////////////
//  失敗した場合は、失敗した行の数をマイナスで返す。
//  ヘッダーでVersion 1でも強引に読み込む。
////////////////////////////////////////////////////////////////////////////////
int StradaGAD::Read(FILE* fp) {
	char* p;
	int n;
	char buf[2048];
	if( fgets(buf, 2048, fp) == NULL) return (-1);
	buf[ strlen(buf) - 1 ] = '\0';
	if( strncmp(buf, "GAD", 3) != 0 ) return (-1);
	if( strlen(buf) > 4 ) {
		if( buf[4] == '*' ) csv = true; else csv = false;
		if( strlen(buf) > 6 ) {
			title = &buf[5];
		}
	}
	if( fgets(buf, 2048, fp) == NULL) return (-2);
	buf[ strlen(buf) - 1 ] = '\0';
	if( csv ) {
		p = strtok(buf, ",");
		if ( p == NULL ) return (-2);
		nZone = atoi(p);
		p = strtok(NULL, ",");
		if (p == NULL ) return (-3);
		nData = atoi(p);
	} else {
		nZone = getbufInt(buf, 0, 5 );
		nData = getbufInt(buf, 5, 5 );
	}
	if( nZone <= 0 || nData <= 0 ) return (-2);
	int counter = -3;
	GA.resize(nData);
	for(int i=0; i< nData; i++) {
		GA[i].data.resize(nZone+1,0);
	}
	for(int i=0; i < nData; i++) {
		if( fgets(buf, 2048, fp) == NULL) {
			clear();
			return (counter);
		}
		counter--;
		buf[ strlen(buf) - 1 ] = '\0';
		if ( csv ) {
			p = strtok(buf,",");
			if ( p == NULL ) {
				clear();
				return (counter);
			}
			p = strtok(NULL, ",");
			GA[i].name =  p;
		} else {
			GA[i].name = &buf[5];
		}
		boost::trim(GA[i].name);
	}
	for(int i=0; i < nZone; i++) {
		if( fgets(buf, 2048, fp) == NULL) {
			clear();
			return (counter);
		}
		counter--;
		buf[ strlen(buf) - 1 ] = '\0';
		if( csv ) {
			p = strtok(buf, ",");
			n = nData;
			while (n) {
				if( p == NULL ) {
					clear();
					return (counter);
				}
				GA[nData - n].data[i] = atof(p);
				n--;
				p = strtok(NULL, ",");
			}
		} else {
			for(int j=0; j < nData; j++) {
				GA[j].data[i] = getbufFlt(buf, 8*j, 8);
			}
		}
	}
	return (0);
}
////////////////////////////////////////////////////////////////////////////////
//  ファイル名から読み込み
////////////////////////////////////////////////////////////////////////////////
void StradaGAD::Read(char* fname) {
	FILE* fp;
	if((fp = fopen(fname ,"rt"))==NULL) throw std::runtime_error("GAD");
	int ret = Read(fp);
	fclose(fp);
	if( ret < 0 ) {
		err_msg = "GAD Reading Error line :" + std::to_string(-ret);
		throw std::runtime_error( msg() );
	}
}
////////////////////////////////////////////////////////////////////////////////
//  ファイルの書き込み
//  2011-9-15(Thu) 最大有効桁数を8桁に変更
////////////////////////////////////////////////////////////////////////////////
void StradaGAD::Write(FILE* fp){
	if( csv) {
		fprintf(fp, "GAD2*%s\n", title.c_str());
		fprintf(fp,"%d,%d\n",nZone, nData);
		if( nData < 1 ) return;
		for(int i=0; i < nData; i++) {
			fprintf(fp,"%d,\"%s\"\n", i+1, GA[i].name.c_str());
		}
		for(int i=0; i < nZone; i++) {
			for(int j=0; j < nData-1; j++) {
				fprintf(fp,"%.8g,", GA[j].data[i] );
			}
			fprintf(fp,"%.8g\n", GA[nData-1].data[i]);
		}
	} else {
		fprintf(fp, "GAD2 %s\n", title.c_str());
		fprintf(fp,"%5d%5d\n", nZone, nData);
		if( nData < 1 ) return;

		for(int i=0; i < nData; i++) {
			fprintf(fp,"%5d%-20s\n", i+1, GA[i].name.c_str());
		}

		for(int i=0; i < nZone; i++) {
			for(int j=0; j < nData; j++) {
				fprintf(fp,"%8.0f", GA[j].data[i] );
			}
			fprintf(fp,"\n");
		}
	}
}

void StradaGAD::set_title(int n, const char* str){
	if ( n < 0 || n >= nData ) return;
	GA[n].name = str;
}

void StradaGAD::set_data(int nd, int nz, double n) {
	if ( nd < 0 || nd >= nData || nz < 0 || nz >= nZone ) return;
	GA[nd].data[nz] = n ;
}

int StradaGAD::field_no(const char* str) {
	int ret = -1;
	for(int i=0; i < nData; i++) {
		if( GA[i].name.compare(str) == 0 ){
			ret = i;
			break;
		}
	}
	return ret;
}

void StradaGAD::calc_total() {
    for(int i=0; i < nData; i++) {
		double dt = 0;
		for(int j=0; j < nZone; j++) {
			dt += GA[i].data[j];
		}
		GA[i].data[nZone] = (int)dt ;
	}
}

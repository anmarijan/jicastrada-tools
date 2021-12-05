#include <string.h>
#include <stdlib.h>
#include <stdexcept>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <boost/tokenizer.hpp>
#include "StradaGAD.h"
#include "tool.h"
#include "StradaCmn.h"
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
typedef boost::tokenizer< boost::escaped_list_separator<char> > TOKENIZER;
typedef boost::tokenizer< boost::escaped_list_separator<char> >::iterator TOKENIZER_ITERATOR;
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
//
////////////////////////////////////////////////////////////////////////////////
void StradaGAD::Read(const char* fname) {
	std::ifstream ifs(fname, std::ios_base::in);
	if (!ifs) {
		throw std::runtime_error("Cannot open AOD file.");
	}
	try {
		std::string buff;
		if (std::getline(ifs, buff).fail()) throw (1);
		if (buff.compare(0, 3, "GAD") != 0) throw (1);
		if ( buff.length() > 4) {
			if (buff[4] == '*') csv = true; else csv = false;
			if (buff.length() > 6) {
				title = buff.substr(5);
			}
		}
		if (std::getline(ifs, buff).fail()) throw (2);
		if (csv) {
			boost::tokenizer<boost::escaped_list_separator<char> > tokens(buff);
			boost::tokenizer<boost::escaped_list_separator<char> >::iterator it = tokens.begin();
			if (it == tokens.end()) throw(2);
			nZone = std::stoi(*it);
			++it; if (it == tokens.end()) throw(2);
			nData = std::stoi(*it);
		}
		else {
			nZone = std::stoi(buff.substr(0, 5));
			nData = std::stoi(buff.substr(5, 5));
		}
		if (nZone <= 0 || nData <= 0) throw (2);
		int counter = 3;
		GA.resize(nData);
		for (int i = 0; i < nData; i++) {
			int S = nZone + 1;
			GA[i].data.resize(S, 0);
		}
		for (int i = 0; i < nData; i++) {
			if (std::getline(ifs, buff).fail()) {
				clear();
				throw (counter);
			}
			counter++;
			if (csv) {
				TOKENIZER tokens(buff);
				TOKENIZER_ITERATOR it = tokens.begin();
				if (it == tokens.end()) {
					clear();
					throw(counter);
				}
				++it;
				if (it == tokens.end()) {
					clear();
					throw(counter);
				}
				GA[i].name = trim(*it);
			}
			else {
				GA[i].name = trim(buff.substr(5));
			}
		}
		for (int i = 0; i < nZone; i++) {
			if (std::getline(ifs, buff).fail()) {
				clear();
				throw (counter);
			}
			counter++;
			if (csv) {
				TOKENIZER tokens(buff);
				TOKENIZER_ITERATOR it = tokens.begin();
				int n = nData;
				while (n) {
					if (it == tokens.end()) {
						clear();
						throw (counter);
					}
					GA[nData - n].data[i] = std::stod(*it);
					n--;
					++it;
				}
			}
			else {
				for (int j = 0; j < nData; j++) {
					GA[j].data[i] = std::stod(buff.substr(8*j,8));
				}
			}
		}
	}
	catch (int n) {
		err_msg = "GAD Reading Error line :" + std::to_string(n);
		throw std::runtime_error(msg());
	}
}
////////////////////////////////////////////////////////////////////////////////
//
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

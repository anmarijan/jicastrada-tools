/*-------------------------------------------------------------------------*/
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <cfloat>
#include <limits.h> //INT_MAX
/*-------------------------------------------------------------------------*/
#include <set>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <boost/tokenizer.hpp>
/*-------------------------------------------------------------------------*/
#include "StradaCmn.h"
#include "StradaIRE.h"
#include "StradaINT.h"
#include "tool.h"
/*-------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void intrusive_ptr_add_ref(IRELinkV2* p) {
	p->ref_counter++;
}
void intrusive_ptr_release(IRELinkV2* p) {
	if( p->ref_counter > 1 ) p->ref_counter--;
	else {
		p->ref_counter = 0;
		delete p;
	}
}
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
OnewayResult::OnewayResult(){
	avSp = ltSp = VCR = 0;
	Vol = ATL = 0;
	for(int i=0; i < 10; i++){
		inVol[i] = 0;
		thVol[i] = 0;
		btVol[i] = 0;
	}
	for(int i=0; i< 6; i++) trVol[i] = 0;
}
/////////////////////////////////////////////////////////////////////////////
// Total of all modes (inside=1, in-out=2, out-out=3)
/////////////////////////////////////////////////////////////////////////////
float OnewayResult::total(int i) {
	float s = 0;
	if( i == 1 ) {
		for(int i=0; i < 10; i++) s += inVol[i];
	} else if(i == 2) {
		for(int i=0; i < 10; i++) s += thVol[i];
	} else if(i == 3) {
		for(int i=0; i < 10; i++) s += btVol[i];
	} else s = Vol;
	return s;
}
/////////////////////////////////////////////////////////////////////////////
// Volume by mode (inside=1, in-out=2, out-out=3)
/////////////////////////////////////////////////////////////////////////////
float OnewayResult::mode_vol(int m, int t) {
	float vol = 0;
	switch (t) {
		case 0:
			vol = inVol[m] + thVol[m] + btVol[m];
			break;
		case 1:
			vol = inVol[m];
			break;
		case 2:
			vol = thVol[m];
			break;
		case 3:
			vol = btVol[m];
			break;
		default:
			break;
	}
	return vol;
}
/////////////////////////////////////////////////////////////////////////////
// Calculate no. of passengers, it is zero when PCU=0
/////////////////////////////////////////////////////////////////////////////
float OnewayResult::pass_vol(float pcu[], float apc[], int t) {
	float vol = 0;
	float k;
	for(int i=0; i < 10; i++) {
		if ( pcu[i] > 0 ) k = apc[i]/pcu[i];
		else k = 0;
		switch (t) {
			case 0:
				vol += (inVol[i] + thVol[i] + btVol[i] ) * k;
				break;
			case 1:
				vol += inVol[i] * k;
				break;
			case 2:
				vol += thVol[i] * k;
				break;
			case 3:
				vol += btVol[i] * k;
				break;
			default:
				break;
		}
	}
	return vol;
}
/////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////
float OnewayResult::veh_vol(float pcu[], int t) {
	float vol = 0;
	float k;
	for(int i=0; i < 10; i++) {
		if ( pcu[i] > 0 ) k = 1/pcu[i];
		else k = 0;
		switch (t) {
			case 0:
				vol += (inVol[i] + thVol[i] + btVol[i] ) * k;
				break;
			case 1:
				vol += inVol[i] * k;
				break;
			case 2:
				vol += thVol[i] * k;
				break;
			case 3:
				vol += btVol[i] * k;
				break;
			default:
				break;
		}
	}
	return vol;
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
int OnewayResult::Read(const char* buf){

	try {
		avSp = getbufFlt(buf, 0, 5);
		ltSp = getbufFlt(buf, 5, 5);
		VCR  = getbufFlt(buf,10, 5);
		Vol  = getbufFlt(buf,15, 7);

		for(int i=0; i < 10; i++){
			inVol[i] = getbufFlt(buf, 22 + 21*i 	, 7 );
			thVol[i] = getbufFlt(buf, 22 + 21*i + 7 , 7 );
			btVol[i] = getbufFlt(buf, 22 + 21*i + 14, 7 );
		}
	//22+21*9+14+7 = 232
		ATL = getbufFlt(buf,232,6);

		for(int i=0; i < 6; i++)
			trVol[i] = getbufFlt(buf, 238 + 7*i, 7);
	} catch(std::runtime_error& ){
		return(-1);
	}
	return(0);
// 232+6+7*6 =
}

int OnewayResult::ReadCSV(std::string& str){
	std::string line = str;
	try {
		boost::tokenizer<boost::escaped_list_separator<char> > tokens(line);
		boost::tokenizer<boost::escaped_list_separator<char> >::iterator it = tokens.begin();
		if (it == tokens.end()) throw 1; else avSp = std::stof(*it);
		++it; if (it == tokens.end()) throw 1; else ltSp = std::stof(*it);
		++it; if (it == tokens.end()) throw 1; else VCR = std::stof(*it);
		++it; if (it == tokens.end()) throw 1; else Vol = std::stof(*it);
		for (int i = 0; i < 10; i++) {
			++it; if (it == tokens.end()) throw 1; else inVol[i] = std::stof(*it);
			++it; if (it == tokens.end()) throw 1; else thVol[i] = std::stof(*it);
			++it; if (it == tokens.end()) throw 1; else btVol[i] = std::stof(*it);
		}
		++it; if (it == tokens.end()) throw 1; else ATL = std::stof(*it);
		for (int i = 0; i < 6; i++) {
			++it; if (it == tokens.end()) throw 1; else	trVol[i] = std::stof(*it);
		}
	}
	catch (...) {
		return(-1);
	}
	return(0);
}
//---------------------------------------------------------------------------
// Save file
//---------------------------------------------------------------------------
void OnewayResult::Write(FILE* fp){

	char buff[300];
	fixfloat(buff,    avSp, 5);
	fixfloat(buff+5 , ltSp, 5);
	fixfloat(buff+10, VCR,  5);
	fixfloat(buff+15, Vol,  7);
	for(size_t i=0; i < 10; i++){
		fixfloat(buff+22+21*i, inVol[i], 7);
		fixfloat(buff+29+21*i, thVol[i], 7);
		fixfloat(buff+36+21*i, btVol[i], 7);
	}
	fixfloat(buff+232, ATL, 6);
	for(size_t i=0; i < 6; i++) fixfloat(buff+238+7*i, trVol[i], 7);

	fprintf(fp, "%s", buff);
//	if( VCR > 99.99 ) VCR = 99.99;
//	fprintf(fp,"%5.1f%5.1f%5.2f%7d",avSp,ltSp,VCR,Vol);
//	for(int i=0; i < 10; i++){
//		fprintf(fp,"%7d%7d%7d",inVol[i],thVol[i],btVol[i]);
//	}
//	fprintf(fp,"%6.1f",ATL);
//	for(int i=0; i < 6; i++) fprintf(fp,"%7d",trVol[i]);
}

void OnewayResult::WriteCSV(FILE* fp) {
	fprintf(fp,"%g,%g,%g,%g,", avSp, ltSp, VCR, Vol);
	for(int i=0; i< 10; i++) fprintf(fp,"%g,%g,%g,",inVol[i], thVol[i], btVol[i]);
	fprintf(fp,"%g,", ATL);
	for(int i=0; i < 6; i++) fprintf(fp, "%g,",trVol[i]);
}

IRELinkV2::IRELinkV2() : SLinkV2() {
	ref_counter = 0;

}

IRELinkV2::IRELinkV2(SLinkV2& s) : SLinkV2(s) {
	ref_counter = 0;
}

IRELinkV2::IRELinkV2(const LinkPtr link) : SLinkV2(*(link.get())) {
	ref_counter = 0;
}
//------------------------------------------------------------------------------
//! Read link data
//------------------------------------------------------------------------------
int IRELinkV2::Read(const char* buf){
	std::string line = buf;
	std::string str;
	try {
		str = trim(line.substr(0, 10)); strcpy_s(name, 11, str.c_str());
		str = trim(line.substr(10, 10)); strcpy_s(sNode, 11, str.c_str());
		str = trim(line.substr(20, 10)); strcpy_s(eNode, 11, str.c_str());

		length = std::stof(line.substr(30, 7));
        Vmax   = std::stof(line.substr(37, 5));
        Capa   = std::stof(line.substr(42, 8));
        QV	   = std::stoi(line.substr(50, 2));
        for(size_t i=0; i < 10; i++) ways[i] = line[52+i] ;

		str = line.substr(62, 280);
        if( result[0].Read(str.c_str()) == -1) throw std::runtime_error("IRE");

		str = line.substr(342, 280);
		if( result[1].Read(str.c_str()) == -1) throw std::runtime_error("IRE");

        linktype = line[622];
        if(line[623] == '0') evaluation = true; else evaluation = false;
        display = line[624];
        aFlag1	= line[625];
		nFlag2 = std::stoi(line.substr(626, 2));
		nFlag3 = std::stoi(line.substr(628, 2));
        aFlag4[0]  = line[630];
        aFlag4[1]  = line[631];
        aFlag5[0]  = line[632];
        aFlag5[1]  = line[633];
        aFlag5[2]  = line[634];

		iX = std::stof(line.substr(645, 10));
		iY = std::stof(line.substr(655, 10));
		jX = std::stof(line.substr(665, 10));
		jY = std::stof(line.substr(675, 10));

        dummy = std::stoi(line.substr(685, 5));

        if( dummy > 0 ) {
			dX[0] = std::stof(line.substr(690, 10));
			dY[0] = std::stof(line.substr(700, 10));

			if( dummy > 1 ) {
				dX[1] = std::stof(line.substr(710, 10)); 
				dY[1] = std::stof(line.substr(720, 10)); 

				if( dummy > 2 ) {
					dX[2] = std::stof(line.substr(730, 10)); 
					dY[2] = std::stof(line.substr(740, 10)); 
				}
            }

        }
	} catch (const std::exception& e) {
		throw e;
    }
	return(1);
}
//---------------------------------------------------------------------------
// SPACEs are trimmed
//---------------------------------------------------------------------------
void IRELinkV2::ReadCSV(const char* buf){
	std::vector<char> vec(strlen(buf)+1);
	vec[strlen(buf)] = '\0';
	for (size_t i = 0; i < strlen(buf); i++) vec[i] = buf[i];
	char* buffer = &vec[0];
	char* arr[200];
	for (int i = 0; i < 200; i++) arr[i] = nullptr;
	if( csv_parser(buffer, arr, 200, ',') == false) throw std::runtime_error("IRE") ;
	size_t count;
	for (count = 0; count < 200; count++) {
		if (arr[count] == nullptr) break;
	}
	if (count < 113) throw std::runtime_error("IRE CSV");
	try {

		strcpy_s(name, 11, arr[0]); trim(name);
		strcpy_s(sNode, 11, arr[1]); trim(sNode);
		strcpy_s(eNode, 11, arr[2]); trim(eNode);
		length = (float)atof(arr[3]);
		Vmax = (float)atof(arr[4]);
		Capa = (float)atof(arr[5]);
		QV = atoi(arr[6]);
		for (size_t i = 0; i < 10; i++) {
			ways[i] = arr[7 + i][0];
		}
		for (size_t r = 0; r < 2; r++) {
			size_t idx = 17 + 41 * r;
			result[r].avSp = (float)atof(arr[idx]);
			result[r].ltSp = (float)atof(arr[idx + 1]);
			result[r].VCR = (float)atof(arr[idx + 2]);
			result[r].Vol = (float)atof(arr[idx + 3]);
			for (size_t i = 0; i < 10; i++) {
				result[r].inVol[i] = (float)atof(arr[idx + 4 + 3 * i]);
				result[r].thVol[i] = (float)atof(arr[idx + 5 + 3 * i]);
				result[r].btVol[i] = (float)atof(arr[idx + 6 + 3 * i]);
			}
			result[r].ATL = (float)atof(arr[idx + 34]);
			for (size_t i = 0; i < 6; i++) {
				result[r].trVol[i] = (float)atof(arr[idx + 35 + i]);
			}
		}
		linktype = arr[99][0];
		if (arr[100][0] == '0') evaluation = true; else evaluation = false;
		display = arr[101][0];
		if (arr[102][0] == '\0') aFlag1 = ' ';
		else aFlag1 = arr[102][0];
		nFlag2 = atoi(arr[103]);
		nFlag3 = atoi(arr[104]);
		for (size_t i = 0; i < 2; i++) {
			if (arr[105][i] == '\0') break;
			aFlag4[i] = arr[105][i];
		}
		for (size_t i = 0; i < 3; i++) {
			if (arr[106][i] == '\0') break;
			aFlag5[i] = arr[106][i];
		}
		// arr[107] reserved
		iX = (float)atof(arr[108]);
		iY = (float)atof(arr[109]);
		jX = (float)atof(arr[110]);
		jY = (float)atof(arr[111]);
		dummy = atoi(arr[112]);
		if (dummy > 0) {
			if (arr[113] == nullptr) throw 113; else dX[0] = (float)atof(arr[113]);
			if (arr[114] == nullptr) throw 114; else dY[0] = (float)atof(arr[114]);
			if (dummy > 1) {
				if (arr[115] == nullptr) throw 115; else dX[1] = (float)atof(arr[115]);
				if (arr[116] == nullptr) throw 116; else dY[1] = (float)atof(arr[116]);
				if (dummy > 2) {
					if (arr[117] == nullptr) throw 117; else dX[2] = (float)atof(arr[117]);
					if (arr[118] == nullptr) throw 118; else dY[2] = (float)atof(arr[118]);
				}
			}
		}
	}
	catch (int e) {
		throw std::runtime_error("IRE CSV:" + std::to_string(e));
	}
}
/////////////////////////////////////////////////////////////////////////////
// Persons (both)
/////////////////////////////////////////////////////////////////////////////
float IRELinkV2::pass_vol(float pcu[], float apc[], int t) {
	return result[0].pass_vol(pcu, apc, t) + result[1].pass_vol(pcu, apc, t);
}
/////////////////////////////////////////////////////////////////////////////
// Vehicles (both)
/////////////////////////////////////////////////////////////////////////////
float IRELinkV2::veh_vol(float pcu[], int t) {
	return result[0].veh_vol(pcu, t) + result[1].veh_vol(pcu, t);
}
/////////////////////////////////////////////////////////////////////////////
// PCU-KM
/////////////////////////////////////////////////////////////////////////////
float IRELinkV2::pcu_km(int m, int t) {
	return length * (result[0].mode_vol(m,t) + result[1].mode_vol(m,t) );
}
/////////////////////////////////////////////////////////////////////////////
// PCU-HOUR
/////////////////////////////////////////////////////////////////////////////
float IRELinkV2::pcu_hr(int m, int t) {
	double a , b;
	if( result[0].ltSp <= 0 ) a = 0;
	else a = result[0].mode_vol(m,t)/result[0].ltSp ;
	if( result[1].ltSp <= 0 ) b = 0;
	else b = result[1].mode_vol(m,t)/result[1].ltSp ;

	return (float) (length * ( a + b )) ;
}
/////////////////////////////////////////////////////////////////////////////
// PCU-HOUR:Speed
/////////////////////////////////////////////////////////////////////////////
double IRELinkV2::pcu_hr_sp(int m, double sp, int t) {
	double a, b;
	if (sp <= 0 ) return 0;
	if( result[0].ltSp <= 0 ) a = 0;
	else if( result[0].ltSp < sp ) a = result[0].mode_vol(m,t)/result[0].ltSp ;
	else a = result[0].mode_vol(m,t)/sp;

	if( result[1].ltSp <= 0 ) b = 0;
	else if( result[1].ltSp < sp ) b = result[1].mode_vol(m,t)/result[1].ltSp ;
	else b = result[1].mode_vol(m,t)/sp;

	return length * (a + b );
}
/////////////////////////////////////////////////////////////////////////////
// VCR (both)
/////////////////////////////////////////////////////////////////////////////
double IRELinkV2::VCR() {
	double ret = 0;
	if (Capa > 0) {
		ret = (result[0].Vol+result[1].Vol)/Capa ;
	}
	return ret;
 }
/////////////////////////////////////////////////////////////////////////////
// VCR (direction d)
/////////////////////////////////////////////////////////////////////////////
double IRELinkV2::VCR(int d) {
	double ret = 0;
	if (Capa > 0) {
		ret = result[d].Vol / (Capa * 0.5) ;
	}
	return ret;
 }

//---------------------------------------------------------------------------
// Read a link
//---------------------------------------------------------------------------
void IRELinkV2::Write(FILE* fp){
	char buff[64];
	fprintf(fp,"%-10s%-10s%-10s", name, sNode, eNode);
	fixfloat(buff, length, 7);
	fprintf(fp,"%7s%5.1f%8d",buff, Vmax,(int)Capa);
	fprintf(fp,"%2d", QV);
	for(int i=0; i< 10; i++) fprintf(fp,"%c",ways[i]);

	result[0].Write(fp);
	result[1].Write(fp);

	if(evaluation) fprintf(fp,"%c0",linktype); else fprintf(fp,"%c1",linktype);
	fprintf(fp,"%c%c%2d%2d",display, aFlag1,nFlag2, nFlag3);
	fprintf(fp,"%c%c%c%c%c",aFlag4[0],aFlag4[1],aFlag5[0],aFlag5[1],aFlag5[2]);
	fprintf(fp,"          ");
	fprintf(fp,"%10.8g%10.8g%10.8g%10.8g", iX,iY,jX,jY);
	fprintf(fp,"%5d",dummy);
	for(int i=0; i < dummy; i++) {
		fprintf(fp,"%10.8g%10.8g",dX[i],dY[i]);
	}
	fprintf(fp,"\n");
}

void IRELinkV2::WriteCSV(FILE* fp) {
	fprintf(fp,"%s,%s,%s,%g,%g,%g,%d,", name, sNode, eNode, length, Vmax, Capa, QV);
	for(int i=0; i < 10; i++) fprintf(fp,"%c,",ways[i]);
	result[0].WriteCSV(fp);
	result[1].WriteCSV(fp);
	if(evaluation) fprintf(fp,"%c,0,",linktype); else fprintf(fp,"%c,1,",linktype);
	fprintf(fp,"%c,%c,%2d,%2d,",display, aFlag1,nFlag2, nFlag3);
	fprintf(fp,"%c%c,%c%c%c,",aFlag4[0],aFlag4[1],aFlag5[0],aFlag5[1],aFlag5[2]);
	fprintf(fp,"\"          \",");  //Strada Ver 3.5
//	fprintf(fp,"          ");
	fprintf(fp,"%.7g,%.7g,%.7g,%.7g,", iX,iY,jX,jY);
	fprintf(fp,"%5d",dummy);
	for(int i=0; i < dummy; i++) {
		fprintf(fp,",%.7g,%.7g",dX[i],dY[i]);
	}
	fprintf(fp,"\n");
}
//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
StradaIRE::StradaIRE(){
	nLink = nNode = nMode = 0;
	version = '2';
    coordinate = 0;
	csv = false;
	for( int i=0; i < 10; i++){
		APC[i] = 0;
		PCU[i] = 0;
	}
    Ranks[0] =  5;
    Ranks[1] = 10;
    Ranks[2] = 15;
    Ranks[3] = 20;
    Ranks[4] = 30;
	mix1 = mix2 = mix3 = mix4 = miy1 = miy2 = miy3 = miy4 = 0;
}
//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
StradaIRE::~StradaIRE(){
}

void StradaIRE::clear(){
	links.clear();
	nLink = nNode = 0;
}

void StradaIRE::init(StradaINT& s_int){

	if( s_int.nLink < 0 || s_int.nLink > 40000 ||
		s_int.nNode < 0 || s_int.nNode > 30000 )  return;

    clear();
	SLinkV2 s;
	for(int i=0; i < nLink; i++){
		s = *(s_int.getLink(i));
		links.push_back(new IRELinkV2(s));
	}
	nLink = s_int.nLink;
	nNode = s_int.nNode;
	coordinate = s_int.coordinate;

}

void StradaIRE::resize(int nlink) {
	clear();
	for(int i=0; i < nlink; i++) {
		links.push_back(new IRELinkV2());
	}
	nLink = nlink;
}

void StradaIRE::WriteAsINTV2(FILE* fp){
    StradaINT s_int;
	SLinkV2 s;
	for(int i=0; i < nLink; i++){
		s = *(links[i]);
		s_int.links.push_back(new INTLinkV2(s));
	}
    s_int.nLink = nLink;
    s_int.nNode = nNode;
    s_int.coordinate = coordinate;

    s_int.Write(fp);
}
////////////////////////////////////////////////////////////////////////////////
//  Save as INT2 format
////////////////////////////////////////////////////////////////////////////////
void StradaIRE::WriteAsINTV2(char* fname){
	FILE* fp = NULL;
	errno_t error = fopen_s(&fp, fname, "wt");
	if( error != 0 || fp == NULL) throw std::runtime_error("IRE2");
	else {
		WriteAsINTV2(fp);
		fclose(fp);
	}
}
//------------------------------------------------------------------------------
// Read IRE file
//------------------------------------------------------------------------------
void StradaIRE::Read(const char* fname){
	std::ifstream ifs(fname);
	if (!ifs) {
		throw std::runtime_error("Cannot open IRE file");
	}
	std::string buff;
	try {
		if (std::getline(ifs, buff).fail()) throw(1);
        if (buff.length() < 5) throw(1);
		if (buff.compare(0, 3, "IRE") != 0) throw(1);
        version = buff[3];
		if (buff[4] == '*') csv = true; else csv = false;
		comment = buff.substr(5);
		if (std::getline(ifs, buff).fail()) throw(2);
		try {
			if (csv) {
				boost::tokenizer<boost::escaped_list_separator<char> > tokens(buff);
				boost::tokenizer<boost::escaped_list_separator<char> >::iterator it = tokens.begin();
				if (it == tokens.end()) throw std::exception(); else nLink = std::stoi(*it); ++it;
				if (it == tokens.end()) throw std::exception(); else nNode = std::stoi(*it); ++it;
				if (it == tokens.end()) throw std::exception(); else nMode = std::stoi(*it);
				for (int i = 0; i < 5; i++) {
					++it; if (it == tokens.end()) throw std::exception(); else Ranks[i] = std::stof(*it);
				}
				++it; if (it == tokens.end()) throw std::exception(); else coordinate = std::stoi(*it);
				for (int i = 0; i < 10; i++) {
					++it; if (it == tokens.end()) throw std::exception(); else APC[i] = std::stof(*it);
					++it; if (it == tokens.end()) throw std::exception(); else PCU[i] = std::stof(*it);
				}
			}
			else {

				nLink = std::stoi(buff.substr(0, 5));
				nNode = std::stoi(buff.substr(5, 5));
				nMode = std::stoi(buff.substr(10, 5));

				for (size_t i = 0; i < 5; i++) Ranks[i] = std::stof(buff.substr(15 + 5 * i, 5));
				coordinate = std::stoi(buff.substr(40, 5));

				for (size_t i = 0; i < 10; i++) {
					APC[i] = std::stof(buff.substr(45 + i * 10, 5));
					PCU[i] = std::stof(buff.substr(50 + i * 10, 5)); 
				}
			}

		}
		catch (const std::exception&) {
			throw(3);
		}

		try {
			links.clear();
			for (int i = 0; i < nLink; i++) {
				links.push_back(new IRELinkV2());
			}
		}
		catch (std::bad_alloc& ) {
			links.clear();
			throw (-1);
		}
		int count;

		try {
			for (count = 0; count < nLink; count++) {
				if (std::getline(ifs, buff).fail()) throw std::runtime_error("IRE2");
				if (csv) {
					links[count]->ReadCSV(buff.c_str());
				}
				else {
					if (links[count]->Read(buff.c_str()) == -1) throw std::runtime_error("LINE");
				}
			}
		}
		catch (const std::exception& e) {
			links.clear();
			msg = e.what() + std::to_string(count);
			throw(-2);
		}

	}
	catch (int e) {
		buff = "IRE2(" + std::to_string(e) + ")";
		throw std::runtime_error(buff);
	}

}
////////////////////////////////////////////////////////////////////////////////
//  Save IRE file
////////////////////////////////////////////////////////////////////////////////
void StradaIRE::Write(FILE* fp){
	char buff[10];
	// Count the number of nodes
	std::set<std::string> node_table;
	for(int i=0; i < nLink; i++){
		node_table.insert(links[i]->sNode);
		node_table.insert(links[i]->eNode);
	}
	nNode = static_cast<int>(node_table.size());

	if( csv ) fprintf(fp, "IRE2* ");
	else fprintf(fp,"IRE2 ");

	fprintf(fp, "%s\n", comment.c_str());

	if ( csv ) {
		fprintf(fp,"%d,%d,%d,",nLink, nNode, nMode);
		for(int i=0; i < 5; i++) fprintf(fp,"%f,",Ranks[i]);
		fprintf(fp, "%d,", coordinate);
		for(int i=0; i < 9; i++) fprintf(fp, "%f,%f,",APC[i], PCU[i]);
		fprintf(fp, "%f,%f\n", APC[9], PCU[9]);
		for(int i=0; i < nLink; i++) links[i]->WriteCSV(fp);

	} else {
		fprintf(fp,"%5d%5d%5d",nLink,nNode,nMode);
		for(int i=0; i < 5; i++) {
			fixfloat(buff, Ranks[i], 5);
			fprintf(fp,"%5s" ,buff);
		}

		fprintf(fp,"%1d    ", coordinate);
		for(int i=0; i < 10; i++){
			fixfloat(buff, APC[i], 5);
			fprintf(fp,"%s",buff);
			fixfloat(buff, PCU[i], 5);
			fprintf(fp,"%s",buff);
		}
		fprintf(fp,"\n");


		for(int i=0; i < nLink; i++) links[i]->Write(fp);
	}
}
////////////////////////////////////////////////////////////////////////////////
//  Save IRE file as fname
////////////////////////////////////////////////////////////////////////////////
void StradaIRE::Write(char* fname) {
	FILE* fp = NULL;
	errno_t error = fopen_s(&fp, fname, "wt");
	if(error != 0 || fp==NULL) throw std::runtime_error("IRE2");
	else {
		Write(fp);
		fclose(fp);
	}
}
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void StradaIRE::WriteMInfo(const char* fname, double dai, int vtype) {
    FILE* fp_mif = NULL;
    FILE* fp_mid = NULL;
	std::string file_name;
    double width;
	std::string fullname = fname;
	std::string bodyname;
	size_t pos = fullname.find_last_of(".");
	if (pos > 0) bodyname = fullname.substr(0, pos);
	else bodyname = fullname;
    float xo, yo, xm, ym;
    xo = FLT_MAX ;
    yo = FLT_MAX ;
    xm = 0;
    ym = 0;
	file_name = bodyname + ".mif";
	errno_t error = fopen_s(&fp_mif, file_name.c_str(), "wt");
	if (error == 0 && fp_mif != NULL) {
        file_name = bodyname + ".mid";
		error = fopen_s(&fp_mid, file_name.c_str(), "wt");
        if(error == 0 && fp_mid != NULL) {
              conv(1,0);

              for( int i=0; i < nLink ; i++) {
                  xo = ( links[i]->iX < xo )? links[i]->iX : xo ;
                  xo = ( links[i]->jX < xo )? links[i]->jX : xo ;
                  yo = ( links[i]->iY < yo )? links[i]->iY : yo ;
                  yo = ( links[i]->jY < yo )? links[i]->jY : yo ;

                  xm = ( links[i]->iX > xm )? links[i]->iX : xm ;
                  xm = ( links[i]->jX > xm )? links[i]->jX : xm ;
                  ym = ( links[i]->iY > ym )? links[i]->iY : ym ;
                  ym = ( links[i]->jY > ym )? links[i]->jY : ym ;

              }

              fprintf(fp_mif,"Version 300\n");
              fprintf(fp_mif,"Charset \"WindowsJapanese\"\n");
              fprintf(fp_mif,"Delimiter \",\"\n");

              fprintf(fp_mif,"CoordSys NonEarth Units \"km\" Bounds (%g,%g) (%g,%g)\n", xo,yo, xm,ym);
              fprintf(fp_mif,"Columns 8\n");
              fprintf(fp_mif,"  LinkName Char(10)\n");
              fprintf(fp_mif,"  Length Decimal(12,4)\n");
              fprintf(fp_mif,"  Capacity Integer\n");
              fprintf(fp_mif,"  Volume Integer\n");
              fprintf(fp_mif,"  Vol1 Integer\n");
              fprintf(fp_mif,"  Vol2 Integer\n");
              fprintf(fp_mif,"  VCR Decimal(6,2)\n");
              fprintf(fp_mif,"  Display Char(1)\n");


              fprintf(fp_mif,"Data\n\n");

              for(int i=0; i < nLink; i++) {
              	switch (vtype)
                {
					case 1 : width = links[i]->result[0].Vol /dai;
						break;
					case 2 : width = links[i]->result[1].Vol /dai;
						break;
					default:width = links[i]->Vol() / dai ;
						break;
                }
                calc_mixy(links[i]->iX, links[i]->iY, links[i]->jX, links[i]->jY, width );
                fprintf(fp_mid, "%-10s,%12.4f,%10d,%10d,%10d,%10d,%12.4f,%c\n",
                    links[i]->name, links[i]->length, (int)links[i]->Capa,
                    (int)links[i]->Vol(), (int)links[i]->result[0].Vol, (int)links[i]->result[1].Vol, links[i]->VCR(), links[i]->display);
                fprintf(fp_mif,"Region 1\n");
                fprintf(fp_mif, "  5\n");
                fprintf(fp_mif,"%12.7f %12.7f\n", mix1, miy1);
                fprintf(fp_mif,"%12.7f %12.7f\n", mix2, miy2);
                fprintf(fp_mif,"%12.7f %12.7f\n", mix3, miy3);
                fprintf(fp_mif,"%12.7f %12.7f\n", mix4, miy4);
                fprintf(fp_mif,"%12.7f %12.7f\n", mix1, miy1);

              }

            fclose(fp_mid);
        }
        fclose(fp_mif);
        conv(1, 0);
    }
}
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void StradaIRE::calc_mixy(double xi, double yi, double xj, double yj, double width) {
    double r = std::sqrt(std::pow(xi-xj,2)+std::pow(yi-yj,2));
    double dx,dy;
    double w = width / 2;
    if(r == 0) {
    	dx = dy = 0;
    } else {
    	dx = (yj - yi) / r;
        dy = (xj - xi) / r;
    }
    mix1 = xi - w * dx;
    mix2 = xj - w * dx;
    mix3 = xj + w * dx;
    mix4 = xi + w * dx;

    miy1 = yi + w * dy;
    miy2 = yj + w * dy;
    miy3 = yj - w * dy;
    miy4 = yi - w * dy;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void StradaIRE::conv(int cd, int mergin) {
	if( coordinate != '0' ) return;

	int y_max = 0;
    int y_min = INT_MAX ;

	for(int i=0; i < nLink; i++) {
		if( y_max < links[i]->iY ) y_max = (int)links[i]->iY;
		if( y_max < links[i]->jY ) y_max = (int)links[i]->jY;
		if( y_min > links[i]->iY ) y_min = (int)links[i]->iY;
		if( y_min > links[i]->jY ) y_min = (int)links[i]->jY;
	}
    fprintf(stdout, "%d %d:",y_min, y_max);
	for(int i=0; i < nLink; i++) {
		links[i]->iY = y_max - links[i]->iY + y_min + mergin;
		links[i]->jY = y_max - links[i]->jY + y_min + mergin;
	}
    fprintf(stdout, "Converted\n");
}

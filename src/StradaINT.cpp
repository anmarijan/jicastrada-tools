#include <pch.h>
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cstring>
#include <cfloat>	//FLT_MAX
//---------------------------------------------------------------------------
#include <string>
#include <vector>
#include <set>
#include <stdexcept>
#include <boost/tokenizer.hpp>
//#include <boost/algorithm/string.hpp>
//#include <boost/algorithm/string/trim.hpp>
#include "StradaCmn.h"
//---------------------------------------------------------------------------
#include "tool.h"
#include "StradaINT.h"
//---------------------------------------------------------------------------
#define INT_BUF 512
static char buff[INT_BUF];
//---------------------------------------------------------------------------
// For "sort by name"
//---------------------------------------------------------------------------
struct StradaINTComparison {
	bool operator () (INTLinkV2 * t1, INTLinkV2 * t2) const
    {
	    bool check = true;
		if( strcmp(t1->name, t2->name ) < 0 ) check = false;
    	return check;
    }
};
void intrusive_ptr_add_ref(INTLinkV2* p) {
	p->ref_counter++;
}
void intrusive_ptr_release(INTLinkV2* p) {
	if( p->ref_counter > 1 ) p->ref_counter--;
	else {
		p->ref_counter = 0;
		delete p;
	}
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
INTLinkV2::INTLinkV2() : SLinkV2(){
	ref_counter = 0;
	for(int i=0; i < 10; i++) fare[i] = 0.0;
	QV = 6;
	color = '0';
	bLinkOD = false;
	bRouteInf = false;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
INTLinkV2::INTLinkV2(SLinkV2& s) : SLinkV2(s) {
	ref_counter = 0;
	for(int i=0; i < 10; i++) fare[i] = 0.0;
	color = '0';
	bLinkOD = false;
	bRouteInf = false;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
INTLinkV2& INTLinkV2::operator=(const INTLinkV2& obj) {
	SLinkV2::operator=(obj);
	for(int i=0; i < 10; i++) fare[i] = obj.fare[i];
	color = obj.color;
	bLinkOD = obj.bLinkOD;
	bRouteInf = obj.bRouteInf;
	return (*this);
}
////////////////////////////////////////////////////////////////////////////////
//	Read 1 line (Ver2)
////////////////////////////////////////////////////////////////////////////////
bool INTLinkV2::Read(const char* link_str, size_t &pos){
	std::string buff = link_str;
	std::string str;
	try {
		str = trim(buff.substr(0, 10));
        strcpy_s(name, sizeof(name), str.c_str());
		pos = 10;  str = trim(buff.substr(10, 10)); strcpy_s(sNode, sizeof(sNode), str.c_str());
		pos = 20;  str = trim(buff.substr(20, 10)); strcpy_s(eNode, sizeof(eNode), str.c_str());
		if (name[0] == '\0' || sNode[0] == '\0' || eNode[0] == '\0') return false;
		pos = 30; length = std::stof(buff.substr(30, 7));
		pos = 37; Vmax = std::stof(buff.substr(37, 5));
		pos = 42; Capa = std::stof(buff.substr(42, 8));
		pos = 50;  QV = std::stoi(buff.substr(50, 3));

		char c;
		for (size_t i = 0; i < 10; i++) {
			pos = 53 + i * 5;
			str = buff.substr(pos, 5);
			fare[i] = std::stof(str);
			pos = 103 + i;
			c = buff[pos];
			if (c != '1' && c != '2' && c != '3') c = '0';
			ways[i] = c;
		}
		pos = 113;  linktype = buff[113];
		pos = 114;  if (buff[114] == '1') evaluation = false;
		else evaluation = true;

		pos = 115; display = buff[115];
		pos = 116; aFlag1 = buff[116];
		pos = 117; nFlag2 = std::stoi(str = buff.substr(117, 2));
		pos = 119; nFlag3 = std::stoi(str = buff.substr(119, 2));
		pos = 121;
		aFlag4[0] = buff[pos++]; aFlag4[1] = buff[pos++];
		aFlag5[0] = buff[pos++]; aFlag5[1] = buff[pos++]; aFlag5[2] = buff[pos++];
		color = buff[pos];
		pos = 140;  iX = std::stof(buff.substr(140, 10));
		pos = 150;  iY = std::stof(buff.substr(150, 10));
		pos = 160;  jX = std::stof(buff.substr(160, 10));
		pos = 170;  jY = std::stof(buff.substr(170, 10));
		pos = 180;  dummy = std::stoi(buff.substr(180, 5));
		if (dummy < 0 || dummy > 3) return false;
		if (dummy > 0) {
			pos = 185;  dX[0] = std::stof(buff.substr(185, 10));
			pos = 195;  dY[0] = std::stof(buff.substr(195, 10));
			if (dummy > 1) {
				pos = 205;  dX[1] = std::stof(buff.substr(205, 10));
				pos = 215;  dY[1] = std::stof(buff.substr(215, 10));
				if (dummy > 2) {
					pos = 225;  dX[2] = std::stof(buff.substr(225, 10));
					pos = 235;  dY[2] = std::stof(buff.substr(235, 10));
				}
			}
		}
	}
	catch (std::invalid_argument&) {
		return false;
	}
	catch (std::out_of_range&) {
		return false;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
// Copy attributes only
////////////////////////////////////////////////////////////////////////////////
void INTLinkV2::set_attr(INTLinkV2& link) {
	Vmax = link.Vmax;
	Capa = link.Capa;
	QV   = link.QV ;
	for(int i=0; i < 10; i++) {
		fare[i] = link.fare[i];
		ways[i] = link.ways[i];
	}
	evaluation  = link.evaluation;
	linktype    = link.linktype;
	display = link.display;
	aFlag1  = link.aFlag1;
	nFlag2  = link.nFlag2;
	nFlag3  = link.nFlag3;
	aFlag4[0] = link.aFlag4[0];
	aFlag4[1] = link.aFlag4[1];
	aFlag5[0] = link.aFlag5[0];
	aFlag5[1] = link.aFlag5[1];
	aFlag5[2] = link.aFlag5[2];
	color   = link.color;
}
#define SAFE_CPY(A,B) strncpy_s(A , sizeof((A)), B , sizeof((A))-1 ); A[sizeof(A)-1]=0; trim(A);
////////////////////////////////////////////////////////////////////////////////
//	Read a line of Ver2 CSV format 
////////////////////////////////////////////////////////////////////////////////
bool INTLinkV2::ReadCSV(char* link_str) {
	char* pdata[41];
	char* temp = csv_parser(link_str, pdata, 40, ',', '.'); //destroy buf

	SAFE_CPY(name, pdata[0]);
	SAFE_CPY(sNode, pdata[1]);
	SAFE_CPY(eNode, pdata[2]);
	length = (float)atof(pdata[3]);
	Vmax = (float)atof(pdata[4]);
	Capa = (float)atof(pdata[5]);
	QV = atoi(pdata[6]);
	for (int i = 0; i < 10; i++) fare[i] = (float)atof(pdata[7 + i]);
	for (int i = 0; i < 10; i++) ways[i] = pdata[17 + i][0];
	linktype = pdata[27][0];
	char a = pdata[28][0];
	if (a == '0') evaluation = true; else evaluation = false;
	display = pdata[29][0];
	aFlag1 = pdata[30][0];
	nFlag2 = atoi(pdata[31]);
	nFlag3 = atoi(pdata[32]);
	for (int counter = 0; pdata[33][counter] != 0 && counter < 2; counter++) aFlag4[counter] = pdata[33][counter];
	for (int counter = 0; pdata[34][counter] != 0 && counter < 3; counter++) aFlag5[counter] = pdata[34][counter];
	color = pdata[35][0];
	iX = (float)atof(pdata[36]);
	iY = (float)atof(pdata[37]);
	jX = (float)atof(pdata[38]);
	jY = (float)atof(pdata[39]);
	dummy = atoi(pdata[40]);
	if (dummy > 0 && temp != 0) csv_parser(temp, pdata, dummy * 2, ',', '.');
	for (int i = 0; i < dummy; i++) {
		dX[i] = (float)atof(pdata[2 * i]);
		dY[i] = (float)atof(pdata[2 * i + 1]);
	}
	return true;
}


////////////////////////////////////////////////////////////////////////////////
//	Read a line in Ver4 format
////////////////////////////////////////////////////////////////////////////////
bool INTLinkV2::ReadAsV4(char* buf) {
    char* pdata[46];
	char* temp = csv_parser(buf, pdata, 45, ',', '.'); //destroy buf

	SAFE_CPY(name, pdata[0]);
	SAFE_CPY(sNode, pdata[1]);
	SAFE_CPY(eNode, pdata[2]);
	length = (float) atof(pdata[3]);
	Vmax = (float) atof(pdata[4]);
	Capa = (float) atof(pdata[5]);
	QV = atoi(pdata[6]);
	for(int i=0; i < 10; i++) fare[i] = (float) atof(pdata[7+i]);
	for(int i=0; i < 10; i++) ways[i] = pdata[17+i][0];
	linktype = pdata[27][0];
	char a = pdata[28][0];
    if( a == '0') evaluation = true; else evaluation = false;
	display = pdata[29][0];
	aFlag1 = pdata[30][0];
	nFlag2 = atoi(pdata[31]);
	nFlag3 = atoi(pdata[32]);
	for( int counter=0; pdata[33][counter] != 0 && counter < 2 ; counter++) aFlag4[counter] = pdata[33][counter];
	for( int counter=0; pdata[34][counter] != 0 && counter < 3 ; counter++) aFlag5[counter] = pdata[34][counter];
//  nFlag6, 7, 8, 9
	color = pdata[39][0];
	iX = (float)atof(pdata[40]);
	iY = (float)atof(pdata[41]);
	jX = (float)atof(pdata[42]);
	jY = (float)atof(pdata[43]);
	dummy = atoi(pdata[44]);

	if( dummy > 0 && temp != 0 ) csv_parser(temp, pdata, dummy * 2, ',', '.' );
	for(int i=0; i < dummy; i++) {
		dX[i] = (float)atof(pdata[2*i]);
		dY[i] = (float)atof(pdata[2*i+1]);
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//	Read a line (Ver1)
////////////////////////////////////////////////////////////////////////////////
bool INTLinkV2::ReadAsV1(const char* buf){
	std::string line = buf;
	std::string str = trim(line.substr(0, 5)); strcpy_s(name, 5, str.c_str());
	str = trim(line.substr(5, 5)); strcpy_s(sNode, 5, str.c_str());
	str = trim(line.substr(10,5)); strcpy_s(eNode, 5, str.c_str());
////////////////
	try {
		length = std::stof(line.substr(15, 5));
		Vmax = std::stof(line.substr(20, 5));
		Capa = std::stof(line.substr(25, 8));
		QV = std::stoi(line.substr(33, 2));
		for (size_t i = 0; i < 5; i++) {
			fare[i] = std::stof(line.substr( 35 + i * 5, 5));
		}
		char c;
		for (int i = 0; i < 5; i++) {
			c = line[60 + i];
			if (c != '1' && c != '2' && c != '3') c = '0';
			ways[i] = c;
		}

		linktype = line[65];
		if (line[66] == '1') evaluation = false; else evaluation = true;
		display = line[67];
		aFlag1 = line[68];
		color = line[69];
		iX = std::stof(line.substr(70, 5));
		iY = std::stof(line.substr(75, 5));
		jX = std::stof(line.substr(80, 5));
		jY = std::stof(line.substr(85, 5));
	}
	catch (std::invalid_argument&) {
		return(false);
	}
	catch (std::out_of_range&) {
		return(false);
	}
	return(true);
}
////////////////////////////////////////////////////////////////////////////////
//  Read link data (one line)
////////////////////////////////////////////////////////////////////////////////
void INTLinkV2::Write(FILE* fp){

	fprintf(fp,"%-10s%-10s%-10s", name, sNode, eNode);
	if( Vmax > 999.9 ) Vmax = 999.9f ;
	fixfloat(buff, length, 7);
	fprintf(fp,"%s",buff);
	fixfloat(buff, Vmax, 5);
	if (Capa > 99999999) fprintf(fp, "%s99999999", buff);
	else fprintf(fp,"%s%8d",buff, (int)Capa);
	fprintf(fp,"%3d", QV);
	for(int i=0; i< 10; i++) {
        fixfloat(buff,fare[i],5);
        fprintf(fp,"%5s",buff);
    }
	for(int i=0; i< 10; i++) fprintf(fp,"%c",ways[i]);
	if(evaluation) fprintf(fp,"%c0",linktype); else fprintf(fp,"%c1",linktype);
	fprintf(fp,"%c%c%2d%2d",display, aFlag1,nFlag2, nFlag3);
	fprintf(fp,"%c%c%c%c%c%c",aFlag4[0],aFlag4[1],aFlag5[0],aFlag5[1],aFlag5[2],color);
	fprintf(fp,"             ");
//	fprintf(fp,"%10.8g%10.8g%10.8g%10.8g", iX,iY,jX,jY);
    fixfloat(buff, iX, 10); fprintf(fp,"%s", buff);
    fixfloat(buff, iY, 10); fprintf(fp,"%s", buff);
    fixfloat(buff, jX, 10); fprintf(fp,"%s", buff);
    fixfloat(buff, jY, 10); fprintf(fp,"%s", buff);

	fprintf(fp,"%5d",dummy);
	for(int i=0; i < dummy; i++ ) {
		fixfloat(buff, dX[i], 10); fprintf(fp,"%s", buff);
		fixfloat(buff, dY[i], 10); fprintf(fp,"%s", buff);
	}
	fprintf(fp,"\n");
}
////////////////////////////////////////////////////////////////////////////////
//	CSV
////////////////////////////////////////////////////////////////////////////////
void INTLinkV2::WriteCSV(FILE* fp, int version) {
    if( version == 2 ) {
        fprintf(fp, "\"%-10s\",\"%-10s\",\"%-10s\",", name, sNode, eNode);
    } else {
        fprintf(fp, "%s,%s,%s,", name, sNode, eNode);
    }
    fprintf(fp,"%g,%g,%.0f,%d,", length, Vmax, Capa, QV );
    for(int i=0; i < 10; i++) fprintf(fp, "%g,", fare[i]);
    for(int i=0; i < 10; i++) fprintf(fp, "%c,", ways[i]);
	if(evaluation) fprintf(fp,"%c,0,%c,",linktype,display); else fprintf(fp,"%c,1,%c,",linktype,display);
    if( version == 2 )
        fprintf(fp,"\"%c\",%d,%d,",aFlag1, nFlag2, nFlag3);
    else
        fprintf(fp,"%c,%d,%d,",aFlag1,nFlag2, nFlag3);
    if( version == 2 )
        fprintf(fp,"\"%c%c\",\"%c%c%c\",",aFlag4[0],aFlag4[1],aFlag5[0],aFlag5[1],aFlag5[2]);
    else
        fprintf(fp,"%c%c,%c%c%c,,,,,",aFlag4[0],aFlag4[1],aFlag5[0],aFlag5[1],aFlag5[2]);

    fprintf(fp,"%c,%.7g,%.7g,%.7g,%.7g,%d", color,iX,iY,jX,jY,dummy);
    if ( dummy > 0 ) {
        for(int i=0; i < dummy; i++) {
            fprintf(fp,",%.7g,%.7g", dX[i], dY[i]);
        }
    }

    fprintf(fp,"\n");
}
////////////////////////////////////////////////////////////////////////////////
//	Check direction
////////////////////////////////////////////////////////////////////////////////
bool INTLinkV2::checkway(int m, int j){
	bool ret;
	if( ways[m] == '0' ) ret = true;
	else if(ways[m] == '3') ret = false;
	else if((ways[m] == '2' && j == 0 )||(ways[m] == '1' && j == 1)) ret = true;
	else ret = false;
	return ret;
}

int INTLinkV2::way() {
	int w = 3;
	if( ways[0] == '0') return 0;
	if( ways[0] == '1') w = 1;
	else if (ways[0] == '2') w = 2;

	for(int i=1; i < 10; i++) {
		if( ways[i] == '0' ) return 0;
		if( ways[i] == '3' ) continue;
		if( ways[i] == '1' ) {
			if( w == 3 ) w = 1;
			else if ( w ==2 ) return 0;
		} else {
			if( w == 3 ) w = 2;
			else if ( w == 1 ) return 0;
		}
	}
	return w;
}

StradaINT::StradaINT()
{
	nLink = nNode = 0;
	coordinate = 0; csv = true; version = 4;
}

StradaINT::~StradaINT(){
}
/////////////////////////////////////////////////////////////////////////////
//  Create n_link links
/////////////////////////////////////////////////////////////////////////////
bool StradaINT::alloc_links(int n_link) {
    if( nLink > 0 ) return false;
    if( n_link <= 0 ) return false;
    nLink = n_link;
    try {
		for (int i = 0; i < nLink; i++) {
			LinkPtr link = new INTLinkV2();
			links.push_back(link);
		}
    } catch( std::bad_alloc& ) {
		links.clear();
        return false;
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////
bool StradaINT::Read(FILE* fp)
{
	using namespace std;

	int nrlink;
	int nrnode;

	if( fgets(buff,INT_BUF,fp) == NULL ) {
		errmsg = "INT2 cannot read 1st line of buffer";
		return false;
	}
	if( strncmp(buff, "INT", 3 ) != 0 ) {
		errmsg = "INT2 Header line error";
		return false;
	}
    version = buff[3] - '0' ;
	csv = ( buff[4] == '*' ) ? true : false;

    if( version != 1 && version != 2 && version != 4 ) {
        errmsg = "Version error";
        return false;
    }
    if( version == 4 ) csv = true;

    buff[strlen(buff)-1]=0;

    if( version < 4 ) {
		comment = &buff[5];
    } else {
		comment = &buff[7];
	}
	if( fgets(buff,INT_BUF,fp) == NULL ) {
		errmsg =  "INT2 Cannot read 2nd line of buffer";
		return false;
	}

	try {
		int c = 0;
		if( csv ) {
			for(int i = 0; buff[i] != '\0'; i++) {
				if( buff[i] == ',' ) c++;
				if( c == 2 ) break;
			}
		}
		if(c == 0) {
			nrlink = getbufInt(buff, 0, 5 );
			nrnode = getbufInt(buff, 5, 5 );
			coordinate = getbufInt(buff, 10, 5 );
		} else {
			std::string str = buff;
			boost::tokenizer<boost::escaped_list_separator<char> > tokens(str);
			boost::tokenizer<boost::escaped_list_separator<char> >::iterator it = tokens.begin();
			nrlink = std::stoi(*it); ++it;
			nrnode = std::stoi(*it); ++it;
			coordinate = std::stoi(*it);
		}
	} catch(const std::exception& ) {
		errmsg = "INT2 LINK=" + std::to_string(nrlink) + " NODE=" + std::to_string(nrnode) + " COORDINATE=" + std::to_string(coordinate);
		return false;
	}
	set<string> node_table;
	set<string> link_table;
	rewind(fp);
	fgets(buff, INT_BUF, fp);
	fgets(buff, INT_BUF, fp);
	bool check = true;
	int line_no = 2;
	size_t pos = 0;
	while( fgets(buff, INT_BUF, fp) != NULL ){
		line_no++;
		LinkPtr link = new INTLinkV2();
		if( version == 2 ) {
			if( csv ) {
				if( link->ReadCSV(buff) == false ) {
					errmsg =  "INT2 Error at line " + std::to_string(line_no);
					check = false;
					break;
				}
			} else if( link->Read(buff, pos) == false ) {
				errmsg = "INT2 Error at line " + std::to_string(line_no) + "," + std::to_string(pos);
				check = false;
				break;
			}
		}
		else if ( version == 4 ) {
			if( link->ReadAsV4(buff) == false ) {
				errmsg = "INT4 Error at line " + std::to_string(line_no);
				check = false;
				break;
			}
		} else {
			if( link->ReadAsV1(buff) == false ) {
				errmsg = "INT Error at line " + std::to_string(line_no);
				check = false;
				break;
			}
		}
		links.push_back(link);
		
		if( link_table.find(link->name) != link_table.end()) {
			std::string lname = link->name;
			errmsg = "INT2 Link name duplication " + lname;
			check = false;
			break;
		}
		link_table.insert(link->name);
		node_table.insert(link->sNode);
		node_table.insert(link->eNode);
	}
	if( check == false) {
		links.clear();
		return false;
	}
	nLink = links.size();
	//Remove the link whose name of start and end nodes is the same
	bool b_found_same_node = false;
	std::list<LinkPtr>::iterator it = links.begin();
	while( it != links.end()) {
		LinkPtr link = *it;
		if( strcmp(link->sNode,link->eNode) == 0 ) {
			if( !b_found_same_node ) {
				fprintf(stderr, "The same name for both nodes:\nLINK      NODE\n");
				b_found_same_node = true;
			}
			fprintf(stderr,"%10s: %10s\n", link->name, link->sNode);
			it = links.erase(it);
		} else ++it;
	}
	if (b_found_same_node) {
		nLink = links.size();
	}
	errmsg = "";
	if( nLink != nrlink) {
		errmsg =  "No. of links changed [" + std::to_string(nrlink) + "] -> [" + std::to_string(nLink) + "]\n";
	}
	if( nrnode != (int)node_table.size() ) {
		sprintf_s(buff, sizeof(buff), "No. of nodes changed [%5d] -> [%5zd]\n",nrnode,node_table.size());
		nNode = node_table.size();
		errmsg.append(buff);
	} else nNode = nrnode;

	return true;
}
//----------------------------------------------------------------------------
// Read INT file 
//----------------------------------------------------------------------------
void StradaINT::Read(const char* file_name){
	FILE* fp = NULL;
	errno_t error = fopen_s(&fp, file_name, "rt");
	if(error != 0 || fp == NULL ) {
        std::string str1("Cannot open INT: ");
        std::string fname = file_name;
        throw std::runtime_error(str1 + fname);
	}
	else {
		bool ret = Read(fp);
		fclose(fp);
		if (ret == false) throw std::runtime_error(errmsg);
	}
}

void StradaINT::Write(FILE* fp){

	std::set<std::string> node_table;
	for(const auto& link : links ){
		node_table.insert(link->sNode);
		node_table.insert(link->eNode);
	}
	nNode = node_table.size();
    if( version == 2 ) {
        fprintf(fp,"INT2");
        if ( csv  ) fprintf(fp, "* ");
        else fprintf(fp, "  ");
    }
    else
        fprintf(fp,"INT4*,. ");

    fprintf(fp,"%s\n",comment.c_str());
    if( version == 2 ) {
        if( csv ) fprintf(fp,"%d,%d,%d\n",nLink,nNode,coordinate);
        else fprintf(fp,"%5d%5d%5d\n",nLink,nNode,coordinate);
    } else
        fprintf(fp,"%d,%d,%d\n",nLink,nNode,coordinate);

    if( version == 2 ) {
        if ( csv ) for(const auto& link : links ) link->WriteCSV(fp, 2);
        else for(const auto& link : links) link->Write(fp);
    } else
        for(const auto& link : links ) link->WriteCSV(fp, 4);
//	fprintf(fp, "%c", 0x1A);
}
////////////////////////////////////////////////////////////////////////////////
// Save as
////////////////////////////////////////////////////////////////////////////////
void StradaINT::Write(char* file_name){
	FILE* fp =  NULL;
	errno_t error = fopen_s(&fp, file_name, "wt");
	if(error != 0 || fp == NULL) throw std::runtime_error("INT");
	else {
		Write(fp);
		fclose(fp);
	}
}
// First: i=0
LinkPtr StradaINT::getLink(int i){
	if(	 i < 0	||	i >= nLink ) return nullptr;
	std::list<LinkPtr>::iterator it = std::next(links.begin(),i);
	// next(links.begin(),0) = links.begin() ?
	LinkPtr link = *it;
	return link;
}

void StradaINT::clear(){
	links.clear();
    nLink = 0;
}
////////////////////////////////////////////////////////////////////////////////
// Convert coordinate system
////////////////////////////////////////////////////////////////////////////////
void StradaINT::conv(int cd, int mergin) {
	if( coordinate != 0 && coordinate != 1) return;

	float y_max = 0;
    float y_min = FLT_MAX ;

	for(const auto& link : links ) {
		if( y_max < link->iY ) y_max = link->iY;
		if( y_max < link->jY ) y_max = link->jY;
		if( y_min > link->iY ) y_min = link->iY;
		if( y_min > link->jY ) y_min = link->jY;
	}
	for(auto& link : links) {
		link->iY = y_max - link->iY + y_min + mergin;
		link->jY = y_max - link->jY + y_min + mergin;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Make temporary file from a text file of "X1 Y1 X2 Y2" format
////////////////////////////////////////////////////////////////////////////////
int StradaINT::LinkXYRead(char* f_name) {

	FILE* fp = NULL;
	float x_1, x_2, y_1, y_2;

	if( links.size() > 0 ) return -1;
	errno_t error = fopen_s(&fp, f_name, "rt");
	if (error != 0 || fp == NULL) return (-2);

	nLink = line_cord_check(fp);
	if( nLink < 1) {
		nLink = 0;
		fclose(fp);
		return -3;
	}
	fprintf(stderr, "No. of links read%d\n", nLink);
	std::vector<SNodeV2> nodes(2*nLink);
	for(int i=0; i < nLink ; i++) {
		fgets(buff, 100, fp);
		x_1 = getbufFlt(buff, 0, 10);
		y_1 = getbufFlt(buff,10, 10);
		x_2 = getbufFlt(buff,20, 10);
		y_2 = getbufFlt(buff,30, 10);
		nodes[2*i].X   = x_1;
		nodes[2*i].Y   = y_1;
		nodes[2*i+1].X = x_2;
		nodes[2*i+1].Y = y_2;
	}
	nNode = 0;
	for(int i=0; i < 2*nLink; i++) {
		if( nodes[i].name[0] == 'N' ) continue;
		sprintf_s(nodes[i].name, sizeof(nodes[i].name),"N%05d", i);
		nNode++;
		for(int j=i+1; j < 2*nLink; j++) {
			if(nodes[j].name[0] == 'N' ) continue;
			if( nodes[i].X == nodes[j].X && nodes[i].Y == nodes[j].Y ) {
				sprintf_s(nodes[j].name, sizeof(nodes[j].name),"N%05d", i);
			}
		}
	}
	for(int i=0; i < nLink; i++){
		LinkPtr link = new INTLinkV2();
		sprintf_s(link->name, sizeof(link->name),"L%05d", i);
		strncpy_s(link->sNode, sizeof(link->sNode),nodes[2*i].name,  10);
		strncpy_s(link->eNode, sizeof(link->eNode),nodes[2*i+1].name,10);
		link->iX = nodes[2*i].X;
		link->iY = nodes[2*i].Y;
		link->jX = nodes[2*i+1].X;
		link->jY = nodes[2*i+1].Y;
		links.push_back(link);
	}
	fclose(fp);
	coordinate = 0;
	version = 2;
	return 0;
}
//----------------------------------------------------------------------------
// Sort by name
//----------------------------------------------------------------------------
void StradaINT::sort() {
	links.sort();
}
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//  Get boundary
////////////////////////////////////////////////////////////////////////////////
void INTLinkV2::get_boundary(double &x1, double &y1, double &x2, double &y2)
{
	if( iX < jX ) {
		x1 = iX;
		x2 = jX;
	} else {
		x1 = jX;
		x2 = iX;
	}
	if( iY < jY ) {
		y1 = iY;
		y2 = jY;
	} else {
		y1 = jY;
		y2 = iY;
	}
	for(int i=0; i < dummy; i++) {
		if( x1 > dX[i] ) x1 = dX[i];
		else if( x2 < dX[i] ) x2 = dX[i];
		if( y1 > dY[i] ) y1 = dY[i];
		else if( y2 < dY[i] ) y2 = dY[i];
	}
}

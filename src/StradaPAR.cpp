#include <pch.h>
//---------------------------------------------------------------------------
#include <new>
#include <string.h>
#include <cfloat>
#include <stdexcept>
#include <set>
#include <string>
#include <fstream>
#include <boost/tokenizer.hpp>
//---------------------------------------------------------------------------
#include "StradaCmn.h"
#include "StradaPAR.h"
#include "tool.h"
//---------------------------------------------------------------------------
// extern char strada_error[256];

#define NCHAR 512
//---------------------------------------------------------------------------

str10::str10() {
	memset(name, 0, 11);
}
str10::str10(const str10& obj) {
	strcpy_s(name, 11, obj.name);
}
str10& str10::operator=(const str10& obj) {
	strcpy_s(name, 11, obj.name);
	return *this;
}
CentInfo::CentInfo(){
	memset(name, 0, 11);
	flag = true;
}
CentInfo::CentInfo(const CentInfo& obj) {
	strcpy_s(name, 11, obj.name); flag = obj.flag;
}
CentInfo& CentInfo::operator=(const CentInfo& obj) {
	strcpy_s(name, 11, obj.name); flag = obj.flag;
    return *this;
}

TurnControl::TurnControl(){
	memset(FromNode,0,11);
	memset(	 ToNode,0,11);
	memset(TurnNode,0,11);
	penalty = 0;
}
TurnControl::TurnControl(const char* s, const char* e, const char* n, float f) {
	strcpy_s(FromNode, 11, s); strcpy_s(ToNode, 11, e); strcpy_s(TurnNode, 11, n);
	penalty = f;
}
TurnControl::TurnControl(const TurnControl& obj) {
	strcpy_s(FromNode, 11, obj.FromNode); 
	strcpy_s(ToNode, 11, obj.ToNode); 
	strcpy_s(TurnNode, 11, obj.TurnNode);
	penalty = obj.penalty;
}
TurnControl& TurnControl::operator=(const TurnControl& obj) {
	strcpy_s(FromNode, 11, obj.FromNode);
	strcpy_s(ToNode, 11, obj.ToNode);
	strcpy_s(TurnNode, 11, obj.TurnNode);
	penalty = obj.penalty;
    return *this;
}

LinkNode::LinkNode(){
	memset(sNode, 0, 11);
	memset(eNode, 0, 11);
	memset(name , 0, 11);
}
LinkNode::LinkNode(const char* s, const char* e, const char* n) {
	strcpy_s(sNode, 11, s); strcpy_s(eNode,11, e ); strcpy_s(name, 11, n);
}
LinkNode::LinkNode(const LinkNode& obj) {
	strcpy_s(sNode, 11, obj.sNode); 
	strcpy_s(eNode, 11, obj.eNode); 
	strcpy_s(name, 11, obj.name);
}
LinkNode& LinkNode::operator=(const LinkNode& obj) {
	strcpy_s(sNode, 11, obj.sNode);
	strcpy_s(eNode, 11, obj.eNode);
	strcpy_s(name, 11, obj.name);
    return *this;
}

QV_PARAM::QV_PARAM(){
	v1 = v2 = v3 = v4 = 0;
	q1 = q2 = q3 = q4 = 0;
	code = 0; delay = 0;
}
AssRate::AssRate(){
	flag = true;
	for(int i=0; i  < 10;i++) rate[i]=10;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
StradaPAR::StradaPAR(){
	nLink = nNode = nZone = nMode = 0;
	nTurn = nDirection = nOdDetail = nRouteInf = 0;
	init();
}
void StradaPAR::init(){
	name = "PAR FILE";
	nParam = 0;
	nMode = 1;	//Initial value of mode is 1
	centroids.clear(); nZone = 0;
	Turns.clear(); 	   nTurn = 0;

	od_links.clear();  nOdDetail = 0;
	ri_links.clear();  nRouteInf = 0;
    d_nodes.clear();   nDirection =  0;
	for(int i=0; i < 99; i++) {
		qvdata[i].reset();
	}
	bSearchByMode = false;
	bCountByMode  = false;
	bCalcOdDetail = false;
	bAnalyzeTurn  = false;
	ZoneImpedance = '0';
	ConvertType   = '0';
	LinkCostType  = '0';
	bParam = false;
	bTurnControl = false;
	bRouteInformation = false;
	bTripRank = false;
	bDivideCapacity = false;
	bPreLoad = false;
	bMinRoute = false;
	EquibriumType = '0';
	MaxIteration = 999;
	Error = 0.005f;
	Damp = 1.0;
	for(int i=0; i < 10 ; i++) assign_rate[i] = 10;
	for(int i=0; i< 10; i++){
		time_value[i] = 1.0;
		sp_modify[i] = 1.0;
		APC[i] = 1.0;
		PCU[i] = 1.0;
		rate_by_mode[i] = false;
		for(int j=0; j < 10; j++)
			arate_mode[i][j] = 10;
	}
	base_mode = 1;
	pub_mode = 5;
	trip_range[0] = 5;
	trip_range[1] =10;
	trip_range[2] =15;
	trip_range[3] =20;
	trip_range[4] =25;
	trip_range[5] =30;
}
////////////////////////////////////////////////////////////////////////////////
// Save
////////////////////////////////////////////////////////////////////////////////
void StradaPAR::Write(FILE* fp){

	char buff[256];

	fprintf(fp,"PAR2 %s\n", name.c_str());
	fprintf(fp,"%5d%5d%5d%5d\n",nLink,nNode,nZone,nMode);

	buff[0] = (bSearchByMode) ?  '1' : '0' ;
	buff[1] = (bCountByMode) ? '1' : '0' ;
	buff[2] = (bCalcOdDetail) ? '1' : '0';
	buff[3] = (bAnalyzeTurn) ? '1' : '0';
	buff[4] = ZoneImpedance;
	buff[5] = ConvertType;
	buff[6] = LinkCostType;
	buff[7] = (bParam) ? '1' : '0' ;
	buff[8] = (bTurnControl) ? '1' : '0';
	buff[9] = (bRouteInformation) ? '1' : '0';
	buff[10] = (bTripRank) ? '1' : '0';
	buff[11] = (bDivideCapacity) ? '1' : '0';
	buff[12] = (bPreLoad) ? '1' : '0' ;
	buff[13] = (bMinRoute) ? '1' : '0';
	buff[14] =	EquibriumType;
	buff[15] = '\0';
	fprintf(fp,"%s",buff);
	fprintf(fp,"%3d%7.5f%5.3f    1   25",MaxIteration,Error,Damp);
	for(int i=0; i < 10; i++) fprintf(fp,"%3d",assign_rate[i]);
	for(int i=0; i < 10; i++){
		if(rate_by_mode[i]){
			fprintf(fp,"%2d",i+1);
			for(int j=0; j < 10; j++) fprintf(fp,"%3d",arate_mode[i][j]);
		}
	}
	fprintf(fp,"\n");

	write_od(fp);
	fprintf(fp, "%5d",base_mode);
	for(int i=0; i < 10; i++) {
        fixfloat(buff, time_value[i], 10);
    	fprintf(fp, "%s", buff);
    }
	for(int i=0; i < 10; i++) fprintf(fp, "%5.3f", sp_modify[i]);
	for(int i=0; i < 10; i++) fprintf(fp, "%5.2f%5.2f", PCU[i], APC[i]);
	fprintf(fp, "\n");

	write_bpr(fp);
	write_turn(fp);
	write_direc(fp);
	write_detail(fp);
	write_route(fp);
	write_divparam(fp);
    write_range(fp);
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void StradaPAR::Write(const char* file_name) {
	FILE* fp = NULL;
	errno_t error = fopen_s(&fp, file_name, "wt");
	if(error != 0 || fp == NULL) throw std::runtime_error("PAR");
	else {
		Write(fp);
		fclose(fp);
	}
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void StradaPAR::write_od(FILE* fp){
	int n = nZone / 10;
	int m = nZone % 10;
	for(int i=0; i < n; i++){
		for(int j=0; j < 10; j++){
			int idx = 10 * i + j;
			fprintf(fp, "%-10s", centroids[idx].name);
			if(centroids[idx].flag) fprintf(fp, "* ");
			else fprintf(fp, "  ");
		}
		fprintf(fp, "\n" );
	}
	if( m > 0 ) {
		for(int i=0; i < m; i++){
			int idx = 10 * n + i;
			fprintf(fp, "%-10s", centroids[idx].name);
			if(centroids[idx].flag) fprintf(fp, "* "); else fprintf(fp, "  ");
		}
		fprintf(fp,"\n");
	}
}
////////////////////////////////////////////////////////////////////////////////
//	QV, BPR function parameters
////////////////////////////////////////////////////////////////////////////////
void StradaPAR::write_bpr(FILE* fp){

	if(bParam ){
		int count = 0;
		for(int i=0; i < 99; i++) {
			if( qvdata[i] ) {
				if(count == 0) fprintf(fp,"A%4d%5d", nParam,i+1);
				else fprintf(fp,"A    %5d",i+1);
				fprintf(fp,"%7.4g%7.4g%7.4g%7.4g%7.4g%7.4g%7.4g%7.4g%10.4g\n",
					qvdata[i]->v1,qvdata[i]->q1,qvdata[i]->v2,qvdata[i]->q2,
					qvdata[i]->v3,qvdata[i]->q3,qvdata[i]->v4,qvdata[i]->q4,
					qvdata[i]->delay);
				count++;
			}
		}
	}
}
//---------------------------------------------------------------------------
// Direction penalty
//---------------------------------------------------------------------------
void StradaPAR::write_turn(FILE* fp){
	int i = 0;
	for(const auto& turn : Turns ){
		if( i == 0 ) {
			fprintf(fp, "B%4d%-10s%-10s%-10s%10.4g\n",nTurn,
				turn.FromNode, turn.TurnNode, turn.ToNode, turn.penalty);
				i++;
		} else {
			fprintf(fp, "B    %-10s%-10s%-10s%10.4g\n",
				turn.FromNode, turn.TurnNode, turn.ToNode, turn.penalty);
		}
	}
}
void StradaPAR::write_direc(FILE* fp){

	int n,m;
	int mx = nDirection;
	if( nDirection > 0 ){
		if( nDirection > 9999 ) {
			mx = 9999 ;
		}
		n = mx / 15 ;
		m = mx % 15 ;
		std::list<str10>::const_iterator it = d_nodes.cbegin();
		for(int i=0; i < n; i++){
			if(i==0) fprintf(fp, "C%4d",mx); else fprintf(fp,"C    ");
			for(int j=0; j < 15; j++) {
				fprintf(fp,"%-10s",it->name);
				++it;
			}
			fprintf(fp, "\n");
		}
		if( m > 0 ){
			if(n == 0) fprintf(fp, "C%4d",mx); else fprintf(fp,"C    ");
			for(int i=0; i < m; i++){
				fprintf(fp,"%-10s", it->name);
				++it;
			}
			fprintf(fp, "\n" );
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
//  Record of OD details
////////////////////////////////////////////////////////////////////////////////
void StradaPAR::write_detail(FILE* fp){

	int i = 0;
	for(const auto& x: od_links){
		if(i==0) fprintf(fp,"D%4d",nOdDetail);
		else fprintf(fp,"D    ");
		fprintf(fp,"%-10s%-10s%-10s\n",	x.name, x.sNode, x.eNode);
		i++;
		if(i >= 9999) break;
	}
}
//---------------------------------------------------------------------------
// Routes
//---------------------------------------------------------------------------
void StradaPAR::write_route(FILE* fp){

	int i=0;
	for(const auto& ri : ri_links){
		if(i==0) fprintf(fp,"E%4d",nRouteInf); else fprintf(fp,"E    ");
		fprintf(fp,"%-10s%-10s%-10s\n", ri.name, ri.sNode, ri.eNode);
		i++;
		if(i >= 9999) break;
	}
}
//---------------------------------------------------------------------------
//  F: Conversion parameters
//---------------------------------------------------------------------------
void StradaPAR::write_divparam(FILE* fp){

	if( ConvertType == '1' ){
		for(int i=0; i < nMode; i++) {
			fprintf(fp,"F%4d%10.5f%10.5f%10.5f%10.5f%10.5f\n",
				i+1,jhpc[i].a, jhpc[i].b, jhpc[i].c, jhpc[i].K, jhpc[i].S);
		}
	} else if( ConvertType == '2' ){
		fprintf(fp,"F%4d%10.4g%10.4g%10.4g\n",pub_mode, pubp[pub_mode-1].a, pubp[pub_mode-1].b, pubp[pub_mode-1].c);
	}

}
////////////////////////////////////////////////////////////////////////////////
//  G: Distribution of trip length
////////////////////////////////////////////////////////////////////////////////
void StradaPAR::write_range(FILE* fp){
	if( bTripRank ) {
		fprintf(fp,"G    ");
		for(int i=0; i < 5; i++) fprintf(fp, "%5.1f", trip_range[i]);
		fprintf(fp,"\n");
	}
}
////////////////////////////////////////////////////////////////////////////////
// CSV: 
////////////////////////////////////////////////////////////////////////////////
bool StradaPAR::ReadV4(FILE* fp, int& line_number) {

	char buf[NCHAR];
	char* pdata[256];

	if(fgets(buf,NCHAR,fp)==NULL) return false;
	line_number++;
	csv_parser(buf, pdata, 4, ',', '.');
	for(int i=0; i < 4; i++) if( pdata[i] == NULL ) return false;

	nLink = atoi(pdata[0]);
	nNode = atoi(pdata[1]);
	nZone = atoi(pdata[2]);
	nMode = atoi(pdata[3]);

	if(fgets(buf, NCHAR,fp)==NULL) return false;
	line_number++;
	csv_parser(buf, pdata, 130, ',', '.');
	for(int i=0; i < 20; i++) if( pdata[i] == NULL ) return false;

	bSearchByMode = (pdata[0][0]=='1') ? true : false ;
	bCountByMode  = (pdata[1][0]=='1') ? true : false ;
	bCalcOdDetail = (pdata[2][0]=='1') ? true : false ;
	bAnalyzeTurn  = (pdata[3][0]=='1') ? true : false ;
	ZoneImpedance = pdata[4][0];
	ConvertType   = pdata[5][0];
	LinkCostType  = pdata[6][0];
	bParam		  = (pdata[7][0]=='1') ? true : false ;
	bTurnControl  = (pdata[8][0]=='1') ? true : false ;
	bRouteInformation = (pdata[9][0]=='1') ? true : false ;
	bTripRank	  = (pdata[10][0]=='1') ? true : false ;
	bDivideCapacity  = (pdata[11][0]=='1') ? true : false ;
	bPreLoad  	  = (pdata[12][0]=='1') ? true : false ;
	bMinRoute	  = (pdata[13][0]=='1') ? true : false ;
	EquibriumType = pdata[14][0];
	MaxIteration = atoi(pdata[15]); if(MaxIteration == 0 ) MaxIteration = 999;
	Error		 = (float)atof(pdata[16]); if(Error == 0) Error = 0.005f;
	Damp		 = (float)atof(pdata[17]); if(Damp  == 0) Damp  = 1.0;
	unit_hours   = atoi(pdata[18]);
	time_units   = atoi(pdata[19]);

	if ( EquibriumType == '0' ) {
		for(int i=0; i < 10; i++) assign_rate[i] = atoi(pdata[20+i]);
		for(int i=0; i < 9; i++) {
			if( pdata[30+11*i] == NULL ) break;
			int mode = atoi(pdata[30+11*i]);
			if( mode < 1 || mode > 10 ) return false;
			rate_by_mode[mode-1] = true;
			for(int j=0; j < 10; j++ ) {
				if (pdata[31+11*i+j] == NULL ) return false;
				arate_mode[mode-1][j] = atoi(pdata[31+11*i+j]);
			}
		}
	}
	centroids.clear();
	centroids.resize(nZone);

	read_odv4(fp, line_number);

	if(fgets(buf, NCHAR,fp)==NULL) return false;
	line_number++;
	csv_parser(buf, pdata, 42, ',', '.');
	for(int i=0; i < 41; i++ ) if ( pdata[i]==NULL ) return false;

	base_mode = atoi(pdata[0]);
	for(int i=0; i < 10; i++) {
		float d = (float)atof(pdata[1+i]);	//MAXFLOAT
		if( d == 1e10 ) {
			time_value[i] = FLT_MAX ;
		}
		else time_value[i] = d ;

		sp_modify[i]  = (float)atof(pdata[11+i]);
		PCU[i] = (float)atof(pdata[21+2*i]);
		APC[i] = (float)atof(pdata[21+2*i+1]);

	}

	return true;
}
////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
int StradaPAR::Read(FILE* fp){

	int ret = 1;
	int c, mode;
	char buf[NCHAR];
	int opt[8];
	long loc[8];

	if(fgets(buf, NCHAR,fp)==NULL) return (ret);
	buf[strlen(buf)-1]='\0';
	if( strlen(buf) < 4 ) return (ret);
	if( strncmp(buf, "PAR", 3) != 0 ) return (ret);
	version = buf[3];
	if (strlen(buf) > 6) name = &buf[5];
	ret++;

	if( version == '2' ) {
		if(fgets(buf,NCHAR,fp)==NULL) return(ret);

		nLink = getbufInt(buf, 0,5);	//printf( "LINK:%5d\n", nLink);
		nNode = getbufInt(buf, 5,5);	//printf( "NODE:%5d\n", nNode);
		nZone = getbufInt(buf,10,5);	//printf( "ZONE:%5d\n", FnZone);
		nMode = getbufInt(buf,15,5);	//printf( "MODE:%5d\n", nMode);

		ret++;
		if(fgets(buf,NCHAR,fp)==NULL) return(ret);
		buf[strlen(buf)-1] = '\0';
		size_t buf_length = strlen(buf);
		if( buf_length < 40 ) return(ret);

		bSearchByMode = (buf[0]=='1') ? true : false ;
		bCountByMode  = (buf[1]=='1') ? true : false ;
		bCalcOdDetail = (buf[2]=='1') ? true : false ;
		bAnalyzeTurn  = (buf[3]=='1') ? true : false ;
		ZoneImpedance = buf[4];
		ConvertType   = buf[5];
		LinkCostType  = buf[6];
		bParam		  = (buf[7]=='1') ? true : false ;
		bTurnControl  = (buf[8]=='1') ? true : false ;
		bRouteInformation = (buf[9]=='1') ? true : false ;
		bTripRank		 = (buf[10]=='1') ? true : false ;
		bDivideCapacity  = (buf[11]=='1') ? true : false ;
		bPreLoad  	  = (buf[12]=='1') ? true : false ;
		bMinRoute		 = (buf[13]=='1') ? true : false ;
		EquibriumType	 = buf[14];
		MaxIteration = getbufInt(buf,15,3); if(MaxIteration == 0 ) MaxIteration = 999;
		Error		 = getbufFlt(buf,18,7); if(Error == 0) Error = 0.005f;
		Damp		 = getbufFlt(buf,25,5); if(Damp  == 0) Damp  = 1.0;
		unit_hours   = getbufInt(buf,30,5);
		time_units   = getbufInt(buf,35,5);
		if( buf_length > 40 ) {
			for(int i=0; i < 10; i++)
				assign_rate[i] = getbufInt(buf,40+3*i,3);
			if( buf_length > 70 ) {
				c = (buf_length - 70) / 32;
				for(int i=0; i < c; i++) {
					mode = getbufInt(buf, 102+32*i, 2);
					if( mode > 0 && mode <= 10 ) {
						rate_by_mode[mode] = true;
						for(int j=0; j < 10; j++ ) {
							arate_mode[mode][j] = getbufInt(buf, 105+32*i,3);
						}
					}
				}
			}
		}
		centroids.clear();
		if( nZone > 0 ) {
			centroids.resize(nZone);
			if( read_od(fp,ret) == -1) {
				init();
				return (ret);
			}
		}
		ret++;
		if(fgets(buf,NCHAR,fp)==NULL) {
			init();
			return(ret);
		}
		base_mode = getbufInt(buf,0,5);
		for(int i=0; i < 10; i++) {
			float d = getbufFlt(buf,5+10*i,10);	//MAXFLOAT
			if( d == 1e10 ) {
				time_value[i] = FLT_MAX ;
			}
			else time_value[i] = d ;
			sp_modify[i]  = getbufFlt(buf,105+5*i,5);
			PCU[i] = getbufFlt(buf,155+10*i,5);
			APC[i] = getbufFlt(buf,160+10*i,5);
		}
	} else {
		ReadV4(fp, ret);
	}
	// Check A..H
	for(int i=0; i < 8; i++)  opt[i] = 0;
	long cur_pos;
	int cur_line = ret;
	int rm ;
	bool (StradaPAR::*pFunc[])(FILE*, int, int&) = {
		&StradaPAR::read_param,
		&StradaPAR::read_turn,
		&StradaPAR::read_direction,
		&StradaPAR::read_detail,
		&StradaPAR::read_route,
		&StradaPAR::read_divparam,
		&StradaPAR::read_range
	};
	cur_pos = ftell(fp);
	while (fgets(buf,NCHAR,fp) ) {
		cur_line++;
		rm = buf[0] - 'A'; //
		if( rm < 0 || rm > 7 ) return (cur_line);
		if( opt[rm] == 0 ) loc[rm] = cur_pos;
		opt[rm]++;
		cur_pos = ftell(fp);
	}
	fseek(fp, cur_pos, SEEK_SET);

	for(int i=0; i < 7; i++) {
		if( opt[i] > 0 ) {
			fseek(fp, loc[i], SEEK_SET);
			if( (this->*pFunc[i])(fp, opt[i], ret) == false ) return (ret);
		}
	}
	return(0);
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
bool StradaPAR::read_odv4(FILE* fp, int& line_number) {
	char buf[NCHAR];
	char* pdata[256];
	int n = nZone / 10;
	int m = nZone % 10;
	for(int i=0; i < n; i++ ) {
		if(fgets(buf, NCHAR,fp)==NULL) return false;
		line_number++;
		csv_parser(buf, pdata, 10, ',', '.');
		for(int j=0; j < 10; j++) {
			int idx = 10 * i + j;
			strncpy_s( centroids[idx].name, 11, pdata[j], 10 );
			centroids[idx].name[10] = '\0';
		}
	}
	if ( m > 0 ) {
		if(fgets(buf, NCHAR,fp)==NULL) return false;
		line_number++;
		csv_parser(buf, pdata, m, ',', '.');
		for(int j=0; j < 10; j++) {
			int idx = 10 * n + j;
			strncpy_s( centroids[idx].name, 11, pdata[j], 10 );
			centroids[idx].name[10] = '\0';
		}
	}
	line_number++;
	if(fgets(buf, NCHAR,fp)==NULL) return false;
	std::string line = buf;
	boost::tokenizer<boost::escaped_list_separator<char> > tokens(line);
	boost::tokenizer<boost::escaped_list_separator<char> >::iterator it = tokens.begin();
	if (it == tokens.end()) return false;// Z
	++it; if (it == tokens.end()) return false; else n = std::stoi(*it);
	for(int i=0; i < n; i++) {
		int k;
		++it; if (it == tokens.end()) return false; else k = std::stoi(*it);
		k--;
		if( k < 0 || k > nZone-1 ) return false;
		centroids[k].flag = true;
	}

	return true;
}
////////////////////////////////////////////////////////////////////////////////
//	OD Data
////////////////////////////////////////////////////////////////////////////////
int StradaPAR::read_od(FILE* fp, int &line_number){

	using namespace std;

	char buf[256];
	size_t n, m ;
	size_t Z = static_cast<size_t>(nZone);
	n = Z / 10;
	m = Z % 10;

	for(size_t i=0; i<n ; i++){
		line_number++;
		if(fgets(buf,256,fp)==NULL) return(-1);
		buf[strlen(buf) - 1] = '\0';
		std::string line = buf;
		for(size_t j =0;j < 10;j++){
			std::string str = trim(line.substr(12 * j, 10));
			if (str == "") return (-1);
			strcpy_s(centroids[10 * i + j].name, 11, str.c_str());
			if(line[12*j+10] == '*') centroids[10*i+j].flag = true;
			else centroids[10*i+j].flag = false;
		}
	}
	if( m > 0 ) {
		line_number++;
		if(fgets(buf,256,fp)==NULL) return(-1);
		buf[strlen(buf) - 1] = '\0';
		std::string line = buf;
		for(size_t i = 0; i < m; i++){
			std::string str = trim(line.substr(12 * i, 10));
			if (str == "") return (-1);
			strcpy_s(centroids[10 * n + i].name, 11, str.c_str());
			if(line[12*i+10] == '*') centroids[10*n+i].flag = true;
			else centroids[10*n+i].flag = false;
		}
	}
	// Check duplication
	set<string> zone_list;
	for(int i=0; i < nZone; i++ ) {
		if( zone_list.find(centroids[i].name) != zone_list.end() ) {
			printf("Duplicate centroid name: %s\n", centroids[i].name);
			return (-1);
		}
		zone_list.insert(centroids[i].name);
	}
	return(1);
}

////////////////////////////////////////////////////////////////////////////////
// change node name
// return: No. of nodes which were renamed
////////////////////////////////////////////////////////////////////////////////
int StradaPAR::rename_nodes(const char* curname, const char* newname) {
	int count = 0;
	for(int i = 0 ; i < nZone; i++) {
		if ( strncmp(centroids[i].name, curname, 10 ) == 0 ) {
			strncpy_s(centroids[i].name, sizeof(centroids[i].name), newname, 10);
			count++;
		}
	}
	for(auto& turn : Turns) {
		if ( strncmp(turn.FromNode, curname, 10 ) == 0 ) {
			strncpy_s(turn.FromNode, sizeof(turn.FromNode), newname, 10);
			count++;
		}
		if ( strncmp(turn.ToNode, curname, 10 ) == 0 ) {
			strncpy_s(turn.ToNode, sizeof(turn.ToNode), newname, 10);
			count++;
		}
		if ( strncmp(turn.TurnNode, curname, 10 ) == 0 ) {
			strncpy_s(turn.TurnNode, sizeof(turn.TurnNode), newname, 10);
			count++;
		}
	}
	for(auto& dn : d_nodes ) {
		if ( strncmp(dn.name, curname, 10 ) == 0 ) {
			strncpy_s(dn.name, sizeof(dn.name), newname, 10);
			count++;
		}
	}
	for(auto& x : od_links ) {
		if ( strncmp(x.sNode, curname, 10 ) == 0 ) {
			strncpy_s(x.sNode, sizeof(x.sNode), newname, 10);
			count++;
		}
		if ( strncmp(x.eNode, curname, 10 ) == 0 ) {
			strncpy_s(x.eNode, sizeof(x.eNode), newname, 10);
			count++;
		}
	}
	for(auto& ri : ri_links) {
		if ( strncmp(ri.sNode, curname, 10 ) == 0 ) {
			strncpy_s(ri.sNode, sizeof(ri.sNode), newname, 10);
			count++;
		}
		if ( strncmp(ri.eNode, curname, 10 ) == 0 ) {
			strncpy_s(ri.eNode, sizeof(ri.eNode), newname, 10);
			count++;
		}
	}
	return count;
}

////////////////////////////////////////////////////////////////////////////////
//	A: Parameters of volume-speed relationship
////////////////////////////////////////////////////////////////////////////////
bool StradaPAR::read_param(FILE* fp,int c, int &line_number)
{
	char buf[100];
	char* pdata[256];
	int code;
	size_t len;
	float v1, v2, v3, v4, q1, q2, q3, q4, delay;
	line_number++;
	try {

		if(fgets(buf,100,fp)==NULL) throw(1);
		if( version == '4' ) {
			csv_parser(buf, pdata, 2, ',', '.');
			if( pdata[0] == NULL || pdata[0][0] != 'A' || pdata[1] == NULL ) throw(2);
			nParam = atoi(pdata[1]);
			if(fgets(buf,100,fp)==NULL) throw(3);
		} else {
			if( buf[0] != 'A') throw(4);
			buf[strlen(buf)-1] = '\0';
			len = strlen(buf);
			nParam = getbufInt(buf,1,4);
		}
		if( nParam <= 0 || nParam > 99 ) throw(5);
		for(int i=0; i < 99; i++) qvdata[i].reset();

		for(int i=0; i < nParam;i++){
			delay = 0;
			v1 = v2 = v3 = v4 = q1 = q2 = q3 = q4 = 0;
			if( version == '4' ) {
				csv_parser(buf, pdata, 11, ',', '.');
				for(int j=0; j < 11; j++ ) if( pdata[j] == NULL ) throw(0);
				if ( pdata[0][0] != 'A' ) throw(0);
				code = atoi(pdata[1]);
				v1 = (float)atof(pdata[2]);
				q1 = (float)atof(pdata[3]);
				v2 = (float)atof(pdata[4]);
				q2 = (float)atof(pdata[5]);
				v3 = (float)atof(pdata[6]);
				q3 = (float)atof(pdata[7]);
				v4 = (float)atof(pdata[8]);
				q4 = (float)atof(pdata[9]);
				delay = (float)atof(pdata[10]);
			} else {
				code = getbufInt(buf, 5, 5) - 1;
				if( code < 0 || code >= 99 ) throw(0);
				v1 = getbufFlt(buf, 10, 7);
				q1 = getbufFlt(buf, 17, 7);
				if( len == 76 ) {
					v2 = getbufFlt(buf, 24, 7);
					q2 = getbufFlt(buf, 31, 7);
					v3 = getbufFlt(buf, 38, 7);
					q3 = getbufFlt(buf, 45, 7);
					v4 = getbufFlt(buf, 52, 7);
					q4 = getbufFlt(buf, 59, 7);
					delay = getbufFlt(buf, 66, 10);
				}
			}
			line_number++;
			qvdata[code].reset(new QV_PARAM());
			qvdata[code]->v1 = v1; qvdata[code]->v2 = v2; qvdata[code]->v3 = v3; qvdata[code]->v4 = v4;
			qvdata[code]->q1 = q1; qvdata[code]->q2 = q2; qvdata[code]->q3 = q3; qvdata[code]->q4 = q4;
			qvdata[code]->delay = delay;
			if( i == nParam - 1 ) break;
			if(fgets(buf,100,fp)==NULL) {
				nParam = 0;
				throw(0);
			}
		}
	} catch (int &e) {
		for(int i=0; i < 99; i++) {
			qvdata[i].reset();
		}
		nParam = 0;
		sprintf_s(strada_error, sizeof(strada_error), "A:%d", e);
		return false;
	} catch ( std::runtime_error& ) {
		for(int i=0; i < 99; i++) {
			qvdata[i].reset();
		}
		nParam = 0;
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
//  B:Turn
//---------------------------------------------------------------------------
bool StradaPAR::read_turn(FILE* fp,int c, int& line_number)
{
	char buf[100];
	char* pdata[256];

	line_number++;
	try {
		if(fgets(buf,100,fp)==NULL) throw(0);
		if( buf[0] != 'B' ) throw(0);
		if( version == '4' ) {
			csv_parser(buf, pdata, 2, ',', '.');
			if ( pdata[1] == NULL ) throw(0);
			nTurn = atoi(pdata[1]);

		} else {
			nTurn = getbufInt( buf, 1, 4);
		}
		if( nTurn <= 0 ) throw(0);
		Turns.clear();
///		Turns.resize(nTurn);  //list
		for(int i=0; i < nTurn ; i++){
			TurnControl turn;
			if( version == 4 ) {
				if(fgets(buf,100,fp)==NULL) throw(0);
				csv_parser(buf, pdata, 5, ',','.');
				for(int j=0; j < 5; j++) if( pdata[j] == NULL ) throw(1);
				if( pdata[0][0] != 'B' ) throw(1);
				for(int j=0; j < 3; j++) {
					trim( pdata[1+j] );
				}
				strncpy_s( turn.FromNode, sizeof(turn.FromNode), pdata[1], 10); turn.FromNode[10] = 0;
				strncpy_s( turn.TurnNode, sizeof(turn.TurnNode), pdata[2], 10); turn.TurnNode[10] = 0;
				strncpy_s( turn.ToNode, sizeof(turn.ToNode), pdata[3], 10); turn.ToNode[10] = 0;

				turn.penalty = (float) atof(pdata[4]);

			} else {
				if( i > 0 ){
					line_number++;
					if( fgets(buf, 100, fp) == NULL ) throw(1);
					if(buf[0] != 'B') throw(1);
				}
				buf[strlen(buf) - 1] = '\0';
				std::string line = buf;
				std::string str = trim(line.substr(5, 10));
				strcpy_s( turn.FromNode, 10, str.c_str());
				str = trim(line.substr(15, 10));
				strcpy_s( turn.TurnNode, 10, str.c_str());
				str = trim(line.substr(25, 10));
				strcpy_s( turn.ToNode, 10, str.c_str());
				turn.penalty = std::stof(line.substr(35,10));
			}
			Turns.push_back(turn);
		}
	} catch(int &e) {
		Turns.clear();
		nTurn = 0;
		sprintf_s(strada_error, sizeof(strada_error), "B:%d", e);
		return false;
	}
	return true;
}
/////////////////////////////////////////////////////////////////////////////////
//  C: List of nodes where directional traffic is recorded
/////////////////////////////////////////////////////////////////////////////////
bool StradaPAR::read_direction(FILE* fp,int c,  int &line_number)
{

	char buf[256];
	char* pdata[256];

	char* ptr;
	int m, n ;

	line_number++;
	try {
		if(fgets(buf,256,fp)==NULL) throw(0);

		if( version == '4' ) {
			csv_parser( buf, pdata, 2, ',', '.');
			if( pdata[0] == NULL || pdata[0][0] != 'C' || pdata[1] == NULL ) throw(1);
			nDirection = atoi(pdata[1]);
		} else {
			if( buf[0] != 'C' ) throw(2);
			nDirection = getbufInt( buf, 1, 4);
		}
		if( nDirection <= 0 ) throw(3);
		d_nodes.clear();

		m = nDirection / 15;
		n = nDirection - m * 15 ;
		if( version == '4' ) {
			for(int j =0; j < m; j++ ) {
				if(fgets(buf,256,fp)==NULL) throw(5);
				line_number++;
				csv_parser( buf, pdata, 16, ',', '.');
				for(int k=0; k < 16; k++) if (pdata[k] == NULL ) throw(6);
				if( pdata[0][0] != 'C' ) throw(7);
				for(int k=0; k < 15; k++) {
					trim(pdata[1+k]);
					str10 dn;
					strncpy_s(dn.name, sizeof(dn.name), pdata[1+k], 10); dn.name[10] = '\0';
					d_nodes.push_back(dn);
				}
			}
			if ( n > 0 ) {
				if(fgets(buf,256,fp)==NULL) throw(8);
				line_number++;
				csv_parser( buf, pdata, n+1, ',', '.');
				for(int k=0; k < n+1; k++) if (pdata[k] == NULL ) throw(9);
				for(int j=0; j < n; j++) {
				//printf("%s\n", pdata[1+j]);
					trim(pdata[1+j]);
					str10 dn;
					strncpy_s(dn.name, sizeof(dn.name), pdata[1+j], 10); dn.name[10] = '\0';
					d_nodes.push_back(dn);
				}
			}
		} else {
			for(int j =0; j < m; j++ ) {
				for(int k=0; k < 15; k++) {
					str10 dn;
					strncpy_s(dn.name, 11, &buf[5+10*k], 10);
					dn.name[10] = '\0';
					trim(dn.name);
					d_nodes.push_back(dn);
					//printf("%d %d %s\n", j, k, d_nodes[15*j+k].name);
				}
				line_number++;
				ptr = fgets(buf, 256, fp);
				if( ptr == NULL ) throw(10);
				if(buf[0] != 'C') throw(11);
			}
			for(int j=0; j < n; j++) {
				str10 dn;
				strncpy_s(dn.name, sizeof(dn.name), &buf[5+10*j], 10);
				dn.name[10] = '\0';
				trim(dn.name);
				d_nodes.push_back(dn);
			}
		}
	} catch(int& e) {
		d_nodes.clear();
		nDirection = 0;
		sprintf_s(strada_error, sizeof(strada_error), "C:%d", e);
		return false;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//  D: OD Detail
////////////////////////////////////////////////////////////////////////////////
bool StradaPAR::read_detail(FILE* fp, int c, int &line_number)
{

	char buf[256];
	char* pdata[256];

	line_number++;
	try {
		if(fgets(buf,256,fp)==NULL) throw(1);
		if( version == '4' ) {
			csv_parser( buf, pdata, 2, ',', '.');
			if( pdata[0] == NULL || pdata[0][0] != 'D' || pdata[1] == NULL ) throw(2);
			nOdDetail = atoi( pdata[1] );
		} else {
			if( buf[0] != 'D' ) throw(3);
			nOdDetail = getbufInt( buf, 1, 4);
		}
		if( nOdDetail <= 0 ) throw(4);
		od_links.clear();
		for(int j =0; j < nOdDetail; j++ ) {
			LinkNode odlink;
			if( version == '4' ) {
				if(fgets(buf,256,fp)==NULL) throw(6);
				line_number++;
				csv_parser( buf, pdata, 4, ',', '.');
				for(int k=0; k < 4; k++) if (pdata[k] == NULL ) throw(7);
				for(int k=1; k < 4; k++) trim(pdata[k]);
				strncpy_s(odlink.name, sizeof(odlink.name), pdata[1], 10) ; odlink.name[10] = '\0';
				strncpy_s(odlink.sNode, sizeof(odlink.sNode), pdata[2], 10) ; odlink.sNode[10] = '\0';
				strncpy_s(odlink.eNode, sizeof(odlink.eNode), pdata[3], 10) ; odlink.eNode[10] = '\0';
			} else {
				strncpy_s(odlink.name, 11, &buf[5], 10); odlink.name[10] = '\0';
				trim(odlink.name);
				strncpy_s(odlink.sNode, 11, &buf[15], 10); odlink.sNode[10] = '\0';
				trim(odlink.sNode);
				strncpy_s(odlink.eNode, 11, &buf[25], 10); odlink.eNode[10] = '\0';
				trim(odlink.eNode);
				if( j != nOdDetail -1 ){
					if(fgets(buf,256,fp)==NULL || buf[0] != 'D' ) {
						od_links.clear();
						nOdDetail = 0 ;
						throw(8);
					}
					line_number++;
				}
			}
			od_links.push_back(odlink);
		}
	} catch(int& e) {
		od_links.clear();
		nOdDetail = 0;
		sprintf_s(strada_error, sizeof(strada_error), "D:%d", e);
		return false;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//  E: Route information
////////////////////////////////////////////////////////////////////////////////
bool StradaPAR::read_route(FILE* fp,int c,  int &line_number){

	char buf[256];
	char* pdata[256];
	int n;

	line_number++;
	try {
		if(fgets(buf,256,fp)==NULL) throw(1);
		if( version == '4' ) {
			csv_parser( buf, pdata, 2, ',', '.');
			if( pdata[0] == NULL || pdata[0][0] != 'E' || pdata[1] == NULL ) throw(2);
			n = atoi( pdata[1] ) ;
		} else {
			if( buf[0] != 'E' ) throw(3);
			n = getbufInt( buf, 1, 4);
		}
		if( n <= 0 ) throw(4);
		nRouteInf = n;
		ri_links.clear();
		for(int j =0; j < nRouteInf; j++ ) {
			LinkNode ri;
			if( version == '4' ) {
				if(fgets(buf,256,fp)==NULL) throw(6);
				line_number++;
				csv_parser( buf, pdata, 4, ',', '.');
				for(int k=0; k < 4; k++) if (pdata[k] == NULL ) throw(7);
				for(int k=1; k < 4; k++) trim(pdata[k]);
				strncpy_s(ri.name, sizeof(ri.name), pdata[1], 10) ; ri.name[10] = '\0';
				strncpy_s(ri.sNode, sizeof(ri.sNode), pdata[2], 10) ; ri.sNode[10] = '\0';
				strncpy_s(ri.eNode, sizeof(ri.eNode), pdata[3], 10) ; ri.eNode[10] = '\0';

			} else {
				strncpy_s(ri.name, 11, &buf[5], 10); ri.name[10] = '\0';
				trim(ri.name);
				strncpy_s(ri.sNode, 11, &buf[15], 10); ri.sNode[10] = '\0';
				trim(ri.sNode);
				strncpy_s(ri.eNode, 11, &buf[25], 10); ri.eNode[10] = '\0';
				trim(ri.eNode);
				if( j != nRouteInf -1 ){
					if(fgets(buf,256,fp)==NULL || buf[0] != 'E' ) {
						ri_links.clear();
						nRouteInf = 0 ;
						throw(7);
					}
				}
			}
		}
	} catch(int& e ) {
		ri_links.clear();
		nRouteInf = 0;
		sprintf_s(strada_error, sizeof(strada_error), "E:%d", e);
		return false;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//  F: Conversion parameter
////////////////////////////////////////////////////////////////////////////////
bool StradaPAR::read_divparam(FILE* fp,int c,  int &line_number){

	char buf[256];
	char* pdata[256];
	int m ;
	size_t len;
	float ra[5];
	try {
		for(int i=0; i < c; i++) {
			line_number++;
			if(fgets(buf,256,fp)==NULL) throw(1);
			if( version == '4' ) {
				csv_parser( buf, pdata, 7, ',', '.');
				if( pdata[0] == NULL || pdata[0][0] != 'F' || pdata[1] == NULL ) throw(2);
				m = atoi( pdata[1] ) ;
				if( m <= 0 || m > 10) throw(3);
				for(int j=2; j < 7; j++ ) if ( pdata[j] == NULL ) throw(3);
				for(int j=0; j < 5; j++) ra[j] = (float) atof( pdata[2+j] );
				if( ConvertType == '1' ) {
					jhpc[m-1].mode = m ;
					jhpc[m-1].a = ra[0];
					jhpc[m-1].b = ra[1];
					jhpc[m-1].c = ra[2];
					jhpc[m-1].K = ra[3];
					jhpc[m-1].S = ra[4];
				} else if ( ConvertType == '2' ) {
					pub_mode = m;
					pubp[m-1].a = ra[0];
					pubp[m-1].b = ra[1];
					pubp[m-1].c = ra[2];
					pubp[m-1].mode = true;
				}
			} else {
				if( buf[0] != 'F' ) throw(5);
				len = strlen(buf);
				buf[len-1]=0;
				len--;
				if( len >= 35 ) {
					m = getbufInt(buf, 1, 4 );
					if( i == 0 ) pub_mode = m;
					pubp[m-1].a = getbufFlt(buf, 5, 10);
					pubp[m-1].b = getbufFlt(buf,15, 10);
					pubp[m-1].c = getbufFlt(buf,25, 10);
					pubp[m-1].mode = true;
				}
				if( len == 55 ) {
					if ( m > 0 && m <= 10 ) {
						jhpc[m-1].mode = m ;
						jhpc[m-1].a = getbufFlt(buf, 5, 10);
						jhpc[m-1].b = getbufFlt(buf,15, 10);
						jhpc[m-1].c = getbufFlt(buf,25, 10);
						jhpc[m-1].K = getbufFlt(buf,35, 10);
						jhpc[m-1].S = getbufFlt(buf,45, 10);
					}
				}
			}
		}
	} catch (int &e ) {
		sprintf_s(strada_error, sizeof(strada_error),"F:%d", e);
		return false;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//  G: Distribution of trip length
////////////////////////////////////////////////////////////////////////////////
bool StradaPAR::read_range(FILE* fp,int c,  int &line_number){

	char buf[256];
	char* pdata[256];

	line_number++;
	try {
		if(fgets(buf,256,fp)==NULL) throw(1);
		if ( version == '4' ) {
			csv_parser( buf, pdata, 6, ',', '.');
			for(int i=0; i < 6; i++) if ( pdata[i] == NULL )throw(2);
			if( pdata[0][0] != 'G' ) throw(3);
			for(int i=0; i < 5; i++) {
				trip_range[i] = (float) atof ( pdata[1+i] );
			}
		} else {
			if( buf[0] != 'G' ) throw(4);
			for(int i=0; i < 5; i++) {
				trip_range[i] = getbufFlt(buf, 5+5*i, 5);
			}
		}
	} catch (int& e) {
		sprintf_s(strada_error, sizeof(strada_error), "G:%d", e);
		return false;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void StradaPAR::Read(const char* file_name){

	FILE* fp = NULL;
	char msg[256];
	errno_t error = fopen_s(&fp, file_name, "rt");
	if(error != 0 || fp == NULL) {
		fprintf(stderr, "Cannot open file %s.\n", file_name);
		throw std::runtime_error("PAR");
	}
	else {
		int ret = Read(fp);
		fclose(fp);
		if (ret != 0) {
			snprintf(msg, sizeof(msg), "PAR Error at Line %d [%s]", ret, strada_error);
			throw std::runtime_error(msg);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void StradaPAR::zone_init(int value)
{
	if( value == 0 ) centroids.clear();
	else centroids.resize(value);
	nZone = value;
}
void StradaPAR::direction_init(int value)
{
	if(value <= 0 ) {
		d_nodes.clear();
		nDirection = 0;
	} else {
		d_nodes.clear();
		nDirection = value;
	}
}
// TurnControl -> list
void StradaPAR::turn_init(int value)
{
	Turns.clear();
	nTurn = value;
}

void StradaPAR::oddetail_init(int value)
{
	if(value <= 0) nOdDetail = 0;
	else nOdDetail = value;
	od_links.clear();
}

void StradaPAR::routeinf_init(int value)
{
	if(value < 0) nRouteInf = 0;
	else nRouteInf = value;
	ri_links.clear();
}

void StradaPAR::set_qv(int n, const QV_PARAM& qv)
{
	if( n >= 0 && n < 99 ) {
		if ( ! qvdata[n] ) {
			qvdata[n].reset(new QV_PARAM(qv));
			nParam++;
		}
		else *(qvdata[n].get()) = qv;
	}
}

void StradaPAR::remove_qv(int n )
{
	if( n >= 0 && n < 99 ) {
		if( qvdata[n] ) {
			qvdata[n].reset();
			nParam--;
		}
	}
}

void StradaPAR::print_data()
{
//A
	for(int i=0; i < 99; i++ ) {
		if( qvdata[i] ) {
			printf("A%d %g %g %g %g %g %g %g %g %g",
				qvdata[i]->code, qvdata[i]->v1, qvdata[i]->q1,qvdata[i]->v2, qvdata[i]->q2,
				qvdata[i]->v3, qvdata[i]->q3, qvdata[i]->v4, qvdata[i]->q4, qvdata[i]->delay);
		}
	}
	printf("\n");
// B
	int counter =0;
	for(const auto& turn : Turns) {
		printf("B%d %s %s %s %g\n", counter+1,
		turn.FromNode, turn.TurnNode, turn.ToNode, turn.penalty);
		counter++;
	}
// C
	counter = 0;
	for(const auto& dn : d_nodes ) {
		printf("C%d %s\n", counter+1, dn.name);
	}
// D
	counter = 0;
	for(const auto& odlink : od_links) {
		printf("D%d %s %s %s\n",
		counter+1, odlink.name, odlink.sNode, odlink.eNode);
	}
// E
	counter = 0;
	for(const auto& ri : ri_links ) {
		printf("E%d %s %s %s\n",
		counter+1, ri.name, ri.sNode, ri.eNode);
	}
// F
	if( ConvertType == '1' ) {
		for(int i=0; i < 10; i++) {
			printf("F%d %d %g %g %g %g %g\n", i+1,
			jhpc[i].mode, jhpc[i].a,jhpc[i].b, jhpc[i].c, jhpc[i].K, jhpc[i].S);
		}
	} else if (ConvertType == '2' ) {
		for(int i=0; i < 10; i++) {
			if (pubp[i].mode )
				printf("F%d %d %g %g %g\n", i+1, pub_mode, pubp[i].a, pubp[i].b, pubp[i].c);
		}

	}
// G
	printf("G ");
	for(int i=0; i < 5; i++ ) printf("%g ", trip_range[i]);
}

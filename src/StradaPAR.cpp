#include <new>
//---------------------------------------------------------------------------
#include <string.h>
#include <float.h>
//---------------------------------------------------------------------------
#include <stdexcept>
#include <set>
#include <string>
#include <fstream>
//---------------------------------------------------------------------------
#include "StradaPAR.h"
#include "tool.h"
//---------------------------------------------------------------------------
// extern char strada_error[256];

#define NCHAR 512
//---------------------------------------------------------------------------

str10::str10() {
	memset(name, 0, 11);
}
CentInfo::CentInfo(){
	memset(name, 0, 11);
	flag = true;
}
TurnControl::TurnControl(){
	memset(FromNode,0,11);
	memset(	 ToNode,0,11);
	memset(TurnNode,0,11);
	penalty = 0;
}
LinkNode::LinkNode(){
	memset(sNode, 0, 11);
	memset(eNode, 0, 11);
	memset(name , 0, 11);
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
//	 StradaPARの初期化処理
////////////////////////////////////////////////////////////////////////////////
StradaPAR::StradaPAR(){
	nLink = nNode = nZone = nMode = 0;
	nTurn = nDirection = nOdDetail = nRouteInf = 0;
	init();
}

void StradaPAR::init(){
	strcpy(name, "PAR FILE");
	nParam = 0;
	nMode = 1;	//車種数の初期値は１
	centroids.clear(); nZone = 0;
	Turns.clear(); 	   nTurn = 0;
	d_nodes.clear();   nDirection =  0;
	od_links.clear();  nOdDetail = 0;
	ri_links.clear();  nRouteInf = 0;
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
//	保存
////////////////////////////////////////////////////////////////////////////////
void StradaPAR::Write(FILE* fp){

	char buff[256];

	fprintf(fp,"PAR2 %s\n", name);
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
	FILE* fp;
	if((fp = fopen(file_name ,"wt"))==NULL) throw std::runtime_error("PAR");
	Write(fp);
	fclose(fp);
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void StradaPAR::write_od(FILE* fp){
	int n = nZone / 10;
	int m = nZone % 10;
	for(int i=0; i < n; i++){
		for(int j=0; j < 10; j++){
			fprintf(fp, "%-10s", centroids[10*i+j].name);
			if(centroids[10*i+j].flag) fprintf(fp, "* ");
			else fprintf(fp, "  ");
		}
		fprintf(fp, "\n" );
	}
	if( m > 0 ) {
		for(int i=0; i < m; i++){
			fprintf(fp, "%-10s", centroids[10*n+i].name);
			if(centroids[10*n+i].flag) fprintf(fp, "* "); else fprintf(fp, "  ");
		}
		fprintf(fp,"\n");
	}
}
////////////////////////////////////////////////////////////////////////////////
//	QV, BPR 関数のパラメータ : 書き込み
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
//!	方向規制 : 書き込み
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
//  ＯＤ内訳情報の記録
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
//! 経路書き込み
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
//!  F: 転換率パラメーターの保存
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
//  G: トリップ長分布ランクの記録
////////////////////////////////////////////////////////////////////////////////
void StradaPAR::write_range(FILE* fp){
	if( bTripRank ) {
		fprintf(fp,"G    ");
		for(int i=0; i < 5; i++) fprintf(fp, "%5.1f", trip_range[i]);
		fprintf(fp,"\n");
	}
}
////////////////////////////////////////////////////////////////////////////////
// CSV: 区切り文字は ,
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
		float d = (float)atof(pdata[1+i]);	//MAXFLOATを設定するため
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
// この関数は、成功した場合には０を返し、それ以外は失敗した行番号を返す
////////////////////////////////////////////////////////////////////////////////
int StradaPAR::Read(FILE* fp){

	int ret = 1;
	int buf_length;
	int c, mode;
	char buf[NCHAR];
	int opt[8];
	long loc[8];

	if(fgets(buf, NCHAR,fp)==NULL) return (ret);
	buf[strlen(buf)-1]='\0';
	if( strlen(buf) < 4 ) return (ret);
	if( strncmp(buf, "PAR", 3) != 0 ) return (ret);
	version = buf[3];
	if(strlen(buf)>6)
		strncpy(name, &buf[5],255);
//	printf("%s\n",buf );
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
		buf_length = strlen(buf);
		if( buf_length < 40 ) return(ret);     //40まではある、と思う

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
	// これ以降の項目は記述されていない場合がある。
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
			float d = getbufFlt(buf,5+10*i,10);	//MAXFLOATを設定するため
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
	// A..H が存在するかどうかを確認する
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
		rm = buf[0] - 'A'; // CSV形式であってもスペースは文字の一部と判断する
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
			strncpy( centroids[10*i+j].name, pdata[j], 10 );
			centroids[10*i+j].name[10] = '\0';
		}
	}
	if ( m > 0 ) {
		if(fgets(buf, NCHAR,fp)==NULL) return false;
		line_number++;
		csv_parser(buf, pdata, m, ',', '.');
		for(int j=0; j < 10; j++) {
			strncpy( centroids[10*n+j].name, pdata[j], 10 );
			centroids[10*n+j].name[10] = '\0';
		}
	}
	line_number++;
	if(fgets(buf, NCHAR,fp)==NULL) return false;
	char* token = buf;
	char* p = token;
	p = strtok(buf, ",");// Z
	p = strtok(NULL, ",");
	n = atoi(p);
	for(int i=0; i < n; i++) {
		if( (p = strtok(NULL, ",")) == NULL ) return false;
		m = atoi(p);
		if( m < 1 || m > nZone ) return false;
		centroids[m-1].flag = true;
	}

	return true;
}
////////////////////////////////////////////////////////////////////////////////
//	OD Dataの読み込み
////////////////////////////////////////////////////////////////////////////////
int StradaPAR::read_od(FILE* fp, int &line_number){

	using namespace std;

	char buf[256];
	int n, m ;
	int len ;
	n = nZone / 10;
	m = nZone % 10;

	for(int i=0; i<n ; i++){
		line_number++;
		if(fgets(buf,256,fp)==NULL) return(-1);
		for(int j =0;j < 10;j++){
			strncpy(centroids[10*i+j].name,&buf[12*j],10);
			trim(centroids[10*i+j].name,11);
			len = strlen(centroids[10*i+j].name);
			if( len == 0) return(-1);
			if(buf[12*j+10] == '*') centroids[10*i+j].flag = true;
			else centroids[10*i+j].flag = false;
		}
	}
	if( m > 0 ) {
		line_number++;
		if(fgets(buf,256,fp)==NULL) return(-1);

		for(int i = 0; i < m; i++){
			strncpy(centroids[10*n+i].name,&buf[12*i],10);
			trim(centroids[10*n+i].name,11);
			len = strlen(centroids[10*n+i].name);
			if( len == 0) return(-1);
			if(buf[12*i+10] == '*') centroids[10*n+i].flag = true;
			else centroids[10*n+i].flag = false;
		}
	}
	//重複をチェックする
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
			strncpy(centroids[i].name, newname, 10);
			count++;
		}
	}
	for(auto& turn : Turns) {
		if ( strncmp(turn.FromNode, curname, 10 ) == 0 ) {
			strncpy(turn.FromNode, newname, 10);
			count++;
		}
		if ( strncmp(turn.ToNode, curname, 10 ) == 0 ) {
			strncpy(turn.ToNode, newname, 10);
			count++;
		}
		if ( strncmp(turn.TurnNode, curname, 10 ) == 0 ) {
			strncpy(turn.TurnNode, newname, 10);
			count++;
		}
	}
	for(auto& dn : d_nodes ) {
		if ( strncmp(dn.name, curname, 10 ) == 0 ) {
			strncpy(dn.name, newname, 10);
			count++;
		}
	}
	for(auto& x : od_links ) {
		if ( strncmp(x.sNode, curname, 10 ) == 0 ) {
			strncpy(x.sNode, newname, 10);
			count++;
		}
		if ( strncmp(x.eNode, curname, 10 ) == 0 ) {
			strncpy(x.eNode, newname, 10);
			count++;
		}
	}
	for(auto& ri : ri_links) {
		if ( strncmp(ri.sNode, curname, 10 ) == 0 ) {
			strncpy(ri.sNode, newname, 10);
			count++;
		}
		if ( strncmp(ri.eNode, curname, 10 ) == 0 ) {
			strncpy(ri.eNode, newname, 10);
			count++;
		}
	}
	return count;
}

////////////////////////////////////////////////////////////////////////////////
//	A:速度計算式パラメータの読み込み
////////////////////////////////////////////////////////////////////////////////
bool StradaPAR::read_param(FILE* fp,int c, int &line_number)
{
	char buf[100];
	char* pdata[256];
	int code;
	int len;
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
		sprintf(strada_error, "A:%d", e);
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
//!  B:方向規制データの読み込み
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
				strncpy( turn.FromNode, pdata[1], 10); turn.FromNode[10] = 0;
				strncpy( turn.TurnNode, pdata[2], 10); turn.TurnNode[10] = 0;
				strncpy( turn.ToNode,   pdata[3], 10); turn.ToNode[10] = 0;

				turn.penalty = (float) atof(pdata[4]);

			} else {
				if( i > 0 ){
					line_number++;
					if( fgets(buf, 100, fp) == NULL ) throw(1);
					if(buf[0] != 'B') throw(1);
				}
				strncpy( turn.FromNode, &buf[5], 10);
				trim(turn.FromNode, 11);
				strncpy( turn.TurnNode, &buf[15], 10);
				trim(turn.TurnNode, 11);
				strncpy( turn.ToNode, &buf[25], 10);
				trim(turn.ToNode, 11);
				turn.penalty = getbufFlt(buf, 35, 10);
			}
			Turns.push_back(turn);
		}
	} catch(int &e) {
		Turns.clear();
		nTurn = 0;
		sprintf(strada_error, "B:%d", e);
		return false;
	}
	return true;
}
/////////////////////////////////////////////////////////////////////////////////
//  C:方向別交通量算定ノードの読み込み
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
					strncpy(dn.name, pdata[1+k], 10); dn.name[10] = '\0';
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
					strncpy(dn.name, pdata[1+j], 10); dn.name[10] = '\0';
					d_nodes.push_back(dn);
				}
			}
		} else {
			for(int j =0; j < m; j++ ) {
				for(int k=0; k < 15; k++) {
					str10 dn;
					strncpy(dn.name, &buf[5+10*k], 10);
					trim(dn.name,10);
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
				strncpy(dn.name, &buf[5+10*j], 10);
				trim(dn.name,10);
				d_nodes.push_back(dn);
			}
		}
	} catch(int& e) {
		d_nodes.clear();
		nDirection = 0;
		sprintf(strada_error, "C:%d", e);
		return false;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//  D: OD内訳の読み込み
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
				strncpy(odlink.name, pdata[1], 10) ; odlink.name[10] = '\0';
				strncpy(odlink.sNode, pdata[2], 10) ; odlink.sNode[10] = '\0';
				strncpy(odlink.eNode, pdata[3], 10) ; odlink.eNode[10] = '\0';
			} else {
				strncpy(odlink.name, &buf[5], 10);
				trim(odlink.name,10);
				strncpy(odlink.sNode, &buf[15], 10);
				trim(odlink.sNode,10);
				strncpy(odlink.eNode, &buf[25], 10);
				trim(odlink.eNode,10);
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
		sprintf(strada_error, "D:%d", e);
		return false;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//  E: 経路情報の読み込み
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
				strncpy(ri.name, pdata[1], 10) ; ri.name[10] = '\0';
				strncpy(ri.sNode, pdata[2], 10) ; ri.sNode[10] = '\0';
				strncpy(ri.eNode, pdata[3], 10) ; ri.eNode[10] = '\0';

			} else {
				strncpy(ri.name, &buf[5], 10);
				trim(ri.name,10);
				strncpy(ri.sNode, &buf[15], 10);
				trim(ri.sNode,10);
				strncpy(ri.eNode, &buf[25], 10);
				trim(ri.eNode,10);
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
		sprintf(strada_error, "E:%d", e);
		return false;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//  F:転換率パラメーターの読み込み
//  転換率の設定によって読み込み方を変更する
////////////////////////////////////////////////////////////////////////////////
bool StradaPAR::read_divparam(FILE* fp,int c,  int &line_number){

	char buf[256];
	char* pdata[256];
	int m ;
	int len;
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
		sprintf(strada_error, "F:%d", e);
		return false;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//  G: トリップ長分布ランクの読み込み
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
		sprintf(strada_error, "G:%d", e);
		return false;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void StradaPAR::Read(const char* file_name){

	FILE* fp;
	char msg[256];
	if((fp = fopen(file_name,"rt"))==NULL) {
		fprintf(stderr, "Cannot open file %s.\n", file_name);
		throw std::runtime_error("PAR");
	}
	int ret =Read(fp);
	fclose(fp);
	if( ret != 0 ) {
		snprintf(msg, sizeof(msg), "PAR Error at Line %d [%s]",ret, strada_error);
		throw std::runtime_error(msg);
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
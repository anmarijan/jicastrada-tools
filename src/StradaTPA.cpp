#include <pch.h>
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdexcept>
/*----------------------------------------------------------------------------*/
#include "StradaTPA.h"
#include "tool.h"
/*----------------------------------------------------------------------------*/
extern char strada_error[256];
/*----------------------------------------------------------------------------*/
TermPenalty::TermPenalty() {
	memset(node, 0, 11);
	memset(fline, 0, 11);
	memset(tline, 0, 11);
	fpenalty = tpenalty = 0;
}

int TermPenalty::Read(const char* str) {
	char temp[6];
	int len = strlen(str);
	if ( len < 40 ) return -1;
	strncpy_s(node, sizeof(node), str, 10);
	strncpy_s(fline, sizeof(fline), str+10, 10);
	strncpy_s(tline, sizeof(tline), str+20, 10);
	strncpy_s(temp, sizeof(temp), str+30, 5);
	temp[5] = 0;
	fpenalty = atoi(temp);
	strncpy_s(temp, sizeof(temp), str+35, 5);
	temp[5] = 0;
	tpenalty = atoi(temp);
	return 1;
}

double StradaMode::board_fare(bool txf) {
	if( txf==true && allow_transfer==true) return 0;
	float cst = base_fare * cFareTime ;
	return static_cast<double>(cst);
}

double StradaMode::fare_cost(double sumdst){
	double f = 0;
	double ex_dst = sumdst - base_dist;
	if(ex_dst > 0){
		f = cFareTime * ex_dst	* excess ;
	}
	return f;
}
////////////////////////////////////////////////////////////////////////////////
//  TPA
////////////////////////////////////////////////////////////////////////////////
const char* StradaTPA::getCentroid(int i) {
	return centroids[i].c_str();
}
StradaTPA::StradaTPA(){
	nZone = nLine = 0; nMode = nPenalty = 0; nRep1 = nRep2 = nRep3 = 0 ;
	for (int i = 0; i < 10; i++) {
		b_report[i] = false;
		rate[i] = 10;
	}
	max_headway_adj = 0; max_path = 3; max_trans = 2; min_load_limit = 0;
	type = 'F'; timevalue = 1; mshar_type = 1; path_limit = 3;
	vf = wf = 1.0;
}
void StradaTPA::clear(){
    modes.clear();
    centroids.clear();
    LineToLine.clear();
    NodeToNode.clear();
    Interline.clear();
	nZone = nLine = 0; nMode = nPenalty = 0; nRep1 = nRep2 = nRep3 = 0 ;
	vf = wf = 1.0;
}

int StradaTPA::Read(FILE* fp) {

	char buf[200];
    char name[12];
    int line_no = 0;
    int nIterator;
	if(fgets(buf,100,fp)==NULL) return (-1);
    line_no++;

	if( strncmp(buf, "TPA2", 4) != 0 ) {
		fprintf(stderr, "This is not a TPA file.\n");
		return(-1);
	}
	header = buf;

	if(fgets(buf,100,fp)==NULL) return (-2);
    line_no++;

    try {
        nZone = getbufInt(buf,0,5);
        nMode = getbufInt(buf,5,5);
        nLine = getbufInt(buf,10,5);
        nPenalty = getbufInt(buf,15,5);

        if(fgets(buf,100,fp)==NULL) return (-3);
        line_no++;
        buf[strlen(buf)-1]='\0';
        nIterator = (strlen(buf) - 56)/3;

        if( strlen(buf) < 59 ) return (-3);
        line_no++;

        vf = getbufFlt(buf,0,5);
        wf = getbufFlt(buf,5,5);
        timevalue = getbufFlt(buf,10,5);
        type = buf[17];
        max_trans  = getbufInt(buf,18,4);
//	if(max_trans <= 0 || max_trans > 3 ) max_trans = 3;//
        max_path   = getbufInt(buf,22,4);
        if( max_path  <= 0 || max_path > 15 ) max_path = 5;
        path_limit = getbufInt(buf,26,5);
        mshar_type = getbufInt(buf,31,5);
        max_headway_adj = getbufInt(buf,41,3);
        min_load_limit	= getbufInt(buf,44,2);
        for(int j=0;j<10;j++){
            if(buf[46+j] == 'T') b_report[j] = true;
            else b_report[j] = false;
        }
        for(int j=0;j<nIterator;j++){
            rate[j] = getbufInt(buf,56+3*j,3);
        }
// report data 1
        if(fgets(buf,200,fp)==NULL) return(-4);
        line_no++;
        nRep1 = pack_data(buf, LineToLine);
// report data 2
        if(fgets(buf,200,fp)==NULL) return(-5);
        line_no++;
        nRep2 = pack_data(buf, NodeToNode);
// report data 3
        if(fgets(buf,200,fp)==NULL) return(-6);
        line_no++;
        nRep3 = pack_data(buf, Interline);
// mode data
        modes.resize(nMode);
        for(int j = 0; j < nMode; j++ ){
            line_no++;
            if( fgets(buf,100,fp)==NULL) throw;
            buf[strlen(buf)-1]= '\0';
            if( modes[j].Read(buf) == -1) throw;
        }

        centroids.resize(nZone);

        int m = nZone / 10;
        int n = nZone % 10;
        for(int j =0; j<m; j++){

            line_no++;
            if(fgets(buf,200,fp)==NULL) throw;

            for(int k=0;k<10;k++){
                strncpy_s(name, sizeof(name), &buf[10*k], 10);
                trim(name,11);
                centroids[10*j+k] = name;
            }
        }
        if(n!=0){
            line_no++;
            if(fgets(buf,200,fp)==NULL) throw;

            for(int j = 0; j < n; j++){
                strncpy_s(name, sizeof(name), &buf[10*j], 10);
                trim(name,11);
                centroids[10*m+j] = name;
            }
        }
// Turn penalty
        penalties.resize(nPenalty);
        for(int i=0; i < nPenalty; i++) {
            line_no++;
            if(fgets(buf,100,fp)==NULL) return(-1)*line_no;
            if( penalties[i].Read(buf) < 0 ) return (-1)*line_no;
        }
    } catch (...) {
        return (-1)*line_no;
    }
	return 1;
}

StradaMode::StradaMode() {
	id = 0;
	 allow_transfer = false;
	 base_fare		= 0;
	 base_dist		= 0;
	 excess			= 0;
	 capacity		= 0;
	 min_frequency	= 0;
	 max_frequency	= 0;
	 PCU			= 0;
	 cWalkTime		= 0;
	 cWaitTime		= 0;
	 cLoadTime		= 0;
	 cFareTime		= 0;
	 cTravelTime	= 0;
	 cTransTime		= 0;
	 cCongTime		= 0;
	 min_speed		= 0;
	 max_speed		= 0;
}

int StradaMode::Read( char* buf)
{
    if( strlen(buf) < 85 ) return (-1);

		id = getbufInt(buf,0,3);
		if(buf[4]=='T') allow_transfer = true;
		else allow_transfer = false;
		base_fare = getbufFlt(buf, 5,5);
		base_dist = getbufFlt(buf,10,5);
		excess	  = getbufFlt(buf,15,5);
		capacity	  = getbufInt(buf,20,5);
		min_frequency = getbufInt(buf,25, 5);
		max_frequency = getbufInt(buf,30, 5);
		PCU			= getbufFlt(buf,35,5);
		cWalkTime	= getbufFlt(buf,40,5);
		cWaitTime	= getbufFlt(buf,45,5);
		cLoadTime	= getbufFlt(buf,50,5);
		cFareTime	= getbufFlt(buf,55,5);
		cTravelTime = getbufFlt(buf,60,5);
		cTransTime	= getbufFlt(buf,65,5);
		cCongTime	= getbufFlt(buf,70,5);
		min_speed	= getbufFlt(buf,75,5);
		max_speed	= getbufFlt(buf,80,5);

        if( base_fare < 0 ) return -1;
        if( base_dist < 0 ) return -1;
        if( excess < 0 ) return -1;

		return(1);

}
int StradaMode::Read(FILE* fp){
	char buf[100];

	if(fgets(buf,100,fp)==NULL) return(-1);

		id = getbufInt(buf,0,3);
		if(buf[4]=='T') allow_transfer = true;
		else allow_transfer = false;
		base_fare = getbufFlt(buf, 5,5);
		base_dist = getbufFlt(buf,10,5);
		excess	  = getbufFlt(buf,15,5);
		capacity	  = getbufInt(buf,20,5);
		min_frequency = getbufInt(buf,25, 5);
		max_frequency = getbufInt(buf,30, 5);
		PCU			= getbufFlt(buf,35,5);
		cWalkTime	= getbufFlt(buf,40,5);
		cWaitTime	= getbufFlt(buf,45,5);
		cLoadTime	= getbufFlt(buf,50,5);
		cFareTime	= getbufFlt(buf,55,5);
		cTravelTime = getbufFlt(buf,60,5);
		cTransTime	= getbufFlt(buf,65,5);
		cCongTime	= getbufFlt(buf,70,5);
		min_speed	= getbufFlt(buf,75,5);
		max_speed	= getbufFlt(buf,80,5);

        if( base_fare < 0 ) return -1;
        if( base_dist < 0 ) return -1;
        if( excess < 0 ) return -1;

		return(1);
}

void StradaMode::Write(FILE* fp){
	char a = (allow_transfer)? 'T' : 'F';
	fprintf(fp,"%3d %c%5.2g%5.2g%5.2g%5d%5d%5d%5g%5g%5g%5g%5g%5g%5g%5g%5g%5g\n",
			id,a,base_fare,base_dist,excess,
			capacity,min_frequency,	max_frequency,
			PCU,cWalkTime,cWaitTime,cLoadTime,cFareTime,
			cTravelTime,cTransTime,cCongTime,min_speed,max_speed);
}
void StradaMode::setMode(StradaMode* sm){
	 allow_transfer = sm->allow_transfer;
	 base_fare		= sm->base_fare;
	 base_dist		= sm->base_dist;
	 excess			= sm->excess;
	 capacity		= sm->capacity;
	 min_frequency	= sm->min_frequency;
	 max_frequency	= sm->max_frequency;
	 PCU			= sm->PCU;
	 cWalkTime		= sm->cWalkTime;
	 cWaitTime		= sm->cWaitTime;
	 cLoadTime		= sm->cLoadTime;
	 cFareTime		= sm->cFareTime;
	 cTravelTime	= sm->cTravelTime;
	 cTransTime		= sm->cTransTime;
	 cCongTime		= sm->cCongTime;
	 min_speed		= sm->min_speed;
	 max_speed		= sm->max_speed;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
int StradaTPA::pack_data(char* buf, std::vector<std::string> &target){
	//assert( target == NULL );
	int len = strlen(buf);
    char temp[12];
    if( len > 0 ) {
    	buf[len-1] = '\0';
        len--;
    } else return 0;

	while (len > 0 && buf[len-1] == ' ') {
		buf[len-1] = '\0';
		len--;
	}
	int ret = len / 10;
	if( len % 10 > 0 ) ret += 1;

    if( ret == 0 ) return 0 ;

//	target = new char*[ret];
    target.resize(ret);

	for(int i=0; i< ret ; i++){
//		target[i] = new char[11];

		for(int j=0; j < 10; j++){
            temp[j] = buf[10*i+j];
//			target[i][j] = buf[5*i+j];
		}
        temp[10] = '\0';
		trim(temp);
        target[i] = temp;
	}
	return ret ;
}

void StradaTPA::Read(const char* fname){

	FILE* fp;
	errno_t error = fopen_s(&fp, fname, "rt");
	if(error != 0 || fp==NULL) throw std::runtime_error("Cannot open TPA file.");
	else {
		int ret = Read(fp);
		fclose(fp);
		if (ret < 0) {
			sprintf_s(strada_error, sizeof(strada_error), "Failure in TPA file at line %d.\n", -ret);
			throw std::runtime_error(strada_error);
		}
	}
}

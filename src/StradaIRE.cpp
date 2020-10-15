#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <limits.h> //INT_MAX
/*-------------------------------------------------------------------------*/
#include <set>
#include <string>
#include <stdexcept>
/*-------------------------------------------------------------------------*/
#include "StradaCmn.h"
#include "StradaIRE.h"
#include "StradaINT.h"
#include "tool.h"
/*-------------------------------------------------------------------------*/
#define READ_DUMMY_XY(A)  kip = strtok(NULL,","); \
if(kip == NULL) throw std::runtime_error("CSV in Link Dummy XY"); \
(A) = (float)atof(kip);

#ifndef  KIP_ERROR
#define KIP_ERROR if ( kip == NULL ) throw std::runtime_error("CSV in Link");
#endif
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
// 全モードの合計（内々=1、内外=2、外外=3）
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
// モード別（内々=1、内外=2、外外=3）10/03/21(日) 21:02:06
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
// 旅客数の計算 2010/01/24 4:07    PCU=0の時は旅客数ゼロ
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
// 台数の計算 2010/01/24 4:15
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
//! 方向別交通量部分のバッファからの読み込み
/* @return 0: 正常終了、-1:浮動小数点の変換エラー */
//---------------------------------------------------------------------------
int OnewayResult::Read(char* buf){

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

int OnewayResult::ReadCSV(){

	char* kip;

	try {
		kip = strtok(NULL,","); avSp = (float)atof(kip);
		kip = strtok(NULL,","); ltSp = (float)atof(kip);
		kip = strtok(NULL,","); VCR  = (float)atof(kip);
		kip = strtok(NULL,","); Vol  = (float)atof(kip);

		for(int i=0; i < 10; i++){
			kip = strtok(NULL,","); inVol[i] = (float)atof(kip);
			kip = strtok(NULL,","); thVol[i] = (float)atof(kip);
			kip = strtok(NULL,","); btVol[i] = (float)atof(kip);
		}

		kip = strtok(NULL,","); ATL  = (float)atof(kip);

		for(int i=0; i < 6; i++) {
			kip = strtok(NULL,",");
			trVol[i] = (float)atof(kip);
		}
	} catch(std::exception& ){
		return(-1);
	}
	return(0);
// 232+6+7*6 =
}
//---------------------------------------------------------------------------
//! ファイルへの書き込み
//---------------------------------------------------------------------------
void OnewayResult::Write(FILE* fp){

	char buff[300];
	fixfloat(buff,    avSp, 5);
	fixfloat(buff+5 , ltSp, 5);
	fixfloat(buff+10, VCR,  5);
	fixfloat(buff+15, Vol,  7);
	for(int i=0; i < 10; i++){
		fixfloat(buff+22+21*i, inVol[i], 7);
		fixfloat(buff+29+21*i, thVol[i], 7);
		fixfloat(buff+36+21*i, btVol[i], 7);
	}
	fixfloat(buff+232, ATL, 6);
	for(int i=0; i < 6; i++) fixfloat(buff+238+7*i, trVol[i], 7);

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

}

IRELinkV2::IRELinkV2(SLinkV2& s) : SLinkV2(s) {

}
//------------------------------------------------------------------------------
//! 結果のリンクデータ読み込み
//------------------------------------------------------------------------------
int IRELinkV2::Read(char* buf){

	try {
        strncpy(name, &buf[0], 10);    trim(name,11);  name[10]  = '\0';
        strncpy(sNode, &buf[10], 10);  trim(sNode,11); sNode[10] = '\0';
        strncpy(eNode, &buf[20], 10);  trim(eNode,11); eNode[10] = '\0';

        length = getbufFlt(buf, 30, 7);
        Vmax   = getbufFlt(buf, 37, 5);
        Capa   = getbufFlt(buf, 42, 8);
        QV	   = getbufInt(buf, 50, 2);
        for(int i=0; i < 10; i++) ways[i] = buf[52+i] ;

        char str[300];
        memset(str, 0, 300);
        strncpy(str, &buf[62], 280);
        if( result[0].Read(str) == -1) throw std::runtime_error("IRE");

        memset(str, 0, 300);
        strncpy(str, &buf[342], 280);
        if( result[1].Read(str) == -1) throw std::runtime_error("IRE");

        linktype = buf[622];
        if(buf[623] == '0') evaluation = true; else evaluation = false;
        display = buf[624];
        aFlag1	= buf[625];
        nFlag2	= getbufInt(buf, 626, 2);
        nFlag3	= getbufInt(buf, 628, 2);
        aFlag4[0]  = buf[630];
        aFlag4[1]  = buf[631];
        aFlag5[0]  = buf[632];
        aFlag5[1]  = buf[633];
        aFlag5[2]  = buf[634];

        iX = getbufFlt(buf,645,10);
        iY = getbufFlt(buf,655,10);
        jX = getbufFlt(buf,665,10);
        jY = getbufFlt(buf,675,10);

        dummy = getbufInt(buf, 685, 5);

        if( dummy > 0 ) {
			dX[0] = getbufFlt(buf, 690, 10);
			dY[0] = getbufFlt(buf, 700, 10);

			if( dummy > 1 ) {
				dX[1] = getbufFlt(buf, 710, 10);
				dY[1] = getbufFlt(buf, 720, 10);

				if( dummy > 2 ) {
					dX[2] = getbufFlt( buf, 730, 10);
					dY[2] = getbufFlt( buf, 740, 10);
				}
            }

        }
	} catch (const std::exception& e) {
		throw e;
    }
	return(1);
}
//---------------------------------------------------------------------------
//! CSV読み込み：bufの最後の改行文字は削除して渡すこと
// bufは有限長の文字列であること
// SPACEs are trimmed
//---------------------------------------------------------------------------
void IRELinkV2::ReadCSV(char* buf){
	char* kip;
	int counter = 0;	// , の数を数える（100以上であるはず）
	kip = buf;
	while( *kip != '\0') {
		if(*kip == ',' ) counter++;
		kip++;
	}
	if ( counter < 109 ) throw  std::runtime_error("CSV in IRELink");

	kip = strtok(buf, ","); dqconv(kip, name, 10); trim(name,11);

	kip = strtok(NULL,","); dqconv(kip, sNode, 10); trim(name,11);
	kip = strtok(NULL,","); dqconv(kip, eNode, 10); trim(name,11);
    kip = strtok(NULL,","); length = (float)atof(kip);
    kip = strtok(NULL,","); Vmax   = (float)atof(kip);
    kip = strtok(NULL,","); Capa   = (float)atof(kip);
    kip = strtok(NULL,","); QV     = (int)atof(kip);
    for(int i=0; i < 10; i++) {
		kip = strtok(NULL,",");
		ways[i] = *kip;
	}

    if( result[0].ReadCSV() == -1) throw std::runtime_error("IRE");
    if( result[1].ReadCSV() == -1) throw std::runtime_error("IRE");

	kip = strtok(NULL,","); linktype = *kip;
	kip = strtok(NULL,",");
    if( *kip == '0') evaluation = true; else evaluation = false;
	kip = strtok(NULL,","); display = *kip;
	kip = strtok(NULL,","); dqconv(kip, aFlag4, 1); aFlag1 = aFlag4[0];
	kip = strtok(NULL,","); nFlag2 = atoi(kip);
	kip = strtok(NULL,","); nFlag3 = atoi(kip);
//	printf("%d %d", nFlag2, nFlag3);
	kip = strtok(NULL,","); dqconv(kip, aFlag4, 2);
	kip = strtok(NULL,","); dqconv(kip, aFlag5, 3);

	kip = strtok(NULL,",");	//dummy

	kip = strtok(NULL,","); iX = (float)atof(kip);
	kip = strtok(NULL,","); iY = (float)atof(kip);
	kip = strtok(NULL,","); jX = (float)atof(kip);
	kip = strtok(NULL,","); jY = (float)atof(kip);

	kip = strtok(NULL,","); dummy = atoi(kip);
//	printf("%g %g %g %g\n", iX, iY, jX, jY);
	if( dummy > 0 ) {
		READ_DUMMY_XY( dX[0] )
		READ_DUMMY_XY( dY[0] )
		if( dummy > 1 ) {
			READ_DUMMY_XY( dX[1] )
			READ_DUMMY_XY( dY[1] )
			if( dummy > 2 ) {
				READ_DUMMY_XY( dX[2] )
				READ_DUMMY_XY( dY[2] )
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// 人数（両方向）2010/01/24 6:03
/////////////////////////////////////////////////////////////////////////////
float IRELinkV2::pass_vol(float pcu[], float apc[], int t) {
	return result[0].pass_vol(pcu, apc, t) + result[1].pass_vol(pcu, apc, t);
}
/////////////////////////////////////////////////////////////////////////////
// 台数（両方向）2010/01/24 6:03
/////////////////////////////////////////////////////////////////////////////
float IRELinkV2::veh_vol(float pcu[], int t) {
	return result[0].veh_vol(pcu, t) + result[1].veh_vol(pcu, t);
}
/////////////////////////////////////////////////////////////////////////////
// PCU-KM 10/01/30(土) 18:44:14
/////////////////////////////////////////////////////////////////////////////
float IRELinkV2::pcu_km(int m, int t) {
	return length * (result[0].mode_vol(m,t) + result[1].mode_vol(m,t) );
}
/////////////////////////////////////////////////////////////////////////////
// PCU-HOUR 10/01/30(土) 19:02:54
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
// PCU-HOUR:Speed 11/06/25(土)
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
// 2013/12/23
/////////////////////////////////////////////////////////////////////////////
double IRELinkV2::VCR() {
	double ret = 0;
	if (Capa > 0) {
		ret = (result[0].Vol+result[1].Vol)/Capa ;
	}
	return ret;
 }
/////////////////////////////////////////////////////////////////////////////
// 2013/12/23
/////////////////////////////////////////////////////////////////////////////
double IRELinkV2::VCR(int d) {
	double ret = 0;
	if (Capa > 0) {
		ret = result[d].Vol / (Capa * 0.5) ;
	}
	return ret;
 }

//---------------------------------------------------------------------------
//! リンクの書き込み
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
	fprintf(fp,"%10.8g,%10.8g,%10.8g,%10.8g,", iX,iY,jX,jY);
	fprintf(fp,"%5d",dummy);
	for(int i=0; i < dummy; i++) {
		fprintf(fp,",%10.8g,%10.8g",dX[i],dY[i]);
	}
	fprintf(fp,"\n");
}
//------------------------------------------------------------------------------
//! コンストラクタ
//------------------------------------------------------------------------------
StradaIRE::StradaIRE(){
	nLink = nNode = nMode = 0;
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
    memset(msg, '\0', 64);
}
//------------------------------------------------------------------------------
//! デストラクタ
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
//  INT2形式で保存

////////////////////////////////////////////////////////////////////////////////
void StradaIRE::WriteAsINTV2(char* fname){
	FILE* fp;
	if((fp = fopen(fname ,"wt"))==NULL) throw std::runtime_error("IRE2");
	WriteAsINTV2(fp);
	fclose(fp);
}
//---------------------------------------------------------------------------
//! IRE2ファイルの読み込み
//---------------------------------------------------------------------------
void StradaIRE::Read(const char* fname) {
	FILE* fp;
    char buff[20];
	if((fp = fopen(fname ,"rt"))==NULL) {
		std::string str1("Cannot open IRE: ");
		std::string str2(fname);
		throw std::runtime_error(str1+str2);
	}
	int ret = Read(fp);
	fclose(fp);
	if( ret < 0 ) {
    	sprintf(buff, "IRE2(%d)", -ret);
    	throw std::runtime_error(buff);
    }
}
//------------------------------------------------------------------------------
//! IREファイルの読み込み
// 1: 最初の行
// 2: IRE2ファイルではない
// 3: ２行目読み込み失敗
// 4: ２行目の数値変換失敗
//------------------------------------------------------------------------------
int StradaIRE::Read(FILE* fp){

	char buf[1024];
	char* kip;

	if( fgets(buf, 1024, fp) == NULL ) return(-1);
	if( buf[0] != 'I' || buf[1] != 'R' || buf[2] != 'E' || buf[3] != '2' )
		return(-2);

	if(buf[4] == '*') csv = true; else csv = false;
    buf[strlen(buf)-1]=0;
    sprintf(comment, &buf[5], 256);

	if( fgets(buf, 1024, fp) == NULL) return (-3);
    try {
		if( csv ) {
			kip = strtok(buf, ","); KIP_ERROR; nLink = atoi(kip);
			kip = strtok(NULL, ","); KIP_ERROR; nNode = atoi(kip);
			kip = strtok(NULL, ","); KIP_ERROR; nMode = atoi(kip);
			for(int i=0; i < 5; i++) {
				kip = strtok(NULL, ","); KIP_ERROR; Ranks[i] = (float)atof(kip);
			}
			kip = strtok(NULL, ","); KIP_ERROR; coordinate = atoi(kip);
			for(int i=0; i < 10; i++){
				kip = strtok(NULL, ","); KIP_ERROR; APC[i] = (float)atof(kip);
				kip = strtok(NULL, ","); KIP_ERROR; PCU[i] = (float)atof(kip);
			}

		} else {

			nLink = getbufInt(buf, 0, 5);
			nNode = getbufInt(buf, 5, 5);
			nMode = getbufInt(buf,10, 5);

			for(int i=0; i < 5; i++) Ranks[i] = getbufFlt(buf, 15+5*i,5);
			coordinate = getbufInt(buf, 40, 5);

			for(int i=0; i < 10; i++){
				APC[i] = getbufFlt(buf, 45 + i* 10, 5);
				PCU[i] = getbufFlt(buf, 50 + i* 10, 5);
			}
		}

    } catch(const std::exception& ) {
		return (-4);
    }

	try {
		links.clear();
		for(int i=0; i < nLink; i++){
			links.push_back(new IRELinkV2());
		}
    } catch(std::bad_alloc& e) {
		links.clear();
        return (-5);
    }
	int count;

    try {
		for(count=0; count < nLink; count++){
			if( fgets(buf,1024, fp) == NULL ) throw std::runtime_error("IRE2");
			if( csv ) {
				links[count]->ReadCSV(buf);
			} else {
				if( links[count]->Read(buf) == -1) throw std::runtime_error("LINE");
			}
		}
    } catch (const std::exception& e) {
		links.clear();
        sprintf(msg, "%s %d",e.what(), count);
        return(-6);
	}
	return(1);
}
////////////////////////////////////////////////////////////////////////////////
//  IREファイルの保存
////////////////////////////////////////////////////////////////////////////////
void StradaIRE::Write(FILE* fp){
	char buff[10];
	//ノード数はここで再確認
	std::set<std::string> node_table;
	for(int i=0; i < nLink; i++){
		node_table.insert(links[i]->sNode);
		node_table.insert(links[i]->eNode);
	}
	nNode = node_table.size();

	fprintf(fp,"IRE2 ");
	//print_header(fp,"IRE2");
    fprintf(fp, "%s\n", comment);

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
//  IREファイルの保存: ファイル名指定
////////////////////////////////////////////////////////////////////////////////
void StradaIRE::Write(char* fname) {
	FILE* fp;
	if((fp = fopen(fname ,"wt"))==NULL) throw std::runtime_error("IRE2");
	Write(fp);
	fclose(fp);
}
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void StradaIRE::WriteMInfo(const char* fname, double dai, int vtype) {
    FILE* fp_mif;
    FILE* fp_mid;
	char file_name[255];
    double width;

    set_fname(fname, file_name, "mif" );
    float xo, yo, xm, ym;
    xo = FLT_MAX ;
    yo = FLT_MAX ;
    xm = 0;
    ym = 0;
    //    float yo = min_y;
//    float xm = max_x;
//    float ym = max_y;

    if( (fp_mif = fopen(file_name, "wt")) != NULL ) {
        set_fname(fname, file_name, "mid" );
        if( (fp_mid = fopen(file_name, "wt")) != NULL ) {

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
void StradaIRE::calc_mixy(double xi, double yi, double xj, double yj,
                                                            double width) {
    double r = sqrt(pow(xi-xj,2)+pow(yi-yj,2)); //長さ
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

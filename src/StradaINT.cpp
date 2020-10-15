#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>	//FLT_MAX
//---------------------------------------------------------------------------
#include <string>
#include <vector>
#include <set>
#include <stdexcept>
//---------------------------------------------------------------------------
#include "tool.h"
#include "StradaINT.h"
//---------------------------------------------------------------------------
#define INT_BUF 512

#define READ_DUMMY_XY(A)  kip = strtok(NULL,","); \
if(kip == NULL) throw std::runtime_error("CSV in IRELink Dummy XY"); \
(A) = (float)atof(kip);

#ifndef KIP_ERROR
#define KIP_ERROR if ( kip == NULL ) throw std::runtime_error("CSV in Link");
#endif

static char buff[INT_BUF];
//---------------------------------------------------------------------------
//! 大小比較用の構造体
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
//	１ライン分を読み込む（Ver2形式）
////////////////////////////////////////////////////////////////////////////////
bool INTLinkV2::Read(char* link_str){

	char temp[20];

	strncpy(name, &link_str[0], 10);	trim(name,11);	name[10]  = '\0';
	strncpy(sNode, &link_str[10], 10);	trim(sNode,11); sNode[10] = '\0';
	strncpy(eNode, &link_str[20], 10);	trim(eNode,11); eNode[10] = '\0';
	if( name[0] == '\0' || sNode[0] == '\0' || eNode[0] == '\0') return false;

	memset(temp, 0, 20); strncpy(temp, &link_str[30], 7); temp[sizeof(temp) - 1] = 0;
	length = (float)atof( temp );
	memset(temp,0, 20); strncpy( temp, &link_str[37], 5) ;
	Vmax   = (float)atof( temp );
	memset(temp,0, 20); strncpy( temp, &link_str[42], 8) ;
	Capa   = (float)atof( temp );
	memset(temp,0, 20); strncpy( temp, &link_str[50], 3) ;
	QV	   = atoi(temp );

	char c;
	memset(temp, 0, 20);
	for(int i=0; i < 10; i++ ) {
		fare[i] = (float)atof(strncpy( temp, &link_str[53+i*5], 5));
		c = link_str[103+i];
		if( c != '1' && c != '2' && c != '3') c = '0';
		ways[i] = c;
	}
	linktype = link_str[113];
	if( link_str[114] == '1' ) evaluation = false;
		else evaluation = true;

	display = link_str[115];
	aFlag1	= link_str[116];
	memset(temp, 0, 20); strncpy( temp, &link_str[117], 2 );
	nFlag2 = atoi( temp );
	memset(temp, 0, 20); strncpy( temp, &link_str[119], 2 );
	nFlag3 = atoi( temp );
	aFlag4[0] = link_str[121]; aFlag4[1] = link_str[122];
	aFlag5[0] = link_str[123]; aFlag5[1] = link_str[124]; aFlag5[2] = link_str[125];
	color = link_str[126];
	memset(temp,0, 20);
	iX = (float)atof( strncpy( temp, &link_str[140], 10)) ;
	iY = (float)atof( strncpy( temp, &link_str[150], 10)) ;
	jX = (float)atof( strncpy( temp, &link_str[160], 10)) ;
	jY = (float)atof( strncpy( temp, &link_str[170], 10)) ;
	temp[5] = '\0';
	dummy = atoi( strncpy( temp, & link_str[180], 5));
    if( dummy < 0 || dummy > 3 ) return false;
	if( dummy > 0 ){
		dX[0] = (float)atof( strncpy( temp, &link_str[185], 10)) ;
		dY[0] = (float)atof( strncpy( temp, &link_str[195], 10)) ;
		if(dummy > 1) {
		dX[1] = (float)atof( strncpy( temp, &link_str[205], 10)) ;
		dY[1] = (float)atof( strncpy( temp, &link_str[215], 10)) ;
			if(dummy > 2 ) {
				dX[2] = (float)atof( strncpy( temp, &link_str[225], 10) ) ;
				dY[2] = (float)atof( strncpy( temp, &link_str[235], 10) ) ;
			}
		}
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
// 属性のみを複写する
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
////////////////////////////////////////////////////////////////////////////////
//	１ライン分を読み込む（CSV Ver2形式）
////////////////////////////////////////////////////////////////////////////////
bool INTLinkV2::ReadCSV(char* link_str) {
	char* kip;
	int counter = 0;	// , の数を数える（40以上であるはず）
	char temp[20];
	kip = link_str;
	while( *kip != '\0') {
		if(*kip == ',' ) counter++;
		kip++;
	}
	if ( counter < 40 ) throw  std::runtime_error("CSV in INTLink");
	kip = strtok(link_str, ","); dqconv(kip, name, 10); trim(name);
	kip = strtok(NULL,","); dqconv(kip, sNode, 10);  trim(sNode);
	kip = strtok(NULL,","); dqconv(kip, eNode, 10); trim(eNode);
    kip = strtok(NULL,","); length = (float)atof(kip);
    kip = strtok(NULL,","); Vmax   = (float)atof(kip);
    kip = strtok(NULL,","); Capa   = (float)atof(kip);
    kip = strtok(NULL,","); QV     = atoi(kip);

    for(int i=0; i < 10; i++) {
		kip = strtok(NULL,",");
		fare[i] = (float)atof(kip);
	}

	for(int i=0; i < 10; i++) {
		kip = strtok(NULL,",");
		ways[i] = *kip;
	}
	kip = strtok(NULL,","); linktype = *kip;
	kip = strtok(NULL,",");
    if( *kip == '0') evaluation = true; else evaluation = false;
	kip = strtok(NULL,","); display = *kip;
	kip = strtok(NULL,","); dqconv(kip, temp, 1); aFlag1 = temp[0];
	kip = strtok(NULL,","); nFlag2 = atoi(kip);
	kip = strtok(NULL,","); nFlag3 = atoi(kip);
	kip = strtok(NULL,","); dqconv(kip, temp, 2);
	counter = 0;
	while( temp[counter] != 0 && counter < 2 ) {
		aFlag4[counter] = temp[counter];
		counter++;
	}
	kip = strtok(NULL,","); dqconv(kip, temp, 3);
	for( counter=0; temp[counter] != 0 && counter < 3 ; counter++) aFlag5[counter] = temp[counter];

	kip = strtok(NULL,",");	color = *kip;

	kip = strtok(NULL,","); iX = (float)atof(kip);
	kip = strtok(NULL,","); iY = (float)atof(kip);
	kip = strtok(NULL,","); jX = (float)atof(kip);
	kip = strtok(NULL,","); jY = (float)atof(kip);

	kip = strtok(NULL,","); dummy = atoi(kip);

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
	return true;
}

#define SAFE_CPY(A,B) strncpy(A ,B , sizeof((A))-1 ); A[sizeof(A)-1]=0; trim(A);
////////////////////////////////////////////////////////////////////////////////
//	１ライン分を読み込む（Ver4形式）
////////////////////////////////////////////////////////////////////////////////
bool INTLinkV2::ReadAsV4(char* buf) {
    char* pdata[46];
	char* temp = csv_parser(buf, pdata, 45, ',', '.'); //buf の内部を破壊する

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
//	１ライン分を読み込む（Ver1形式）
////////////////////////////////////////////////////////////////////////////////
bool INTLinkV2::ReadAsV1(char* buf){

	substring(name ,buf, 0, 5);	trim(name ,6);
	substring(sNode,buf, 5,10);	trim(sNode,6);
	substring(eNode,buf,10,15);	trim(eNode,6);
////////////////
	length = getbufFlt(buf,15,5); //if(errno==ERANGE) return false;
	Vmax = getbufFlt(buf,20,5);   //if(errno==ERANGE) return false;
	Capa = getbufFlt(buf,25,8);   // Capa > 0 のはずだが
	QV	 = getbufInt(buf,33,2);
	for(int i = 0; i < 5 ; i++ ) {
		fare[i] = getbufFlt(buf,35 + i * 5, 5);
//		if(errno==ERANGE) return false;
	}
	char c;
	for(int i=0; i < 5; i++) {
		c = buf[60+i];	//1, 2, 3 以外は全て０とする。
		if( c != '1' && c != '2' && c != '3') c = '0';
		ways[i] = c;
	}

	linktype  = buf[65];
	if( buf[66] == '1' ) evaluation = false; else evaluation = true;
	display = buf[67];
	aFlag1	= buf[68];
	color = buf[69];
	iX	  = getbufFlt(buf,70, 5);
	iY	  = getbufFlt(buf,75, 5);
	jX	  = getbufFlt(buf,80, 5);
	jY	  = getbufFlt(buf,85, 5);
	return(true);
}
////////////////////////////////////////////////////////////////////////////////
//  リンクデータの書き込み（１行分）
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
//	CSV書き込み
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
//	方向制限のチェック
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
	memset(errmsg,'\0', 128);
	memset(comment, 0, 256);
}

StradaINT::~StradaINT(){
}
/////////////////////////////////////////////////////////////////////////////
//  リンクリストが空である場合にn_link分のリンクを作成
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
    } catch( std::bad_alloc& e ) {
		links.clear();
        return false;
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// CSV形式の場合、データ長が256を超える可能性がある。
/////////////////////////////////////////////////////////////////////////////
bool StradaINT::Read(FILE* fp)
{
	using namespace std;

	int nrlink;
	int nrnode;
	int c;
	char* kip;

	if( fgets(buff,INT_BUF,fp) == NULL ) {
		sprintf(errmsg, "INT2 cannot read 1st line of buffer");
		return false;
	}
	if( strncmp(buff, "INT", 3 ) != 0 ) {
		sprintf(errmsg, "INT2 Header line error");
		return false;
	}
    version = buff[3] - '0' ;
	csv = ( buff[4] == '*' ) ? true : false;

    if( version != 1 && version != 2 && version != 4 ) {
        sprintf(errmsg, "Version error");
        return false;
    }
    if( version == 4 ) csv = true;

    buff[strlen(buff)-1]=0;
    memset(comment, 0, 256);
    if( version < 4 ) {
        sprintf(comment, &buff[5], 256);
    } else {
        sprintf(comment, &buff[7], 256);
	}
	if( fgets(buff,INT_BUF,fp) == NULL ) {
		sprintf(errmsg, "INT2 Cannot read 2nd line of buffer");
		return false;
	}

	try {
		c = 0;
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
			kip = strtok(buff, ","); KIP_ERROR; nrlink = atoi(kip);
			kip = strtok(NULL, ","); KIP_ERROR; nrnode = atoi(kip);
			kip = strtok(NULL, ","); KIP_ERROR; coordinate = atoi(kip);
		}
	} catch(const std::exception& ) {
		sprintf(errmsg, "INT2  LINK=%d NODE=%d COORDINATE=%d", nrlink, nrnode, coordinate);
		return false;
	}
	//行数を数える
	c = 0;
	while( fgets(buff, INT_BUF, fp) != NULL ){
		c++;
	}
	if( c == 0 ) {
		sprintf(errmsg, "INT2 There is no link field");
		return false;
	}
	for(int i=0; i < c; i++) {
		links.push_back(new INTLinkV2());
	}

//	AList link_list;
//	ListIterator iter;
	set<string> node_table;
	set<string> link_table;

	rewind(fp);
	fgets(buff, INT_BUF, fp);
	fgets(buff, INT_BUF, fp);
	bool check = true;
	while( fgets(buff, INT_BUF, fp) != NULL ){
		LinkPtr link = new INTLinkV2();
		if( version == 2 ) {
			if( csv ) {
				if( link->ReadCSV(buff) == false ) {
					sprintf(errmsg, "INT2 Link format incorrect");
					check = false;
					break;
				}
			} else if( link->Read(buff) == false ) {
				sprintf(errmsg, "INT2 Link format incorrect");
				check = false;
				break;
			}
		}
		else if ( version == 4 ) {
			if( link->ReadAsV4(buff) == false ) {
				sprintf(errmsg,"INT4 Link format incorrect");
				check = false;
				break;
			}
		} else {
			if( link->ReadAsV1(buff) == false ) {
				sprintf(errmsg, "INT2 Link format ver 1 incorrect");
				check = false;
				break;
			}
		}
		links.push_back(link);
		if( link_table.find(link->name) != link_table.end()) {
			sprintf(errmsg, "INT2 Link name duplication %s", link->name);
			check = false;
			break;
		}
		link_table.insert(link->name);
		node_table.insert(link->sNode);
		node_table.insert(link->eNode);
	}
//    printf("%d %d\n", a, c);
	if( check == false) {
		links.clear();
		return false;
	}
	nLink = c;
	//始点と終点が同じ名前のリンクを削除する。
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
	memset(errmsg,'\0', 128);
	if( nLink != nrlink) {
		sprintf(errmsg, "No. of links changed [%5d] -> [%5d]\n",nrlink, nLink);
	}
	if( nrnode != (int)node_table.size() ) {
		sprintf(buff, "No. of nodes changed [%5d] -> [%5d]\n",nrnode,node_table.size());
		nNode = node_table.size();
		strcat(errmsg, buff);
	} else nNode = nrnode;

	return true;
}
//----------------------------------------------------------------------------
//!	ファイル名を指定して読み込み
//----------------------------------------------------------------------------
void StradaINT::Read(const char* file_name){
	FILE* fp;
	if((fp = fopen(file_name ,"rt"))==NULL) {
        std::string str1("Cannot open INT: ");
        std::string fname = file_name;
        throw std::runtime_error(str1 + fname);
    }
	bool ret = Read(fp);
	fclose(fp);

	if( ret == false ) throw std::runtime_error(errmsg);
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

    fprintf(fp,"%s\n",comment);
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
//	ファイル名を指定して書き込み
////////////////////////////////////////////////////////////////////////////////
void StradaINT::Write(char* file_name){
	FILE* fp;
	if((fp = fopen(file_name ,"wt"))==NULL) throw std::runtime_error("INT");
	Write(fp);
	fclose(fp);
}
// 最初の値はi=0
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
// 数学座標とスクリーン座標の変換を行う
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
// X1 Y1 X2 Y2 のテキストファイルから仮リンクファイルを作成する
////////////////////////////////////////////////////////////////////////////////
int StradaINT::LinkXYRead(char* f_name) {

	FILE* fp;
	float x_1, x_2, y_1, y_2;

	if( links.size() > 0 ) return -1;	//初期化していない。

	if((fp = fopen(f_name ,"rt"))==NULL) return -2;

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
		sprintf(nodes[i].name, "N%05d", i);
		nNode++;
		for(int j=i+1; j < 2*nLink; j++) {
			if(nodes[j].name[0] == 'N' ) continue;
			if( nodes[i].X == nodes[j].X && nodes[i].Y == nodes[j].Y ) {
				sprintf(nodes[j].name, "N%05d", i);
			}
		}
	}
	for(int i=0; i < nLink; i++){
		LinkPtr link = new INTLinkV2();
		sprintf(link->name, "L%05d", i);
		strncpy(link->sNode, nodes[2*i].name,  10);
		strncpy(link->eNode, nodes[2*i+1].name,10);
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
//! 名前で並び替え
//----------------------------------------------------------------------------
void StradaINT::sort() {
	links.sort();
}
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//  境界線の取得
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

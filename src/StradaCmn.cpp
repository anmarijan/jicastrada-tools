/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
/*----------------------------------------------------------------------------*/
#include "tool.h"
#include "StradaCmn.h"
//---------------------------------------------------------------------------
void print_header(FILE* fp, const char* str){

	time_t timer;
	struct tm *lt;

	timer = time(NULL);
	lt = localtime(&timer);

	fprintf(fp ,"%s %04d-%02d-%02d %02d:%02d:%02d\n", str,
			lt->tm_year+1900, lt->tm_mon+1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);

}
/*----------------------------------------------------------------------------*/
int line_cord_check(FILE* fp){
	int ret = 0;
	char buff[100];
	int x1,x2, y1, y2;
	long int curpos = ftell(fp);	//ファイルポインタを保存
	if( curpos == -1L ) return -1;
	for(int i=0; i < 36000; i++) {
		if( fgets(buff, 100, fp) == NULL ) break;
		buff[40] = '\0';
		if( sscanf(buff, "%10d%10d%10d%10d", &x1, &y1, &x2, &y2) == 0 ) {
			if( fseek(fp, curpos, SEEK_SET) != 0 ) return -2;
			return -3;
        }
		ret++;
	}
	if( fseek(fp, curpos, SEEK_SET) != 0 ) return -2;
	return ret;
}
/*----------------------------------------------------------------------------*/
SNodeV2::SNodeV2(){
	memset(name, 0, 11);
}
/*----------------------------------------------------------------------------*/
SLink::SLink(){
	memset(name ,0, 6);
	memset(sNode,0, 6);
	memset(eNode,0, 6);
	iX = iY = jX = jY = 0;
	Q = 0;
	distance = Vmax = 0;
	code = 1;
	evflag = dspflag = '0';
	userflag = ' ';
	route = ' ';
}
bool SLink::check_node(char* s_node, char* e_node){
	if( ( strcmp(s_node, sNode ) == 0 && strcmp(e_node, eNode ) == 0 ) ||
		( strcmp(s_node, eNode ) == 0 && strcmp(e_node, sNode ) == 0 )
	) return true;
	return false;
}
/*----------------------------------------------------------------------------*/
SLinkV2::SLinkV2(){
	memset(name, 0, 11);
	memset(sNode,0, 11);
	memset(eNode,0, 11);
	length = 1.0; Vmax = 100; Capa = 10000; QV = 0;
	for(int i=0; i < 10; i++){
		ways[i] = '0';
	}
	linktype = '0';
	display =  '0'; aFlag1 = ' ';
	nFlag2 = nFlag3 = 0;
	aFlag4[0] = aFlag4[1] = aFlag5[0] = aFlag5[1] = aFlag5[2] = ' ';
	iX = iY = jX = jY = 0;
	dummy = 0;
	for(int i=0; i < 3; i++) {
		dX[i] = dY[i] = 0;
	}
	route = ' ';
}
SLinkV2& SLinkV2::operator=(const SLinkV2& obj){
	strcpy(name, obj.name);
	strcpy(sNode, obj.sNode);
	strcpy(eNode, obj.eNode);
	length = obj.length;
	Vmax = obj.Vmax;
	Capa = obj.Capa;
	QV = obj.QV;
	for(int i=0; i < 10 ; i++) ways[i] = obj.ways[i];
	linktype = obj.linktype;
	evaluation = obj.evaluation;
	display = obj.display;
	aFlag1 = obj.aFlag1;
	nFlag2 = obj.nFlag2;
	nFlag3 = obj.nFlag3;
	for(int i=0; i < 2; i++) aFlag4[i] = obj.aFlag4[i];
	for(int i=0; i < 3; i++) aFlag5[i] = obj.aFlag5[i];
	iX = obj.iX;
	iY = obj.iY;
	jX = obj.jX;
	jY = obj.jY;
	dummy = obj.dummy;
	for(int i=0; i < 3; i++) {
		dX[i] = obj.dX[i];
		dY[i] = obj.dY[i];
	}
	return (*this);
}
bool SLinkV2::check_node(char* s_node, char* e_node){
	if( ( strcmp(s_node, sNode ) == 0 && strcmp(e_node, eNode ) == 0 ) ||
		( strcmp(s_node, eNode ) == 0 && strcmp(e_node, sNode ) == 0 )
	) return true;
	return false;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
bool SLinkV2::setFlag(int m, int j){
	assert( m >= 0 && m < 10 );
	assert( j >= 0 && j < 4 );
	char c[] = { '0', '1', '2', '3' };
	if( ways[m] == c[j] ) return false;
	ways[m] = c[j];
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//	方向規制の値を返す
////////////////////////////////////////////////////////////////////////////////
int SLinkV2::getFlag(int m){
	int ret;
	if( m < 0 || m >= 10 ) ret = 3;
	else ret = (int)(ways[m] - '0');
	return ret;
}
////////////////////////////////////////////////////////////////////////////////
// Clear dummy nodes 2011/10/22 18:07:01
////////////////////////////////////////////////////////////////////////////////
void SLinkV2::clear_dummy_nodes() {
	dummy = 0;
	for(int i=0; i < 3; i++) {
		dX[i] = dY[i] = 0;
	}
}
////////////////////////////////////////////////////////////////////////////////
// Node の始点と終点を入れ替える
////////////////////////////////////////////////////////////////////////////////
void SLinkV2::swap_nodes() {
	char temp[11];
	float s;
	float cx[3];
	float cy[3];
	strcpy(temp, sNode);
	strcpy(eNode,sNode);
	strcpy(sNode,temp );
	s = iX; iX = jX; jX = s;
	s = iY; iY = jY; jY = s;
	for(int i=0; i < dummy; i++) {
		cx[i] = dX[i];
		cy[i] = dY[i];
	}
	for(int i=0; i < dummy; i++) {
		dX[i] = cx[dummy-1-i];
		dY[i] = cy[dummy-1-i];
	}
}
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
/*----------------------------------------------------------------------------*/
#define __STDC_WANT_LIB_EXT1__ 1
#include "tool.h"
#include "StradaCmn.h"
//---------------------------------------------------------------------------
void print_header(FILE* fp, const char* str){

	time_t timer;
	struct tm lt;
	bool result = true;
	timer = time(NULL);
	#if defined(_MSC_VER) || defined(__MINGW32__)
	errno_t error = localtime_s(&lt,&timer);
	if( error != 0) result = false;
	#else
	if( localtime_s(&timer,&lt) == NULL) result = false;
	#endif
	if (result) {
		fprintf(fp, "%s %04d-%02d-%02d %02d:%02d:%02d\n", str,
			lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);
	}
	else {
		fprintf(fp, "%s\n", str);
	}
}
/*----------------------------------------------------------------------------*/
int line_cord_check(FILE* fp){
	int ret = 0;
	char buff[100];
	int x1,x2, y1, y2;
	long int curpos = ftell(fp);
	if( curpos == -1L ) return -1;
	for(int i=0; i < 36000; i++) {
		if( fgets(buff, 100, fp) == NULL ) break;
		buff[40] = '\0';
		if( sscanf_s(buff, "%10d%10d%10d%10d", &x1, &y1, &x2, &y2) == 0 ) {
			if( fseek(fp, curpos, SEEK_SET) != 0 ) return -2;
			return -3;
        }
		ret++;
	}
	if( fseek(fp, curpos, SEEK_SET) != 0 ) return -2;
	return ret;
}
/*----------------------------------------------------------------------------*/
std::string trim(const std::string& str) {
	std::string result;
	const char* spaces = " \t";
	std::string::size_type left = str.find_first_not_of(spaces);
	if (left != std::string::npos) {
		std::string::size_type right = str.find_last_not_of(spaces);
		result = str.substr(left, right - left + 1);
	}
	return result;
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
	strcpy_s(name, sizeof(name), obj.name);
	strcpy_s(sNode, sizeof(sNode),obj.sNode);
	strcpy_s(eNode, sizeof(eNode),obj.eNode);
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
//	Return direction flag number
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
// Swap start and end points
////////////////////////////////////////////////////////////////////////////////
void SLinkV2::swap_nodes() {
	char temp[21];
	float s;
	float cx[3];
	float cy[3];
	strcpy_s(temp, sizeof(temp), sNode);
	strcpy_s(eNode,sizeof(eNode),sNode);
	strcpy_s(sNode,sizeof(sNode),temp );
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

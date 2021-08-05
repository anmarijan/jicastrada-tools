//---------------------------------------------------------------------------
#ifndef StradaCmnH
#define StradaCmnH
//---------------------------------------------------------------------------
#include <stdio.h>
#include <string>
//---------------------------------------------------------------------------
#ifndef MAXPATH
#define MAXPATH 260
#endif
typedef unsigned char byte;
//---------------------------------------------------------------------------
void print_header(FILE* fp, const char* str);
//Check the format of "x1 y1 x2 y2" and count the number of lines
// Width of a column is 10, the maximum lines is 36000
int line_cord_check(FILE* fp);
//---------------------------------------------------------------------------
std::string trim(const std::string& str);
//---------------------------------------------------------------------------
class SNodeV2 {
public:
	char	name[11];
    float 	X, Y;
    SNodeV2();
};
// common part of INT and IRE
class SLink {
public:
	char	name[6];
	char	sNode[6];
	char	eNode[6];
	float	distance;
	float	Vmax;
	int		Q;
	char	code;
	char	evflag;
	char	dspflag;
	char	userflag;
	int		iX;
	int		iY;
	int		jX;
	int		jY;
	char	route;
public:
	SLink();
	bool check_node(char* s_node, char* e_node);
};
// Link structure (INT, IRE) Ver 2.0
class SLinkV2 {
public:
	char name[11];
	char sNode[11];
	char eNode[11];

	float	length;
	float	Vmax;
	float	Capa;
	byte 	QV;
	char	ways[10];	//!0:i<->j, 1:i<-j, 2:i->j, 3:><
	char	linktype;	//!0:general, 1:highway, 2:railway
	bool	evaluation;	//!1:NOT evaluate
	char	display;

	char	aFlag1;
	byte	nFlag2;
	byte	nFlag3;
	char	aFlag4[2];
	char	aFlag5[3];
	float	iX;
	float	iY;
	float	jX;
	float	jY;
	byte 	dummy;
	float	dX[3];
	float	dY[3];

	char	route;	//flag of shortest path

	SLinkV2();
	SLinkV2& operator=(const SLinkV2& obj);
	bool check_node(char* s_node, char* e_node);
	int getFlag(int m);
	bool setFlag(int m, int i);
	void clear_dummy_nodes();
	void swap_nodes();
};

#endif

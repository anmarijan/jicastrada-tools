//---------------------------------------------------------------------------
#ifndef StradaCmnH
#define StradaCmnH
//---------------------------------------------------------------------------
#include <stdio.h>
//---------------------------------------------------------------------------
#ifndef MAXPATH
#define MAXPATH 260
#endif
typedef unsigned char byte;
//---------------------------------------------------------------------------
void print_header(FILE* fp, const char* str);

//x1 y1 x2 y2 形式のテキストファイルの検査と行数の数え上げ
//各項目は10桁ずつで、最大読み込みは36000行とする。
int line_cord_check(FILE* fp);
//---------------------------------------------------------------------------
class SNodeV2 {
public:
	char	name[11];
    float 	X, Y;
    SNodeV2();
};
//INTとIREで共通になる部分
class SLink {
public:
	char	name[6];	//当該リンクの名前
	char	sNode[6];
	char	eNode[6];
	float	distance;
	float	Vmax;
	int		Q;
	char	code;		//数。読込文字
	char	evflag;		//経済評価
	char	dspflag;	//画面表示用
	char	userflag;
	int		iX;
	int		iY;
	int		jX;
	int		jY;
	char	route;	//最短経路表示用のフラグ
public:
	SLink();
	bool check_node(char* s_node, char* e_node);
};
//! リンク構造（INT, IRE共通）Ver 2.0
//  パディングしているので順序重要
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

	char	route;	//最短経路表示用のフラグ

	SLinkV2();
	SLinkV2& operator=(const SLinkV2& obj);
	bool check_node(char* s_node, char* e_node);
	int getFlag(int m);
	bool setFlag(int m, int i);
	void clear_dummy_nodes();
	void swap_nodes();
};

#endif

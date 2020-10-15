//---------------------------------------------------------------------------
#ifndef StradaDREH
#define StradaDREH
//---------------------------------------------------------------------------
#include <stdlib.h>
//---------------------------------------------------------------------------
/*
class NodeOD {
public:
	char name[11];
	int  data[4][4];
	char s_nodes[4][11];
	char e_nodes[4][11];
public:
	NodeOD();
};
*/
class NodeDirection {
public:
	char name[11];
	char s_node[11];
	char e_node[11];
	char s_link[11];
	char e_link[11];
	int total;
	int vol[10];
	float sx, sy, nx, ny, ex, ey;
public:
	NodeDirection();
	int Read( FILE* fp);
	void Write( FILE* fp);
};

class StradaDRE {
public:
	int nDirection;
	int nNode;
	int nMode;
	int coordinate;
	NodeDirection* direc;
//	  NodeOD* nod;
public:
	StradaDRE();
	~StradaDRE();
	int Read(FILE* fp);
    void Read(char* fname);
	void Write(FILE* fp);
	void Write(const char* fname);
};


#endif

//---------------------------------------------------------------------------
#ifndef StradaDREH
#define StradaDREH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <vector>
//---------------------------------------------------------------------------
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
	void Write( FILE* fp) const;
};

class StradaDRE {
public:
	int nNode;
	int nMode;
	int coordinate;
	std::vector<NodeDirection> direc;
public:
	StradaDRE();
	~StradaDRE();
	int Read(FILE* fp);
    void Read(char* fname);
	void Write(FILE* fp);
	void Write(const char* fname);
};


#endif

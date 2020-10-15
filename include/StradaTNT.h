//---------------------------------------------------------------------------
#ifndef StradaTNTH
#define StradaTNTH
//---------------------------------------------------------------------------
#include <list>
#include <vector>
#include "StradaCmn.h"
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// トランジットラインの各ノード
/////////////////////////////////////////////////////////////////////////////
struct TRNNode {
    char access;
    char name[11];
};
/////////////////////////////////////////////////////////////////////////////
// トランジットライン
/////////////////////////////////////////////////////////////////////////////
class TRNLine {
public:
	int id;
	char name[11];
	char flag;
	short mode;
	short way;
	int frequency;
	int speed ;
	int nNode ;
	std::list<TRNNode> nodes;
public:
    TRNLine();
    bool ReadCSV(char* buff);
    void Write(FILE* fp);
    void clone(TRNLine* ptr);
};
/////////////////////////////////////////////////////////////////////////////
// TNT ver 2.0
/////////////////////////////////////////////////////////////////////////////
class StradaTNT {
public:
	int		nLine;
	int		nMode;
	std::vector<TRNLine> lines;
public:
	StradaTNT();
	int Read(FILE *fp);
    void Read(const char* file_name);
    void ReadCSV(FILE* fp);
	void Write(char* fname);
	void Write(FILE* fp);
    TRNLine* getLine(int i);
    int get_route_count();
    int get_node_count();
};

#endif
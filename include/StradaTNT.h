//---------------------------------------------------------------------------
#ifndef StradaTNTH
#define StradaTNTH
//---------------------------------------------------------------------------
#include <list>
#include <vector>
#include <string>
#include "StradaCmn.h"
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// Node of transit line
/////////////////////////////////////////////////////////////////////////////
struct TRNNode {
    char access;
    char name[11];
};
/////////////////////////////////////////////////////////////////////////////
// Transit Line
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
    bool ReadCSV(const std::string& str);
    void Write(FILE* fp);
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
    void Read(const char* file_name);
	void Write(char* fname);
	void Write(FILE* fp);
    TRNLine* getLine(int i);
    int get_route_count();
    int get_node_count();
};

#endif

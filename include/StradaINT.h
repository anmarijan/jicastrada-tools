#ifndef StradaINTH
#define StradaINTH
#include <cstring>
#include <string>
#include <list>
#include <boost/intrusive_ptr.hpp>
#include "StradaCmn.h"
//----------------------------------------------------------------------------
// Link (Ver 2)
//----------------------------------------------------------------------------
class INTLinkV2 : public SLinkV2
{
	int ref_counter;
public:
	float	fare[10];	//
	char	color;		//
	bool	bLinkOD;	// Link OD flag
	bool	bRouteInf;	// Route information flag

public:
	INTLinkV2();
	INTLinkV2(SLinkV2& s);
	INTLinkV2& operator=(const INTLinkV2& obj);

	bool checkway(int m,int j);	//i=0: sNode->eNode, i=1: eNode->sNode
	int way();  // A->B = 1, B->A = -1, both = 0
	void set_attr(INTLinkV2& link);
	void get_boundary(double &x1, double &y1, double &x2, double &y2);

	bool Read(const char* buff, size_t &pos);
	bool ReadAsV1(const char* buf);	//Ver1.0 format
	bool ReadCSV(char* buff);
    bool ReadAsV4(char* buff);
	void Write(FILE* fp);
    void WriteCSV(FILE* fp, int varsion);

	friend void intrusive_ptr_add_ref(INTLinkV2* p);
	friend void intrusive_ptr_release(INTLinkV2* p);
	// for sort by name
	bool operator < (const INTLinkV2& c_link) const
    {
	    bool check = false;
		if( strcmp(name, c_link.name ) < 0 ) check = true;
    	return check;
    }
};
using LinkPtr = boost::intrusive_ptr<INTLinkV2>;
//----------------------------------------------------------------------------
// INT file class
//----------------------------------------------------------------------------
class StradaINT
{
	std::string errmsg;
public:
	bool csv;		//
	int version;	//
	int nLink;		//
	int nNode;		//
	int coordinate;	//(0:screen, 1:mathematical, 2:latitude,longitude)

    std::string comment;
	std::list<LinkPtr> links;	// Array  of link pointer

	StradaINT();
	~StradaINT();
	void clear();
    void sort();
	bool Read(FILE* fp);
	void Read(const char* file_name);
	void Write(FILE* fp);
    void Write(char* file_name);
    const char* msg() {return errmsg.c_str(); };

	LinkPtr getLink(int i);
    bool alloc_links(int n_link);
	void conv(int cd, int mergin);
	int LinkXYRead(char* f_name);
};

#endif

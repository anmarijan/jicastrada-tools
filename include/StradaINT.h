#ifndef StradaINTH
#define StradaINTH

#include "StradaCmn.h"
#include <cstring>
#include <list>
#include <boost/intrusive_ptr.hpp>
//----------------------------------------------------------------------------
//! ネットワークのリンク (Ver 2)
//----------------------------------------------------------------------------
class INTLinkV2 : public SLinkV2
{
	int ref_counter;
public:
	float	fare[10];	//!< 料金
	char	color;		//!< 色設定
	bool	bLinkOD;	//!< リンクのODを記録するか否か
	bool	bRouteInf;	//経路情報を記録するか否か

public:
	INTLinkV2();
	INTLinkV2(SLinkV2& s);
	INTLinkV2& operator=(const INTLinkV2& obj);

	bool checkway(int m,int j);	//i=0：順方向、i=1：逆方向
	int way();  // A->B = 1, B->A = -1, どちらでもなれば0
	void set_attr(INTLinkV2& link);
	/*
	void setRoute(bool chk) {bRouteInf = chk;};
	void setODDetail(bool chk) {bLinkOD = chk;};

	bool getRoute() { return bRouteInf ;}
	bool getODDetail() { return bLinkOD; }
	*/
	void get_boundary(double &x1, double &y1, double &x2, double &y2);

	bool Read(char* buff);
	bool ReadAsV1(char* buf);	//Ver1.0形式のラインを読み込む
	bool ReadCSV(char* buff);
    bool ReadAsV4(char* buff);
	void Write(FILE* fp);
    void WriteCSV(FILE* fp, int varsion);

	friend void intrusive_ptr_add_ref(INTLinkV2* p);
	friend void intrusive_ptr_release(INTLinkV2* p);
	// 名前で並べ替えるようにするため
	bool operator < (const INTLinkV2& c_link) const
    {
	    bool check = false;
		if( strcmp(name, c_link.name ) < 0 ) check = true;
    	return check;
    }
};
using LinkPtr = boost::intrusive_ptr<INTLinkV2>;
//----------------------------------------------------------------------------
//! リンクファイルを扱うクラス
//----------------------------------------------------------------------------
class StradaINT
{
	char errmsg[128];	//!< エラー発生時のメッセージ
public:
	bool csv;		//!< csv形式か否か
	int version;	//!< 読み込み時のバージョン
	int nLink;		//!< リンク数
	int nNode;		//!< ノード数
	int coordinate;	//!< 座標設定 (0:screen, 1:mathematical, 2:latitude,longitude)

    char comment[256];
	std::list<LinkPtr> links;	//!< リンクポインタ配列

	StradaINT();
	~StradaINT();
	void clear();
    void sort();
	bool Read(FILE* fp);
	void Read(const char* file_name);//始点と終点の名称が同じ場合、最後に削除
	void Write(FILE* fp);
    void Write(char* file_name);
    char* msg() {return errmsg; };

	LinkPtr getLink(int i);
    bool alloc_links(int n_link);
	void conv(int cd, int mergin);
	int LinkXYRead(char* f_name);
};

#endif

//---------------------------------------------------------------------------
#ifndef StradaAODH
#define StradaAODH
//---------------------------------------------------------------------------
#include "StradaCmn.h"
#include <memory>
#include <string>
#include <vector>
//---------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
/// OD Table
////////////////////////////////////////////////////////////////////////////////
class ODMatrix {
public:
	std::string name;	//!ＯＤ表の名前
	std::unique_ptr<double[]> data;
	int nZone;
	ODMatrix();
	ODMatrix(int n);
	ODMatrix(const char* str_name, int n);
	void init(int n);
};
//! OD表ファイルを扱うクラス
class StradaAOD {
public:
	int version;
	int nZone;
	int nTable;
	int type;
	bool csv;
    bool zeroskip; //保存の際にゼロデータを保存しない
	std::string name;
	std::vector<ODMatrix> tables;
public:
	StradaAOD();
	StradaAOD(const StradaAOD &aod);
	StradaAOD(int nTable, int nZone);
	void allocTable(int nTable,int nZone);
	void Read(const char* fname);
	void Write(FILE* fp);
	void Write(const char* fname);
	void clear();
public:
	double getOD(int tbl,int org,int dst);
	double getSumOD(int org, int dst);
	void setOD(int tbl,int org,int dst,double number);
	void plusOD(int tbl,int org,int dst,double number);
	void multOD(int tbl, double x);  //tblの表の数字を全てx倍する
	void setTableName(int tbl, const char* name);
	double getMaxOD(int tbl);  //各テーブル内で最大の値
	double getMaxOD();			//全てのテーブルの中で最大の値
    void clearOD();           //全てのデータをゼロにする
private:
	int colnum;
	int colwid;
	int decwid;
	char data_type;
	int FTFormat(char* str);
};
#endif

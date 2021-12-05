//---------------------------------------------------------------------------
#ifndef calcaodH
#define calcaodH
//---------------------------------------------------------------------------
#include "StradaAOD.h"
#include <string>
//---------------------------------------------------------------------------
class CalcAOD {
public:
    std::string errmsg;
private:
    StradaAOD aod1;
    StradaAOD aod2;
    StradaAOD aod3;

public:
	int nTable = 0;
	int nZone = 0;
    bool ReadStradaAOD(int n, char* fname);
    void WriteStradaAOD(int n, char* fname);
    void plus();
    void minus();
	void sum();  //aod1の全てのテーブルを合計した値をaod3とする。
	double multiod(const char* iod_fname, double output[]);
    int growth(const char* fname, int flag);    //ベクトルを掛ける。flag=0:発生ゾーン別、flag=1:集中ゾーン別
	void zero_inner(double a); //内々トリップを全てゼロにする。
    void zero_plus();
    bool calc_fratar(char* fname);  //fname: GADファイルの名前
    bool make_gad(char* fname, bool csv);
	bool mult_pair(char* fname);    //掛け合わせる乗数のリスト
	void scalar(int t, double a);	//テーブルt を一律a倍する
	void insert();	//テーブルの追加
    void aodcopy();
    double getOD(int n, int t, int i, int j);	//10
    void clear(int t);	//11
    void extract(int t, bool csv = false);
    void append();	//aod1の最後にaod2を追加したものをaod3とする
    				//aod2の空の場合又はZONE数が異なる場合は、ゼロODを追加する。
    void show_total();
    void show_hs(int n);	//nゾーンの発生集中の各合計を表示
	void show_innertrip_rate();
};


#endif

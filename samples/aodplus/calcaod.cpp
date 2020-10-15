//---------------------------------------------------------------------------
#include <string.h>
#include <stdio.h>
#include <vector>
#include <stdexcept>
//---------------------------------------------------------------------------
#include "calcaod.h"
#include "fratar.h"
#include "StradaGAD.h"
#include "tool.h"

////////////////////////////////////////////////////////////////////////////////
//  ファイル読み込み
////////////////////////////////////////////////////////////////////////////////
bool CalcAOD::ReadStradaAOD(int n, char* fname)
{
    try {
        if( n == 1 ) {
			aod1.Read(fname);
			nTable = aod1.nTable;
			nZone  = aod1.nZone;
		}
        else if( n==2 ) aod2.Read(fname);
        else aod3.Read(fname);
    } catch(std::exception& e){
		fprintf(stderr, "Cannot read %s\n", fname);
        strcpy(errmsg, e.what());
        return false;
    }
    return true;
}
////////////////////////////////////////////////////////////////////////////////
//  ファイル書き込み
////////////////////////////////////////////////////////////////////////////////
void CalcAOD::WriteStradaAOD(int n, char* fname)
{
    if( n == 1 && aod1.nZone == 0 ) return;
    if( n == 2 && aod2.nZone == 0 ) return;
    if( n == 3 && aod3.nZone == 0 ) return;

    FILE* fp;
    if( (fp = fopen(fname, "wt")) != NULL )
    {
        if( n == 1 ) aod1.Write(fp);
        else if( n == 2) aod2.Write(fp);
        else aod3.Write(fp);
        fclose(fp);
    }
}
////////////////////////////////////////////////////////////////////////////////
//  二つのファイルを足し合わせる。
////////////////////////////////////////////////////////////////////////////////
void CalcAOD::plus(){

	if( aod1.nZone == 0 || aod2.nZone == 0 ) return;
	if( aod1.nZone != aod2.nZone || aod1.nTable != aod2.nTable ) return;
	if( aod3.nZone != aod1.nZone || aod3.nTable != aod1.nTable ) aod3.clear();

    int nTable = aod1.nTable;
    int nZone  = aod1.nZone;
    float data;

    aod3.allocTable(aod1.nTable, aod1.nZone);
    for(int t=0; t < nTable; t++ ) {
        aod3.setTableName(t, aod1.tables[t].name.c_str());
        for(int i=0; i < nZone; i++) {
            for(int j=0; j < nZone; j++ ) {
                data = (float)(aod1.getOD(t,i,j) + aod2.getOD(t,i,j));
                aod3.setOD(t,i,j,data);
            }
        }
    }
    aod3.csv = aod1.csv; aod3.type = aod1.type;
}
////////////////////////////////////////////////////////////////////////////////
//  二つのファイルの引き算
////////////////////////////////////////////////////////////////////////////////
void CalcAOD::minus(){

    if( aod1.nZone == 0 || aod2.nZone == 0 ) return;
    if( aod1.nZone != aod2.nZone || aod1.nTable != aod2.nTable ) return;
	//aod3は初期化される。
    if( aod3.nZone != aod1.nZone || aod3.nTable != aod1.nTable ) aod3.clear();

    int nTable = aod1.nTable;
    int nZone  = aod1.nZone;
    float data;

    aod3.allocTable(aod1.nTable, aod1.nZone);
    for(int t=0; t < nTable; t++ ) {
        aod3.setTableName(t, aod1.tables[t].name.c_str());
        for(int i=0; i < nZone; i++) {
            for(int j=0; j < nZone; j++ ) {
                data = (float)(aod1.getOD(t,i,j) - aod2.getOD(t,i,j));
                aod3.setOD(t,i,j,data);
            }
        }

    }
    aod3.csv = aod1.csv;
    aod3.type = aod1.type;
}
////////////////////////////////////////////////////////////////////////////////
//  テーブル t の数値を全て a 倍する。
//  tbl == -1 の時は全てのテーブルを対象とする。
////////////////////////////////////////////////////////////////////////////////
void CalcAOD::scalar(int tbl, double a){

	if( aod1.nZone == 0 ) return;
	if( aod3.nZone != aod1.nZone || aod3.nTable != aod1.nTable ) {
		aod3.clear();
		aod3.allocTable(aod1.nTable, aod1.nZone);
	}

    int nTable = aod1.nTable;
    int nZone  = aod1.nZone;
    double data;

    for(int t=0; t < nTable; t++ ) {
        aod3.setTableName(t, aod1.tables[t].name.c_str());
        for(int i=0; i < nZone; i++) {
            for(int j=0; j < nZone; j++ ) {
				if ( tbl == -1 || tbl == t ) {
					data = a * aod1.getOD(t,i,j);
	                aod3.setOD(t,i,j,data);
	            } else {
	                aod3.setOD(t,i,j,aod1.getOD(t,i,j));
				}
            }
        }

    }
    aod3.csv = aod1.csv;
    aod3.type = aod1.type;
}
////////////////////////////////////////////////////////////////////////////////
// 列別、又は行別にかけ算する
////////////////////////////////////////////////////////////////////////////////
int CalcAOD::growth(const char* fname, int flag) {

    FILE* fp;
    int nTable = aod1.nTable;
    int nZone = aod1.nZone;
    double data;
    std::vector<double> vec;
    char buff[256];
    int count;

    if((fp = fopen(fname, "rt"))==NULL) return (-1);

	if( aod1.nZone == 0 ) return (-2);
	if( aod3.nZone != aod1.nZone || aod3.nTable != aod1.nTable ) {
		aod3.clear();
		aod3.allocTable(aod1.nTable, aod1.nZone);
	}
    vec.reserve(nZone);
    count = 0;
    while( fgets(buff, 256, fp) ) {
        if( count == nZone ) break;
        buff[strlen(buff)-1] = '\0';
        vec[count] = atof(buff);
        count++;
    }
    fclose(fp);
    if (count < nZone) return (-3);
    for(int t=0; t < nTable; t++) {
        for(int i=0; i < nZone; i++) {
            for(int j=0; j < nZone; j++) {
                data = aod1.getOD(t,i,j);
                if( flag == 0 ) aod3.setOD(t,i,j, data*vec[i]);
                else aod3.setOD(t,i,j,data*vec[j]);
            }
        }
    }
    aod3.csv = aod1.csv;
    aod3.type = aod1.type;
    return 0;
}
////////////////////////////////////////////////////////////////////////////////
//  AOD1の全てのテーブルの値を合計する。
////////////////////////////////////////////////////////////////////////////////
void CalcAOD::sum() {
	if ( aod1.nZone == 0 ) return;
	aod3.clear();
	aod3.allocTable(1, aod1.nZone);

	int nTable = aod1.nTable;
	int nZone = aod1.nZone;
	double data;

	for(int i=0; i < nZone; i++) {
		for(int j=0; j < nZone; j++) {
			data = 0;
			for (int t=0; t < nTable; t++)
				data += aod1.getOD(t,i,j);
			aod3.setOD(0,i,j,data);
		}
	}
	aod3.csv = aod1.csv;
	aod3.type = aod1.type;
}
////////////////////////////////////////////////////////////////////////////////
//  AOD1の値が０である時のみ、AOD2の値を加える。
////////////////////////////////////////////////////////////////////////////////
void CalcAOD::zero_plus(){

    if( aod1.nZone == 0 || aod2.nZone == 0 ) return;
    if( aod1.nZone != aod2.nZone || aod1.nTable != aod2.nTable ) return;
    if( aod3.nZone != aod1.nZone || aod3.nTable != aod1.nTable ) aod3.clear();

    int nTable = aod1.nTable;
    int nZone  = aod1.nZone;
    float data1, data2;

    aod3.allocTable(nTable, nZone);
    for(int t=0; t < nTable; t++ ) {
        aod3.setTableName(t, aod1.tables[t].name.c_str());
        for(int i=0; i < nZone; i++) {
            for(int j=0; j < nZone; j++ ) {
                data1 = aod1.getOD(t,i,j);
                data2 = aod2.getOD(t,i,j);
                if( data1 == 0 ) aod3.setOD(t,i,j,data2);
                else aod3.setOD(t,i,j,data1);
            }
        }
    }
    aod3.csv = aod1.csv; aod3.type = aod1.type;
}
////////////////////////////////////////////////////////////////////////////////
//  フレーター法による計算を行う。
////////////////////////////////////////////////////////////////////////////////
bool CalcAOD::calc_fratar(char* fname)
{
	FILE* fp;
	StradaGAD gad;
	fratar frt;

    if( aod1.nZone < 2 ) return false;

	if( (fp = fopen(fname , "rt")) != NULL )
	{
		int ret = gad.Read(fp);
		if( ret < 0 ) {
            sprintf(errmsg, "gad file reading error");
			return false;
		}
		fclose(fp);
	} else {
        sprintf(errmsg, "gad file open error");
        return false;
	}

	int n_ga = 2 * aod1.nTable;

	if(gad.nZone != aod1.nZone || gad.nData != n_ga ) {
        sprintf(errmsg, "no. of zone or data does not match");
        return false;
	}
	aod3.clear();

	aod3.allocTable(aod1.nTable, aod1.nZone);
	aod3.name = aod1.name;
	for(int t=0; t < aod1.nTable; t++) {
		aod3.setTableName(t, aod1.tables[t].name.c_str());
	}
    //計算開始
	frt.init(aod1.nZone);

	for(int t=0; t < aod1.nTable; t++) {
		for(int i=0; i < aod1.nZone; i++) {
			frt.set_ga(i,(float)gad.GA[2*t].data[i],
										(float)gad.GA[2*t+1].data[i]);
			for(int j=0; j < aod1.nZone; j++) {
				frt.set_od(i,j, (float)aod1.getOD(t,i,j));
			}
		}
		int ret = frt.calc();
		if( ret < 0 ) {
			if( ret == -2) {
                sprintf(errmsg,"Convergence error :table %d", t+1);
            }
			else if(ret < -100) {
				ret = -(100+ret);
				sprintf(errmsg, "0 G/A was found in zone %d (table %d)." ,ret, t+1);
			}
			aod3.clear();
			return false;
		}

        // 計算結果を保存する
		for(int i=0; i < aod1.nZone; i++) {
			for(int j=0; j < aod1.nZone; j++ ) {
				aod3.setOD(t, i, j, (int)frt.get_od(i,j));
			}
		}
	}
    aod3.csv = aod1.csv; aod3.type = aod1.type;
    return true;
}
////////////////////////////////////////////////////////////////////////////////
//  GADファイルを作成する
////////////////////////////////////////////////////////////////////////////////
bool CalcAOD::make_gad(char* fname, bool csv) {

	FILE* fp;
	float data_g, data_a;
	StradaGAD gad;
	char buf[25]; //ODのテーブル名は半角10字まで(STRADA Mannual)

	gad.init(aod1.nZone, 2 * aod1.nTable);
	for(int i=0; i < aod1.nTable; i++) {
		sprintf(buf,"%sG", aod1.tables[i].name.c_str());
		gad.set_title(2*i, buf);
		sprintf(buf,"%sA", aod1.tables[i].name.c_str());
		gad.set_title(2*i+1, buf);
		for(int j=0; j < aod1.nZone; j++) {
			data_g = 0;
			data_a = 0;
			for(int k=0; k< aod1.nZone; k++){
				data_g += aod1.getOD(i,j,k);
				data_a += aod1.getOD(i,k,j);
			}
			gad.set_data(2*i  ,j, data_g);
			gad.set_data(2*i+1,j, data_a);
		}
	}
	gad.csv = csv;

    if( (fp = fopen(fname, "wt")) != NULL )
    {
        gad.Write(fp);
        fclose(fp);
    } else {
        sprintf(errmsg, "File (%s) creation error.", fname);
        return false;
    }
    return true;
}
////////////////////////////////////////////////////////////////////////////////
//  個別のODペアを掛け算する。
//  入力ファイル：    5    10     20
//                Zone1 Zone2 掛け率
////////////////////////////////////////////////////////////////////////////////
bool CalcAOD::mult_pair(char* fname) {
    FILE* fp;
    char buf[256];
    int nTable = aod1.nTable;
    int nZone  = aod1.nZone;
    int iz, jz;
    int length, max_t;
    float rate, data;

    if( aod1.nZone == 0 ) {
		sprintf(errmsg, "No OD Matrix");
        return false;
    }

    if( (fp = fopen(fname, "rt")) == NULL )
    {
        sprintf(errmsg, "File (%s) was not found.", fname);
        return false;
    }

    aodcopy();  //aod1の内容をaod3に複写する。

    while( fgets(buf,256, fp) != NULL) {
        length = strlen(buf);
        if( length < 21 )
        {
            sprintf(errmsg, "Input file format error (%s).",fname);
            return false;
        }
        max_t = (length - 11 ) / 10 ;
        iz = getbufInt(buf, 0, 5 );
        jz = getbufInt(buf, 5, 5 );
        if( iz <= 0 || iz > nZone || jz <= 0 || jz > nZone)
        {
            sprintf(errmsg, "Input file format error (%s).",fname);
            return false;
        }
        if( max_t > nTable ) max_t = nTable;
        for(int t=0; t < max_t; t++)
        {
            rate = getbufFlt(buf, 10 + 10*t, 10);
            data = aod1.getOD(t,iz-1, jz-1);
            data *= rate;
            aod3.setOD(t,iz-1,jz-1,data);
        }
    }
    fclose(fp);
    aod3.csv = aod1.csv; aod3.type = aod1.type;
    return true;
}
////////////////////////////////////////////////////////////////////////////////
//  AODの内容をコピーする
////////////////////////////////////////////////////////////////////////////////
void CalcAOD::aodcopy()
{
    if( aod1.nZone == 0 || aod1.nTable == 0 ) return;

    int nTable = aod1.nTable;
    int nZone  = aod1.nZone;
    float data;

    aod3.clear();

    aod3.allocTable(nTable, nZone);
    for(int t=0; t < nTable; t++ ) {
        aod3.setTableName(t, aod1.tables[t].name.c_str());
        for(int i=0; i < nZone; i++) {
            for(int j=0; j < nZone; j++ ) {
                data = aod1.getOD(t,i,j);
                aod3.setOD(t,i,j,data);
            }
        }
    }
    aod3.csv = aod1.csv; aod3.type = aod1.type;
}
/////////////////////////////////////////////////////////////////////////////
//  AOD と IODを掛け合わせて合計する（人キロを計算するときなど）
/////////////////////////////////////////////////////////////////////////////
double CalcAOD::multiod(const char* iod_fname, double output[])
{
	StradaAOD iod;
	double total = 0;
	try {
		iod.Read((char*)iod_fname);
		if( aod1.nTable != iod.nTable || aod1.nZone != iod.nZone )
			throw std::runtime_error("Number of table or zone is different.");
		for(int t= 0; t < aod1.nTable; t++ ) {
			output[t] = 0;
			for(int i=0; i < aod1.nZone; i++) {
				for(int j=0; j < aod1.nZone; j++) {
					output[t] += aod1.getOD(t,i,j) * iod.getOD(t,i,j);
				}
			}
		}
	} catch (std::exception& e) {
		printf("%s\n", e.what());
	}
	for(int t=0; t < aod1.nTable; t++) total += output[t];
	return total;
}
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void CalcAOD::insert() {
	if( aod1.nZone == 0 || aod2.nZone == 0 ) return;
	if( aod1.nZone != aod2.nZone ) return;

	float data;
	int nTable = aod1.nTable + aod2.nTable;
	int nZone = aod1.nZone;

	aod3.clear();
	aod3.allocTable(nTable, nZone);

	for(int t=0; t < aod1.nTable; t++ ) {
		aod3.setTableName(t, aod1.tables[t].name.c_str());
		for(int i=0; i < nZone; i++) {
			for(int j=0; j < nZone; j++ ) {
				data = aod1.getOD(t,i,j);
				aod3.setOD(t,i,j,data);
			}
		}
	}
	for(int t=0; t < aod2.nTable; t++ ) {
		aod3.setTableName(aod1.nTable + t, aod2.tables[t].name.c_str());
		for(int i=0; i < nZone; i++) {
			for(int j=0; j < nZone; j++ ) {
				data = aod2.getOD(t, i, j);
				aod3.setOD(aod1.nTable +t, i, j, data);
			}
		}
	}
    aod3.csv = aod1.csv; aod3.type = aod1.type;
}

float CalcAOD::getOD(int n, int t, int i, int j) {
	if( t >= aod1.nTable || i >= aod1.nZone || j >= aod1.nZone) return 0;
	if( n == 1 ) return aod1.getOD(t,i,j);
	else if( n == 2 ) return aod2.getOD(t,i,j);
	else if( n == 3 ) return aod3.getOD(t,i,j);
	return 0;
}

void CalcAOD::zero_inner(double a) {
	aod3.clear();
	aod3.allocTable(aod1.nTable, aod1.nZone);
	for(int t=0; t < aod1.nTable; t++) {
		for(int i=0; i < aod1.nZone; i++) {
			for(int j=0; j < aod1.nZone; j++) {
				if( i == j ) aod3.setOD(t,i,j,a);
				else aod3.setOD(t,i,j,aod1.getOD(t,i,j));
			}
		}
	}
    aod3.csv = aod1.csv; aod3.type = aod1.type;
}

void CalcAOD::clear(int t) {
	if( t >= aod1.nTable ) return;
	aod3.clear();
	aod3.allocTable(aod1.nTable, aod1.nZone);
	for(int tbl = 0; tbl < aod1.nTable; tbl++) {
		for(int i=0; i < aod1.nZone; i++ ) {
			for(int j=0; j < aod1.nZone; j++) {
				if( t < 0 || t == tbl)
					aod3.setOD(tbl,i,j,0);
				else
					aod3.setOD(tbl,i,j,aod1.getOD(tbl,i,j));
			}
		}
	}
}

void CalcAOD::extract(int t, bool csv) {
	if( t < 0 || t >= aod1.nTable ) return;
	aod3.clear();
	aod3.allocTable(1, aod1.nZone);
	aod3.setTableName(0, aod1.tables[t].name.c_str());

	for(int i=0; i < aod1.nZone; i++) {
		for(int j=0; j < aod1.nZone; j++) {
			aod3.setOD(0,i,j,aod1.getOD(t,i,j));
		}
	}
	if( csv ) {
		aod3.csv = true;
		aod3.type = 2;
	} else {
		aod3.csv = aod1.csv;
		aod3.type = aod1.type;
	}
}
/////////////////////////////////////////////////////////////////////////////
//  APPEND
/////////////////////////////////////////////////////////////////////////////
void CalcAOD::append() {
	int tables;
	aod3.clear();
	aod3.type = aod1.type;
	if( aod2.nTable > 0 && aod2.nZone == aod1.nZone ) {
		tables = aod1.nTable + aod2.nTable;
		aod3.allocTable(tables, aod1.nZone);
		for(int t = 0; t < aod1.nTable; t++) {
			aod3.setTableName(t, aod1.tables[t].name.c_str());
			for(int i=0; i < aod1.nZone; i++) {
				for(int j=0; j < aod1.nZone; j++) {
					aod3.setOD(t,i,j,aod1.getOD(t,i,j));
				}
			}
		}
		for(int t = 0; t < aod2.nTable; t++) {
			aod3.setTableName(aod1.nTable+t, aod2.tables[t].name.c_str());
			for(int i=0; i < aod1.nZone; i++) {
				for(int j=0; j < aod1.nZone; j++) {
					aod3.setOD(aod1.nTable+t,i,j,aod2.getOD(t,i,j));
				}
			}
		}
	} else {
		aod3.allocTable(aod1.nTable+1, aod1.nZone);
		for(int t = 0; t < aod1.nTable; t++) {
			aod3.setTableName(t, aod1.tables[t].name.c_str());
			for(int i=0; i < aod1.nZone; i++) {
				for(int j=0; j < aod1.nZone; j++) {
					aod3.setOD(t,i,j,aod1.getOD(t,i,j));
				}
			}
		}
		for(int i=0; i < aod1.nZone; i++) {
			for(int j=0; j < aod1.nZone; j++) {
				aod3.setOD(aod1.nTable,i,j,0);
			}
		}
	}
	aod3.csv = aod1.csv;
}

void CalcAOD::show_total() {
	for(int t=0; t < aod1.nTable; t++) {
		float v = 0;
		for(int i=0; i < aod1.nZone; i++) {
			for(int j=0; j < aod1.nZone; j++) {
				v += aod1.getOD(t,i,j);
			}
		}
		printf("%5d%10.0f\n",t+1,v);
	}
}

void CalcAOD::show_hs(int n) {
	double h, s;
	if ( n > aod1.nZone-1 ) {
		printf("No. of zones: %d\n", aod1.nZone);
		return;
	}
	printf("Table     Generation  Attraction\n");
	for(int t=0; t < aod1.nTable; t++) {
		h = 0;
		s = 0;
		for(int i=0; i < aod1.nZone; i++) {
			h += aod1.getOD(t, n, i);
			s += aod1.getOD(t, i, n);
		}
		printf("%10d%10.0lf  %10.0lf\n", t, h , s);
	}
}

void CalcAOD::show_innertrip_rate() {
	double** data;
	double inner_total = 0;
	double trip_total = 0;
	int nCol = (aod1.nTable + 1) * 2 ;
	data = new double*[nCol];
	for(int i=0; i < nCol; i++) data[i] = new double[aod1.nZone+1];
// 最後は合計欄

	for(int t=0; t < aod1.nTable; t++) {
		double sum_inner=0;
		double sum_gen = 0;
		double sum_att = 0;
		for(int i=0; i < aod1.nZone; i++) {
			double inner = aod1.getOD(t,i,i);
			double gen = 0;
			double att = 0;
			sum_inner += inner;
			for(int j=0; j < aod1.nZone; j++) {
				gen += aod1.getOD(t,i,j);
				att += aod1.getOD(t,j,i);
			}
			sum_gen += gen;
			sum_att += att;
//			printf("%d %d %f %f %f\n", t, i, inner, gen, att);
			if (gen > 0) data[2*t][i] = inner/gen*100;
			else data[2*t][i] = 0;
			if (att > 0) data[2*t+1][i] = inner/att*100;
			else data[2*t+1][i] = 0;
		}
		if (sum_gen > 0)
			data[2*t][aod1.nZone] = sum_inner/sum_gen * 100;
		else
			data[2*t][aod1.nZone] = 0;
		if (sum_att > 0 )
			data[2*t+1][aod1.nZone] = sum_inner/sum_att *100;
		else
			data[2*t+1][aod1.nZone] = 0;
	}

	for(int i=0; i < aod1.nZone; i++) {
		double inner = 0;
		double gen = 0;
		double att = 0;
		for(int t = 0; t < aod1.nTable; t++) {
			inner += aod1.getOD(t,i,i);
			for(int j=0; j < aod1.nZone; j++) {
				gen += aod1.getOD(t,i,j);
				att += aod1.getOD(t,j,i);
			}
		}
		if (gen > 0 ) data[2*aod1.nTable][i] = inner/gen*100;
		else data[2*aod1.nTable][i] = 0;
		if (att > 0 ) data[2*aod1.nTable+1][i] = inner/att*100;
		else data[2*aod1.nTable+1][i] = 0;
	}

	for(int t=0; t < aod1.nTable; t++) {
		for(int i=0; i < aod1.nZone; i++) {
			inner_total += aod1.getOD(t,i,i);
			for(int j=0; j < aod1.nZone; j++) {
				trip_total += aod1.getOD(t,i,j);
			}
		}
	}
	data[2*aod1.nTable][aod1.nZone] = inner_total/trip_total * 100;
	data[2*aod1.nTable+1][aod1.nZone] = inner_total/trip_total * 100;
	for (int i=0; i < aod1.nZone; i++) {
		printf("%5d", i+1);
		for(int t=0; t < aod1.nTable+1; t++) {
			printf("%8.3f%8.3f", data[2*t][i], data[2*t+1][i]);
		}
		printf("\n");
	}
	printf("TOTAL");
	for(int t=0; t < aod1.nTable+1;t++){
		printf("%8.3f%8.3f", data[2*t][aod1.nZone], data[2*t+1][aod1.nZone]);
	}
	printf("\n");
	for(int i=0; i < nCol; i++) delete data[i];
	delete[] data;
}

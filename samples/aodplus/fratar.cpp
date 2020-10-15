//---------------------------------------------------------------------------
#include <new>
#include <stdio.h>
#include <math.h>
#include <string.h>
//---------------------------------------------------------------------------
#include "fratar.h"
//---------------------------------------------------------------------------
fratar::fratar(){
	nZone = 0;
	data = NULL;
	Gp = Ap = Gf = Af = NULL;
	Fg = Fa = Lg = La = NULL;
	max_iter = 1000;
	min_error = 0.01;
	memset(err_msg,'\0',128);
}
//---------------------------------------------------------------------------
bool fratar::init(int nZone) {
	this->nZone = nZone;
	try {
		data =  new double*[nZone];
		for(int i=0; i < nZone; i++){
			data[i] = new double[nZone];
			for(int j=0;j < nZone; j++) data[i][j] = 0;
		}
		Gp = new double[nZone];
		Ap = new double[nZone];
		Gf = new double[nZone];
		Af = new double[nZone];
		Fg = new double[nZone];
		Fa = new double[nZone];
		Lg = new double[nZone];
		La = new double[nZone];
		for(int i=0; i < nZone; i++) {
			Fg[i] = Fa[i] = Lg[i] = La[i] = 0;
			Gp[i] = Ap[i] = Gf[i] = Af[i] = 0;
		}

	} catch (std::bad_alloc& e) {
		if (La != NULL) {delete[] La; La = NULL;}
		if (Lg != NULL) {delete[] Lg; Lg = NULL;}
		if (Fa != NULL) {delete[] Fa; Fa = NULL;}
		if (Fg != NULL) {delete[] Fg; Fg = NULL;}
		if (Af != NULL) {delete[] Af; Af = NULL;}
		if (Gf != NULL) {delete[] Gf; Gf = NULL;}
		if (Ap != NULL) {delete[] Ap; Ap = NULL;}
		if (Gp != NULL) {delete[] Gp; Gp = NULL;}
		if (data != NULL) {
			for(int i=0; i < nZone; i++) {
				if(data[i]!=NULL) delete data[i];
			}
			data = NULL;
		}
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------
//! データをゼロにする。
void fratar::clear(){
	for(int i=0; i < nZone; i++) {
		Fg[i] = Fa[i] = Lg[i] = La[i] = 0;
		Gp[i] = Ap[i] = Gf[i] = Af[i] = 0;
    	for(int j=0; j < nZone; j++) {
			data[i][j] = 0;
        }
	}
}
//---------------------------------------------------------------------------
//! デストラクタ
fratar::~fratar(){

	for(int i=0; i < nZone; i++)
        delete[] data[i];
    delete[] data;
    delete[] Gp;
    delete[] Ap;
    delete[] Gf;
    delete[] Af;
    delete[] Fg;
    delete[] Fa;
    delete[] Lg;
    delete[] La;
}
int fratar::start() {
	double dg = 0;
	double da = 0;
	double pg, pa;
	for(int i=0; i < nZone; i++ ) {
		Gp[i] = Ap[i] = 0;
		dg += Gf[i];
		da += Af[i];
	}
	for(int i=0; i < nZone; i++) {
		for(int j=0; j < nZone; j++) {
			Gp[i] += data[i][j];
			Ap[j] += data[i][j];
		}
	}
	pg  = 0;
	pa = 0;
	for(int i=0; i < nZone; i++ ) {
		if( (Gf[i] != 0 && Gp[i] == 0) || (Af[i] != 0 && Ap[i] == 0) ) {
			 sprintf(err_msg,"%10.0lf%10.0lf%10.0lf%10.0lf\n",
									Gf[i],Gp[i],Af[i],Ap[i]);
			return (i+1);
		}
		if( Gp[i] > 0 ) Fg[i] = Gf[i] / Gp[i]; else Fg[i] = 0;
		if( Ap[i] > 0 ) Fa[i] = Af[i] / Ap[i]; else Fa[i] = 0;
		pg += Gp[i];
		pa += Ap[i];
	}
//	Ft = (dg+da)/(pg+pa);
	Ft = dg+da;
	return 0;
}
//---------------------------------------------------------------------------
//! 計算本体
/*! @return 収束したらtrue、失敗したらマイナス値 */
//---------------------------------------------------------------------------
void fratar::update() {

	for(int i=0; i < nZone; i++){
		if( Gp[i] > 0 ) Fg[i] = Gf[i] / Gp[i]; else Fg[i] = 0;
		if( Ap[i] > 0 ) Fa[i] = Af[i] / Ap[i]; else Fa[i] = 0;
	}

	for(int i=0; i < nZone; i++){
		double sum_g = 0;
		double sum_a = 0;
		for(int j=0; j < nZone; j++) {
			sum_g += data[i][j] * Fa[j];
			sum_a += data[j][i] * Fg[j];
		}
		if( sum_g == 0 ) Lg[i] = 0;
		else Lg[i] = Gp[i]/sum_g;
		if( sum_a == 0 ) La[i] = 0;
		else La[i] = Ap[i]/sum_a;
	}
	for(int i=0; i < nZone; i++) {
		for(int j=0; j < nZone; j++) {
			data[i][j] =
				data[i][j]*Fg[i]*Fa[j]*(Lg[i]+La[j])/2;
		}
	}

	for(int i=0; i < nZone; i++ ) {
		Gp[i] = Ap[i] = 0;
	}
	for(int i=0; i < nZone; i++) {
		for(int j=0; j < nZone; j++) {
			Gp[i] += data[i][j];
			Ap[j] += data[i][j];
		}
	}
}
// 平均成長率法
void fratar::update_avg() {

	for(int i=0; i < nZone; i++){
		if( Gp[i] > 0 ) Fg[i] = Gf[i] / Gp[i]; else Fg[i] = 0;
		if( Ap[i] > 0 ) Fa[i] = Af[i] / Ap[i]; else Fa[i] = 0;
	}

	for(int i=0; i < nZone; i++) {
		for(int j=0; j < nZone; j++) {
			data[i][j] =
				data[i][j]*(Fg[i]+Fa[j])/2;
		}
	}

	for(int i=0; i < nZone; i++ ) {
		Gp[i] = Ap[i] = 0;
	}
	for(int i=0; i < nZone; i++) {
		for(int j=0; j < nZone; j++) {
			Gp[i] += data[i][j];
			Ap[j] += data[i][j];
		}
	}
}
// デトロイト法
void fratar::update_det() {

	double pg, pa;
	pg = pa = 0;
	for(int i=0; i < nZone; i++){
		if( Gp[i] > 0 ) Fg[i] = Gf[i] / Gp[i]; else Fg[i] = 0;
		if( Ap[i] > 0 ) Fa[i] = Af[i] / Ap[i]; else Fa[i] = 0;
		pg += Gp[i];
		pa += Ap[i];
	}

	for(int i=0; i < nZone; i++) {
		for(int j=0; j < nZone; j++) {
			data[i][j] =
				data[i][j]*Fg[i]*Fa[j]/(Ft/(pg+pa));
		}
	}

	for(int i=0; i < nZone; i++ ) {
		Gp[i] = Ap[i] = 0;
	}
	for(int i=0; i < nZone; i++) {
		for(int j=0; j < nZone; j++) {
			Gp[i] += data[i][j];
			Ap[j] += data[i][j];
		}
	}
}

bool fratar::check() {
	bool flag = true;
	max_error = 0;
	for(int i=0; i< nZone; i++) {
		if( Fg[i] > 0 ) {
			if( fabs(Fg[i]-1) >= min_error ) flag = false;
			if( fabs(Fg[i]-1) > max_error ) {
				max_error = fabs(Fg[i]-1);
				gen_f = Gf[i]; gen_p = Gp[i];
				att_f = Af[i]; att_p = Ap[i];
				chk_zone=i+1;
			}
		}
		if( Fa[i] > 0 ) {
			if( fabs(Fa[i]-1) >= min_error ) flag = false;
			if( fabs(Fa[i]-1) > max_error ) {
				max_error = fabs(Fa[i]-1);
				gen_f = Gf[i]; gen_p = Gp[i];
				att_f = Af[i]; att_p = Ap[i];
				chk_zone=i+1;
			}
		}
	}
	return flag;
}

int fratar::calc(){
    //収束計算

	for(count=0; count < max_iter; count++) {
		//現在の発生集中総量を計算する
		for(int i=0; i < nZone; i++ ) {
			Gp[i] = Ap[i] = 0;
		}
		for(int i=0; i < nZone; i++) {
			for(int j=0; j < nZone; j++) {
				Gp[i] += data[i][j];
				Ap[j] += data[i][j];
			}
		}
		bool check = true;
		max_error = 0;
		for(int i=0; i< nZone; i++) {
			if( Gp[i] == 0 ) Fg[i] = 0;
			else {
				Fg[i] = Gf[i] / Gp[i];
				if( fabs(Fg[i]-1) >= min_error ) check = false;
				if( fabs(Fg[i]-1) > max_error ) {
					max_error = fabs(Fg[i]-1);
					gen_f = Gf[i]; gen_p = Gp[i];
					att_f = Af[i]; att_p = Ap[i];
					chk_zone=i+1;
				}
			}
			if( Ap[i] == 0 ) Fa[i] = 0;
			else {
				Fa[i] = Af[i] / Ap[i];
				if( fabs(Fa[i]-1) >= min_error ) check = false;
				if( fabs(Fa[i]-1) > max_error ) {
					max_error = fabs(Fa[i]-1);
					gen_f = Gf[i]; gen_p = Gp[i];
					att_f = Af[i]; att_p = Ap[i];
					chk_zone=i+1;
				}
			}
		}
//        printf("max: %f\n", max_error);
		if( check == true ) return 0;  //収束する。ここで外に出る。

		for(int i=0; i < nZone; i++){
			double sum_g = 0;
			double sum_a = 0;
			for(int j=0; j < nZone; j++) {
				sum_g += data[i][j] * Fa[j];
				sum_a += data[j][i] * Fg[j];
			}
			if( sum_g == 0 ) Lg[i] = 0;
			else Lg[i] = Gp[i]/sum_g;
			if( sum_a == 0 ) La[i] = 0;
			else La[i] = Ap[i]/sum_a;
		}
		for(int i=0; i < nZone; i++) {
			for(int j=0; j < nZone; j++) {
				data[i][j] =
					data[i][j]*Fg[i]*Fa[j]*(Lg[i]+La[j])/2;
			}
		}

	}
	return (-2);
}
//---------------------------------------------------------------------------
//! ODデータを設定する
void fratar::set_od(int i, int j, float od){
	data[i][j] = od;
}
//---------------------------------------------------------------------------
//! ODデータを取得(計算後に取得する事で答えを得る)
float fratar::get_od(int i, int j ){
	return data[i][j];
}
//---------------------------------------------------------------------------
//! 現在発生量
float fratar::get_gen_f(int zone) {
	return Gf[zone];
}
//---------------------------------------------------------------------------
//! 現在集中量
float fratar::get_att_f(int zone) {
	return Af[zone];
}
//---------------------------------------------------------------------------
//! 将来の発生集中総量を各ゾーンに設定する。
void fratar::set_ga(int zone, float generation, float attraction)
{
    Gf[zone] = generation;
    Af[zone] = attraction;
}
////////////////////////////////////////////////////////////////////////////////
//  エラー値(%ではない)： > 0
////////////////////////////////////////////////////////////////////////////////
bool fratar::set_error(float e){
    if( e <= 0 ) return false;
    min_error = e;
    return true;
}
////////////////////////////////////////////////////////////////////////////////
//  もとの発生集中量と、現在の量を比較して表示
////////////////////////////////////////////////////////////////////////////////
void fratar::print_ga(){
	double a, b;
	for(int i=0; i < nZone; i++ ) {
		Gp[i] = Ap[i] = 0;
	}
	for(int i=0; i < nZone; i++) {
		for(int j=0; j < nZone; j++) {
			Gp[i] += data[i][j];
			Ap[j] += data[i][j];
		}
	}
    for(int i=0; i < nZone; i++) {
		if( Gp[i] > 0 ) a = Gf[i] / Gp[i]; else a = 0;
		if( Ap[i] > 0 ) b = Af[i] / Ap[i]; else b = 0;
        printf("%10.0lf%10.0lf%10.5lf%10.0lf%10.0lf%10.5lf\n",
            Gf[i], Gp[i], a, Af[i], Ap[i], b);
    }
}

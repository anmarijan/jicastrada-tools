//---------------------------------------------------------------------------
#ifndef MatrixH
#define MatrixH
//---------------------------------------------------------------------------
#include <stdexcept>
#include <stdio.h>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
template <class T> class Matrix {
	T* data;
	int nRow;
	int nCol;
public:
	Matrix();
	~Matrix();
	bool init(int s, int u);
	bool init(int s, int u, T f);
	T get(int i, int j);
	T* getp(int i, int j);
	void set(int i, int j, T d);
	void add(int i, int j, T d);
};
//---------------------------------------------------------------------------
template <class T> Matrix<T>::Matrix() {
	nCol = nRow = 0;
    data = NULL;
}

template <class T> bool Matrix<T>::init(int s, int u) {
	if( s > 0 && u > 0) {
		try {
			data = new T[s*u];
    		nRow = s;
            nCol = u;
            return true;
        } catch(const std::bad_alloc& e) {
			return false;
        }
    }
    return false;
}

template <class T> bool Matrix<T>::init(int s, int u, T f) {
	if( s > 0 && u > 0) {
		try {
			data = new T[s*u];
    		nRow = s;
            nCol = u;
            for(int i=0; i < s*u; i++) data[i] = f;
            return true;
        } catch(const std::bad_alloc& e) {
			return false;
        }
    }
    return false;
}


template <class T> Matrix<T>::~Matrix() {
	delete[] data;
}

template <class T> T Matrix<T>::get(int i, int j) {
	if( i < 0 || i >= nRow || j < 0 || j >= nCol ) throw out_of_range("Matrix");
	return data[i*nCol+j];
}

template <class T> T* Matrix<T>::getp(int i, int j) {
	if( i < 0 || i >= nRow || j < 0 || j >= nCol ) return NULL;
	return &data[i*nCol+j];
}

template <class T> void Matrix<T>::set(int i, int j, T d) {
	if( i < 0 || i >= nRow || j < 0 || j >= nCol ) throw out_of_range("Matrix");
	data[i*nCol+j] = d;
}
template <class T> void Matrix<T>::add(int i, int j, T d) {
	if( i < 0 || i >= nRow || j < 0 || j >= nCol ) throw out_of_range("Matrix");
	data[i*nCol+j] += d;
}
#endif

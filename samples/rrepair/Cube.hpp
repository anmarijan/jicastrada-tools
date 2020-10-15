//---------------------------------------------------------------------------
#ifndef CubeH
#define CubeH
//---------------------------------------------------------------------------
#include <stdexcept>
#include <stdio.h>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
template <class T> class Cube {
	T* data;
    int nTable;
    int nRow;
    int nCol;
public:
	Cube();
    ~Cube();
    bool init(int r, int s, int u, T d);
    void set(int r, int s, int u, T d);
	void add(int r, int s, int u, T d);
    T get(int r, int s, int u);
    T* getp(int r, int s, int u);
};

//---------------------------------------------------------------------------
template <class T> Cube<T>::Cube() {
	nTable = nRow = nCol = 0;
    data = NULL;
}
template <class T> Cube<T>::~Cube() {
    delete[] data;
}
template <class T> bool Cube<T>::init(int r, int s, int u, T d) {
	if( r > 0 && s > 0 && u > 0) {
		try {
			data = new T[r*s*u];
            nTable = r;
    		nRow = s;
            nCol = u;
            for(int i=0; i < r*s*u; i++) data[i] = d;
            return true;
        } catch(const std::bad_alloc& e) {
			return false;
        }
    }
    return false;
}
template <class T> void Cube<T>::set(int r, int s, int u, T d) {
	if( r >= 0 && s >= 0 && u >= 0 && r < nTable && s < nRow && u < nCol) {
		data[nRow*nCol*r + nCol*s + u] = d;
    }
}

template <class T> void Cube<T>::add(int r, int s, int u, T d) {
	if( r >= 0 && s >= 0 && u >= 0 && r < nTable && s < nRow && u < nCol) {
		data[nRow*nCol*r + nCol*s + u] += d;
    }
}

template <class T> T Cube<T>::get(int r, int s, int u) {
	T ret;
	if( r >= 0 && s >= 0 && u >= 0 && r < nTable && s < nRow && u < nCol) {
		ret = data[nRow*nCol*r + nCol*s + u];
    }
	return ret;
}

template <class T> T* Cube<T>::getp(int r, int s, int u) {
	if( r >= 0 && s >= 0 && u >= 0 && r < nTable && s < nRow && u < nCol) {
		return &data[nRow*nCol*r + nCol*s + u];
    }
    return NULL;
}
#endif

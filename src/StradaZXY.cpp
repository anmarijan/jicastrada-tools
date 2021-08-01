#include <pch.h>
//---------------------------------------------------------------------------
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cfloat>
#include <stdexcept>
#include "tool.h"
#include "StradaCmn.h"
#include "StradaZXY.h"
//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// Centroid
////////////////////////////////////////////////////////////////////////////////

void Border::set_center() {
	long double x1, x2, y1, y2;
	long double sx, sy, sum, area;
	if( nPoint == 0 ) return;
	x1 = pt[0].x;
	y1 = pt[0].y;
	sx = sy = sum = 0;
	for(int i=1; i < nPoint; i++) {
		x2 = pt[i].x;
		y2 = pt[i].y;
		area = x2 * y1 - x1 * y2 ;
		sx += area * ( x1 + x2 );
		sy += area * ( y1 + y2 );
		sum += area/2;
		x1 = x2;
		y1 = y2;
	}
	x = sx / sum / 6 ;
	y = sy / sum / 6 ;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
Border::~Border() {
	delete[] pt;
}

void Border::init(int n) {
	nPoint = n ;
	pt = new XYPoint[nPoint];
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
bool Border::inside(double cx, double cy) {
	int count = 0;

	if( nPoint < 3 ) return false;

	for( int i=0; i < nPoint-1; i++ ) {
		if( ( pt[i].y <= cy && pt[i+1].y > cy ) || (pt[i].y > cy && pt[i+1].y <= cy )) {
			double d = (cy-pt[i].y)/(pt[i+1].y-pt[i].y);
			if ( cx < (pt[i].x + d * (pt[i+1].x-pt[i].x)) ) count++;
		}
	}
	if( count % 2 == 1 ) return true;
	return false;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
StradaZXY::StradaZXY(){
	version = 2;
	nZone = 0;
	nBorder = 0;
	scale = 1;
	coordinate = 0;
	zones = NULL;
	borders = NULL;
	csv = true;
	memset(filename, 0, 21);
}
StradaZXY::~StradaZXY(){
	delete[] zones;
	delete[] borders;
}
void StradaZXY::clear() {
	delete[] zones;
	delete[] borders;
	zones = NULL;
	borders = NULL;
	version = 2;
	nZone = 0;
	nBorder = 0;
	scale = 1;
	coordinate = 0;
	memset(filename, 0, 21);
}
void StradaZXY::init(int n_zone, int n_border) {
	nZone   = n_zone;
	nBorder = n_border;
	zones = new XYPoint[nZone];
	borders = new Border[nBorder];
}
///////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
int StradaZXY::Read(FILE* fp) {
	char buff[256];
	char str[20] ={};
    char* pdata[5];

	if( fgets(buff,256,fp) == NULL ) return (-1);
	strncpy_s(str, sizeof(str), buff, 5);
	if( strncmp( str, "ZXY", 3) != 0 ) return (-1);
	if( str[3] == ' ' ) version = 1;
	else if (str[3] == '2' ) version = 2;
	else version = -1;
	if( version != 1 && version != 2 ) return (-1);
	csv = ( str[4] == '*' ) ? true : false;
	
	if( fgets(buff,256,fp) == NULL ) return (-2);
	if( csv ) {
		csv_parser(buff, pdata, 2, ',', '.');
		nZone = atoi(pdata[0]);
		coordinate = atoi(pdata[1]);
	} else {
		nZone  = getbufInt(buff, 0 , 5);
		coordinate = getbufInt(buff, 5 , 5);
	}
	if( version == 1 ) {
		scale = getbufInt(buff, 10, 5 );
		strncpy_s(filename, sizeof(filename), &buff[20], 20);
	}
	int iz;
	double ix, iy;
	int row = 3;

	if( csv ) {
		for (int i=0; i < nZone; i++ ) {
			if( fgets(buff, 256, fp) == NULL ) return (-row);
			csv_parser(buff, pdata, 3, ',', '.');
			iz = atoi(pdata[0]);
			ix = atof(pdata[1]);
			iy = atof(pdata[2]);
			if( iz != i+1 ) return (-row);
			if( coordinate == 2 ) {
				if( ix < 0 || ix > 180 ) return (-row);
				if( iy < -90 || iy > 90 ) return (-row);
	        }
			row++;


		}
	} else {
		for (int i=0; i < nZone; i++ ) {
			if( fgets(buff, 256, fp) == NULL ) return (-row);
			iz = getbufInt(buff, 0, 5);
			if( version == 1 ) {
				ix = getbufDbl(buff, 5, 5);
				iy = getbufDbl(buff,10, 5);
			} else {
				ix = getbufDbl(buff, 5, 10);
				iy = getbufDbl(buff,15, 10);
			}
			if( iz != i+1 ) return (-row);
			if( coordinate == 2 ) {
				if( ix < 0 || ix > 180 ) return (-row);
				if( iy < -90 || iy > 90 ) return (-row);
	        }
			row++;
		}
	}
	nBorder = 0;
	if( csv ) {
		while(true) {
			if( fgets(buff, 256, fp) == NULL ) break;
			csv_parser(buff, pdata, 1, ',', '.');
			iz = atoi(pdata[0]);
			if(iz < 2 ) return (-row);
			row++;
			for(int i=0; i < iz; i++) {
				if( fgets(buff, 256, fp) == NULL) return (-row);
				csv_parser(buff, pdata, 3, ',', '.');
				ix = atof(pdata[0]);
				iy = atof(pdata[1]);
				row++;
			}
			nBorder++;


		}
	} else {
		while(true) {
			if( fgets(buff, 256, fp) == NULL ) break;
			iz = getbufInt(buff, 0, 5);
			if(iz < 2 ) return (-row);
			row++;
			for(int i=0; i < iz; i++) {
				if( fgets(buff, 256, fp) == NULL) return (-row);
				if( version == 1 ) {
					ix = getbufDbl(buff, 5,5);
					iy = getbufDbl(buff,10,5);
				} else {
					ix = getbufDbl(buff, 5,10);
					iy = getbufDbl(buff,15,10);
				}
				row++;
			}
			nBorder++;
		}
	}
	if ( fseek(fp, 0L, SEEK_SET) != 0 ) {
		fprintf(stderr, "SEEK ERROR");
	}
	fgets(buff, 256, fp);
	fgets(buff, 256, fp);
	try {
		zones  = new XYPoint[nZone];
		borders = new Border[nBorder];
	} catch (const std::bad_alloc& ) {
		delete[] zones;   zones = NULL;
		delete[] borders; borders = NULL;
		return -row;
	}
	row = 3;
	for(int i=0; i < nZone; i++) {
		fgets(buff,256,fp);
		row++;
		if( csv ) {
			csv_parser(buff, pdata, 3, ',', '.');
			zones[i].x = atof(pdata[1]);
			zones[i].y = atof(pdata[2]);
		} else {
			if( version == 1 ) {
				zones[i].x = getbufDbl(buff,  5, 5);
				zones[i].y = getbufDbl(buff, 10, 5);
			} else {
				zones[i].x = getbufDbl(buff,  5, 10);
				zones[i].y = getbufDbl(buff, 15, 10);
			}
		}
	}

	for(int i=0; i < nBorder; i++) {
		fgets(buff,256,fp);
		row++;
		if( csv ) {
			char* p = buff;
			while ( *p != ',' && *p != '\n' && *p != '\0' ) p++;
			*p = 0;
			borders[i].nPoint = atoi(buff);
			borders[i].pt = new XYPoint[borders[i].nPoint];

			for(int j=0; j < borders[i].nPoint; j++) {
				fgets(buff,256,fp);
				row++;
				csv_parser(buff, pdata, 3, ',', '.');
				borders[i].pt[j].x = atof(pdata[0]);
				borders[i].pt[j].y = atof(pdata[1]);
			}
		} else {
			borders[i].nPoint = getbufInt(buff, 0, 5);
		//	printf("B %d %d- %s\n", i, borders[i].nPoint, buff);
			borders[i].pt = new XYPoint[borders[i].nPoint];
			for(int j=0; j < borders[i].nPoint; j++) {
				fgets(buff,256,fp);
				row++;
				if( version == 1) {
					borders[i].pt[j].x = getbufDbl(buff,	5, 5);
					borders[i].pt[j].y = getbufDbl(buff, 10, 5);
				} else {
					borders[i].pt[j].x = getbufDbl(buff,	5, 10);
					borders[i].pt[j].y = getbufDbl(buff, 15, 10);
				}
			}
		}
	}

    calc_boundary();
	return row;

}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void StradaZXY::Write(const char* fname) {
	FILE* fp = NULL;
	char buff[12];
	errno_t error = fopen_s(&fp, fname, "wt");
	if( error == 0 && fp != NULL )
	{
		if( version == 1) fprintf(fp, "ZXY StradaZXY\n");
		else {
			if( csv ) fprintf(fp, "ZXY2* StradaZXY\n");
			else fprintf(fp, "ZXY2  StradaZXY\n");
		}

		if( version == 1) {
			fprintf(fp, "%5d%5d%5d%20s\n", nZone, coordinate, scale, filename);
			for(int i=0; i < nZone; i++) {
				fprintf(fp, "%5d", i+1 );
				fixfloat(buff, zones[i].x, 5);
				fprintf(fp, "%5s"  , buff);
				fixfloat(buff, zones[i].y, 5);
				fprintf(fp, "%5s\n", buff );
			}
			for(int i=0; i < nBorder; i++) {
				fprintf(fp, "%5d\n", borders[i].nPoint);
				for(int j=0; j < borders[i].nPoint; j++ ) {
					fixfloat(buff, borders[i].pt[j].x, 5);
					fprintf(fp, "     %5s", buff);
					fixfloat(buff, borders[i].pt[j].y, 5);
					fprintf(fp, "%5s\n", buff);
				}
			}

		} else {
			if( csv ) {
				fprintf(fp, "%d,%d,\n", nZone, coordinate);
				for(int i=0; i < nZone; i++) {
					fprintf(fp, "%d,%f,%f,\n", i+1, zones[i].x, zones[i].y);
				}
				for(int i=0; i < nBorder; i++) {
					fprintf(fp, "%d,\n", borders[i].nPoint);
					for(int j=0; j < borders[i].nPoint; j++ ) {
						fprintf(fp, "%f,%f,\n", borders[i].pt[j].x, borders[i].pt[j].y) ;
					}
				}
			} else {
				fprintf(fp, "%5d%5d\n", nZone, coordinate);

				for(int i=0; i < nZone; i++) {
					fixfloat(buff, zones[i].x, 10);
					fprintf(fp, "%5d%10s", i+1, buff);
					fixfloat(buff, zones[i].y, 10);
					fprintf(fp, "%10s\n", buff );

				}
				for(int i=0; i < nBorder; i++) {
					fprintf(fp, "%5d\n", borders[i].nPoint);
					for(int j=0; j < borders[i].nPoint; j++ ) {
						fixfloat(buff, borders[i].pt[j].x, 10);
						fprintf(fp, "     %10s", buff) ;
						fixfloat(buff, borders[i].pt[j].y, 10);
						fprintf(fp, "%10s\n", buff) ;
					}
				}
			}
		}
		fclose(fp);
	}
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void StradaZXY::Read(char* fname) {
	FILE* fp;
	char str[20];
	errno_t error = fopen_s(&fp, fname, "rt");
	if( error != 0 || fp == NULL ) throw std::runtime_error("ZXY");
	else {
		int ret = Read(fp);
		if (ret < 0) {
			sprintf_s(str, sizeof(str), "ZXY %d", -ret);
			throw std::runtime_error(str);
		}
		fclose(fp);
	}
}
////////////////////////////////////////////////////////////////////////////////
//  Coordinate conversion
////////////////////////////////////////////////////////////////////////////////
void StradaZXY::conv(int cd, int mergin){

	double y;

	if( cd == coordinate ) return;
	if( coordinate != 0 && coordinate != 1 ) return;

	double y_max = 0;
	double y_min = FLT_MAX;
	for(int i=0; i < nBorder; i++) {
		for(int j=0; j < borders[i].nPoint; j++) {
			if( y_max < borders[i].pt[j].y ) y_max = borders[i].pt[j].y;
			if( y_min > borders[i].pt[j].y ) y_min = borders[i].pt[j].y;
		}
	}

	for(int i=0; i < nZone; i++) {
		y = zones[i].y;
		zones[i].y = y_max - y + mergin;
	}

	for(int i=0; i < nBorder; i++) {
		for(int j=0; j < borders[i].nPoint; j++) {
			y = borders[i].pt[j].y;
			borders[i].pt[j].y = y_max - y + mergin;
		}
	}


}
////////////////////////////////////////////////////////////////////////////////
//   Coordinate conversion
////////////////////////////////////////////////////////////////////////////////
void StradaZXY::circulate(){

	double x1, x2, y1, y2;
	double sum;
	double temp ;
	div_t dx;

	for(int i=0; i < nBorder; i++ ) {

		sum = 0;
		x1 = (double)borders[i].pt[0].x;
		y1 = (double)borders[i].pt[1].y;
		for(int j=1; j <= borders[i].nPoint; j++) {
			x2 = (double)borders[i].pt[j].x;
			y2 = (double)borders[i].pt[j].y;

			sum += x2 * y1 - y2 * x1;
			x1 = x2;
			y1 = y2;
		}
		if( sum < 0 ) { //
			dx = div(borders[i].nPoint, 2);
			for(int j = 1; j < dx.quot; j++) {
				int k = borders[i].nPoint-1-j;
				temp = borders[i].pt[j].x;
				borders[i].pt[j].x = borders[i].pt[k].x;
				borders[i].pt[k].x = temp;
				temp = borders[i].pt[j].y;
				borders[i].pt[j].y = borders[i].pt[k].y;
				borders[i].pt[k].y = temp;

			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void StradaZXY::calc_boundary() {
	min_x = FLT_MAX; //MAXdouble;
	min_y = FLT_MAX;
	max_x = 0;
	max_y = 0;

	for(int i=0; i < nBorder; i++) {
		for(int j=0; j < borders[i].nPoint; j++) {
			min_x = ( min_x < borders[i].pt[j].x) ? min_x : borders[i].pt[j].x;
			min_y = ( min_y < borders[i].pt[j].y) ? min_y : borders[i].pt[j].y;
			max_x = ( max_x > borders[i].pt[j].x) ? max_x : borders[i].pt[j].x;
			max_y = ( max_y > borders[i].pt[j].y) ? max_y : borders[i].pt[j].y;
		}
		borders[i].set_center();
	}
	width = max_x - min_x;
	height = max_y - min_y ;
}

/////////////////////////////////////////////////////////////////////////////
// MapInfo format
/////////////////////////////////////////////////////////////////////////////
void StradaZXY::WriteMInfo(const char* fname) {

    FILE* fp_mif;
    FILE* fp_mid;
	std::string file_name;
	std::string fullname = fname;
	std::string bodyname;
	size_t pos = fullname.find_last_of(".");
	if (pos > 0) bodyname = fullname.substr(0, pos);
	else bodyname = fullname;
	file_name = bodyname + ".mif";
    double xo = min_x;
    double yo = min_y;
    double xm = max_x;
    double ym = max_y;
	errno_t error = fopen_s(&fp_mif, file_name.c_str(), "wt");
    if(error == 0 && fp_mif != NULL ) {
		file_name = bodyname + ".mid";
		error = fopen_s(&fp_mid, file_name.c_str(), "wt");
        if( error == 0 && fp_mid != NULL ) {
              fprintf(fp_mif,"Version 300\n");
              fprintf(fp_mif,"Charset \"WindowsJapanese\"\n");
              fprintf(fp_mif,"Delimiter \",\"\n");
              fprintf(fp_mif,"CoordSys NonEarth Units \"km\" Bounds (%g,%g) (%g,%g)\n",xo,yo,xm,ym);
              fprintf(fp_mif,"Columns 2\n");
              fprintf(fp_mif,"  XCenter Integer\n");
              fprintf(fp_mif,"  YCenter Integer\n");
              fprintf(fp_mif,"Data\n\n");
			  /*
			  for(int i=0; i < nBorder; i++) {
				fprintf(fp_mid, "%5g\,%5g\,\n", zones[i].x, zones[i].y );
				fprintf(fp_mif,"Region 1\n");
				fprintf(fp_mif, "%3d\n", zones[i].nPoint);
				for(int j=0; j < zones[i].nPoint; j++ ) {
					fprintf(fp_mif,"%12g %12g\n",zones[i].pt[j].x ,zones[i].pt[j].y);
				}
			  }
			  */
            fclose(fp_mid);
        }
        fclose(fp_mif);

    }
}

/////////////////////////////////////////////////////////////////////////////
// MapInfo format with GAD FILE data
/////////////////////////////////////////////////////////////////////////////
void StradaZXY::WriteMInfo(char* fname, StradaGAD& gad) {

	FILE* fp_mif = NULL;
	FILE* fp_mid = NULL;
	std::string file_name;
	std::string fullname = fname;
	size_t pos = fullname.find_last_of(".");
	std::string bodyname;
	if (pos > 0) bodyname = fullname.substr(0, pos);
	else bodyname = fullname;
	file_name = bodyname + ".mif";
	double xo = min_x;
	double yo = min_y;
	double xm = max_x;
	double ym = max_y;
	if( gad.nZone != nZone) {
		printf("No of zones is different GAD:%d, ZXY:%d\n", gad.nZone, nZone);
	}
	errno_t error = fopen_s(&fp_mif, file_name.c_str(), "wt");
	if(error == 0 && fp_mif != NULL ) {
		file_name = bodyname + ".mid";
		error = fopen_s(&fp_mid, file_name.c_str(), "wt");
		if(error == 0 && fp_mif != NULL) {
			  fprintf(fp_mif,"Version 300\n");
			  fprintf(fp_mif,"Charset \"WindowsJapanese\"\n");
			  fprintf(fp_mif,"Delimiter \",\"\n");
			  fprintf(fp_mif,"CoordSys NonEarth Units \"km\" Bounds (%g,%g) (%g,%g)\n",xo,yo,xm,ym);
			  fprintf(fp_mif,"Columns %d\n", 2+gad.nData);
			  fprintf(fp_mif,"  XCenter Integer\n");
			  fprintf(fp_mif,"  YCenter Integer\n");
			  for(int i=0; i < gad.nData; i++ ) {
				fprintf(fp_mif, "  %s Integer\n", gad.GA[i].name.c_str());
			  }
			  fprintf(fp_mif,"Data\n\n");

			  for(int i=0; i < nZone; i++) {
				fprintf(fp_mid, "%5g,%5g,", zones[i].x, zones[i].y );
				for(int j=0; j < gad.nData; j++ ) {
					fprintf(fp_mid, "%8f,", gad.GA[j].data[i]);
				}
				fprintf(fp_mid, "\n");
				fprintf(fp_mif,"Region 1\n");
				fprintf(fp_mif, "%3d\n", borders[i].nPoint);
				for(int j=0; j < borders[i].nPoint; j++ ) {
					fprintf(fp_mif,"%12g %12g\n",borders[i].pt[j].x ,borders[i].pt[j].y);
				}
			  }

			fclose(fp_mid);
		}
		fclose(fp_mif);
	}
}


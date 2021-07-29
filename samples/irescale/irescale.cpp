//---------------------------------------------------------------------------
#include <stdlib.h>
#include <cmath>
#include <stdexcept>
#include "StradaIRE.h"
//---------------------------------------------------------------------------

int main(int argc, char* argv[])
{

	if( argc != 4 ) {
		puts("Usage: irescale [input] [output] [scale]");
		puts("This program multiply the traffic volume in [input] file by [scale]");
		return 1;
    }
    double scale;
	StradaIRE ire;
    scale = atof(argv[3]);
    if (scale <= 0 ) {
    	printf("Scale error: %s\n", argv[3]);
        return 1;
    }
    printf("Scale = %f\n", scale);

	try {
		ire.Read(argv[1]);
	} catch( const std::runtime_error& e) {
		fprintf(stderr, "%s\n", e.what());
		exit(1);
	}

    for(int i=0; i < ire.nLink; i++) {
       	IRELinkPtr link = ire.links[i];
		link->Capa *= scale;
    	for(int j=0; j < 2; j++) {
            link->result[j].Vol *= scale;
            for(int m=0; m < 10; m++) {
            	link->result[j].inVol[m] *= scale;
            	link->result[j].thVol[m] *= scale;
            	link->result[j].btVol[m] *= scale;
            }
        }
    }
    ///////////////////////////////////////////////////////////////
    //
	for(int i=0; i < ire.nLink; i++) {
       	IRELinkPtr link = ire.links[i];
        link->Capa =ceil(link->Capa);
    	for(int j=0; j < 2; j++) {
            link->result[j].Vol = ceil(link->result[j].Vol);
            for(int m=0; m < 10; m++) {
            	link->result[j].inVol[m] = ceil(link->result[j].inVol[m]);
            	link->result[j].thVol[m] = ceil(link->result[j].thVol[m]);
            	link->result[j].btVol[m] = ceil(link->result[j].btVol[m]);
            }
        }
    }

    ire.Write(argv[2]);

	return 0;
}
//---------------------------------------------------------------------------

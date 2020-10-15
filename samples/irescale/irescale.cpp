//---------------------------------------------------------------------------
#include <stdlib.h>
#include <cmath>
#include "StradaIRE.h"
//---------------------------------------------------------------------------

int main(int argc, char* argv[])
{

	if( argc != 4 ) {
		puts("Usage: irescale [input] [output] [scale]");
		puts("This program multiply the traffic volume in [input] file by [scale]");
		return 1;
    }
    FILE* fp;
    double scale;
	StradaIRE ire;
    scale = atof(argv[3]);
    if (scale <= 0 ) {
    	printf("Scale error: %s\n", argv[3]);
        return 1;
    }
    printf("Scale = %f\n", scale);

//    char* str = ".456";
//    float test = atof(str);
//    printf("%f\n", test);
	if( (fp = fopen(argv[1],"rt")) == NULL ) {
		fprintf(stderr,"Cannot open file %s.\n", argv[1]);
		exit(1);
	}

	if( ire.Read(fp) == -1 ) {
		fclose(fp);
		fprintf(stderr,"Reading error in %s.\n", argv[1]);
		exit(1);
	}
	fclose(fp);

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

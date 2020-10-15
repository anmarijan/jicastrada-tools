/*
 二つの結果ファイル(IRE)の交通量を比較する。
*/
//---------------------------------------------------------------------------
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
//---------------------------------------------------------------------------
#include "StradaIRE.h"
//---------------------------------------------------------------------------

// 車種別に速度調整がされている場合がある。

void usage() {
    puts("\nUsage: irecomp ire1 ire2");
	puts("Print results (below) to standard output");
	puts("ire1(A->B) ire1(B->A) ire2(A->B) ire2(B->A) [ire1-ire2](A->B) [ier1-ire2](B->A)");
}


int main(int argc, char* argv[])
{
    if( argc < 3 ) {
		usage();
        exit(1);
    }
    StradaIRE ire1, ire2;

//初期化
    try {
        ire1.Read(argv[1]);
		ire2.Read(argv[2]);
    } catch (std::runtime_error& e) {
		fprintf(stderr,"Cannot read file. \nException: %s ",  e.what() );
		fprintf(stderr,"Message: %s\n", ire1.msg);
		fprintf(stderr,"Message: %s\n", ire2.msg);
		exit(1);
	}

	int nLink = ire1.nLink;
	if( nLink != ire2.nLink ) {
		fprintf(stderr, "No. of Links different\n");
	}

	for(int i=0; i < nLink; i++) {
		IRELinkPtr link1 = ire1.getLink(i);
		IRELinkPtr link2 = ire2.getLink(i);
		float v11 = link1->result[0].Vol;
		float v12 = link1->result[1].Vol;
		float v21 = link2->result[0].Vol;
		float v22 = link2->result[1].Vol;


		printf("%s\t%g\t%g\t%g\t%g\t%g\t%g\n",
			   link1->name,
			   v11, v12, v21, v22, v11-v21, v12-v22);
	}
    return 0;
}
//---------------------------------------------------------------------------

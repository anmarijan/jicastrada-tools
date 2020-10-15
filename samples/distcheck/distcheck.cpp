/*
リンク長を座標値から計算して、lengthの値と比較する。
結果はstdoutに出力
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdexcept>

#include "StradaINT.h"

void usage() {
    printf("Usage: distcheck file\n");
    printf("file: INT [input]\n");
}

int main(int argc, char* argv[]) {
    StradaINT sint;
    LinkPtr link;
    double d;
    double ix, iy, jx, jy;

    if( argc != 2 ) {
        usage();
        exit(1);
    }

    try {
        sint.Read(argv[1]);

	}catch (std::exception& e ) {
		fprintf(stderr, "%s\n", e.what());
		fprintf(stderr, "%s\n", sint.msg());
        return (1);
	}

    for(const auto& link: sint.links) {
        d = 0;
        ix = link->iX;
        iy = link->iY;
        if( link->dummy > 0 ) {
            jx = link->dX[0];
            jy = link->dY[0];
            d += sqrt(pow(ix-jx,2)+pow(iy-jy,2));
            ix = jx;
            iy = jy;
            if( link->dummy > 1) {
                jx = link->dX[1];
                jy = link->dY[1];
                d += sqrt(pow(ix-jx,2)+pow(iy-jy,2));
                ix = jx;
                iy = jy;
                if( link->dummy > 2) {
                    jx = link->dX[2];
                    jy = link->dY[2];
                    d += sqrt(pow(ix-jx,2)+pow(iy-jy,2));
                    ix = jx;
                    iy = jy;
                }
            }
        }
        jx = link->jX;
        jy = link->jY;
        d += sqrt(pow(ix-jx,2)+pow(iy-jy,2));
        d /= 1000;
        printf("%-10s\t%10.3f\t%10.3f\t%10.3f\n", link->name, link->length, d, d/ link->length);
    }
    return 0;
}

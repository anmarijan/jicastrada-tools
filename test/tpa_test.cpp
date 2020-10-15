//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>

#include "StradaTPA.h"
//---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	if( argc != 2 ) {
		printf("usage: tpa_test tpa_file");
		exit(1);
	}

	StradaTPA par;

	try {
		printf("start\n");
		par.Read(argv[1]);
		printf("Success!\n");
/*
		printf("ZONE = %d\n", par.nZone);
		printf("LINK = %d\n", par.nLink);

		printf("ConvertType = %c\n", par.ConvertType);
		printf("MODE = %d\n", par.pub_mode);
		printf("PUB A = %lf\n", par.pubp[0].a);
		printf("PUB B = %lf\n", par.pubp[0].b);
		printf("PUB C = %lf\n", par.pubp[0].c);
*/
//		printf("%5d\n", ire.nLink);
	}catch (std::exception& e ) {
		printf("%s\n", e.what());
//		printf("%s\n", ire.msg);

		return 0;
	}

    return 1;
}
//---------------------------------------------------------------------------

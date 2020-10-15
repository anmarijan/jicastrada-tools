//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>

#include "StradaIRE.h"
//---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	if( argc != 2 ) {
		printf("usage: ire_test ire_file");
		exit(1);
	}

	StradaIRE ire;
	
	try {
		printf("start\n");
		ire.Read(argv[1]);
		printf("Success!");
		printf("%5d\n", ire.nLink);
	}catch (std::exception& e ) {
		printf("%s\n", e.what());
		printf("%s\n", ire.msg);
		printf("%5d %5d %5d\n", ire.nLink, ire.nNode, ire.nMode);
		return 0;
	}

    return 1;
}
//---------------------------------------------------------------------------

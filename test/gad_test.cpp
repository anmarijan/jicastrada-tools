//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#pragma hdrstop

#include "StradaGAD.h"
//---------------------------------------------------------------------------

#pragma argsused

int main(int argc, char* argv[])
{
	if( argc != 2 ) {
		printf("usage: gad_test\n");
		exit(1);
	}

	StradaGAD gad;

	try {
		printf("start\n");
		gad.Read(argv[1]);
		printf("Success!");
		printf("%5d\n", gad.nData);
	}catch (std::exception& e ) {
		
		printf("Error: %s\n", e.what());
		return 0;
	}
    return 1;
}
//---------------------------------------------------------------------------

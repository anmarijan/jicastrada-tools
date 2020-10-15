//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>

#include "StradaTNT.h"
//---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	if( argc != 2 ) {
		printf("usage: tnt_test tnt_file");
		exit(1);
	}
	StradaTNT tnt;
	try {
		printf("start\n");
		tnt.Read(argv[1]);
		printf("Success!");
		printf("%5d\n", tnt.nLine);

		for(int i=0; i < tnt.nLine; i++) {
			TRNLine* line = &tnt.lines[i];
			printf("%-10s\n",line->name);
		}

	}catch (std::exception& e ) {
		printf("%s\n", e.what());
		return 0;

	}
    return 1;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>

#include "StradaPAR.h"
//---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	if( argc != 2 ) {
		printf("usage: par_test par_file");
		exit(1);
	}
	StradaPAR par;
	try {
		printf("Size of StradaPAR is: %zu\n", sizeof( par ));
		printf("start\n");
		par.Read(argv[1]);
		printf("Success!\n");
		printf("%s\n",par.name.c_str());
		par.print_data();
	}catch (std::exception& e ) {
		printf("%s\n", e.what());
		printf("%s\n", par.msg());
		return 0;
	}

    return 1;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>

#include "StradaINT.h"
//---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	if( argc != 2 ) {
		printf("usage: int_test int_file");
		exit(1);
	}
	FILE* fp;
	StradaINT sint;
	INTLinkV2 link;

	try {
		printf("Size of INTLinkV2 is: %zu\n", sizeof( link ));
		printf("start\n");
		sint.Read(argv[1]);
		printf("Success!\n");
		printf("LINK:%5d\n", sint.nLink);
		printf("NODE:%5d\n", sint.nNode);
		printf("Message:%s\n", sint.msg());

        fp = fopen("test.int", "wt");
        if ( fp ) {
            sint.Write(fp);
            fclose(fp);
        }
		LinkPtr link_ptr = sint.getLink(0);
		link_ptr->color = '1';

		link_ptr->aFlag1 = 'X';
		link_ptr->aFlag4[0] = 'Q';
		link_ptr->aFlag4[1] = 'B';
		link_ptr->aFlag5[0] = 'P';
		link_ptr->aFlag5[1] = 'R';
		link_ptr->aFlag5[2] = 'E';
		link_ptr->dummy = '8';
		link_ptr->route = '9';
		link_ptr->bRouteInf = true;

		fp = fopen("test.bin", "wb");
		fwrite(link_ptr.get(), sizeof(link), 1, fp);
		fclose(fp);

	}catch (std::exception& e ) {
		printf("%s\n", e.what());
		printf("%s\n", sint.msg());

		return 0;
	}

    return 1;
}
//---------------------------------------------------------------------------

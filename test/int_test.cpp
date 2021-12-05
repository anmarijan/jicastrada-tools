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
	StradaINT sint;
	INTLinkV2 link;
	try {
		sint.Read(argv[1]);
	}
	catch (std::runtime_error& e) {
		fprintf(stderr, "%s", e.what());
		exit(1);
	}
	try {
		printf("Size of INTLinkV2 is: %zu\n", sizeof( link ));
		printf("start\n");
		sint.Read(argv[1]);
		printf("Success!\n");
		printf("LINK:%5d\n", sint.nLink);
		printf("NODE:%5d\n", sint.nNode);
		printf("Message:%s\n", sint.msg());
		LinkPtr link;
		for (int i = 0; i < sint.nLink; i++) {
			link = sint.getLink(i);
			printf("%s\n", link->name);
		}

		FILE* fp = NULL;
		errno_t error = fopen_s(&fp, "test.int", "wt");
		if ( error == 0 ) {
            sint.Write(fp);
            fclose(fp);
        }

		sint.clear();
		sint.alloc_links(10);
		LinkPtr link_ptr;
		for (int i = 0; i < 10; i++) {
			link_ptr = sint.getLink(i);
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
		}
		error = fopen_s(&fp, "test.bin", "wb");
		if (error == 0 && fp != nullptr) {
			fwrite(link_ptr.get(), sizeof(link), 1, fp);
			fclose(fp);
		}

	}catch (std::exception& e ) {
		printf("%s\n", e.what());
		printf("%s\n", sint.msg());

		return 0;
	}

    return 1;
}
//---------------------------------------------------------------------------

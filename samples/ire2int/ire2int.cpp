/*
結果ファイル(IRE)からリンクファイル(INT)を作成
*/
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>

#include "StradaIRE.h"
//---------------------------------------------------------------------------


int main(int argc, char* argv[])
{
	if( argc != 3 ) {
		puts("JICA STRADA TOOL: Convert IRE file to INT file");
		puts("Usage: ire2int ire_file int_file");
		exit(1);
	}

	FILE* fp;
	StradaIRE ire;

	fp = fopen(argv[2],"rb");
	if (fp) {
		fclose(fp);
		printf("%s already exists\n", argv[2]);
		exit(1);
	}

	try {
		ire.Read(argv[1]);
		ire.WriteAsINTV2(argv[2]);
		printf("Success!");
	}catch (std::exception& e ) {
		printf("%s\n", e.what());
		printf("%s\n", ire.msg.c_str());
		return 0;
	}
    return 1;
}
//---------------------------------------------------------------------------

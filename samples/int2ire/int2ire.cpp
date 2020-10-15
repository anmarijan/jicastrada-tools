//---------------------------------------------------------------------------
#include <stdlib.h>
#include <stdexcept>
//---------------------------------------------------------------------------
#include "StradaINT.h"
#include "StradaIRE.h"
//---------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//  INTファイルからIREファイルを作成
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    StradaIRE  s_ire;
    StradaINT  s_int;
    FILE* fp;

    if( argc != 3 ) {
        puts("Usage: int2ire input.int output.ire");
        exit(1);
    }

    try {
        s_int.Read(argv[1]);
        s_ire.init(s_int);
    } catch (std::runtime_error& e) {
        fprintf(stderr, "File reading error - %s\n", e.what());
        exit(1);
    }

    if( (fp = fopen(argv[2], "wt")) != NULL )
    {
        s_ire.Write(fp);
        fclose(fp);
    } else {
        printf("\nFile creation error\n");
    }

    return 0;
}
//---------------------------------------------------------------------------

/*
パラメータファイル(PAR)内の時間価値を変更
*/
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include "tool.h"
#include "StradaPAR.h"
//---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    FILE* fp;
    StradaPAR par;

    if( argc != 4 ) {
        printf("Usage: parchg PAR_FILE DEF_FILE [output_file]\n");
        puts("If output_file is blank, RAR_FILE will be overwritten.");
        puts("DEL_FILE: a text file of C scanf format as command value");
        printf("\ttval0 - tval9: Time value\n");
        exit (1);
    }
    char buf[2048];
    char command[100];
    char value[100];

    float fx;
    int ix;
    char ch;
    int slen;   //コマンドの文字列長

    try {
        par.Read(argv[1]);
    } catch (std::runtime_error& e) {
        printf("Cannot read file %s\n", argv[1]);
        return (1);
    }

    if( (fp = fopen(argv[2], "rt")) != NULL )
    {
        while( fgets(buf, 2048, fp) != NULL ) {
            sscanf(buf,"%s%s\n", command, value );
            trim(command);
            trim(value);
            slen = strlen(command);
            if( slen == 5 && strncmp(command, "tval", 4) == 0 ) {
                ch = command[4];
                ix = (int) (ch - '0');
                if( ix >= 0 && ix < 10 ){
                    fx = atof(value);
                    par.time_value[ix] = fx ;
                }
            }
        }
        fclose(fp);
        if( (fp = fopen(argv[3], "wt")) != NULL )
        {
            par.Write(fp);
            fclose(fp);
        } else {
            printf("\nFile creat error - %s\n", argv[2]);
            return (-1);
        }

    } else {
        printf("\nFile open error - %s\n", argv[2]);
        return (-1);
    }

    return 0;
}

//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//---------------------------------------------------------------------------
#include "calcaod.h"
#include "tool.h"
//---------------------------------------------------------------------------
//各コマンドの長さは最大10文字とする。
#define COM_MAX_LENGTH 10
//---------------------------------------------------------------------------
const char* commands[] ={ "PLUS","MINUS","SCALAR","MULTIOD" ,"ODMULT","FRATAR",
					"GAD", "OD", "HS","TOTAL", "INSERT", "CLEAR", "EXTRACT","APPEND",
					"SUM", "SET", "GROWTH" };

void usemessage(int n){
	printf("Usage: ");
	if( n == 0 ) {
        puts("aodplus input_file command [arguments]");
        puts("command");
		//OD表作成
        puts("\tPLUS   : C = A + B");
        puts("\tMINUS  : C = A - B");
        puts("\tSCALAR : C = s*A, return matrix multiplied by a scalar");
        puts("\tMULTIOD: return the sum of A[t,i,j] * B[t,i,j]");
		puts("\tODMULT : C[i,j] = A[i,j] * X[i,j]");
        puts("\tFRATAR : C = fratar OD from A and gad");
		puts("\tSUM    : C[i,j] = Sum A[t,i,j]");
        puts("\tGROWTH : C[i,j] = C[i,j] * (G[i] or G[j])");
		//GAD作成
        puts("\tGAD    : gad = generation and attraction of A");
		//OD表表示
		puts("\tOD     : show the value of i, j in table t");
		puts("\tHS     : show the total of generation and attraction of zone i");
		puts("\tTOTAL  : show the total by table");
		//編集
//		puts("\tINSERT : file3 = tables in file1 and tables in file2");
		puts("\tCLEAR  : set 0 to all i,j pairs in table t");
		puts("\tEXTRACT: extract a table from an AOD file");
		puts("\tAPPEND : append tables in an AOD file to another AOD file");
		puts("\tSET INNER : set a value for all inner od pairs (i=j)");
		puts("\tINRATE : show inner trip rates");
		puts("\nHelp: aodplus -h command (ex. aodplus -h PLUS)");
	} else if( n == 1) {
		puts("aodplus file1.aod PLUS file2.aod [file3.aod]");
		puts("file3.aod = file1.aod + file2.aod");
		puts("If file3.aod is null, file1.aod is replaced by the result.");
	} else if( n == 2) {
		puts("aodplus file1.aod MINUS file2.aod [file3.aod]");
		puts("file3.aod = file1.aod - file2.aod");
		puts("If file3.aod is null, file1.aod is replaced by the result.");
	} else if( n == 3) {
		puts("aodplus file1.aod SCALAR n s file2.aod");
		puts("file2.aod = file1.aod multiplied by s");
		puts("n: Table number of n to be multiplied. If n is 0, all tables are multiplied.");
	} else if( n == 4) {
		puts("aodplus file1.aod MULTIOD file2.aod [s]");
        puts("Return the sum of A[t,i,j] * B[t,i,j] /s");
		puts("\twhere, A = file1.aod, B = file2.aod");
	} else if( n == 5) {
		puts("aodplus file1.aod ODMULT scale_file [output.aod]");
		puts("C[i,j] = A[i,j] * X[i,j], for (i,j) in scale_file");
		puts("\twhere, A = file1.aod, X = scale_file");
		puts("Format of scale_file (fixed text format):");
		puts("1-5: zone-i, 6-10:zone-j, 11-20:scale");
	} else if( n == 6) {
		puts("aodplus file1.aod FRATAR gad_file [output.aod]");
	} else if( n == 7) {
		puts("aodplus file1.aod GAD output [Options]");
		puts("Options");
		puts("-c\tCSV format");
	} else if( n == 8) {
		puts("aodplus file1.aod OD t i j");
		puts("t: Table Number");
		puts("i: Col Number");
		puts("j: Row Number");
	} else if (n == 9) {
		puts("aodplus file.aod HS n");
		puts("n: No. of Zone");
	} else if (n == 10) {
		puts("aodplus file1.aod TOTAL");
//	} else if (n ==11) {
//		puts("aodplus file1.aod INSERT file2_aod [file3_aod]");
	} else if (n ==12) {
		puts("aodplus file1.aod CLEAR t [file2.aod]");
		puts("t: Table Number");
		puts("If file2.aod is null, file1.aod is replaced by the result.");
	} else if (n==13) {
		puts("aodplus file1.aod EXTRACT t file2.aod [-c]");
		puts("\tt: Table number to be extracted");
		puts("-c\tCSV format");
	} else if (n==14) {
		puts("aodplus file1.aod APPEND file2.aod file3.aod");
	} else if (n==15) {
		puts("aodplus file1.aod SET INNER n file2.aod");
	} else if (n==16) {
		puts("aodplus file1.aod INRATE");
	} else if (n==17) {
        puts("aodplus input.aod GROWTH [GEN|ATT] growth.txt output.aod");
        puts("groth.txt: vector file for grwoth rates");
        puts("GEN      :  C[i,j] = G[i]*C[i,j]");
        puts("ATT      :  C[i,j] = G[j]*C[i,j]");
    }
	exit(1);
};

////////////////////////////////////////////////////////////////////////////////
//  プログラム本体
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	CalcAOD caod;
	char sw[COM_MAX_LENGTH+1];
	double data;
	int iflag;
	int oc;	//出力AODの番号
	bool check;
	//char ch;
	//optarg = NULL;
	//optind = 3 ;
	//char* input;
	//char* output;

	if( argc < 3 ) {
		usemessage(0);
	}
	strncpy(sw, argv[2],COM_MAX_LENGTH);
	sw[COM_MAX_LENGTH] = '\0';
	conv_upper(sw);

	if( argc == 3 && (argv[1][0] == '-' || argv[1][0] == '/') && (argv[1][1] == 'h' || argv[1][1] == '?')) {
		int i ;
		for(i=0; i < 17; i++ ) {
			if ( strcmp(sw, commands[i]) == 0 ) break;
		}
		usemessage(i+1);
	}

	// file1は必ずAODで最初に読み込む
	// 後のファイルはスイッチに応じて読み込む
	if( caod.ReadStradaAOD(1, argv[1]) == false ) {
		printf("%s\n", caod.errmsg);
		exit(1);
	}

	if( strcmp(sw, "PLUS") == 0 ) {
		if( argc < 4 ) usemessage(1);
		if( caod.ReadStradaAOD(2, argv[3]) == false ) {
			printf("%s\n", caod.errmsg);
			exit(1);
		}
		caod.plus();
		printf("\n%s + %s = ", argv[1], argv[3]);
		if( argc > 4) oc = 4; else oc = 1;
		caod.WriteStradaAOD(3,argv[oc]);
		printf("%s\n", argv[oc]);

	} else if( strcmp(sw, "MINUS") == 0 ) {
		if( argc < 4 ) usemessage(2);
		if( caod.ReadStradaAOD(2, argv[3]) == false ) {
			printf("%s\n", caod.errmsg);
			exit(1);
		}
		caod.minus();
		printf("\n%s - %s = ", argv[1], argv[3]);
		if( argc > 4) oc = 4; else oc = 1;
		caod.WriteStradaAOD(3,argv[oc]);
		printf("%s\n", argv[oc]);

	} else if( strcmp(sw, "SCALAR") == 0 ) {
		if( argc < 6 ) usemessage(3);
        iflag = atoi(argv[3])-1;
		data = atof(argv[4]);
		caod.scalar(iflag, data);
		caod.WriteStradaAOD(3,argv[5]);

	} else if( strcmp(sw, "MULTIOD") == 0 ) {
		if( argc < 4 ) usemessage(4);
		data = 1;
		if( argc > 4 ) {
			data = atof(argv[4]);
			if (data ==0 ) data = 1;
		}
		double fdata[10];
		double total = caod.multiod(argv[3], fdata);
		for(int t=0; t < caod.nTable; t++) {
			printf("%10d", t+1);
		 }
		 puts("   Total  ");
		for(int t=0; t < caod.nTable; t++) {
			printf("%10.0f", fdata[t]/data);
		}
		printf("%10.0f\n", total/data);

	} else if( strcmp(sw, "ODMULT") == 0 ) {
		if( caod.mult_pair(argv[3]) ) {
			caod.WriteStradaAOD(3,argv[4]);
			printf("%s * %s = %s\n", argv[1], argv[3], argv[4]);
		} else {
			printf("%s\n", caod.errmsg );
			exit(1);
		}
	} else if( strcmp(sw, "FRATAR") == 0 ) {    // Fratar法の計算
		if( caod.calc_fratar(argv[3]) ) {
			caod.WriteStradaAOD(3,argv[4]);
			printf("%s\n", argv[4]);
		} else {
			printf("%s\n", caod.errmsg);
			exit(1);
		}
	} else if( strcmp(sw, "GAD") == 0 ) {

		if( argc == 5 && argv[4][0] == '-' && argv[4][1] == 'c' ) check = true;
		else check = false;

		if( caod.make_gad(argv[3], check) ) {
			printf("The file (%s) was saved as a GAD file.\n", argv[3]);
		} else {
			printf("%s\n", caod.errmsg);
			exit(1);
		}

	} else if( strcmp(sw, "INSERT") == 0 ) {

		if( argc < 4 ) usemessage(10);
		if( caod.ReadStradaAOD(2, argv[3]) == false ) {
			printf("%s\n", caod.errmsg);
			exit(1);
		}
		caod.insert();
		printf("\n%s and %s = ", argv[1], argv[3]);
		if( argc > 4 ) oc = 4; else oc = 1;
		caod.WriteStradaAOD(3,argv[oc]);
		printf("%s\n", argv[oc]);

	} else if (strcmp(sw, "OD") == 0 ) {

		if (argc != 6 ) usemessage(8);
		int t = atoi(argv[3]) -1;
		int i = atoi(argv[4]) -1;
		int j = atoi(argv[5]) -1;
		if(t < 0 || i < 0 || j < 0 ) usemessage(8);
		printf("%f\n", caod.getOD(1,t,i,j));

	} else if (strcmp(sw, "CLEAR") == 0 ) {

		if( argc < 4 ) usemessage(11);
		int t = atoi(argv[3]) - 1;
		caod.clear(t);
		if( argc == 4 ) oc = 1; else oc = 4;
		caod.WriteStradaAOD(3, argv[oc]);

	} else if (strcmp(sw, "HS") == 0 ) {
		if( argc < 4 ) usemessage(9);
		int t = atoi(argv[3]);
		printf("ZONE: %d\n", t);
		caod.show_hs(t-1);
	} else if (strcmp(sw, "TOTAL") == 0 ) {
		caod.show_total();

	} else if (strcmp(sw, "EXTRACT") == 0 ) {
		if( argc < 5) usemessage(12);
		int t = atoi(argv[3]) - 1;
		if( argc == 6 && argv[5][0] == '-' && argv[5][1] == 'c' ) check = true;
		else check = false;
		caod.extract(t, check);
		caod.WriteStradaAOD(3, argv[4]);
	} else if (strcmp(sw, "APPEND") == 0 ) {
		if(argc != 5) usemessage(13);
		if( !caod.ReadStradaAOD(2, argv[3]) ) {
			fprintf(stderr, "Cannot read %s\n", argv[3]);
			exit(-1);
		}
		caod.append();
		caod.WriteStradaAOD(3, argv[4]);
	} else if (strcmp(sw, "SUM") == 0 ) {
		if( argc < 4 ) usemessage(14);
		caod.sum();
		caod.WriteStradaAOD(3,argv[3]);
	} else if (strcmp(sw, "SET") == 0 ) {
		if( argc < 6 ) usemessage(15);
		strncpy(sw, argv[3],COM_MAX_LENGTH);
		sw[COM_MAX_LENGTH] = '\0';
		conv_upper(sw);
		if( strcmp(sw, "INNER") == 0 ) {
			data = atof(argv[4]);
			caod.zero_inner(data);
			caod.WriteStradaAOD(3,argv[5]);
		}
	} else if (strcmp(sw, "INRATE")==0 ) {
		caod.show_innertrip_rate();
    } else if (strcmp(sw,"GROWTH")==0) {
        int flag;
        if( argc < 5 ) usemessage(17);
        strncpy(sw, argv[3], COM_MAX_LENGTH);
        sw[COM_MAX_LENGTH] = '\0';
        conv_upper(sw);
        if( strcmp(sw, "GEN") == 0 ) flag = 0;
        else flag = 1;
        if( caod.growth(argv[4],flag) == 0 ) {
            caod.WriteStradaAOD(3,argv[5]);
        } else {
            fprintf(stderr,"File error");
        }
	} else {
		usemessage(0);
	}

	return (0);
}
//---------------------------------------------------------------------------

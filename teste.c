#include <stdio.h>
#include <string.h>


//#define MAXKEYS 3
#define NOKEY "@@@"
#define NIL -1
#define PAGESIZE 50

typedef struct{
    char CodCli[3]; //3 0 1 2  = 3 bytes
	char CodF[3]; //3 0 1 2   = 3 bytes
	int offSet_MainFile; //4 =  4 bytes
} Chave; //10


typedef struct{ 
    int keycount; //4  
    Chave key[3];   //30  (3*10)
    int child[4]; // 16 (4*4) 

} Pagina; //50 

int main(){

	Chave chave;
		Pagina pagina;

	strcpy(chave.CodCli, "01");
	strcpy(chave.CodF, "01");
	chave.offSet_MainFile = 4;

	pagina.key[0] = chave;

	printf("tamanho pagina: %ld\n", sizeof(pagina));
	
	
/*
	char codcli1[3] = "01";
	char codf1[3] = "04";

	char codcli2[3] = "01";
	char codf2[3] = "01";

	strcat(codcli1, codf1);
	strcat(codcli2, codf2);
	//0301 - 0103
	if(strcmp(codcli1, codcli2) > 0){
		printf("1 e maior");
	}else if(strcmp(codcli1, codcli2) < 0){
		printf("2 e maior");
	}else{
		printf("igual");
	}
	printf("%s", codcli1);

	strcpy(codcli1, " ");
	printf("\n");
	printf("a: %s", codcli1);*/
	return 0;
}

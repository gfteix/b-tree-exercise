#include <stdio.h>
#include <string.h>
#include <stdlib.h>


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
	pagina.keycount = 3;

	
	strcpy(chave.CodCli, "01");
	strcpy(chave.CodF, "01");

	pagina.key[0] = chave;
	// ------------- pos = 01
	strcpy(chave.CodCli, "01");
	strcpy(chave.CodF, "04");

	pagina.key[1] = chave;

	strcpy(chave.CodCli, "01");
	strcpy(chave.CodF, "06");
	pagina.key[2] = chave;
//*************//
	Chave key;
	strcpy(key.CodCli, "01");
	strcpy(key.CodF, "05");


	int i=0; 
	char pageKey[6] = "";
	int pos;

	strcat(pageKey, pagina.key[i].CodCli);
	strcat(pageKey, pagina.key[i].CodF);   
// 0, 1, 2
//0101 , 0104, 0106
//***0102***
	int keyCodCli, keyCodF, pageCodCli, pageCodF;

	keyCodCli = atoi(key.CodCli);
	keyCodF = atoi(key.CodF);

	for(i = 0; i < pagina.keycount; i++){ // i = 0 ou i = 1;
		pageCodCli = atoi(pagina.key[i].CodCli);
		pageCodF = atoi(pagina.key[i].CodF);

		if(keyCodCli + keyCodF <= pageCodCli + pageCodF){
			break;
		}
	}
	pos = i;
	printf("pos: %d", pos);

	return 0;
}

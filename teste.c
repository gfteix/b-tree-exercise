#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct{
    char CodCli[3]; //3
	char CodF[3]; //3
	short offSet_MainFile; //2
} Chave; //8

int main(int argc, char *argv[]){
	
	char keyString[6] = "";
	Chave chave;
	strcpy(chave.CodCli, "111");
	strcpy(chave.CodF,"111");

	strncat(keyString, chave.CodCli, 3);
	strncat(keyString, chave.CodF, 3);

	printf("strlen: %ld, %s\n",strlen(keyString), keyString);	
	int valor = atoi(keyString);
	printf("%d", valor);
	return 0;
}

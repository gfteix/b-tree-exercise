#include <stdio.h>
#include <string.h>

int main(){

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
	printf("a: %s", codcli1);
	return 0;
}

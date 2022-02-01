#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define MAXKEYS 3
#define NOKEY '@'
#define NIL -1


typedef struct{
	int qtdInserido;
    int qtdBuscado;
} Controle;

typedef struct{
	char CodCli[3];
	char CodF[3];
	char NomeCli[50];
	char NomeF[50];
	char Genero[50];
} ClienteFilme;

typedef struct{
    char CodCli[3];
	char CodF[3];
} Indice;

typedef struct{
    int keycount;
    Indice key[MAXKEYS];
    int child[MAXKEYS + 1];
} Pagina;

void carrega_arquivo(ClienteFilme **vetor_insere, Indice **vetor_busca, Controle *controle);

int main(){
    Controle *controle = (Controle *)malloc(sizeof(Controle));
    ClienteFilme *vetor_insere = (ClienteFilme*)malloc(sizeof(ClienteFilme));
    Indice *vetor_busca = (Indice*)malloc(sizeof(Indice)); 

    int opcao;
    while (opcao != '5'){
		printf("\n1. Insercao");
		printf("\n2. Listar os dados de todos os clientes");
		printf("\n3. Listar os dados de um cliente específico");
		printf("\n4. Carrega Arquivos");
		printf("\n5. Sair\n");
		scanf("%d", &opcao);
		switch (opcao){
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
                carrega_arquivo(&vetor_insere, &vetor_busca, controle);
				break;
			case 5:
				break;
		}
	}
}

void carrega_arquivo(ClienteFilme **vetor_insere, Indice **vetor_busca, Controle *controle){


	FILE *fd;

	if ((fd = fopen("controle.bin", "rb+")) == NULL)
	{
		printf("Nao foi possivel encontrar o arquivo de controle =(\nVamos criar um...!\n");
		fd = fopen("controle.bin", "wb+");
	}
	if(fread(controle, sizeof(Controle), 1, fd)){
		printf("\nArquivo Controle.bin carregado");
	}else{
		controle->qtdInserido = 0;
        controle->qtdBuscado = 0;
	}
	fclose(fd);


	fd = fopen("insere.bin", "rb+");

	int k = 0;
	ClienteFilme clienteFilmeAux;
	while ((fread(&clienteFilmeAux, sizeof(ClienteFilme), 1, fd)))
	{
		(*vetor_insere) = (ClienteFilme*)realloc((*vetor_insere), (k+1) * sizeof(ClienteFilme));
		(*vetor_insere)[k] = clienteFilmeAux;
		k++;
	}
	printf("\nArquivo Insere.bin carregado");
	fclose(fd);

	fd = fopen("busca.bin", "rb+");
	k = 0;
	Indice buscaAux;
	while (fread(&buscaAux, sizeof(Indice), 1, fd))
	{
		(*vetor_busca) = (Indice *)realloc((*vetor_busca), (k+1) * sizeof(Indice));
		(*vetor_busca)[k] = buscaAux;
		k++;
	}
	printf("\nArquivo busca.bin carregado");
	fclose(fd);
}
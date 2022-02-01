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
	int offSet_MainFile;
} Chave;

typedef struct{
    char CodCli[3];
	char CodF[3];
} Busca;
typedef struct{
    int keycount;
    Chave key[MAXKEYS];
    int child[MAXKEYS + 1];
	//int numPagina;
} Pagina;

void carrega_arquivo(ClienteFilme **vetor_insere, Busca **vetor_busca, Controle *controle);
void inserir_mainFile(ClienteFilme *vetor_insere, Controle *controle);
int inserir_indice();
int inserir_chave(int rrn, Chave Chave);

int getRoot(FILE *btreeFile);

int main(){
    Controle *controle = (Controle *)malloc(sizeof(Controle));
    ClienteFilme *vetor_insere = (ClienteFilme*)malloc(sizeof(ClienteFilme));
    Busca *vetor_busca = (Busca*)malloc(sizeof(Busca)); 

    int opcao;
    FILE* file;
    int validade; //se já existe no indice-> validade = 0; 
    while (opcao != 5){
		printf("\n1. Insercao");
		printf("\n2. Listar os dados de todos os clientes");
		printf("\n3. Listar os dados de um cliente específico");
		printf("\n4. Carrega Arquivos");
		printf("\n5. Sair\n");
		scanf("%d", &opcao);
		switch (opcao){
			case 1:
				validade = inserir_indice(vetor_insere, controle);
				if(validade == 1){
					inserir_mainFile(vetor_insere, controle);
				}else{
					printf("A chave a ser inserida já existe no indice\n");
				}
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
                carrega_arquivo(&vetor_insere, &vetor_busca, controle);
				break;
			case 5:
               
                file = fopen("controle.bin", "rb+");
	            fseek(file, 0, SEEK_SET);
                fwrite(controle, sizeof(Controle), 1, file);
				fclose(file);
        
                break;
		}
	}
    free(vetor_insere);
    free(vetor_busca);
    free(controle);
    return 0;
}

int getRoot(FILE *btreeFile){
    int root;
    fseek(btreeFile, 0, SEEK_SET);
    if(fread(&root, sizeof(int), 1, btreeFile)== NULL){
        printf("ERRO: nao foi possivel encontrar a raiz\n");
        return -1;
    }
    return root;
}

int inserir_indice(ClienteFilme *vetor_insere, Controle *controle){
	FILE* btreeFile;
    int root;
	if(btreeFile = fopen("arvoreb.bin", "rb+") == NULL){
        btreeFile = fopen("arvoreb.bin", "wb+");
		fseek(btreeFile, 0, SEEK_SET);
        root = 4;
		fwrite(&root, sizeof(int), 1, btreeFile);
		
		Chave auxChave;
        stpcpy(auxChave.CodCli,vetor_insere[controle->qtdInserido].CodCli);
        strcpy(auxChave.CodF, vetor_insere[controle->qtdInserido].CodF);
        auxChave.offSet_MainFile = 0;

		Pagina auxPagina;
		auxPagina.key[0] = auxChave;
        auxPagina.child[0] = -1;
        auxPagina.child[1] = -1;
		auxPagina.keycount = 1;
		
		fwrite(&auxPagina, sizeof(Pagina), 1, btreeFile);
        fclose(btreeFile);
		return 1;
    }else{
        Chave novaChave;
        stpcpy(novaChave.CodCli,vetor_insere[controle->qtdInserido].CodCli);
        strcpy(novaChave.CodF, vetor_insere[controle->qtdInserido].CodF);
        novaChave.offSet_MainFile = 160 * controle->qtdInserido; // tamanho fixo * qtdinserido

        root = getRoot(btreeFile);
        int inserir_chave(root, novaChave); // provavelmente tem mais parametros
    }   


}

int inserir_chave(int rrn, Chave Chave /*, promo_r_child, promo_kil*/){
    
    Pagina pagina;
    if(rrn == NIL){
        //
    }else{
        //ler a pagina do rrn correspondente
        //procurar pela chave na pagina -> se já existir, emitir erro
        //-> se nao existir, retornar posição em que deveria estar
    }
    //3
    // [< 2  > , 4]
    // return_page = insert(Pagina.child[1], Chave) -> recursão
}
void inserir_mainFile(ClienteFilme *vetor_insere, Controle *controle){
	FILE* mainFile;
    //3#3#50#50#50;
    //160
	char registro[160];
	sprintf(registro, "%s#%s#%s#%s#%s",
			vetor_insere[controle->qtdInserido].CodCli,
			vetor_insere[controle->qtdInserido].CodF,
			vetor_insere[controle->qtdInserido].NomeCli,
			vetor_insere[controle->qtdInserido].NomeF,
			vetor_insere[controle->qtdInserido].Genero);
            
	if ((mainFile = fopen("mainfile.bin", "rb+")) == NULL)
	{
		printf("Nao foi possivel encontrar o arquivo de controle =(\nVamos criar um...!\n");
		mainFile = fopen("mainfile.bin", "wb+");
	}

	fseek(mainFile, 0, SEEK_END);
	fwrite(registro, sizeof(registro), 1, mainFile);
   
    controle->qtdInserido++;

	fclose(mainFile);
	
}

void carrega_arquivo(ClienteFilme **vetor_insere, Busca **vetor_busca, Controle *controle){
	
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
	Busca buscaAux;
	while (fread(&buscaAux, sizeof(Busca), 1, fd))
	{
		(*vetor_busca) = (Busca *)realloc((*vetor_busca), (k+1) * sizeof(Busca));
		(*vetor_busca)[k] = buscaAux;
		k++;
	}
	printf("\nArquivo busca.bin carregado");
	fclose(fd);


	
	
}


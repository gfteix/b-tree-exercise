#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define MAXKEYS 3
#define NOKEY "@"
#define NIL -1
#define PAGESIZE 34

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
    char CodCli[3]; //3
	char CodF[3]; //3
	short offSet_MainFile; //2
} Chave; //8

typedef struct{
	int teste;
} TESTE;

typedef struct{
    char CodCli[3];
	char CodF[3];
} Busca;
typedef struct{ 
    short keycount; //2 
    Chave key[MAXKEYS];   //30  (8*3) = 24
    short child[MAXKEYS + 1]; // (2*4) = 8
} Pagina; //34

void carrega_arquivo(ClienteFilme **vetor_insere, Busca **vetor_busca, Controle *controle);
void inserir_mainFile(ClienteFilme *vetor_insere, Controle *controle);
int inserir_indice();
int inserir_chave(short rrn, Chave chave, FILE *file);
void lerPagina(short rrn, Pagina *pagina, FILE* file);
int procurar_chave(Chave chave, Pagina *pagina, short *pos);
int create_tree(Chave chave);
int create_root(FILE* file, Chave chave, short left, short right);
int getpage(FILE *file);
int getRoot(FILE *file);
int write_page(short rrn, Pagina *page, FILE *file);

int main(){
    Controle *controle = (Controle *)malloc(sizeof(Controle));
    ClienteFilme *vetor_insere = (ClienteFilme*)malloc(sizeof(ClienteFilme));
    Busca *vetor_busca = (Busca*)malloc(sizeof(Busca)); 
	Pagina teste;
	TESTE testando;
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

int getRoot(FILE *file){
	printf("entrou no  getRoot\n");
    int rrnRoot;
    fseek(file, 0, SEEK_SET);
    if(fread(&rrnRoot, sizeof(int), 1, file)==0){
        printf("ERRO: nao foi possivel encontrar a raiz\n");
        return -1;
    }
    return rrnRoot;
}
//definir um melhor retorno
int inserir_indice(ClienteFilme *vetor_insere, Controle *controle){
	printf("entrou no  inserir_indice\n");
	FILE* file;
    int rrnRoot;
	int promoted;
	
	Chave chaveAux;
	stpcpy(chaveAux.CodCli,vetor_insere[controle->qtdInserido].CodCli);
	strcpy(chaveAux.CodF, vetor_insere[controle->qtdInserido].CodF);
	chaveAux.offSet_MainFile = controle->qtdInserido * 160;
	
	if((file = fopen("arvoreb.bin", "rb+")) == NULL){ //se o arquivo ainda nao existe
	printf("ENTREI NO IF");
		//fclose(file);
		create_tree(chaveAux);
		return 1;
    }else{ //arquivo e árvore já existem
        
        rrnRoot = getRoot(file);
        promoted = inserir_chave(rrnRoot, chaveAux, file); // provavelmente tem mais parametros
		/*if(prometed){
			root = create_root(...);
		}*/
		return 1;
	}   
}
int inserir_chave(short rrn, Chave chave, FILE* file/*, promo_r_child, promo_kil*/){
    printf("entrou no  inserir_chave\n");
	Pagina auxPagina;
	int encontrou;
	short pos;
    if(rrn == NIL){
        //
    }
	lerPagina(rrn, &auxPagina, file); // fornecemos o rrn de uma pagina e enviamos uma pagina aux para termos em memoria a pagina 
    // testar se o auxPagina está vindo com conteudo do lerPagina()
	encontrou = procurar_chave(chave, &auxPagina, &pos);
	
	printf("pos: %d", pos);
	return 1;

}
int procurar_chave(Chave chave, Pagina *pagina, short *pos){
	printf("entrou no procurar_chave\n");
	int i=0; 
	char key[6]; // 6 ? ou nao
	char pageKey[6];
	strcat(key, chave.CodCli);
	strcat(key, chave.CodF);

	strcat(pageKey, pagina->key[i].CodCli);
	strcat(pageKey, pagina->key[i].CodF);
	
	for(i = 1; i < pagina->keycount && strcmp(key, pageKey) > 0; i++){ // i = 0 ou i = 1;
		strcpy(pageKey, " ");
		strcat(pageKey, pagina->key[i].CodCli);
		strcat(pageKey, pagina->key[i].CodF);
	}
	
	*pos = i;
													//1010
	if (*pos < pagina->keycount && strcmp(key, strcat(pagina->key[i].CodCli, pagina->key[i].CodF)) == 0 ){
		return 1;
	}else{
		return 0;
	}

}
void lerPagina(short rrn, Pagina *pagina, FILE* file){
	printf("entrou no ler pagina");
	int endereco;
	endereco = rrn * PAGESIZE + 2;
	fseek(file, endereco, SEEK_SET);
	fread(pagina, sizeof(Pagina), 1, file);
		
}
int create_tree(Chave chave){
	printf("entrou no create_tree");
	short firstRoot = 0; 
	FILE* file;
	file = fopen("arvoreb.bin", "wb+");
	fseek(file, 0, SEEK_SET);
	fwrite(&firstRoot, sizeof(short), 1, file);
	return create_root(file, chave, NIL, NIL);
	fclose(file);
}
int create_root(FILE* file, Chave chave, short left, short right){
	printf("entrou no  create_root\n");
	Pagina pagina;
	short rrn;
	//inicializando a pagina
	for(int i = 0; i < MAXKEYS; i++){
		strcpy(pagina.key[i].CodCli,NOKEY);
		strcpy(pagina.key[i].CodF, NOKEY);
		pagina.key[i].offSet_MainFile = NIL;
		pagina.child[i] = NIL;	
	}
	pagina.child[MAXKEYS] = NIL;
	//fim da inicialização
	pagina.key[0] = chave;
	pagina.key[0].offSet_MainFile = chave.offSet_MainFile;
	printf("\n dentro do create Root: pagina.key[0].offSet_MainFile: %d\n", pagina.key[0].offSet_MainFile);
	pagina.child[0] = left;
	pagina.child[1] = right;
	pagina.keycount = 1;
	
	rrn = getpage(file);
	//getpag
	//offset = rrn * tamPagina = 0 * 50 + tamHeader
	printf("rrn: %d", rrn);
	write_page(rrn,&pagina,file);
	//escrevendo o rrn do root no header
	fseek(file, 0, SEEK_SET);
	fwrite(&rrn, sizeof(short), 1, file);
	return rrn;
}

int getpage(FILE *file){ //retorna o rrn da  ultima pagina
printf("entrou no getpage");
	fseek(file, 0, SEEK_END); 

	short rrn = ftell(file)-2; 
	if(rrn <= 0){
		return 0;
	}

	return rrn/PAGESIZE;  
	

}
int write_page(short rrn, Pagina *page, FILE *file){
	printf("\nrrn dentro do write page: %d", rrn);
	printf("entrou no write_page\n");
	int addr = rrn * PAGESIZE + 2; // 0 * 50 + 2 = 2
	printf("dentro do write page: pagina.key[0].offSet_MainFile: %d\n", page->key[0].offSet_MainFile); 
	printf("dentro do write page: pagina.key[0].chaveCli, chaveF: %s , %s\n", page->key[0].CodCli, page->key[0].CodF);

	printf("\n\ndentro do write page: pagina.key[1].offSet_MainFile: %d\n", page->key[1].offSet_MainFile); 
	printf("\n\ndentro do write page: pagina.key[1].chaveCli, chaveF: %s , %s\n", page->key[1].CodCli, page->key[1].CodF);
	fseek(file, addr, SEEK_SET);
	return fwrite(page, sizeof(Pagina), 1, file);
}

void inserir_mainFile(ClienteFilme *vetor_insere, Controle *controle){
	printf("entrou no  inserir_mainFile\n");
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


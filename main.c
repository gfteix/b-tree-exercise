#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*
TAREFAS:
- Arrumar offsets na árvore b

*/

#define MAXKEYS 3
#define MINKEYS MAXKEYS/2
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
int inserir_indice(ClienteFilme *vetor_insere, Controle *controle);
int inserir_chave(short rrn, Chave chave, FILE* file, short *promo_r_child, Chave *promo_key);
void lerPagina(short rrn, Pagina *pagina, FILE* file);
int procurar_chave(Chave chave, Pagina *pagina, short *pos);
void create_tree(Chave chave);
short create_root(FILE* file, Chave chave, short left, short right);
short getpage(FILE *file);
short getRoot(FILE *file);
void write_page(short rrn, Pagina *page, FILE *file);
void inserir_na_pagina(Chave chave, short r_child, Pagina *p_page);
void split(Chave chave,short r_child, Pagina *p_antiga, Chave *promo_key, short *promo_r_child,Pagina *novaPagina, FILE* file);
void pageinit(Pagina *p_page);
void lista_cliente_especifico(Busca *vetor_busca, Controle *controle);
int buscaChave(short rrn, Chave chaveProcurada, FILE *treeFile, short *keyoffSetMainFile);

int main(){
    Controle *controle = (Controle *)malloc(sizeof(Controle));
    ClienteFilme *vetor_insere = (ClienteFilme*)malloc(sizeof(ClienteFilme));
    Busca *vetor_busca = (Busca*)malloc(sizeof(Busca)); 
	TESTE testando;
    int opcao;
    FILE* file;
    int validade; 
	short teste;
	
	
	
    while (opcao != 5){
		printf("\n1. Insercao");
		printf("\n2. Listar os dados de todos os clientes");
		printf("\n3. Listar os dados de um cliente específico");
		printf("\n4. Carrega Arquivos");
		printf("\n5. Sair\n");
		scanf("%d", &opcao);
		switch (opcao){
			case 1:
				printf("\n%s %s", vetor_insere[controle->qtdInserido].CodCli, vetor_insere[controle->qtdInserido].CodF);
				validade = inserir_indice(vetor_insere, controle);
				if(validade == 1){
					inserir_mainFile(vetor_insere, controle);
					printf("\nChave %s %s inserida com sucesso", vetor_insere[controle->qtdInserido-1].CodCli, vetor_insere[controle->qtdInserido-1].CodF);
				}else{
					printf("\nChave %s %s duplicada", vetor_insere[controle->qtdInserido].CodCli, vetor_insere[controle->qtdInserido].CodF);
				}
				
				break;
			case 2:
				break;
			case 3:
				lista_cliente_especifico(vetor_busca, controle);
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

short getRoot(FILE *file){
    short rrnRoot;
    fseek(file, 0, SEEK_SET);
    fread(&rrnRoot, sizeof(short), 1, file);
    return rrnRoot;
}

int inserir_indice(ClienteFilme *vetor_insere, Controle *controle){
	FILE* file;
    short rrnRoot;
	int promoted;
	Chave promo_key;
	short promo_rrn;

	Chave chaveAux;
	stpcpy(chaveAux.CodCli,vetor_insere[controle->qtdInserido].CodCli);
	strcpy(chaveAux.CodF, vetor_insere[controle->qtdInserido].CodF);
	chaveAux.offSet_MainFile = controle->qtdInserido * 160;
	
	if((file = fopen("arvoreb.bin", "rb+")) == NULL){ //se o arquivo ainda nao existe
		create_tree(chaveAux);
		return 1;
    }else{ //arquivo e árvore já existem
        rrnRoot = getRoot(file);
		if(promoted == -1){
			return 0;
		}
        promoted = inserir_chave(rrnRoot, chaveAux, file, &promo_rrn, &promo_key); // provavelmente tem mais parametros
		if(promoted == -1){
			return 0;
		}
		else if(promoted){
			rrnRoot = create_root(file, promo_key, rrnRoot, promo_rrn);
		}
		fclose(file);
		return 1;
	} 
	  
}

int inserir_chave(short rrn, Chave chave, FILE* file, short *promo_r_child, Chave *promo_key){
	Pagina auxPagina, novaPagina;
	int encontrou;
	short pos;
	int promoted;
	short p_b_rrn;
	Chave p_b_key;

	
    if(rrn == NIL){
        //
		*promo_key = chave;
		*promo_r_child = NIL;
		return 1;
    }
	lerPagina(rrn, &auxPagina, file); // fornecemos o rrn de uma pagina e enviamos uma pagina aux para termos em memoria a pagina 
	encontrou = procurar_chave(chave, &auxPagina, &pos);
	if(encontrou == 1){
		return -1;
	}
	promoted = inserir_chave(auxPagina.child[pos], chave, file, &p_b_rrn, &p_b_key);
	// nao passa daqui
	if(promoted == -1){
		return -1;
	}
	if(!promoted){
		return 0;
	}
	if(auxPagina.keycount < MAXKEYS){
		inserir_na_pagina(p_b_key, p_b_rrn, &auxPagina);
		write_page(rrn, &auxPagina, file);
		return 0;
		
	}else{
		split(p_b_key, p_b_rrn, &auxPagina, promo_key, promo_r_child, &novaPagina, file);
		write_page(rrn, &auxPagina, file);
		write_page(*promo_r_child, &novaPagina, file);
		return 1;
	}
	return 1;

}

void inserir_na_pagina(Chave chave, short r_child, Pagina *p_page){
	int j;
	int keyCodCli, keyCodF, pageCodCli, pageCodF;

	keyCodCli = atoi(chave.CodCli);
	keyCodF = atoi(chave.CodF);
	
	for(j = p_page->keycount; ((keyCodCli + keyCodF) < (atoi(p_page->key[j-1].CodCli) + atoi(p_page->key[j-1].CodF)) && j > 0); j--){
		strcpy(p_page->key[j].CodCli, p_page->key[j-1].CodCli);
		strcpy(p_page->key[j].CodF, p_page->key[j-1].CodF);

		p_page->key[j].offSet_MainFile = p_page->key[j-1].offSet_MainFile;
		
		p_page->child[j+1] =  p_page->child[j];
	}

	p_page->keycount++;
	//p_page->key[j] = chave;
	strcpy(p_page->key[j].CodCli, chave.CodCli);
	strcpy(p_page->key[j].CodF, chave.CodF);
	p_page->key[j].offSet_MainFile = chave.offSet_MainFile;
	p_page->child[j+1] = r_child;
	
}

int procurar_chave(Chave chave, Pagina *pagina, short *pos){
	int i=0; 
	
	int keyCodCli, keyCodF, pageCodCli, pageCodF;

	keyCodCli = atoi(chave.CodCli);
	keyCodF = atoi(chave.CodF);

	for(i = 0; i < pagina->keycount; i++){ // i = 0 ou i = 1;
		pageCodCli = atoi(pagina->key[i].CodCli);
		pageCodF = atoi(pagina->key[i].CodF);

		if(keyCodCli + keyCodF <= pageCodCli + pageCodF){
			break;
		}
	}
	*pos = i;
	
	                                                             
	if (*pos < pagina->keycount && (strcmp(chave.CodCli, pagina->key[*pos].CodCli) == 0
	&& strcmp(chave.CodF, pagina->key[*pos].CodF) == 0 )){
		return 1; // key esta na pagina
	}else{
		return 0; // key nao esta na pagina
	}
}

void lerPagina(short rrn, Pagina *pagina, FILE* file){
	short endereco;
	endereco = rrn * PAGESIZE + 2;
	fseek(file, endereco, SEEK_SET);
	fread(pagina, sizeof(Pagina), 1, file);
		
}

void create_tree(Chave chave){
	short firstRoot = 0; 
	FILE* file;
	file = fopen("arvoreb.bin", "wb+");
	fseek(file, 0, SEEK_SET);
	fwrite(&firstRoot, sizeof(short), 1, file);
	create_root(file, chave, NIL, NIL);
	fclose(file);
}

short create_root(FILE* file, Chave chave, short left, short right){
	Pagina pagina;
	short rrn;
	pageinit(&pagina);
	//fim da inicialização
	pagina.key[0] = chave;
	pagina.key[0].offSet_MainFile = chave.offSet_MainFile;
	pagina.child[0] = left;
	pagina.child[1] = right;
	pagina.keycount = 1;
	
	rrn = getpage(file);
	//offset = rrn * tamPagina = 0 * 50 + tamHeader
	write_page(rrn,&pagina,file);
	//escrevendo o rrn do root no header
	fseek(file, 0, SEEK_SET);
	fwrite(&rrn, sizeof(short), 1, file);
	return rrn;
}

short getpage(FILE *file)
{ //retorna o rrn da  ultima pagina
	fseek(file, 0, SEEK_END); 

	short rrn = ftell(file) - 2; 
	if(rrn <= 0){
		return 0;
	}

	return rrn/PAGESIZE;  
	

}

void write_page(short rrn, Pagina *page, FILE *file){
	int addr = rrn * PAGESIZE + 2; // 0 * 50 + 2 = 2
	fseek(file, addr, SEEK_SET);
	fwrite(page, sizeof(Pagina), 1, file);
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
		printf("\nNao foi possivel encontrar o mainFile =(\nVamos criar um...!\n");
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

void split(Chave chave,short r_child, Pagina *p_antiga, Chave *promo_key, short *promo_r_child,Pagina *novaPagina, FILE* file){
	int j;
	short mid = 1; // onde o split vai ocorrer
	Chave workkeys[MAXKEYS+1]; // temporario para segurar as chaves antes do split
	short workchil[MAXKEYS+2]; // temporario para segurar os ponteiros antes do split


	int chaveCodCli, chaveCodF;
	chaveCodCli = atoi(chave.CodCli);
	chaveCodF = atoi(chave.CodF);

	
	for (j = 0; j < MAXKEYS; j++){   // passando pela pagina para salvar na pagina temporaria work
		strcpy(workkeys[j].CodCli, p_antiga->key[j].CodCli);
		strcpy(workkeys[j].CodF , p_antiga->key[j].CodF);
		workkeys[j].offSet_MainFile = p_antiga->key[j].offSet_MainFile;
		workchil[j] = p_antiga->child[j];
	}
	
	workchil[j] = p_antiga->child[j];

	for (j = MAXKEYS; (chaveCodCli+chaveCodF) < (atoi(workkeys[j-1].CodCli) + atoi(workkeys[j-1].CodF)) && j > 0; j--){
		workkeys[j] = workkeys[j-1];
		workchil[j+1] = workchil[j];
	}
	workkeys[j] = chave;
	workchil[j+1] = r_child;

	*promo_r_child = getpage(file);
	pageinit(novaPagina);

	strcpy(p_antiga->key[0].CodCli, workkeys[0].CodCli);
	strcpy(p_antiga->key[0].CodF, workkeys[0].CodF);
	p_antiga->key[0].offSet_MainFile = workkeys[0].offSet_MainFile;

	p_antiga->child[0] = workchil[0];
	p_antiga->child[1] = workchil[1];

	strcpy(p_antiga->key[1].CodCli, NOKEY);
	strcpy(p_antiga->key[1].CodF, NOKEY);
	p_antiga->key[1].offSet_MainFile = NIL;

	strcpy(p_antiga->key[2].CodCli, NOKEY);
	strcpy(p_antiga->key[2].CodF, NOKEY);
	p_antiga->key[2].offSet_MainFile = NIL;

	p_antiga->child[2] = NIL;	
	p_antiga->child[3] = NIL;
	
	for(j = 0; j < MAXKEYS; j++){
		 //j=2  === > novapagina key [2] = [1+1+2] = [4]
		strcpy(novaPagina->key[j].CodCli, workkeys[mid+1+j].CodCli);  
		strcpy(novaPagina->key[j].CodF, workkeys[mid+1+j].CodF); 
		novaPagina->child[j] = workchil[mid+1+j];
		novaPagina->key[j].offSet_MainFile = workkeys[mid+1+j].offSet_MainFile;
	}
	strcpy(novaPagina->key[2].CodCli, NOKEY);  
	strcpy(novaPagina->key[2].CodF, NOKEY); 
	novaPagina->key[2].offSet_MainFile = NIL;
	// GAMBIARRA ACIMA
	strcpy(promo_key->CodCli, workkeys[mid].CodCli);
	strcpy(promo_key->CodF, workkeys[mid].CodF);
	promo_key->offSet_MainFile = workkeys[mid].offSet_MainFile;
	p_antiga->keycount = 1;
	novaPagina->keycount = 2;
	printf("\nDivisao de no");
	printf("\nChave %s %s promovida", promo_key->CodCli, promo_key->CodF);
}

void pageinit(Pagina *p_page)
{
	int i;
	for(i = 0; i < MAXKEYS; i++){
		strcpy(p_page->key[i].CodCli, NOKEY);
		strcpy(p_page->key[i].CodF, NOKEY);
		p_page->key[i].offSet_MainFile = NIL;
		p_page->child[i] = NIL;	
	}
	p_page->child[MAXKEYS] = NIL;


}

int buscaChave(short rrn, Chave chaveProcurada, FILE *treeFile, short *keyOffSetMainFile){
	Pagina auxPagina;
	short pos;
	int foundKey;

	if(rrn == NIL || *keyOffSetMainFile != NIL){
		return 1;
	}

	lerPagina(rrn, &auxPagina, treeFile);
	foundKey = procurar_chave(chaveProcurada, &auxPagina, &pos);
	printf("\nfoundkey depois de procurar chave na pagina: %d", foundKey);
	if(foundKey == 1){
		*keyOffSetMainFile = auxPagina.key[pos].offSet_MainFile;
		printf("\noffset na recursao: %d", *keyOffSetMainFile);
		return 1;
	}else{   /// foundkey retornar 0 do procurar chave
		foundKey = buscaChave(auxPagina.child[pos], chaveProcurada, treeFile, &foundKey);
		return *keyOffSetMainFile;
	}

	

}

void lista_cliente_especifico(Busca *vetor_busca, Controle *controle){
	Chave chaveProcurada;
	short rrnRoot;
	short keyOffSetMainFile = -1;
	FILE *treeFile;
	FILE *mainFile;
	int foundKey = 0;

	if(treeFile = fopen("arvoreb.bin", "rb+") == NULL){
		printf("\nNão foi possível abrir o arquivo da arvoreB");
		return;
	}

	strcpy(chaveProcurada.CodCli, vetor_busca[controle->qtdBuscado].CodCli);
	strcpy(chaveProcurada.CodF, vetor_busca[controle->qtdBuscado].CodF);
	rrnRoot = getRoot(treeFile);
	printf("rrn da root: %d", rrnRoot);
	foundKey = buscaChave(rrnRoot, chaveProcurada, treeFile, &keyOffSetMainFile);
	if(foundKey == 1){
		printf("\nChave Encontrada!");
		printf("\nTestando depois das recursoes : OffSet da chave no mainFile %d", keyOffSetMainFile);
	}else{
		printf("\nChave não encontrada!");
	}								
	
}


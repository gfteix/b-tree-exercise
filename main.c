#include <stdio.h>
#include <string.h>
#include <stdlib.h>


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
int inserir_indice();
int inserir_chave(short rrn, Chave chave, FILE* file, short *promo_r_child, Chave *promo_key);
void lerPagina(short rrn, Pagina *pagina, FILE* file);
int procurar_chave(Chave chave, Pagina *pagina, short *pos);
int create_tree(Chave chave);
int create_root(FILE* file, Chave chave, short left, short right);
int getpage(FILE *file);
int getRoot(FILE *file);
int write_page(short rrn, Pagina *page, FILE *file);
void inserir_na_pagina(Chave chave, short r_child, Pagina *p_page);
void split(Chave chave,short r_child, Pagina *p_antiga, Chave *promo_key, short *promo_r_child,Pagina *novaPagina, FILE* file);
void pageinit(Pagina *p_page);

int main(){
    Controle *controle = (Controle *)malloc(sizeof(Controle));
    ClienteFilme *vetor_insere = (ClienteFilme*)malloc(sizeof(ClienteFilme));
    Busca *vetor_busca = (Busca*)malloc(sizeof(Busca)); 
	TESTE testando;
    int opcao;
    FILE* file;
    int validade; //se já existe no indice-> validade = 0; 
	short teste;
	
	/*file = fopen("arvoreb.bin", "rb+");
	fseek(file, 26, SEEK_SET);
	fread(&teste, sizeof(short),1, file);
	offset printando certo
	*/
	
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
				printf("offset chave 3 no main file: %d", teste);
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
    short rrnRoot;
	int promoted;
	Chave promo_key;
	short promo_rrn;

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
        promoted = inserir_chave(rrnRoot, chaveAux, file, &promo_rrn, &promo_key); // provavelmente tem mais parametros
		if(promoted){
			rrnRoot = create_root(file, promo_key, rrnRoot, promo_rrn);
		}
		return 1;
	}   
}
int inserir_chave(short rrn, Chave chave, FILE* file, short *promo_r_child, Chave *promo_key){
    printf("entrou no  inserir_chave\n");
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
    // testar se o auxPagina está vindo com conteudo do lerPagina()
	encontrou = procurar_chave(chave, &auxPagina, &pos);
	if(encontrou){
		printf("\nErro: Chave duplicada.");
		return 0;
	}
	promoted = inserir_chave(auxPagina.child[pos], chave, file, &p_b_rrn, &p_b_key);
	// nao passa daqui
	printf("\n%d", promoted);
	if(!promoted){
		return 0;
	}
	if(auxPagina.keycount < MAXKEYS){
		inserir_na_pagina(p_b_key, p_b_rrn, &auxPagina);
		write_page(rrn, &auxPagina, file);
		return 0;
		
	}else{
		printf("vai ter que splitar");
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

		p_page->child[j+1] =  p_page->child[j];
	}
	p_page->keycount++;
	p_page->key[j] = chave;
	p_page->child[j+1] = r_child;
	
}
int procurar_chave(Chave chave, Pagina *pagina, short *pos){
	printf("entrou no procurar_chave\n");
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
		strcpy(pagina.key[i].CodCli, NOKEY);
		strcpy(pagina.key[i].CodF, NOKEY);
		pagina.key[i].offSet_MainFile = NIL;
		pagina.child[i] = NIL;	
	}
	pagina.child[MAXKEYS] = NIL;
	pageinit(&pagina);
	//fim da inicialização
	pagina.key[0] = chave;
	pagina.key[0].offSet_MainFile = chave.offSet_MainFile;
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

	short rrn = ftell(file) - 2; 
	if(rrn <= 0){
		return 0;
	}

	return rrn/PAGESIZE;  
	

}
int write_page(short rrn, Pagina *page, FILE *file){
	int addr = rrn * PAGESIZE + 2; // 0 * 50 + 2 = 2
	fseek(file, addr, SEEK_SET);
	return fwrite(page, sizeof(Pagina), 1, file);
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
		workchil[j] = p_antiga->child[j];
	}
	
	workchil[j] = p_antiga->child[j];

	for (j = MAXKEYS; (chaveCodCli+chaveCodF) < (atoi(workkeys[j-1].CodCli) + atoi(workkeys[j-1].CodF)) && j > 0; j--){
		workkeys[j] = workkeys[j-1];
		workchil[j+1] = workchil[j];
	}
	workkeys[j] = chave;
	workchil[j+1] = r_child;
	//2 3 4 5

	
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
	
	
	
	/*for (j = 0; j < MINKEYS; j++){
		p_antiga->key[j] = workkeys[j];
		p_antiga->child[j] = workchil[j];
		novaPagina->key[j] = workkeys[j+1+MINKEYS];
		novaPagina->child[j] = workchil[j+1+MINKEYS];
		p_antiga->key[j+MINKEYS] = NOKEY;
		p_antiga->child[j+1+MINKEYS] = NIL;
	}
	p_antiga->child[MINKEYS] = workchil[MINKEYS];
	novaPagina->child[MINKEYS] = workchil[j+1+MINKEYS];
	novaPagina->keycount = MAXKEYS - MINKEYS;
	p_antiga->keycount = MINKEYS;
	*promo_key = workkeys[MINKEYS];*/

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

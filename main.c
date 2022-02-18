/*
@Authors 
Gabriel Ferreira Teixeira
Guilherme Henrique Zampronio

 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXKEYS 3
#define MINKEYS MAXKEYS/2
#define NOKEY "@@@"
#define NIL -1
#define PAGESIZE 34

typedef struct{
	int register_index;
	int insert_counter;
    int search_counter;
} Control;

typedef struct{
	char client_code[3];
	char movie_code[3];
	char cli_name[50];
	char movie_name[50];
	char genre[50];
} Register; 

typedef struct{
    char client_code[3]; 
	char movie_code[3]; 
	short offset_in_mainfile; 
} Key;

typedef struct{
    char client_code[3];
	char movie_code[3];
} SearchStruct;

typedef struct{ 
    short keycount;
	short child[MAXKEYS + 1]; 
    Key key[MAXKEYS];  
} Page;

typedef struct{
	short offSet;
	short position;
	short page;
} SearchedClientInfo;

void load_files(Register **registers, SearchStruct **search_indexes, Control *control);
void write_control_file(Control *control);

void insert_into_mainfile(Register *registers, Control *control);
int insert_register(Register *registers, Control *control);
int insert_key(short rrn, Key key, FILE* file, short *promo_r_child, Key *promo_key);

int fetch_key_in_page(Key key, Page *page, short *pos);
int fetch_key_in_tree(short rrn, Key searched_key, FILE *tree_file, SearchedClientInfo *searched_client_info);

void create_tree(Key key);
short create_root(FILE* file, Key key, short left, short right);

void page_init(Page *p_page);
void write_page(short rrn, Page *page, FILE *file);
void read_page(short rrn, Page *page, FILE* file);
void insert_into_page(Key key, short r_child, Page *p_page);
void split(Key key,short r_child, Page *older_page, Key *promo_key, short *promo_r_child,Page *newer_page, FILE* file);
short get_page(FILE *file);
short get_root(FILE *file);

void print_register(int offset);
void print_all(Page *page, FILE* file);
void list_client(SearchStruct *search_indexes, Control *control);

int get_number_of_children(Page *page);

int main(){
    Control *control = (Control *)malloc(sizeof(Control));
    Register *registers = (Register*)malloc(sizeof(Register));
    SearchStruct *search_indexes = (SearchStruct*)malloc(sizeof(SearchStruct)); 
    int option;
    int validate; 
	short root_rnn;	
	FILE* file;
	Page* page =  (Page*)malloc(sizeof(Page));;
	
    while (option != 5){
		printf("\n1. Insercao");
		printf("\n2. Listar os dados de todos os clientes");
		printf("\n3. Listar os dados de um cliente especifico");
		printf("\n4. Load Files");
		printf("\n5. Sair\n");
		scanf("%d", &option);
		switch (option){
			case 1:
				printf("\n%s %s", registers[control->register_index].client_code, registers[control->register_index].movie_code);
				validate = insert_register(registers, control);
				if(validate == 1){
					insert_into_mainfile(registers, control);
					printf("\nChave %s %s inserida com sucesso", registers[control->register_index].client_code, registers[control->register_index].movie_code);
				}else{
					printf("\nChave %s %s duplicada", registers[control->register_index].client_code, registers[control->register_index].movie_code);
				}
				control->register_index++;
				write_control_file(control);
				break;
			case 2:
				file = fopen("arvoreb.bin", "rb+");
				root_rnn = get_root(file);
				read_page(root_rnn, page, file);
				print_all(page, file);
				fclose(file);
				break;
			case 3:
				list_client(search_indexes, control);
				write_control_file(control);
				break;
			case 4:
                load_files(&registers, &search_indexes, control);
				break;
			case 5:
                break;
		}
	}
    free(registers);
    free(search_indexes);
    free(control);
    return 0;
}

short get_root(FILE *file){
    short root_rrn;
    fseek(file, 0, SEEK_SET);
    fread(&root_rrn, sizeof(short), 1, file);
    return root_rrn;
}

int insert_register(Register *registers, Control *control){
    short root_rrn;
	short promo_rrn;
	int promoted=0;
	FILE* file;
	Key promo_key;

	Key keyAux;
	stpcpy(keyAux.client_code,registers[control->register_index].client_code);
	strcpy(keyAux.movie_code, registers[control->register_index].movie_code);
	keyAux.offset_in_mainfile = control->insert_counter * 160;
	
	if((file = fopen("arvoreb.bin", "rb+")) == NULL){ 
		create_tree(keyAux);
		return 1;
    }else{ 
        root_rrn = get_root(file);
		if(promoted == -1){
			return 0;
		}
        promoted = insert_key(root_rrn, keyAux, file, &promo_rrn, &promo_key); // provavelmente tem mais parametros
		if(promoted == -1){
			return 0;
		}
		else if(promoted){
			root_rrn = create_root(file, promo_key, root_rrn, promo_rrn);
		}
		fclose(file);
		return 1;
	} 
	  
}

int insert_key(short rrn, Key key, FILE* file, short *promo_r_child, Key *promo_key){
	Page aux_page, newer_page;
	int encontrou;
	short pos;
	int promoted;
	short p_b_rrn;
	Key p_b_key;

	
    if(rrn == NIL){
		*promo_key = key;
		*promo_r_child = NIL;
		return 1;
    }
	read_page(rrn, &aux_page, file); 
	encontrou = fetch_key_in_page(key, &aux_page, &pos);
	if(encontrou == 1){
		return -1;
	}
	promoted = insert_key(aux_page.child[pos], key, file, &p_b_rrn, &p_b_key);
	if(promoted == -1){
		return -1;
	}
	if(!promoted){
		return 0;
	}
	if(aux_page.keycount < MAXKEYS){
		insert_into_page(p_b_key, p_b_rrn, &aux_page);
		write_page(rrn, &aux_page, file);
		return 0;
		
	}else{
		split(p_b_key, p_b_rrn, &aux_page, promo_key, promo_r_child, &newer_page, file);
		write_page(rrn, &aux_page, file);
		write_page(*promo_r_child, &newer_page, file);
		return 1;
	}
	return 1;

}
void insert_into_page(Key key, short r_child, Page *p_page){
	int j;
	int complete_key;
	char string_key[7] = "";

	int  p_page_complete_key;
	char p_page_string_key[7] = "";

	strncat(string_key, key.client_code, 3); 
	strncat(string_key, key.movie_code, 3); 
	complete_key = atoi(string_key); 

	for(j = p_page->keycount; j > 0; j--){

		strncat(p_page_string_key, p_page->key[j-1].client_code, 3);
		strncat(p_page_string_key, p_page->key[j-1].movie_code, 3);
		p_page_complete_key = atoi(p_page_string_key);
		if(complete_key <= p_page_complete_key){ 
				strcpy(p_page->key[j].client_code, p_page->key[j-1].client_code);
				strcpy(p_page->key[j].movie_code, p_page->key[j-1].movie_code);
				p_page->key[j].offset_in_mainfile = p_page->key[j-1].offset_in_mainfile;
				p_page->child[j+1] =  p_page->child[j];
		}else{
			break;
		}
		p_page_string_key[0] = '\0';
	}
	p_page->keycount++;
	strcpy(p_page->key[j].client_code, key.client_code);
	strcpy(p_page->key[j].movie_code, key.movie_code);
	p_page->key[j].offset_in_mainfile = key.offset_in_mainfile;
	p_page->child[j+1] = r_child;
	
}

int fetch_key_in_page(Key key, Page *page, short *pos){
	int i=0; 
	
	int complete_key;
	char string_key[7] = "";

	int  p_page_complete_key;
	char p_page_string_key[7] = "";

	strncat(string_key, key.client_code, 3);
	strncat(string_key, key.movie_code, 3);
	complete_key = atoi(string_key);

	for(i = 0; i < page->keycount; i++){ 
		strncat(p_page_string_key, page->key[i].client_code, 3);
		strncat(p_page_string_key, page->key[i].movie_code, 3);
		p_page_complete_key = atoi(p_page_string_key);
		if(complete_key <= p_page_complete_key){
			break;
		}
		p_page_string_key[0] = '\0';
	}
	*pos = i;
	
	if (*pos < page->keycount && (strcmp(key.client_code, page->key[*pos].client_code) == 0
	&& strcmp(key.movie_code, page->key[*pos].movie_code) == 0 )){
		return 1; // key is on the page
	}else{
		return 0; // key is't on the page
	}
	return 0;
}

void read_page(short rrn, Page *page, FILE* file){
	short address;
	address = rrn * PAGESIZE + 2;
	fseek(file, address, SEEK_SET);
	fread(page, sizeof(Page), 1, file);
		
}

void create_tree(Key key){
	short first_root = 0; 
	FILE* file;
	file = fopen("arvoreb.bin", "wb+");
	fseek(file, 0, SEEK_SET);
	fwrite(&first_root, sizeof(short), 1, file);
	create_root(file, key, NIL, NIL);
	fclose(file);
}

short create_root(FILE* file, Key key, short left, short right){
	Page page;
	short rrn;
	page_init(&page);
	page.key[0] = key;
	page.key[0].offset_in_mainfile = key.offset_in_mainfile;
	page.child[0] = left;
	page.child[1] = right;
	page.keycount = 1;
	
	rrn = get_page(file);
	write_page(rrn,&page,file);
	fseek(file, 0, SEEK_SET);
	fwrite(&rrn, sizeof(short), 1, file);
	return rrn;
}

short get_page(FILE *file)
{ 
	fseek(file, 0, SEEK_END); 
	short rrn = ftell(file) - 2; 
	if(rrn <= 0){
		return 0;
	}
	return rrn/PAGESIZE;  
}

void write_page(short rrn, Page *page, FILE *file){
	int address = rrn * PAGESIZE + 2; 
	fseek(file, address, SEEK_SET);
	fwrite(page, sizeof(Page), 1, file);
}

void insert_into_mainfile(Register *registers, Control *control){
	FILE* mainfile;
	char _register[160];
	sprintf(_register, "%s#%s#%s#%s#%s",
			registers[control->register_index].client_code,
			registers[control->register_index].movie_code,
			registers[control->register_index].cli_name,
			registers[control->register_index].movie_name,
			registers[control->register_index].genre);
            
	if ((mainfile = fopen("mainfile.bin", "rb+")) == NULL)
	{
		printf("\nmainfile criado.\n");
		mainfile = fopen("mainfile.bin", "wb+");
	}
	fseek(mainfile, 0, SEEK_END);
	fwrite(_register, sizeof(_register), 1, mainfile);
    control->insert_counter++;
	fclose(mainfile);
	
}

void load_files(Register **registers, SearchStruct **search_indexes, Control *control){
	
	FILE *fd;
	if ((fd = fopen("controle.bin", "rb+")) == NULL)
	{
		printf("\nArquivo de controle criado\n");
		fd = fopen("controle.bin", "wb+");
	}
	if(fread(control, sizeof(Control), 1, fd)){
		printf("\nArquivo controle.bin carregado");
	}else{
		control->register_index = 0;
		control->insert_counter = 0;
        control->search_counter = 0;
	}
	fclose(fd);

	fd = fopen("insere.bin", "rb+");

	int k = 0;
	Register register_aux;
	while ((fread(&register_aux, sizeof(Register), 1, fd)))
	{
		(*registers) = (Register*)realloc((*registers), (k+1) * sizeof(Register));
		(*registers)[k] = register_aux;
		k++;
	}
	printf("\nArquivo insere.bin carregado");
	fclose(fd);

	fd = fopen("busca.bin", "rb+");
	k = 0;
	SearchStruct buscaAux;
	while (fread(&buscaAux, sizeof(SearchStruct), 1, fd))
	{
		(*search_indexes) = (SearchStruct *)realloc((*search_indexes), (k+1) * sizeof(SearchStruct));
		(*search_indexes)[k] = buscaAux;
		k++;
	}
	printf("\nArquivo busca.bin carregado");
	fclose(fd);

}

void split(Key key,short r_child, Page *older_page, Key *promo_key, short *promo_r_child,Page *newer_page, FILE* file){
	int j;
	short mid = 1; 
	Key workkeys[MAXKEYS+1]; 
	short workchil[MAXKEYS+2]; 

	int complete_key;
	char string_key[7] = "";

	int  p_page_complete_key;
	char p_page_string_key[7] = "";
/*


*/
	strncat(string_key, key.client_code, 3);
	strncat(string_key, key.movie_code, 3);
	complete_key = atoi(string_key);
	
	for (j = 0; j < MAXKEYS; j++){   // passando pela page para salvar na page temporaria 
		strcpy(workkeys[j].client_code, older_page->key[j].client_code);
		strcpy(workkeys[j].movie_code , older_page->key[j].movie_code);
		workkeys[j].offset_in_mainfile = older_page->key[j].offset_in_mainfile;
		workchil[j] = older_page->child[j];
	}
	
	workchil[j] = older_page->child[j];

	for (j = MAXKEYS;  j > 0; j--){
		strncat(p_page_string_key, workkeys[j-1].client_code, 3);
		strncat(p_page_string_key, workkeys[j-1].movie_code, 3);
		p_page_complete_key = atoi(p_page_string_key);

		if(complete_key <= p_page_complete_key)
		{
			workkeys[j] = workkeys[j-1];
			workchil[j+1] = workchil[j];
		}else{
			break;
		}
		p_page_string_key[0] = '\0';
	}
	workkeys[j] = key;
	workchil[j+1] = r_child;

	*promo_r_child = get_page(file);
	page_init(newer_page);

	strcpy(older_page->key[0].client_code, workkeys[0].client_code);
	strcpy(older_page->key[0].movie_code, workkeys[0].movie_code);
	older_page->key[0].offset_in_mainfile = workkeys[0].offset_in_mainfile;

	older_page->child[0] = workchil[0];
	older_page->child[1] = workchil[1];

	strcpy(older_page->key[1].client_code, NOKEY);
	strcpy(older_page->key[1].movie_code, NOKEY);
	older_page->key[1].offset_in_mainfile = NIL;

	strcpy(older_page->key[2].client_code, NOKEY);
	strcpy(older_page->key[2].movie_code, NOKEY);
	older_page->key[2].offset_in_mainfile = NIL;

	older_page->child[2] = NIL;	
	older_page->child[3] = NIL;
	
	for(j = 0; j < MAXKEYS; j++){
		strcpy(newer_page->key[j].client_code, workkeys[mid+1+j].client_code);  
		strcpy(newer_page->key[j].movie_code, workkeys[mid+1+j].movie_code); 
		newer_page->child[j] = workchil[mid+1+j];
		newer_page->key[j].offset_in_mainfile = workkeys[mid+1+j].offset_in_mainfile;
	}
	strcpy(newer_page->key[2].client_code, NOKEY);  
	strcpy(newer_page->key[2].movie_code, NOKEY); 
	newer_page->key[2].offset_in_mainfile = NIL;
	strcpy(promo_key->client_code, workkeys[mid].client_code);
	strcpy(promo_key->movie_code, workkeys[mid].movie_code);
	promo_key->offset_in_mainfile = workkeys[mid].offset_in_mainfile;
	older_page->keycount = 1;
	newer_page->keycount = 2;
	printf("\nDivisao de no");
	printf("\nChave %s %s promovida", promo_key->client_code, promo_key->movie_code);
}

void page_init(Page *p_page)
{
	int i;
	for(i = 0; i < MAXKEYS; i++){
		strcpy(p_page->key[i].client_code, NOKEY);
		strcpy(p_page->key[i].movie_code, NOKEY);
		p_page->key[i].offset_in_mainfile = NIL;
		p_page->child[i] = NIL;	
	}
	p_page->child[MAXKEYS] = NIL;


}

int fetch_key_in_tree(short rrn, Key searched_key, FILE *tree_file, SearchedClientInfo *searched_client_info){
	Page aux_page;
	short pos;
	int found_key;

	if(rrn != NIL && searched_client_info->offSet != NIL){
		return 1;
	}
	if(rrn == NIL){
		return 0;
	}
	read_page(rrn, &aux_page, tree_file);
	found_key = fetch_key_in_page(searched_key, &aux_page, &pos);
	if(found_key == 1){
		searched_client_info->offSet = aux_page.key[pos].offset_in_mainfile;
		searched_client_info->page = rrn;
		searched_client_info->position = pos;
		return 1;
	}else{   /// foundkey retornar 0 do procurar key
		found_key = fetch_key_in_tree(aux_page.child[pos], searched_key, tree_file, &*searched_client_info);
		if(found_key == 1){
			return 1;
		}else{
			return 0;
		}
	}


	

}

void list_client(SearchStruct *search_indexes, Control *control){
	short root_rrn;
    int found_key = 0;
	SearchedClientInfo searched_client_info;
	Key searched_key;
	FILE *tree_file;
	searched_client_info.offSet = -1;
	tree_file = fopen("arvoreb.bin", "rb+");
	
	strcpy(searched_key.client_code, search_indexes[control->search_counter].client_code);
	strcpy(searched_key.movie_code, search_indexes[control->search_counter].movie_code);
	root_rrn = get_root(tree_file);

	found_key = fetch_key_in_tree(root_rrn, searched_key, tree_file, &searched_client_info);
	if(found_key == 1){
		printf("\nChave %s %s encontrada, page %d, position %d", searched_key.client_code, 
		searched_key.movie_code, searched_client_info.page, searched_client_info.position);
		print_register(searched_client_info.offSet);
		control->search_counter++;
	}else{
		printf("\nChave %s %s nao encontrada!", searched_key.client_code, searched_key.movie_code);
		control->search_counter++;
	}		
	fclose(tree_file);

}
int get_number_of_children(Page *page){
	int num_children=0;
	for(int i=0; i<4; i++){
		if(page->child[i] != -1){
			num_children++;
		}
	}
	return num_children;

}

void print_register(int offset){
	FILE *mainfile;
	char _register[160];
	mainfile = fopen("mainfile.bin", "rb+");
	fseek(mainfile, offset, SEEK_SET);
	fread(_register, sizeof(char), 160, mainfile);
	printf("\n%s", _register);

}
void print_all(Page *page, FILE* file){
	Page aux_page;
	if(page != NULL){
		if(get_number_of_children(page) == 0){
			for(int j=0; j<page->keycount; j++){
				print_register(page->key[j].offset_in_mainfile);
			}
		}
		for(int i=0; i< get_number_of_children(page); i++){
			read_page(page->child[i], &aux_page, file);
			print_all(&aux_page, file);
			if( i < page->keycount){
				print_register(page->key[i].offset_in_mainfile);
			}
		}
	}
}

void write_control_file(Control *control){
	FILE* file;
	file = fopen("controle.bin", "rb+");
	fseek(file, 0, SEEK_SET);
	fwrite(control, sizeof(Control), 1, file);
	fclose(file);
}

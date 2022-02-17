#include<stdio.h>
#include<conio.h>
#include<string.h>

int main() {
    FILE *fd;
    
    //////////////////////////////   
    struct CliF {
        char CodCli[3];
        char CodF[3];
        char NomeCli[50];
        char NomeF[50];
        char Genero[50];
    } vet[11] = {{"00", "01", "Nome-00", "Filme-01", "Gen-01"},              
                {"00", "02", "Nome-00", "Filme-02", "Gen-02"},
				{"00", "03", "Nome-00", "Filme-03", "Gen-03"},
				{"00", "04", "Nome-00", "Filme-04", "Gen-04"},
				{"00", "05", "Nome-00", "Filme-05", "Gen-05"},
				{"00", "06", "Nome-00", "Filme-06", "Gen-06"},
				{"00", "07", "Nome-00", "Filme-07", "Gen-07"},
				{"00", "08", "Nome-00", "Filme-08", "Gen-08"},
				{"00", "09", "Nome-00", "Filme-09", "Gen-09"},
				
				{"00", "10", "Nome-00", "Filme-10", "Gen-10"},
				{"00", "10", "Nome-00", "Filme-10", "Gen-10"}};//duplicada
       
    fd = fopen("insere.bin", "w+b");
    fwrite(vet, sizeof(vet), 1, fd);
    fclose(fd);
    
    //////////////////////////////
	struct busca {
        char CodCli[3];
        char CodF[3];
    } vet_b[5] = {{"00","10"},
                  {"00","08"},
				  {"00","04"},
				  {"00","03"},
				  {"00","00"}};//não encontrada
    
    fd = fopen("busca.bin", "w+b");
    fwrite(vet_b, sizeof(vet_b), 1, fd);
    fclose(fd);    
}


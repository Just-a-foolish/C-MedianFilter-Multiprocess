#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>

/*---------------------------------------------------------------------*/
#pragma pack(1)

/*---------------------------------------------------------------------*/
struct cabecalho {
	unsigned short tipo;
	unsigned int tamanho_arquivo;
	unsigned short reservado1;
	unsigned short reservado2;
	unsigned int offset;
	unsigned int tamanho_image_header;
	int largura;
	int altura;
	unsigned short planos;
	unsigned short bits_por_pixel;
	unsigned int compressao;
	unsigned int tamanho_imagem;
	int largura_resolucao;
	int altura_resolucao;
	unsigned int numero_cores;
	unsigned int cores_importantes;
}; 
typedef struct cabecalho CABECALHO;

struct rgb{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
};
typedef struct rgb RGB;

/*---------------------------------------------------------------------*/

int compare_function(const void *a, const void *b){
	int *x = (int *) a;
	int *y = (int *) b;
	return *x - *y;
}

void filtroMediana(int largura, int inicio, int final, RGB *vetor, int filtro){

	int matrizAuxBlue[500];
	int matrizAuxGreen[500];
	int matrizAuxRed[500];
	
	int i, j, k, l;
	int a = 0;
	int indice = filtro/2;
	int meio = ((filtro*filtro)/2);
	int tamanhoSort = (filtro*filtro)*4;
		
	for(i=inicio+4; i<final-filtro; i++){
	  for(j=filtro-2; j<largura-filtro; j++){
	    for(k=indice*(-1); k<=indice; k++){
	      for(l=indice*(-1); l<=indice; l++){
	     	matrizAuxRed[a]  =vetor[(k*largura) + (i*largura+j) + l].red;
	     	matrizAuxGreen[a]=vetor[(k*largura) + (i*largura+j) + l].green;
	     	matrizAuxBlue[a] =vetor[(k*largura) + (i*largura+j) + l].blue;
	     	a++;      	
	     }
	  }
	  a=0;
	  qsort(matrizAuxRed, tamanhoSort, sizeof(*matrizAuxRed), compare_function);
	  qsort(matrizAuxBlue, tamanhoSort, sizeof(*matrizAuxBlue), compare_function);
	  qsort(matrizAuxGreen, tamanhoSort, sizeof(*matrizAuxGreen), compare_function);
	  vetor[i * largura + j].red = matrizAuxRed[meio];
	  vetor[i * largura + j].blue = matrizAuxBlue[meio];
	  vetor[i * largura + j].green = matrizAuxGreen[meio];	  
	 }
	}
}


int main(int argc, char **argv ){

	CABECALHO cabecalho;
	RGB pixel;
	int i, j;

	
	// DADOS DE INICIALIZA????O
	char entrada[20] = {'b','o','r','b','o','l','e','t','a','.','b','m','p','\0'};
	char saida[20] = {'t','e','s','t','e','\0'};
	int quantProcessos = 3;
	int tamanhoMascara = 7;
	
/*---------------------------------------------------------------------*/
			
	FILE *fin = fopen(entrada, "rb");

	if ( fin == NULL ){
		printf("Erro ao abrir o arquivo %s\n", entrada);
		exit(0);
	}  

	FILE *fout = fopen(saida, "wb");

	if ( fout == NULL ){
		printf("Erro ao abrir o arquivo %s\n", saida);
		exit(0);
	}  

	// l?? e escreve o cabe??alho no novo arquivo
	fread(&cabecalho, sizeof(CABECALHO), 1, fin);
	fwrite(&cabecalho, sizeof(CABECALHO), 1, fout);
	
	// gera espa??o de mem??ria compartilhada que ir?? receber a imagem
	int chave = 4;
	int shmid = shmget(chave, cabecalho.altura*cabecalho.largura*sizeof(RGB), IPC_CREAT | 0600); 
	RGB *vetor = shmat(shmid, 0, 0);
	
	for(i=0; i<cabecalho.altura; i++){
		int ali = (cabecalho.largura * 3) % 4;
		
		if (ali != 0){
			ali = 4 - ali;
		}

		for(j=0; j<cabecalho.largura; j++){
			fread(&vetor[i * cabecalho.largura + j], sizeof(RGB), 1, fin);
		}
		
		unsigned char aux;
		for(j=0; j<ali; j++){
			fwrite(&aux, sizeof(unsigned char), 1, fout);
		}
	}
		
	//SEQUENCIAL	
	if(quantProcessos == 1){
		filtroMediana(cabecalho.largura, 0 ,cabecalho.altura, vetor, tamanhoMascara);
	}
	
	 //2 PROCESSOS
	if(quantProcessos == 2){
		int pid;
		pid = fork();
		
		if(pid > 0){
		filtroMediana(cabecalho.largura,  0 ,cabecalho.altura/2+5, vetor, tamanhoMascara);
		wait(NULL);
		} else {
		filtroMediana(cabecalho.largura, 200 ,cabecalho.altura, vetor, tamanhoMascara);
		}			
	}
	
	//3 PROCESSOS
	if(quantProcessos == 3){	
	int pid;

	pid = fork();

		if (pid > 0){
			pid = fork();
			if ( pid > 0 ){
				filtroMediana(cabecalho.largura,  0 ,138, vetor, 				tamanhoMascara);
				wait(NULL);
				wait(NULL);
			}
			else{
			filtroMediana(cabecalho.largura, 134 , 272 , vetor, tamanhoMascara);
			}
		}		
		else{
		filtroMediana(cabecalho.largura, 266 , 400 , vetor, tamanhoMascara);
		}
	} 
	
	//4 PROCESSOS
	if(quantProcessos == 4){	
	
	int pid;

	pid = fork();

	if (pid >0){
		pid = fork();
		if ( pid > 0 ){
			pid = fork();
			if ( pid > 0 ){
				filtroMediana(cabecalho.largura, 0 , 105 , vetor, 					tamanhoMascara);
				wait(NULL);
				wait(NULL);
				wait(NULL);
			}
			else{
				filtroMediana(cabecalho.largura, 100 , 205 , vetor, 					tamanhoMascara);
			}
		}
		else{
			filtroMediana(cabecalho.largura, 200 , 305 , vetor, tamanhoMascara);
		}
	}
	else{
		filtroMediana(cabecalho.largura, 300 , 400 , vetor, tamanhoMascara);
	}
	}
	


			
	// escreve o vetor com o filtro aplicado no arquivo de sa??da
	for(i=0; i<cabecalho.altura; i++){
		int ali = (cabecalho.largura * 3) % 4;		
		if (ali != 0){
			ali = 4 - ali;
		}

		for(j=0; j<cabecalho.largura; j++){
			fwrite(&vetor[i * cabecalho.largura + j], sizeof(RGB), 1, fout);
		}
		
		unsigned char aux;
		for(j=0; j<ali; j++){
			fwrite(&aux, sizeof(unsigned char), 1, fout);
		}
	}	

	shmdt(vetor);
	fclose(fin);
	fclose(fout);
	
		
}
/*---------------------------------------------------------------------*/



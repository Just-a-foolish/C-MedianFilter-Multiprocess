#include <stdio.h>
#include <stdlib.h>

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


int main(int argc, char **argv ){


	CABECALHO cabecalho;
	RGB pixel;
	int i, j;
	
	char entrada[100] = {'b','o','r','b','o','l','e','t','a','.','b','m','p','\0'};
	char saida[100] = {'t','e','s','t','e','\0'};
	int quantProcessos = 1;
	int mascara = 3;
	
	//matriz teste para sort de mascara 3x3
	int matrizAux[9];
	
	
	/*printf("Digite o nome do arquivo de entrada:\n");
	scanf("%s", entrada);

	printf("Digite o nome do arquivo de saida:\n");
	scanf("%s", saida);

	printf("Digite a quantidade de processos utilizados:\n");
	scanf("%d", quantProcessos);
	
	printf("Digite o tamanho da máscara utilizada:\n");
	scanf("%d", mascara);
	*/		
	
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

	// lê e escreve o cabeçalho no novo arquivo
	fread(&cabecalho, sizeof(CABECALHO), 1, fin);
	fwrite(&cabecalho, sizeof(CABECALHO), 1, fout);
	
	// aloca a matriz da imagem em um vetor
	RGB *vetor = (RGB *) malloc(cabecalho.altura * cabecalho.largura * sizeof(RGB)); 
	
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
	
	
	
	// aplica o filtro mediana
	for(i=1; i<cabecalho.altura-1; i++){
		for(j=1; j<cabecalho.largura-1; j++){
			//azul
			matrizAux[0] = vetor[(i-1) * cabecalho.largura + (j-1)].blue;
			matrizAux[1] = vetor[(i-1) * cabecalho.largura + j].blue;
			matrizAux[2] = vetor[(i-1) * cabecalho.largura + (j+1)].blue;
			matrizAux[3] = vetor[i * cabecalho.largura + (j-1)].blue;
			matrizAux[4] = vetor[i * cabecalho.largura + j].blue;
			matrizAux[5] = vetor[i * cabecalho.largura + (j+1)].blue;
			matrizAux[6] = vetor[(i+1) * cabecalho.largura + (j-1)].blue;
			matrizAux[7] = vetor[(i+1) * cabecalho.largura + j].blue;
			matrizAux[8] = vetor[(i+1) * cabecalho.largura + (j+1)].blue;
						
			qsort(matrizAux, sizeof(matrizAux), sizeof(*matrizAux),compare_function);
			
			vetor[i * cabecalho.largura + j].blue = matrizAux[4];
			//verde
			matrizAux[0] = vetor[(i-1) * cabecalho.largura + (j-1)].green;
			matrizAux[1] = vetor[(i-1) * cabecalho.largura + j].green;
			matrizAux[2] = vetor[(i-1) * cabecalho.largura + (j+1)].green;
			matrizAux[3] = vetor[i * cabecalho.largura + (j-1)].green;
			matrizAux[4] = vetor[i * cabecalho.largura + j].green;
			matrizAux[5] = vetor[i * cabecalho.largura + (j+1)].green;
			matrizAux[6] = vetor[(i+1) * cabecalho.largura + (j-1)].green;
			matrizAux[7] = vetor[(i+1) * cabecalho.largura + j].green;
			matrizAux[8] = vetor[(i+1) * cabecalho.largura + (j+1)].green;
			
			qsort(matrizAux, sizeof(matrizAux), sizeof(*matrizAux),compare_function);			
			
			vetor[i * cabecalho.largura + j].green = matrizAux[4];
			//vermelho
			matrizAux[0] = vetor[(i-1) * cabecalho.largura + (j-1)].red;
			matrizAux[1] = vetor[(i-1) * cabecalho.largura + j].red;
			matrizAux[2] = vetor[(i-1) * cabecalho.largura + (j+1)].red;
			matrizAux[3] = vetor[i * cabecalho.largura + (j-1)].red;
			matrizAux[4] = vetor[i * cabecalho.largura + j].red;
			matrizAux[5] = vetor[i * cabecalho.largura + (j+1)].red;
			matrizAux[6] = vetor[(i+1) * cabecalho.largura + (j-1)].red;
			matrizAux[7] = vetor[(i+1) * cabecalho.largura + j].red;
			matrizAux[8] = vetor[(i+1) * cabecalho.largura + (j+1)].red;
			
			qsort(matrizAux, sizeof(matrizAux), sizeof(*matrizAux),compare_function);			
			
			vetor[i * cabecalho.largura + j].red = matrizAux[4];		
		}
	}
	
	
		
	// escreve o vetor com o filtro aplicado no arquivo de saída
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
		
	//getchar();
	free(vetor);
	fclose(fin);
	fclose(fout);
}
/*---------------------------------------------------------------------*/



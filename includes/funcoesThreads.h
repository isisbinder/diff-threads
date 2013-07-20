#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define ERR_ALOC "Erro na alocação de memória na thread principal.\n"

typedef struct {
         unsigned int tamanhoArquivo; // Chave da lista.
         unsigned int tamanhoLista; // Tamanho da lista dos nomes de arquivos unificados.
         char ** caminho; // Lista dos arquivos de tamanho correspondente a <i>tamanhoArquivo</i> que foram unificados.
} Unificados;

// Utilizada para passar os nomes dos arquivos para as threads comparadoras.
typedef struct {
         char * arquivo1;
         char * arquivo2;
} dadosThread;

int buscaTamanho(int);
void *thread_principal();
void *thread_comparadora(void *);
char *repetido(const char *,const Unificados );
void libera();


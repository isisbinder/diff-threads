#include "checarArgumentos.h"
#include "constroiLista.h"
#include "funcoesThreads.h"
#include "tipoArquivos.h"
#define MAX_THREADS 32
#define ERR_THR "Número de threads maior do que o permitido (%d).\n"

bool isVerbose = false;
int threadQtd = 2;
extern int arquivosExistentes;
extern char **lista;
 
void libera() {
   for(int i=0;i<arquivosExistentes;i++) {
         free(lista[i]);
         lista[i] = NULL;
   }
}

int main(int argc, char **argv) {
         // O diretório deve ser o último argumento oferecido.
         checarArgumentos(argc,argv);
 
         // Verificar limite de threads.
         if (threadQtd > MAX_THREADS) {
                 fprintf(stderr,ERR_THR,MAX_THREADS);
                 exit (1);
         }
        // Verificar se diretório existe
        struct stat temp;
        if (stat(argv[argc-1],&temp) == -1) {
                 fprintf(stderr,"%m\n");
                 exit(1);
         }
 
         // Criar lista dos arquivos.
         lista = (char**)malloc(0);
         ftw(argv[argc-1], &adicionaNome,20);

         // Criar thread distribuidora.
         pthread_t distribuidora;
         pthread_create(&distribuidora, NULL, thread_principal, NULL);
         int threadError;
         if ( (threadError = pthread_join(distribuidora, NULL)) ) {
                 fprintf(stderr,"%m - (%d)",threadError);
                 free(lista);
                 lista = NULL;
                 exit(1);
         }

         free(lista);
         lista = NULL;
         return 0;
}


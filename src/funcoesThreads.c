#include "funcoesThreads.h"
#include <stdbool.h>
#include <string.h>

Unificados * listaUnificados;
unsigned int tamanhoUnificados;

extern bool isVerbose;
extern int arquivosExistentes;
extern int threadQtd;
extern char ** lista;
extern bool diffPreparacao(const char *fp1, const char*fp2);

pthread_mutex_t mutex1, mutex2, mutex3,mutex4;
/** @brief Procura na lista dos arquivos unificados por um registro relacionado ao tamanho de    arquivo
  *   passado como argumento.
  *   @param Tamanho do arquivo a ser procurado.
  *   @return -1 se o tamanho não for encontrado e, caso contrário, sua posição na lista.
  */
int buscaTamanho(int tamanhoDoArquivo) {
        for(int i=0; i<tamanhoUnificados; i++) {
                 if (listaUnificados[i].tamanhoArquivo == tamanhoDoArquivo)
                         return i;
         }
         return -1;
}


/** @brief Realiza uma busca sequencial na lista de arquivos unificados para evitar nomes
  *   repetidos.
  *   @param Arquivo a ser procurado.
  *   @param Estrutura que possivelmente contém o nome do arquivo pesquisado.
  *   @return <i>NULL</i> se a chave não foi encontrada. Retorna o endereço dela na lista cont  ida na
  *     estrutura caso contrário.
  */
char * repetido(const char * chave, const Unificados celula) {
         for(int i=0; i< celula.tamanhoLista; i++)
                 if (!strcmp(chave, celula.caminho[i]))
                         return celula.caminho[i];
         return NULL;
}


/** @brief Chama as funções em <i>confereArquivos.c</i>.
  *   @details Caso seja feito algum hardlink, adiciona um elemento a uma lista de estruturas,
  *   que contém o tamanho do arquivo unificado e uma lista de nomes dos links criados.
  */
void *thread_comparadora(void * compartilhamento) {
         char * arq2;
         char * arq1;
         int posicao;
         struct stat st;

         arq2 = (char*) malloc(0);
         arq1 = (char*) malloc(0);
         while(1) {
                 pthread_mutex_lock(&mutex1);
                 arq2 = (char*)realloc(arq2, (strlen(((dadosThread*)compartilhamento)->arquivo2)+1) * sizeof(char));
                 strcpy(arq2, ((dadosThread*)compartilhamento)->arquivo2);
 
                 arq1 = (char*) realloc(arq1, (strlen(((dadosThread *)compartilhamento)->arquivo1)+1) * sizeof(char));
                 strcpy(arq1, ((dadosThread*)compartilhamento)->arquivo1);
 
                 // Assinala que a thread principal pode modificar os índices.
                 pthread_mutex_unlock(&mutex2);
 
                 if (diffPreparacao(arq1, arq2)) {
                         pthread_mutex_lock(&mutex3);
 
                         /* Para criar o hardlink é necessário remover o arquivo que se tornará o link, caso contrário,
                          * a função indica o erro em errno.
                          */
                         remove(arq2);
                         link(arq1, arq2);
 
                         if (isVerbose) {
                                  stat(arq1, &st);
                                  posicao = buscaTamanho(st.st_size);

                                 /* Se o tamanho já existe na lista de unificados, simplesmente adiciona o caminho do arquivo na
                                  * lista interna.
                                  */
                                  if (posicao > -1) {
                                          if ( repetido(arq1,listaUnificados[posicao]) == NULL ) {
                                                  listaUnificados[posicao].caminho = (char**)realloc(listaUnificados[posicao].caminho, (listaUnificados[posicao].tamanhoLista+1)*sizeof(char*));
                                                  listaUnificados[posicao].caminho[listaUnificados[posicao].tamanhoLista] = (char*)malloc(strlen(arq1));
                                                  strcpy(listaUnificados[posicao].caminho[listaUnificados[posicao].tamanhoLista], arq1);
                                                  listaUnificados[posicao].tamanhoLista++;
                                           }
                                  } else {
                                         listaUnificados = (Unificados*)realloc(listaUnificados,(tamanhoUnificados+1)*sizeof(Unificados));
                                         listaUnificados[tamanhoUnificados].tamanhoArquivo = st.st_size;
                                         listaUnificados[tamanhoUnificados].caminho = (char**)malloc(1*sizeof(char *));
                                         listaUnificados[tamanhoUnificados].caminho[0] = (char*) malloc((strlen(arq1)+1)*sizeof(char));
                                         strcpy(listaUnificados[tamanhoUnificados].caminho[0], arq1);
                                         listaUnificados[tamanhoUnificados].tamanhoLista = 1;
                                         tamanhoUnificados++;
 
                                 }
                        }
 
                        pthread_mutex_unlock(&mutex3);

                } //FIM DO IF DE DIFF

                 pthread_mutex_unlock(&mutex4);
 
        } //FIM DO WHILE

        free(arq1);
        free(arq2);
        arq1 = arq2 = NULL;
}


/** @brief Imprime a lista dos arquivos unificados (os que foram transformados em hardlinks).
  */
void imprimeUnificados() {
         int idxTemp;
         while (tamanhoUnificados > 0) {
                 puts("---------------------------------------------------------------------");
                 printf("Arquivos unificados de tamanho %d\n",listaUnificados[tamanhoUnificados-1].tamanhoArquivo);
                 idxTemp = listaUnificados[tamanhoUnificados-1].tamanhoLista;
                 while ( idxTemp > 0) {
                         printf("\t%s\n",listaUnificados[tamanhoUnificados-1].caminho[idxTemp-1]);
                         idxTemp--;
                 }
                 tamanhoUnificados--;
         }
 }


/** @brief Thread que cria o pool e distribui o trabalho.
  */
void *thread_principal() {
         dadosThread compartilhamento;
         compartilhamento.arquivo1 = (char*)malloc(0);
         compartilhamento.arquivo2 = (char*)malloc(0);
 
         if(compartilhamento.arquivo1 == NULL || compartilhamento.arquivo2 == NULL) {
                 fprintf(stderr,ERR_ALOC);
                 pthread_exit(NULL);
         }
         // Inicializar lista de arquivos unificados.
         listaUnificados = (Unificados*) malloc(0);
 
         pthread_t listaThreads[threadQtd-1];
         int indice;
         unsigned int idxFixo = 0;
         unsigned int idxVar = 1;
         tamanhoUnificados = 0;

         // Código de tratamento de erro omitido.
         pthread_mutex_init(&mutex1, 0);
         pthread_mutex_lock(&mutex1);
         pthread_mutex_init(&mutex2, 0);
         pthread_mutex_lock(&mutex2);
         pthread_mutex_init(&mutex4,0);
         pthread_mutex_lock(&mutex4);
 
         // O mutex3 deve ser destrancado.
         pthread_mutex_init(&mutex3, 0);
 
         for(indice = 0; indice < threadQtd-1; indice++)
                 pthread_create(&listaThreads[indice], NULL, thread_comparadora, &compartilhamento);

         while(1) {
                 compartilhamento.arquivo1 = (char*)realloc(compartilhamento.arquivo1, (strlen(lista[idxFixo])+1)*sizeof(char));
                 strcpy(compartilhamento.arquivo1, lista[idxFixo]);
                 compartilhamento.arquivo2 = (char*)realloc(compartilhamento.arquivo2, (strlen(lista[i  dxVar])+1)*sizeof(char));
                 strcpy(compartilhamento.arquivo2, lista[idxVar]);

                 // Sinalizar que uma thread comparadora pode ler a estrutura.
                 pthread_mutex_unlock(&mutex1);
 
                 // Esperar sinal da thread comparadora para incrementar os índices
                 pthread_mutex_lock(&mutex2);
                 if (idxVar < arquivosExistentes)
                         idxVar++;
 
                 if (idxVar == arquivosExistentes) {
                         idxFixo++;
                         idxVar = idxFixo+1;
                 }
 
                 pthread_mutex_lock(&mutex4);
           /* O problema aqui ocorre se a thread principal for alocada para executar antes que a última thread
                * comparadora coloque o resultado do diff no array de unificados. Nesse caso, quando -v é
                * especificado, o resumo da unificacao não é impresso.
                * Por isso o mutex no início dessa regiao: ele é iniciado trancado pela thread principal antes das
                * threads comparadoras serem criadas.
                */
           if (idxVar == arquivosExistentes && idxFixo == arquivosExistentes-1)
                   break;
 
        }// FIM DO WHILE
 
         /**
           * Se for modo verbose, ao terminar, imprime o conteúdo da lista criada pelas threads comparadoras.
           */
         if (isVerbose)
                 imprimeUnificados();
 
         free(listaUnificados);
         listaUnificados = NULL;
 
         pthread_mutex_destroy(&mutex1);
         pthread_mutex_destroy(&mutex2);
 
         pthread_mutex_destroy(&mutex3);
         pthread_mutex_destroy(&mutex4);
         pthread_exit(NULL);
 }


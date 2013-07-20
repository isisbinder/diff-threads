#include "comum.h"
#include "tipoArquivos.h"
#include <string.h>
#define EOPEN 1
#define CHUNK 512
#define ER_FOPEN "Erro ao abrir arquivo.\n"

char errors = 0;


/** @brief  Verifica se os arquivos são do mesmo tipo.
  *  @param Estrutura contendo as informações do arquivo #1.
  *  @param Estrutura contendo as informações do arquivo #2.
  *  @return <i>True</i> se os arquivos forem do mesmo tipo, <i>False</i> caso contrário.
  *  @note São testados somente arquivos regulares (os hardlinks se encaixam nesse grupo).
  */
bool tiposIguais(struct stat* info1, struct stat * info2) {
        return (S_ISREG(info1->st_mode) == S_ISREG(info2->st_mode));
}


/** @brief Verifica se os dois arquivos possuem tamanhos iguais.
  *  @param Estrutura contendo as informações do arquivo #1.
  *  @param Estrutura contendo as informações do arquivo #2.
  *  @return <i>True</i> se os arquivos possuírem o mesmo tamanho. <i>False</i> caso contrário  .
  */
bool tamanhosIguais(struct stat* info1, struct stat* info2) {
        return (info1->st_size == info2->st_size);
}


/** @brief Realiza a comparação entre dois arquivos através da leitura de seu conteúdo.
  *   @param Caminho do arquivo #1.
  *   @param Caminho do arquivo #2.
  *   @return <i>True</i> se os arquivos são iguais. <i>False</i> caso contrário.
  */
bool thread_diff(const char *fp1, const char *fp2) {
         FILE *arquivo_1 = fopen(fp1, "rb");
         FILE *arquivo_2 = fopen(fp2, "rb");
         if (arquivo_1 == NULL || arquivo_2 == NULL) {
                 perror(ER_FOPEN);
                 exit (1);
         }
         size_t lidos;
         char *bufferArq1 = (char*)malloc(sizeof(char)*CHUNK);
         char *bufferArq2 = (char*)malloc(sizeof(char)*CHUNK);
          if (bufferArq1 != NULL && bufferArq1 != NULL) {
                 int idx;
                 /* Lê uma quantidade de bytes especificados em CHUNK nos dois arquivos
                  * e compara um a um. Ao encontrar o primeiro byte diferente, fecha ambos os arquivos e retorna false.
                  * Senão, continua lendo e comparando até chegar ao final do arquivo ou encontrar um byte diferente.
                  */
 while (!feof(arquivo_1) && !feof(arquivo_2)) {
                         lidos = fread(bufferArq1,sizeof(char),CHUNK,arquivo_1);
                         fread(bufferArq2, sizeof(char), CHUNK, arquivo_2);
                          if (lidos > 0) {
                                 idx = 0;
                                  while (idx < lidos && bufferArq1[idx] == bufferArq2[idx])
                                         idx++;
                                  if (idx < lidos) {
                                          fclose(arquivo_1);
                                          fclose(arquivo_2);
                                          return false;
                                  }
                           }
                  }
                  fclose(arquivo_1);
                  fclose(arquivo_2);
                  return true;
         }
 }
 

/** @brief Realiza a comparação entre os arquivos especificados como argumentos.
   *  @param Caminho do arquivo #1.
   *  @param Caminho do arquivo #2.
   *  @return <i>True</i> Se os arquivos forem iguais, <i>False</i> caso contrário.
   *  @detail Existe uma seqüência de operações a serem realizadas. Primeiro se compara os tipos dos arquivos, para evitar a comparação de um link simbólico com um arquivo regular, por exemplo.<br/>
   *  Se os dois arquivos forem do mesmo tipo, verifica-se seu temanho. Apenas se os tamanhos forem iguais a verificação através da leitura do conteúdo é feita.
   */
bool diffPreparacao(const char *fp1, const char*fp2) {
         struct stat infoArq1, infoArq2;
         // PROBLEMA DO FSTAT COM UM HARDLINK JÁ EXISTENTE. RETORNA -1 E ENOENT (errno)
         if (stat(fp1,&infoArq1) == 0 && stat(fp2,&infoArq2) == 0) {
                 if (tiposIguais(&infoArq1,&infoArq2) ){
                         if (infoArq1.st_size != infoArq2.st_size)
                                 return false;
                                                 //TODO condicao para arquivos de tamanho maior que zero
                         return thread_diff(fp1, fp2);
                 } else return false;
         } else {
                 fprintf(stderr,"%m (%d) -- %d\n",errno,ENOENT);
                 exit (1);
         }
}


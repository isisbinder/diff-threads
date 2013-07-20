#include "constroiLista.h"

int arquivosExistentes = 0;
char ** lista;

/** @brief Monta a lista dos arquivos do diretório a ser unificado.
  *   @param Caminho do arquivo lido por <i>ntfw</i>.
  *   @param Estrutura com os dados do arquivo obtidos via <i>ntfw</i>, provavelmente por <i>stat</i>.
  *   @param <i>Não é usado</i>.
  *   @param <i>Não é usado</i>.
  *   @note Chamado pela função <i>ntfw</i>.
  */
int adicionaNome(const char *fpath, const struct stat *sb, int tflag) {
         if (S_ISREG(sb->st_mode)) {
                  lista = (char**) realloc(lista,sizeof(char*)*(arquivosExistentes+1));
                 lista[arquivosExistentes] = (char*)malloc((strlen(fpath)+1)*sizeof(char));
                 strcpy(lista[arquivosExistentes],fpath);
                 arquivosExistentes++;
         }
         return 0;
}


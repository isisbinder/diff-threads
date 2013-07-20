#include "checarArgumentos.h"
extern bool isVerbose;
extern int threadQtd;

/** @brief Verifica se o argumento passado para <i>-np</i> contém apenas dígitos.
  *  @param Argumento da opção <i>-np</i>.
  *  @return <b>True</b> se o argumento contém apenas dígitos e <b>False</b> caso contrário.
  */
bool digitos(char *argumento) {
         int tamanho = strlen(argumento);
         for(int indice = 0; indice < tamanho; indice++)
                 if (!isdigit(argumento[indice]))
                         return false;
         return true;
 }


/** @brief Verifica se os argumentos estão corretos.
  *  @param <i>Numero</i>: número de argumentos da linha de comando.
  *  @param <i>Args</i> : array de strings contendo os argumentos.
  *   @note Os argumentos devem ser digitados em uma ordem fixada (a linha de comando não é fl  exivel).
  */
void checarArgumentos(int numero, char *args[]) {
         if ( numero < 2 || numero > 5) {
                 fprintf(stderr,OP_NUM);
                 exit (1);
         }
 
         // Caso 1: O usuário fornece apenas 2 argumentos, sendo um deles uma das opções
         if (numero == 2 && (!strcmp("-v",args[1]) || !strcmp("-np",args[1]))) {
                 fprintf(stderr,OP_NUM);
                 exit(1);
         }

         /* Caso 2: com 3 argumentos, temos:
          *   unifica -v diretorio (válido, mesmo que diretório seja um número ou comece com um da      sh)
          *   unifica -v -np (inválido)
          *   unifica -np 3 (inválido)
          *   unifica -np diretorio (inválido)
          */
         if (numero == 3) {
                 if (!strcmp(args[1],"-v")) {
                         if (!strcmp(args[2],"-np")) {
                                 fprintf(stderr,OP_NUM);
                                 exit(1);
                         }
                         isVerbose = true;
                 } else if (!strcmp(args[1],"-np")) {
                         fprintf(stderr,OP_NUM);
                         exit(1);
                 }
         }
         /* Caso 3: com 4 argumentos, temos as seguintes possibilidades:
          *  unifica -np 3 diretorio (valido)
          *  As demais são inválidas (unifica -v -np 3 , unifica -v 2 -np, unifica -v -np diretori      o)
          */
         if (numero == 4) {
                 if (!strcmp(args[1],"-np") && digitos(args[2]) && (strcmp(args[3],"-v") || strcmp(arg  s[3],"-np")))
                          threadQtd  = atoi(args[2]);
                  else {
                         fprintf(stderr,OP_NUM);
                         exit(1);
                  }
         }

         if (numero == 5) {
                 if (!strcmp(args[1],"-v") && !strcmp(args[2],"-np") && digitos(args[3])) {
                   isVerbose = true;
                   threadQtd = atoi(args[3]);
                 } else {
                   fprintf(stderr,OP_NUM);
                   exit(1);
                 }
         }
}


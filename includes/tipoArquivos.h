#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

bool tiposIguais(struct stat* , struct stat * );
bool tamanhosIguais(struct stat* , struct stat* );
bool thread_diff(const char *, const char *);
bool diffPreparacao(const char *, const char*);


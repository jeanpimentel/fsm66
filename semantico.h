#ifndef SEMANTICO_H_INCLUDED
#define SEMANTICO_H_INCLUDED

#include "tabela_simbolos.h"

#define IMPRIMIR_VERIFICACOES_DEBUG 0
#define IMPRIMIR_VERIFICACOES_DEBUG2 0

void semantico_inicializar(Program*);
void semantico_finalizar();
escopo* obter_escopo();
#endif // SEMANTICO_H_INCLUDED

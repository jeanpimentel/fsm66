#ifndef LEXICO_H_INCLUDED
#define LEXICO_H_INCLUDED

#define TAMANHO_BUFFER 2
#define TAMANHO_INICIAL_LEXEMA 5
#define TAMANHO_PASSO_CRESCIMENTO_LEXEMA 10
#define FIM_BUFFER 0

void lexico_inicializar(FILE*);
void lexico_finalizar();

int lexico_proximo_token();
char* lexico_obter_lexema();
const char* lexico_nome_token(int);
int lexico_obter_numero_linha();

#endif // LEXICO_H_INCLUDED

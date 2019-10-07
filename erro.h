#ifndef ERRO_H_INCLUDED
#define ERRO_H_INCLUDED

#define ERRO_INESPERADO 0


#define ERRO_TIPO_LEXICO 10
#define ERRO_TIPO_SINTATICO 20
#define ERRO_TIPO_SEMANTICO 30


#define ERRO_CARACTER_INVALIDO 100
#define ERRO_NUMERO_INVALIDO 101
#define ERRO_FIM_ARQUIVO_LITERAL 102
#define ERRO_FIM_ARQUIVO_COMENTARIO 103

#define ERRO_TOKEN_INESPERADO 200
#define ERRO_TOKEN_ESPERADO 201
#define ERRO_FIM_ARQUIVO_INESPERADO 202

#define ERRO_FUNCAO_SEM_RETORNO 301
#define ERRO_ATRIBUICAO_INVALIDA 302
#define ERRO_ATRIBUICAO_INVALIDA_VETOR_EM_ELEMENTO_VETOR 303
#define ERRO_ATRIBUICAO_INVALIDA_ELEMENTO_VETOR_EM_VETOR 304
#define ERRO_ATRIBUICAO_INVALIDA_ELEMENTO_EM_VETOR 305
#define ERRO_ 30
#define ERRO_ 30
#define ERRO_ 30
#define ERRO_ 30

void erro(int, int, int);
void erro(int, int, int, char);
void erro(int, int, int, char*);
void erro_sintatico_token_esperado(int, char*, char*);

void erro_imprimir_erros();
void erro_incrementar();
int erro_quantidade_erros();

#endif // ERRO_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#include "lexico.h"
#include "erro.h"

int quantidade_erros = 0;

const char* erro_mensagem(int);
const char* tipo_erro(int);

void erro(int tipo, int erro, int linha) {
    quantidade_erros++;
    fprintf(stderr, "ERRO%s: %s na linha %d.\n", tipo_erro(tipo), erro_mensagem(erro), linha);
}

void erro(int tipo, int erro, int linha, char extra) {
    quantidade_erros++;
    fprintf(stderr, "ERRO%s: %s [%c] na linha %d.\n", tipo_erro(tipo), erro_mensagem(erro), extra, linha);
}

void erro(int tipo, int erro, int linha, char* extra) {
    quantidade_erros++;
    fprintf(stderr, "ERRO%s: %s [%s] na linha %d.\n", tipo_erro(tipo), erro_mensagem(erro), extra, linha);
}

void erro_sintatico_token_esperado(int linha, char* esperado, char* encontrado) {
    quantidade_erros++;
    fprintf(stderr, "ERRO SINTATICO: %s [%s] na linha %d. %s [%s]\n", "Token encontrado", encontrado, linha, "Token esperado", esperado);
}

const char* tipo_erro(int tipo) {
    switch(tipo) {
        case ERRO_TIPO_LEXICO: return " LEXICO"; break;
        case ERRO_TIPO_SINTATICO: return " SINTATICO"; break;
        default: return ""; break;
    }
}

const char* erro_mensagem(int erro) {
    switch(erro) {
        case ERRO_CARACTER_INVALIDO: return "Caracter inválido"; break;
        case ERRO_NUMERO_INVALIDO: return "Número inválido"; break;
        case ERRO_FIM_ARQUIVO_LITERAL: return "Fim de arquivo inesperado durante literal"; break;
        case ERRO_FIM_ARQUIVO_COMENTARIO: return "Fim de arquivo inesperado durante comentário"; break;
        case ERRO_TOKEN_INESPERADO: return "Token inesperado"; break;
        case ERRO_TOKEN_ESPERADO: return "Token esperado"; break;
        case ERRO_FIM_ARQUIVO_INESPERADO: return "Fim de arquivo inesperado"; break;
        default: return ""; break;
    }
}

void erro_incrementar() {
    quantidade_erros++;
}

int erro_quantidade_erros() {
    return quantidade_erros;
}

void erro_imprimir_erros() {}

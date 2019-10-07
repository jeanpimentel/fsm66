#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexico.h"
#include "simbolos.h"
#include "tabela_simbolos.h"
#include "erro.h"

FILE* arquivo_fonte;

char buffer[TAMANHO_BUFFER+1];
int proxima_posicao_buffer;
char proximo_caracter_buffer;

char c;

char* lexema;
int tamanho_lexema;
int proxima_posicao_lexema;

int contador_linhas;

tabela_simbolos* tabela_identificadores;
tabela_simbolos* tabela_reservadas;
tabela_simbolos* tabela_literais;

void lexico_carregar_buffer();
char lexico_proximo_caracter();

void lexico_inicializar(FILE* arquivo) {
    arquivo_fonte = arquivo;
    lexico_carregar_buffer();
    lexema = (char*) malloc(TAMANHO_INICIAL_LEXEMA * sizeof(char));
    int i;
    for(i = 0; i < TAMANHO_INICIAL_LEXEMA; i++) lexema[i] = 0;
    tamanho_lexema = TAMANHO_INICIAL_LEXEMA;
    proxima_posicao_lexema = 0;
    contador_linhas = 1;
    c = lexico_proximo_caracter();
    tabela_identificadores = new tabela_simbolos();
    tabela_literais = new tabela_simbolos();
    tabela_reservadas = new tabela_simbolos();
    tabela_reservadas->inserir("inicio", new simbolo_palavra_reservada(INICIO));
    tabela_reservadas->inserir("fim", new simbolo_palavra_reservada(FIM));
    tabela_reservadas->inserir("funcao", new simbolo_palavra_reservada(FUNCAO));
    tabela_reservadas->inserir("procedimento", new simbolo_palavra_reservada(PROCEDIMENTO));
    tabela_reservadas->inserir("vetor", new simbolo_palavra_reservada(VETOR));
    tabela_reservadas->inserir("inteiro", new simbolo_palavra_reservada(INTEIRO));
    tabela_reservadas->inserir("real", new simbolo_palavra_reservada(REAL));
    tabela_reservadas->inserir("char", new simbolo_palavra_reservada(CHAR));
    tabela_reservadas->inserir("logico", new simbolo_palavra_reservada(LOGICO));
    tabela_reservadas->inserir("tipo", new simbolo_palavra_reservada(TIPO));
    tabela_reservadas->inserir("registro", new simbolo_palavra_reservada(REGISTRO));
    tabela_reservadas->inserir("fimregistro", new simbolo_palavra_reservada(FIMREGISTRO));
    tabela_reservadas->inserir("se", new simbolo_palavra_reservada(SE));
    tabela_reservadas->inserir("entao", new simbolo_palavra_reservada(ENTAO));
    tabela_reservadas->inserir("senao", new simbolo_palavra_reservada(SENAO));
    tabela_reservadas->inserir("fimse", new simbolo_palavra_reservada(FIMSE));
    tabela_reservadas->inserir("enquanto", new simbolo_palavra_reservada(ENQUANTO));
    tabela_reservadas->inserir("faca", new simbolo_palavra_reservada(FACA));
    tabela_reservadas->inserir("fimfaca", new simbolo_palavra_reservada(FIMFACA));
    tabela_reservadas->inserir("leia", new simbolo_palavra_reservada(LEIA));
    tabela_reservadas->inserir("imprima", new simbolo_palavra_reservada(IMPRIMA));
    tabela_reservadas->inserir("nao", new simbolo_palavra_reservada(NAO));
    tabela_reservadas->inserir("verdadeiro", new simbolo_palavra_reservada(VERDADEIRO));
    tabela_reservadas->inserir("falso", new simbolo_palavra_reservada(FALSO));
    tabela_reservadas->inserir("ou", new simbolo_palavra_reservada(OU));
    tabela_reservadas->inserir("e", new simbolo_palavra_reservada(E));
    tabela_reservadas->inserir("div", new simbolo_palavra_reservada(DIV));
    tabela_reservadas->inserir("resto", new simbolo_palavra_reservada(RESTO));
}

void lexico_finalizar() {
//    tabela_identificadores->to_string();
//    tabela_literais->to_string();
//    tabela_reservadas->to_string();
//    tabela_literais->to_string();
    free(lexema);
    if(tabela_identificadores != NULL) delete tabela_identificadores;
    if(tabela_literais != NULL) delete tabela_literais;
    if(tabela_reservadas != NULL) delete tabela_reservadas;
}

void lexico_carregar_buffer() {
    int lidos = fread(buffer, sizeof(char), TAMANHO_BUFFER * sizeof(char), arquivo_fonte);
    if(feof(arquivo_fonte)) buffer[lidos] = EOF;
    else buffer[lidos] = FIM_BUFFER;
    proxima_posicao_buffer = 0;
}

char lexico_proximo_caracter() {
    char proximo_caracter_buffer = buffer[proxima_posicao_buffer];
    proxima_posicao_buffer++;
    if(buffer[proxima_posicao_buffer] == FIM_BUFFER) lexico_carregar_buffer();
    return proximo_caracter_buffer;
}

void lexico_adicionar_ao_lexema(char c) {
//    printf("Tamanho atual: %d\n", tamanho_lexema);
//    printf("Proxima posicao: %d\n", proxima_posicao_lexema);
    if(proxima_posicao_lexema + 1 >= tamanho_lexema) {
        int tamanho_original = tamanho_lexema;
        tamanho_lexema = tamanho_lexema + TAMANHO_PASSO_CRESCIMENTO_LEXEMA;
        lexema = (char*) realloc(lexema, tamanho_lexema * sizeof(char));
        int i;
        for(i = tamanho_original; i < tamanho_lexema; i++) lexema[i] = 0;
    }
    lexema[proxima_posicao_lexema] = c;
    lexema[++proxima_posicao_lexema] = 0;
}

void lexico_limpar_lexema() {
    tamanho_lexema = TAMANHO_INICIAL_LEXEMA;
    lexema = (char*) realloc(lexema, tamanho_lexema * sizeof(char));
    int i;
    for(i = 0; i < tamanho_lexema; i++) lexema[i] = 0;
    proxima_posicao_lexema = 0;
}

char* lexico_obter_lexema() {
    return lexema;
}

int lexico_obter_numero_linha() {
    return contador_linhas;
}

int lexico_proximo_token() {
    lexico_limpar_lexema();
    int estado_atual = 0;
    while(1) {
        switch(estado_atual) {
            case 0:
                if(isspace(c)) {
                    if(c == '\n') contador_linhas++;
                    c = lexico_proximo_caracter();
                    estado_atual = 0;
                    break;
                }
                if(isalpha(c)) {
                    lexico_adicionar_ao_lexema(c);
                    estado_atual = 9;
                    break;
                }
                if(isdigit(c)) {
                    lexico_adicionar_ao_lexema(c);
                    estado_atual = 1;
                    break;
                }
                switch(c) {
                    case ';': estado_atual = 24; break;
                    case '-': estado_atual = 49; break;
                    case '+': estado_atual = 48; break;
                    case ',': estado_atual = 13; break;
                    case '=': estado_atual = 14; break;
                    case ':': estado_atual = 12; break;
                    case '<': estado_atual = 15; break;
                    case '>': estado_atual = 17; break;
                    case '{': estado_atual = 11; break;
                    case '"': estado_atual = 37; break;
                    case '[': estado_atual = 45; break;
                    case ']': estado_atual = 41; break;
                    case '(': estado_atual = 44; break;
                    case ')': estado_atual = 42; break;
                    case '.': estado_atual = 21; break;
                    case '*': estado_atual = 31; break;
                    case '/': estado_atual = 34; break;
                    case EOF: estado_atual = 43; break;
                    default: estado_atual = 38; break;
                }
                break;
            case 1:
                c = lexico_proximo_caracter();
                if(isdigit(c)) {
                    lexico_adicionar_ao_lexema(c);
                    estado_atual = 1;
                }
                else {
                    switch(c) {
                        case '.': lexico_adicionar_ao_lexema(c); estado_atual = 3; break;
                        case 'E': lexico_adicionar_ao_lexema(c); estado_atual = 5; break;
                        case 'e': lexico_adicionar_ao_lexema('E'); estado_atual = 5; break;
                        default: estado_atual = 2; break;
                    }
                }
                break;
            case 2: return NUMERO_INTEIRO;
            case 3:
                c = lexico_proximo_caracter();
                if(isdigit(c)) {
                    lexico_adicionar_ao_lexema(c);
                    estado_atual = 3;
                }
                else {
                    switch(c) {
                        case 'E': lexico_adicionar_ao_lexema(c); estado_atual = 5; break;
                        case 'e': lexico_adicionar_ao_lexema('E'); estado_atual = 5; break;
                        default: estado_atual = 8; break;
                    }
                }
                break;
            case 5:
                c = lexico_proximo_caracter();
                if(isdigit(c)) {
                    lexico_adicionar_ao_lexema(c);
                    estado_atual = 7;
                }
                else {
                    switch(c) {
                        case '+': lexico_adicionar_ao_lexema(c); estado_atual = 6; break;
                        case '-': lexico_adicionar_ao_lexema(c); estado_atual = 6; break;
                        default: estado_atual = 26; break;
                    }
                }
                break;
            case 6:
                c = lexico_proximo_caracter();
                if(isdigit(c)) {
                    lexico_adicionar_ao_lexema(c);
                    estado_atual = 7;
                }
                else estado_atual = 26;
                break;
            case 7:
                c = lexico_proximo_caracter();
                if(isdigit(c)) {
                    lexico_adicionar_ao_lexema(c);
                    estado_atual = 7;
                }
                else estado_atual = 8;
                break;
            case 8: return NUMERO_REAL;
            case 9:
                c = lexico_proximo_caracter();
                if(isalnum(c) || c == '_') {
                    lexico_adicionar_ao_lexema(c);
                    estado_atual = 9;
                }
                else estado_atual = 10;
                break;
            case 10:
            {
                char* lex = (char*) calloc(strlen(lexico_obter_lexema())+1, sizeof(char));
                strcpy(lex, lexico_obter_lexema());
                /* Transformando em minusculo pois a linguagem nao eh case sensitive. */
                unsigned int i;
                for(i = 0; i <= strlen(lex); i++) lex[i] = tolower(lex[i]);
                simbolo_palavra_reservada* info;
                info = (simbolo_palavra_reservada*) tabela_reservadas->buscar(lex);
                if(info == NULL) {
                    if(tabela_identificadores->buscar(lex) == NULL) tabela_identificadores->inserir(lex, new simbolo());
                    return IDENTIFICADOR;
                }
                else return info->token;
                break;
            }
            case 11:
                if(c == '\n') contador_linhas++;
                c = lexico_proximo_caracter();
                switch(c) {
                    case '}': c = lexico_proximo_caracter(); estado_atual = 0; break;
                    case EOF: estado_atual = 46; break;
                    default: estado_atual = 11; break;
                }
                break;
            case 12: c = lexico_proximo_caracter(); return DOIS_PONTOS;
            case 13: c = lexico_proximo_caracter(); return VIRGULA;
            case 14: c = lexico_proximo_caracter(); return IGUAL;
            case 15:
                c = lexico_proximo_caracter();
                switch(c) {
                    case '>': estado_atual = 16; break;
                    case '-': estado_atual = 18; break;
                    case '=': estado_atual = 19; break;
                    default: estado_atual = 23; break;
                }
                break;
            case 16: c = lexico_proximo_caracter(); return DIFERENTE;
            case 17:
                c = lexico_proximo_caracter();
                switch(c) {
                    case '=': estado_atual = 20; break;
                    default: estado_atual = 22; break;
                }
                break;
            case 18: c = lexico_proximo_caracter(); return ATRIBUICAO;
            case 19: c = lexico_proximo_caracter(); return MENOR_IGUAL;
            case 20: c = lexico_proximo_caracter(); return MAIOR_IGUAL;
            case 21:
                c = lexico_proximo_caracter();
                if(isdigit(c)) {
                    lexico_adicionar_ao_lexema(c);
                    estado_atual = 3;
                }
                else estado_atual = 25;
                break;
            case 22: return MAIOR;
            case 23: return MENOR;
            case 24: c = lexico_proximo_caracter(); return PONTO_VIRGULA;
            case 25: return PONTO;
            case 26: erro(ERRO_TIPO_LEXICO, ERRO_NUMERO_INVALIDO, lexico_obter_numero_linha()); estado_atual = 0; break;
            case 31:
                c = lexico_proximo_caracter();
                switch(c) {
                    case '*': estado_atual = 32; break;
                    default: estado_atual = 33; break;
                }
                break;
            case 32: c = lexico_proximo_caracter(); return EXPONENCIACAO;
            case 33: return MULTIPLICACAO;
            case 34: c = lexico_proximo_caracter(); return DIVISAO;
            case 37:
                c = lexico_proximo_caracter();
                if(c == '\n') contador_linhas++;
                switch(c) {
                    case '"': estado_atual = 39; break;
                    case EOF: estado_atual = 47; break;
                    default: lexico_adicionar_ao_lexema(c); estado_atual = 37; break;
                }
                break;
            case 38: erro(ERRO_TIPO_LEXICO, ERRO_CARACTER_INVALIDO, lexico_obter_numero_linha(), c); lexico_limpar_lexema(); c = lexico_proximo_caracter(); estado_atual = 0; break;
            case 39:
                c = lexico_proximo_caracter();
                if(tabela_literais->buscar(lexico_obter_lexema()) == NULL) tabela_literais->inserir(lexico_obter_lexema(), new simbolo());
                return LITERAL;
            case 41: c = lexico_proximo_caracter(); return FECHA_COLCHETES;
            case 42: c = lexico_proximo_caracter(); return FECHA_PARENTESES;
            case 43: return EOF;
            case 44: c = lexico_proximo_caracter(); return ABRE_PARENTESES;
            case 45: c = lexico_proximo_caracter(); return ABRE_COLCHETES;
            case 46: erro(ERRO_TIPO_LEXICO, ERRO_FIM_ARQUIVO_COMENTARIO, lexico_obter_numero_linha()); lexico_limpar_lexema(); return EOF;
            case 47: erro(ERRO_TIPO_LEXICO, ERRO_FIM_ARQUIVO_LITERAL, lexico_obter_numero_linha()); lexico_limpar_lexema(); return EOF;
            case 48: c = lexico_proximo_caracter(); return MAIS;
            case 49: c = lexico_proximo_caracter(); return MENOS;
            default: erro(ERRO_TIPO_LEXICO, ERRO_INESPERADO, lexico_obter_numero_linha()); lexico_limpar_lexema(); return EOF;
        }
    }
}

const char* lexico_nome_token(int token) {
    switch (token) {
        case PONTO_VIRGULA: return "PONTO_VIRGULA"; break;
        case MENOS: return "MENOS"; break;
        case MAIS: return "MAIS"; break;
        case VIRGULA: return "VIRGULA"; break;
        case IGUAL: return "IGUAL"; break;
        case DOIS_PONTOS: return "DOIS_PONTOS"; break;
        case DIFERENTE: return "DIFERENTE"; break;
        case ATRIBUICAO: return "ATRIBUICAO"; break;
        case MENOR_IGUAL: return "MENOR_IGUAL"; break;
        case MENOR: return "MENOR"; break;
        case MAIOR_IGUAL: return "MAIOR_IGUAL"; break;
        case MAIOR: return "MAIOR"; break;
        case LITERAL: return "LITERAL"; break;
        case ABRE_COLCHETES: return "ABRE_COLCHETES"; break;
        case FECHA_COLCHETES: return "FECHA_COLCHETES"; break;
        case ABRE_PARENTESES: return "ABRE_PARENTESES"; break;
        case FECHA_PARENTESES: return "FECHA_PARENTESES"; break;
        case FIM_ARQUIVO: return "FIM_ARQUIVO"; break;
        case NUMERO_INTEIRO: return "NUMERO_INTEIRO"; break;
        case NUMERO_REAL: return "NUMERO_REAL"; break;
        case PONTO: return "PONTO"; break;
        case DIVISAO: return "DIVISAO"; break;
        case MULTIPLICACAO: return "MULTIPLICACAO"; break;
        case EXPONENCIACAO: return "EXPONENCIACAO"; break;
        case IDENTIFICADOR: return "IDENTIFICADOR"; break;
        case INICIO: return "INICIO"; break;
        case FIM: return "FIM"; break;
        case VETOR: return "VETOR"; break;
        case INTEIRO: return "INTEIRO"; break;
        case REAL: return "REAL"; break;
        case CHAR: return "CHAR"; break;
        case LOGICO: return "LOGICO"; break;
        case TIPO: return "TIPO"; break;
        case REGISTRO: return "REGISTRO"; break;
        case FIMREGISTRO: return "FIMREGISTRO"; break;
        case FUNCAO: return "FUNCAO"; break;
        case PROCEDIMENTO: return "PROCEDIMENTO"; break;
        case SE: return "SE"; break;
        case ENTAO: return "ENTAO"; break;
        case SENAO: return "SENAO"; break;
        case FIMSE: return "FIMSE"; break;
        case ENQUANTO: return "ENQUANTO"; break;
        case FACA: return "FACA"; break;
        case FIMFACA: return "FIMFACA"; break;
        case LEIA: return "LEIA"; break;
        case IMPRIMA: return "IMPRIMA"; break;
        case NAO: return "NAO"; break;
        case VERDADEIRO: return "VERDADEIRO"; break;
        case FALSO: return "FALSO"; break;
        case E: return "E"; break;
        case OU: return "OU"; break;
        case DIV: return "DIV"; break;
        case RESTO: return "RESTO"; break;
        case EOF: return "EOF"; break;

//        case EQ: return "EQ"; break;
//        case NEQ: return "NEQ"; break;
//        case LT: return "LT"; break;
//        case GT: return "GT"; break;
//        case LE: return "LE"; break;
//        case GE: return "GE"; break;

        default: fprintf(stderr, "TOKEN INVÁLIDO"); break;
    }
    return "";
}

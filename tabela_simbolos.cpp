#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexico.h"
#include "tabela_simbolos.h"
#include "simbolos.h"

escopo::escopo(escopo* _pai, const char* _nome) {
    pai = _pai;
    nome = _nome;
    tab_variaveis = new tabela_simbolos();
    tab_registros = new tabela_simbolos();
    tab_funcoes = new tabela_simbolos();

}

escopo::~escopo() {
    if (pai != NULL) delete pai;
    if (tab_variaveis != NULL) delete tab_variaveis;
    if (tab_registros != NULL) delete tab_registros;
    if (tab_funcoes != NULL) delete tab_funcoes;
}

simbolo_variavel* escopo::buscar_variavel(char* var,int* nivel) {
    if (this->tab_variaveis != NULL) {
        simbolo_variavel* svar = (simbolo_variavel*) this->tab_variaveis->buscar(var);
        if (svar != NULL) {
            return svar;
        }
    }
    if (this->pai != NULL) {
        (*nivel)++;
        return this->pai->buscar_variavel(var,nivel);
    }
    return NULL;

}
simbolo_funcao* escopo::buscar_funcao(char* var,int* nivel) {
    if (this->tab_funcoes != NULL) {
        simbolo_funcao* svar = (simbolo_funcao*) this->tab_funcoes->buscar(var);
        if (svar != NULL) {
            return svar;
        }
    }
    if (this->pai != NULL) {
        (*nivel)++;
        return this->pai->buscar_funcao(var,nivel);
    }
    return NULL;

}

void escopo::imprimir() {
    printf("\n\n\n\n");
    tab_variaveis->imprimir("VARIAVEIS");
    tab_registros->imprimir("REGISTROS");
    tab_funcoes->imprimir("FUNCOES");
}

simbolo::simbolo() {
}

simbolo::~simbolo() {
}

simbolo_palavra_reservada::simbolo_palavra_reservada(int _token) {
    token = _token;
}

simbolo_palavra_reservada::~simbolo_palavra_reservada() {
}

simbolo_variavel::simbolo_variavel(int _tipo, const char* _lexema_tipo, int _lower, int _upper, int _nlocal) {
    tipo = _tipo;
    lexema_tipo = _lexema_tipo;
    lowerbound = _lower;
    upperbound = _upper;
    nlocal = _nlocal;
    name = NULL;
}

simbolo_variavel::~simbolo_variavel() {
    nlocal = -1;
    name = NULL;
}

void simbolo_variavel::imprimir() {
    printf("TIPO: ---------- %s\n", lexico_nome_token(tipo));
    printf("LEXEMA_TIPO: --- %s\n", lexema_tipo);
    printf("LOWER: --------- %d\n", lowerbound);
    printf("UPPER: --------- %d\n", upperbound);
    printf("NLOCAL: -------- %d\n", nlocal);
}

simbolo_registro::simbolo_registro(escopo* _escopo) {
    escopo_registro = _escopo;
}

simbolo_registro::~simbolo_registro() {
}

void simbolo_registro::imprimir() {
    escopo_registro->imprimir();
}

simbolo_funcao::simbolo_funcao() {
    nome_funcao = NULL;
    escopo_funcao = NULL;
    tem_retorno = false;
    tipo_retorno = 0;
    quantidade_parametros = 0;
    parametros = NULL;
}

simbolo_funcao::~simbolo_funcao() {
}

void simbolo_funcao::imprimir() {
    printf("NOME: --------------- %s\n", nome_funcao);
    printf("ESCOPO: ------------- \n");
    printf("TEM RETORNO: -------- %d\n", tem_retorno);
    printf("TIPO RETORNO: ------- %s\n", lexico_nome_token(tipo_retorno));
    printf("QTDE PARAMETROS: ---- %d\n", quantidade_parametros);
    printf("PARAMETROS: --------- \n");
}

tabela_simbolos::tabela_simbolos() {
    proxima_posicao = 0;
    tamanho_arranjo = TAMANHO_INICIAL_ARRANJO;
    arranjo = (char*) malloc(TAMANHO_INICIAL_ARRANJO * sizeof (char));
    int i;
    for (i = 0; i < TAMANHO_INICIAL_ARRANJO; i++) arranjo[i] = NULL;
    for (i = 0; i < TAMANHO_TABELA; i++) tabela[i] = NULL;
}

tabela_simbolos::~tabela_simbolos() {
    int i;
    for (i = 0; i < TAMANHO_TABELA; i++) {
        struct entrada* no = tabela[i];
        struct entrada* proxima;
        while (no != NULL) {
            proxima = no->proximo;
            free(no);
            no = proxima;
        }
    }
    free(arranjo);
}

int tabela_simbolos::adicionar_lexema_arranjo(const char* cadeia) {
    int posicao = proxima_posicao;
    if ((proxima_posicao + strlen(cadeia) + 1) >= tamanho_arranjo) {
        unsigned int novo_tamanho = tamanho_arranjo;
        do novo_tamanho += TAMANHO_PASSO_CRESCIMENTO_ARRANJO; while ((proxima_posicao + strlen(cadeia) + 1) >= novo_tamanho);
        arranjo = (char*) realloc(arranjo, novo_tamanho * sizeof (char));
        unsigned int i = 0;
        for (i = tamanho_arranjo; i < novo_tamanho; i++) arranjo[i] = 0;
        tamanho_arranjo = novo_tamanho;
    }
    strncpy(arranjo + proxima_posicao, cadeia, strlen(cadeia));
    proxima_posicao += strlen(cadeia);
    arranjo[++proxima_posicao] = 0;
    return posicao;
}

char* tabela_simbolos::obter_lexema_arranjo(int posicao) {
    char* palavra = (char*) calloc(strlen(arranjo + posicao) + 1, sizeof (char));
    strcpy(palavra, arranjo + posicao);
    return palavra;
}

int tabela_simbolos::funcao_hash(char* cadeia) {
    char* p;
    unsigned int h = 0, g;
    for (p = cadeia; *p != '\0'; p = p + 1) {
        h = (h << 4) + (*p);
        if ((g = h & 0xf0000000)) {
            h = h ^ (g >> 24);
            h = h ^ g;
        }
    }
    return h % TAMANHO_TABELA;
}

void tabela_simbolos::inserir(char* lexema, simbolo* _info) {
    int posicao = adicionar_lexema_arranjo(lexema);
    struct entrada* no = (entrada*) malloc(sizeof (struct entrada));
    no->posicao_arranjo = posicao;
    no->info = _info;
    no->proximo = NULL;
    int indice = funcao_hash(lexema);
    if (tabela[indice] == NULL) tabela[indice] = no;
    else {
        struct entrada* primeiro_no = tabela[indice];
        tabela[indice] = no;
        no->proximo = primeiro_no;
    }
}

void tabela_simbolos::inserir(const char* lexema, simbolo* _info) {
    tabela_simbolos::inserir((char*) lexema, _info);
}

simbolo* tabela_simbolos::buscar(char* lexema) {
    int indice = funcao_hash(lexema);
    if (tabela[indice] != NULL) {
        struct entrada* no = tabela[indice];
        do {
            if (strcmp(obter_lexema_arranjo(no->posicao_arranjo), lexema) == 0) return no->info;
            no = no->proximo;
        } while (no != NULL);
    }
    return NULL;
}

void tabela_simbolos::imprimir(const char* cabecalho) {
    printf("-------------------------------------------------------------------------------\n");
    printf(" TABELA DE SIMBOLOS: %s\n", cabecalho);
    printf("-------------------------------------------------------------------------------\n");
    int i;
    for (i = 0; i < TAMANHO_TABELA; i++) {
        if (tabela[i] != NULL) {
            printf("- INDICE: %d\t\n", i);
            struct entrada* no = tabela[i];
            do {
                printf("\t\t%d -> ", no->posicao_arranjo);
                printf("%s\n", obter_lexema_arranjo(no->posicao_arranjo));
                no = no->proximo;
            } while (no != NULL);
        }
    }
    printf("\n");
}

void tabela_simbolos::to_string() {
    imprimir("{MODO DEBUG}");
    printf(" ARRANJO: \n");
    printf("-------------------------------------------------------------------------------\n");
    printf(" <IND>\t<NUM>\t<CHAR>\n");
    unsigned int i;
    for (i = 0; i < tamanho_arranjo; i++) printf(" %d\t%d\t%c\n", i, arranjo[i], arranjo[i]);
}

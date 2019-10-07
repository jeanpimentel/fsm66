/**
* @file     sintatico.cpp
* @brief    Analisador Sintático.
* @date     26-09-2010
* @version  0.1
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "lexico.h"
#include "sintatico.h"
#include "arvore_sintaxe_abstrata.h"
#include "simbolos.h"
#include "erro.h"

int sintatico_proximo_token_em_iniciais(int*);
int sintatico_proximo_token_em_sincronizadores(int*);

/** @brief  Próximo token (lookahead). */
int proximo_token;

/** @brief  Conjuntos iniciais. */
int iniciais_program[] = {1, INICIO};
int iniciais_instructions[] = {14, INICIO, SE, ENQUANTO, LEIA, IMPRIMA, IDENTIFICADOR, FUNCAO, PROCEDIMENTO, TIPO, NUMERO_INTEIRO, NUMERO_REAL, CHAR, LOGICO, VETOR};
int iniciais_statement_start[] = {5, INICIO, SE, ENQUANTO, LEIA, IMPRIMA};
int iniciais_compound_statement[] = {1, INICIO};
int iniciais_conditional[] = {1, SE};
int iniciais_subprogram_declaration[] = {2, FUNCAO, PROCEDIMENTO};
int iniciais_subprogram[] = {2, FUNCAO, PROCEDIMENTO};
int iniciais_type_declaration[] = {1, TIPO};
int iniciais_variable_declaration[] = {5, INTEIRO, REAL, CHAR, LOGICO, VETOR};
int iniciais_type[] = {5, INTEIRO, REAL, CHAR, LOGICO, VETOR};
int iniciais_standard_type[] = {4, INTEIRO, REAL, CHAR, LOGICO};
int iniciais_resolve_id[] = {7, ABRE_COLCHETES, PONTO, ABRE_PARENTESES, DOIS_PONTOS, ATRIBUICAO, PONTO_VIRGULA, FIM};
int iniciais_statement_declaration[] = {2, ABRE_COLCHETES, PONTO};
int iniciais_procedure_call[] = {1, ABRE_PARENTESES};
int iniciais_id_list[] = {1, IDENTIFICADOR};
int iniciais_id_list_recursive[] = {1, VIRGULA};
int iniciais_type_2[] = {6, INTEIRO, REAL, CHAR, LOGICO, VETOR, IDENTIFICADOR};
int iniciais_type_3[] = {5, INTEIRO, REAL, CHAR, LOGICO, IDENTIFICADOR};
int iniciais_arguments[] = {1, ABRE_PARENTESES};
int iniciais_optional_statement[] = {6, IDENTIFICADOR, INICIO, SE, ENQUANTO, LEIA, IMPRIMA};
int iniciais_statement_list[] = {6, IDENTIFICADOR, INICIO, SE, ENQUANTO, LEIA, IMPRIMA};
int iniciais_statement[] = {6, IDENTIFICADOR, INICIO, SE, ENQUANTO, LEIA, IMPRIMA};
int iniciais_statement_list_optional[] = {1, PONTO_VIRGULA};
int iniciais_statement_list_recursive[] = {6, IDENTIFICADOR, INICIO, SE, ENQUANTO, LEIA, IMPRIMA};
int iniciais_statement_id[] = {4, ABRE_COLCHETES, PONTO, ABRE_PARENTESES, ATRIBUICAO};
int iniciais_declarations[] = {9, INTEIRO, REAL, CHAR, LOGICO, VETOR, FUNCAO, PROCEDIMENTO, TIPO, IDENTIFICADOR};
int iniciais_variable_declaration_recursive[] = {5, INTEIRO, REAL, CHAR, LOGICO, VETOR, IDENTIFICADOR};
int iniciais_optional_id[] = {1, PONTO};
int iniciais_optional_senao[] = {2, SENAO, FIMSE};
int iniciais_expression_list[] = {10, NAO, MAIS, MENOS, IDENTIFICADOR, NUMERO_INTEIRO, NUMERO_REAL, VERDADEIRO, FALSO, LITERAL, ABRE_PARENTESES};
int iniciais_expression_ou[] = {10, NAO, MAIS, MENOS, IDENTIFICADOR, NUMERO_INTEIRO, NUMERO_REAL, VERDADEIRO, FALSO, LITERAL, ABRE_PARENTESES};
int iniciais_expression_e[] = {10, NAO, MAIS, MENOS, IDENTIFICADOR, NUMERO_INTEIRO, NUMERO_REAL, VERDADEIRO, FALSO, LITERAL, ABRE_PARENTESES};
int iniciais_expression_comparative[] = {10, NAO, MAIS, MENOS, IDENTIFICADOR, NUMERO_INTEIRO, NUMERO_REAL, VERDADEIRO, FALSO, LITERAL, ABRE_PARENTESES};
int iniciais_expression_relative[] = {10, NAO, MAIS, MENOS, IDENTIFICADOR, NUMERO_INTEIRO, NUMERO_REAL, VERDADEIRO, FALSO, LITERAL, ABRE_PARENTESES};
int iniciais_expression_sum[] = {10, NAO, MAIS, MENOS, IDENTIFICADOR, NUMERO_INTEIRO, NUMERO_REAL, VERDADEIRO, FALSO, LITERAL, ABRE_PARENTESES};
int iniciais_expression_mult[] = {10, NAO, MAIS, MENOS, IDENTIFICADOR, NUMERO_INTEIRO, NUMERO_REAL, VERDADEIRO, FALSO, LITERAL, ABRE_PARENTESES};
int iniciais_expression_unit[] = {10, NAO, MAIS, MENOS, IDENTIFICADOR, NUMERO_INTEIRO, NUMERO_REAL, VERDADEIRO, FALSO, LITERAL, ABRE_PARENTESES};
int iniciais_factor[] = {7, IDENTIFICADOR, NUMERO_INTEIRO, NUMERO_REAL, VERDADEIRO, FALSO, LITERAL, ABRE_PARENTESES};
int iniciais_complement_id[] = {4, ABRE_COLCHETES, PONTO, ABRE_PARENTESES, FIM};
int iniciais_expression_list_recursive[] = {1, VIRGULA};
int iniciais_expression_ou_recursive[] = {1, OU};
int iniciais_expression_e_recursive[] = {1, E};
int iniciais_expression_comparative_recursive[] = {2, IGUAL, DIFERENTE};
int iniciais_expression_relative_recursive[] = {4, MAIOR, MAIOR_IGUAL, MENOR_IGUAL, MENOR};
int iniciais_expression_sum_recursive[] = {2, MAIS, MENOS};
int iniciais_expression_mult_recursive[] = {4, MULTIPLICACAO, DIVISAO, DIV, RESTO};
int iniciais_comparative_operator[] = {2, IGUAL, DIFERENTE};
int iniciais_relative_operator[] = {4, MAIOR, MAIOR_IGUAL, MENOR_IGUAL, MENOR};
int iniciais_sum_operator[] = {2, MAIS, MENOS};
int iniciais_mult_operator[] = {4, MULTIPLICACAO, DIVISAO, DIV, RESTO};
int iniciais_unit_operator[] = {3, NAO, MAIS, MENOS};

/** @brief  Conjuntos sincronizadores. */
int sincronizadores_program[] = {EOF};
int sincronizadores_instructions[] = {FIM, PONTO_VIRGULA, EOF};
int sincronizadores_statement_start[] = {PONTO_VIRGULA, EOF};
int sincronizadores_compound_statement[] = {INICIO, SE, ENQUANTO, LEIA, IMPRIMA, IDENTIFICADOR, TIPO, FUNCAO, PROCEDIMENTO, INTEIRO, REAL, CHAR, LOGICO, VETOR, SENAO, FIMSE, FIMFACA, PONTO_VIRGULA, EOF};
int sincronizadores_conditional[] = {SENAO, FIMSE, FIMFACA, PONTO_VIRGULA, EOF};
int sincronizadores_subprogram_declaration[] = {INICIO, SE, ENQUANTO, LEIA, IMPRIMA, IDENTIFICADOR, TIPO, FUNCAO, PROCEDIMENTO, INTEIRO, REAL, CHAR, LOGICO, VETOR, EOF};
int sincronizadores_subprogram[] = {INTEIRO, REAL, CHAR, LOGICO, VETOR, FUNCAO, PROCEDIMENTO, TIPO, IDENTIFICADOR, EOF};
int sincronizadores_type_declaration[] = {INICIO, SE, ENQUANTO, LEIA, IMPRIMA, IDENTIFICADOR, TIPO, FUNCAO, PROCEDIMENTO, INTEIRO, REAL, CHAR, LOGICO, VETOR, EOF};
int sincronizadores_variable_declaration[] = {INICIO, SE, ENQUANTO, LEIA, IMPRIMA, IDENTIFICADOR, TIPO, FUNCAO, PROCEDIMENTO, INTEIRO, REAL, CHAR, LOGICO, VETOR, EOF};
int sincronizadores_type[] = {DOIS_PONTOS, NUMERO_INTEIRO, FECHA_COLCHETES, IDENTIFICADOR, EOF};
int sincronizadores_standard_type[] = {DOIS_PONTOS, PONTO_VIRGULA, EOF};
int sincronizadores_resolve_id[] = {PONTO_VIRGULA, FIM, EOF};
int sincronizadores_statement_declaration[] = {MULTIPLICACAO, DIVISAO, DIV, RESTO, MAIS, MENOS, ATRIBUICAO, EOF};
int sincronizadores_procedure_call[] = {PONTO_VIRGULA, FIM, SENAO, FIMSE, FIMFACA, MULTIPLICACAO, DIVISAO, DIV, RESTO, MAIS, MENOS, EOF};
int sincronizadores_id_list[] = {PONTO_VIRGULA, FECHA_PARENTESES, EOF};
int sincronizadores_id_list_recursive[] = {PONTO_VIRGULA, FECHA_PARENTESES, EOF};
int sincronizadores_type_2[] = {DOIS_PONTOS, EOF};
int sincronizadores_type_3[] = {DOIS_PONTOS, PONTO_VIRGULA, IDENTIFICADOR, INTEIRO, REAL, CHAR, LOGICO, EOF};
int sincronizadores_arguments[] = {DOIS_PONTOS, PONTO_VIRGULA, EOF};
int sincronizadores_optional_statement[] = {FIM, EOF};
int sincronizadores_statement[] = {SENAO, FIMSE, FIMFACA, PONTO_VIRGULA, EOF};
int sincronizadores_statement_list[] = {FIM, PONTO_VIRGULA, EOF};
int sincronizadores_statement_list_optional[] = {FIM, PONTO_VIRGULA, EOF};
int sincronizadores_statement_list_recursive[] = {FIM, PONTO_VIRGULA, EOF};
int sincronizadores_statement_id[] = {SENAO, FIMSE, FIMFACA, PONTO_VIRGULA, EOF};
int sincronizadores_declarations[] = {INICIO, SE, ENQUANTO, LEIA, IMPRIMA, IDENTIFICADOR, TIPO, FUNCAO, PROCEDIMENTO, INTEIRO, REAL, CHAR, LOGICO, VETOR, SENAO, FIMSE, FIMFACA, PONTO_VIRGULA, EOF};
int sincronizadores_variable_declaration_recursive[] = {FIMREGISTRO, PONTO_VIRGULA, EOF};
int sincronizadores_optional_id[] = {NAO, IDENTIFICADOR, NUMERO_INTEIRO, NUMERO_REAL, VERDADEIRO, FALSO, LITERAL, ABRE_PARENTESES, MULTIPLICACAO, DIVISAO, DIV, RESTO, MAIS, MENOS, ATRIBUICAO, EOF};
int sincronizadores_optional_senao[] = {SENAO, FIMSE, FIMFACA, PONTO_VIRGULA, EOF};
int sincronizadores_expression_list[] = {FECHA_PARENTESES, EOF};
int sincronizadores_expression_ou[] = {PONTO_VIRGULA, FIM, FACA, ENTAO, FECHA_PARENTESES, FECHA_COLCHETES, VIRGULA, SENAO, FIMSE, FIMFACA, EOF};
int sincronizadores_expression_e[] = {PONTO_VIRGULA, FIM, FACA, ENTAO, FECHA_PARENTESES, FECHA_COLCHETES, VIRGULA, SENAO, FIMSE, FIMFACA,OU, EOF};
int sincronizadores_expression_comparative[] = {PONTO_VIRGULA, FIM, FACA, ENTAO, FECHA_PARENTESES, FECHA_COLCHETES, VIRGULA, SENAO, FIMSE, FIMFACA,OU,E, EOF};
int sincronizadores_expression_relative[] = {PONTO_VIRGULA, FIM, FACA, ENTAO, FECHA_PARENTESES, FECHA_COLCHETES, VIRGULA, SENAO, FIMSE, FIMFACA,IGUAL,E, DIFERENTE, OU, EOF};
int sincronizadores_expression_sum[] = {MENOR, MENOR_IGUAL, MAIOR_IGUAL, MAIOR, EOF};
int sincronizadores_expression_mult[] = {MAIS, MENOS, EOF};
int sincronizadores_expression_unit[] = {MULTIPLICACAO, DIVISAO, DIV, RESTO, MAIS, MENOS, SE, ENQUANTO, FIMFACA, FIMSE, FIM, PONTO_VIRGULA, IDENTIFICADOR, NUMERO_INTEIRO, NUMERO_REAL, EOF};
int sincronizadores_factor[] = {MULTIPLICACAO, DIVISAO, DIV, RESTO, MAIS, MENOS, EOF};
int sincronizadores_complement_id[] = {MULTIPLICACAO, DIVISAO, DIV, RESTO, MAIS, MENOS, EOF};
int sincronizadores_expression_list_recursive[] = {FECHA_PARENTESES, EOF};
int sincronizadores_expression_ou_recursive[] = {OU, E, EOF};
int sincronizadores_expression_e_recursive[] = {OU, EOF};
int sincronizadores_expression_comparative_recursive[] = {E, EOF};
int sincronizadores_expression_relative_recursive[] = {IGUAL, DIFERENTE, OU, EOF};
int sincronizadores_expression_sum_recursive[] = {MENOR, MENOR_IGUAL, MAIOR_IGUAL, MAIOR, EOF};
int sincronizadores_expression_mult_recursive[] = {MAIS, MENOS, EOF};
int sincronizadores_comparative_operator[] = {NAO, MAIS, MENOS, IDENTIFICADOR, NUMERO_INTEIRO, NUMERO_REAL, VERDADEIRO, FALSO, LITERAL, ABRE_PARENTESES, EOF};
int sincronizadores_relative_operator[] = {NAO, MAIS, MENOS, IDENTIFICADOR, NUMERO_INTEIRO, NUMERO_REAL, VERDADEIRO, FALSO, LITERAL, ABRE_PARENTESES, EOF};
int sincronizadores_sum_operator[] = {NAO, MAIS, MENOS, IDENTIFICADOR, NUMERO_INTEIRO, NUMERO_REAL, VERDADEIRO, FALSO, LITERAL, ABRE_PARENTESES, EOF};
int sincronizadores_mult_operator[] = {NAO, MAIS, MENOS, IDENTIFICADOR, NUMERO_INTEIRO, NUMERO_REAL, VERDADEIRO, FALSO, LITERAL, ABRE_PARENTESES, EOF};
int sincronizadores_unit_operator[] = {NAO, MAIS, MENOS, IDENTIFICADOR, NUMERO_INTEIRO, NUMERO_REAL, VERDADEIRO, FALSO, LITERAL, ABRE_PARENTESES, EOF};

/** @brief  Construtor do analisador sintático. */
Program* sintatico_inicializar() {
    proximo_token = lexico_proximo_token();
    return program();
}

/** @brief  Destrutor do analisador sintático. */
void sintatico_finalizar() {}

/**
 * @brief   Verifica se o proximo_token está contido no conjunto de iniciais.
 * @param   iniciais Conjunto de elementos iniciais da produção.
 * @return  1 = Sim ou 0 = Não
 */
int sintatico_proximo_token_em_iniciais(int iniciais[]) {
    int i;
    int tamanho = iniciais[0];
    for(i = 1; i <= tamanho; i++) if(proximo_token == iniciais[i]) return 1;
    return 0;
}

/**
 * @brief   Verifica se o proximo_token está contido no conjunto de sincronizadores.
 * @param   sincronizadores Conjunto de elementos sincronizadores da produção.
 * @return  1 = Sim ou 0 = Não
 */
int sintatico_proximo_token_em_sincronizadores(int sincronizadores[]) {
    int i = 0;
    while(sincronizadores[i] != EOF) {
        if(proximo_token == sincronizadores[i]) return 1;
        i++;
    }
    return (proximo_token == EOF);
}

/**
 * @brief   Casa o token.
 * @param   token Token casado.
 * @return  O lexema do token casado.
 */
char* sintatico_casar(int token) {
    char* lexema_atual = (char*) calloc(strlen(lexico_obter_lexema())+1, sizeof(char));
    strcpy(lexema_atual, lexico_obter_lexema());
    if(IMPRIMIR_MATCH) {
        printf("MATCH: %s", lexico_nome_token(token));
        if(token == IDENTIFICADOR || token == NUMERO_INTEIRO || token == NUMERO_REAL || token == LITERAL) printf(".%s",lexema_atual);
        printf("\n");
    }
    if(proximo_token != EOF) proximo_token = lexico_proximo_token();
    return lexema_atual;
}

/**
 * @brief   Recupera o erro sintático para continuação da análise.
 * @param   sincronizadores Conjunto de sincronizadores da produção.
 */
char* sintatico_recuperar(int sincronizadores[]) {
    int linha = lexico_obter_numero_linha();

    if(proximo_token == EOF) erro(ERRO_TIPO_SINTATICO, ERRO_FIM_ARQUIVO_INESPERADO, linha);
    else if(proximo_token == IDENTIFICADOR || proximo_token == NUMERO_INTEIRO || proximo_token == NUMERO_REAL || proximo_token == LITERAL) {
        int tamanho = (strlen(lexico_nome_token(proximo_token))+1 + 1 + strlen(lexico_obter_lexema())+1);
        char* encontrado = (char*) calloc(tamanho, sizeof(char));
        strcpy(encontrado, lexico_nome_token(proximo_token));
        strcat(encontrado, ".");
        strcat(encontrado, lexico_obter_lexema());
        erro(ERRO_TIPO_SINTATICO, ERRO_TOKEN_INESPERADO, linha, encontrado);
        free(encontrado);
    }
    else erro(ERRO_TIPO_SINTATICO, ERRO_TOKEN_INESPERADO, linha, (char*) lexico_nome_token(proximo_token));

    /* Impressão do synch para debug. */
    if(IMPRIMIR_SYNCH) {
        int i = 0;
        printf("\tSYNCH = {");
        while(sincronizadores[i] != EOF) { printf("%s,",lexico_nome_token(sincronizadores[i])); i++; }
        printf(" EOF}\n");
    }

    if(!sintatico_proximo_token_em_sincronizadores(sincronizadores)) {
        do proximo_token = lexico_proximo_token();
        while(!sintatico_proximo_token_em_sincronizadores(sincronizadores));
    }
    if(proximo_token != EOF) {
        if(IMPRIMIR_MATCH) {
            printf("RECUPERACAO - %s", lexico_nome_token(proximo_token));
            if(proximo_token == IDENTIFICADOR || proximo_token == NUMERO_INTEIRO || proximo_token == NUMERO_REAL || proximo_token == LITERAL) printf(".%s",lexico_obter_lexema());
            printf("\n");
        }
    }
    return lexico_obter_lexema();
}

/**
 * @brief   Tenta casar o token e se não, recupera do erro sintático para continuação da análise.
 * @param   token Token que tentará ser casado.
 * @param   sincronizadores Conjunto de sincronizadores da produção.
 */
char* sintatico_casar_ou_recuperar(int token_esperado, int sincronizadores[]) {
    int linha = lexico_obter_numero_linha();
    if(token_esperado == proximo_token) return sintatico_casar(proximo_token);
    else {
        if(proximo_token == IDENTIFICADOR || proximo_token == NUMERO_INTEIRO || proximo_token == NUMERO_REAL || proximo_token == LITERAL) {
            int tamanho = (strlen(lexico_nome_token(proximo_token))+1 + 1 + strlen(lexico_obter_lexema())+1);
            char* encontrado = (char*) calloc(tamanho, sizeof(char));
            strcpy(encontrado, lexico_nome_token(proximo_token));
            strcat(encontrado, ".");
            strcat(encontrado, lexico_obter_lexema());
            erro_sintatico_token_esperado(linha, (char*) lexico_nome_token(token_esperado), encontrado);
            free(encontrado);
        }
        else erro_sintatico_token_esperado(linha, (char*) lexico_nome_token(token_esperado), (char*) lexico_nome_token(proximo_token));

        /* Impressão do synch para debug. */
        if(IMPRIMIR_SYNCH) {
            int i = 0;
            printf("\tSYNCH = {");
            while(sincronizadores[i] != EOF) { printf("%s,",lexico_nome_token(sincronizadores[i])); i++; }
            printf(" EOF}\n");
        }

        if(!sintatico_proximo_token_em_sincronizadores(sincronizadores)) {
            do proximo_token = lexico_proximo_token();
            while(!sintatico_proximo_token_em_sincronizadores(sincronizadores) && proximo_token != token_esperado);
        }
        if(proximo_token == token_esperado) {
            printf("RECUPERACAO - ");
            return sintatico_casar(token_esperado);
        }
        return lexico_obter_lexema();
    }
}

/** @brief  Produção: program. */
Program* program() {
    int linha = lexico_obter_numero_linha();

    VarList* _varlist = NULL;
    TypeList* _typelist = NULL;
    SubProgList* _subproglist = NULL;
    StatementList* _statementlist = NULL;

    if(IMPRIMIR_NOME_PRODUCOES) printf("Program\n");
    if(proximo_token == EOF) {
        erro(ERRO_TIPO_SINTATICO, ERRO_FIM_ARQUIVO_INESPERADO, linha);
    } else {
        sintatico_casar_ou_recuperar(INICIO, sincronizadores_program);
        instructions(&_varlist, &_typelist, &_subproglist, &_statementlist);
        sintatico_casar_ou_recuperar(FIM, sincronizadores_program);
        sintatico_casar_ou_recuperar(EOF, sincronizadores_program);
    }

    return new Program(_varlist, _typelist, _subproglist, _statementlist, linha);

}

/** @brief  Produção: instructions. */
void instructions(VarList** varlist, TypeList** typelist, SubProgList** subproglist, StatementList** statementlist) {
    int linha = lexico_obter_numero_linha();

    if(IMPRIMIR_NOME_PRODUCOES) printf("Instructions\n");
    if(sintatico_proximo_token_em_iniciais(iniciais_statement_start)) {
        Statement* _statement = statement_start();
        if(_statement != NULL){

            *statementlist = new StatementList(_statement, NULL,linha);
        }
        statement_list_optional(statementlist);
    }
    else if(proximo_token == IDENTIFICADOR) {
        char* lexema = (char*) calloc(strlen(lexico_obter_lexema())+1, sizeof(char));
        strcpy(lexema, lexico_obter_lexema());
        sintatico_casar(IDENTIFICADOR);
        /* Resolução do id. Neste ponto pode ser uma declaração (VarDecl), uma atribuição (Assign) ou chamada de função (Call). */
        Statement* statement = resolve_id(new Identificador(lexema,linha), varlist, typelist, subproglist, statementlist);
        /* Verifica se é o primeiro statement, instanciando o statement list e segue enfileirando novos Statements. */
        if(statement != NULL) *statementlist = new StatementList(statement, NULL,linha);
        statement_list_optional(statementlist);
    }
    else if(sintatico_proximo_token_em_iniciais(iniciais_subprogram_declaration)) {
        SubProgList* _subprogram_declaration = subprogram_declaration();
        if(_subprogram_declaration != NULL) {
            if(*subproglist == NULL) *subproglist = _subprogram_declaration;
            else {
                SubProgList* p = *subproglist;
                while(p->subproglist2 != NULL) p = p->subproglist2;
                p->subproglist2 = _subprogram_declaration;
            }
        }
        instructions(varlist, typelist, subproglist, statementlist);
    }
    else if(sintatico_proximo_token_em_iniciais(iniciais_type_declaration)) {
        TypeList* _type_declaration = type_declaration();
        if(_type_declaration != NULL) {
            if(*typelist == NULL) *typelist = _type_declaration;
            else {
                TypeList* p = *typelist;
                while(p->typelist != NULL) p = p->typelist;
                p->typelist = _type_declaration;
            }
        }
        instructions(varlist, typelist, subproglist, statementlist);
    }
    else if(sintatico_proximo_token_em_iniciais(iniciais_variable_declaration)) {
        VarList* _variable_declaration = variable_declaration();
        if(_variable_declaration != NULL) {
            if(*varlist == NULL) *varlist = _variable_declaration;
            else {
                VarList* p = *varlist;
                while(p->varlist != NULL) p = p->varlist;
                p->varlist = _variable_declaration;
            }
        }
        instructions(varlist, typelist, subproglist, statementlist);
    }
    else sintatico_recuperar(sincronizadores_instructions);
}

/** @brief  Produção: declarations. */
void declarations(VarList** _varlist, TypeList** _typelist, SubProgList** _subproglist) {
    int linha = lexico_obter_numero_linha();

    if(IMPRIMIR_NOME_PRODUCOES) printf("Declarations\n");
    if(sintatico_proximo_token_em_iniciais(iniciais_variable_declaration)) {
        VarList* _variable_declaration = variable_declaration();
        if(_variable_declaration != NULL) {
            if(*_varlist == NULL) *_varlist = _variable_declaration;
            else {
                VarList* p = *_varlist;
                while(p->varlist != NULL) p = p->varlist;
                p->varlist = _variable_declaration;
            }
        }
        declarations(_varlist, _typelist, _subproglist);
    }
    else if(sintatico_proximo_token_em_iniciais(iniciais_subprogram_declaration)) {
        SubProgList* _subprogram_declaration = subprogram_declaration();
        if(_subprogram_declaration != NULL) {
            if(*_subproglist == NULL) *_subproglist = _subprogram_declaration;
            else {
                SubProgList* p = *_subproglist;
                while(p->subproglist2 != NULL) p = p->subproglist2;
                p->subproglist2 = _subprogram_declaration;
            }
        }
        declarations(_varlist, _typelist, _subproglist);
    }
    else if(sintatico_proximo_token_em_iniciais(iniciais_type_declaration)) {
        TypeList* _type_declaration = type_declaration();
        if(_type_declaration != NULL) {
            if(*_typelist == NULL) *_typelist = _type_declaration;
            else {
                TypeList* p = *_typelist;
                while(p->typelist != NULL) p = p->typelist;
                p->typelist = _type_declaration;
            }
        }
        declarations(_varlist, _typelist, _subproglist);
    }
    else if(proximo_token == IDENTIFICADOR) {
        char* lexema = (char*) calloc(strlen(lexico_obter_lexema())+1, sizeof(char));
        strcpy(lexema, lexico_obter_lexema());
        sintatico_casar(IDENTIFICADOR);
        sintatico_casar_ou_recuperar(DOIS_PONTOS, sincronizadores_declarations);
        VarList* _id_list = id_list(new Type(new Identificador(lexema,linha), new NumberInteger((char*)"0",linha), new NumberInteger((char*)"0",linha),linha));
        if(_id_list != NULL) {
            if(*_varlist == NULL) *_varlist = _id_list;
            else {
                VarList* p = *_varlist;
                while(p->varlist != NULL) p = p->varlist;
                p->varlist = _id_list;
            }
        }
        sintatico_casar_ou_recuperar(PONTO_VIRGULA, sincronizadores_declarations);
        declarations(_varlist, _typelist, _subproglist);
    }
}

/** @brief  Produção: resolve_id. */
Statement* resolve_id(Identificador* _identificador, VarList** varlist, TypeList** typelist, SubProgList** subproglist, StatementList** statementlist) {
    int linha = lexico_obter_numero_linha();

    if(IMPRIMIR_NOME_PRODUCOES) printf("Resolve_Id\n");
    if(sintatico_proximo_token_em_iniciais(iniciais_statement_declaration)) {
        Variable* _variable = statement_declaration(_identificador);
        sintatico_casar_ou_recuperar(ATRIBUICAO, sincronizadores_statement_id);
        Expression* _expression = expression_ou();
        if(_expression != NULL) return new Assign(_variable, _expression,linha);
        else return NULL;
    }
    else if(proximo_token == ATRIBUICAO) {
        sintatico_casar(ATRIBUICAO);
        Expression* _expression_ou = expression_ou();
        if(_expression_ou != NULL) return new Assign(new Variable(_identificador,linha), _expression_ou,linha);
        else return NULL;
    } else if(sintatico_proximo_token_em_iniciais(iniciais_procedure_call)) {
        return (Statement*) procedure_call(_identificador);
    } else if(proximo_token == DOIS_PONTOS) {
        sintatico_casar(DOIS_PONTOS);
        VarList* _id_list = id_list(new Type(_identificador, new NumberInteger((char*)"0",linha), new NumberInteger((char*)"0",linha),linha));
        if(_id_list != NULL) {
            if(*varlist == NULL) *varlist = _id_list;
            else {
                VarList* p = *varlist;
                while(p->varlist != NULL) p = p->varlist;
                p->varlist = _id_list;
            }
        }
        sintatico_casar_ou_recuperar(PONTO_VIRGULA, sincronizadores_resolve_id);
        instructions(varlist, typelist, subproglist, statementlist);
    }
    return NULL;
}

/** @brief  Produção: id_list. */
VarList* id_list(Type* _type) {
    int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Id_List\n");
    char* lexema = sintatico_casar_ou_recuperar(IDENTIFICADOR, sincronizadores_id_list);
    VarList* _id_list_recursive = id_list_recursive(_type);
    return new VarList(_type, new Identificador(lexema,linha), _id_list_recursive,linha);

}

/** @brief  Produção: id_list_recursive. */
VarList* id_list_recursive(Type* _type) {
    int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Id_List_Recursive\n");
    if(proximo_token == VIRGULA) {
        sintatico_casar(VIRGULA);
        char* lexema = sintatico_casar_ou_recuperar(IDENTIFICADOR, sincronizadores_id_list_recursive);
        VarList* _id_list_recursive = id_list_recursive(_type);
        return new VarList(_type, new Identificador(lexema,linha), _id_list_recursive,linha);
    }
    return NULL;
}

/** @brief  Produção: variable_declaration. */
VarList* variable_declaration() {
    if(IMPRIMIR_NOME_PRODUCOES) printf("Variable_Declaration\n");
    Type* _type = type();
    sintatico_casar_ou_recuperar(DOIS_PONTOS, sincronizadores_variable_declaration);
    VarList* _id_list = id_list(_type);
    sintatico_casar_ou_recuperar(PONTO_VIRGULA, sincronizadores_variable_declaration);
    return _id_list;
}

/** @brief  Produção: variable_declaration_recursive. */
VarList* variable_declaration_recursive() {
    if(IMPRIMIR_NOME_PRODUCOES) printf("Variable_Declaration_Recursive\n");
    if(sintatico_proximo_token_em_iniciais(iniciais_type_2)) {
        Type* _type_2 = type_2();
        sintatico_casar_ou_recuperar(DOIS_PONTOS, sincronizadores_variable_declaration_recursive);
        VarList* _id_list = id_list(_type_2);
        sintatico_casar_ou_recuperar(PONTO_VIRGULA, sincronizadores_variable_declaration_recursive);
        VarList* _variable_declaration_recursive = variable_declaration_recursive();
        if(_variable_declaration_recursive != NULL) {
            VarList* p = _id_list;
            while(p->varlist != NULL) p = p->varlist;
            p->varlist = _variable_declaration_recursive;
        }
        return _id_list;
    }
    return NULL;
}

/** @brief  Produção: type. */
Type* type() {
    int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Type\n");
    if(sintatico_proximo_token_em_iniciais(iniciais_standard_type)) {
        int _standard_type = standard_type();
        if(_standard_type != ERRO){
            return new Type(new Primitive(_standard_type,linha),new NumberInteger((char*)"0",linha),new NumberInteger((char*)"0",linha),linha);
        }
        else  return NULL;
    }
    else if(proximo_token == VETOR) {
        sintatico_casar(VETOR);
        sintatico_casar_ou_recuperar(ABRE_COLCHETES, sincronizadores_type);
        char* lexema_min = sintatico_casar_ou_recuperar(NUMERO_INTEIRO, sincronizadores_type);
        sintatico_casar_ou_recuperar(DOIS_PONTOS, sincronizadores_type);
        char* lexema_max = sintatico_casar_ou_recuperar(NUMERO_INTEIRO, sincronizadores_type);
        sintatico_casar_ou_recuperar(FECHA_COLCHETES, sincronizadores_type);
        Type* _type_3 = type_3(new NumberInteger(lexema_min,linha), new NumberInteger(lexema_max,linha));
        if(_type_3 != NULL) return _type_3;
        else return NULL;
    }
    else {
        sintatico_recuperar(sincronizadores_type);
        return NULL;
    }
}

/** @brief  Produção: type_2. */
Type* type_2() {
    int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Type_2\n");
    if(sintatico_proximo_token_em_iniciais(iniciais_type)) {
        return type();
    }
    else if(proximo_token == IDENTIFICADOR) {
        char* lexema = (char*) calloc(strlen(lexico_obter_lexema())+1, sizeof(char));
        strcpy(lexema, lexico_obter_lexema());
        sintatico_casar(IDENTIFICADOR);
        return new Type(new Identificador(lexema,linha), new NumberInteger((char*)"0",linha), new NumberInteger((char*)"0",linha),linha);
    }
    else {
        sintatico_recuperar(sincronizadores_type_2);
        return NULL;
    }
}

/** @brief  Produção: type_3. */
Type* type_3(NumberInteger* lower, NumberInteger* upper) {
    int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Type_3\n");
    if(sintatico_proximo_token_em_iniciais(iniciais_standard_type)) {
        int _standard_type = standard_type();
        if(_standard_type != ERRO) return new Type(new Primitive(_standard_type,linha), lower, upper,linha);
        else return NULL;
    }
    else if(proximo_token == IDENTIFICADOR) {
        char* lexema = (char*) calloc(strlen(lexico_obter_lexema())+1, sizeof(char));
        strcpy(lexema, lexico_obter_lexema());
        sintatico_casar(IDENTIFICADOR);
        return new Type(new Identificador(lexema,linha), lower, upper,linha);
    }
    else {
        sintatico_recuperar(sincronizadores_type_3);
        return NULL;
    }
}

/** @brief  Produção: standard_type. */
int standard_type() {
    if(IMPRIMIR_NOME_PRODUCOES) printf("Standard_Type\n");
    switch(proximo_token) {
        case INTEIRO: sintatico_casar(INTEIRO); return INTEIRO;
        case REAL: sintatico_casar(REAL); return REAL;
        case CHAR: sintatico_casar(CHAR); return CHAR;
        case LOGICO: sintatico_casar(LOGICO); return LOGICO;
        default: sintatico_recuperar(sincronizadores_standard_type); return ERRO;
    }
}

/** @brief  Produção: subprogram_declaration. */
SubProgList* subprogram_declaration() {
    int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Subprogram_Declaration\n");
    SubProgList* _subprogram = subprogram();
    VarList* _varlist = NULL;
    TypeList* _typelist = NULL;
    SubProgList* _subproglist = NULL;
    declarations(&_varlist, &_typelist, &_subproglist);
    Statement* _compound_statement = compound_statement();
    if(_compound_statement != NULL) _subprogram->statementlist = new StatementList(_compound_statement, NULL,linha);
    if(_varlist != NULL) _subprogram->varlist = _varlist;
    if(_typelist != NULL) _subprogram->typelist = _typelist;
    if(_subproglist != NULL) _subprogram->subproglist1 = _subproglist;
    return _subprogram;
}

/** @brief  Produção: type_declaration. */
TypeList* type_declaration() {
    int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Type_Declaration\n");
    sintatico_casar_ou_recuperar(TIPO, sincronizadores_type_declaration);
    char* lexema = sintatico_casar_ou_recuperar(IDENTIFICADOR, sincronizadores_type_declaration);
    sintatico_casar_ou_recuperar(IGUAL, sincronizadores_type_declaration);
    sintatico_casar_ou_recuperar(REGISTRO, sincronizadores_type_declaration);
    Type* _type_2 = type_2();
    sintatico_casar_ou_recuperar(DOIS_PONTOS, sincronizadores_type_declaration);
    VarList* _id_list = id_list(_type_2);
    sintatico_casar_ou_recuperar(PONTO_VIRGULA, sincronizadores_type_declaration);
    VarList* _variable_declaration_recursive = variable_declaration_recursive();
    if(_variable_declaration_recursive != NULL) {
        VarList* p = _id_list;
        while(p->varlist != NULL) p = p->varlist;
        p->varlist = _variable_declaration_recursive;
    }
    sintatico_casar_ou_recuperar(FIMREGISTRO, sincronizadores_type_declaration);
    sintatico_casar_ou_recuperar(PONTO_VIRGULA, sincronizadores_type_declaration);
    return new TypeList(new Identificador(lexema,linha), _id_list, NULL,linha);
}

/** @brief  Produção: subprogram. */
SubProgList* subprogram() {
    int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Subprogram\n");
    switch(proximo_token) {
        case FUNCAO:
        {
            sintatico_casar(FUNCAO);
            char* lexema = sintatico_casar_ou_recuperar(IDENTIFICADOR, sincronizadores_subprogram);
            VarList* _arguments = arguments();
            sintatico_casar_ou_recuperar(DOIS_PONTOS, sincronizadores_subprogram);
            Type* _type_3 = type_3(new NumberInteger((char*)"0",linha), new NumberInteger((char*)"0",linha));
            sintatico_casar_ou_recuperar(PONTO_VIRGULA, sincronizadores_subprogram);
            return new SubProgList(_type_3, new Identificador(lexema,linha), _arguments, NULL, NULL, NULL, NULL, NULL,linha);
        }
        case PROCEDIMENTO:
        {
            sintatico_casar(PROCEDIMENTO);
            char* lexema = sintatico_casar_ou_recuperar(IDENTIFICADOR, sincronizadores_subprogram);
            VarList* _arguments = arguments();
            sintatico_casar_ou_recuperar(PONTO_VIRGULA, sincronizadores_subprogram);
            return new SubProgList(NULL, new Identificador(lexema,linha), _arguments, NULL, NULL, NULL, NULL, NULL,linha);
        }
        default:
            sintatico_recuperar(sincronizadores_subprogram);
            return NULL;
    }
}

/** @brief  Produção: arguments. */
VarList* arguments() {
    if(IMPRIMIR_NOME_PRODUCOES) printf("Arguments\n");
    if(proximo_token == ABRE_PARENTESES) {
        sintatico_casar(ABRE_PARENTESES);
        VarList* _id_list = id_list(NULL);
        sintatico_casar_ou_recuperar(FECHA_PARENTESES, sincronizadores_arguments);
        return _id_list;
    }
    return NULL;
}

/** @brief  Produção: compound_statement. */
Statement* compound_statement() {
    int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Compound_Statement\n");
    sintatico_casar_ou_recuperar(INICIO, sincronizadores_compound_statement);
    StatementList* _statementlist = NULL;
    _statementlist = optional_statement(_statementlist);
    sintatico_casar_ou_recuperar(FIM, sincronizadores_compound_statement);
    return new CompoundStatement(_statementlist,linha);
}

/** @brief  Produção: optional_statement. */
StatementList* optional_statement(StatementList* statementlist) {
    if(IMPRIMIR_NOME_PRODUCOES) printf("Optional_Statement\n");
    if(sintatico_proximo_token_em_iniciais(iniciais_statement_list)) {
        return statement_list(&statementlist);
    }
    return NULL;
}

/** @brief  Produção: statement_list. */
StatementList* statement_list(StatementList** statementlist) {
    int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Statement_List\n");
    Statement* _statement = statement();
    StatementList* stmtlistprox = NULL;
    if(_statement != NULL){
        stmtlistprox = new StatementList(_statement, NULL,linha);
        statement_list_optional(&stmtlistprox);
        /* Iniciando um statement list do zero. */
        if(*statementlist == NULL) return stmtlistprox;
        /* Continuando uma lista. */
        (*statementlist)->statementlist = stmtlistprox;
        return stmtlistprox;
    }
    statement_list_optional(statementlist);
    return *statementlist;
}

/** @brief  Produção: statement_list_optional. */
void statement_list_optional(StatementList** statementlist) {
    if(IMPRIMIR_NOME_PRODUCOES) printf("Statement_List_Optional\n");
    if(proximo_token == PONTO_VIRGULA) {
        sintatico_casar(PONTO_VIRGULA);
        statement_list_recursive(statementlist);
    }
}

/** @brief  Produção: statement_list_recursive. */
void statement_list_recursive(StatementList** statementlist) {
    if(IMPRIMIR_NOME_PRODUCOES) printf("Statement_List_Recursive\n");
    if(sintatico_proximo_token_em_iniciais(iniciais_statement_list)) {
        statement_list(statementlist);
    }
}

/** @brief  Produção: statement. */
Statement* statement() {
    int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Statement\n");
    if(proximo_token == IDENTIFICADOR) {
        char* lexema = (char*) calloc(strlen(lexico_obter_lexema())+1, sizeof(char));
        strcpy(lexema, lexico_obter_lexema());
        sintatico_casar(IDENTIFICADOR);
        return (Statement*) statement_id(new Identificador(lexema,linha));
    }
    else if(sintatico_proximo_token_em_iniciais(iniciais_compound_statement)) return compound_statement();
    else if(sintatico_proximo_token_em_iniciais(iniciais_conditional)) return conditional();
    else {
        switch(proximo_token) {
            case ENQUANTO:
            {
                sintatico_casar(ENQUANTO);
                Expression* expression = expression_ou();
                sintatico_casar_ou_recuperar(FACA, sincronizadores_statement_start);
                Statement* _statement = statement();
                sintatico_casar_ou_recuperar(FIMFACA, sincronizadores_statement_start);
                if(expression==NULL) return NULL;
                return new While(expression, _statement,linha);
            }
            case LEIA:
            {
                sintatico_casar(LEIA);
                sintatico_casar_ou_recuperar(ABRE_PARENTESES, sincronizadores_statement_start);
                ExprList* expressionlist = expression_list();
                sintatico_casar_ou_recuperar(FECHA_PARENTESES, sincronizadores_statement_start);
                if(expressionlist==NULL) return NULL;
                return new Read(expressionlist,linha);
            }
            case IMPRIMA:
            {
                sintatico_casar(IMPRIMA);
                sintatico_casar_ou_recuperar(ABRE_PARENTESES, sincronizadores_statement_start);
                ExprList* expressionlist = expression_list();
                sintatico_casar_ou_recuperar(FECHA_PARENTESES, sincronizadores_statement_start);
                if(expressionlist==NULL) return NULL;
                return new Write(expressionlist,linha);
            }
            default: sintatico_recuperar(sincronizadores_statement_start);
        }
    }
    return NULL;
}

/** @brief  Produção: statement_start. */
Statement* statement_start() {
    int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Statement_Start\n");
    if(sintatico_proximo_token_em_iniciais(iniciais_compound_statement)) return compound_statement();
    else if(sintatico_proximo_token_em_iniciais(iniciais_conditional)) return conditional();
    else {
        switch (proximo_token) {
            case ENQUANTO:
            {
                sintatico_casar(ENQUANTO);
                Expression* expression = expression_ou();
                sintatico_casar_ou_recuperar(FACA, sincronizadores_statement_start);
                Statement* _statement = statement();
                sintatico_casar_ou_recuperar(FIMFACA, sincronizadores_statement_start);
                if(expression==NULL) return NULL;
                return new While(expression, _statement,linha);
            }
            case LEIA:
            {
                sintatico_casar(LEIA);
                sintatico_casar_ou_recuperar(ABRE_PARENTESES, sincronizadores_statement_start);
                ExprList* expressionlist = expression_list();
                sintatico_casar_ou_recuperar(FECHA_PARENTESES, sincronizadores_statement_start);
                if(expressionlist==NULL) return NULL;
                return new Read(expressionlist,linha);
            }
            case IMPRIMA:
            {
                sintatico_casar(IMPRIMA);
                sintatico_casar_ou_recuperar(ABRE_PARENTESES, sincronizadores_statement_start);
                ExprList* expressionlist = expression_list();
                sintatico_casar_ou_recuperar(FECHA_PARENTESES, sincronizadores_statement_start);
                if(expressionlist==NULL) return NULL;
                return new Write(expressionlist,linha);
            }
            default: sintatico_recuperar(sincronizadores_statement_start);
        }
    }
    return NULL;
}

/** @brief  Produção: statement_id. */
Expression* statement_id(Identificador* _identificador) {
    int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Statement_Id\n");
    if(sintatico_proximo_token_em_iniciais(iniciais_statement_declaration)) {
        Variable* variable = statement_declaration(_identificador);
        sintatico_casar_ou_recuperar(ATRIBUICAO, sincronizadores_statement_id);
        Expression* expression = expression_ou();
        if(expression != NULL) return new Assign(variable, expression,linha);
        else return NULL;
    } else if(proximo_token == ATRIBUICAO) {
        sintatico_casar(ATRIBUICAO);
        Variable* variable = statement_declaration(_identificador);
        Expression* expression = expression_ou();
        if(expression != NULL) return new Assign(variable, expression,linha);
        else return NULL;
    } else if(sintatico_proximo_token_em_iniciais(iniciais_procedure_call)) {
        return procedure_call(_identificador);
    }
    //return NULL;
    return new Variable(_identificador,linha);
}

/** @brief  Produção: statement_declaration. */
Variable* statement_declaration(Identificador* _identificador) {
    int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Statement_Declaration\n");
    if(proximo_token == ABRE_COLCHETES) {
        sintatico_casar(ABRE_COLCHETES);
        Expression* _expression_ou = expression_ou();
        sintatico_casar_ou_recuperar(FECHA_COLCHETES, sincronizadores_declarations);
        Variable* _optional_id = optional_id();
        return new Variable(_optional_id, _identificador, _expression_ou,linha);
    }
    else if(sintatico_proximo_token_em_iniciais(iniciais_optional_id)) {
        Variable* _optional_id = optional_id();
        return new Variable(_optional_id, _identificador, NULL,linha);
    }
    return new Variable(_identificador,linha);
}

/** @brief  Produção: optional_id. */
Variable* optional_id() {
    int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Optional_Id\n");
    if(proximo_token == PONTO) {
        sintatico_casar(PONTO);
        char* lexema = sintatico_casar_ou_recuperar(IDENTIFICADOR, sincronizadores_optional_id);
        Variable* _statement_declaration = statement_declaration(new Identificador(lexema,linha));
        return _statement_declaration;
    }
    return NULL;
}

/** @brief  Produção: procedure_call. */
Expression* procedure_call(Identificador* _identificador) {
      int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Procedure_Call\n");
    if(proximo_token == ABRE_PARENTESES) {
        sintatico_casar(ABRE_PARENTESES);
        ExprList* _expression_list = expression_list();
        sintatico_casar_ou_recuperar(FECHA_PARENTESES, sincronizadores_procedure_call);
        return new Call(_identificador,_expression_list,linha);
    }
    return NULL;
}

/** @brief  Produção: conditional. */
Statement* conditional() {
      int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Conditional\n");
    sintatico_casar_ou_recuperar(SE, sincronizadores_conditional);
    Expression* expression = expression_ou();
    sintatico_casar_ou_recuperar(ENTAO, sincronizadores_conditional);
    Statement* statementse = statement();
    Statement* statementelse = optional_senao();
    if(expression==NULL) return NULL;
    return new If(expression, statementse, statementelse,linha);
}

/** @brief  Produção: optional_senao. */
Statement* optional_senao() {
    if(IMPRIMIR_NOME_PRODUCOES) printf("Optional_Senao\n");
    if(proximo_token == SENAO) {
        sintatico_casar(SENAO);
        Statement* _statement = statement();
        sintatico_casar_ou_recuperar(FIMSE, sincronizadores_optional_senao);
        return _statement;
    }
    else if(proximo_token == FIMSE) sintatico_casar(FIMSE);
    else sintatico_recuperar(sincronizadores_optional_senao);
    return NULL;
}

/** @brief  Produção: expression_list. */
ExprList* expression_list() {
      int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Expression_List\n");
    Expression* _expression_ou = expression_ou();
    ExprList* _expression_list_recursive = expression_list_recursive();
    if(_expression_ou == NULL) return NULL;
    return new ExprList(_expression_ou, _expression_list_recursive,linha);
}

/** @brief  Produção: expression_list_recursive. */
ExprList* expression_list_recursive() {
      int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Expression_List_Recursive\n");
    if(proximo_token == VIRGULA) {
        sintatico_casar(VIRGULA);
        Expression* _expression_ou = expression_ou();
        ExprList* _expression_list_recursive = expression_list_recursive();
        return new ExprList(_expression_ou, _expression_list_recursive,linha);
    }
    return NULL;
}

/** @brief  Produção: expression_ou. */
Expression* expression_ou() {
      int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Expression_Ou\n");
    Expression* _expression_e = expression_e();
    Expression* _expression_ou_recursive = expression_ou_recursive();
    if(_expression_ou_recursive == NULL || _expression_e == NULL) return _expression_e;
    return new BooleanOp(OU, _expression_e, _expression_ou_recursive,linha);
}

/** @brief  Produção: expression_ou_recursive. */
Expression* expression_ou_recursive() {
      int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Expression_Ou_Recursive\n");
    if(proximo_token == OU) {
        sintatico_casar(OU);
        Expression* _expression_e = expression_e();
        Expression* _expression_ou_recursive = expression_ou_recursive();
        if(_expression_ou_recursive == NULL) return _expression_e;
        return new BooleanOp(OU, _expression_e, _expression_ou_recursive,linha);
    }
    return NULL;
}

/** @brief  Produção: expression_e. */
Expression* expression_e() {
      int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Expression_E\n");
    Expression* _expression_comparative = expression_comparative();
    Expression* _expression_e_recursive = expression_e_recursive();
    if(_expression_e_recursive == NULL || _expression_comparative == NULL) return _expression_comparative;
    return new BooleanOp(E, _expression_comparative, _expression_e_recursive,linha);
}

/** @brief  Produção: expression_e_recursive. */
Expression* expression_e_recursive() {
      int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Expression_E_Recursive\n");
    if(proximo_token == E) {
        sintatico_casar(E);
        Expression* _expression_comparative = expression_comparative();
        Expression* _expression_e_recursive = expression_e_recursive();
        if(_expression_e_recursive == NULL) return _expression_comparative;
        return new BooleanOp(E, _expression_comparative, _expression_e_recursive,linha);
    }
    return NULL;
}

/** @brief  Produção: expression_comparative. */
Expression* expression_comparative() {
      int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Expression_Comparative\n");
    Expression* _expression_relative = expression_relative();
    int compOp = ERRO;
    Expression* _expression_comparative_recursive = expression_comparative_recursive(&compOp);
    if(_expression_comparative_recursive == NULL || compOp == ERRO || _expression_relative == NULL) return _expression_relative;
    return new ComparativeOp(compOp, _expression_relative, _expression_comparative_recursive,linha);
}

/** @brief  Produção: expression_comparative_recursive. */
Expression* expression_comparative_recursive(int* compOp) {
      int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Expression_Comparative_Recursive\n");
    if(sintatico_proximo_token_em_iniciais(iniciais_comparative_operator)) {
        *compOp = comparative_operator();
        int compOpLocal = ERRO;
        Expression* _expression_relative = expression_relative();
        Expression* _expression_comparative_recursive = expression_comparative_recursive(&compOpLocal);
        if(_expression_comparative_recursive == NULL) return _expression_relative;
        return new ComparativeOp(compOpLocal, _expression_relative, _expression_comparative_recursive,linha);
    }
    return NULL;
}

/** @brief  Produção: expression_relative. */
Expression* expression_relative() {
      int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Expression_Relative\n");
    Expression* _expression_sum = expression_sum();
    int relOp = ERRO;
    Expression* _expression_relative_recursive = expression_relative_recursive(&relOp);
    if(_expression_relative_recursive == NULL || relOp == ERRO || _expression_sum == NULL) return _expression_sum;
    return new RelationOp(relOp, _expression_sum, _expression_relative_recursive,linha);
}

/** @brief  Produção: expression_relative_recursive. */
Expression* expression_relative_recursive(int* relOp) {
      int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Expression_Relative_Recursive\n");
    if(sintatico_proximo_token_em_iniciais(iniciais_relative_operator)) {
        *relOp = relative_operator();
        int relOpLocal = ERRO;
        Expression* _expression_sum = expression_sum();
        Expression* _expression_relative_recursive = expression_relative_recursive(&relOpLocal);
        if(_expression_relative_recursive == NULL) return _expression_sum;
        return new MultiplicationOp(relOpLocal, _expression_sum, _expression_relative_recursive,linha);
    }
    return NULL;
}

/** @brief  Produção: expression_sum. */
Expression* expression_sum() {
      int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Expression_Sum\n");
    Expression* _expression_mult = expression_mult();
    int sumOp = ERRO;
    Expression* _expression_sum_recursive = expression_sum_recursive(&sumOp);
    if(_expression_sum_recursive == NULL || sumOp == ERRO || _expression_mult == NULL) return _expression_mult;
    return new AdditionalOp(sumOp, _expression_mult, _expression_sum_recursive,linha);
}

/** @brief  Produção: expression_sum_recursive. */
Expression* expression_sum_recursive(int* sumOp) {
      int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Expression_Sum_Recursive\n");
    if(sintatico_proximo_token_em_iniciais(iniciais_sum_operator)) {
        *sumOp = sum_operator();
        int sumOpLocal = ERRO;
        Expression* _expression_mult = expression_mult();
        Expression* _expression_sum_recursive = expression_sum_recursive(&sumOpLocal);
        if(_expression_sum_recursive == NULL) return _expression_mult;
        return new AdditionalOp(sumOpLocal, _expression_mult, _expression_sum_recursive,linha);
    }
    return NULL;
}

/** @brief  Produção: expression_mult. */
Expression* expression_mult() {
      int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Expression_Mult\n");
    Expression* _expression_unit = expression_unit();
    int multOp = ERRO;
    Expression* _expression_mult_recursive = expression_mult_recursive(&multOp);
    if(_expression_mult_recursive == NULL || multOp == ERRO || _expression_unit == NULL) return _expression_unit;
    return new MultiplicationOp(multOp, _expression_unit, _expression_mult_recursive,linha);
}

/** @brief  Produção: expression_mult_recursive. */
Expression* expression_mult_recursive(int* multOp) {
      int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Expression_Mult_Recursive\n");
    if(sintatico_proximo_token_em_iniciais(iniciais_mult_operator)) {
        *multOp = mult_operator();
        int multOpLocal = ERRO;
        Expression* _expression_unit = expression_unit();
        Expression* _expression_mult_recursive = expression_mult_recursive(&multOpLocal);
        if(_expression_mult_recursive == NULL) return _expression_unit;
        return new MultiplicationOp(multOpLocal, _expression_unit, _expression_mult_recursive,linha);
    }
    return NULL;
}

/** @brief  Produção: expression_unit. */
Expression* expression_unit() {
      int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Expression_Unit\n");
    if(sintatico_proximo_token_em_iniciais(iniciais_unit_operator)) {
        int unitOp = unit_operator();
        Expression* _expression_unit = expression_unit();
        if(unitOp == NAO && _expression_unit != NULL) return new Not(_expression_unit,linha);
        else if(unitOp != ERRO && _expression_unit != NULL) return new Sign(unitOp, _expression_unit,linha);
        return NULL;
    }
    else if(sintatico_proximo_token_em_iniciais(iniciais_factor)) return factor();
    else sintatico_recuperar(sincronizadores_expression_unit); return NULL;
}

/** @brief  Produção: factor. */
Expression* factor() {
      int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Factor\n");
    char* lexema = (char*) calloc(strlen(lexico_obter_lexema())+1, sizeof(char));
    strcpy(lexema, lexico_obter_lexema());
    switch(proximo_token) {
        case IDENTIFICADOR:
            sintatico_casar(IDENTIFICADOR);
            return complement_id(new Identificador(lexema,linha));
        case NUMERO_INTEIRO:
            sintatico_casar(NUMERO_INTEIRO);
            return new NumberInteger(lexema,linha);
        case NUMERO_REAL:
            sintatico_casar(NUMERO_REAL);
            return new NumberReal(lexema,linha);
        case VERDADEIRO:
            sintatico_casar(VERDADEIRO);
            return new Boolean(VERDADEIRO,linha);
        case FALSO:
            sintatico_casar(FALSO);
            return new Boolean(FALSO,linha);
        case LITERAL:
            sintatico_casar(LITERAL);
            return new Literal(lexema,linha);
        case ABRE_PARENTESES:
        {
            sintatico_casar(ABRE_PARENTESES);
            Expression* _expression_ou = expression_ou();
            sintatico_casar_ou_recuperar(FECHA_PARENTESES, sincronizadores_factor);
            return _expression_ou;
        }
        default:
            sintatico_recuperar(sincronizadores_factor);
            return NULL;
    }
}

/** @brief  Produção: complement_id. */
Expression* complement_id(Identificador* _identificador) {
      int linha = lexico_obter_numero_linha();
    if(IMPRIMIR_NOME_PRODUCOES) printf("Complement_Id\n");
    if(sintatico_proximo_token_em_iniciais(iniciais_statement_declaration)) {
        return statement_declaration(_identificador);
    }
    else if(sintatico_proximo_token_em_iniciais(iniciais_procedure_call)) {
        return procedure_call(_identificador);
    }
    return new Variable(_identificador,linha);
}

/** @brief  Produção: comparative_operator. */
int comparative_operator() {
    if(IMPRIMIR_NOME_PRODUCOES) printf("Comparative_Operator\n");
    switch(proximo_token) {
        case IGUAL: sintatico_casar(IGUAL); return IGUAL;
        case DIFERENTE: sintatico_casar(DIFERENTE); return DIFERENTE;
        default: sintatico_recuperar(sincronizadores_comparative_operator); return ERRO;
    }
}

/** @brief  Produção: relative_operator. */
int relative_operator() {
    if(IMPRIMIR_NOME_PRODUCOES) printf("Relative_Operator\n");
    switch(proximo_token) {
        case MENOR: sintatico_casar(MENOR); return MENOR;
        case MENOR_IGUAL: sintatico_casar(MENOR_IGUAL); return MENOR_IGUAL;
        case MAIOR: sintatico_casar(MAIOR); return MAIOR;
        case MAIOR_IGUAL: sintatico_casar(MAIOR_IGUAL); return MAIOR_IGUAL;
        default: sintatico_recuperar(sincronizadores_relative_operator); return ERRO;
    }
}

/** @brief  Produção: sum_operator. */
int sum_operator() {
    if(IMPRIMIR_NOME_PRODUCOES) printf("Sum_Operator\n");
    switch(proximo_token) {
        case MAIS: sintatico_casar(MAIS); return MAIS;
        case MENOS: sintatico_casar(MENOS); return MENOS;
        default: sintatico_recuperar(sincronizadores_sum_operator);
    }
    return ERRO;
}

/** @brief  Produção: mult_operator. */
int mult_operator() {
    if(IMPRIMIR_NOME_PRODUCOES) printf("Mult_Operator\n");
    switch(proximo_token) {
        case MULTIPLICACAO: sintatico_casar(MULTIPLICACAO); return MULTIPLICACAO;
        case DIVISAO: sintatico_casar(DIVISAO); return DIVISAO;
        case DIV: sintatico_casar(DIV); return DIV;
        case RESTO: sintatico_casar(RESTO); return RESTO;
        default: sintatico_recuperar(sincronizadores_mult_operator); return ERRO;
    }
}

/** @brief  Produção: unit_operator. */
int unit_operator() {
    if(IMPRIMIR_NOME_PRODUCOES) printf("Unit_Operator\n");
    switch(proximo_token) {
        case NAO: sintatico_casar(NAO); return NAO;
        case MAIS: sintatico_casar(MAIS); return MAIS;
        case MENOS: sintatico_casar(MENOS); return MENOS;
        default: sintatico_recuperar(sincronizadores_unit_operator); return ERRO;
    }
}

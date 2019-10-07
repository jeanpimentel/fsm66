#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <typeinfo>
#include <iostream>

#include "arvore_sintaxe_abstrata.h"
#include "simbolos.h"
#include "tabela_simbolos.h"
#include "erro.h"
#include "semantico.h"
#include "semantico_escopo.h"

#include "lexico.h"

int esc_tipo_variavel = 0;
int esc_tipo_retorno  = 0;
char* esc_lexema_id;
char* esc_nome_funcao;
int esc_lowerbound = 0;
int esc_upperbound = 0;
bool esc_tem_parametros = false;
int esc_quantidade_parametros = 0;
argumento* esc_argumentos = NULL;
bool esc_lado_esquerdo = false;
int esc_quantidade_retorno = 0;

simbolo_funcao* esc_funcao_sendo_declarada = NULL;
tabela_simbolos* esc_funcoes_declaradas = new tabela_simbolos();
tabela_simbolos* esc_nome_funcoes_declaradas = new tabela_simbolos();


void SemanticoEscopoVisitor::visit(Program* _program) {
    iniciar_escopo("_PROGRAMA_");
    if(_program->typelist != NULL) _program->typelist->accept(this);
    if(_program->varlist != NULL) _program->varlist->accept(this);
    if(_program->subproglist != NULL) _program->subproglist->accept(this);
    if(_program->statementlist != NULL) _program->statementlist->accept(this);
    escopo_program = finalizar_escopo();
}

void SemanticoEscopoVisitor::visit(SubProgList* _subproglist) {

    simbolo_funcao* s = new simbolo_funcao();

    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("NOME DA FUNCAO\n");
    _subproglist->identificador->accept(this);

    char* esc_nome_funcao = (char*) malloc((strlen(esc_lexema_id) + 1) * sizeof(char));
    strcpy(esc_nome_funcao, esc_lexema_id);
    s->nome_funcao = esc_nome_funcao;
    esc_nome_funcoes_declaradas->inserir(esc_nome_funcao, new simbolo());

    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("RETORNO DA FUNCAO\n");
    esc_tipo_retorno = 0;
    if(_subproglist->type != NULL) _subproglist->type->accept(this);
    if(esc_tipo_retorno == 0) s->tem_retorno = false;
    else {
        s->tem_retorno = true;
        s->tipo_retorno = esc_tipo_retorno;
    }

    iniciar_escopo((const char*) esc_nome_funcao);

    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("TIPOS DA FUNCAO\n");
    if(_subproglist->typelist != NULL) _subproglist->typelist->accept(this);

    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("VARIAVEIS DA FUNCAO\n");
    if(_subproglist->varlist != NULL) _subproglist->varlist->accept(this);

    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("PARAMETROS DA FUNCAO\n");
    if(_subproglist->arguments != NULL) {
        iniciar_escopo("_PARAMETROS_");
        esc_tem_parametros = true;
        esc_quantidade_parametros = 0;
       /** DEsalocar CADA N�*/
        esc_argumentos = NULL;
        _subproglist->arguments->accept(this);
        s->quantidade_parametros = esc_quantidade_parametros;
        esc_tem_parametros = false;
        s->parametros = esc_argumentos;
        finalizar_escopo();
    }

    simbolo_funcao* temp = new simbolo_funcao();
    temp->nome_funcao = s->nome_funcao;
    temp->quantidade_parametros = s->quantidade_parametros;
    temp->tem_retorno = s->tem_retorno;
    esc_funcoes_declaradas->inserir(temp->nome_funcao, temp);

    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("FUNCOES DA FUNCAO\n");
    if(_subproglist->subproglist1 != NULL) {
        _subproglist->subproglist1->accept(this);
    }

    esc_funcao_sendo_declarada = s;
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("CORPO DA FUNCAO %s\n",esc_funcao_sendo_declarada->nome_funcao);
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("\t\t---------------------------------------> aqui comeca o corpo da funcao\n\n");
    esc_quantidade_retorno = 0;
    if(_subproglist->statementlist != NULL) _subproglist->statementlist->accept(this);
    if(s->tem_retorno && esc_quantidade_retorno == 0) {
        erro_incrementar();
        fprintf(stderr, "ERRO SEMANTICO: FUNCAO %s NA LINHA %d NAO POSSUI RETORNO.\n", esc_nome_funcao,_subproglist->linha);
    }
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("\t\t---------------------------------------> aqui termina o corpo da funcao\n\n");
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("\t\t---------------------------------------> retornos: %d\n\n", esc_quantidade_retorno);
    esc_funcao_sendo_declarada = NULL;

    s->escopo_funcao = finalizar_escopo();

    if(buscar_escopo_atual(esc_nome_funcao) == NULL) escopo_atual->tab_funcoes->inserir(esc_nome_funcao, s);
    else {
        if(s->tem_retorno) {
            erro_incrementar();
            fprintf(stderr, "ERRO SEMANTICO: %s NA LINHA %d FOI DECLARADA ANTERIORMENTE \n", esc_nome_funcao,_subproglist->linha);
        }
        else {
            erro_incrementar();
            fprintf(stderr, "ERRO SEMANTICO: %s NA LINHA %d FOI DECLARADO ANTERIORMENTE \n", esc_nome_funcao,_subproglist->linha);
        }
    }

    if(_subproglist->subproglist2 != NULL) _subproglist->subproglist2->accept(this);
}

void SemanticoEscopoVisitor::visit(TypeList* _typelist) {
    iniciar_escopo("_REGISTRO_");
    if(_typelist->varlist != NULL) _typelist->varlist->accept(this);
    escopo* escopo_registro = finalizar_escopo();
    _typelist->identificador->accept(this); //nome do registro

    if(buscar_escopo_atual(esc_lexema_id) == NULL) escopo_atual->tab_registros->inserir(esc_lexema_id, new simbolo_registro(escopo_registro));
    else {
        erro_incrementar();
        fprintf(stderr, "ERRO SEMANTICO: %s JA DECLARADO ANTERIORMENTE NA LINHA %d \n", esc_lexema_id,_typelist->linha);
    }
    if(_typelist->typelist != NULL) _typelist->typelist->accept(this);
}

void SemanticoEscopoVisitor::visit(VarList* _varlist) {
    int tipo_variavel = NULL;
    if(_varlist->type != NULL) {
        _varlist->type->accept(this);
        tipo_variavel = esc_tipo_variavel;
    }
    char* nome_tipo = NULL;
    if(esc_tipo_variavel == IDENTIFICADOR) {
        nome_tipo = (char*) malloc((strlen(esc_lexema_id) + 1) * sizeof(char));
        strcpy(nome_tipo, esc_lexema_id);
    }

    _varlist->identificador->accept(this);

    if(esc_tem_parametros) {
        esc_quantidade_parametros++;
        simbolo_variavel* var = (simbolo_variavel*) escopo_atual->pai->tab_variaveis->buscar(esc_lexema_id);
        if(var == NULL) {
            erro_incrementar();
            fprintf(stderr, "ERRO SEMANTICO: PARAMETRO %s NAO DECLARADO NA LINHA %d \n", esc_lexema_id,_varlist->linha);
        }
        else {
            escopo_atual->tab_variaveis->inserir(esc_lexema_id, var);

            if(esc_argumentos==NULL){
                esc_argumentos = new argumento();
                esc_argumentos->var = var;
                esc_argumentos->prox = NULL;
            }else{
                argumento* argumentos = esc_argumentos;
                 while(argumentos->prox!=NULL){
                    argumentos = argumentos->prox;
                }
                argumentos->prox         = new argumento();
                argumentos->prox->var    = var;
                argumentos->prox->prox   = NULL;
            }

        }
    }
    else {
        if(esc_nome_funcoes_declaradas->buscar(esc_lexema_id) != NULL) {
            erro_incrementar();
            fprintf(stderr, "ERRO SEMANTICO: %s NA LINHA %d FOI DECLARADA COMO FUNCAO ANTERIORMENTE \n", esc_lexema_id,_varlist->linha);
        }
        else {
            simbolo_registro* reg = buscar_registro_nos_escopos(esc_lexema_id);
            if(reg != NULL) {
                erro_incrementar();
                fprintf(stderr, "ERRO SEMANTICO: %s NA LINHA %d FOI DECLARADA COMO REGISTRO ANTERIORMENTE \n", esc_lexema_id, _varlist->linha);
            }
            else {
                int nlocal = escopo_atual->pai == NULL?1:0;
                if(buscar_escopo_atual(esc_lexema_id) == NULL) escopo_atual->tab_variaveis->inserir(esc_lexema_id, new simbolo_variavel(tipo_variavel, nome_tipo, esc_lowerbound, esc_upperbound, nlocal));
                else {
                    erro_incrementar();
                    fprintf(stderr, "ERRO SEMANTICO: VARIAVEL %s JA DECLARADA ANTERIORMENTE NA LINHA %d \n", esc_lexema_id, _varlist->linha);
                }
            }
        }
    }

    if(_varlist->varlist != NULL) _varlist->varlist->accept(this);
}

void SemanticoEscopoVisitor::visit(Type* _type) {
    if(_type->identificador != NULL) {
        _type->identificador->accept(this);
        simbolo_registro* sreg = buscar_registro_nos_escopos(esc_lexema_id);
        if(sreg == NULL) {
            erro_incrementar();
            fprintf(stderr, "ERRO SEMANTICO: TIPO %s NAO DECLARADO NA LINHA %d \n", esc_lexema_id, _type->linha);
        }
    }
    else if(_type->primitive != NULL) {
        _type->primitive->accept(this);
    }
    esc_lowerbound = atoi(_type->lowerbound->numberinteger);
    esc_upperbound = atoi(_type->upperbound->numberinteger);
}

void SemanticoEscopoVisitor::visit(If* _if) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("if!\n");
    _if->expression->accept(this);
    if(_if->statement1 != NULL) _if->statement1->accept(this);
    if(_if->statement2 != NULL) _if->statement2->accept(this);
}

void SemanticoEscopoVisitor::visit(While* _while) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("while!\n");
    _while->expression->accept(this);
    if(_while->statement != NULL) _while->statement->accept(this);
}

void SemanticoEscopoVisitor::visit(Read* _read) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("read!\n");
    _read->exprlist->accept(this);
}

void SemanticoEscopoVisitor::visit(Write* _write) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("write!\n");
    _write->exprlist->accept(this);
}

void SemanticoEscopoVisitor::visit(Assign* _assign) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("assign!\n");
    esc_lado_esquerdo = true;
    _assign->variable->accept(this);
    esc_lado_esquerdo = false;
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("to indo la em expression!\n");
    _assign->expression->accept(this);
}

void SemanticoEscopoVisitor::visit(StatementList* _stmtlist) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("\n\nstmt list!\n");
    _stmtlist->statement->accept(this);
    if(_stmtlist->statementlist != NULL) _stmtlist->statementlist->accept(this);
}

void SemanticoEscopoVisitor::visit(CompoundStatement* _compoundstmt) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("compound!\n");
    if(_compoundstmt->statementlist != NULL) _compoundstmt->statementlist->accept(this);
}

void SemanticoEscopoVisitor::visit(Variable* _variable) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("---------> variable!\n");
    _variable->identificator->accept(this);

    bool subfuncao = false;
    escopo* e = escopo_atual;
    do {
        if(strcmp(e->nome, esc_lexema_id) != 0) {
            subfuncao = true;
            break;
        }
        e = e->pai;
    }
    while(e->pai);

    if(esc_funcao_sendo_declarada != NULL) {
        if(esc_lado_esquerdo && strcmp(esc_funcao_sendo_declarada->nome_funcao, esc_lexema_id) == 0) {
//            printf("\t\t\t--------------- funcao ---------------------> %s\n", esc_funcao_sendo_declarada->nome_funcao);
//            printf("\t\t\t-------------- variavel --------------------> %s\n\n\n\n", esc_lexema_id);
            esc_quantidade_retorno++;
        }
    }

    // comparamos o nome da funcao (contido no escopo atual) com o identificador.
    // isso eh necessario pois essa funcao AINDA nao consta no escopo pai.
    // se for, pode significar recursividade (tem-se que checar se eh L-Value ou R-Value)
    // se nao for, verificaremos e se for o caso, emitiremos erro.
    if(subfuncao) {
        simbolo* simb = NULL;
        simb = buscar_variavel_nos_escopos(esc_lexema_id);

        if(simb == NULL) {
            if(esc_lado_esquerdo) {

                simb = buscar_funcao_nos_escopos(esc_lexema_id);
                if(simb == NULL) simb = esc_funcoes_declaradas->buscar(esc_lexema_id);

                if(simb == NULL) {
                    erro_incrementar();
                    fprintf(stderr, "ERRO SEMANTICO: VARIAVEL %s NAO DECLARADA NA LINHA %d \n", esc_lexema_id, _variable->linha);
                }
            }
            else {
                simb = buscar_funcao_nos_escopos(esc_lexema_id);
                if(simb == NULL) simb = esc_funcoes_declaradas->buscar(esc_lexema_id);

                if(simb == NULL) {
                    erro_incrementar();
                    fprintf(stderr, "ERRO SEMANTICO: FUNCAO %s NAO DECLARADA NA LINHA %d \n", esc_lexema_id, _variable->linha);
                }
                else {
                    if(((simbolo_funcao*) simb)->quantidade_parametros > 0) {
                        if(((simbolo_funcao*) simb)->tem_retorno) {
                            erro_incrementar();
                            fprintf(stderr, "ERRO SEMANTICO: QUANTIDADE INVALIDA DE PARAMETROS NA CHAMADA DA FUNCAO %s  NA LINHA %d \n", esc_lexema_id,_variable->linha);
                        }
                        else {
                            erro_incrementar();
                            fprintf(stderr, "ERRO SEMANTICO: QUANTIDADE INVALIDA DE PARAMETROS NA CHAMADA DO PROCEDIMENTO %s NA LINHA %d \n", esc_lexema_id,_variable->linha);
                        }
                    }
                }
            }
        }
    }
    else esc_quantidade_retorno++;

    if(_variable->expression != NULL) _variable->expression->accept(this);
    if(_variable->variable != NULL) {
        if(IMPRIMIR_VERIFICACOES_DEBUG) printf("---------> campo de registro!\n");
        simbolo_variavel* svar = buscar_variavel_nos_escopos(_variable->identificator->identificador);
        if(svar != NULL) {

            simbolo_registro* sreg = NULL;
            if(svar->lexema_tipo != NULL) sreg = (simbolo_registro*) buscar_registro_nos_escopos((char*) svar->lexema_tipo);
            if(sreg != NULL) {

                escopo* novo_escopo = sreg->escopo_registro;
                novo_escopo->pai = escopo_atual;
                escopo_atual = novo_escopo;

                _variable->variable->accept(this);

                escopo_atual = escopo_atual->pai;
                return;
            }

            _variable->variable->accept(this);
            return;
        }
        _variable->variable->accept(this);
    }
}

void SemanticoEscopoVisitor::visit(NumberInteger* _numint) {}
void SemanticoEscopoVisitor::visit(NumberReal* _numreal) {}
void SemanticoEscopoVisitor::visit(Boolean* _boolean) {}
void SemanticoEscopoVisitor::visit(Literal* _literal) {}

void SemanticoEscopoVisitor::visit(Call* _call) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("call!\n");
    _call->identificador->accept(this);

    simbolo_funcao* stmp = (simbolo_funcao*) esc_funcoes_declaradas->buscar(_call->identificador->identificador);
    if(esc_funcao_sendo_declarada != NULL && stmp != NULL) {

        esc_quantidade_parametros = 0;
        if(_call->exprlist != NULL) _call->exprlist->accept(this);

        if(stmp->quantidade_parametros != esc_quantidade_parametros) {
            if(stmp->tem_retorno) {
                erro_incrementar();
                fprintf(stderr, "ERRO SEMANTICO: QUANTIDADE INVALIDA DE PARAMETROS NA CHAMADA DA FUNCAO %s NA LINHA %d \n", _call->identificador->identificador, _call->identificador->linha);
            }
            else {
                erro_incrementar();
                fprintf(stderr, "ERRO SEMANTICO: QUANTIDADE INVALIDA DE PARAMETROS NA CHAMADA DO PROCEDIMENTO %s NA LINHA %d \n", _call->identificador->identificador, _call->identificador->linha);
            }
        }
        return;
    }

    simbolo_funcao* simb = buscar_funcao_nos_escopos(esc_lexema_id);
    if(simb == NULL) {
        erro_incrementar();
        fprintf(stderr, "ERRO SEMANTICO: FUNCAO/PROCEDIMENTO %s NAO DECLARADO NA LINHA %d \n", esc_lexema_id, _call->identificador->linha);
    }
    else {

        esc_quantidade_parametros = 0;
        if(_call->exprlist != NULL) _call->exprlist->accept(this);

        if(((simbolo_funcao*) simb)->quantidade_parametros != esc_quantidade_parametros) {
            if(((simbolo_funcao*) simb)->tem_retorno) {
                erro_incrementar();
                fprintf(stderr, "ERRO SEMANTICO: QUANTIDADE INVALIDA DE PARAMETROS NA CHAMADA DA FUNCAO %s NA LINHA %d \n", _call->identificador->identificador, _call->identificador->linha);
            }
            else {
                erro_incrementar();
                fprintf(stderr, "ERRO SEMANTICO: QUANTIDADE INVALIDA DE PARAMETROS NA CHAMADA DO PROCEDIMENTO %s NA LINHA %d \n", _call->identificador->identificador, _call->identificador->linha);
            }
        }

    }
}

void SemanticoEscopoVisitor::visit(RelationOp* _relop) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("rel op!\n");
    _relop->expression1->accept(this);
    _relop->expression2->accept(this);
}

void SemanticoEscopoVisitor::visit(ComparativeOp* _compop) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("comp op!\n");
    _compop->expression1->accept(this);
    _compop->expression2->accept(this);
}

void SemanticoEscopoVisitor::visit(Sign* _sign) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("sign!\n");
    _sign->expression->accept(this);
}

void SemanticoEscopoVisitor::visit(AdditionalOp* _addop) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("add op!\n");
    _addop->expression1->accept(this);
    _addop->expression2->accept(this);
}

void SemanticoEscopoVisitor::visit(MultiplicationOp* _multop) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("mult op!\n");
    _multop->expression1->accept(this);
    _multop->expression2->accept(this);
}

void SemanticoEscopoVisitor::visit(BooleanOp* _boolop) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("bool op!\n");
    _boolop->expression1->accept(this);
    _boolop->expression2->accept(this);
}

void SemanticoEscopoVisitor::visit(Not* _not) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("not!\n");
    _not->expression->accept(this);
}

void SemanticoEscopoVisitor::visit(ExprList* _explist) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("expr list!\n");
    esc_quantidade_parametros++;
    _explist->expression->accept(this);
    if(_explist->explist != NULL) _explist->explist->accept(this);
}

void SemanticoEscopoVisitor::visit(Identificador* _id) {
    esc_tipo_variavel = IDENTIFICADOR;
    esc_tipo_retorno = IDENTIFICADOR;
    esc_lexema_id = (char*) realloc(esc_lexema_id, (strlen(_id->identificador)+1) * sizeof(char));
    strcpy(esc_lexema_id, _id->identificador);
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("[IDENTIFICADOR]-%s\n",esc_lexema_id);
}

void SemanticoEscopoVisitor::visit(Primitive* _primivite) {
    esc_tipo_variavel = _primivite->primitivetype;
    esc_tipo_retorno = _primivite->primitivetype;
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("[PRIMITIVO]-%d\n",_primivite->primitivetype);
}





void SemanticoEscopoVisitor::iniciar_escopo(const char* _nome) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("\n\nINICIANDO ESCOPO [%s]\n", _nome);
    escopo_atual = new escopo(escopo_atual, _nome);
}

escopo* SemanticoEscopoVisitor::finalizar_escopo() {
    escopo* finalizado = escopo_atual;
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("\nFINALIZANDO ESCOPO [%s] ACIMA\n\n", finalizado->nome);
    escopo_atual = escopo_atual->pai;
    return finalizado;
}

simbolo* SemanticoEscopoVisitor::buscar_escopo_atual(char* lexema) {
    simbolo* s;
    s = escopo_atual->tab_variaveis->buscar(lexema);
    if(s != NULL) return (simbolo_variavel*) s;
    s = escopo_atual->tab_registros->buscar(lexema);
    if(s != NULL) return (simbolo_registro*) s;
    s = escopo_atual->tab_funcoes->buscar(lexema);
    if(s != NULL) return (simbolo_funcao*) s;
    return NULL;
}

simbolo_variavel* SemanticoEscopoVisitor::buscar_variavel_nos_escopos(char* lexema) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("\tBUSCANDO EM VARIAVEIS\n");
    simbolo_variavel* var;
    escopo* escopo_busca = escopo_atual;
    do {
        var = (simbolo_variavel*) escopo_busca->tab_variaveis->buscar(lexema);
        if(var != NULL) return var;
        else escopo_busca = escopo_busca->pai;
        if(IMPRIMIR_VERIFICACOES_DEBUG) printf("\tdescendo escopo...\n");
    }
    while(escopo_busca != NULL);
    delete escopo_busca;
    return NULL;
}

simbolo_registro* SemanticoEscopoVisitor::buscar_registro_nos_escopos(char* lexema) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("\tBUSCANDO EM REGISTROS\n");
    simbolo_registro* reg;
    escopo* escopo_busca = escopo_atual;
    do {
        reg = (simbolo_registro*) escopo_busca->tab_registros->buscar(lexema);
        if(reg != NULL) return reg;
        else escopo_busca = escopo_busca->pai;
        if(IMPRIMIR_VERIFICACOES_DEBUG) printf("\tdescendo escopo...\n");
    }
    while(escopo_busca != NULL);
    delete escopo_busca;
    return NULL;
}

simbolo_funcao* SemanticoEscopoVisitor::buscar_funcao_nos_escopos(char* lexema) {
    if(IMPRIMIR_VERIFICACOES_DEBUG) printf("\tBUSCANDO EM FUNCOES\n");
    simbolo_funcao* func;
    escopo* escopo_busca = escopo_atual;
    do {
        func = (simbolo_funcao*) escopo_busca->tab_funcoes->buscar(lexema);
        if(func != NULL) return func;
        else escopo_busca = escopo_busca->pai;
        if(IMPRIMIR_VERIFICACOES_DEBUG) printf("\tdescendo escopo...\n");
    }
    while(escopo_busca != NULL);
    delete escopo_busca;
    return NULL;
}

escopo* SemanticoEscopoVisitor::obter_escopo_program() {
    return escopo_program;
}


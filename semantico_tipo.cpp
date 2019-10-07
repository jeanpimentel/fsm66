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
#include "semantico_tipo.h"

#include "lexico.h"

char* tp_lexema_id;
int tipo = 0;
bool tem_indice_acesso_vetor = false;
int tp_lowerbound = 0;
int tp_upperbound = 0;
bool chamada_de_funcao = false;

void SemanticoTipoVisitor::visit(Program* _program) {
    if(_program->subproglist != NULL) _program->subproglist->accept(this);
    if(_program->statementlist != NULL) _program->statementlist->accept(this);
}

void SemanticoTipoVisitor::visit(SubProgList* _subproglist) {
	if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("subproglist!\n");
    _subproglist->identificador->accept(this);
    if(_subproglist->type != NULL) _subproglist->type->accept(this);
    if(_subproglist->typelist != NULL) _subproglist->typelist->accept(this);
	if(_subproglist->varlist != NULL) _subproglist->varlist->accept(this);

	if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("PARAMETROS DA FUNCAO\n");
	if(_subproglist->arguments != NULL) {
        //tp_parametros = true;
        _subproglist->arguments->accept(this);
        //tp_parametros = false;
    }

    if(_subproglist->subproglist1 != NULL) {
        if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("SUBPROGRAMAS DA FUNCAO\n");
	    _subproglist->subproglist1->accept(this);
    }
	if(_subproglist->statementlist != NULL) {
	    if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("CORPO DA FUNCAO %s\n", _subproglist->identificador->identificador);
        simbolo_funcao* s = NULL;
        s = (simbolo_funcao*) buscar_funcao_nos_escopos(_subproglist->identificador->identificador);
        if(s != NULL) {
            escopo* escopo_antigo = escopo_atual;
            escopo_atual = s->escopo_funcao;
            _subproglist->statementlist->accept(this);
            escopo_atual = escopo_antigo;
        }
	}
    if(_subproglist->subproglist2 != NULL) {
        if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("LISTA DE FUNCOES\n");
        _subproglist->subproglist2->accept(this);
    }
}

void SemanticoTipoVisitor::visit(TypeList* _typelist) {}

void SemanticoTipoVisitor::visit(VarList* _varlist) {}

void SemanticoTipoVisitor::visit(Type* _type) {
	if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("type!\n");
    if(_type->identificador != NULL) _type->identificador->accept(this);
    else if(_type->primitive != NULL) _type->primitive->accept(this);
}

void SemanticoTipoVisitor::visit(If* _if) {
	if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("if!\n");
	tipo = 0;
    _if->expression->accept(this);
    if(tipo != LOGICO) { erro_incrementar(); fprintf(stderr, "ERRO SEMANTICO: EXPRESSAO DE TESTE DO SE NA LINHA %d DEVE SER DO TIPO LOGICO. \n", _if->linha); }
    if(_if->statement1 != NULL) _if->statement1->accept(this);
    if(_if->statement2 != NULL) _if->statement2->accept(this);
}

void SemanticoTipoVisitor::visit(While* _while) {
	if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("while!\n");
    tipo = 0;
    _while->expression->accept(this);
    if(tipo != LOGICO) { erro_incrementar(); fprintf(stderr, "ERRO SEMANTICO: EXPRESSAO DE TESTE DO ENQUANTO NA LINHA %d DEVE SER DO TIPO LOGICO. \n", _while->linha); }
    if(_while->statement != NULL) _while->statement->accept(this);
}

void SemanticoTipoVisitor::visit(Read* _read) {
	if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("read!\n");
	ExprList* _expression = _read->exprlist;
	while(_expression != NULL) {
	    tipo = 0;
        if(typeid(*_expression->expression) == typeid(Variable)) {
            _expression->expression->accept(this);
            if(tipo == INTEIRO || tipo == REAL || tipo == CHAR || tipo == LOGICO) {
                // eh vetor e nao tem indice de acesso?
                if(tp_lowerbound != tp_upperbound && !tem_indice_acesso_vetor) { erro_incrementar(); fprintf(stderr, "ERRO SEMANTICO: ARGUMENTO %s EH VETOR NO LEIA NA LINHA %d.\n", tp_lexema_id, _read->linha); }
            }
            else { erro_incrementar(); fprintf(stderr, "ERRO SEMANTICO: ARGUMENTO %s DE TIPO INVALIDO NO LEIA NA LINHA %d.\n", tp_lexema_id, _read->linha); }
        }
        else { erro_incrementar(); fprintf(stderr, "ERRO SEMANTICO: R-VALUE NO LEIA NA LINHA %d.\n", _read->linha); }
        _expression = _expression->explist;
	}
}

void SemanticoTipoVisitor::visit(Write* _write) {
	if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("write!\n");
    ExprList* _expression = _write->exprlist;
	while(_expression != NULL) {
	    tipo = 0;
	    _expression->expression->accept(this);

	    if(typeid(*_expression->expression) == typeid(Call)) {
	        if(tipo == 0) {
	            erro_incrementar();
                fprintf(stderr, "ERRO SEMANTICO: CHAMADA DE PROCEDIMENTO NO IMPRIMA NA LINHA %d.\n", _write->linha);
                _expression = _expression->explist;
                continue;
	        }
	    }

        if(tipo == INTEIRO || tipo == REAL || tipo == CHAR || tipo == LOGICO || tipo == LITERAL) {
            if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("TEM %s DO TIPO %s\n",tp_lexema_id,lexico_nome_token(tipo));
            // eh vetor e nao tem indice de acesso?
            if(tp_lowerbound != tp_upperbound && !tem_indice_acesso_vetor) { erro_incrementar(); fprintf(stderr, "ERRO SEMANTICO: ARGUMENTO %s EH VETOR NO IMPRIMA NA LINHA %d.\n", tp_lexema_id, _write->linha); }
        }
        else { erro_incrementar(); fprintf(stderr, "ERRO SEMANTICO: ARGUMENTO %s DE TIPO INVALIDO NO IMPRIMA NA LINHA %d.\n", tp_lexema_id, _write->linha); }
        _expression = _expression->explist;
	}
}

void SemanticoTipoVisitor::visit(Assign* _assign) {
	if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("assign!\n");

	/* resetando os valores */
	tipo = 0;
	tp_upperbound = 0;
	tp_lowerbound = 0;
	tem_indice_acesso_vetor = false;

    _assign->variable->accept(this);

    //escopo_atual->imprimir();

    simbolo_funcao* f = (simbolo_funcao*) escopo_atual->tab_funcoes->buscar(tp_lexema_id);
    if(f != NULL) {
        if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("\t\t--------> Achei!\n\n");
        erro_incrementar();
        fprintf(stderr, "ERRO SEMANTICO: ATRIBUICAO INVALIDA A UM R-VALUE NA LINHA %d. \n", _assign->linha);
    }

    int tipo_variavel = tipo;
    int tamanho_vetor_variavel = tp_upperbound - tp_lowerbound + 1;
    bool elemento_vetor_variavel = tem_indice_acesso_vetor;
    char* nome_variavel = (char*) malloc((strlen(tp_lexema_id) + 1) * sizeof(char));
    strcpy(nome_variavel, tp_lexema_id);

    if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("tamanho: %d\n",tamanho_vetor_variavel);
    if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("tem indice: %d\n",tem_indice_acesso_vetor);
    if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("elemento vetor var: %d\n",elemento_vetor_variavel);
    if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("\n\n\n\t\tEXPLORANDO A EXPRESSAO\n\n\n");

    /* resetando os valores */
	tipo = 0;
	tp_upperbound = 0;
	tp_lowerbound = 0;
	tem_indice_acesso_vetor = false;
	chamada_de_funcao = false;

    _assign->expression->accept(this);

    int tipo_expressao = tipo;
    int tamanho_vetor_expressao = tp_upperbound - tp_lowerbound + 1;
    bool elemento_vetor_expressao = tem_indice_acesso_vetor;

    if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("tamanho: %d\n",tamanho_vetor_expressao);
    if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("tem indice: %d\n",tem_indice_acesso_vetor);
    if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("elemento vetor expr: %d\n",elemento_vetor_expressao);

    // se a expressao eh invalida.
    if(tipo == 0) {
        if(chamada_de_funcao) {
            erro_incrementar();
            fprintf(stderr, "ERRO SEMANTICO: ATRIBUICAO INVALIDA NA LINHA %d. PROCEDIMENTO NAO POSSUI RETORNO.\n", _assign->linha);
        }
        else {
            erro_incrementar();
            fprintf(stderr, "ERRO SEMANTICO: ATRIBUICAO INVALIDA NA LINHA %d. \n",_assign->linha);
        }
    }
    // se os tipos sao incompativeis.
    else if(tipo_variavel != tipo_expressao) { erro_incrementar(); fprintf(stderr, "ERRO SEMANTICO: ATRIBUICAO INVALIDA NA LINHA %d.\n", _assign->linha); }
    // se sao vetores, testar o tipo de atribuicao. vetor <- vetor ou elemento <- elemento.
    else if(tamanho_vetor_variavel > 1 && tamanho_vetor_expressao > 1) {
        if(elemento_vetor_variavel == true && elemento_vetor_expressao == false) { erro_incrementar(); fprintf(stderr, "ERRO SEMANTICO: ATRIBUICAO DE UM VETOR A UMA POSICAO DE VETOR NA LINHA %d.\n",_assign->linha); }
        else if(elemento_vetor_variavel == false && elemento_vetor_expressao == true) { erro_incrementar(); fprintf(stderr, "ERRO SEMANTICO: ATRIBUICAO DE UMA POSICAO DE VETOR A UM VETOR NA LINHA %d.\n",_assign->linha); }
        // se a variavel eh vetor e menor q o vetor da expressao.
        if(tamanho_vetor_variavel < tamanho_vetor_expressao) {
            erro_incrementar();
            fprintf(stderr, "ERRO SEMANTICO: ATRIBUICAO INVALIDA NA LINHA %d. VETOR %s EH MAIOR QUE O VETOR %s.\n", _assign->linha, nome_variavel, tp_lexema_id);
        }
    }
    // se somente variavel eh vetor, testar se ha indice de acesso.
    else if(tamanho_vetor_variavel > 1 && elemento_vetor_variavel == false && tamanho_vetor_expressao == 1) {
        erro_incrementar();
        fprintf(stderr, "ERRO SEMANTICO: ATRIBUICAO DE UM ELEMENTO A UM VETOR NA LINHA %d.\n",_assign->linha);
    }
}

void SemanticoTipoVisitor::visit(StatementList* _stmtlist) {
	if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("stmtlist!\n");
    _stmtlist->statement->accept(this);
    if(_stmtlist->statementlist != NULL) _stmtlist->statementlist->accept(this);
}

void SemanticoTipoVisitor::visit(CompoundStatement* _compoundstmt) {
	if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("compoundstmt!\n");
    if(_compoundstmt->statementlist != NULL) _compoundstmt->statementlist->accept(this);
}

void SemanticoTipoVisitor::visit(Variable* _variable) {
	if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("variable!\n");
    _variable->identificator->accept(this);

    simbolo_funcao* func = buscar_funcao_nos_escopos(tp_lexema_id);
    if(func != NULL) {
        if(func->tem_retorno) tipo = func->tipo_retorno;
        else {
            tipo = 0;
            chamada_de_funcao = true;
        }

        tp_lowerbound = 0;
        tp_upperbound = 0;
        return;
    }

    simbolo_variavel* var = buscar_variavel_nos_escopos(tp_lexema_id);
    if(var != NULL) tipo = var->tipo;
    else {
        tipo = 0;
        return;
    }

    if(_variable->expression != NULL) {
        if(var->lowerbound == var->upperbound) { erro_incrementar(); fprintf(stderr, "ERRO SEMANTICO: VARIAVEL %s NA LINHA %d NAO EH UM VETOR.\n", tp_lexema_id, _variable->linha); }
        else {

            _variable->expression->accept(this);
            if(tipo != INTEIRO) {
                erro_incrementar();
                fprintf(stderr, "ERRO SEMANTICO: INDICE DO VETOR NA LINHA %d NAO EH INTEIRO. \n", _variable->linha);
                tipo = var->tipo;
            }

            if(tipo == INTEIRO && typeid(*_variable->expression) == typeid(NumberInteger)) {
                int indice = atoi(((NumberInteger*) (_variable->expression))->numberinteger);
                if(indice < var->lowerbound || indice > var->upperbound) { erro_incrementar(); printf("ERRO SEMANTICO: INDICE DO VETOR NA LINHA %d FORA DOS LIMITES.\n", _variable->linha); }
            }

        }
        tem_indice_acesso_vetor = true;
    }else{
        tem_indice_acesso_vetor = false;
    }

    if(var != NULL) {
        tipo = var->tipo;
        tp_lowerbound = var->lowerbound;
        tp_upperbound = var->upperbound;
        if(tp_lowerbound > tp_upperbound) { erro_incrementar(); fprintf(stderr, "ERRO SEMANTICO: LIMITE INFERIOR EH MAIOR QUE O LIMITE SUPERIOR DO VETOR NA LINHA %d.\n", _variable->linha); }
    }
    else {
        tipo = 0;
        tp_lowerbound = 0;
        tp_upperbound = 0;
    }

    if(_variable->variable != NULL) {

        if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("---------> campo de registro!\n\n\n");

        simbolo_registro* reg = NULL;
        if(var->lexema_tipo != NULL) reg = (simbolo_registro*) buscar_registro_nos_escopos((char*) var->lexema_tipo);
        if(reg != NULL) {

            escopo* escopo_antigo = escopo_atual;
            escopo_atual = reg->escopo_registro;

            _variable->variable->accept(this);

            escopo_atual = escopo_antigo;
        }
        else tipo = 0;
    }
}

void SemanticoTipoVisitor::visit(NumberInteger* _numint) {
	if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("numint!\n");
	tipo = INTEIRO;
}

void SemanticoTipoVisitor::visit(NumberReal* _numreal) {
	if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("numreal!\n");
	tipo = REAL;
}

void SemanticoTipoVisitor::visit(Boolean* _boolean) {
	if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("boolean!\n");
	tipo = LOGICO;
}

void SemanticoTipoVisitor::visit(Literal* _literal) {
	if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("literal!\n");
	tipo = LITERAL;
}

void SemanticoTipoVisitor::visit(Call* _call) {
	if(IMPRIMIR_VERIFICACOES_DEBUG2) printf("call!\n");
    _call->identificador->accept(this);

    int temp_tipo = 0;

    simbolo_funcao* s = NULL;
    s = (simbolo_funcao*) buscar_funcao_nos_escopos(tp_lexema_id);
    if(s != NULL) {
        //s->imprimir();
        if(s->tem_retorno) {
            temp_tipo = s->tipo_retorno;
        //    printf("isso eh funcao\n\n");
        }// parei aqui
        else {
            chamada_de_funcao = true;
            //    printf("isso eh procedimento\n\n");
        }


        if (_call->exprlist != NULL) {
            // Verificar os tipos e os tamnhos de cada argumento
            ExprList* exp = _call->exprlist;

            argumento* param = s->parametros;
            int nParametro = 1;
            int qtddParametros = 0;
            while (exp != NULL) {
                exp->expression->accept(this);
                // Escapando erro quando o n�mero de par�metros n�o corresponde
                // Erro j� exibido em semantico_escopo.
                if (param == NULL) {
                    exp = exp->explist;
                    qtddParametros++;
                    continue;
                }

                if (tipo != param->var->tipo) {
                    erro_incrementar();
                    fprintf(stderr, "ERRO SEMANTICO: %s ESPERADO E %s ENCONTRADO NO PARAMETRO %d EM %s NA LINHA %d. \n", lexico_nome_token(param->var->tipo), lexico_nome_token(tipo), nParametro, _call->identificador->identificador, _call->linha);
                }


                if (param->var->upperbound - param->var->lowerbound != 0) {
                    if (tp_upperbound - tp_lowerbound == 0 || tem_indice_acesso_vetor) {
                        erro_incrementar();
                        fprintf(stderr, "ERRO SEMANTICO: VETOR[%d:%d] %s ESPERADO E  %s RECEBIDO NA LINHA %d.\n", param->var->lowerbound, param->var->upperbound, lexico_nome_token(param->var->tipo), lexico_nome_token(tipo), _call->linha);
                    } else if (tp_upperbound - tp_lowerbound != param->var->upperbound - param->var->lowerbound) {
                        erro_incrementar();
                        fprintf(stderr, "ERRO SEMANTICO: O VETOR RECEBIDO NO PARAMETRO %d EM %s POSSUI TAMANHO DIFERENTE DO ESPERADO NA LINHA %d.\n", nParametro, _call->identificador->identificador, _call->linha);
                    }

                } else {
                    if (tp_upperbound - tp_lowerbound != 0 && !tem_indice_acesso_vetor) {
                        erro_incrementar();
                        fprintf(stderr, "ERRO SEMANTICO:  %s ESPERADO E  VETOR[%d:%d] %s RECEBIDO NA LINHA %d.\n", lexico_nome_token(param->var->tipo), tp_lowerbound, tp_upperbound, lexico_nome_token(tipo), _call->linha);
                    }
                }

                // Obtendo o pr�ximo statementlist incrementando contatdor de par�metros

                nParametro++;
                param = param->prox;
                exp = exp->explist;
                qtddParametros++;

            }
            if (qtddParametros != s->quantidade_parametros) {
                erro_incrementar();
                fprintf(stderr, "ERRO SEMANTICO: %d PARAMETROS ESPERADOS E %d ENCONTRADOS NA CHAMADA DE %s NA LINHA %d.\n", s->quantidade_parametros, qtddParametros, _call->identificador->identificador, _call->linha);
            }


        }

        tipo = temp_tipo;
    }
    tp_upperbound = 0;
    tp_lowerbound = 0;

}

void SemanticoTipoVisitor::visit(RelationOp* _relop) { // OK // OK
    if (IMPRIMIR_VERIFICACOES_DEBUG2) printf("rel op!\n");

    /* resetando os valores */
    tipo = 0;
    tp_upperbound = 0;
    tp_lowerbound = 0;
    tem_indice_acesso_vetor = false;

    _relop->expression1->accept(this);
    int tipo1 = tipo;
    bool vetor1_sem_indice = tp_lowerbound != tp_upperbound && !tem_indice_acesso_vetor;

    /* resetando os valores */
    tipo = 0;
    tp_upperbound = 0;
    tp_lowerbound = 0;
    tem_indice_acesso_vetor = false;

    _relop->expression2->accept(this);
    int tipo2 = tipo;
    bool vetor2_sem_indice = tp_lowerbound != tp_upperbound && !tem_indice_acesso_vetor;

    tipo = LOGICO;
    if (vetor1_sem_indice || vetor2_sem_indice) {
        erro_incrementar();
        fprintf(stderr, "ERRO SEMANTICO: USO INDEVIDO DE VETORES NA LINHA %d. \n", _relop->linha);
        tipo = 0;
    } else if (tipo1 != tipo2 || (tipo1 == tipo2 && (tipo1 != INTEIRO && tipo1 != REAL))) {
        erro_incrementar();
        fprintf(stderr, "ERRO SEMANTICO: EXPRESSAO COM TIPOS INCOMPATIVEIS NA LINHA %d. \n", _relop->linha);
        tipo = 0;
    }
}

void SemanticoTipoVisitor::visit(ComparativeOp* _compop) { // OK // OK
    if (IMPRIMIR_VERIFICACOES_DEBUG2) printf("comp op!\n");

    /* resetando os valores */
    tipo = 0;
    tp_upperbound = 0;
    tp_lowerbound = 0;
    tem_indice_acesso_vetor = false;

    _compop->expression1->accept(this);
    int tipo1 = tipo;
    bool vetor1_sem_indice = tp_lowerbound != tp_upperbound && !tem_indice_acesso_vetor;

    /* resetando os valores */
    tipo = 0;
    tp_upperbound = 0;
    tp_lowerbound = 0;
    tem_indice_acesso_vetor = false;

    _compop->expression2->accept(this);
    int tipo2 = tipo;
    bool vetor2_sem_indice = tp_lowerbound != tp_upperbound && !tem_indice_acesso_vetor;

    tipo = LOGICO;
    if (vetor1_sem_indice || vetor2_sem_indice) {
        erro_incrementar();
        fprintf(stderr, "ERRO SEMANTICO: USO INDEVIDO DE VETORES NA LINHA %d. \n", _compop->linha);
        tipo = 0;
    } else if (tipo1 != tipo2) {
        erro_incrementar();
        fprintf(stderr, "ERRO SEMANTICO: EXPRESSAO COM TIPOS INCOMPATIVEIS NA LINHA %d. \n", _compop->linha);
        tipo = 0;
    }
}

void SemanticoTipoVisitor::visit(Sign* _sign) { // OK // OK
    if (IMPRIMIR_VERIFICACOES_DEBUG2) printf("sign!\n");

    /* resetando os valores */
    tipo = 0;
    tp_upperbound = 0;
    tp_lowerbound = 0;
    tem_indice_acesso_vetor = false;

    _sign->expression->accept(this);
    bool vetor1_sem_indice = tp_lowerbound != tp_upperbound && !tem_indice_acesso_vetor;

    if (vetor1_sem_indice) {
        erro_incrementar();
        fprintf(stderr, "ERRO SEMANTICO: USO INDEVIDO DE VETORES NA LINHA %d. \n", _sign->linha);
        tipo = 0;
    } else if (tipo != INTEIRO && tipo != REAL) {
        erro_incrementar();
        fprintf(stderr, "ERRO SEMANTICO: EXPRESSAO COM TIPOS INCOMPATIVEIS NA LINHA %d. \n", _sign->linha);
        tipo = 0;
    }
}

void SemanticoTipoVisitor::visit(AdditionalOp* _addop) { // OK // OK
    if (IMPRIMIR_VERIFICACOES_DEBUG2) printf("add op!\n");

    /* resetando os valores */
    tipo = 0;
    tp_upperbound = 0;
    tp_lowerbound = 0;
    tem_indice_acesso_vetor = false;

    _addop->expression1->accept(this);
    int tipo1 = tipo;
    bool vetor1_sem_indice = tp_lowerbound != tp_upperbound && !tem_indice_acesso_vetor;

    /* resetando os valores */
    tipo = 0;
    tp_upperbound = 0;
    tp_lowerbound = 0;
    tem_indice_acesso_vetor = false;

    _addop->expression2->accept(this);
    int tipo2 = tipo;
    bool vetor2_sem_indice = tp_lowerbound != tp_upperbound && !tem_indice_acesso_vetor;

    if (vetor1_sem_indice || vetor2_sem_indice) {
        erro_incrementar();
        fprintf(stderr, "ERRO SEMANTICO: USO INDEVIDO DE VETORES NA LINHA %d. \n", _addop->linha);
        tipo = 0;
    } else if (tipo1 != tipo2 || (tipo1 == tipo2 && (tipo1 != INTEIRO && tipo1 != REAL))) {
        erro_incrementar();
        fprintf(stderr, "ERRO SEMANTICO: EXPRESSAO COM TIPOS INCOMPATIVEIS NA LINHA %d. \n", _addop->linha);
        tipo = 0;
    }
}

void SemanticoTipoVisitor::visit(MultiplicationOp* _multop) { // OK // OK
    if (IMPRIMIR_VERIFICACOES_DEBUG2) printf("mult op!\n");

    /* resetando os valores */
    tipo = 0;
    tp_upperbound = 0;
    tp_lowerbound = 0;
    tem_indice_acesso_vetor = false;

    _multop->expression1->accept(this);
    int tipo1 = tipo;
    bool vetor1_sem_indice = tp_lowerbound != tp_upperbound && !tem_indice_acesso_vetor;

    /* resetando os valores */
    tipo = 0;
    tp_upperbound = 0;
    tp_lowerbound = 0;
    tem_indice_acesso_vetor = false;

    _multop->expression2->accept(this);
    int tipo2 = tipo;
    bool vetor2_sem_indice = tp_lowerbound != tp_upperbound && !tem_indice_acesso_vetor;

    tipo = 0;
    if (vetor1_sem_indice || vetor2_sem_indice) {
        erro_incrementar();
        fprintf(stderr, "ERRO SEMANTICO: USO INDEVIDO DE VETORES NA LINHA %d. \n", _multop->linha);
        tipo = 0;
    } else if (_multop->operation == MULTIPLICACAO) {
        if (tipo1 != tipo2 || (tipo1 == tipo2 && (tipo1 != INTEIRO && tipo1 != REAL))) {
            erro_incrementar();
            fprintf(stderr, "ERRO SEMANTICO: EXPRESSAO COM TIPOS INCOMPATIVEIS NA LINHA %d. \n", _multop->linha);
            tipo = 0;
        } else tipo = tipo1;
    } else if (_multop->operation == DIVISAO) {
        if (tipo1 != tipo2 || (tipo1 == tipo2 && (tipo1 != INTEIRO && tipo1 != REAL))) {
            erro_incrementar();
            fprintf(stderr, "ERRO SEMANTICO: EXPRESSAO COM TIPOS INCOMPATIVEIS NA LINHA %d. \n", _multop->linha);
            tipo = 0;
        } else tipo = REAL;
    } else if (_multop->operation == DIV) {
        if (tipo1 != tipo2 || (tipo1 == tipo2 && (tipo1 != INTEIRO && tipo1 != REAL))) {
            erro_incrementar();
            fprintf(stderr, "ERRO SEMANTICO: EXPRESSAO COM TIPOS INCOMPATIVEIS NA LINHA %d. \n", _multop->linha);
            tipo = 0;
        } else tipo = INTEIRO;
    } else if (_multop->operation == RESTO) {
        if (tipo1 != tipo2 || (tipo1 == tipo2 && (tipo1 != INTEIRO && tipo1 != REAL))) {
            erro_incrementar();
            fprintf(stderr, "ERRO SEMANTICO: EXPRESSAO COM TIPOS INCOMPATIVEIS NA LINHA %d. \n", _multop->linha);
            tipo = 0;
        } else tipo = tipo1;
    }
}

void SemanticoTipoVisitor::visit(BooleanOp* _boolop) { // OK // OK
    if (IMPRIMIR_VERIFICACOES_DEBUG2) printf("bool op!\n");

    /* resetando os valores */
    tipo = 0;
    tp_upperbound = 0;
    tp_lowerbound = 0;
    tem_indice_acesso_vetor = false;

    _boolop->expression1->accept(this);
    int tipo1 = tipo;
    bool vetor1_sem_indice = tp_lowerbound != tp_upperbound && !tem_indice_acesso_vetor;

    /* resetando os valores */
    tipo = 0;
    tp_upperbound = 0;
    tp_lowerbound = 0;
    tem_indice_acesso_vetor = false;

    _boolop->expression2->accept(this);
    int tipo2 = tipo;
    bool vetor2_sem_indice = tp_lowerbound != tp_upperbound && !tem_indice_acesso_vetor;

    if (vetor1_sem_indice || vetor2_sem_indice) {
        erro_incrementar();
        fprintf(stderr, "ERRO SEMANTICO: USO INDEVIDO DE VETORES NA LINHA %d. \n", _boolop->linha);
        tipo = 0;
    } else if (tipo1 != tipo2 || (tipo1 == tipo2 && tipo1 != LOGICO)) {
        erro_incrementar();
        fprintf(stderr, "ERRO SEMANTICO: EXPRESSAO COM TIPOS INCOMPATIVEIS NA LINHA %d. \n", _boolop->linha);
        tipo = 0;
    }
}

void SemanticoTipoVisitor::visit(Not* _not) { // OK // OK
    if (IMPRIMIR_VERIFICACOES_DEBUG2) printf("not!\n");

    /* resetando os valores */
    tipo = 0;
    tp_upperbound = 0;
    tp_lowerbound = 0;
    tem_indice_acesso_vetor = false;

    _not->expression->accept(this);

    bool vetor1_sem_indice = tp_lowerbound != tp_upperbound && !tem_indice_acesso_vetor;
    if (vetor1_sem_indice) {
        erro_incrementar();
        fprintf(stderr, "ERRO SEMANTICO: USO INDEVIDO DE VETORES NA LINHA %d. \n", _not->linha);
        tipo = 0;
    } else if (tipo != LOGICO) {
        erro_incrementar();
        fprintf(stderr, "ERRO SEMANTICO: EXPRESSAO COM TIPOS INCOMPATIVEIS NA LINHA %d. \n", _not->linha);
        tipo = 0;
    }
}

void SemanticoTipoVisitor::visit(ExprList* _explist) {
    if (IMPRIMIR_VERIFICACOES_DEBUG2) printf("expr list!\n");
    _explist->expression->accept(this);
    if (_explist->explist != NULL) _explist->explist->accept(this);
}

void SemanticoTipoVisitor::visit(Identificador* _id) {
    tp_lexema_id = (char*) realloc(tp_lexema_id, (strlen(_id->identificador) + 1) * sizeof (char));
    strcpy(tp_lexema_id, _id->identificador);
    if (IMPRIMIR_VERIFICACOES_DEBUG2) printf("[IDENTIFICADOR]-%s\n", _id->identificador);
}

void SemanticoTipoVisitor::visit(Primitive* _primivite) {
    if (IMPRIMIR_VERIFICACOES_DEBUG2) printf("[PRIMITIVO]-%d\n", _primivite->primitivetype);
}

void SemanticoTipoVisitor::iniciar_escopo(const char* _nome) {
    if (IMPRIMIR_VERIFICACOES_DEBUG2) printf("\n\nINICIANDO ESCOPO [%s]\n", _nome);
    escopo_atual = new escopo(escopo_atual, _nome);
}

escopo* SemanticoTipoVisitor::finalizar_escopo() {
    escopo* finalizado = escopo_atual;
    if (IMPRIMIR_VERIFICACOES_DEBUG2) printf("\nFINALIZANDO ESCOPO [%s] ACIMA\n\n", finalizado->nome);
    escopo_atual = escopo_atual->pai;
    return finalizado;
}

simbolo* SemanticoTipoVisitor::buscar_escopo_atual(char* lexema) {
    simbolo* s;
    s = escopo_atual->tab_variaveis->buscar(lexema);
    if (s != NULL) return (simbolo_variavel*) s;
    s = escopo_atual->tab_registros->buscar(lexema);
    if (s != NULL) return (simbolo_registro*) s;
    s = escopo_atual->tab_funcoes->buscar(lexema);
    if (s != NULL) return (simbolo_funcao*) s;
    return NULL;
}

simbolo_variavel* SemanticoTipoVisitor::buscar_variavel_nos_escopos(char* lexema) {
    if (IMPRIMIR_VERIFICACOES_DEBUG2) printf("\tBUSCANDO %s EM VARIAVEIS\n", lexema);
    simbolo_variavel* var;
    escopo* escopo_busca = escopo_atual;
    do {
        var = (simbolo_variavel*) escopo_busca->tab_variaveis->buscar(lexema);
        if (var != NULL) return var;
        else escopo_busca = escopo_busca->pai;
        if (IMPRIMIR_VERIFICACOES_DEBUG2) printf("\tdescendo escopo...\n");
    } while (escopo_busca != NULL);
    delete escopo_busca;
    return NULL;
}

simbolo_registro* SemanticoTipoVisitor::buscar_registro_nos_escopos(char* lexema) {
    if (IMPRIMIR_VERIFICACOES_DEBUG2) printf("\tBUSCANDO EM REGISTROS\n");
    simbolo_registro* reg;
    escopo* escopo_busca = escopo_atual;
    do {
        reg = (simbolo_registro*) escopo_busca->tab_registros->buscar(lexema);
        if (reg != NULL) return reg;
        else escopo_busca = escopo_busca->pai;
        if (IMPRIMIR_VERIFICACOES_DEBUG2) printf("\tdescendo escopo...\n");
    } while (escopo_busca != NULL);
    delete escopo_busca;
    return NULL;
}

simbolo_funcao* SemanticoTipoVisitor::buscar_funcao_nos_escopos(char* lexema) {
    if (IMPRIMIR_VERIFICACOES_DEBUG2) printf("\tBUSCANDO EM FUNCOES\n");
    simbolo_funcao* func;
    escopo* escopo_busca = escopo_atual;
    do {
        func = (simbolo_funcao*) escopo_busca->tab_funcoes->buscar(lexema);
        if (func != NULL) return func;
        else escopo_busca = escopo_busca->pai;
        if (IMPRIMIR_VERIFICACOES_DEBUG2) printf("\tdescendo escopo...\n");
    } while (escopo_busca != NULL);
    delete escopo_busca;
    return NULL;
}

escopo* SemanticoTipoVisitor::obter_escopo_program() {
    return escopo_program;
}

void SemanticoTipoVisitor::definir_escopo_program(escopo* _escopo) {
    escopo_atual = _escopo;
}


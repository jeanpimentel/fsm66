#include "gerador_ri.h"

VisitorGeradorRI::VisitorGeradorRI(escopo* e) {
    registradores = new TempList(new Temp((char *) "$t0"),
            new TempList(new Temp((char *) "$t1"),
            new TempList(new Temp((char *) "$t2"),
            new TempList(new Temp((char *) "$t3"),
            new TempList(new Temp((char *) "$t4"),
            new TempList(new Temp((char *) "$t5"),
            new TempList(new Temp((char *) "$t6"),
            new TempList(new Temp((char *) "$t7"),
            new TempList(new Temp((char *) "$t8"),
            new TempList(new Temp((char *) "$t9"),
            NULL))))))))));

    this->_escopo_base = e;
    this->_escopo_atual = e;
    this->_fragment = NULL;
    this->_body = NULL;
    this->_stmt = NULL;
    this->_exp = NULL;
    this->_explist = NULL;

}

VisitorGeradorRI::~VisitorGeradorRI() {
    delete registradores;
}

void VisitorGeradorRI::devolveRegistrador(Temp *temp) {
    Temp *reg = new Temp(temp->temp);
    registradores = new TempList(reg, registradores);
}

Temp *VisitorGeradorRI::obterRegistrador() {
    if (registradores == NULL) { exit(-2); }
    Temp* temp = registradores->temp;
    TempList *descartado = registradores;
    registradores = registradores->proximo;
    descartado->temp = NULL;
    descartado->proximo = NULL;
    delete descartado;
    return temp;
}

Fragment* VisitorGeradorRI::obter_ultimo_subprog() {

    Fragment* topo = this->raizri;
    if (topo == NULL) {
        return NULL;
    }
    while (topo->proximo != NULL) {
        topo = topo->proximo;
    }
    return topo;

}

void VisitorGeradorRI::visit(Program* program) {
    // Verificando vari�veis n�o locais
    Procedure* _main = new Procedure(new FrameMIPS(), NULL);
    _main->frame->rotulo = new Label((char*) "_inicio_");

    this->nlocal = 0;
    this->_body = NULL;
    this->_stmt = NULL;
    if (program->statementlist != NULL) program->statementlist->accept(this);
    // Trazendo corpo:
    _main->body = this->_body;
    this->nlocal = 1;


    if (program->varlist != NULL) program->varlist->accept(this);

    this->raizri = this->_fragment;
    this->_fragment = NULL;

    if (program->subproglist != NULL) program->subproglist->accept(this);

    Fragment* f = this->raizri;
    while(f->proximo != NULL) {
        f = f->proximo;
    }
    f->proximo = this->_fragment;
    this->_fragment = NULL;

    _main->label = new Label((char*)"_inicio_");
    _main->proximo = this->raizri;
    this->raizri = _main;
    return;

    if (program->typelist != NULL) program->typelist->accept(this);

}

void VisitorGeradorRI::visit(VarList* varlist) {
    if (this->nlocal == 1) {
        Fragment* nextone = NULL;
        // Estamos no escopo global, vamos pegar cada vari�vel e guardar em um Fragment apropriado
        while (varlist != NULL) {
            Identificador* var = varlist->identificador;
            // Buscando a vari�vel nos escopos, para obter mais informa��es
            simbolo_variavel *svar = (simbolo_variavel*) _escopo_base->tab_variaveis->buscar(var->identificador);

            if (svar == NULL) {
                // Erro @jean!
                exit(-1);
            }
            if (svar->tipo == INTEIRO) {
                nextone = new Integer(svar->upperbound - svar->lowerbound);
            } else if (svar->tipo == REAL) {
                nextone = new Float(svar->upperbound - svar->lowerbound);
            } else if (svar->tipo == LOGICO) {
                nextone = new BooleanRI(svar->upperbound - svar->lowerbound);
            } else {
                // boolean, registro...
                exit(-1);
            }
            // Mais tarde vamos precisar desta informacao
            char* nomelabel = (char*) malloc(sizeof (char) *(strlen(var->identificador) + strlen((char*) "global_") + 1));
            sprintf(nomelabel, "global_%s", var->identificador);
            nextone->label = new Label(nomelabel);

            varlist = varlist->varlist;

            if (_fragment != NULL) {
                nextone->proximo = _fragment;
            } else {
                nextone->proximo = NULL;
            }

            _fragment = nextone;
            nextone = NULL;
        }
    } else {
        if (varlist->identificador != NULL) varlist->identificador->accept(this);
        /* Verifica se ha tipo na variavel, pois a mesma pode ser argumento. */
        if (varlist->type != NULL) varlist->type->accept(this);
        /* Verifica se ha uma proxima varlist para visitar. */
        if (varlist->varlist != NULL) varlist->varlist->accept(this);
    }

}

void VisitorGeradorRI::visit(StatementList* statementlist) {

    // Os ultimos serao os primeiros
    if (statementlist->statementlist != NULL) statementlist->statementlist->accept(this);

    // Obtendo a proxima instrucao.
    this->_stmt = NULL;
    statementlist->statement->accept(this);
    if (this->_stmt == NULL) {
        return;
    }

    if(dynamic_cast<CompoundStatement*>(statementlist->statement) != NULL) {
        return;
    }
    this->_body = new StmtList(this->_stmt, this->_body);





}

void VisitorGeradorRI::visit(Assign* assign) {

    this->_exp = NULL;
    // Para onde.
    assign->variable->accept(this);
    Exp* exp1 = this->_exp;

    // Calcula a expressao anterior e vamos mover pra um registrado, o que estiver disponivel
    assign->expression->accept(this);
    Exp* exp2 = this->_exp;
    //TEMP *temp = new TEMP(this->obterRegistrador());

    if (this->_exp == NULL) {
        exit(56);
    }

    this->_stmt = new MOVE(exp1, exp2);

}

void VisitorGeradorRI::visit(AdditionalOp* additionalop) {
    Exp* exp1;
    Exp* exp2;
    this->_exp = NULL;
    additionalop->expression1->accept(this);
    exp1 = this->_exp;

    this->_exp = NULL;
    additionalop->expression2->accept(this);
    exp2 = this->_exp;

    this->_exp = new BINOP(additionalop->operation, exp1, exp2);
}

void VisitorGeradorRI::visit(Call* call) {

    char* nomelabel = (char*) malloc(sizeof (char) *(strlen(call->identificador->identificador) + strlen((char*) "subprog_") + 1));
    sprintf(nomelabel, "subprog_%s", call->identificador->identificador);
    Label* l = new Label(nomelabel);

    ExpList* parametros = this->_explist;
    this->_explist = NULL;


    if (call->exprlist != NULL) {
        this->_explist = NULL;
        call->exprlist->accept(this);

        Temp* fp = new Temp();
        fp->travado = REG_FP;
        // funções
        this->_exp = new CALL(
                new NAME(l),
                new ExpList(new TEMP(new Temp((char*) "$SL")), this->_explist)

                );
        // procedimentos
        this->_stmt = new EXP(this->_exp);
    }

   this->_explist =  parametros;

}

void VisitorGeradorRI::visit(SubProgList* subproglist) {
    int i = 0;
    simbolo_funcao* sf = (simbolo_funcao*) _escopo_atual->buscar_funcao(subproglist->identificador->identificador, &i);
    if (sf == NULL) {
        printf("ERRO - Função não definida em escopo base - %s", subproglist->identificador->identificador);
        exit(-1);
    }
    _escopo_atual = sf->escopo_funcao;

    ExpList* parametros = this->_explist;
    this->_explist = NULL; //  Limpando pra chamada de funçoes dentro de funções.

    //if (subproglist->typelist != NULL) subproglist->typelist->accept(this);
    subproglist->identificador->accept(this);
    //if (subproglist->type != NULL) subproglist->type->accept(this);



    // Inicializando offset
    this->_offset = 0;

    // Adicionando os argumentos no frame
    VarList* args = subproglist->arguments;
    LocalList* vars = NULL;

    args = subproglist->varlist;
    while (args != NULL) {
        i = 0;
        simbolo_variavel* svar = (simbolo_variavel*) this->_escopo_atual->buscar_variavel(args->identificador->identificador, &i);
        // incrementando offset
        this->_offset += TAMANHO_PALAVRA;
        svar->offset = this->_offset;
        //
        if (vars == NULL) {
            vars = new LocalList(new atFrame(svar->offset), NULL);
        } else {
            while (vars->prox != NULL) {
                vars = vars->prox;
            }
            vars->prox = new LocalList(new atFrame(svar->offset), NULL);
        }
        args = args->varlist;
    }


    // Criando frame
    char* nomelabel = (char*) malloc(sizeof (char) *(strlen(subproglist->identificador->identificador) + strlen((char*) "subprog_") + 1));
    sprintf(nomelabel, "subprog_%s", subproglist->identificador->identificador);
    FrameMIPS* frame = new FrameMIPS(new Label(nomelabel), new Temp(), vars);

    _frame = frame;

    // Carregando instruções dentro do frame
    this->_body = NULL;
    this->_stmt = NULL;
    if (subproglist->statementlist != NULL) subproglist->statementlist->accept(this);


    // Criando Procedure
    Procedure* subprog = new Procedure(frame, NULL);
    subprog->label = new Label(nomelabel);
    subprog->body = this->_body;

    // Empilhando subprog
    if (this->raizri != NULL) {
        subprog->proximo = this->raizri;
    }
    this->raizri = subprog;


    // percorrer em profundidade, os últimos serão os primeiros.
    // primeiros os subprogramas deste programa
    if (subproglist->subproglist1 != NULL) subproglist->subproglist1->accept(this);
    // Depois os programas que o seguem no mesmo nível
    if (subproglist->subproglist2 != NULL) subproglist->subproglist2->accept(this);

    this->_explist = parametros;
}

void VisitorGeradorRI::visit(TypeList* typelist) {
    typelist->identificador->accept(this);
    if (typelist->varlist != NULL) {
        typelist->varlist->accept(this);
    }
    if (typelist->typelist != NULL) {
        typelist->typelist->accept(this);
    }
}

void VisitorGeradorRI::visit(Type* type) {

    /* Ha dois tipos de Type, identificador ou tipo primitivo. Verificando qual o caso. */
    if (type->identificador != NULL) {
        type->identificador->accept(this);
    } else if (type->primitive != NULL) {
        type->primitive->accept(this);
    }
    if (type->lowerbound->numberinteger != type->upperbound->numberinteger) {
        type->lowerbound->accept(this);
        type->upperbound->accept(this);
    }

}

// ok

void VisitorGeradorRI::visit(If* iff) {

    StmtList* corpo_fechado = this->_body;
    this->_body = NULL;

    //iff->expression->accept(this);
    //if (iff->statement1 != NULL) iff->statement1->accept(this);
    //if (iff->statement2 != NULL) iff->statement2->accept(this);

    Exp* exp1;
    Stmt* stmt1;
    Stmt* stmt2;

    Label* label_senao = new Label(SENAO);
    Label* label_entao = new Label(ENTAO);
    Label* label_fimse = new Label(FIMSE);

    if ((iff->expression) && (iff->statement1)) {

        iff->expression->accept(this);
        exp1 = this->_exp;

        if (iff->statement1 != NULL) iff->statement1->accept(this);
        stmt1 = this->_stmt;

        if (iff->statement2) {

            this->_body = NULL;
            if (iff->statement2 != NULL) iff->statement2->accept(this);
            stmt2 = this->_stmt;

            this->_stmt = new SEQ(
                                new CJUMP(DIFERENTE, exp1, new CONST(0), label_entao, label_senao),
                                new SEQ(
                                    new LABEL(label_entao),
                                    new SEQ(
                                        new SEQ(
                                            stmt1,
                                            new JUMP(new NAME(label_fimse))
                                        ),
                                        new SEQ(
                                            new LABEL(label_senao),
                                            new SEQ(
                                                stmt2,
                                                new LABEL(label_fimse)
                                            )
                                        )
                                    )
                                )
                            );

        } else {

            this->_stmt = new SEQ(
                    new CJUMP(
                    DIFERENTE,
                    exp1,
                    new CONST(0),
                    label_entao,
                    label_fimse),
                    new SEQ(
                    new LABEL(label_entao),
                    new SEQ(
                    stmt1,
                    new LABEL(label_fimse)
                    )
                    )
                    );

        }
    }

    this->_body = corpo_fechado;

}

// ok

void VisitorGeradorRI::visit(While* whilee) {

    StmtList* corpo_fechado = this->_body;
    this->_body = NULL;

    //whilee->expression->accept(this);
    //if (whilee->statement != NULL)whilee->statement->accept(this);

    Exp* exp;
    Stmt* stmt;

    if ((whilee->expression) && (whilee->statement)) {

        Label *teste = new Label(ENQUANTO);
        Label *inicio = new Label(INICIO);
        Label *fim = new Label(FIMFACA);

        whilee->expression->accept(this);
        exp = this->_exp;

        if (whilee->statement != NULL) whilee->statement->accept(this);
        stmt = this->_stmt;

        this->_stmt = new SEQ(
                        new LABEL(teste),
                        new SEQ(
                            new CJUMP(DIFERENTE, exp, new CONST(0), inicio, fim),
                            new SEQ(
                                new LABEL(inicio),
                                new SEQ(
                                    stmt,
                                    new SEQ(
                                        new JUMP(new NAME(teste)),
                                        new LABEL(fim)
                                    )
                                )
                            )
                        )
                    );

    }

    this->_body = corpo_fechado;

}

void VisitorGeradorRI::visit(Read* read) {

    this->_explist = NULL;
    this->_exp = NULL;

    read->exprlist->accept(this);

    this->_stmt = new EXP(
            new CALL(
            new NAME(new Label((char*)"LEIA")),
            this->_explist
            )
            );



}

void VisitorGeradorRI::visit(Write* write) {

    this->_exp = NULL;
    this->_explist = NULL;

    write->exprlist->accept(this);

    this->_stmt = new EXP(
            new CALL(
            new NAME(new Label((char*)"IMPRIMA")),
            this->_explist
            )
            );




}

void VisitorGeradorRI::visit(CompoundStatement* compoundstatement) {

    if (compoundstatement->statementlist != NULL) compoundstatement->statementlist->accept(this);

    StmtList* stmtlist = this->_body;
    SEQ* topo = NULL;
    this->_stmt = NULL;
    while(stmtlist != NULL) {
        if(this->_stmt == NULL) {
            this->_stmt = new SEQ(stmtlist->stmt, NULL);
            topo = (SEQ*) this->_stmt;
        }
        else {
            while(topo->stmt2 != NULL) topo = (SEQ*) topo->stmt2;
            topo->stmt2 = new SEQ(stmtlist->stmt, NULL);
        }
        stmtlist = stmtlist->stmtlist;
    }


}

void VisitorGeradorRI::visit(Variable* variable) {

    Identificador* identificador = variable->identificator;
    if (variable->expression != NULL) {
        variable->expression->accept(this);
    }
    //if (variable->variable != NULL) variable->variable->accept(this);


      // Buscando a variavel nos escopos, para obter mais informacoes
    int i = 0;
    simbolo_variavel *svar = (simbolo_variavel*) _escopo_atual->buscar_variavel(identificador->identificador, &i);
    simbolo_funcao* sfunc = NULL;

    if (svar == NULL) {


        i = 0;
        simbolo_funcao* sfunc = (simbolo_funcao*) _escopo_atual->buscar_funcao(identificador->identificador, &i);
        if (sfunc != NULL) {
            // preciso trazer o escopo e colocar no resultado do frame...
            Temp* t = new Temp();
            t->travado = REG_V0;
            this->_exp = new TEMP(t);
            return;
        }

        // Pode ser o nome
        printf("Não encontrada no escopo: %s", identificador->identificador);
        //erro @jean
        exit(-1);
    }

    if (svar->nlocal == 1) {

        Exp* desloc = NULL;
        if(variable->expression != NULL){
            variable->expression->accept(this);
            desloc = new BINOP(MULTIPLICACAO,new CONST(4),new BINOP(MENOS,this->_exp,new CONST(svar->lowerbound)));
        }else{
            desloc = new CONST(0);
        }

        // Variáveis não locais
        char* nomelabel = (char*) malloc(sizeof (char) *(strlen(identificador->identificador) + strlen((char*) "global_") + 1));
        sprintf(nomelabel, "global_%s", identificador->identificador);
        this->_exp = new MEM(new BINOP(MAIS,new NAME(new Label(nomelabel)),desloc));
    } else {
        Exp* acesso = NULL;

        Temp* fp = new Temp();
        fp->travado = REG_FP;

        // variáveis acessadas de outro escopo
        if (i > 0) {
            i--;
            acesso = new MEM(new TEMP(fp));
            while (i > 0) {
                acesso = new MEM((MEM*) acesso);
                i--;
            }
        } else {
            acesso = new TEMP(fp);
        }


        Exp* desloc = NULL;
        if(variable->expression != NULL){
            variable->expression->accept(this);

            desloc = new BINOP(MAIS,new CONST(svar->offset), new BINOP(MULTIPLICACAO,new CONST(4),new BINOP(MENOS,this->_exp,new CONST(svar->lowerbound))));
        }else{
            desloc = new CONST(svar->offset);
        }

        // Variáveis locais, deste escopo
        this->_exp = new MEM(new BINOP(MAIS, acesso, desloc));
    }

}

//
// ok!

void VisitorGeradorRI::visit(NumberInteger* numberinteger) {
    this->_exp = new CONST(atoi(numberinteger->numberinteger));
}

//
// ok!

void VisitorGeradorRI::visit(NumberReal* numberreal) {
    this->_exp = new CONSTF(atof(numberreal->numberreal));
}

//
// ok

void VisitorGeradorRI::visit(Boolean* boolean) {
    if (boolean->boolean == VERDADEIRO) this->_exp = new CONST(1);
    else this->_exp = new CONST(0);
}

//
// ok

void VisitorGeradorRI::visit(Literal* literal) {
    LiteralRI* l = new LiteralRI(literal->literal);

    l->proximo = _fragment;
    _fragment = l;

    this->_exp = new NAME(l->label);
}



//
// ok

void VisitorGeradorRI::visit(RelationOp* relationop) {
    Exp* exp1;
    Exp* exp2;

    Temp* temp = new Temp();
    temp->travado = REG_T8;
    Label* label_t = new Label(VERDADEIRO);
    Label* label_f = new Label(FALSO);

    relationop->expression1->accept(this);
    exp1 = this->_exp;

    relationop->expression2->accept(this);
    exp2 = this->_exp;

    this->_exp = new ESEQ(
                    new SEQ(
                        new MOVE(new TEMP(temp), new CONST(1)),
                        new SEQ(
                            new CJUMP(relationop->operation, exp1, exp2, label_t, label_f),
                            new SEQ(
                                new LABEL(label_f),
                                new SEQ(
                                    new MOVE(new TEMP(temp), new CONST(0)),
                                    new LABEL(label_t)
                                )
                            )
                        )
                    ),
                    new TEMP(temp)
                );

}

//
// ok

void VisitorGeradorRI::visit(ComparativeOp* comparativeop) {
    Exp* exp1;
    Exp* exp2;

    Temp* temp = new Temp();
    temp->travado = REG_T9;
    Label* label_t = new Label(VERDADEIRO);
    Label* label_f = new Label(FALSO);

    comparativeop->expression1->accept(this);
    exp1 = this->_exp;

    comparativeop->expression2->accept(this);
    exp2 = this->_exp;

    this->_exp = new ESEQ(
                    new SEQ(
                        new MOVE(new TEMP(temp), new CONST(1)),
                        new SEQ(
                            new CJUMP(comparativeop->operation, exp1, exp2, label_t, label_f),
                            new SEQ(
                                new LABEL(label_f),
                                new SEQ(
                                    new MOVE(new TEMP(temp), new CONST(0)),
                                    new LABEL(label_t)
                                )
                            )
                        )
                    ),
                    new TEMP(temp)
                );


//    this->_exp = new BINOP(comparativeop->operation, exp1, exp2);
}

//
// ok

void VisitorGeradorRI::visit(Sign* sign) {
    if (sign->sign == MENOS) {
        sign->expression->accept(this);
        this->_exp = new BINOP(MENOS, new CONST(0), this->_exp);
    }
}

//
// ok

void VisitorGeradorRI::visit(MultiplicationOp* multiplicationop) {
    Exp* exp1;
    Exp* exp2;

    multiplicationop->expression1->accept(this);
    exp1 = this->_exp;

    multiplicationop->expression2->accept(this);
    exp2 = this->_exp;

    this->_exp = new BINOP(multiplicationop->operation, exp1, exp2);
}

//
// ok

void VisitorGeradorRI::visit(BooleanOp* booleanop) {
    Exp* exp1;
    Exp* exp2;
    Temp* temp = new Temp();
    temp->travado = REG_T9;
    Label* label1 = new Label(SE);
    Label* label2 = new Label(SENAO);
    Label* label3 = new Label(ENTAO);

    booleanop->expression1->accept(this);
    exp1 = this->_exp;

    booleanop->expression2->accept(this);
    exp2 = this->_exp;

    if (booleanop->operation == E) {

        this->_exp = new ESEQ(
                new SEQ(
                new MOVE(
                new TEMP(temp),
                new CONST(0)
                ),
                new SEQ(
                new CJUMP(DIFERENTE, exp1, new CONST(0), label1, label2),
                new SEQ(
                new LABEL(label1),
                new SEQ(
                new CJUMP(DIFERENTE, exp2, new CONST(0), label3, label2),
                new SEQ(
                new LABEL(label3),
                new SEQ(
                new MOVE(
                new TEMP(temp),
                new CONST(1)
                ),
                new LABEL(label2)
                )
                )
                )
                )
                )
                ),
                new TEMP(temp)
                );

    } else {

        this->_exp = new ESEQ(
                new SEQ(
                new MOVE(
                new TEMP(temp),
                exp1
                ),
                new SEQ(
                new CJUMP(DIFERENTE, exp2, new CONST(0), label1, label2),
                new SEQ(
                new LABEL(label1),
                new SEQ(
                new MOVE(
                new TEMP(temp),
                new CONST(1)
                ),
                new LABEL(label2)
                )
                )
                )
                ),
                new TEMP(temp)
                );
    }
}

//
// ok

void VisitorGeradorRI::visit(Not* noat) {

    noat->expression->accept(this);
    Exp* e = this->_exp;

    Label* label_verdadeiro = new Label(VERDADEIRO);
    Label* label_falso = new Label(FALSO);
    Label* label_fim = new Label(FIM);
    Temp* t = new Temp();
    t->travado = REG_T8;

    this->_exp = new ESEQ(
            new SEQ(
            new CJUMP(DIFERENTE, e, new CONST(0), label_verdadeiro, label_falso),
            new SEQ(
            new SEQ(
            new LABEL(label_verdadeiro),
            new MOVE(new TEMP(t), new CONST(0))
            ),
            new SEQ(
            new SEQ(
            new JUMP(new NAME(label_fim)),
            new LABEL(label_falso)
            ),
            new SEQ(
            new MOVE(new TEMP(t), new CONST(1)),
            new LABEL(label_fim)
            )
            )
            )
            ),
            new TEMP(t)
            );

}

void VisitorGeradorRI::visit(ExprList* exprlist) {

    if (exprlist->explist != NULL) exprlist->explist->accept(this);

    exprlist->expression->accept(this);
    Exp* e = this->_exp;


    this->_explist = new ExpList(e, this->_explist);



    //    exprlist->expression->accept(this);
    //    /* Caso a lista continue, imprime. */
    //

}

void VisitorGeradorRI::visit(Identificador* identificador) {

    }



void VisitorGeradorRI::visit(Primitive* primitive) {
}

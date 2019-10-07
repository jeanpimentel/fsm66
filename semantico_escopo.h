#ifndef SEMANTICO_ESCOPO_H
#define	SEMANTICO_ESCOPO_H

#include "tabela_simbolos.h"
#include "arvore_sintaxe_abstrata.h"

class SemanticoEscopoVisitor : public Visitor {
public:

    escopo* escopo_program;
    escopo* escopo_atual;

    int escopo_registro;


    void visit(Program* program);
    void visit(SubProgList*);
    void visit(TypeList*);
    void visit(VarList*);
    void visit(Type*);
    void visit(If*);
    void visit(While*);
    void visit(Read*);
    void visit(Write*);
    void visit(Assign*);
    void visit(StatementList*);
    void visit(CompoundStatement*);
    void visit(Variable*);
    void visit(NumberInteger*);
    void visit(NumberReal*);
    void visit(Boolean*);
    void visit(Literal*);
    void visit(Call*);
    void visit(RelationOp*);
    void visit(ComparativeOp*);
    void visit(Sign*);
    void visit(AdditionalOp*);
    void visit(MultiplicationOp*);
    void visit(BooleanOp*);
    void visit(Not*);
    void visit(ExprList*);
    void visit(Identificador*);
    void visit(Primitive*);

    void iniciar_escopo(const char*);
    escopo* finalizar_escopo();
    void inserir_no_escopo();
    simbolo* buscar_escopo_atual(char*);
    simbolo_variavel* buscar_variavel_nos_escopos(char*);
    simbolo_registro* buscar_registro_nos_escopos(char*);
    simbolo_funcao* buscar_funcao_nos_escopos(char*);
    escopo* obter_escopo_program();

};

#endif	/* SEMANTICO_ESCOPO_H */


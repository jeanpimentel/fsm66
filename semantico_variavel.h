#ifndef SEMANTICO_VARIAVEL_H
#define	SEMANTICO_VARIAVEL_H

#include "tabela_simbolos.h"
#include "arvore_sintaxe_abstrata.h"

class SemanticoVariavelVisitor : public Visitor {
public:

    escopo* escopo_program;
    escopo* escopo_atual;

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
    void definir_escopo_program(escopo*);

};

#endif	/* SEMANTICO_VARIAVEL_H */

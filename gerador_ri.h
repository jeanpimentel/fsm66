#ifndef GERADOR_RI_H_INCLUDED
#define GERADOR_RI_H_INCLUDED

#include "representacao_intermediaria.h"
#include "arvore_sintaxe_abstrata.h"
#include "simbolos.h"
#include "tabela_simbolos.h"

#define TAMANHO_PALAVRA 4;

class VisitorGeradorRI : public Visitor {
private:
    Exp* exp;

public:

    int nlocal;
    escopo*  _escopo_base;
    escopo*  _escopo_atual;
    Fragment* _fragment;
    StmtList* _body;
    Stmt*     _stmt;
    ExpList*  _explist;
    Exp*      _exp;
    FrameMIPS* _frame;
    int       _offset;

    //raizri
    Fragment* raizri;
    //indice de registradores disponiveis
    TempList* registradores;
    VisitorGeradorRI(escopo* escopo);
    ~VisitorGeradorRI();

    void devolveRegistrador(Temp*);
    Temp* obterRegistrador();
    Fragment* obter_ultimo_subprog();

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

};

#endif // GERADOR_RI_H_INCLUDED

#ifndef ARVORE_SINTAXE_ABSTRATA_H
#define	ARVORE_SINTAXE_ABSTRATA_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

class Visitor;

/** @brief Classe para um n� Identificador na ASA */
class Identificador {
public:
    int linha;

    char* identificador;

    Identificador(char* id, int ln);
    ~Identificador();
    void accept(Visitor* v);
};

/** @brief Classe para um n� Primitives na ASA */
class Primitive {
public:
    int linha;

    int primitivetype;

    Primitive(int type, int ln);
    ~Primitive();
    void accept(Visitor* v);
};

/** @brief Classe para um n� Statement na ASA */
class Statement {
public:
    virtual void accept(Visitor* v) = 0;
};

/** @brief Classe para um n� Expression na ASA */
class Expression {
public:
    virtual void accept(Visitor* v) = 0;
};

/** @brief Classe para um n� If na ASA */
class If : public Statement {
public:
    int linha;

    Expression* expression;
    Statement* statement1;
    Statement* statement2;

    If(Expression* exp, Statement* stmt1, Statement* stmt2, int ln);
    ~If();
    void accept(Visitor* v);
};

/** @brief Classe para um n� While na ASA */
class While : public Statement {
public:
    int linha;

    Expression* expression;
    Statement* statement;

    While(Expression* exp, Statement* stmt, int ln);
    ~While();
    void accept(Visitor* v);

};

/** @brief Classe para um n� StatementList na ASA */
class StatementList : public Statement {
public:
    int linha;


    Statement* statement;
    StatementList* statementlist;

    StatementList(Statement* stmt, StatementList* stmtlist, int ln);
    StatementList();
    ~StatementList();
    void accept(Visitor* v);
};

/** @brief Classe para um n� CompoundStatement na ASA */
class CompoundStatement : public Statement {
public:
    int linha;

    StatementList* statementlist;
    CompoundStatement(StatementList* statement, int ln);
    ~CompoundStatement();
    void accept(Visitor* v);
};

/** @brief Classe para um n� Variable na ASA */
class Variable : public Expression {
public:
    int linha;

    Variable* variable;
    Identificador* identificator;
    Expression* expression;

    Variable(Variable* var, Identificador* id, Expression* ex, int lnp);
    Variable(Identificador* id, Expression* exp, int ln);
    Variable(Identificador* id, int ln);
    ~Variable();
    void accept(Visitor* v);
};

/** @brief Classe para um n� Assign na ASA */
class Assign : public Statement, public Expression {
public:
    int linha;

    Variable* variable;
    Expression* expression;

    Assign(Variable* var, Expression* exp, int ln);
    ~Assign();
    void accept(Visitor* v);
};

/** @brief Classe para um n� NumberReal na ASA */
class NumberReal : public Expression {
public:
    int linha;

    char* numberreal;

    NumberReal(char* real, int ln);
    ~NumberReal();
    void accept(Visitor* v);
};

/** @brief Classe para um n� NumberInteger na ASA */
class NumberInteger : public Expression {
public:
    int linha;

    char* numberinteger;

    NumberInteger(char* integer, int ln);
    ~NumberInteger();
    void accept(Visitor* v);
};

/** @brief Classe para um n� Boolean na ASA */
class Boolean : public Expression {
public:
    int linha;

    int boolean;

    Boolean(int integer, int ln);
    ~Boolean();
    void accept(Visitor* v);
};

/** @brief Classe para um n� Literal na ASA */
class Literal : public Expression {
public:
    int linha;

    char* literal;

    Literal(char* lit, int ln);
    ~Literal();
    void accept(Visitor* v);
};

/** @brief Classe para um n� RelationOp na ASA */
class RelationOp : public Expression {
public:
    int linha;

    int operation;
    Expression* expression1;
    Expression* expression2;

    RelationOp(int op, Expression* exp1, Expression* exp2, int ln);
    ~RelationOp();
    void accept(Visitor* v);
};

/** @brief Classe para um n� ComparativeOp na ASA */
class ComparativeOp : public Expression {
public:
    int linha;

    int operation;
    Expression* expression1;
    Expression* expression2;

    ComparativeOp(int op, Expression* exp1, Expression* exp2, int ln);
    ~ComparativeOp();
    void accept(Visitor* v);
};

/** @brief Classe para um n� AdditionalOp na ASA */
class AdditionalOp : public Expression {
public:
    int linha;

    int operation;
    Expression* expression1;
    Expression* expression2;

    AdditionalOp(int op, Expression* exp1, Expression* exp2, int ln);
    ~AdditionalOp();
    void accept(Visitor* v);
};

/** @brief Classe para um n� MultiplicationOp na ASA */
class MultiplicationOp : public Expression {
public:
    int linha;

    int operation;
    Expression* expression1;
    Expression* expression2;

    MultiplicationOp(int op, Expression* exp, Expression* exp2, int ln);
    ~MultiplicationOp();
    void accept(Visitor* v);
};

/** @brief Classe para um n� BooleanOp na ASA */
class BooleanOp : public Expression {
public:
    int linha;

    int operation;
    Expression* expression1;
    Expression* expression2;

    BooleanOp(int op, Expression* exp1, Expression* exp2, int ln);
    ~BooleanOp();
    void accept(Visitor* v);
};

/** @brief Classe para um n� Not na ASA */
class Not : public Expression {
public:
    int linha;

    Expression* expression;

    Not(Expression* exp, int ln);
    ~Not();
    void accept(Visitor* v);
};

/** @brief Classe para um n� Sign na ASA. */
class Sign : public Expression {
public:
    int linha;

    int sign;
    Expression* expression;

    Sign(int op, Expression* exp, int ln);
    ~Sign();
    void accept(Visitor* v);
};

/** @brief Classe para um n� ExprList na ASA */
class ExprList {
public:
    int linha;

    Expression* expression;
    ExprList* explist;

    ExprList(Expression* exp, ExprList* expl, int ln);
    ExprList();
    ~ExprList();
    void accept(Visitor* v);
};

/** @brief Classe para um n� Call na ASA */
class Call : public Expression, public Statement {
public:
    int linha;

    Identificador* identificador;
    ExprList* exprlist;

    Call(Identificador* id, ExprList* expr, int ln);
    ~Call();
    void accept(Visitor* v);
};

/** @brief Classe para um n� Read na ASA */
class Read : public Statement {
public:
    int linha;

    ExprList* exprlist;

    Read(ExprList* exprl, int ln);
    ~Read();
    void accept(Visitor* v);
};

/** @brief Classe para um n� Write na ASA */
class Write : public Statement {
public:
    int linha;

    ExprList* exprlist;

    Write(ExprList* exprl, int ln);
    ~Write();
    void accept(Visitor* v);
};

/** @brief Classe para um n� Type na ASA */
class Type {
public:
    int linha;

    Identificador* identificador;
    Primitive* primitive;
    NumberInteger* lowerbound;
    NumberInteger* upperbound;

    Type(Identificador* id, NumberInteger* lower, NumberInteger* upper, int ln);
    Type(Primitive* pri, NumberInteger* lower, NumberInteger* upper, int ln);
    ~Type();
    void accept(Visitor* v);
};

/** @brief Classe para um n� VarList na ASA */
class VarList {
public:
    int linha;

    Type* type;
    Identificador* identificador;
    VarList* varlist;

    VarList(Type* tp, Identificador* id, VarList* varl, int ln);
    VarList(Identificador* id, VarList* varl, int ln);
    VarList();
    ~VarList();
    void accept(Visitor* v);
};

/** @brief Classe para um n� TypeList na ASA */
class TypeList {
public:
    int linha;

    Identificador* identificador;
    VarList* varlist;
    TypeList* typelist;

    TypeList(Identificador* id, VarList* varl, TypeList* typel, int ln);
    TypeList();
    ~TypeList();
    void accept(Visitor* v);
};

/** @brief Classe para um n� SubProgList na ASA */
class SubProgList {
public:
    int linha;

    Type* type;
    Identificador* identificador;
    VarList* arguments;
    VarList* varlist;
    TypeList* typelist;
    StatementList* statementlist;
    SubProgList* subproglist1;
    SubProgList* subproglist2;

    SubProgList(Type* tp, Identificador* id, VarList* args, VarList* var, TypeList* tplist, StatementList* stmtlist, SubProgList* subprogl1, SubProgList* subprog2, int ln);
    SubProgList();
    ~SubProgList();
    void accept(Visitor* v);

};

/** @brief Classe para um n� Program na ASA */
class Program {
public:
    int linha;

    VarList *varlist;
    SubProgList *subproglist;
    TypeList *typelist;
    StatementList *statementlist;

    Program(VarList* varl, TypeList* typel, SubProgList* subprogl, StatementList* stmtl, int ln);
    ~Program();
    void accept(Visitor* v);
};


/**
 * @brief   Classe base para implementa��o dos visitor.
 * @details Possui um m�todo de visita para cada componente poss�vel em uma ASA. \n
 * Todos devem ser sobrescritos na subclasse de forma apropriada.
 */
class Visitor {
public:

    virtual void visit(Program*) = 0;
    virtual void visit(SubProgList*) = 0;
    virtual void visit(TypeList*) = 0;
    virtual void visit(VarList*) = 0;
    virtual void visit(Type*) = 0;
    virtual void visit(If*) = 0;
    virtual void visit(While*) = 0;
    virtual void visit(Read*) = 0;
    virtual void visit(Write*) = 0;
    virtual void visit(Assign*) = 0;
    virtual void visit(StatementList*) = 0;
    virtual void visit(CompoundStatement*) = 0;
    virtual void visit(Variable*) = 0;
    virtual void visit(NumberInteger*) = 0;
    virtual void visit(NumberReal*) = 0;
    virtual void visit(Boolean*) = 0;
    virtual void visit(Literal*) = 0;
    virtual void visit(Call*) = 0;
    virtual void visit(RelationOp*) = 0;
    virtual void visit(ComparativeOp*) = 0;
    virtual void visit(Sign*) = 0;
    virtual void visit(AdditionalOp*) = 0;
    virtual void visit(MultiplicationOp*) = 0;
    virtual void visit(BooleanOp*) = 0;
    virtual void visit(Not*) = 0;
    virtual void visit(ExprList*) = 0;
    virtual void visit(Identificador*) = 0;
    virtual void visit(Primitive*) = 0;
};

/**
 * @brief Subclasse de Visitor.
 * @details Ao visitar cada n�, imprime toda a �rvore de maneira identada.
 */
class ImprimeVisitor : public Visitor {
private:
    int aninhamento;

    void aumentar_identacao();
    void diminuir_identacao();
    void imprimir_identado(const char* no);
public:

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

#endif	/* ARVORE_SINTAXE_ABSTRATA_H */


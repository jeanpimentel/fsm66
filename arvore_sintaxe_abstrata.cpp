#include "arvore_sintaxe_abstrata.h"
#include "lexico.h"

/** @brief  Nó Program. */
Program::Program(VarList* var, TypeList *type, SubProgList* subprog, StatementList* statement, int ln) {
    varlist = var;
    subproglist = subprog;
    typelist = type;
    statementlist = statement;
    linha = ln;
}

Program::~Program() {
    if(varlist != NULL)         delete varlist;
    if(subproglist != NULL)     delete subproglist;
    if(typelist != NULL)        delete typelist;
    if(statementlist != NULL)   delete statementlist;
}

void Program::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó SubProgList */
SubProgList::SubProgList(Type* tp, Identificador* id, VarList* args, VarList* var, TypeList* tplist, StatementList* stmtlist, SubProgList* subprogl1, SubProgList* subprogl2, int ln) {
    type = tp;
    identificador = id;
    arguments = args;
    varlist = var;
    typelist = tplist;
    statementlist = stmtlist;
    subproglist1 = subprogl1;
    subproglist2 = subprogl2;

    linha = ln;
}

SubProgList::SubProgList() {
    type = NULL;
    identificador = NULL;
    arguments = NULL;
    varlist = NULL;
    typelist = NULL;
    statementlist = NULL;
    subproglist1 = NULL;
    subproglist2 = NULL;
}

SubProgList::~SubProgList() {
    if(type != NULL)          delete type;
    if(identificador!= NULL)  delete identificador;
    if(arguments != NULL)     delete arguments;
    if(varlist != NULL)       delete varlist;
    if(typelist != NULL)      delete typelist;
    if(statementlist != NULL) delete statementlist;
    if(subproglist1 != NULL)  delete subproglist1;
    if(subproglist2 != NULL)  delete subproglist2;
}

void SubProgList::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó TypeList */
TypeList::TypeList(Identificador* id, VarList* varl, TypeList* typel, int ln) {
    identificador = id;
    varlist = varl;
    typelist = typel;
    linha = ln;
}

TypeList::TypeList() {
    identificador = NULL;
    varlist       = NULL;
    typelist      = NULL;
}

TypeList::~TypeList() {
    if (identificador != NULL)  delete identificador;
    if (varlist != NULL)        delete varlist;
    if (typelist != NULL)       delete typelist;
}

void TypeList::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó VarList */
VarList::VarList(Type* tp, Identificador* id, VarList* varl, int ln) {
    type = tp;
    identificador = id;
    varlist = varl;
    linha = ln;
}

VarList::VarList(Identificador* id, VarList* varl, int ln) {
    type = NULL;
    identificador = id;
    varlist = varl;
    linha = ln;
}

VarList::VarList() {
    type         = NULL;
    identificador = NULL;
    varlist = NULL;
}

VarList::~VarList() {
    if(type != NULL)          delete type;
    if(identificador != NULL) delete identificador;
    if(varlist != NULL)       delete varlist;
}

void VarList::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó Type */
Type::Type(Identificador* id, NumberInteger* lower, NumberInteger* upper,int ln) {
    identificador = id;
    primitive   = NULL;
    lowerbound = lower;
    upperbound = upper;
    linha = ln;
}

Type::Type(Primitive* pri, NumberInteger* lower, NumberInteger* upper,int ln) {
    primitive = pri;
    identificador = NULL;
    lowerbound = lower;
    upperbound = upper;
    linha = ln;
}

Type::~Type() {
    if(primitive != NULL)     delete primitive;
    if(identificador != NULL) delete identificador;
    if(lowerbound != NULL)    delete lowerbound;
    if(upperbound != NULL)    delete upperbound;
}

void Type::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó If */
If::If(Expression* exp, Statement* stmt1, Statement* stmt2,int ln) {
    expression = exp;
    statement1 = stmt1;
    statement2 = stmt2;

    linha = ln;
}

If::~If() {
     if(expression != NULL)   delete expression;
     if(statement1 != NULL)   delete statement1;
     if(statement2 != NULL)   delete statement2;
}

void If::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó While */
While::While(Expression* exp, Statement* stmt, int ln) {
    expression = exp;
    statement = stmt;

    linha = ln;
}

While::~While() {
    if(expression != NULL) delete expression;
    if(statement  != NULL) delete statement;
}

void While::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó Read */
Read::Read(ExprList* exprl,int ln) {
    exprlist = exprl;

    linha = ln;
}

Read::~Read() {
    if(exprlist != NULL) delete exprlist;
}

void Read::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó Write */
Write::Write(ExprList* exprl, int ln) {
    exprlist = exprl;

    linha = ln;
}

Write::~Write() {
    if(exprlist != NULL) delete exprlist;
}

void Write::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó Assign */
Assign::Assign(Variable* var, Expression* exp, int ln) {
    variable = var;
    expression = exp;

    linha = ln;
}

Assign::~Assign() {
    if(variable != NULL)   delete variable;
    if(expression != NULL) delete expression;
}

void Assign::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó StatementList */
StatementList::StatementList(Statement* stmt, StatementList* stmtlist, int ln) {
    statement = stmt;
    statementlist = stmtlist;
    linha = ln;
}

StatementList::~StatementList() {
    if(statement != NULL)      delete statement;
    if(statementlist != NULL)  delete statementlist;
}

void StatementList::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó Variable */
Variable::Variable(Variable* var, Identificador* id, Expression* exp, int ln) {
    variable = var;
    identificator = id;
    expression = exp;
    linha = ln;
}

Variable::Variable(Identificador* id, Expression* exp, int ln) {
    variable      = NULL;
    identificator = id;
    expression    = exp;
    linha         = ln;

    linha = ln;
}

Variable::Variable(Identificador* id, int ln) {
    variable      = NULL;
    identificator = id;
    expression    = NULL;

    linha = ln;
}

Variable::~Variable() {
    if(variable != NULL)      delete variable;
    if(identificator != NULL) delete identificator;
    if(expression != NULL)    delete expression;
}

void Variable::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó NumberReal */
NumberReal::NumberReal(char* real, int ln) {
    numberreal = real;

    linha = ln;
}

NumberReal::~NumberReal() {}

void NumberReal::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó NumberInteger */
NumberInteger::NumberInteger(char* integer, int ln) {
    numberinteger = integer;

    linha = ln;
}

NumberInteger::~NumberInteger() {}

void NumberInteger::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó Boolean */
Boolean::Boolean(int integer, int ln) {
    boolean = integer;

    linha = ln;
}

Boolean::~Boolean() {}

void Boolean::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó Literal */
Literal::Literal(char* lit, int ln) {
    literal = lit;

    linha = ln;
}

Literal::~Literal() {
    free(literal);
}

void Literal::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó Call */
Call::Call(Identificador* id, ExprList* expr, int ln) {
    identificador = id;
    exprlist = expr;

    linha = ln;
}

Call::~Call() {
     if(identificador != NULL) delete identificador;
     if(exprlist != NULL)      delete exprlist;
}

void Call::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó RelationOp */
RelationOp::RelationOp(int op, Expression* exp1, Expression* exp2, int ln) {
    operation = op;
    expression1 = exp1;
    expression2 = exp2;

    linha = ln;
}

RelationOp::~RelationOp() {
     if(expression1 != NULL) delete expression1;
     if(expression2 != NULL) delete expression2;
}

void RelationOp::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó ComparativeOp */
ComparativeOp::ComparativeOp(int op, Expression* exp1, Expression* exp2, int ln) {
    operation = op;
    expression1 = exp1;
    expression2 = exp2;

    linha = ln;
}

ComparativeOp::~ComparativeOp() {
     if(expression1 != NULL) delete expression1;
     if(expression2 != NULL) delete expression2;
}

void ComparativeOp::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó AdditionalOp */
AdditionalOp::AdditionalOp(int op, Expression* exp1, Expression* exp2, int ln) {
    operation = op;
    expression1 = exp1;
    expression2 = exp2;

    linha = ln;
}

AdditionalOp::~AdditionalOp() {
    if(expression1 != NULL) delete expression1;
    if(expression2 != NULL) delete expression2;
}

void AdditionalOp::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó MultiplicationOp */
MultiplicationOp::MultiplicationOp(int op, Expression* exp1, Expression* exp2, int ln) {
    operation = op;
    expression1 = exp1;
    expression2 = exp2;

    linha = ln;
}

MultiplicationOp::~MultiplicationOp() {
   if(expression1 != NULL)  delete expression1;
    if(expression2 != NULL) delete expression2;
}

void MultiplicationOp::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó BooleanOp */
BooleanOp::BooleanOp(int op, Expression* exp1, Expression* exp2, int ln) {
    operation = op;
    expression1 = exp1;
    expression2 = exp2;

    linha = ln;
}

BooleanOp::~BooleanOp() {
   if(expression1 != NULL) delete expression1;
   if(expression2 != NULL) delete expression2;
}

void BooleanOp::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó Not */
Not::Not(Expression* exp, int ln) {
    expression = exp;

    linha = ln;
}

Not::~Not() {
     if(expression != NULL) delete expression;
}

void Not::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó Sign */
Sign::Sign(int op, Expression* exp, int ln) {
    sign = op;
    expression = exp;

    linha = ln;
}

Sign::~Sign() {
    if(expression != NULL) delete expression;
}

void Sign::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó ExpressionList */
ExprList::ExprList(Expression* exp, ExprList* expl, int ln) {
    expression = exp;
    explist = expl;

    linha = ln;
}

ExprList::ExprList() {
    expression = NULL;
    explist    = NULL;
    linha = 0;
}

ExprList::~ExprList() {
    if(expression != NULL) delete expression;
    if(explist != NULL)    delete explist;
}

void ExprList::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó CompoundStatement */
CompoundStatement::CompoundStatement(StatementList* statement, int ln){
    statementlist = statement;

    linha = ln;
}
CompoundStatement::~CompoundStatement(){
    if(statementlist != NULL) delete statementlist;
}

void CompoundStatement::accept(Visitor* v){
    v->visit(this);
}

/** @brief  Nó identificador */
Identificador::Identificador(char* id, int ln) {
    identificador = id;

    linha = ln;
}

Identificador::~Identificador() {}

void Identificador::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Nó Primitive */
Primitive::Primitive(int type, int ln) {
    primitivetype = type;

    linha = ln;
}

Primitive::~Primitive() {}

void Primitive::accept(Visitor* v) {
    v->visit(this);
}

/** @brief  Aumenta a identação durante a impressão da árvore. */
void ImprimeVisitor::aumentar_identacao() {
    aninhamento += 4;
}

/** @brief  Diminui a identação durante a impressão da árvore. */
void ImprimeVisitor::diminuir_identacao() {
    aninhamento -= 4;
}

/**
 * @brief  Imprime as informações da árvore de forma identada.
 * @param  texto Informação a ser impressa.
 */
void ImprimeVisitor::imprimir_identado(const char* texto) {
    int i;
    printf("\n");
    for (i = 0; i < aninhamento; i++) printf(" ");
    printf("%s", texto);
}

void ImprimeVisitor::visit(Program* program) {
    ImprimeVisitor::aninhamento = 0;
    imprimir_identado("[PROGRAM]");
    if(program->varlist != NULL) program->varlist->accept(this);
    if(program->typelist != NULL) program->typelist->accept(this);
    if(program->subproglist != NULL) program->subproglist->accept(this);
    if(program->statementlist != NULL) program->statementlist->accept(this);
}

void ImprimeVisitor::visit(VarList* varlist) {
    aumentar_identacao();
    imprimir_identado("[VARLIST]");
    printf("( linha - %d )", varlist->linha);
    varlist->identificador->accept(this);
    /* Verifica se há tipo na variável, pois a mesma pode ser argumento. */
    if(varlist->type != NULL) varlist->type->accept(this);
    /* Verifica se há uma próxima varlist para visitar. */
    if(varlist->varlist != NULL) varlist->varlist->accept(this);
    diminuir_identacao();
}

void ImprimeVisitor::visit(SubProgList* subproglist) {
    if(subproglist->identificador == NULL) return;
    aumentar_identacao();
    imprimir_identado("[SUBPROGLIST]");
    printf("( linha - %d )", subproglist->linha);
    subproglist->identificador->accept(this);
    if(subproglist->type != NULL)           subproglist->type->accept(this);
    if(subproglist->arguments != NULL)      subproglist->arguments->accept(this);
    if(subproglist->varlist != NULL)        subproglist->varlist->accept(this);
    if(subproglist->typelist != NULL)       subproglist->typelist->accept(this);
    if(subproglist->subproglist1 != NULL)   subproglist->subproglist1->accept(this);
    if(subproglist->statementlist != NULL)  subproglist->statementlist->accept(this);
    if(subproglist->subproglist2 != NULL)   subproglist->subproglist2->accept(this);
    diminuir_identacao();
}

void ImprimeVisitor::visit(TypeList* typelist) {
    aumentar_identacao();
    imprimir_identado("[TYPELIST]");
    printf("( linha - %d )", typelist->linha);
    typelist->identificador->accept(this);
    if(typelist->varlist != NULL) {
        typelist->varlist->accept(this);
    }
    if(typelist->typelist != NULL) {
        typelist->typelist->accept(this);
    }
    diminuir_identacao();
}

void ImprimeVisitor::visit(Type* type) {
    aumentar_identacao();
    imprimir_identado("[TYPE]");
     printf("( linha - %d )", type->linha);
    /* Há dois tipos de Type, identificador ou tipo primitivo. Verificando qual o caso. */
    if(type->identificador != NULL) {
        type->identificador->accept(this);
    } else if(type->primitive != NULL) {
        type->primitive->accept(this);
    }
    if(type->lowerbound->numberinteger != type->upperbound->numberinteger) {
        type->lowerbound->accept(this);
        type->upperbound->accept(this);
    }
    diminuir_identacao();
}

void ImprimeVisitor::visit(If* iff) {
    aumentar_identacao();
    imprimir_identado("[IF]");
    printf("( linha - %d )", iff->linha);
    iff->expression->accept(this);
    if(iff->statement1 != NULL) iff->statement1->accept(this);
    if(iff->statement2 != NULL) iff->statement2->accept(this);
    diminuir_identacao();
}

void ImprimeVisitor::visit(While* whilee) {
    aumentar_identacao();
    imprimir_identado("[WHILE]");
    printf("( linha - %d )", whilee->linha);
    whilee->expression->accept(this);
    if(whilee->statement != NULL)whilee->statement->accept(this);
    diminuir_identacao();
}

void ImprimeVisitor::visit(Read* read) {
    aumentar_identacao();
    imprimir_identado("[READ]");
    printf("( linha - %d )", read->linha);
    read->exprlist->accept(this);
    diminuir_identacao();
}

void ImprimeVisitor::visit(Write* write) {
    aumentar_identacao();
    imprimir_identado("[WRITE]");
    printf("( linha - %d )", write->linha);
    write->exprlist->accept(this);
    diminuir_identacao();
}

void ImprimeVisitor::visit(Assign* assign) {
    aumentar_identacao();
    imprimir_identado("[ASSIGN]");
    printf("( linha - %d )", assign->linha);
    assign->variable->accept(this);
    assign->expression->accept(this);
    diminuir_identacao();
}

void ImprimeVisitor::visit(StatementList* statementlist) {
    aumentar_identacao();
    imprimir_identado("[STATEMENTLIST]");
    printf("( linha - %d )", statementlist->linha);
    statementlist->statement->accept(this);
    if(statementlist->statementlist != NULL) statementlist->statementlist->accept(this);
    diminuir_identacao();
}

void ImprimeVisitor::visit(CompoundStatement* compoundstatement) {
    aumentar_identacao();
    imprimir_identado("[COMPOUNDSTATEMENT]");
    printf("( linha - %d )", compoundstatement->linha);
    if (compoundstatement->statementlist != NULL) compoundstatement->statementlist->accept(this);
    diminuir_identacao();
}

void ImprimeVisitor::visit(Variable* variable) {
    aumentar_identacao();
    imprimir_identado("[VARIABLE]");
    printf("( linha - %d )", variable->linha);
    variable->identificator->accept(this);
    if(variable->expression != NULL) variable->expression->accept(this);
    if(variable->variable != NULL) variable->variable->accept(this);
    diminuir_identacao();
}

void ImprimeVisitor::visit(NumberInteger* numberinteger) {
    aumentar_identacao();
    imprimir_identado("[NUMBERINTEGER]-");
    printf("( linha - %d )", numberinteger->linha);
    printf("%s", numberinteger->numberinteger);
    diminuir_identacao();
}

void ImprimeVisitor::visit(NumberReal* numberreal) {
    aumentar_identacao();
    imprimir_identado("[NUMBERREAL]-");
    printf("( linha - %d )", numberreal->linha);
    printf("%s", numberreal->numberreal);
    diminuir_identacao();
}

void ImprimeVisitor::visit(Boolean* boolean) {
    aumentar_identacao();
    imprimir_identado("[BOOLEAN]-");
    printf("( linha - %d )", boolean->linha);
    printf("%s", lexico_nome_token(boolean->boolean));
    diminuir_identacao();
}

void ImprimeVisitor::visit(Literal* literal) {
    aumentar_identacao();
    imprimir_identado("[LITERAL]-");
    printf("( linha - %d ) ", literal->linha);
    printf("%s", literal->literal);
    diminuir_identacao();
}

void ImprimeVisitor::visit(Call* call) {
    aumentar_identacao();
    imprimir_identado("[CALL]");
    printf("( linha - %d ) ", call->linha);
    call->identificador->accept(this);
    if(call->exprlist != NULL) call->exprlist->accept(this);
    diminuir_identacao();
}

void ImprimeVisitor::visit(RelationOp* relationop) {
    aumentar_identacao();
    imprimir_identado("[RELATIONALOP]-");
    printf("( linha - %d ) ", relationop->linha);
    printf("%s", lexico_nome_token(relationop->operation));
    relationop->expression1->accept(this);
    relationop->expression2->accept(this);
    diminuir_identacao();
}

void ImprimeVisitor::visit(ComparativeOp* comparativeop) {
    aumentar_identacao();
    imprimir_identado("[COMPARATIVEOP]-");
    printf("( linha - %d ) ", comparativeop->linha);
    printf("%s", lexico_nome_token(comparativeop->operation));
    comparativeop->expression1->accept(this);
    comparativeop->expression2->accept(this);
    diminuir_identacao();
}

void ImprimeVisitor::visit(Sign* sign) {
    aumentar_identacao();
    imprimir_identado("[SIGN] -");
    printf("( linha - %d ) ", sign->linha);
    printf("%s", lexico_nome_token(sign->sign));
    sign->expression->accept(this);
    diminuir_identacao();
}

void ImprimeVisitor::visit(AdditionalOp* additionalop) {
    aumentar_identacao();
    imprimir_identado("[ADDITIONALOP]-");
    printf("( linha - %d ) ", additionalop->linha);
    printf("%s", lexico_nome_token(additionalop->operation));
    additionalop->expression1->accept(this);
    additionalop->expression2->accept(this);
    diminuir_identacao();
}

void ImprimeVisitor::visit(MultiplicationOp* multiplicationop) {
    aumentar_identacao();
    imprimir_identado("[MULTIPLICATIONOP]-");
    printf("( linha - %d ) ", multiplicationop->linha);
    printf("%s", lexico_nome_token(multiplicationop->operation));
    multiplicationop->expression1->accept(this);
    multiplicationop->expression2->accept(this);
    diminuir_identacao();
}

void ImprimeVisitor::visit(BooleanOp* booleanop) {
    aumentar_identacao();
    imprimir_identado("[BOOLEANOP]-");
    printf("( linha - %d ) ", booleanop->linha);
    printf("%s", lexico_nome_token(booleanop->operation));
    booleanop->expression1->accept(this);
    booleanop->expression2->accept(this);
    diminuir_identacao();
}

void ImprimeVisitor::visit(Not* noat) {
    aumentar_identacao();
    imprimir_identado("[NOT]");
    printf("( linha - %d ) ", noat->linha);
    noat->expression->accept(this);
    diminuir_identacao();
}

void ImprimeVisitor::visit(ExprList* exprlist) {
    aumentar_identacao();
    imprimir_identado("[EXPRLIST]");
    printf("( linha - %d ) ", exprlist->linha);
    exprlist->expression->accept(this);
    /* Caso a lista continue, imprime. */
    if(exprlist->explist != NULL) exprlist->explist->accept(this);
    diminuir_identacao();
}

void ImprimeVisitor::visit(Identificador* identificador) {
    aumentar_identacao();
    imprimir_identado("[IDENTIFICADOR]-");
    printf("( linha - %d ) ", identificador->linha);
    printf("%s",identificador->identificador);
    diminuir_identacao();
}

void ImprimeVisitor::visit(Primitive* primitive) {
    aumentar_identacao();
    imprimir_identado("[PRIMITIVE]-");
    printf("( linha - %d ) ", primitive->linha);
    printf("%s", lexico_nome_token(primitive->primitivetype));
    diminuir_identacao();
}

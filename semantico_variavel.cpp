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
#include "semantico_escopo.h"
#include "semantico_variavel.h"

#include "lexico.h"


void SemanticoVariavelVisitor::visit(Program* _program) {
    //iniciar_escopo("_PROGRAMA_");
    if(_program->varlist != NULL) _program->varlist->accept(this);
    if(_program->typelist != NULL) _program->typelist->accept(this);
    if(_program->subproglist != NULL) _program->subproglist->accept(this);
    if(_program->statementlist != NULL) _program->statementlist->accept(this);
    //escopo_program = finalizar_escopo();
}

void SemanticoVariavelVisitor::visit(SubProgList* _subproglist) {

}

void SemanticoVariavelVisitor::visit(TypeList* _typelist) {

}

void SemanticoVariavelVisitor::visit(VarList* _varlist) {

}

void SemanticoVariavelVisitor::visit(Type* _type) {

}

void SemanticoVariavelVisitor::visit(If* _if) {

}

void SemanticoVariavelVisitor::visit(While* _while) {

}

void SemanticoVariavelVisitor::visit(Read* _read) {

}

void SemanticoVariavelVisitor::visit(Write* _write) {

}

void SemanticoVariavelVisitor::visit(Assign* _assign) {

}

void SemanticoVariavelVisitor::visit(StatementList* _stmtlist) {

}

void SemanticoVariavelVisitor::visit(CompoundStatement* _compoundstmt) {

}

void SemanticoVariavelVisitor::visit(Variable* _variable) {

}

void SemanticoVariavelVisitor::visit(NumberInteger* _numint) {

}

void SemanticoVariavelVisitor::visit(NumberReal* _numreal) {

}

void SemanticoVariavelVisitor::visit(Boolean* _boolean) {

}

void SemanticoVariavelVisitor::visit(Literal* _literal) {

}

void SemanticoVariavelVisitor::visit(Call* _call) {

}

void SemanticoVariavelVisitor::visit(RelationOp* _relop) {

}

void SemanticoVariavelVisitor::visit(ComparativeOp* _compop) {

}

void SemanticoVariavelVisitor::visit(Sign* _sign) {

}

void SemanticoVariavelVisitor::visit(AdditionalOp* _addop) {

}

void SemanticoVariavelVisitor::visit(MultiplicationOp* _multop) {

}

void SemanticoVariavelVisitor::visit(BooleanOp* _boolop) {

}

void SemanticoVariavelVisitor::visit(Not* _not) {

}

void SemanticoVariavelVisitor::visit(ExprList* _explist) {

}

void SemanticoVariavelVisitor::visit(Identificador* _id) {

}

void SemanticoVariavelVisitor::visit(Primitive* _primivite) {

}


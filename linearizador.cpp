#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>

#include "linearizador.h"

Linearizador::Linearizador() { }
Linearizador::~Linearizador() { }


StmtList* Linearizador::linearize(StmtList* _stmtlist){
    //printf("linearize\n");
    if(_stmtlist->stmtlist) return linear(_stmtlist->stmt, linearize(_stmtlist->stmtlist));
    else return linear(_stmtlist->stmt, NULL);
}

StmtList* Linearizador::linear(Stmt* _stmt, StmtList* _stmtlist){
    //printf("linear\n");
    if(_stmt == NULL) return _stmtlist;
    if(typeid(*_stmt) == typeid(SEQ)) return linear2((SEQ*) _stmt, _stmtlist);
    else {
        if(_stmt != NULL) return new StmtList(_stmt, _stmtlist);
        else return _stmtlist;
    }
}

StmtList* Linearizador::linear2(SEQ* _seq, StmtList* _stmtlist){
    //printf("linear2\n");
    return linear(_seq->stmt1, linear(_seq->stmt2, _stmtlist));
}

void Linearizador::visit(Fragment* _fragment) {
    if(typeid(*_fragment) == typeid(Procedure)) {
        _fragment->accept(this);
    }
    if(_fragment->proximo) _fragment->proximo->accept(this);
}

void Linearizador::visit(Procedure* _procedure){
    if(_procedure->body) _procedure->body = linearize(_procedure->body);
    if(_procedure->proximo) _procedure->proximo->accept(this);
}

void Linearizador::visit(FrameMIPS*) {};
void Linearizador::visit(LiteralRI*) {}
void Linearizador::visit(Integer*) {}
void Linearizador::visit(Float*) {}
void Linearizador::visit(BooleanRI*) {}
void Linearizador::visit(Record*) {}
void Linearizador::visit(atFrame*) {}
void Linearizador::visit(atReg*) {}
void Linearizador::visit(LocalList*) {}
void Linearizador::visit(TempList*) {}
void Linearizador::visit(Temp*) {}
void Linearizador::visit(Label*) {}
void Linearizador::visit(ExpList*) {}
void Linearizador::visit(StmtList*) {}

void Linearizador::visit(MEM*) {}
void Linearizador::visit(CALL*) {}
void Linearizador::visit(ESEQ*) {}
void Linearizador::visit(CONST*) {}
void Linearizador::visit(CONSTF*) {}
void Linearizador::visit(NAME*) {}
void Linearizador::visit(TEMP*) {}
void Linearizador::visit(BINOP*) {}

void Linearizador::visit(MOVE*) {}
void Linearizador::visit(EXP*) {}
void Linearizador::visit(JUMP*) {}
void Linearizador::visit(CJUMP*) {}
void Linearizador::visit(SEQ*) {}
void Linearizador::visit(LABEL*) {}

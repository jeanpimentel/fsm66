#ifndef LINEARIZADOR_H_INCLUDED
#define LINEARIZADOR_H_INCLUDED

#include "representacao_intermediaria.h"

class Linearizador : public VisitorRI {
public:
    Linearizador();
    ~Linearizador();

    StmtList* linearize(StmtList*);
    StmtList* linear(Stmt*, StmtList*);
    StmtList* linear2(SEQ*, StmtList*);

    void visit(FrameMIPS*);
    void visit(Fragment*);
    void visit(Procedure*);
    void visit(LiteralRI*);
    void visit(Integer*);
    void visit(Float*);
    void visit(BooleanRI*);
    void visit(Record*);
    void visit(atFrame*);
    void visit(atReg*);
    void visit(LocalList*);
    void visit(TempList*);
    void visit(Temp*);
    void visit(Label*);
    void visit(ExpList*);
    void visit(StmtList*);

    void visit(MEM*);
    void visit(CALL*);
    void visit(ESEQ*);
    void visit(CONST*);
    void visit(CONSTF*);
    void visit(NAME*);
    void visit(TEMP*);
    void visit(BINOP*);

    void visit(MOVE*);
    void visit(EXP*);
    void visit(JUMP*);
    void visit(CJUMP*);
    void visit(SEQ*);
    void visit(LABEL*);

};


#endif // LINEARIZADOR_H_INCLUDED

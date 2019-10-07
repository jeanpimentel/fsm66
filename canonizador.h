#ifndef CANONIZADOR_H_INCLUDED
#define CANONIZADOR_H_INCLUDED

#include "representacao_intermediaria.h"

#define DEBUG_CANONIZADOR_1 0
#define DEBUG_CANONIZADOR_2 0
#define DEBUG_CANONIZADOR_3 0

class Canonizador : public VisitorRI {
public:
    Canonizador();
    ~Canonizador();

    Exp* resolveESEQ(Exp*);
    Stmt* resolveESEQ(Stmt*);

    void visit(FrameMIPS*);
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

#endif // CANONIZADOR_H_INCLUDED

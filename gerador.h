
#ifndef GERADOR_H
#define	GERADOR_H

#define DEBUG_GERADOR_ASM 0

#include "representacao_intermediaria.h"

class Gerador : public VisitorRI {
public:

    FILE *assembly;
    int tempRegProx;
    int qtddArgs;
    int qtddVars;
    int tamanhoFrame;

    Gerador(char*);
    ~Gerador();

    void addCodigo(char*);

    char* getRegistrador(int);
    int retorno;
    bool lvalue;
    bool moveJaPassou;

    void munchExp(CONST *exp);
    void munch(LABEL *label);
    void munch(JUMP *jump);
    void munch(CJUMP *j);
    void munchExp(CALL *call);
    void munch(MOVE* move);
    void munchMove(TEMP* dst, Exp* src);
    void munchMove(NAME* dst, Exp* src);
    void munchMove(MEM* dst, Exp* src);
    void munch(Exp* exp);
    void munchExp(NAME *name);
    void munch(Stmt* s);
    void munchExp(MEM *mem);
    void munchExp(BINOP *binop);
    void munchExp(TEMP *temp);
    void visit(FrameMIPS*);
    void visit(Procedure*);
    void visit(LiteralRI*);
    void visit(Integer*);
    void visit(Float*);
    void visit(BooleanRI*);
    void visit(Record*);
    void visit(atFrame*);
    void visit(atReg*);

    void visit(TempList*);
    void visit(LocalList*);
    void visit(Temp*);
    void visit(Label*);
    void visit(ExpList*);
    void visit(StmtList*);

    void visit(CONST*);
    void visit(CONSTF*);
    void visit(NAME*);
    void visit(TEMP*);
    void visit(BINOP*);
    void visit(MEM*);
    void visit(CALL*);
    void visit(ESEQ*);
    void visit(MOVE*);
    void visit(EXP*);
    void visit(JUMP*);
    void visit(CJUMP*);
    void visit(SEQ*);
    void visit(LABEL*);




};


#endif


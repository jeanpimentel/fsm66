#ifndef REPRESENTACAO_INTERMEDIARIA_H
#define REPRESENTACAO_INTERMEDIARIA_H

#include "arvore_sintaxe_abstrata.h"
#include "simbolos.h"

#include "lexico.h"

#define REG_T8 16
#define REG_T9 17
#define REG_V0 50
#define REG_SP 51
#define REG_FP 52


class VisitorRI;
class Stmt;
class StmtList;
class Temp;
class Frame;
class Label;

// Fragmento de c�digo
class Fragment {
public:
    // Nome da constante ou fun��o, gerado autom�ticamente
    // no construtor
    Label* label;
    // Pr�ximo Fragmento
    Fragment* proximo;

    virtual void accept(VisitorRI*) = 0;
};


// Uma fun��o, procedimento ou o pr�prio Main
class Procedure : public Fragment {
public:
    Frame* frame;
    StmtList* body;

    Procedure(Frame*, StmtList*);
    ~Procedure();

    void accept(VisitorRI*);
};
// Aloca um espa�o para uma

class LiteralRI : public Fragment {
public:
    // Valor da constante
    char* value;

    LiteralRI(char*);
    ~LiteralRI();

    void accept(VisitorRI*);
};
// Guarda um array de

class Integer : public Fragment {
public:
    int nElements;
    Integer(int);
    ~Integer();

    void accept(VisitorRI*);
};

class Float : public Fragment {
public:
    int nElements;
    Float(int);
    ~Float();

    void accept(VisitorRI*);
};

class BooleanRI : public Fragment {
public:
    int nElements;
    BooleanRI(int);
    ~BooleanRI();

    void accept(VisitorRI*);
};

class Record : public Fragment {
public:
    int nBytes;
    int nElements;
    Record(int, int);
    ~Record();

    void accept(VisitorRI*);
};

// Define como ser� o acesso a um dado local
class Local {
public:
    // retorna o c�digo de m�quina p/ acessar o nome
    virtual Stmt* getCode() = 0;
    virtual void accept(VisitorRI*) = 0;
};

class Frame {
public:
    Label* rotulo;
    Temp* fp;
    virtual void accept(VisitorRI*) = 0;
};

// p/ aloca��o no frame

class atFrame : public Local {
public:
    int offSet;
    atFrame(int);
    ~atFrame();
    Stmt* getCode();
    void accept(VisitorRI*);
};

//p/ aloca��o em Registradores

class atReg : public Local {
public:
    Temp* reg;
    atReg(Temp*);
    ~atReg();
    Stmt* getCode();
    void accept(VisitorRI*);
};

class LocalList {
public:
    Local* data;
    LocalList* prox;
    LocalList(Local*, LocalList*);
    ~LocalList();
    void accept(VisitorRI*);
};

class FrameMIPS : public Frame {
public:
    Label* rotulo;
    Temp* regRetorno;
    LocalList* local;

    FrameMIPS(Label*,Temp*,LocalList*);
    FrameMIPS();
    ~FrameMIPS();
    Local* addParametro(int, int);
    Local* addLocal(int, int);
    Local* getVar(){
    }
    void accept(VisitorRI*);
};

class Exp {
public:
    Exp();
    ~Exp();
    virtual void accept(VisitorRI* v) = 0;
};

class ExpList {
public:
    Exp* exp;
    ExpList* explist;

    ExpList(Exp* exp, ExpList* explist);
    ~ExpList();
    void accept(VisitorRI* v);
};

class Stmt {
public:
    Stmt();
    ~Stmt();
    virtual void accept(VisitorRI* v) = 0;
};

class StmtList {
public:
    Stmt* stmt;
    StmtList* stmtlist;

    StmtList(Stmt* stmt, StmtList* stmtlist);
    ~StmtList();
    void accept(VisitorRI* v);
};

class Label {
public:
    char* label;

    Label(int tipo);
    Label(char* label);
    ~Label();
    void accept(VisitorRI* v);
};

class Temp {
public:
    char* temp;
    int travado;
    Temp();
    Temp(char* temp);
    ~Temp();
    void accept(VisitorRI* v);
};

class MEM : public Exp {
public:
    Exp* exp;

    MEM(Exp* exp);
    ~MEM();
    void accept(VisitorRI* v);
};

class CALL : public Exp {
public:
    Exp* func;
    ExpList* args;
    bool canonizado;

    CALL(Exp* func, ExpList* args);
    ~CALL();
    void accept(VisitorRI* v);
};

class ESEQ : public Exp {
public:
    Stmt* stmt;
    Exp* exp;

    ESEQ(Stmt* stmt, Exp* exp);
    ~ESEQ();
    void accept(VisitorRI* v);
};

class CONST : public Exp {
public:
    int i;

    CONST(int i);
    ~CONST();
    void accept(VisitorRI* v);
};

class CONSTF : public Exp {
public:
    float f;

    CONSTF(float f);
    ~CONSTF();
    void accept(VisitorRI* v);
};

class NAME : public Exp {
public:
    Label* label;

    NAME(Label* label);
    ~NAME();
    void accept(VisitorRI* v);
};

class TEMP : public Exp {
public:
    Temp* temp;

    TEMP(Temp* temp);
    ~TEMP();
    void accept(VisitorRI* v);
};

class BINOP : public Exp {
public:
    int op;
    Exp* exp1;
    Exp* exp2;

    BINOP(int op, Exp* exp1, Exp* exp2);
    ~BINOP();
    void accept(VisitorRI* v);
};

class MOVE : public Stmt {
public:
    Exp* dst;
    Exp* exp;

    MOVE(Exp* dst, Exp* exp);
    ~MOVE();
    void accept(VisitorRI* v);
};

class EXP : public Stmt {
public:
    Exp* exp;

    EXP(Exp* exp);
    ~EXP();
    void accept(VisitorRI* v);
};

class JUMP : public Stmt {
public:
    Exp* exp;

    JUMP(Exp* exp);
    ~JUMP();
    void accept(VisitorRI* v);
};

class CJUMP : public Stmt {
public:
    int op;
    Exp* exp1;
    Exp* exp2;
    Label* ltrue;
    Label* lfalse;

    CJUMP(int op, Exp* exp1, Exp* exp2, Label* ltrue, Label* lfalse);
    ~CJUMP();
    void accept(VisitorRI* v);
};

class SEQ : public Stmt {
public:
    Stmt* stmt1;
    Stmt* stmt2;

    SEQ(Stmt* stmt1, Stmt* stmt2);
    ~SEQ();
    void accept(VisitorRI* v);
};

class LABEL : public Stmt {
public:
    Label* label;

    LABEL(Label* label);
    ~LABEL();
    void accept(VisitorRI* v);
};

class TempList {
public:
    Temp *temp;
    TempList *proximo;
    TempList(Temp *temp, TempList *proximo);
    ~TempList();
    void accept(VisitorRI *v);
};

class VisitorRI {
public:

    virtual void visit(FrameMIPS*) = 0;
    virtual void visit(Procedure*) = 0;
    virtual void visit(LiteralRI*) = 0;
    virtual void visit(Integer*) = 0;
    virtual void visit(Float*) = 0;
    virtual void visit(BooleanRI*) = 0;
    virtual void visit(Record*) = 0;
    virtual void visit(atFrame*) = 0;
    virtual void visit(atReg*) = 0;

    virtual void visit(LocalList*) = 0;
    virtual void visit(TempList*) = 0;
    virtual void visit(Temp*) = 0;
    virtual void visit(Label*) = 0;
    virtual void visit(ExpList*) = 0;
    virtual void visit(StmtList*) = 0;

    virtual void visit(CONST*) = 0;
    virtual void visit(CONSTF*) = 0;
    virtual void visit(NAME*) = 0;
    virtual void visit(TEMP*) = 0;
    virtual void visit(BINOP*) = 0;
    virtual void visit(MEM*) = 0;
    virtual void visit(CALL*) = 0;
    virtual void visit(ESEQ*) = 0;
    virtual void visit(MOVE*) = 0;
    virtual void visit(EXP*) = 0;
    virtual void visit(JUMP*) = 0;
    virtual void visit(CJUMP*) = 0;
    virtual void visit(SEQ*) = 0;
    virtual void visit(LABEL*) = 0;
};

class ImprimeVisitorRI : public VisitorRI {
public:
    int aninhamento;

    void aumentar_identacao();
    void diminuir_identacao();
    void imprimir_identado(const char* no);

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

#endif // REPRESENTACAO_INTERMEDIARIA_H

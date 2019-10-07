#include "representacao_intermediaria.h"

static unsigned int label_contador = 0;

Procedure::Procedure(Frame *frame, StmtList *body) {
  this->frame = frame;
  this->body = body;
  this->proximo = NULL;
}
Procedure::~Procedure() {
  if (frame) delete(frame);
  if (body) delete(body);
}
void Procedure::accept(VisitorRI* v) {
    v->visit(this);
}

LiteralRI::LiteralRI(char *value) {
  this->value = value;
  this->label = new Label(LITERAL);
}
LiteralRI::~LiteralRI() {
  if (value) free(value);
  delete this->label;
}
void LiteralRI::accept(VisitorRI* v) {
    v->visit(this);
}

Integer::Integer(int nElements) {
  this->nElements = nElements;
  this->label = new Label(INTEIRO);
  this->proximo = NULL;
}
Integer::~Integer() {
    delete this->label;
}
void Integer::accept(VisitorRI* v) {
    v->visit(this);
}

Float::Float(int nElements) {
  this->nElements = nElements;
  this->label = new Label(REAL);
  this->proximo = NULL;
}
Float::~Float() {
    delete this->label;
}
void Float::accept(VisitorRI* v) {
    v->visit(this);
}

BooleanRI::BooleanRI(int nElements) {
  this->nElements = nElements;
  this->label = new Label(LOGICO);
  this->proximo = NULL;
}
BooleanRI::~BooleanRI() {
    delete this->label;
}
void BooleanRI::accept(VisitorRI* v) {
    v->visit(this);
}

Record::Record(int nBytes, int nElements) {
  this->nElements = nElements;
  this->nBytes = nBytes;
  this->label = new Label(REGISTRO);
  this->proximo = NULL;
}
Record::~Record() {
    delete this->label;
}
void Record::accept(VisitorRI* v) {
    v->visit(this);
}

atFrame::atFrame(int offSet){
    this->offSet = offSet;

}
atFrame::~atFrame(){}
Stmt* atFrame::getCode(){return NULL;}
void atFrame::accept(VisitorRI* v) {
    v->visit(this);
}

atReg::atReg(Temp* reg){
    this->reg = reg;
}
atReg::~atReg(){}
Stmt* atReg::getCode(){return NULL;}
void atReg::accept(VisitorRI* v) {
    v->visit(this);
}

LocalList::LocalList(Local* data, LocalList* prox){
    this->data = data;
    this->prox = prox;
}
LocalList::~LocalList(){
    delete data;
    delete prox;
}
void LocalList::accept(VisitorRI* v){
    v->visit(this);
}

Exp::Exp() {}
Exp::~Exp() {}

ExpList::ExpList(Exp* _exp, ExpList* _explist) {
    exp = _exp;
    explist = _explist;
}
ExpList::~ExpList() {}
void ExpList::accept(VisitorRI* v) {
    v->visit(this);
}

Stmt::Stmt() {}
Stmt::~Stmt() {}

StmtList::StmtList(Stmt* _stmt, StmtList* _stmtlist) {
    stmt = _stmt;
    stmtlist = _stmtlist;
}
StmtList::~StmtList() {}
void StmtList::accept(VisitorRI* v) {
    v->visit(this);
}

Label::Label(char* _label) {
    label = _label;
}
Label::Label(int tipo){
    label_contador++;
    this->label = (char*)malloc(50*sizeof(char));
    //strcpy(nome, lexico_nome_token(tipo));
    //strcpy(nome, (const char*)itoa(contador));
    sprintf(this->label, "%s_%d", lexico_nome_token(tipo), label_contador);
}
Label::~Label() {}
void Label::accept(VisitorRI* v) {
    v->visit(this);
}

Temp::Temp() {
    temp = (char*) "?";
    travado = 0;
}
Temp::Temp(char* _temp) {
    temp = _temp;
    travado = 0;
}
Temp::~Temp() {}
void Temp::accept(VisitorRI* v) {
    v->visit(this);
}

MEM::MEM(Exp* _exp) {
    exp = _exp;
}
MEM::~MEM() {}
void MEM::accept(VisitorRI* v) {
    v->visit(this);
}

CALL::CALL(Exp* _func, ExpList* _args) {
    func = _func;
    args = _args;
    canonizado = false;
}
CALL::~CALL() {}
void CALL::accept(VisitorRI* v) {
    v->visit(this);
}

ESEQ::ESEQ(Stmt* _stmt, Exp* _exp) {
    stmt = _stmt;
    exp = _exp;
}
ESEQ::~ESEQ() {}
void ESEQ::accept(VisitorRI* v) {
    v->visit(this);
}

CONST::CONST(int _i) {
    i = _i;
}
CONST::~CONST() {}
void CONST::accept(VisitorRI* v) {
    v->visit(this);
}

CONSTF::CONSTF(float _f) {
    f = _f;
}
CONSTF::~CONSTF() {}
void CONSTF::accept(VisitorRI* v) {
    v->visit(this);
}

NAME::NAME(Label* _label) {
    label = _label;
}
NAME::~NAME() {}
void NAME::accept(VisitorRI* v) {
    v->visit(this);
}

TEMP::TEMP(Temp* _temp) {
    temp = _temp;
}
TEMP::~TEMP() {}
void TEMP::accept(VisitorRI* v) {
    v->visit(this);
}

BINOP::BINOP(int _op, Exp* _exp1, Exp* _exp2) {
    op = _op;
    exp1 = _exp1;
    exp2 = _exp2;
}
BINOP::~BINOP() {}
void BINOP::accept(VisitorRI* v) {
    v->visit(this);
}

MOVE::MOVE(Exp* _dst, Exp* _exp) {
    dst = _dst;
    exp = _exp;
}
MOVE::~MOVE() {}
void MOVE::accept(VisitorRI* v) {
    v->visit(this);
}

EXP::EXP(Exp* _exp) {
    exp = _exp;
}
EXP::~EXP() {}
void EXP::accept(VisitorRI* v) {
    v->visit(this);
}

JUMP::JUMP(Exp* _exp) {
    exp = _exp;
}
JUMP::~JUMP() {}
void JUMP::accept(VisitorRI* v) {
    v->visit(this);
}

CJUMP::CJUMP(int _op, Exp* _exp1, Exp* _exp2, Label* _ltrue, Label* _lfalse) {
    op = _op;
    exp1 = _exp1;
    exp2 = _exp2;
    ltrue = _ltrue;
    lfalse = _lfalse;
}
CJUMP::~CJUMP() {}
void CJUMP::accept(VisitorRI* v) {
    v->visit(this);
}

SEQ::SEQ(Stmt* _stmt1, Stmt* _stmt2) {
    stmt1 = _stmt1;
    stmt2 = _stmt2;
}
SEQ::~SEQ() {}
void SEQ::accept(VisitorRI* v) {
    v->visit(this);
}

LABEL::LABEL(Label* _label) {
    label = _label;
}
LABEL::~LABEL() {}
void LABEL::accept(VisitorRI* v) {
    v->visit(this);
}

TempList::TempList(Temp *temp, TempList *proximo){
    this->temp = temp;
    this->proximo = proximo;
}
TempList::~TempList(){
    delete temp;
    delete proximo;
}
void TempList::accept(VisitorRI* v) {
    v->visit(this);
}


FrameMIPS::FrameMIPS(Label* rotulo,Temp* regRetorno, LocalList* local){
    this->fp = new Temp((char*)"$fp");
    this->rotulo    = rotulo;
    this->regRetorno= regRetorno;
    this->local     = local;

}
FrameMIPS::FrameMIPS(){
    this->fp = new Temp((char*)"$fp");
    this->rotulo    = NULL;
    this->regRetorno= new Temp((char*)"$V0");
    this->local     = NULL;
}

FrameMIPS::~FrameMIPS() {
    delete this->fp;
    delete this->rotulo;
    delete this->regRetorno;
    delete this->local;
}

void FrameMIPS::accept(VisitorRI* v) {
    v->visit(this);
}



//inicio imprime visitor ri

/** @brief  Aumenta a identacao durante a impressao da arvore. */
void ImprimeVisitorRI::aumentar_identacao() {
    aninhamento += 4;
}

/** @brief  Diminui a identacao durante a impressao da arvore. */
void ImprimeVisitorRI::diminuir_identacao() {
    aninhamento -= 4;
}

/**
 * @brief  Imprime as informacoes da arvore de forma identada.
 * @param  texto Informacao a ser impressa.
 */
void ImprimeVisitorRI::imprimir_identado(const char* texto) {
    int i;
    printf("\n");
    for (i = 0; i < aninhamento; i++) printf(" ");
    printf("%s", texto);
}


void ImprimeVisitorRI::visit(FrameMIPS* _frame){
        aumentar_identacao();
        imprimir_identado("[FrameMIPS]");
        if(_frame->rotulo)       _frame->rotulo->accept(this);
        if(_frame->regRetorno)   _frame->regRetorno->accept(this);
        if(_frame->local)        _frame->local->accept(this);
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(Procedure* _proc){
        aumentar_identacao();
        imprimir_identado("[Procedure]");
        if(_proc->label)   _proc->label->accept(this);
        if(_proc->frame)   _proc->frame->accept(this);
        if(_proc->body)    _proc->body->accept(this);
        if(_proc->proximo) _proc->proximo->accept(this);
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(LiteralRI* _literalRI){
        aumentar_identacao();
        imprimir_identado("[Literal]");
        printf("- %s",_literalRI->value);
        if(_literalRI->label != NULL) _literalRI->label->accept(this);
        if(_literalRI->proximo != NULL) _literalRI->proximo->accept(this);
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(Integer* _int){
        aumentar_identacao();
        imprimir_identado("[Integer]");
        printf("- Dimensao - %d",_int->nElements!=0?_int->nElements:1);
        if(_int->label != NULL) _int->label->accept(this);
        if(_int->proximo != NULL) _int->proximo->accept(this);
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(Float* _float){
        aumentar_identacao();
        imprimir_identado("[Float]");
        printf("- Dimensao - %d",_float->nElements!=0?_float->nElements:1);
        if(_float->label != NULL) _float->label->accept(this);
        if(_float->proximo != NULL) _float->proximo->accept(this);
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(BooleanRI* _bool){
        aumentar_identacao();
        imprimir_identado("[Boolean]");
        printf("- Dimensao - %d",_bool->nElements!=0?_bool->nElements:1);
        if(_bool->label != NULL) _bool->label->accept(this);
        if(_bool->proximo != NULL) _bool->proximo->accept(this);
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(Record* _rec){
        aumentar_identacao();
        imprimir_identado("[Record]");
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(atFrame* _atframe){
        aumentar_identacao();
        imprimir_identado("[atFRAME]");
        printf("offset: %d ",_atframe->offSet);

        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(atReg* _atreg){
        aumentar_identacao();
        imprimir_identado("[atREG]");
        _atreg->reg->accept(this);
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(LocalList* _locallist){
        aumentar_identacao();
        imprimir_identado("[LocalList]");
        _locallist->data->accept(this);
        if(_locallist->prox != NULL){
            _locallist->prox->accept(this);
        }
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(TempList* _templist){
        aumentar_identacao();
        imprimir_identado("[TempList]");
        _templist->temp->accept(this);
        if(_templist->proximo != NULL){
            _templist->proximo->accept(this);
        }
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(Temp* _temp){
        aumentar_identacao();
        imprimir_identado("[Temp]");
        printf("- %s - %d", _temp->temp, _temp->travado);
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(Label* _label){
        aumentar_identacao();
        imprimir_identado("[Label]");
        printf("- %s", _label->label);
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(ExpList* _explist){
        aumentar_identacao();
        imprimir_identado("[ExpList]");
        _explist->exp->accept(this);
        if(_explist->explist != NULL){
            _explist->explist->accept(this);
        }
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(StmtList* _stmtlist){
        aumentar_identacao();
        imprimir_identado("[StmtList]");
        _stmtlist->stmt->accept(this);
        if(_stmtlist->stmtlist != NULL){
            _stmtlist->stmtlist->accept(this);
        }
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(CONST* _const){
        aumentar_identacao();
        imprimir_identado("[CONST]");
        printf("- %d", _const->i);
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(CONSTF* _constf){
        aumentar_identacao();
        imprimir_identado("[CONSTF]");
        printf("- %f", _constf->f);
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(NAME* _name){
        aumentar_identacao();
        imprimir_identado("[NAME]");
        _name->label->accept(this);
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(TEMP* _temp){
        aumentar_identacao();
        imprimir_identado("[TEMP]");
        _temp->temp->accept(this);
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(BINOP* _binop){
        aumentar_identacao();
        imprimir_identado("[BINOP]");
        aumentar_identacao();
        imprimir_identado(lexico_nome_token(_binop->op));
        diminuir_identacao();
        _binop->exp1->accept(this);
        _binop->exp2->accept(this);
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(MEM* _mem){
        aumentar_identacao();
        imprimir_identado("[MEM]");
        _mem->exp->accept(this);
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(CALL* _call){
        aumentar_identacao();
        imprimir_identado("[CALL]");
        _call->func->accept(this);
        if(_call->args != NULL){
            _call->args->accept(this);
        }
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(ESEQ* _eseq){
        aumentar_identacao();
        imprimir_identado("[ESEQ]");
        _eseq->stmt->accept(this);
        _eseq->exp->accept(this);
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(MOVE* _move){
        aumentar_identacao();
        imprimir_identado("[MOVE]");
        _move->dst->accept(this);
        _move->exp->accept(this);
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(EXP* _exp){
        aumentar_identacao();
        imprimir_identado("[EXP]");
        _exp->exp->accept(this);
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(JUMP* _jump){
        aumentar_identacao();
        imprimir_identado("[JUMP]");
        _jump->exp->accept(this);
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(CJUMP* _cjump){
        aumentar_identacao();
        imprimir_identado("[CJUMP]");
        printf("- %s", lexico_nome_token(_cjump->op));
        if(_cjump->exp1 != NULL){
            _cjump->exp1->accept(this);
        }
        if(_cjump->exp2 != NULL){
            _cjump->exp2->accept(this);
        }
        if(_cjump->ltrue != NULL){
            _cjump->ltrue->accept(this);
        }
        if(_cjump->lfalse != NULL){
            _cjump->lfalse->accept(this);
        }
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(SEQ* _seq){
        aumentar_identacao();
        imprimir_identado("[SEQ]");
        if(_seq->stmt1 != NULL){
            _seq->stmt1->accept(this);
        }
        if(_seq->stmt2 != NULL){
            _seq->stmt2->accept(this);
        }
        diminuir_identacao();
    }

void ImprimeVisitorRI::visit(LABEL* _label){
        aumentar_identacao();
        imprimir_identado("[LABEL]");
        _label->label->accept(this);
        diminuir_identacao();
    }

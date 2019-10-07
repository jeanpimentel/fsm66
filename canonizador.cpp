#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>

#include "canonizador.h"

bool foiModificado = false;

bool isNop(Stmt* _stmt) {
  if(typeid(*_stmt) == typeid(EXP)) {
    if((typeid(((EXP*)_stmt)->exp) == typeid(CONST)) || (typeid(((EXP*)_stmt)->exp) == typeid(CONSTF))) return true;
    else return false;
  }
  return false;
}

bool comuta(Exp* _exp, Stmt* _stmt) {
    if( isNop(_stmt) || typeid(*_exp) == typeid(NAME) || typeid(*_exp) == typeid(CONST) || typeid(*_exp) == typeid(CONSTF)) return true;
    return false;
}

Canonizador::Canonizador() { }
Canonizador::~Canonizador() { }

Exp* Canonizador::resolveESEQ(Exp* _exp){

    if(typeid(*_exp) == typeid(ESEQ)) {
        ESEQ* eseq = (ESEQ*) _exp;
        if(typeid(*eseq->exp) == typeid(ESEQ)) {
            ESEQ* eseq2 = (ESEQ*) eseq->exp;
            SEQ* seq = new SEQ(eseq->stmt, eseq2->stmt);
            eseq2->stmt = seq;
            foiModificado = true;
            if(DEBUG_CANONIZADOR_3) printf("--> CANONIZANDO: ESEQ-ESEQ\n");
            return eseq2;
        }
    }

    else if(typeid(*_exp) == typeid(BINOP)) {
        BINOP* binop = (BINOP*) _exp;

        if(typeid(*binop->exp1) == typeid(ESEQ)) {
            ESEQ* eseq1 = (ESEQ*) binop->exp1;
            Exp *aux = eseq1->exp;
            binop->exp1 = aux;
            eseq1->exp = binop;
            foiModificado = true;
            if(DEBUG_CANONIZADOR_3) printf("--> CANONIZANDO: BINOP-ESEQ-ESQ\n");
            return eseq1;
        }

        if(typeid(*binop->exp2) == typeid(ESEQ)) {
            ESEQ* eseq2 = (ESEQ*) binop->exp2;

            if(comuta(binop->exp1, eseq2->stmt)) {
                Exp *aux = eseq2->exp;
                binop->exp1 = aux;
                eseq2->exp = binop;
                foiModificado = true;
                if(DEBUG_CANONIZADOR_3) printf("--> CANONIZANDO: BINOP-ESEQ-DIR\n");
                return eseq2;
            }
            else {
                Temp *temp = new Temp();
                temp->travado = REG_T9;
                ESEQ *eseq3 = new ESEQ(new MOVE(new TEMP(temp),binop->exp1), eseq2);
                binop->exp1 = new TEMP(temp);
                binop->exp2 = eseq2->exp;
                eseq2->exp = binop;
                foiModificado = true;
                if(DEBUG_CANONIZADOR_3) printf("--> CANONIZANDO: BINOP-ESEQ-DIR\n");
                return eseq3;
            }
        }

    }

    else if(typeid(*_exp) == typeid(MEM)) {
        MEM* mem = (MEM*) _exp;
        if(typeid(*mem->exp) == typeid(ESEQ)) {
            ESEQ* eseq1 = (ESEQ*) mem->exp;
            Exp* aux = eseq1->exp;
            mem->exp = aux;
            eseq1->exp = mem;
            foiModificado = true;
            if(DEBUG_CANONIZADOR_3) printf("--> CANONIZANDO: MEM-ESEQ\n");
            return eseq1;
        }

    }

//    else if(typeid(*_exp) == typeid(CALL)) {
////        CALL* call = (CALL*) _exp;
////        if(!call->canonizado) {
////            Temp *temp = new Temp();
////            temp->travado = REG_T9;
////            ESEQ *eseq1 = new ESEQ(new MOVE(new TEMP(temp), call), new TEMP(temp));
////            foiModificado = true;
////            if(DEBUG_CANONIZADOR_3) printf("--> CANONIZANDO: CALL-ESEQ\n");
////            call->canonizado = true;
////            return eseq1;
////        }
//    }

    return _exp;
}

Stmt* Canonizador::resolveESEQ(Stmt* _stmt) {

    if(typeid(*_stmt) == typeid(CJUMP)) {
        CJUMP* cjump = (CJUMP*) _stmt;

        if(typeid(*cjump->exp1) == typeid(ESEQ)) {
            ESEQ* eseq1 = (ESEQ*) cjump->exp1;
            cjump->exp1 = eseq1->exp;
            SEQ* seq1 = new SEQ(eseq1->stmt, cjump);
            foiModificado = true;
            if(DEBUG_CANONIZADOR_3) printf("--> CANONIZANDO: CJUMP-ESEQ-ESQ\n");
            return seq1;
        }

        if(typeid(*cjump->exp2) == typeid(ESEQ)) {
            ESEQ* eseq2 = (ESEQ*) cjump->exp2;

            if(comuta(cjump->exp1,eseq2->stmt)){
                cjump->exp1 = eseq2->exp;
                SEQ *seq1 = new SEQ(eseq2->stmt, cjump);
                foiModificado = true;
                if(DEBUG_CANONIZADOR_3) printf("--> CANONIZANDO: CJUMP-ESEQ-DIR\n");
                return seq1;
            }
            else {
                Temp *temp = new Temp();
                temp->travado = REG_T9;
                cjump->exp2 = eseq2->exp;
                SEQ *seq1 = new SEQ(eseq2->stmt, cjump);
                SEQ *seq2 = new SEQ(new MOVE(new TEMP(temp), cjump->exp1), seq1);
                cjump->exp1 = new TEMP(temp);
                foiModificado = true;
                if(DEBUG_CANONIZADOR_3) printf("--> CANONIZANDO: CJUMP-ESEQ-DIR\n");
                return seq2;
            }

        }

    }

    else if(typeid(*_stmt) == typeid(MOVE)) {
        MOVE* move = (MOVE*) _stmt;

        if(typeid(*move->dst) == typeid(ESEQ)) {
            ESEQ* eseq1 = (ESEQ*) move->dst;
            move->dst = eseq1->exp;
            SEQ *seq1 = new SEQ(eseq1->stmt, move);
            foiModificado = true;
            if(DEBUG_CANONIZADOR_3) printf("--> CANONIZANDO: MOVE-ESEQ-DST\n");
            return seq1;
        }

        if(typeid(*move->exp) == typeid(ESEQ)) {
            ESEQ* eseq2 = (ESEQ*) move->exp;
            Temp *t = new Temp();
            t->travado = REG_T9;
            SEQ *seq1 = new SEQ(new MOVE(new TEMP(t), move->dst), new SEQ(eseq2->stmt, new MOVE(new TEMP(t), eseq2->exp)));
            foiModificado = true;
            if(DEBUG_CANONIZADOR_3) printf("--> CANONIZANDO: MOVE-ESEQ-EXP\n");
            return seq1;
        }

    }

    else if(typeid(*_stmt) == typeid(JUMP)) {
        JUMP* jump = (JUMP*) _stmt;
        if(typeid(*jump->exp) == typeid(ESEQ)) {
            ESEQ* eseq1 = (ESEQ*) jump->exp;
            jump->exp = eseq1->exp;
            SEQ *seq1 = new SEQ(eseq1->stmt, jump);
            foiModificado = true;
            if(DEBUG_CANONIZADOR_3) printf("--> CANONIZANDO: JUMP-ESEQ\n");
            return seq1;
        }
    }

    else if(typeid(*_stmt) == typeid(EXP)) {
        EXP* exp = (EXP*) _stmt;
        if(typeid(*exp->exp) == typeid(ESEQ)) {
            ESEQ* eseq1 = (ESEQ*) exp->exp;
            SEQ* seq1 = new SEQ(eseq1->stmt, new EXP(eseq1->exp));
            foiModificado = true;
            if(DEBUG_CANONIZADOR_3) printf("--> CANONIZANDO: EXP-ESEQ\n");
            return seq1;
        }
    }

    return _stmt;

}

void Canonizador::visit(FrameMIPS* _framemips) {
    if(DEBUG_CANONIZADOR_1) printf("framemips!\n");
    if (_framemips->rotulo) _framemips->rotulo->accept(this);
	if (_framemips->local) _framemips->local->accept(this);
}

void Canonizador::visit(Procedure* _procedure) {
    if(DEBUG_CANONIZADOR_1) printf("\n\nprocedure!\n\n\n");
    int i = 1;
    do {
        if(DEBUG_CANONIZADOR_3) printf("\n\n-=-=-=- %da passagem na RI -=-=-=-\n\n", i);
        foiModificado = false;
        _procedure->frame->accept(this);
        _procedure->body->accept(this);
        i++;
    } while(foiModificado);

    if(_procedure->proximo) _procedure->proximo->accept(this);
}

void Canonizador::visit(LiteralRI* _literal) {}
void Canonizador::visit(Integer* _integer) {}
void Canonizador::visit(Float* _float) {}
void Canonizador::visit(BooleanRI* _boolean) {}
void Canonizador::visit(Record* _record) {}
void Canonizador::visit(atFrame* _atframe) {}
void Canonizador::visit(atReg* _atreg) {}
void Canonizador::visit(LocalList* _locallist) {}
void Canonizador::visit(TempList* _templist) {}
void Canonizador::visit(Temp* _temp) {}
void Canonizador::visit(Label* _label) {}


void Canonizador::visit(ExpList* _explist) {
    if(DEBUG_CANONIZADOR_1) printf("explist!\n");
    if(_explist->exp) {
        //printf("entrando em explist\n");
        _explist->exp = resolveESEQ(_explist->exp);
        //resolveESEQ((Stmt*)_explist->exp);
        _explist->exp->accept(this);
    }
    if(_explist->explist) _explist->explist->accept(this);
}

void Canonizador::visit(StmtList* _stmtlist) {
    if(DEBUG_CANONIZADOR_1) printf("stmtlist!\n");
    if(_stmtlist->stmt) {
        _stmtlist->stmt = resolveESEQ(_stmtlist->stmt);
        _stmtlist->stmt->accept(this);
    }
    if(_stmtlist->stmtlist) _stmtlist->stmtlist->accept(this);
}





void Canonizador::visit(MEM* _mem) {
    if(DEBUG_CANONIZADOR_1) printf("mem!\n");
    if(_mem->exp) {
		_mem->exp = resolveESEQ(_mem->exp);
		_mem->exp->accept(this);
	}
}

void Canonizador::visit(CALL* _call) {
    if(DEBUG_CANONIZADOR_1) printf("call!\n");
    if (_call->func) {
		_call->func = resolveESEQ(_call->func);
		_call->func->accept(this);
	}
    if (_call->args) _call->args->accept(this);
}

void Canonizador::visit(ESEQ* _eseq) {
    if(DEBUG_CANONIZADOR_1) printf("eseq!\n");
    if(_eseq->stmt) {
		_eseq->stmt = resolveESEQ(_eseq->stmt);
		_eseq->stmt->accept(this);
	}
	if(_eseq->exp) {
		_eseq->exp = resolveESEQ(_eseq->exp);
		_eseq->exp->accept(this);
	}
}

void Canonizador::visit(CONST* _const) {
    if(DEBUG_CANONIZADOR_1) printf("CONST: \t--> %d\n", _const->i);
}

void Canonizador::visit(CONSTF* _constf) {
    if(DEBUG_CANONIZADOR_1) printf("CONSTF: \t--> %f\n", _constf->f);
}

void Canonizador::visit(NAME* _name) {
    if(DEBUG_CANONIZADOR_1) printf("name!\n");
    if(_name->label) _name->label->accept(this);
}

void Canonizador::visit(TEMP* _temp) {
    if(DEBUG_CANONIZADOR_1) printf("TEMP: \t--> %s\n", _temp->temp->temp);
    if(_temp->temp) _temp->temp->accept(this);
}

void Canonizador::visit(BINOP* _binop) {
    if(DEBUG_CANONIZADOR_1) printf("binop! \t--> %s\n", lexico_nome_token(_binop->op));
    if (_binop->exp1) {
		_binop->exp1 = resolveESEQ(_binop->exp1);
		_binop->exp1->accept(this);
	}
	if (_binop->exp2) {
		_binop->exp2 = resolveESEQ(_binop->exp2);
        _binop->exp2->accept(this);
	}
}








void Canonizador::visit(MOVE* _move) {
    if(DEBUG_CANONIZADOR_1) printf("move!\n");
    if (_move->dst) {
		_move->dst = resolveESEQ(_move->dst);
		_move->dst->accept(this);
	}
	if (_move->exp) {
		_move->exp = resolveESEQ(_move->exp);
		_move->exp->accept(this);
	}
}

void Canonizador::visit(EXP* _exp) {
    if(DEBUG_CANONIZADOR_1) printf("exp!\n");
    if (_exp->exp) {
		_exp->exp = resolveESEQ(_exp->exp);
		_exp->exp->accept(this);
	}
}

void Canonizador::visit(JUMP* _jump) {
    if(DEBUG_CANONIZADOR_1) printf("jump!\n");
    if (_jump->exp) {
		_jump->exp = resolveESEQ(_jump->exp);
		_jump->exp->accept(this);
	}
}

void Canonizador::visit(CJUMP* _cjump) {
    if(DEBUG_CANONIZADOR_1) printf("cjump!\n");
    if (_cjump->exp1) {
		_cjump->exp1 = resolveESEQ(_cjump->exp1);
        _cjump->exp1->accept(this);
	}
	if (_cjump->exp2) {
		_cjump->exp2 = resolveESEQ(_cjump->exp2);
        _cjump->exp2->accept(this);
	}
    if (_cjump->ltrue) _cjump->ltrue->accept(this);
    if (_cjump->lfalse) _cjump->lfalse->accept(this);
}

void Canonizador::visit(SEQ* _seq) {
    if(DEBUG_CANONIZADOR_1) printf("seq!\n");
    if (_seq->stmt1) {
		_seq->stmt1 = resolveESEQ(_seq->stmt1);
        _seq->stmt1->accept(this);
	}
	if (_seq->stmt2) {
		_seq->stmt2 = resolveESEQ(_seq->stmt2);
        _seq->stmt2->accept(this);
	}
}

void Canonizador::visit(LABEL* _label) {
    if(DEBUG_CANONIZADOR_1) printf("LABEL: \t--> %s\n", _label->label->label);
    if(_label->label) _label->label->accept(this);
}

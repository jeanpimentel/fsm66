#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>

#include "gerador.h"

#include "lexico.h"
#include "erro.h"

#define MAX_CONST_SIZE 256

Gerador::Gerador(char* nomearquivo) {
    assembly = fopen((const char*) nomearquivo, "wb");
    if (assembly == NULL) {
        fprintf(stderr, "Não foi possível gerar o arquivo.");
        exit(-1);
    }
    // Nenhum registradir utilizado
    // O próximo é o Zero.
    this->tempRegProx = -1;
    this->qtddArgs     = 5;
    this->qtddVars     = 24;
    this->tamanhoFrame = 124;
    this->lvalue = false;
    this->moveJaPassou = false;

}

Gerador::~Gerador() {
    fclose(assembly);
}

void Gerador::addCodigo(char* codigo) {
    if(DEBUG_GERADOR_ASM) printf("%s\n", codigo);
    fprintf(assembly, (const char*) codigo);
    fprintf(assembly, "\n");
}

char* Gerador::getRegistrador(int temp) {

    // Alocando o nome do registrador
    char* nomelabel = (char*) malloc(sizeof (char) * MAX_CONST_SIZE);

    if(temp == REG_V0) { sprintf(nomelabel, "$v0"); return nomelabel; }
    if(temp == REG_FP) { sprintf(nomelabel, "$fp"); return nomelabel; }
    if(temp == REG_SP) { sprintf(nomelabel, "$sp"); return nomelabel; }

    // Temos registradores do 0 ao 13
    if (temp == 18) {
        fprintf(stderr, "Ooops! Todos os temporários foram gastos...");
        exit(-1);
    }
    if(temp < 10) sprintf(nomelabel, "$t%d", temp);
    else sprintf(nomelabel, "$s%d", temp-10);

    return nomelabel;
}

void Gerador::munchExp(CONST *exp) {
    tempRegProx++;
    char cons[MAX_CONST_SIZE];
    sprintf(cons, "    li %s, %d", this->getRegistrador(this->tempRegProx), exp->i);
    this->addCodigo(cons);
    retorno = this->tempRegProx;
}

void Gerador::munch(LABEL *label) {
    if(DEBUG_GERADOR_ASM) printf("\t\t---->label\n");
    char cons[MAX_CONST_SIZE];
    sprintf(cons, "\n%s:", label->label->label);
    this->addCodigo(cons);
}

void Gerador::munch(JUMP *jump) {
    if(DEBUG_GERADOR_ASM) printf("\t\t---->jump\n");

    char cons[MAX_CONST_SIZE];
    sprintf(cons, "    jr %s", ((NAME*)jump->exp)->label->label);
    this->addCodigo(cons);
}

void Gerador::munch(CJUMP *j) {
    if(DEBUG_GERADOR_ASM) printf("\t\t---->cjump\n");

    char op[4];
    switch(j->op){
        case IGUAL: sprintf(op, "beq"); break;
        case DIFERENTE: sprintf(op, "bne"); break;
        case MAIOR: sprintf(op, "bgt"); break;
        case MAIOR_IGUAL: sprintf(op, "bge"); break;
        case MENOR: sprintf(op, "blt"); break;
        case MENOR_IGUAL: sprintf(op, "ble"); break;
    }

    int esquerda;
    munch(j->exp1);
    esquerda = retorno;

    int direita;
    munch(j->exp2);
    direita = retorno;

    char cons[MAX_CONST_SIZE];
    sprintf(cons, "    %s %s, %s, %s", op, getRegistrador(esquerda), getRegistrador(direita), j->ltrue->label);
    addCodigo(cons);
    sprintf(cons, "    jr %s", j->lfalse->label);
    addCodigo(cons);
    tempRegProx--;
    tempRegProx--;

}

void Gerador::munchExp(CALL *call) {
    if(DEBUG_GERADOR_ASM) printf("\t\t---->call\n");

    char cons[MAX_CONST_SIZE];
    NAME* nome = dynamic_cast<NAME*>(call->func);
//    MEM* posicao = dynamic_cast<MEM*>(call->func);

    if (nome != NULL && strcmp(nome->label->label,"IMPRIMA") == 0 ) {

        if(DEBUG_GERADOR_ASM) printf("\t\t---->imprima\n");

        ExpList* explist = call->args;
        while(explist != NULL) {

            NAME* literal = dynamic_cast<NAME*>(explist->exp);
            if(literal != NULL) {
                if(DEBUG_GERADOR_ASM) printf("\t\t---->literal\n");

                sprintf(cons, "    li $v0, 4 # IMPRIMA");
                addCodigo(cons);
                sprintf(cons, "    la $a0, %s", literal->label->label);
                addCodigo(cons);
                addCodigo((char*)"    syscall");

            }

            CONST* _const = dynamic_cast<CONST*>(explist->exp);
            if(_const != NULL) {
                if(DEBUG_GERADOR_ASM) printf("\t\t---->const\n");

                sprintf(cons, "    li $v0, 1");
                addCodigo(cons);
                sprintf(cons, "    li $a0, %d", _const->i);
                addCodigo(cons);
                addCodigo((char*)"    syscall");
            }

            CONSTF* _constf = dynamic_cast<CONSTF*>(explist->exp);
            if(_constf != NULL) {
                if(DEBUG_GERADOR_ASM) printf("\t\t---->constf\n");

                sprintf(cons, "    li $v0, 2");
                addCodigo(cons);
                // @TO-DO
                sprintf(cons, "    l.s $f12, %f", _constf->f);
                addCodigo(cons);
                addCodigo((char*)"    syscall");
            }

            if(!literal && !_const && !_constf) {
                munch(explist->exp);

                sprintf(cons, "    li $v0, 1 \t # IMPRIMA");
                addCodigo(cons);
                sprintf(cons, "    move $a0, %s", getRegistrador(retorno));
                addCodigo(cons);
                addCodigo((char*)"    syscall");

                tempRegProx = 0;

            }

            explist = explist->explist;
        }

        retorno = this->tempRegProx;
        return;
    }

    if (nome != NULL && strcmp(nome->label->label,"LEIA") == 0 ) {
        if(DEBUG_GERADOR_ASM) printf("\t\t---->leia\n");

        ExpList* explist = call->args;
        while(explist != NULL) {

            //printf("\t\t\t----> + arg\n");

            if(DEBUG_GERADOR_ASM) printf("\t\t---->inteiro\n");

            sprintf(cons, "    li $v0, 5");
            addCodigo(cons);
            addCodigo((char*)"    syscall");

            if(DEBUG_GERADOR_ASM) printf("\t\t---->to no exp\n");
            lvalue = true;
            munch(explist->exp);
            lvalue = false;

            sprintf(cons, "    sw $v0, 0(%s)", getRegistrador(retorno));
            addCodigo(cons);

            explist = explist->explist;
        }

        retorno = this->tempRegProx;
        return;
    }

    /*
        CALLER
        1. aloca frame basico
        2. calcula e armazena parametros
        3. armazena endereco de retorno
        4. passa o controle pro procedimento
    */
    addCodigo("\t\t # iniciando caller");
    // Aumentar o tamanho da pilha, para escrever os parametros em ordem
    sprintf(cons, "    subu $sp, $sp, %d",(this->qtddArgs+1)*4);
    addCodigo(cons);

    int tArgs = 0;
    ExpList* args = call->args->explist;
    // Salvando os parametros um a um
    while(args != NULL) {

        tArgs++;
        munch(args->exp);
        sprintf(cons, "    sw %s, %d($sp)", getRegistrador(retorno), tArgs*4);
        addCodigo(cons);
        tempRegProx--;

        args = args->explist;
    }
    // Restituindo o Frame
    sprintf(cons, "    addiu $sp, $sp, %d", (this->qtddArgs+1)*4);
    addCodigo(cons);

    // Saltando para o subprograma
    sprintf(cons, "    jal %s", nome->label->label);
    addCodigo(cons);

    // Voltando do subprogram!
    // Guardando o retorno em novo resgitrador
    this->tempRegProx++;
    sprintf(cons,"    move %s, $v0", getRegistrador(this->tempRegProx));
    addCodigo(cons);

    retorno = this->tempRegProx;

}

void Gerador::munch(MOVE* move) {
    if(DEBUG_GERADOR_ASM) printf("\t\t---->munch move\n");

    TEMP* temp = dynamic_cast<TEMP*>(move->dst);
    if(temp != NULL) munchMove(temp, move->exp);

    MEM* mem = dynamic_cast<MEM*>(move->dst);
    if(mem != NULL) munchMove(mem, move->exp);

}

void Gerador::munchMove(TEMP* dst, Exp * exp) {
    if(DEBUG_GERADOR_ASM) printf("\t\t---->munch move temp exp\n");

    retorno = NULL;
    if(DEBUG_GERADOR_ASM) printf("\t\t---->--- inicio dst ---\n");
    munch(dst);
    int regdst = retorno;

    retorno = NULL;
    if(DEBUG_GERADOR_ASM) printf("\t\t---->--- inicio exp ---\n");
    munch(exp);
    int regexp = retorno;

    char cons[MAX_CONST_SIZE];
    if(DEBUG_GERADOR_ASM) printf("\t\t---->--- inicio move regs ---\n");
    sprintf(cons, "    move %s, %s", getRegistrador(regdst), getRegistrador(regexp));
    addCodigo(cons);
    this->tempRegProx--;

    retorno = regdst;
}

void Gerador::munchMove(NAME* dst, Exp* exp) {
    if(DEBUG_GERADOR_ASM) printf("\t\t---->munch move name exp\n");
}

void Gerador::munchMove(MEM* dst, Exp* exp) {
    if(DEBUG_GERADOR_ASM) printf("\t\t---->munch move mem exp\n");

    //printf(" \t\t\t\t---> %s \n", (char*) typeid(*dst->exp));

    retorno = NULL;
    if(DEBUG_GERADOR_ASM) printf("\t\t---->--- inicio dst ---\n");
    lvalue = true;
    moveJaPassou = false;
    munch(dst);
    lvalue = false;
    int regdst = retorno;

    retorno = NULL;
    if(DEBUG_GERADOR_ASM) printf("\t\t---->--- inicio exp ---\n");
    munch(exp);
    int regexp = retorno;

    char cons[MAX_CONST_SIZE];
    sprintf(cons, "    sw %s, 0(%s) \t # MOVE MEM-Exp", getRegistrador(regexp), getRegistrador(regdst));
    addCodigo(cons);
    this->tempRegProx = -1;

    retorno = regdst;

}

void Gerador::munch(Exp * exp) {
    // Temos várias Opções de expressions
    TEMP *temp   = dynamic_cast<TEMP*> (exp);
    BINOP *binop = dynamic_cast<BINOP*> (exp);
    MEM *mem     = dynamic_cast<MEM*> (exp);
    CONST *cons  = dynamic_cast<CONST*> (exp);
    CALL *call   = dynamic_cast<CALL*> (exp);
    NAME *name   = dynamic_cast<NAME*> (exp);

    if (temp != NULL) this->munchExp(temp);
    else if (binop != NULL) this->munchExp(binop);
    else if (mem   != NULL) this->munchExp(mem);
    else if (cons  != NULL) this->munchExp(cons);
    else if (call  != NULL) this->munchExp(call);
    else if (name  != NULL) this->munchExp(name);

}

void Gerador::munchExp(NAME *name) {
    if(DEBUG_GERADOR_ASM) printf("\t\t---->munch name\n");

    tempRegProx++;
    char cons[MAX_CONST_SIZE];
    sprintf(cons, "    la %s, %s \t # NAME", getRegistrador(tempRegProx), name->label->label);
    addCodigo(cons);
    retorno = tempRegProx;
}

void Gerador::munchExp(TEMP *temp) {
    if(DEBUG_GERADOR_ASM) printf("\t\t---->munch temp\n");
    if(temp->temp->travado == 0) retorno = ++this->tempRegProx;
    else {
        this->tempRegProx++;
        retorno = temp->temp->travado;
    }
}

void Gerador::munch(Stmt * s) {
    if(DEBUG_GERADOR_ASM) printf("\t\t---->munch stmt\n");

    EXP* exp = dynamic_cast<EXP*> (s);
    if (exp != NULL) {
        munch(exp->exp);
        tempRegProx = -1;
    }

    MOVE* move = dynamic_cast<MOVE*> (s);
    if (move != NULL) munch(move);

    CJUMP* cjump = dynamic_cast<CJUMP*> (s);
    if (cjump != NULL) munch(cjump);

    LABEL* label = dynamic_cast<LABEL*> (s);
    if (label != NULL) munch(label);

    JUMP* jump = dynamic_cast<JUMP*> (s);
    if (jump != NULL) munch(jump);
}

void Gerador::munchExp(MEM * mem) {
    if(DEBUG_GERADOR_ASM) printf("\t\t---->mem\n");

    bool jaPassei = moveJaPassou;
    moveJaPassou = true;
    munch(mem->exp);

    if(!lvalue || jaPassei) {
        char cons[MAX_CONST_SIZE];
        sprintf(cons, "    lw %s, 0(%s) \t # MEM", getRegistrador(retorno), getRegistrador(retorno));
        addCodigo(cons);
    }
    retorno = retorno;

}

void Gerador::munchExp(BINOP* binop) {
    if(DEBUG_GERADOR_ASM) printf("\t\t---->binop\n");

    int esquerda, centro;
    munch(binop->exp1);
    if(retorno == REG_FP) {
        esquerda = ++this->tempRegProx;
        centro = REG_FP;
    }
    else {
        esquerda = retorno;
        centro = retorno;
    }

    int direita;
    munch(binop->exp2);
    direita = retorno;
    tempRegProx--;
    //if(centro != esquerda) this->tempRegProx--;

    char cons[MAX_CONST_SIZE];
    switch(binop->op) {
        case MAIS: sprintf(cons, "    add %s, %s, %s", getRegistrador(esquerda), getRegistrador(centro), getRegistrador(direita)); break;
        case MENOS: sprintf(cons, "    sub %s, %s, %s", getRegistrador(esquerda), getRegistrador(centro), getRegistrador(direita)); break;
        case MULTIPLICACAO: sprintf(cons, "    mul %s, %s, %s", getRegistrador(esquerda), getRegistrador(centro), getRegistrador(direita)); break;
        case DIVISAO: sprintf(cons, "    div %s, %s, %s", getRegistrador(esquerda), getRegistrador(centro), getRegistrador(direita)); break;
        case RESTO: sprintf(cons, "    rem %s, %s, %s", getRegistrador(esquerda), getRegistrador(centro), getRegistrador(direita)); break;
    }

    addCodigo(cons);
    //addCodigo(" # aqui?");
    retorno = esquerda;

}

void Gerador::visit(FrameMIPS * _framemips) {



}

void Gerador::visit(Procedure * _procedure) {

    char cons[MAX_CONST_SIZE];

    this->addCodigo((char*) "\n   .text");


    // Prologo - Estamos no main?
    if (strcmp(_procedure->label->label, (char*) "_inicio_") == 0) {
        if (_procedure->frame != NULL) _procedure->frame->accept(this);
        this->addCodigo("   .globl main");
        this->addCodigo("main:");
        // exemplo que vi no livro
        addCodigo("    subu $sp, $sp, 32");
        addCodigo("    sw $ra, 20($sp)");
        addCodigo("    sw $fp, 16($sp)");
        addCodigo("    addiu $fp, $sp, 28");
    }
    else {
        // Definindo label do subprograma
        sprintf(cons,"    .globl %s",_procedure->label->label);
        this->addCodigo(cons);
        sprintf(cons,"\n%s:",_procedure->label->label);
        this->addCodigo(cons);
        // Inciando prologo
        addCodigo("\t# Callee - Prologo \n");
        sprintf(cons, "    subu $sp, $sp, %d", this->tamanhoFrame );
        addCodigo(cons);
        // Se nao puder salvar tudo, salve tudo que puder!
        addCodigo("    sw $ra, 0($sp)");
        addCodigo("    sw $fp, 4($sp)");
        sprintf(cons, "    addiu $fp, $sp, %d", this->tamanhoFrame - ((this->qtddArgs+1)*4));
        addCodigo(cons);

        addCodigo("    sw $t0, 8($sp)");
        addCodigo("    sw $t1, 12($sp)");
        addCodigo("    sw $t2, 16($sp)");
        addCodigo("    sw $t3, 20($sp)");
        addCodigo("    sw $t4, 24($sp)");
        addCodigo("    sw $t5, 28($sp)");
        addCodigo("    sw $t6, 32($sp)");
        addCodigo("    sw $t7, 36($sp)");
        addCodigo("    sw $t8, 40($sp)");
        addCodigo("    sw $t9, 44($sp)");
        addCodigo("    sw $s0, 48($sp)");
        addCodigo("    sw $s1, 52($sp)");
        addCodigo("    sw $s2, 56($sp)");
        addCodigo("    sw $s3, 60($sp)");
        addCodigo("    sw $s4, 64($sp)");
        addCodigo("    sw $s5, 68($sp)");
        addCodigo("    sw $s6, 72($sp)");
        addCodigo("    sw $s7, 76($sp)");

    }

    // corpo
    StmtList* instrucoes = _procedure->body;
    while(instrucoes != NULL){
        munch(instrucoes->stmt);
        instrucoes = instrucoes->stmtlist;
    }

    // Epilogo - Estamos no main?
    if (strcmp(_procedure->label->label, (char*) "_inicio_") == 0) {
        // exemplo que vi no livro
        addCodigo("    lw $ra, 20($sp)");
        addCodigo("    lw $fp, 16($sp)");
        addCodigo("    addiu $sp, $sp, 32");
        addCodigo("    jr $ra");
    }
    else {

        // restaurando registradores
        addCodigo("    lw $ra, 0($sp)");
        addCodigo("    lw $fp, 4($sp)");
        // restaurando outros registradores
        addCodigo("    lw $t0, 8($sp)");
        addCodigo("    lw $t1, 12($sp)");
        addCodigo("    lw $t2, 16($sp)");
        addCodigo("    lw $t3, 20($sp)");
        addCodigo("    lw $t4, 24($sp)");
        addCodigo("    lw $t5, 28($sp)");
        addCodigo("    lw $t6, 32($sp)");
        addCodigo("    lw $t7, 36($sp)");
        addCodigo("    lw $t8, 40($sp)");
        addCodigo("    lw $t9, 44($sp)");
        addCodigo("    lw $s0, 48($sp)");
        addCodigo("    lw $s1, 52($sp)");
        addCodigo("    lw $s2, 56($sp)");
        addCodigo("    lw $s3, 60($sp)");
        addCodigo("    lw $s4, 64($sp)");
        addCodigo("    lw $s5, 68($sp)");
        addCodigo("    lw $s6, 72($sp)");
        addCodigo("    lw $s7, 76($sp)");
        // Voltando o $sp pro topo do Caller
        sprintf(cons, "    addiu $sp, $sp, %d", this->tamanhoFrame );
        addCodigo(cons);
        // Jumpeando para o Caller
        sprintf(cons, "    jr $ra");
        addCodigo(cons);
    }

    if(_procedure->proximo != NULL) _procedure->proximo->accept(this);

}

void Gerador::visit(LiteralRI* _literal) {
    addCodigo("    .rdata");

    char cons[MAX_CONST_SIZE];
    sprintf(cons, "%s:", _literal->label->label);
    addCodigo(cons);
    sprintf(cons, "    .asciiz \"%s\"", _literal->value);
    addCodigo(cons);

    if(_literal->proximo != NULL) _literal->proximo->accept(this);
}

void Gerador::visit(Integer* _int) {
    addCodigo("    .data");

    char cons[MAX_CONST_SIZE];
    sprintf(cons, "    .globl %s", _int->label->label);
    addCodigo(cons);
    sprintf(cons, "%s:", _int->label->label);
    addCodigo(cons);

    sprintf(cons, "    .word ");
    strcat(cons, "0 ");
    while(_int->nElements != 0) {
        strcat(cons, "0 ");
        _int->nElements--;
    }
    addCodigo(cons);

    if(_int->proximo != NULL) _int->proximo->accept(this);
}

void Gerador::visit(Float* _float) {
    addCodigo("    .data");

    char cons[MAX_CONST_SIZE];
    sprintf(cons, "    .globl %s", _float->label->label);
    addCodigo(cons);
    sprintf(cons, "%s:", _float->label->label);
    addCodigo(cons);

    sprintf(cons, "    .float ");
    strcat(cons, "0.0 ");
    while(_float->nElements != 0) {
        strcat(cons, "0.0 ");
        _float
        ->nElements--;
    }
    addCodigo(cons);

    if(_float->proximo != NULL) _float->proximo->accept(this);
}

void Gerador::visit(BooleanRI* _bool) {
    addCodigo("    .data");

    char cons[MAX_CONST_SIZE];
    sprintf(cons, "    .globl %s", _bool->label->label);
    addCodigo(cons);
    sprintf(cons, "%s:", _bool->label->label);
    addCodigo(cons);

    sprintf(cons, "    .word ");
    strcat(cons, "0 ");
    while(_bool->nElements != 0) {
        strcat(cons, "0 ");
        _bool->nElements--;
    }
    addCodigo(cons);

    if(_bool->proximo != NULL) _bool->proximo->accept(this);
}

void Gerador::visit(Record*) {
}

void Gerador::visit(atFrame*) {
}

void Gerador::visit(atReg*) {
}

void Gerador::visit(TempList*) {
}

void Gerador::visit(LocalList*) {
}

void Gerador::visit(Temp*) {
}

void Gerador::visit(Label*) {
}

void Gerador::visit(ExpList*) {
}

void Gerador::visit(StmtList*) {
}

void Gerador::visit(CONST*) {

}

void Gerador::visit(CONSTF*) {
}

void Gerador::visit(NAME*) {
}

void Gerador::visit(TEMP*) {
}

void Gerador::visit(BINOP*) {
}

void Gerador::visit(MEM*) {
}

void Gerador::visit(CALL*) {
}

void Gerador::visit(ESEQ*) {
}

void Gerador::visit(MOVE*) {
}

void Gerador::visit(EXP*) {
}

void Gerador::visit(JUMP*) {
}

void Gerador::visit(CJUMP*) {
}

void Gerador::visit(SEQ*) {
}

void Gerador::visit(LABEL*) {
}

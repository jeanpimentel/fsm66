#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "arvore_sintaxe_abstrata.h"
#include "simbolos.h"
#include "erro.h"

#include "semantico_escopo.h"
#include "semantico_tipo.h"
#include "semantico_variavel.h"

escopo* escopo_base;

escopo* obter_escopo() {
    return escopo_base;
}

void semantico_inicializar(Program* _program) {
    if (_program != NULL) {

        SemanticoEscopoVisitor* verificacao_escopos = new SemanticoEscopoVisitor();
        _program->accept(verificacao_escopos);
        escopo_base = verificacao_escopos->obter_escopo_program();
        //escopo_base->imprimir();

        SemanticoTipoVisitor* verificacao_tipos = new SemanticoTipoVisitor();
        verificacao_tipos->definir_escopo_program(escopo_base);
        _program->accept(verificacao_tipos);
        //escopo_base =
                verificacao_tipos->obter_escopo_program();

        delete verificacao_escopos;
        delete verificacao_tipos;
    }
}

void semantico_finalizar() {
}

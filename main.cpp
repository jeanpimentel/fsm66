#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "erro.h"
#include "lexico.h"
#include "sintatico.h"
#include "semantico.h"
#include "simbolos.h"
#include "gerador_ri.h"
#include "representacao_intermediaria.h"
#include "canonizador.h"
#include "linearizador.h"
#include "gerador.h"

#define EXTENSAO ".por"
#define EXTENSAO_ASM ".asm"

static FILE* arquivo_fonte;

int main(int argc, char* argv[]) {

    char* path_arquivo;

    if(argc > 1) {

        int tamanho = strlen(argv[1]) + strlen(EXTENSAO) + 1;
        path_arquivo = (char*) calloc(tamanho, sizeof(char));
        strcpy(path_arquivo, argv[1]);
        char* posicao_extensao = strstr(path_arquivo, EXTENSAO);
        if(posicao_extensao == NULL) strcat(path_arquivo, EXTENSAO);
        printf("Carregando arquivo %s\n\n", path_arquivo);
        arquivo_fonte = fopen(path_arquivo, "rb");
        if(arquivo_fonte == NULL) {
            fputs("Erro na abertura do arquivo!", stderr);
            exit(1);
        }

        if(argc > 2) {
            int tamanho = strlen(argv[2]) + strlen(EXTENSAO_ASM) + 1;
            path_arquivo = (char*) calloc(tamanho, sizeof(char));
            strcpy(path_arquivo, argv[2]);
            char* posicao_extensao = strstr(path_arquivo, EXTENSAO_ASM);
            if(posicao_extensao == NULL) strcat(path_arquivo, EXTENSAO_ASM);
        }
        else {
            int tamanho = strlen(argv[1]) + strlen(EXTENSAO_ASM) + 1;
            path_arquivo = (char*) calloc(tamanho, sizeof(char));
            strcpy(path_arquivo, argv[1]);
            char* posicao_extensao = strstr(path_arquivo, EXTENSAO_ASM);
            if(posicao_extensao == NULL) strcat(path_arquivo, EXTENSAO_ASM);
        }


    } else {
        printf("Nenhum argumento fornecido.\n");
        exit(-1);
    }

    lexico_inicializar(arquivo_fonte);

    Program* _program = sintatico_inicializar();
    sintatico_finalizar();

    lexico_finalizar();
    fclose(arquivo_fonte);

    semantico_inicializar(_program);
    semantico_finalizar();


    if(erro_quantidade_erros() > 0) {
        printf("\n\n\nERROS: %d\n\n\n",  erro_quantidade_erros());
        exit(1);
    }

    ImprimeVisitorRI* imprime  = new ImprimeVisitorRI();

    VisitorGeradorRI* vri  = new VisitorGeradorRI(obter_escopo());
    vri->visit(_program);
    Fragment* RI = vri->raizri;

    printf("\n\n");
    RI->accept(imprime);

    Canonizador* c = new Canonizador();
    RI->accept(c);
    delete c;

    Linearizador* l = new Linearizador();
    RI->accept(l);
    delete l;

    printf("\n\n");
    RI->accept(imprime);

    printf("\n\n-=-=-=-=-=-=-=-=-=-=-=-=-=- ASM -=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n");

    Gerador* gasm = new Gerador(path_arquivo);
    RI->accept(gasm);

    printf("\nASM gerado: %s\n\n", path_arquivo);
    exit(0);
}

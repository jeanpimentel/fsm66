#ifndef TABELA_SIMBOLOS_H_INCLUDED
#define TABELA_SIMBOLOS_H_INCLUDED

#define TAMANHO_TABELA 211
#define TAMANHO_INICIAL_ARRANJO 100
#define TAMANHO_PASSO_CRESCIMENTO_ARRANJO 10

class tabela_simbolos;
class simbolo_variavel;
class simbolo_funcao;

class escopo {
    public:
        tabela_simbolos* tab_variaveis;
        tabela_simbolos* tab_registros;
        tabela_simbolos* tab_funcoes;
        escopo* pai;
        const char* nome;
        escopo(escopo*, const char*);
        ~escopo();

        simbolo_variavel* buscar_variavel(char* , int*);
        simbolo_funcao*   buscar_funcao(char* , int*);

        void imprimir();
};



class simbolo {
    public:
        simbolo();
        ~simbolo();
};

class simbolo_palavra_reservada : public simbolo {
    public:
        int token;
        simbolo_palavra_reservada(int);
        ~simbolo_palavra_reservada();
};

class simbolo_variavel : public simbolo {
    public:
        int tipo;
        const char* lexema_tipo;
        int nlocal;
        char* name;
        int offset;

        int lowerbound;
        int upperbound;
        simbolo_variavel(int, const char*, int, int , int);
        ~simbolo_variavel();
        void imprimir();
};

class simbolo_registro : public simbolo {
    public:
        escopo* escopo_registro;
        
        simbolo_registro(escopo*);
        ~simbolo_registro();
        void imprimir();

};
class argumento{
    public:
        simbolo_variavel* var;
        argumento* prox;
};
class simbolo_funcao : public simbolo {
    public:
        const char* nome_funcao;
        escopo* escopo_funcao;
        bool tem_retorno;
        int tipo_retorno;
        int quantidade_parametros;
        int quantidade_variaveis;
        argumento* parametros;
        simbolo_funcao();
        ~simbolo_funcao();
        void imprimir();
};



class tabela_simbolos {
    protected:
        unsigned int proxima_posicao;
        unsigned int tamanho_arranjo;
        char* arranjo;
        struct entrada {
            int posicao_arranjo;
            simbolo* info;
            struct entrada* proximo;
        } *tabela[TAMANHO_TABELA];

        char* obter_lexema_arranjo(int);
        int funcao_hash(char*);

    public:
        int adicionar_lexema_arranjo(const char*);
        void to_string();

        tabela_simbolos();
        ~tabela_simbolos();
        void inserir(char*, simbolo*);
        void inserir(const char*, simbolo*);
        simbolo* buscar(char*);
        void imprimir(const char*);
};

#endif // TABELA_SIMBOLOS_H_INCLUDED

#ifndef SINTATICO_H_INCLUDED
#define SINTATICO_H_INCLUDED

#include "arvore_sintaxe_abstrata.h"

/** @brief  Indica se será impresso o nome das produções da gramática. */
#define IMPRIMIR_NOME_PRODUCOES 0

/** @brief  Indica se será impresso o match durante a análise. */
#define IMPRIMIR_MATCH 0

/** @brief  Indica se será impresso o connjunto sincronizador em caso de erro.\n Uso em DEBUG. */
#define IMPRIMIR_SYNCH 0

Program* program();
void instructions(VarList** , TypeList** , SubProgList** , StatementList** );
Statement* statement_start();
Statement*  compound_statement();
Statement* conditional();
SubProgList* subprogram_declaration();
SubProgList* subprogram();
TypeList* type_declaration();
VarList* variable_declaration();
Type* type();
int standard_type();
Statement* resolve_id(Identificador*, VarList** varlist, TypeList** typelist, SubProgList** subproglist, StatementList** statementlist);
Variable* statement_declaration(Identificador*);
Expression* procedure_call(Identificador*);
VarList* id_list(Type*);
VarList* id_list_recursive(Type*);
Type* type_2();
Type* type_3(NumberInteger*, NumberInteger*);
VarList* arguments();
StatementList* optional_statement(StatementList*);
StatementList* statement_list(StatementList**);
Statement* statement();
void statement_list_optional(StatementList**);
void statement_list_recursive(StatementList**);
Expression* statement_id(Identificador*);
void declarations(VarList**, TypeList**, SubProgList**);
VarList* variable_declaration_recursive();
Variable* optional_id();
Statement* optional_senao();
ExprList* expression_list();
Expression* expression_ou();
Expression* expression_e();
Expression* expression_comparative();
Expression* expression_relative();
Expression* expression_sum();
Expression* expression_mult();
Expression* expression_unit();
Expression* factor();
Expression* complement_id(Identificador*);
ExprList* expression_list_recursive();
Expression* expression_ou_recursive();
Expression* expression_e_recursive();
Expression* expression_comparative_recursive(int*);
Expression* expression_relative_recursive(int*);
Expression* expression_sum_recursive(int*);
Expression* expression_mult_recursive(int*);
int comparative_operator();
int relative_operator();
int sum_operator();
int mult_operator();
int unit_operator();

Program* sintatico_inicializar();
void sintatico_finalizar();
char* sintatico_casar(int);
char* sintatico_recuperar(int[]);
char* sintatico_casar_ou_recuperar(int, int[]);

#endif // SINTATICO_H_INCLUDED

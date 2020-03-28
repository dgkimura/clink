#ifndef __GRAMMAR_H__
#define __GRAMMAR_H__

#include "ast.h"
#include "parser.h"

/*
 * List of rules that defines the grammar for the parser. It is a copy of the C
 * grammar as defined by K&R in "C Programming Language" 2nd edition reference
 * manual.
 */
struct rule grammar[NUM_RULES] =
{
    /* translation-unit: */
    {
        AST_TRANSLATION_UNIT,
        create_translation_unit_node,
        1,
        { AST_EXTERNAL_DECLARATION }
    },
    {
        AST_TRANSLATION_UNIT,
        create_translation_unit_node,
        2,
        { AST_TRANSLATION_UNIT, AST_EXTERNAL_DECLARATION }
    },
    /* external-declaration: */
    {
        AST_EXTERNAL_DECLARATION,
        create_elided_node,
        1,
        { AST_FUNCTION_DEFINITION }
    },
    {
        AST_EXTERNAL_DECLARATION,
        create_elided_node,
        1,
        { AST_DECLARATION }
    },
    /* function-definition: */
    {
        AST_FUNCTION_DEFINITION,
        create_function_definition,
        2,
        { AST_DECLARATOR, AST_COMPOUND_STATEMENT }
    },
    {
        AST_FUNCTION_DEFINITION,
        create_function_definition,
        3,
        { AST_DECLARATION_SPECIFIERS, AST_DECLARATOR, AST_COMPOUND_STATEMENT }
    },
    {
        AST_FUNCTION_DEFINITION,
        create_function_definition,
        3,
        { AST_DECLARATOR, AST_DECLARATION_LIST, AST_COMPOUND_STATEMENT }
    },
    {
        AST_FUNCTION_DEFINITION,
        create_function_definition,
        4,
        { AST_DECLARATION_SPECIFIERS, AST_DECLARATOR, AST_DECLARATION_LIST, AST_COMPOUND_STATEMENT }
    },
    /* declaration: */
    {
        AST_DECLARATION,
        create_elided_node,
        2,
        { AST_DECLARATION_SPECIFIERS, AST_SEMICOLON }
    },
    {
        AST_DECLARATION,
        create_declaration,
        3,
        { AST_DECLARATION_SPECIFIERS, AST_INIT_DECLARATOR_LIST, AST_SEMICOLON }
    },
    /* declaration-list: */
    {
        AST_DECLARATION_LIST,
        create_,
        1,
        { AST_DECLARATION }
    },
    {
        AST_DECLARATION_LIST,
        create_,
        2,
        { AST_DECLARATION_LIST, AST_DECLARATION }
    },
    /* declaration-specifiers: */
    {
        AST_DECLARATION_SPECIFIERS,
        create_declaration_specifiers,
        1,
        { AST_STORAGE_CLASS_SPECIFIER }
    },
    {
        AST_DECLARATION_SPECIFIERS,
        create_declaration_specifiers,
        2,
        { AST_STORAGE_CLASS_SPECIFIER, AST_DECLARATION_SPECIFIERS }
    },
    {
        AST_DECLARATION_SPECIFIERS,
        create_declaration_specifiers,
        1,
        { AST_TYPE_SPECIFIER }
    },
    {
        AST_DECLARATION_SPECIFIERS,
        create_declaration_specifiers,
        2,
        { AST_TYPE_SPECIFIER, AST_DECLARATION_SPECIFIERS }
    },
    {
        AST_DECLARATION_SPECIFIERS,
        create_declaration_specifiers,
        1,
        { AST_TYPE_QUALIFIER }
    },
    {
        AST_DECLARATION_SPECIFIERS,
        create_declaration_specifiers,
        2,
        { AST_TYPE_QUALIFIER, AST_DECLARATION_SPECIFIERS }
    },
    /* storage-class-specifier: */
    {
        AST_STORAGE_CLASS_SPECIFIER,
        create_storage_class_specifier,
        1,
        { AST_AUTO }
    },
    {
        AST_STORAGE_CLASS_SPECIFIER,
        create_storage_class_specifier,
        1,
        { AST_REGISTER }
    },
    {
        AST_STORAGE_CLASS_SPECIFIER,
        create_storage_class_specifier,
        1,
        { AST_STATIC }
    },
    {
        AST_STORAGE_CLASS_SPECIFIER,
        create_storage_class_specifier,
        1,
        { AST_EXTERN }
    },
    {
        AST_STORAGE_CLASS_SPECIFIER,
        create_storage_class_specifier,
        1,
        { AST_TYPEDEF }
    },
    /* type-specifier: */
    {
        AST_TYPE_SPECIFIER,
        create_type_specifier,
        1,
        { AST_VOID }
    },
    {
        AST_TYPE_SPECIFIER,
        create_type_specifier,
        1,
        { AST_CHAR }
    },
    {
        AST_TYPE_SPECIFIER,
        create_type_specifier,
        1,
        { AST_SHORT }
    },
    {
        AST_TYPE_SPECIFIER,
        create_type_specifier,
        1,
        { AST_INT }
    },
    {
        AST_TYPE_SPECIFIER,
        create_type_specifier,
        1,
        { AST_LONG }
    },
    {
        AST_TYPE_SPECIFIER,
        create_type_specifier,
        1,
        { AST_FLOAT }
    },
    {
        AST_TYPE_SPECIFIER,
        create_type_specifier,
        1,
        { AST_DOUBLE }
    },
    {
        AST_TYPE_SPECIFIER,
        create_type_specifier,
        1,
        { AST_SIGNED }
    },
    {
        AST_TYPE_SPECIFIER,
        create_type_specifier,
        1,
        { AST_UNSIGNED }
    },
    {
        AST_TYPE_SPECIFIER,
        create_type_specifier,
        1,
        { AST_STRUCT_OR_UNION_SPECIFIER }
    },
    {
        AST_TYPE_SPECIFIER,
        create_type_specifier,
        1,
        { AST_ENUM_SPECIFIER }
    },
    {
        AST_TYPE_SPECIFIER,
        create_type_specifier,
        1,
        { AST_TYPEDEF_NAME }
    },
    /* type-qualifier: */
    {
        AST_TYPE_QUALIFIER,
        create_type_qualifier,
        1,
        { AST_CONST }
    },
    {
        AST_TYPE_QUALIFIER,
        create_type_qualifier,
        1,
        { AST_VOLATILE }
    },
    /* struct-or-union-specifier: */
    {
        AST_STRUCT_OR_UNION_SPECIFIER,
        create_,
        4,
        { AST_STRUCT_OR_UNION, AST_LBRACE, AST_STRUCT_DECLARATION_LIST, AST_RBRACE }
    },
    {
        AST_STRUCT_OR_UNION_SPECIFIER,
        create_,
        5,
        { AST_STRUCT_OR_UNION, AST_IDENTIFIER, AST_LBRACE, AST_STRUCT_DECLARATION_LIST, AST_RBRACE }
    },
    {
        AST_STRUCT_OR_UNION_SPECIFIER,
        create_,
        2,
        { AST_STRUCT_OR_UNION, AST_IDENTIFIER }
    },
    /* struct-or-union: */
    {
        AST_STRUCT_OR_UNION,
        create_,
        1,
        { AST_STRUCT }
    },
    {
        AST_STRUCT_OR_UNION,
        create_,
        1,
        { AST_UNION }
    },
    /* struct-declaration-list: */
    {
        AST_STRUCT_DECLARATION_LIST,
        create_,
        1,
        { AST_STRUCT_DECLARATION }
    },
    {
        AST_STRUCT_DECLARATION_LIST,
        create_,
        2,
        { AST_STRUCT_DECLARATION_LIST, AST_STRUCT_DECLARATION }
    },
    /* init-declarator-list: */
    {
        AST_INIT_DECLARATOR_LIST,
        create_init_declarator_list,
        1,
        { AST_INIT_DECLARATOR }
    },
    {
        AST_INIT_DECLARATOR_LIST,
        create_init_declarator_list,
        3,
        { AST_INIT_DECLARATOR_LIST, AST_COMMA, AST_INIT_DECLARATOR }
    },
    /* init-declarator: */
    {
        AST_INIT_DECLARATOR,
        create_elided_node,
        1,
        { AST_DECLARATOR }
    },
    {
        AST_INIT_DECLARATOR,
        create_init_declarator,
        3,
        { AST_DECLARATOR, AST_EQUAL, AST_INITIALIZER }
    },
    /* struct-declaration: */
    {
        AST_STRUCT_DECLARATION,
        create_,
        3,
        { AST_SPECIFIER_QUALIFIER_LIST, AST_STRUCT_DECLARATION_LIST, AST_SEMICOLON }
    },
    /* specifier-qualifier-list: */
    {
        AST_SPECIFIER_QUALIFIER_LIST,
        create_,
        1,
        { AST_TYPE_SPECIFIER }
    },
    {
        AST_SPECIFIER_QUALIFIER_LIST,
        create_,
        2,
        { AST_TYPE_SPECIFIER, AST_SPECIFIER_QUALIFIER_LIST }
    },
    {
        AST_SPECIFIER_QUALIFIER_LIST,
        create_,
        1,
        { AST_TYPE_QUALIFIER }
    },
    {
        AST_SPECIFIER_QUALIFIER_LIST,
        create_,
        2,
        { AST_TYPE_QUALIFIER, AST_SPECIFIER_QUALIFIER_LIST }
    },
    /* struct-declaration-list: */
    {
        AST_STRUCT_DECLARATION_LIST,
        create_,
        1,
        { AST_STRUCT_DECLARATOR }
    },
    {
        AST_STRUCT_DECLARATION_LIST,
        create_,
        3,
        { AST_STRUCT_DECLARATION_LIST, AST_COMMA, AST_STRUCT_DECLARATOR }
    },
    /* struct-declarator: */
    {
        AST_STRUCT_DECLARATOR,
        create_,
        1,
        { AST_DECLARATOR }
    },
    {
        AST_STRUCT_DECLARATOR,
        create_,
        2,
        { AST_COLON, AST_CONSTANT_EXPRESSION }
    },
    {
        AST_STRUCT_DECLARATOR,
        create_,
        3,
        { AST_DECLARATOR, AST_COLON, AST_CONSTANT_EXPRESSION }
    },
    /* enum-specifier: */
    {
        AST_ENUM_SPECIFIER,
        create_,
        2,
        { AST_ENUM, AST_IDENTIFIER }
    },
    {
        AST_ENUM_SPECIFIER,
        create_,
        4,
        { AST_ENUM, AST_LBRACE, AST_ENUMERATOR_LIST, AST_RBRACKET }
    },
    {
        AST_ENUM_SPECIFIER,
        create_,
        5,
        { AST_ENUM, AST_IDENTIFIER, AST_LBRACE, AST_ENUMERATOR_LIST, AST_RBRACKET }
    },
    /* enumerator-list: */
    {
        AST_ENUMERATOR_LIST,
        create_,
        1,
        { AST_ENUMERATOR }
    },
    {
        AST_ENUMERATOR_LIST,
        create_,
        3,
        { AST_ENUMERATOR_LIST, AST_COMMA, AST_ENUMERATOR }
    },
    /* enumerator: */
    {
        AST_ENUMERATOR,
        create_,
        1,
        { AST_IDENTIFIER }
    },
    {
        AST_ENUMERATOR,
        create_,
        3,
        { AST_IDENTIFIER, AST_EQUAL, AST_CONSTANT_EXPRESSION }
    },
    /* declarator: */
    {
        AST_DECLARATOR,
        create_elided_node,
        1,
        { AST_DIRECT_DECLARATOR }
    },
    {
        AST_DECLARATOR,
        create_,
        2,
        { AST_POINTER, AST_DIRECT_DECLARATOR }
    },
    /* direct-declarator: */
    {
        AST_DIRECT_DECLARATOR,
        create_direct_declarator,
        1,
        { AST_IDENTIFIER }
    },
    {
        AST_DIRECT_DECLARATOR,
        create_direct_declarator,
        3,
        { AST_LPAREN, AST_DECLARATOR, AST_RPAREN }
    },
    {
        AST_DIRECT_DECLARATOR,
        create_direct_declarator,
        3,
        { AST_DIRECT_DECLARATOR, AST_LBRACKET, AST_RBRACKET }
    },
    {
        AST_DIRECT_DECLARATOR,
        create_direct_declarator,
        4,
        { AST_DIRECT_DECLARATOR, AST_LBRACKET, AST_CONSTANT_EXPRESSION, AST_RBRACKET }
    },
    {
        AST_DIRECT_DECLARATOR,
        create_direct_declarator,
        3,
        { AST_DIRECT_DECLARATOR, AST_LPAREN, AST_RPAREN }
    },
    {
        AST_DIRECT_DECLARATOR,
        create_direct_declarator,
        4,
        { AST_DIRECT_DECLARATOR, AST_LPAREN, AST_PARAMETER_TYPE_LIST, AST_RPAREN }
    },
    {
        AST_DIRECT_DECLARATOR,
        create_direct_declarator,
        4,
        { AST_DIRECT_DECLARATOR, AST_LPAREN, AST_IDENTIFIER_LIST, AST_RPAREN }
    },
    /* pointer: */
    {
        AST_POINTER,
        create_,
        1,
        { AST_ASTERISK }
    },
    {
        AST_POINTER,
        create_,
        2,
        { AST_ASTERISK, AST_TYPE_QUALIFIER_LIST }
    },
    {
        AST_POINTER,
        create_,
        2,
        { AST_ASTERISK, AST_POINTER }
    },
    {
        AST_POINTER,
        create_,
        3,
        { AST_ASTERISK, AST_TYPE_QUALIFIER_LIST, AST_POINTER }
    },
    /* type-qualifier-list: */
    {
        AST_TYPE_QUALIFIER_LIST,
        create_,
        1,
        { AST_TYPE_QUALIFIER }
    },
    {
        AST_TYPE_QUALIFIER_LIST,
        create_,
        2,
        { AST_TYPE_QUALIFIER_LIST, AST_TYPE_QUALIFIER }
    },
    /* parameter-type-list: */
    {
        AST_PARAMETER_TYPE_LIST,
        create_elided_node,
        1,
        { AST_PARAMETER_LIST }
    },
    {
        AST_PARAMETER_TYPE_LIST,
        create_,
        3,
        { AST_PARAMETER_LIST, AST_COMMA, AST_ELLIPSIS }
    },
    /* parameter-list: */
    {
        AST_PARAMETER_LIST,
        create_parameter_list,
        1,
        { AST_PARAMETER_DECLARATION }
    },
    {
        AST_PARAMETER_LIST,
        create_parameter_list,
        3,
        { AST_PARAMETER_LIST, AST_COMMA, AST_PARAMETER_DECLARATION }
    },
    /* parameter-declaration: */
    {
        AST_PARAMETER_DECLARATION,
        create_parameter_declaration,
        2,
        { AST_DECLARATION_SPECIFIERS, AST_DECLARATOR }
    },
    {
        AST_PARAMETER_DECLARATION,
        create_parameter_declaration,
        2,
        { AST_DECLARATION_SPECIFIERS, AST_ABSTRACT_DECLARATOR }
    },
    {
        AST_PARAMETER_DECLARATION,
        create_parameter_declaration,
        1,
        { AST_DECLARATION_SPECIFIERS }
    },
    /* identifier-list: */
    {
        AST_IDENTIFIER_LIST,
        create_,
        1,
        { AST_IDENTIFIER }
    },
    {
        AST_IDENTIFIER_LIST,
        create_,
        3,
        { AST_IDENTIFIER_LIST, AST_COMMA, AST_IDENTIFIER }
    },
    /* initializer: */
    {
        AST_INITIALIZER,
        create_,
        1,
        { AST_ASSIGNMENT_EXPRESSION }
    },
    {
        AST_INITIALIZER,
        create_,
        3,
        { AST_LBRACE, AST_INITIALIZER_LIST, AST_RBRACE }
    },
    {
        AST_INITIALIZER,
        create_,
        4,
        { AST_LBRACE, AST_INITIALIZER_LIST, AST_COMMA, AST_RBRACE }
    },
    /* initializer-list: */
    {
        AST_INITIALIZER_LIST,
        create_,
        1,
        { AST_INITIALIZER }
    },
    {
        AST_INITIALIZER_LIST,
        create_,
        3,
        { AST_INITIALIZER_LIST, AST_COMMA, AST_INITIALIZER }
    },
    /* type-name: */
    {
        AST_TYPE_NAME,
        create_,
        1,
        { AST_SPECIFIER_QUALIFIER_LIST }
    },
    {
        AST_ABSTRACT_DECLARATOR,
        create_,
        2,
        { AST_SPECIFIER_QUALIFIER_LIST, AST_ABSTRACT_DECLARATOR }
    },
    /* abstract-declarator: */
    {
        AST_ABSTRACT_DECLARATOR,
        create_,
        1,
        { AST_POINTER }
    },
    {
        AST_ABSTRACT_DECLARATOR,
        create_,
        1,
        { AST_DIRECT_ABSTRACT_DECLARATOR }
    },
    {
        AST_ABSTRACT_DECLARATOR,
        create_,
        2,
        { AST_POINTER, AST_DIRECT_ABSTRACT_DECLARATOR }
    },
    /* direct-abstract-declarator: */
    {
        AST_DIRECT_ABSTRACT_DECLARATOR,
        create_,
        3,
        { AST_LPAREN, AST_ABSTRACT_DECLARATOR, AST_RPAREN }
    },
    {
        AST_DIRECT_ABSTRACT_DECLARATOR,
        create_,
        2,
        { AST_LBRACKET, AST_RBRACKET }
    },
    {
        AST_DIRECT_ABSTRACT_DECLARATOR,
        create_,
        3,
        { AST_DIRECT_ABSTRACT_DECLARATOR, AST_LBRACKET, AST_RBRACKET }
    },
    {
        AST_DIRECT_ABSTRACT_DECLARATOR,
        create_,
        3,
        { AST_LBRACKET, AST_CONSTANT_EXPRESSION, AST_RBRACKET }
    },
    {
        AST_DIRECT_ABSTRACT_DECLARATOR,
        create_,
        4,
        { AST_DIRECT_ABSTRACT_DECLARATOR, AST_LBRACKET, AST_CONSTANT_EXPRESSION, AST_RBRACKET }
    },
    {
        AST_DIRECT_ABSTRACT_DECLARATOR,
        create_,
        2,
        { AST_LPAREN, AST_RPAREN }
    },
    {
        AST_DIRECT_ABSTRACT_DECLARATOR,
        create_,
        3,
        { AST_DIRECT_ABSTRACT_DECLARATOR, AST_LPAREN, AST_RPAREN }
    },
    {
        AST_DIRECT_ABSTRACT_DECLARATOR,
        create_,
        3,
        { AST_LPAREN, AST_PARAMETER_TYPE_LIST, AST_RPAREN }
    },
    {
        AST_DIRECT_ABSTRACT_DECLARATOR,
        create_,
        4,
        { AST_DIRECT_ABSTRACT_DECLARATOR, AST_LPAREN, AST_PARAMETER_TYPE_LIST, AST_RPAREN }
    },
    /* statement: */
    {
        AST_STATEMENT,
        create_,
        1,
        { AST_LABELED_STATEMENT }
    },
    {
        AST_STATEMENT,
        create_,
        1,
        { AST_EXPRESSION_STATEMENT }
    },
    {
        AST_STATEMENT,
        create_,
        1,
        { AST_COMPOUND_STATEMENT }
    },
    {
        AST_STATEMENT,
        create_,
        1,
        { AST_SELECTION_STATEMENT }
    },
    {
        AST_STATEMENT,
        create_,
        1,
        { AST_ITERATION_STATEMENT }
    },
    {
        AST_STATEMENT,
        create_,
        1,
        { AST_JUMP_STATEMENT }
    },
    /* labeled-statement: */
    {
        AST_LABELED_STATEMENT,
        create_,
        3,
        { AST_IDENTIFIER, AST_COLON, AST_STATEMENT }
    },
    {
        AST_LABELED_STATEMENT,
        create_,
        4,
        { AST_CASE, AST_CONSTANT_EXPRESSION, AST_COLON, AST_STATEMENT }
    },
    {
        AST_LABELED_STATEMENT,
        create_,
        3,
        { AST_DEFAULT, AST_COLON, AST_STATEMENT }
    },
    /* expression-statement: */
    {
        AST_EXPRESSION_STATEMENT,
        create_,
        1,
        { AST_SEMICOLON }
    },
    {
        AST_EXPRESSION_STATEMENT,
        create_,
        2,
        { AST_EXPRESSION, AST_SEMICOLON }
    },
    /* compound-statement: */
    {
        AST_COMPOUND_STATEMENT,
        create_,
        2,
        { AST_LBRACE, AST_RBRACE }
    },
    {
        AST_COMPOUND_STATEMENT,
        create_,
        3,
        { AST_LBRACE, AST_DECLARATION_LIST, AST_RBRACE }
    },
    {
        AST_COMPOUND_STATEMENT,
        create_,
        3,
        { AST_LBRACE, AST_STATEMENT_LIST, AST_RBRACE }
    },
    {
        AST_COMPOUND_STATEMENT,
        create_,
        4,
        { AST_LBRACE, AST_DECLARATION_LIST, AST_STATEMENT_LIST, AST_RBRACE }
    },
    /* statement-list: */
    {
        AST_STATEMENT_LIST,
        create_,
        2,
        { AST_STATEMENT_LIST, AST_STATEMENT }
    },
    {
        AST_STATEMENT_LIST,
        create_,
        1,
        { AST_STATEMENT }
    },
    /* selection-statement: */
    {
        AST_SELECTION_STATEMENT,
        create_,
        5,
        { AST_IF, AST_LPAREN, AST_EXPRESSION, AST_RPAREN, AST_STATEMENT }
    },
    {
        AST_SELECTION_STATEMENT,
        create_,
        7,
        { AST_IF, AST_LPAREN, AST_EXPRESSION, AST_RPAREN, AST_STATEMENT, AST_ELSE, AST_STATEMENT }
    },
    {
        AST_SELECTION_STATEMENT,
        create_,
        5,
        { AST_SWITCH, AST_LPAREN, AST_EXPRESSION, AST_RPAREN, AST_STATEMENT }
    },
    /* iteration-statement: */
    {
        AST_ITERATION_STATEMENT,
        create_,
        5,
        { AST_WHILE, AST_LPAREN, AST_EXPRESSION, AST_RPAREN, AST_STATEMENT }
    },
    {
        AST_ITERATION_STATEMENT,
        create_,
        7,
        { AST_DO, AST_STATEMENT, AST_WHILE, AST_LPAREN, AST_EXPRESSION, AST_RPAREN, AST_SEMICOLON }
    },
    {
        AST_ITERATION_STATEMENT,
        create_,
        6,
        { AST_FOR, AST_LPAREN, AST_SEMICOLON, AST_SEMICOLON, AST_RPAREN, AST_STATEMENT }
    },
    {
        AST_ITERATION_STATEMENT,
        create_,
        7,
        { AST_FOR, AST_LPAREN, AST_EXPRESSION, AST_SEMICOLON, AST_SEMICOLON, AST_RPAREN, AST_STATEMENT }
    },
    {
        AST_ITERATION_STATEMENT,
        create_,
        7,
        { AST_FOR, AST_LPAREN, AST_SEMICOLON, AST_EXPRESSION, AST_SEMICOLON, AST_RPAREN, AST_STATEMENT }
    },
    {
        AST_ITERATION_STATEMENT,
        create_,
        7,
        { AST_FOR, AST_LPAREN, AST_SEMICOLON, AST_SEMICOLON, AST_EXPRESSION, AST_RPAREN, AST_STATEMENT }
    },
    {
        AST_ITERATION_STATEMENT,
        create_,
        8,
        { AST_FOR, AST_LPAREN, AST_EXPRESSION, AST_SEMICOLON, AST_EXPRESSION, AST_SEMICOLON, AST_RPAREN, AST_STATEMENT }
    },
    {
        AST_ITERATION_STATEMENT,
        create_,
        8,
        { AST_FOR, AST_LPAREN, AST_EXPRESSION, AST_SEMICOLON, AST_SEMICOLON, AST_EXPRESSION, AST_RPAREN, AST_STATEMENT }
    },
    {
        AST_ITERATION_STATEMENT,
        create_,
        8,
        { AST_FOR, AST_LPAREN, AST_SEMICOLON, AST_EXPRESSION, AST_SEMICOLON, AST_EXPRESSION, AST_RPAREN, AST_STATEMENT }
    },
    {
        AST_ITERATION_STATEMENT,
        create_,
        9,
        { AST_FOR, AST_LPAREN, AST_EXPRESSION, AST_SEMICOLON, AST_EXPRESSION, AST_SEMICOLON, AST_EXPRESSION, AST_RPAREN, AST_STATEMENT }
    },
    /* jump-statement: */
    {
        AST_JUMP_STATEMENT,
        create_,
        3,
        { AST_GOTO, AST_IDENTIFIER, AST_SEMICOLON }
    },
    {
        AST_JUMP_STATEMENT,
        create_,
        2,
        { AST_CONTINUE, AST_SEMICOLON }
    },
    {
        AST_JUMP_STATEMENT,
        create_,
        2,
        { AST_BREAK, AST_SEMICOLON }
    },
    {
        AST_JUMP_STATEMENT,
        create_,
        2,
        { AST_RETURN, AST_SEMICOLON }
    },
    {
        AST_JUMP_STATEMENT,
        create_,
        3,
        { AST_RETURN, AST_EXPRESSION, AST_SEMICOLON }
    },
    /* expression: */
    {
        AST_EXPRESSION,
        create_,
        3,
        { AST_EXPRESSION, AST_COMMA, AST_ASSIGNMENT_EXPRESSION }
    },
    {
        AST_EXPRESSION,
        create_,
        1,
        { AST_ASSIGNMENT_EXPRESSION }
    },
    /* assignment-expression: */
    /* TODO: other assignemnt operators*/
    {
        AST_ASSIGNMENT_EXPRESSION,
        create_assignment_expression,
        3,
        { AST_UNARY_EXPRESSION, AST_EQUAL, AST_ASSIGNMENT_EXPRESSION }
    },
    {
        AST_ASSIGNMENT_EXPRESSION,
        create_assignment_expression,
        3,
        { AST_UNARY_EXPRESSION, AST_ASTERISK_EQUAL, AST_ASSIGNMENT_EXPRESSION }
    },
    {
        AST_ASSIGNMENT_EXPRESSION,
        create_assignment_expression,
        3,
        { AST_UNARY_EXPRESSION, AST_BACKSLASH_EQUAL, AST_ASSIGNMENT_EXPRESSION }
    },
    {
        AST_ASSIGNMENT_EXPRESSION,
        create_assignment_expression,
        3,
        { AST_UNARY_EXPRESSION, AST_MOD_EQUAL, AST_ASSIGNMENT_EXPRESSION }
    },
    {
        AST_ASSIGNMENT_EXPRESSION,
        create_assignment_expression,
        3,
        { AST_UNARY_EXPRESSION, AST_PLUS_EQUAL, AST_ASSIGNMENT_EXPRESSION }
    },
    {
        AST_ASSIGNMENT_EXPRESSION,
        create_assignment_expression,
        3,
        { AST_UNARY_EXPRESSION, AST_MINUS_EQUAL, AST_ASSIGNMENT_EXPRESSION }
    },
    {
        AST_ASSIGNMENT_EXPRESSION,
        create_elided_node,
        1,
        { AST_CONDITIONAL_EXPRESSION }
    },
    /* constant-expression: */
    {
        AST_CONSTANT_EXPRESSION,
        create_elided_node,
        1,
        { AST_CONDITIONAL_EXPRESSION }
    },
    /* conditional-expression: */
    {
        AST_CONDITIONAL_EXPRESSION,
        create_,
        5,
        { AST_LOGICAL_OR_EXPRESSION, AST_QUESTIONMARK, AST_EXPRESSION, AST_COLON, AST_CONDITIONAL_EXPRESSION }
    },
    {
        AST_CONDITIONAL_EXPRESSION,
        create_elided_node,
        1,
        { AST_LOGICAL_OR_EXPRESSION }
    },
    /* logical-or-expression: */
    {
        AST_LOGICAL_OR_EXPRESSION,
        create_,
        3,
        { AST_LOGICAL_OR_EXPRESSION, AST_VERTICALBAR_VERTICALBAR, AST_LOGICAL_AND_EXPRESSION }
    },
    {
        AST_LOGICAL_OR_EXPRESSION,
        create_elided_node,
        1,
        { AST_LOGICAL_AND_EXPRESSION }
    },
    /* logical-and-expression: */
    {
        AST_LOGICAL_AND_EXPRESSION,
        create_,
        3,
        { AST_LOGICAL_AND_EXPRESSION, AST_AMPERSAND_AMPERSAND, AST_INCLUSIVE_OR_EXPRESSION }
    },
    {
        AST_LOGICAL_AND_EXPRESSION,
        create_elided_node,
        1,
        { AST_INCLUSIVE_OR_EXPRESSION }
    },
    /* inclusive-or-expression: */
    {
        AST_INCLUSIVE_OR_EXPRESSION,
        create_,
        3,
        { AST_INCLUSIVE_OR_EXPRESSION, AST_VERTICALBAR, AST_EXCLUSIVE_OR_EXPRESSION }
    },
    {
        AST_INCLUSIVE_OR_EXPRESSION,
        create_elided_node,
        1,
        { AST_EXCLUSIVE_OR_EXPRESSION }
    },
    /* exclusive-or-expression: */
    {
        AST_EXCLUSIVE_OR_EXPRESSION,
        create_,
        3,
        { AST_EXCLUSIVE_OR_EXPRESSION, AST_CARET, AST_AND_EXPRESSION }
    },
    {
        AST_EXCLUSIVE_OR_EXPRESSION,
        create_elided_node,
        1,
        { AST_AND_EXPRESSION }
    },
    /* and-expression: */
    {
        AST_AND_EXPRESSION,
        create_,
        3,
        { AST_AND_EXPRESSION, AST_AMPERSAND, AST_EQUALITY_EXPRESSION }
    },
    {
        AST_AND_EXPRESSION,
        create_elided_node,
        1,
        { AST_EQUALITY_EXPRESSION }
    },
    /* equality-expression: */
    {
        AST_EQUALITY_EXPRESSION,
        create_,
        3,
        { AST_EQUALITY_EXPRESSION, AST_EQ, AST_RELATIONAL_EXPRESSION }
    },
    {
        AST_EQUALITY_EXPRESSION,
        create_,
        3,
        { AST_EQUALITY_EXPRESSION, AST_NEQ, AST_RELATIONAL_EXPRESSION }
    },
    {
        AST_EQUALITY_EXPRESSION,
        create_elided_node,
        1,
        { AST_RELATIONAL_EXPRESSION }
    },
    /* relational-expression: */
    {
        AST_RELATIONAL_EXPRESSION,
        create_,
        3,
        { AST_RELATIONAL_EXPRESSION, AST_LT, AST_SHIFT_EXPRESSION }
    },
    {
        AST_RELATIONAL_EXPRESSION,
        create_,
        3,
        { AST_RELATIONAL_EXPRESSION, AST_GT, AST_SHIFT_EXPRESSION }
    },
    {
        AST_RELATIONAL_EXPRESSION,
        create_,
        3,
        { AST_RELATIONAL_EXPRESSION, AST_LTEQ, AST_SHIFT_EXPRESSION }
    },
    {
        AST_RELATIONAL_EXPRESSION,
        create_,
        3,
        { AST_RELATIONAL_EXPRESSION, AST_GTEQ, AST_SHIFT_EXPRESSION }
    },
    {
        AST_RELATIONAL_EXPRESSION,
        create_elided_node,
        1,
        { AST_SHIFT_EXPRESSION }
    },
    /* shift-expression: */
    {
        AST_SHIFT_EXPRESSION,
        create_,
        3,
        { AST_SHIFT_EXPRESSION, AST_SHIFTLEFT, AST_ADDITIVE_EXPRESSION }
    },
    {
        AST_SHIFT_EXPRESSION,
        create_,
        3,
        { AST_SHIFT_EXPRESSION, AST_SHIFTRIGHT, AST_ADDITIVE_EXPRESSION }
    },
    {
        AST_SHIFT_EXPRESSION,
        create_elided_node,
        1,
        { AST_ADDITIVE_EXPRESSION }
    },
    /* additive-expression: */
    {
        AST_ADDITIVE_EXPRESSION,
        create_binary_op,
        3,
        { AST_ADDITIVE_EXPRESSION, AST_PLUS, AST_MULTIPLICATIVE_EXPRESSION }
    },
    {
        AST_ADDITIVE_EXPRESSION,
        create_binary_op,
        3,
        { AST_ADDITIVE_EXPRESSION, AST_MINUS, AST_MULTIPLICATIVE_EXPRESSION }
    },
    {
        AST_ADDITIVE_EXPRESSION,
        create_elided_node,
        1,
        { AST_MULTIPLICATIVE_EXPRESSION }
    },
    /* multplicative-expression: */
    {
        AST_MULTIPLICATIVE_EXPRESSION,
        create_,
        3,
        { AST_MULTIPLICATIVE_EXPRESSION, AST_ASTERISK, AST_CAST_EXPRESSION }
    },
    {
        AST_MULTIPLICATIVE_EXPRESSION,
        create_,
        3,
        { AST_MULTIPLICATIVE_EXPRESSION, AST_BACKSLASH, AST_CAST_EXPRESSION }
    },
    {
        AST_MULTIPLICATIVE_EXPRESSION,
        create_,
        3,
        { AST_MULTIPLICATIVE_EXPRESSION, AST_MOD, AST_CAST_EXPRESSION }
    },
    {
        AST_MULTIPLICATIVE_EXPRESSION,
        create_,
        1,
        { AST_CAST_EXPRESSION }
    },
    /* cast-expression: */
    {
        AST_CAST_EXPRESSION,
        create_,
        1,
        { AST_UNARY_EXPRESSION }
    },
    /* unary-expression: */
    {
        AST_UNARY_EXPRESSION,
        create_,
        2,
        { AST_PLUS_PLUS, AST_UNARY_EXPRESSION }
    },
    {
        AST_UNARY_EXPRESSION,
        create_,
        2,
        { AST_MINUS_MINUS, AST_UNARY_EXPRESSION }
    },
    {
        AST_UNARY_EXPRESSION,
        create_,
        2,
        { AST_AMPERSAND, AST_CAST_EXPRESSION }
    },
    {
        AST_UNARY_EXPRESSION,
        create_,
        2,
        { AST_ASTERISK, AST_CAST_EXPRESSION }
    },
    {
        AST_UNARY_EXPRESSION,
        create_,
        2,
        { AST_PLUS, AST_CAST_EXPRESSION }
    },
    {
        AST_UNARY_EXPRESSION,
        create_,
        2,
        { AST_MINUS, AST_CAST_EXPRESSION }
    },
    {
        AST_UNARY_EXPRESSION,
        create_,
        1,
        { AST_POSTFIX_EXPRESSION }
    },
    /* postfix-expression: */
    {
        AST_POSTFIX_EXPRESSION,
        create_,
        3,
        { AST_POSTFIX_EXPRESSION, AST_ARROW, AST_IDENTIFIER }
    },
    {
        AST_POSTFIX_EXPRESSION,
        create_,
        2,
        { AST_POSTFIX_EXPRESSION, AST_PLUS_PLUS }
    },
    {
        AST_POSTFIX_EXPRESSION,
        create_,
        2,
        { AST_POSTFIX_EXPRESSION, AST_MINUS_MINUS }
    },
    {
        AST_POSTFIX_EXPRESSION,
        create_,
        1,
        { AST_PRIMARY_EXPRESSION }
    },
    /* primary-expression: */
    {
        AST_PRIMARY_EXPRESSION,
        create_,
        1,
        { AST_IDENTIFIER }
    },
    {
        AST_PRIMARY_EXPRESSION,
        create_,
        1,
        { AST_CONSTANT }
    },
    /* constant: */
    {
        AST_CONSTANT,
        create_,
        1,
        { AST_INTEGER_CONSTANT }
    },
    {
        AST_CONSTANT,
        create_,
        1,
        { AST_CHARACTER_CONSTANT }
    },
};

#endif

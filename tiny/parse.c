/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "parse.h"
#include "scan.h"
#include "util.h"

static TokenType token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode *stmt_sequence(void);
static TreeNode *statement(void);
static TreeNode *if_stmt(void);
static TreeNode *repeat_stmt(void);
static TreeNode *assign_stmt(void);
static TreeNode *read_stmt(void);
static TreeNode *write_stmt(void);
static TreeNode *exp(void);
static TreeNode *bitwise_exp(void);
static TreeNode *simple_exp(void);
static TreeNode *term(void);
static TreeNode *factor(void);
static TreeNode *power(void);
static TreeNode *for_stmt(void);

/*
assign-stmt → identifier := regexp | identifier := exp
regexp → regexp-unit {regex-op regexp-unit}
regex-op → ‘|’ | ‘&’ | ‘#’
regexp-unit → identifier | (regexp) | regexp-closure
regexp-closure → identifier# | (regexp)#
*/
static TreeNode *regexp(void);
static TreeNode *regexp_unit(void);
static TreeNode *regexp_closure(void);

static void syntaxError(char *message)
{
    fprintf(listing, "\n>>> ");
    fprintf(listing, "Syntax error at line %d: %s", lineno, message);
    Error = TRUE;
}

static void match(TokenType expected)
{
    if (token == expected)
        token = getToken();
    else
    {
        syntaxError("MATCH: unexpected token -> ");
        printToken(token, tokenString);
        fprintf(listing, "      ");
    }
}

TreeNode *stmt_sequence(void)
{
    TreeNode *t = statement();
    TreeNode *p = t;
    while ((token != ENDFILE) && (token != END) && (token != ELSE) && (token != UNTIL) && (token != ENDDO))
    {
        TreeNode *q;
        // match(SEMI);
        if (token == SEMI)
            match(SEMI);

        q = statement();
        if (q != NULL)
        {
            if (t == NULL)
                t = p = q;
            else /* now p cannot be NULL either */
            {
                p->sibling = q;
                p = q;
            }
        }
    }
    return t;
}

TreeNode *statement(void)
{
    TreeNode *t = NULL;
    switch (token)
    {
    case IF:
        t = if_stmt();
        break;
    case REPEAT:
        t = repeat_stmt();
        break;
    case ID:
        t = assign_stmt();
        break;
    case READ:
        t = read_stmt();
        break;
    case WRITE:
        t = write_stmt();
        break;
    case FOR:
        t = for_stmt();
        break;
    case ENDFILE:
    case ENDDO:
    case ELSE:
        break;
    default:
        syntaxError("STMT: unexpected token -> ");
        printToken(token, tokenString);
        token = getToken();
        break;
    } /* end case */
    return t;
}

TreeNode *if_stmt(void)
{
    TreeNode *t = newStmtNode(IfK);

    match(IF);

    match(LPAREN);
    if (t != NULL)
        t->child[0] = exp();
    match(RPAREN);

    if (t != NULL)
        t->child[1] = stmt_sequence();

    if (token == ELSE)
    {
        match(ELSE);
        if (t != NULL)
            t->child[2] = stmt_sequence();
    }

    return t;
}

TreeNode *repeat_stmt(void)
{
    TreeNode *t = newStmtNode(RepeatK);
    match(REPEAT);
    if (t != NULL)
        t->child[0] = stmt_sequence();
    match(UNTIL);
    if (t != NULL)
        t->child[1] = exp();
    return t;
}

TreeNode *assign_stmt(void)
{
    TreeNode *t = newStmtNode(AssignK);
    if ((t != NULL) && (token == ID))
        t->attr.name = copyString(tokenString);
    match(ID);

    // a += 1 -> a = a + 1
    if (token == PLUSEQ)
    {
        match(PLUSEQ);
        TreeNode *opNode = newExpNode(OpK);
        if (opNode != NULL)
        {
            TreeNode *idNode = newExpNode(IdK);
            if (t != NULL)
                idNode->attr.name = t->attr.name;

            if (opNode != NULL)
            {
                opNode->child[0] = idNode;
                opNode->attr.op = PLUS;
                opNode->child[1] = exp();
            }
        }

        if (t != NULL)
        {
            t->child[0] = opNode;
        }
    }
    else if (token == ASSIGN)
    {
        match(ASSIGN);
        if (t != NULL)
        {
            if (token == REGEXP)
            {
                t->child[0] = regexp();
            }
            else
            {
                t->child[0] = exp();
            }
        }
    }
    return t;
}

TreeNode *read_stmt(void)
{
    TreeNode *t = newStmtNode(ReadK);
    match(READ);
    if ((t != NULL) && (token == ID))
        t->attr.name = copyString(tokenString);
    match(ID);
    return t;
}

TreeNode *write_stmt(void)
{
    TreeNode *t = newStmtNode(WriteK);
    match(WRITE);
    if (t != NULL)
        t->child[0] = exp();
    return t;
}

// bitwise-exp -> term {bitwise-op term}
TreeNode *bitwise_exp(void)
{
    TreeNode *t = term();
    while ((token == BITAND) || (token == BITOR))
    {
        TreeNode *p = newExpNode(OpK);
        if (p != NULL)
        {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            t->child[1] = term();
        }
    }
    return t;
}

TreeNode *exp(void)
{
    TreeNode *t = simple_exp();
    // =（等于），>(大于)、<=(小于等于)、>=(大于等于)、<>(不等于)等运算符号，
    if ((token == LT) || (token == EQ) || (token == GT) || (token == LTEQ) || (token == GTEQ) || (token == NEQ))
    {
        TreeNode *p = newExpNode(OpK);
        if (p != NULL)
        {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
        }
        match(token);
        if (t != NULL)
            t->child[1] = simple_exp();
    }
    return t;
}

// simple-exp -> bitwise-exp { addop bitwise-exp}
TreeNode *simple_exp(void)
{
    TreeNode *t = bitwise_exp();
    while ((token == PLUS) || (token == MINUS))
    {
        TreeNode *p = newExpNode(OpK);
        if (p != NULL)
        {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            t->child[1] = bitwise_exp();
        }
    }
    return t;
}

// term -> {bitnot} term
TreeNode *term(void)
{
    TreeNode *t = power();
    while ((token == TIMES) || (token == OVER) || (token == MOD))
    {
        TreeNode *p = newExpNode(OpK);
        if (p != NULL)
        {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            p->child[1] = power();
        }
    }
    return t;
}

// Function to handle exponentiation
TreeNode *power(void)
{
    TreeNode *t = factor();
    while (token == POWER)
    {
        TreeNode *p = newExpNode(OpK);
        if (p != NULL)
        {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            p->child[1] = factor(); // Right associativity
        }
    }
    return t;
}

TreeNode *factor(void)
{
    TreeNode *t = NULL;
    switch (token)
    {
    case BITNOT:
        t = newExpNode(UnaryOpK);
        if ((t != NULL) && (token == BITNOT))
            t->attr.op = token;
        match(BITNOT);
        if (t != NULL)
            t->child[0] = factor();
        break;
    case NUM:
        t = newExpNode(ConstK);
        if ((t != NULL) && (token == NUM))
            t->attr.val = atoi(tokenString);
        match(NUM);
        break;
    case ID:
        t = newExpNode(IdK);
        if ((t != NULL) && (token == ID))
            t->attr.name = copyString(tokenString);
        match(ID);
        break;
    case REGEXP:
        t = regexp();
        break;
    case LPAREN:
        match(LPAREN);
        t = exp();
        match(RPAREN);
        break;
    default:
        syntaxError("FACTOR: unexpected token -> ");
        printToken(token, tokenString);
        token = getToken();
        break;
    }
    return t;
}

/*
(1) for-stmt-->for identifier:=simple-exp  to  simple-exp  do  stmt-sequence enddo    步长递增1
(2) for-stmt-->for identifier:=simple-exp  downto  simple-exp  do  stmt-sequence enddo    步长递减1

combined: for-stmt-->for identifier:=simple-exp  to|downto  simple-exp  do  stmt-sequence enddo
*/
TreeNode *for_stmt(void)
{
    TreeNode *t = newStmtNode(ForK);
    match(FOR);
    if ((t != NULL) && (token == ID))
        t->child[0] = assign_stmt();

    if (token == TO || DOWNTO)
    {
        match(token);
        if (t != NULL)
            t->child[1] = simple_exp();
    }

    match(DO);
    if (t != NULL)
        t->child[2] = stmt_sequence();

    match(ENDDO);

    return t;
}

static TreeNode *regexp_unit(void)
{
    TreeNode *t = NULL;

    switch (token)
    {
    case ID:
        t = newExpNode(IdK);
        if ((t != NULL) && (token == ID))
            t->attr.name = copyString(tokenString);
        match(ID);
        break;
    case LPAREN:
        match(LPAREN);
        t = regexp();
        match(RPAREN);
        break;
    case REGEXPCLOSURE:
        match(REGEXPCLOSURE);
        break;
    default:
        syntaxError("REGEXP_UNIT: unexpected token -> ");
        printToken(token, tokenString);
        token = getToken();
        break;
    }

    return t;
}

static TreeNode *regexp_closure(void)
{
    TreeNode *t = NULL;

    switch (token)
    {
    case ID:
        t = newExpNode(IdK);
        if ((t != NULL) && (token == ID))
            t->attr.name = copyString(tokenString);
        match(ID);
        break;
    case LPAREN:
        match(LPAREN);
        t = regexp();
        match(RPAREN);
        break;
    default:
        syntaxError("REGEXP_CLOSURE: unexpected token -> ");
        printToken(token, tokenString);
        token = getToken();
        break;
    }

    return t;
}

static TreeNode *regexp(void)
{
    match(REGEXP);

    TreeNode *t = NULL;
    t = regexp_unit();

    // Binary Operation
    while ((token == REGEXPOR) || (token == REGEXPCONCAT))
    {
        TreeNode *p = newExpNode(OpK);
        if (p != NULL)
        {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            p->child[1] = regexp_unit();
        }
    }

    // Unary Operation
    while ((token == REGEXPCLOSURE) || (token == REGEXPOPTIONAL))
    {
        TreeNode *p = newExpNode(UnaryOpK);
        if (p != NULL)
        {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
        }
    }

    match(REGEXP);
    return t;
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly
 * constructed syntax tree
 */
TreeNode *parse(void)
{
    TreeNode *t;
    token = getToken();
    t = stmt_sequence();
    if (token != ENDFILE)
        syntaxError("Code ends before file\n");
    return t;
}

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// kind of token
typedef enum {
    TK_RESERVED,    // symbols
    TK_NUM,         // integer token
    TK_EOF,         // EOF token
} TokenKind;

typedef struct Token Token;


struct Token {
    TokenKind kind; // kind of token
    Token *next;    // next input token
    int val;        // vlaue if kind == TK_NUM
    char *str;      // token string
};

// currently focused token
Token *token;

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// If the next token is the expected symbol, consume it and return true;
// otherwise, return fales.
bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op) return false;
    token = token->next;
    return true;
}

// If the next token is the expected symbol, consume it;
// otherwise, report an error
void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op) error("Not '%c'.", op);
    token = token->next;
}

// If the next token is a number, consume it and return the numeric value.
// Otherwise, report an error
int expect_number() {
    if (token->kind != TK_NUM) error("Not number");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

// Create a new token and link it to 'cur'
Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

// Tokenize an input string 'p' and return it.
Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        // Skip whitespace character.
        if (isspace(*p)) {
            p++;
            continue;
        }
        
        if (*p == '+' || *p == '-') {
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error("Cannot tokenize");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Incorrect number of arguments");
        return 1;
    }

    // Tokenize
    token = tokenize(argv[1]);

    char *p = argv[1];

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("    mov rax, %d\n", expect_number());

    while (!at_eof()) {
        if (consume('+')) {
            printf("    add rax, %d\n", expect_number());
            continue;
        }

            expect('-');
            printf("    sub rax, %d\n", expect_number());
    }

    printf("    ret\n");
        
    return 0;
}

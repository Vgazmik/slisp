#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpc.h"

//If on Windoze
#ifdef _WIN32
#include <string.h>
static char buffer[2048];
//Fake readline
char* readline(char* prompt) {
	fputs(prompt, stdout);
	fgets(buffer, 2048, stdin);
	char* cpy = malloc(strlen(buffer)+1);
	strcpy(cpy, buffer);
	cpy[strlen(cpy)-1] = '\0';
	return cpy;
}
//Fake add_history
void add_history(char* unused) {}
//Not on Windoze
#else
//For editline capability
#include <editline/readline.h>
#include <editline/history.h>
#endif

long eval_op(long x, char* op, long y) {
    if(strcmp(op, "+") == 0) {return x + y;}
    if(strcmp(op, "-") == 0) {return x - y;}
    if(strcmp(op, "*") == 0) {return x * y;}
    if(strcmp(op, "/") == 0) {return x / y;}
    if(strcmp(op, "%") == 0) {return x % y;}
    if(strcmp(op, "^") == 0) {return pow(x, y);}
    //if(strcmp(op, "min") == 0) {return (x < y) ? x : y;}
    //if(strcmp(op, "max") == 0) {return (x > y) ? x : y;}

    return 0;
}

long eval(mpc_ast_t* t) {
    //Base Case
    if(strstr(t->tag, "number")) {
        return atoi(t->contents);
    }
    //Expression
    char* op = t->children[1]->contents;
    long x = eval(t->children[2]);

    //Iterate Remaining Children
    int i = 3;
    while(strstr(t->children[i]->tag, "expr")) {
         x = eval_op(x, op, eval(t->children[i]));
         i++;
    }

    return x;
}

int main(int argc, char** argv) {
    //Create some parsers
    mpc_parser_t* Number   = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr     = mpc_new("expr");
    mpc_parser_t* Slisp    = mpc_new("slisp");

    //Define language
    mpca_lang(MPCA_LANG_DEFAULT,
        "number   : /-?[0-9]+/;                           "
        "operator : '+' | '-' | '*' | '/' | '^' | '%';    "
        "expr     : <number> | '(' <operator> <expr>+ ')';"
        "slisp    : /^/ <operator> <expr>+ /$/;           ",
        Number, Operator, Expr, Slisp);

    //Version Info
	puts("Lispy Version 0.0.0.0.3");
	puts("Press Ctrl+C to Exit\n");

	//Begin processing input
	while(1) {
		//Get user input and store a reference
		char* input = readline("slisp> ");
		//Add input to memory
		add_history(input);
		//Process input
        mpc_result_t r;

        if(mpc_parse("<stdin>", input, Slisp, &r)) {
             long result = eval(r.output);

             printf("%li\n", result);
             mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        //Free input reference memory
		free(input);
	}

    //Undefine and clean up parsers
    mpc_cleanup(4, Number, Operator, Expr, Slisp);

	return 0;
}

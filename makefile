slisp: repl.c mpc.c
	cc -std=c99 -Wall repl.c mpc.c -lm -ledit -o repl
doge: doge.c mpc.c
	cc -std=c99 -Wall doge.c mpc.c -lm -o wow

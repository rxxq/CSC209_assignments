build:
	gcc shell.c parser.c -o shell -g -Wall

clean:
	rm -f shell


backup:
	cp -i shell.c shell.c.bak

# Complie a short program for testing in, out, err redirection
redirection: redirection.c
	gcc -g -Wall -o redirection redirection.c

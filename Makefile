all:
	flex Proiect.l
	bison -d Proiect.y -Wcounterexamples
	gcc lex.yy.c Proiect.tab.c
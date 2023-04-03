#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
   
#define SIZE 105

extern int yylineno; 

struct Variables{
    char* nume, *tip, *valoare;
    bool isConst;
}vars[SIZE];
int nr_var, nr_val_n;
char* variabileNedeclarate[SIZE];

struct Nod
{
    char* informatie;
    struct Nod *st, *dr;
};


void yyerror(char * s){
  printf("Eroare: %s la linia:%d\n",s,yylineno);
  exit(1);
}

char* TransformaIntToString(int x)
{
    char *rez = (char*) malloc(20);
    int nr = 0, cx = x;

    while(cx) cx /= 10, nr++;
    while(x) rez[nr-1] = (char)(x % 10 + '0'), x /= 10, nr--;

    return rez;
}

char* TransformaFloatToString(float x)
{
    const int Precizie = 10000000;
    int ParteaIntreaga = (int)(x), ParteaFractionara;

    x -= ParteaIntreaga;
    x *= Precizie;
    ParteaFractionara = (int)(x);
    while(ParteaFractionara != 0 && ParteaFractionara % 10 == 0) ParteaFractionara /= 10;

    char *rez = (char*) malloc(41);

    if(ParteaIntreaga == 0) rez[0] = '0';
    else rez = TransformaIntToString(ParteaIntreaga);

    if(ParteaFractionara == 0) strcat(rez, ".0");
    else
    {
        strcat(rez, ".");
        strcat(rez, TransformaIntToString(ParteaFractionara));
    }

    return rez;
}


void adaugaVar(char* tip, char* nume){
      for(int i = 0; i < nr_var; i++) if(strcmp(nume, vars[i].nume) == 0)
      {
            char rasp[100] = "variabila ";
            strcat(rasp, nume);
            strcat(rasp, " este deja declarata.");

            yyerror(rasp);
      }
      if(strcmp(tip, "int")==0) vars[nr_var].valoare = "0";
      if(strcmp(tip, "float")==0) vars[nr_var].valoare = "0.0";
      if(strcmp(tip, "bool")==0) vars[nr_var].valoare = "0";
      if(strcmp(tip, "char")==0 || strcmp(tip, "string")==0) vars[nr_var].valoare = "\0";
      vars[nr_var].tip = strdup(tip);
      vars[nr_var].nume = strdup(nume);
      nr_var++;
}

void adaugaVarConst(char* tip, char* nume, char* valoare){
      for(int i = 0; i < nr_var; i++) if(strcmp(nume, vars[i].nume) == 0)
      {
            char rasp[100] = "variabila ";
            strcat(rasp, nume);
            strcat(rasp, " este deja declarata.");

            yyerror(rasp);
      }
      
      vars[nr_var].tip = strdup(tip);
      vars[nr_var].nume = strdup(nume);
      vars[nr_var].isConst = true;
      vars[nr_var].valoare = strdup(valoare);
      nr_var++;
}

void adaugaVariabilaNedeclarata(char* nume)
{
      variabileNedeclarate[nr_val_n++] = nume;
}

void adaugaVariabile(char* tip)
{
      int i;
      for(i = 0; i < nr_val_n; i++)
      {
            adaugaVar(tip, variabileNedeclarate[i]);
      }

      nr_val_n = 0;
}


char* getTypeVar(char * nume){
    int i;
    for(i = 0; i < nr_var; i++)
    {
        if(strcmp(nume, vars[i].nume) == 0) return vars[i].tip;
    }

    char rasp[100] = "variabila ";
    strcat(rasp, nume);
    strcat(rasp, " nu exista.");

    yyerror(rasp);
}

char* getValoareVariabila(char *nume)
{
    for(int i = 0; i < nr_var; i++) if(strcmp(vars[i].nume, nume) == 0) return vars[i].valoare;

    char rasp[100] = "variabila ";
    strcat(rasp, nume);
    strcat(rasp, " nu exista.");
    yyerror(rasp);
}


int setVal(char* nume, char* valoare)
{
    for(int i = 0; i < nr_var; i++)
    {
        if(strcmp(nume, vars[i].nume) == 0)
        {
            if(vars[i].isConst == true) yyerror("Variabilele constante nu pot fi reasignate");
            vars[i].valoare = valoare;
            break;
        }
    }
    return 0;
}
char* TypeOf(char* nume)
{
    int n = strlen(nume), i, j, Nr_Paranteze = 0;
    char* Tip = (char*) "";
    char* Operatori = (char*) "()+-*/^|&";
      char* s = (char*) malloc(n+1);
      strcpy(s, nume);  


      if(n==0) yyerror("Variabila nu a fost initializata");
      if(s[0] == (char)39 && s[2] == (char)39 && n == 3) return (char*) "char";
      else if(n >= 2 && s[0] == (char)34  && s[n-1] == (char)34) return (char*) "string";
      if(strcmp(s, "true")==0 || strcmp(s, "false")==0) {
             return (char*) "bool";
      }
    // Scoatem toate spatiile din s.
    j = 0;
    for(i = 0; i < n; i++)
    {
      if(s[i] == (char)34 || s[i] == (char)39)
        {   
            yyerror("Incompatibilitate de tipuri");
        }
       else if(s[i] != ' ' && s[i] != '\t') s[j++] = s[i];
       
    }
    s[j] = '\0', n = j;

    // Verificam daca exista 2 operatori unul dupa altul, fara paranteze.
    for(i = 0; i < n - 1; i++) if(strchr("+-*/^|&", s[i]) != NULL && strchr("+-*/^|&", s[i+1]) != NULL) yyerror("Eroare semantica (2 operatori consecutivi)");

    for(i = 0; i < n; i++)
    {
        if(strchr(Operatori, s[i]) == NULL)
        {
            if(s[i] == '.') yyerror("Eroare semantica (Expresia contine un punct neprecedat de o cifra)");
            else if(s[i] >= '0' && s[i] <= '9')
            {
                if(s[i] == '0' && i < n - 1 && (s[i+1] >= '0' && s[i+1] <= '9')) yyerror("Eroare semantica (Expresia contine un numar care incepe cu cifra 0.");
                while(i < n && s[i] >= '0' && s[i] <= '9') i++;
                if(i < n && s[i] == '.')
                {
                  i++;
                    while(i < n && s[i] >= '0' && s[i] <= '9') i++;
                    
                    if(i < n && s[i] == '.') yyerror("Eroare semantica (Expresia contine o secventa de numere intercalata de cel putin 2 puncte)");
                    else
                    {
                        if(Tip == "") Tip = (char*) "float";
                        else if(strcmp(Tip, "float") != 0) yyerror("Eroare semantica (Expresia contine variabile de tipuri diferite)");
                    }
                }
                else
                {
                    if(i < n && strchr(Operatori, s[i]) == NULL) yyerror("Eroare semantica (Numele unei variabile incepe cu o cifra)");
                    else if(Tip == "") Tip = (char*) "int";
                    else if(strcmp(Tip, "int") != 0) yyerror("Eroare semantica (Expresia contine variabile de tipuri diferite)");
                }
                i--;
            }
            else
            {
                j = i;
                while(j < n && s[j] != '.' && strchr(Operatori, s[j]) == NULL) j++;

                char* nume = (char*) malloc(j-i+1);
                j = i;
                while(j < n && s[j] != '.' && strchr(Operatori, s[j]) == NULL)
                {
                    nume[j-i] = s[j];
                    j++;
                }
                i = j - 1;

                if(Tip == "") Tip = getTypeVar(nume);
                else if(strcmp(Tip, getTypeVar(nume)) != 0) yyerror("Eroare semantica (Expresia contine variabile de tipuri diferite)");
            }
        }
        else if(s[i] == '(') Nr_Paranteze++;
        else if(s[i] == ')') Nr_Paranteze--;

        if(Nr_Paranteze < 0) yyerror("Expresia nu este parantetizata corect.");
    }

    if(Nr_Paranteze != 0) yyerror("Expresia nu este parantetizata corect.");
    return Tip;
}



int Prioritate(char Operator)
{
    if(Operator == '+' || Operator == '-') return 1;
    else if(Operator == '/' || Operator == '*') return 2;
}

struct Nod* ConstruiesteArbore(char* s)
{
    int n = (int)(strlen(s)), i, j, capat1 = -1, capat2 = -1;
    struct Nod *s1[1005], *s2[1005];
    char *Operatori = (char*) "+-*/", *Tip = TypeOf(s);
    
    if(s[0] == (char)39 || s[0] == (char)34)
    {
        struct Nod* raspuns = (struct Nod*)(malloc(sizeof(struct Nod)));
        raspuns->informatie = (char*)(malloc(n-1)), raspuns->st = NULL, raspuns->dr = NULL;
        
        for(i = 1; i < n - 1; i++) raspuns->informatie[i-1] = s[i];
        return raspuns;
    }

    for(i = 0; i < n; i++) if(strchr(" \t", s[i]) == NULL)
        {
            if(s[i] >= '0' && s[i] <= '9')
            {
                j = i;
                while(j < n && s[j] >= '0' && s[j] <= '9') j++;
                if(j < n && s[j] == '.')
                {
                    while(j < n && s[j] >= '0' && s[j] <= '9') j++;
                }

                s1[++capat1] = (struct Nod*) (malloc(sizeof(struct Nod)));
                s1[capat1]->informatie = (char*) (malloc(j-i+1)), s1[capat1]->st = NULL, s1[capat1]->dr = NULL;

                j = i;
                while(j < n && s[j] >= '0' && s[j] <= '9')
                {
                    s1[capat1]->informatie[j-i] = s[j];
                    j++;
                }
                if(j < n && s[j] == '.')
                {
                    s1[capat1]->informatie[j-i] = '.';
                    while(j < n && s[j] >= '0' && s[j] <= '9')
                    {
                        s1[capat1]->informatie[j-i] = s[j];
                        j++;
                    }
                }

                //s1[capat1]->informatie[j-i] = '\n';
                i = j - 1;
            }
            else if(s[i] == '(')
            {
                s2[++capat2] = (struct Nod*) (malloc(sizeof(struct Nod)));
                s2[capat2]->informatie = (char*) (malloc(2)), s2[capat2]->st = NULL, s2[capat2]->dr = NULL;
                s2[capat2]->informatie[0] = '(';//s2[capat2]->informatie[1] = '\n';
            }
            else if(s[i] == ')')
            {
                while(s2[capat2]->informatie[0] != '(')
                {
                    capat1--;
                    s2[capat2]->st = s1[capat1], s2[capat2]->dr = s1[capat1+1];
                    s1[capat1] = s2[capat2];
                    capat2--;
                }
                capat2--;
            }
            else if(strchr(Operatori, s[i]) != NULL)
            {
                while(capat2 != -1 && s2[capat2]->informatie[0] != '(' && Prioritate(s2[capat2]->informatie[0]) >= Prioritate(s[i]))
                {
                    capat1--;
                    s2[capat2]->st = s1[capat1], s2[capat2]->dr = s1[capat1+1];
                    s1[capat1] = s2[capat2];
                    capat2--;
                }
                s2[++capat2] = (struct Nod*) (malloc(sizeof(struct Nod)));
                s2[capat2]->informatie = (char*) (malloc(2)), s2[capat2]->st = NULL, s2[capat2]->dr = NULL;
                s2[capat2]->informatie[0] = s[i]; //s2[capat2]->informatie[1] = '\n';
            }
            else // Daca incepe numele unei variabile
            {
                while(j < n && s[j] != '.' && strchr(Operatori, s[j]) == NULL)
                {
                    j++;
                }
                s1[++capat1] = (struct Nod*) (malloc(sizeof(struct Nod)));
                s1[capat1]->informatie = (char*) (malloc(j-i+1)), s1[capat1]->st = NULL, s1[capat1]->dr = NULL;

                j = i;
                while(j < n && s[j] != '.' && strchr(Operatori, s[j]) == NULL)
                {
                    s1[capat1]->informatie[j-i] = s[j];
                    j++;
                }

                //s1[capat1]->informatie[j-i] = '\n';
                i = j - 1;
            }
        }

    while(capat2 != -1)
    {
        capat1--;
        s2[capat2]->st = s1[capat1], s2[capat2]->dr = s1[capat1+1];
        s1[capat1] = s2[capat2];
        capat2--;
    }

    return s1[capat1];
}

char* Eval(struct Nod* rad, char* Tip)
{
    char *rez = (char*) malloc(20), *Operatori = (char*) "+-*/";

    if (strcmp(Tip,"int") == 0)
    {
        if((rad->informatie[0] < '0' || rad->informatie[0] > '9') && strchr(Operatori, rad->informatie[0]) == NULL) rad->informatie = getValoareVariabila(rad->informatie);

        if (strcmp(rad->informatie, "+") == 0)
        {
            int x = atoi(Eval(rad->st, Tip)) + atoi(Eval(rad->dr, Tip));
            return TransformaIntToString(x);
        }
        else if (strcmp(rad->informatie, "-") == 0)
        {
            int x = atoi(Eval(rad->st, Tip)) - atoi(Eval(rad->dr, Tip));
            return TransformaIntToString(x);
        }
        else if (strcmp(rad->informatie, "*") == 0)
        {
            int x = atoi(Eval(rad->st, Tip)) * atoi(Eval(rad->dr, Tip));
            return TransformaIntToString(x);
        }
        else if (strcmp(rad->informatie, "/") == 0)
        {
            int x = atoi(Eval(rad->st, Tip)) / atoi(Eval(rad->dr, Tip));
            return TransformaIntToString(x);
        }

        return rad->informatie;
    }
    else if(strcmp(Tip, "float") == 0)
    {
        if((rad->informatie[0] < '0' || rad->informatie[0] > '9') && strchr(Operatori, rad->informatie[0]) == NULL) rad->informatie = getValoareVariabila(rad->informatie);

        if (strcmp(rad->informatie, "+") == 0)
        {
            float x = atof(Eval(rad->st, Tip)) + atof(Eval(rad->dr, Tip));
            return TransformaFloatToString(x);
        }
        else if (strcmp(rad->informatie, "-") == 0)
        {
            float x = atof(Eval(rad->st, Tip)) - atof(Eval(rad->dr, Tip));
            return TransformaFloatToString(x);
        }
        else if (strcmp(rad->informatie, "*") == 0)
        {
            float x = atof(Eval(rad->st, Tip)) * atof(Eval(rad->dr, Tip));
            return TransformaFloatToString(x);
        }
        else if (strcmp(rad->informatie, "/") == 0)
        {
            float x = atof(Eval(rad->st, Tip)) / atof(Eval(rad->dr, Tip));
            return TransformaFloatToString(x);
        }

        return rad->informatie;
    }
    else rad->informatie;
}


struct Parameter{
    char* type, *name;
};

struct Functions{
    char* nume, * ret_type;
    int nr_params;
    struct Parameter param[SIZE];
}fncts[SIZE];

int nr_fncts;

char param_list[SIZE][SIZE];
int param_list_size;


void addFunction(char* ret_type, char* nume){
    for(int i = 0; i < nr_fncts; i++)
        if(strcmp(nume, fncts[i].nume) == 0)
        {
            printf("Eroare: o functie cu acest nume exista deja\n");
            exit(1);
        }

    fncts[nr_fncts].nume = strdup(nume);
    fncts[nr_fncts].ret_type = strdup(ret_type);
    fncts[++nr_fncts].nr_params = 0;
}

void addParameter(char* param_type, char* param_name){
    for(int i = 0; i < fncts[nr_fncts].nr_params; i++)
        if(strcmp(param_name, fncts[nr_fncts].param[i].name) == 0)
        {
            printf("Eroare: doi parametri nu pot avea acelasi nume\n");
            exit(1);
        }

    fncts[nr_fncts].param[fncts[nr_fncts].nr_params].name = strdup(param_name);
    fncts[nr_fncts].param[fncts[nr_fncts].nr_params].type = strdup(param_type);
    fncts[nr_fncts].nr_params++;
}

void addToParamList(char* tip){
    strcpy(param_list[param_list_size], tip);
    param_list_size++;
}

char* GetReturnType(char* functie){
      int i;
      for(i = 0; i < nr_fncts; i++)
        if(strcmp(functie, fncts[i].nume) == 0)
            break;
      if(nr_fncts == i) 
            yyerror("Functia apelata nu a fost declarata\n");
      return fncts[i].ret_type;
}

char* BoolToString(bool b){
      if (b) return "true";
      else return "false";
}

bool StringToBool(char* b){
      return strcmp(b, "true");
}

void checkFunctionCall(char* name){
    int i;
    for(i = 0; i < nr_fncts; i++)
        if(strcmp(name, fncts[i].nume) == 0)
            break;
    if(nr_fncts == i) {
        exit(1); //fct nu exista
    }

      printf("functia: %s are %d parametrii. in lista sunt %d parametri\n", fncts[i].nume, fncts[i].nr_params, param_list_size); 

    if(fncts[i].nr_params != param_list_size) {
            char err[100]="";
            sprintf(err, "Eroare: numar diferit de parametrii la apelul functiei %s\n", name);
            printf("%s",err);
            exit(1);
    } //nr diferit de parametri

    for(int j=0; j<param_list_size;j++)
        if(strcmp(param_list[j], fncts[i].param[j].type) != 0)
        {
            printf("Eroare: parametrii au tipuri diferite\n");
            exit(1);
        }
      param_list_size = 0;
}


struct Member{
    char* type, *name;
};

struct Structs{
    char* nume;
    int nr_members;
    struct Member members[SIZE];
}structs[SIZE];

int nr_structs;



void addStruct(char* nume){
    for(int i = 0; i < nr_structs; i++)
        if(strcmp(nume, structs[i].nume) == 0)
            yyerror("o structura cu acelasi nume exista deja");

    structs[nr_structs].nume = strdup(nume);
    structs[++nr_structs].nr_members = 0;
}

void adaugaVarStruct(char* tip, char* nume){
      
      int i ;
      for(i = 0; i < nr_structs; i++)
        if(strcmp(tip, structs[i].nume) == 0)
            break;
      
      if(i==nr_structs) yyerror("Tip necunoscut");

      adaugaVar(tip, nume); printf("adaugam var: %s %s\n", tip, nume);

      for(int j = 0; j < structs[i].nr_members; j++){
            char numeVar[50] = "";
            sprintf(numeVar, "%s@%s", nume, structs[i].members[j].name);
            adaugaVar(structs[i].members[j].type, numeVar); printf("adaugam var: %s %s\n", structs[i].members[j].type, numeVar);
      }
}

void addMember(char* memb_type, char* memb_name){
    for(int i = 0; i < structs[nr_structs].nr_members; i++)
        if(strcmp(memb_name, structs[nr_structs].members[i].name) == 0)
            yyerror("Exista un parametru cu acelasi nume");

    structs[nr_structs].members[structs[nr_structs].nr_members].name = strdup(memb_name);
    structs[nr_structs].members[structs[nr_structs].nr_members].type = strdup(memb_type);
    structs[nr_structs].nr_members++;
}

int isStruct(char* nume){
    for(int i = 0; i < nr_structs; i++)
        if(strcmp(nume, structs[i].nume) == 0)
            return 1;
      return 0;
}

void PrintSymbolTable(){
    FILE* file = fopen("symbol_table.txt", "w");
    for(int i = 0; i < nr_var; i++)
    {
        fprintf(file, "%s\t", vars[i].tip);
        if(strcmp(vars[i].tip, "string") != 0) 
            fprintf(file, "\t");
        fprintf(file, "%s\t", vars[i].nume);
        fprintf(file, "%s\n", vars[i].valoare);
    }
    fclose(file);

    file = fopen("symbol_table_functions.txt", "w");
    for(int i = 0; i < nr_fncts; i++)
    {
        fprintf(file, "%s\t%s\t", fncts[i].nume, fncts[i].ret_type);
        for(int j = 0; j<fncts[i].nr_params; j++)
            fprintf(file, "%s\t%s\t", fncts[i].param[j].type, fncts[i].param[j].name);
        fprintf(file, "\n");
    }
}
%{
#include <stdio.h>
%}
%%
##.*              ;
[ \t\n]          ;
[^\ \n\t]+       { fprintf(stdout, "%s\n", yytext); }
%%

int
main(int    argc,
     char **argv)
{
   FILE *fh;

   if (argc != 2)
     {
        fprintf(stderr, "*** FontMakerParser needs a file as argument!\n");
        return 127;
     }

   fh = fopen(argv[1], "r");
   if (!fh)
     {
        fprintf(stderr, "*** File [%s] is invalid\n", argv[1]);
        return 1;
     }

   yyin = fh;
   yylex();

   return 0;
}


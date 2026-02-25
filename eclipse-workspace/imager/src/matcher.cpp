/*
 * matcher.cpp
 *
 *  Created on: 19 avr. 2018
 *      Author: Jacques Girard (jb.girard@free.fr)
 */


#include <cstdlib>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "matcher.h"

static int ChercheMatch(  const char *tab_indices[], const char *format,char *gold,const char *ligne,int indice);
static char* TypeData(char *format,char *type_zone);

/*@----------------------------------------------------------------------
| TITLE:	FORMAT							|
| INPUT DATA:	Same parameters as printf				|
| RETURN:	The string that would have been printed by printf 	|
|		applied on the parameters.				|
| DESCRIPTION:  Same as format in common lisp				|
|		This function allows to format a string.		|
|		CAUTION: The returned string should be copied since it  |
|		will be altered after some subsequent 10 calls.		|
|.......................................................................|
| 		Cette fonction format une chaine et la renvoie.		|
|		La chaine renvoyee est static a la fonction et sera 	|
|		alteree apres les 10 prochains appels.			|
----------------------------------------------------------------------@*/

char *format (const char *fmt,...)
{
  va_list args;
  static int rang=0;
  static char chaine[10][LG_WORKING_STRING*10];

  rang = (rang + 1) % 10;
  va_start (args,fmt);
  vsnprintf(chaine[rang],LG_WORKING_STRING*10,fmt,args);
  va_end (args);

  return (chaine[rang]);
}

/*@----------------------------------------------------------------------
| TITLE:	NFORMAT							|
| INPUT DATA:	Indice of an internal buffer 0-4			|
|		+ Same parameters as printf				|
| RETURN:	The string that would have been printed by printf 	|
|		applied on the parameters.				|
| DESCRIPTION:  Same as format in common lisp				|
|		This function allows to format a string.		|
|		CAUTION: The returned string should be copied since it  |
|		will be altered by subsequent calls with the same indice|
|.......................................................................|
| 		Cette fonction format une chaine et la renvoie.		|
|		La chaine renvoyee est static a la fonction et sera 	|
|		alteree au prochain appel avec le meme indice.		|
|	Cette fonction est obsolete. Preferer la fonction format.	|
----------------------------------------------------------------------@*/

char *nformat (int indice,const char *fmt,...)
{
  va_list args;
  static char chaine[5][LG_WORKING_STRING];
  static char vide[1] = {'\0'};

  if (indice <0 || indice > 4) return (vide);

  va_start (args,fmt);
  vsprintf(chaine[indice],fmt,args);
  va_end (args);
  return (chaine[indice]);
}

/*@----------------------------------------------------------------------
| TITLE:	EQ_STRING						|
| INPUT DATA:	- two strings						|
| RETURN:	0 if strings are not identical				|
|		1 otherwise						|
| DESCRIPTION:	This function test the string equality			|
|.......................................................................|
| 		Cette fonction renvoie vrai si les deux chaines sont    |
|		egales.							|
----------------------------------------------------------------------@*/

int eq_string (const char *c1,const char *c2)
{
  return (!strcmp(c1,c2));
}

/*@----------------------------------------------------------------------
| TITLE:	TRIM_STRING						|
| INPUT DATA:	- one string						|
| RETURN:	the string without leading and trailing spaces					|
| DESCRIPTION:	This function supress leading and trailing spaces 			|
|.......................................................................|
| 		Cette fonction renvoie la chaine sans blancs au début ou à la fin.							|
----------------------------------------------------------------------@*/

int trim_string (char *s)
{
	char *pt = s;
	// leading spaces
	while (isspace (*pt)) {
		pt++;
	}
	if (pt != s) {
		memmove (s,pt,strlen(pt)+1);
	}

	if (*s) {
		// trailing spaces
		pt = s + strlen(s) - 1;
		while (isspace (*pt)) {
			*pt-- = 0;
		}
	}
	return (0);
}

/*@----------------------------------------------------------------------
| TITLE:	Sup_spaces					|
| INPUT DATA:	- one string						|
| RETURN:	the string without spaces and tabs					|
| DESCRIPTION:	This function supress  spaces 			|
|.......................................................................|
| 		Cette fonction renvoie la chaine sans blancs.					|
----------------------------------------------------------------------@*/

int sup_spaces (char *s)
{
	replace_all (s," ","");
	replace_all (s,"\t","");
	return (0);
}

/*@----------------------------------------------------------------------
| TITLE:	SUP_CRLF					|
| INPUT DATA:	- one string						|
| RETURN:	the string without trailing CR and or LF char					|
| DESCRIPTION:	This function supress trailing special characters 			|
|.......................................................................|
| 		Cette fonction renvoie la chaine sans caractère CR/LF en fin.	|
----------------------------------------------------------------------@*/

int sup_crlf (char *s)
{
	// Le dernier caractère
	if (s[strlen(s)-1] == '\r') {
		s[strlen(s)-1] = 0;
	}
	else if (s[strlen(s)-1] == '\n') {
		s[strlen(s)-1] = 0;
	}
	// Eventuellement celui d'avant
	if (s[strlen(s)-1] == '\r') {
		s[strlen(s)-1] = 0;
	}
	else if (s[strlen(s)-1] == '\n') {
		s[strlen(s)-1] = 0;
	}
	return (0);
}


/*@----------------------------------------------------------------------
| TITLE   :	REPLACE_FIRST					        |
| INPUT DATA :	- A string						|
|		- A sub string to be replaced (old)			|
|		- A sub string for replacement (news)			|
| RETURN     :	Nothing							|
| DESCRIPTION:	This function allows to replace the first occurence of  |
|		old string by news string  in the str string.		|
|		In case of replacement, intial string is modified	|
|		Sufficient memory must be provided in this string.	|
|.......................................................................|
|		Cette fonction permet de remplacer la premiere occurence|
|		de old par la chaine news dans la chaine str.		|
|		Si il y a remplacement alors chaine est modifiee.	|
|		La memoire pointee par str doit etre suffisante.	|
----------------------------------------------------------------------@*/

void replace_first (char *str,const char *old,const char *news)
{
  int new_size = strlen(news);
  int old_size = strlen(old);
  char *current = strstr(str,old);

  if (current) {
    memmove (current+new_size,current+old_size,strlen(current+old_size)+1);
    memmove (current,news,new_size);
  }
}

/*@----------------------------------------------------------------------
| TITLE	     :	REPLACE_ALL					        |
| INPUT DATA :	- A string						|
|		- A sub string to be replaced (old)			|
|		- A sub string for replacement (news)			|
| RETURN     :	Nothing							|
| DESCRIPTION:	This function allows to replace every occurences of old	|
|		string by news string  in the str string.		|
|		In case of replacement, intial string is modified	|
|		Sufficient memory must be provided in this string.	|
|.......................................................................|
|		Cette fonction permet de remplacer toutes les occurences|
|		de old par la chaine news dans la chaine str.		|
|		Si il y a des remplacements alors chaine est modifiee.	|
|		La memoire pointee par str doit etre suffisante.	|
----------------------------------------------------------------------@*/

void replace_all (char *str,const char *old,const char *news)
{
  int new_size = strlen(news);
  int old_size = strlen(old);
  char *current = strstr(str,old);

  while (current) {
    memmove (current+new_size,current+old_size,strlen(current+old_size)+1);
    memmove (current,news,new_size);
    current = strstr (current+new_size,old);
  }
}

/*@----------------------------------------------------------------------
| TITLE	     :	REPLACE_ALL_CAR 				        |
| INPUT DATA :	- A string						|
|		- A char to be replaced (old)		        	|
|		- A char for replacement (news)		        	|
| RETURN     :	Nothing							|
| DESCRIPTION:	This function allows to replace every occurences of old	|
|		char   by news char    in the str string.		|
|.......................................................................|
----------------------------------------------------------------------@*/

void replace_all_car (char *str,const char old,const char news)
{
  if (old == news) return;

  while (*str) {
    if (*str == old) *str=news;
    str++;
  }
}

/*@----------------------------------------------------------------------
| TITLE   :	IS_BLANK					        |
| INPUT DATA :	- A string						|
| RETURN     :	true if all characters of the string satisfy the 	|
|		standard isspace predicate.				|
| DESCRIPTION:	To check if a string is made only of spaces		|
|.......................................................................|
|		Cette fonction permet de verifier qu'une chaine ne 	|
|		contient que des caracteres d'espacement.		|
----------------------------------------------------------------------@*/

int is_blank (const char *str)
{
  while (isspace (*str)) str++;
  return (!*str);
}

/*@----------------------------------------------------------------------
| TITLE   :	IS_AN_INTEGER					        |
| INPUT DATA :	- A string 						|
| RETURN     :	- true if the begining of the sequence can be considered|
|		  as a numeric value				       	|
| DESCRIPTION:	This function indicates wether a string can be 		|
|		considered as an integer or not.			|
|               Sign is allowed						|
|.......................................................................|
|		Cette fonction permet de verifier d'indiquer si le debut|
|		de la chaine peut etre interprete comme un entier.	|
|		La valeur peut etre signee				|
----------------------------------------------------------------------@*/

int is_an_integer (const char *chaine)
{
  while (isspace (*chaine)) chaine++;
  if ((*chaine == '-') || (*chaine == '+')) chaine++;
  return (isdigit (*chaine));
}

/*@----------------------------------------------------------------------
| TITLE   :	IS_A_VALUE					        |
| INPUT DATA :	- A string 						|
| RETURN     :	- true if the begining of the sequence can be considered|
|		  as a numeric value				       	|
| DESCRIPTION:	This function indicates wether a string can be 		|
|		considered as a value or not.				|
|               Sign is allowed						|
|.......................................................................|
|		Cette fonction permet de verifier d'indiquer si le debut|
|		de la chaine peut etre interprete comme une valeur.	|
|		La valeur peut etre signee				|
----------------------------------------------------------------------@*/

int is_a_value (const char *chaine)
{
  while (isspace (*chaine)) chaine++;
  if ((*chaine == '-') || (*chaine == '+')) chaine++;
  if (*chaine == '.') chaine++;
  return (isdigit (*chaine));
}

/*@----------------------------------------------------------------------
| TITLE:	ISMATCHING                             			|
| INPUT DATA:  pattern (IN CHAR *) string with wildcards   		|
|              string: (IN CHAR *) string without wildcards  		|
| RETURN:                                         			|
|              true if string matches pattern     			|
| DESCRIPTION:                                    			|
|     This function checks wether a string matches			|
|     a given pattern. It uses recursion.         			|
|     Pattern can contain * and ? wildcards.      			|
|     They must be backslashed if they are to     			|
|     appear as normal characters in the pattern  			|
|.......................................................................|
----------------------------------------------------------------------@*/

int IsMatching (const char *pattern,const char *string)
{
if (!*pattern)
  return (!*string);

if (!*string) {
  while (*pattern) if (*pattern++ != '*') return (0);
  return (1);
  }

switch (pattern[0]) {
  case '\\': return ((strlen(pattern) > 1) &&
                    (pattern[1] == string[0]) &&
                    IsMatching (pattern+2,string+1));
  case '?': return (IsMatching(pattern+1,string+1));
  case '*': return (!*(pattern+1) ||  /* accelerator */
                    IsMatching(pattern+1,string) ||
                    IsMatching(pattern  ,string+1));
  default:  return ((pattern[0] == string[0]) &&
                    IsMatching (pattern+1,string+1));
  }
}

/*@----------------------------------------------------------------------
| TITLE:	MATCHSCAN                             			|
| INPUT DATA: Same parameters than sscanf:       			|
|               String representing the input     			|
|                 stream                          			|
|               String representing the pattern   			|
|                 to match the input with.        			|
|               As many locations as number of %  			|
|                 in the pattern to capture the   			|
|                 values.                         			|
|	        Variables whose adress are given in			|
|                 parameters are set with value from 			|
|                 the stream.                        			|
| RETURN:      true on successfull matching.      			|
| DESCRIPTION:                                    			|
|    This function implements a kind of sscanf    			|
|    with more features allowed in pattern        			|
|    description.                                 			|
|     % must be backslashed if they are to        			|
|     appear as normal characters in the pattern  			|
|.......................................................................|
|Attention: 1 blanc ou 1 tabulation dans le pattern match avec une suite|
|	    non vide de blancs et/ou de tabulations			|
|	    Une suite de plusieurs blancs et/ou tabulations dans le	|
|	    pattern ne match jamais.					|
----------------------------------------------------------------------@*/

int MatchScan (const char *ligne, const char *format,...)
{
  const char *tab_indices[100];

  va_list liste_params;
  char tab_params[100];
  char gold[100];
  char chaine[]  = "% "; /* __SOLARIS__ */
  char chainel[] ="%l "; /* __SOLARIS__ */
  int i_tab_p=0,nb_params=0,indice=0,laid;   /* laid comme i2 !! */
  void *tampon;
  char format1[LG_WORKING_STRING],*ptformat1=format1;
  strcpy (format1,format);
  va_start (liste_params,format);
  while(*ptformat1) {
    if(*ptformat1 == '%') {
      gold[indice] = 1;
      ptformat1 = TypeData (ptformat1,&tab_params[indice]);
      if (ptformat1 == (char*)0) return (0);
      nb_params++;
      indice++;
    }
    else {
      if (*ptformat1 == '\\' && *(ptformat1+1)=='%') {
	ptformat1++;
	if (!*ptformat1) return (0);
      }
      gold[indice] = 0;
      tab_params[indice] = *ptformat1;
      nb_params++;
      indice++;
    }
    ptformat1++;
  }
  tab_params[indice] = '\0';
  if(!ChercheMatch(tab_indices, tab_params,gold,ligne,1)) return(0);
  tab_indices[0] = ligne;
  tab_indices[nb_params] = ligne+strlen(ligne);

  // Affectation des variables
  for(laid = 0; laid < nb_params; laid++) {
    if (gold[i_tab_p])
      switch(tab_params[i_tab_p]) {
      case 's' :
      case 'p' :
        tampon = va_arg(liste_params,char *);
        strncpy((char*)tampon,tab_indices[laid],
                (tab_indices[laid+1]-tab_indices[laid]));
        ((char *)tampon)[tab_indices[laid+1]-tab_indices[laid]] = '\0';
        break;
      case 't' :
        tampon = va_arg(liste_params,char *);
        strncpy((char*)tampon,tab_indices[laid],
                (tab_indices[laid+1]-tab_indices[laid]));
        ((char *)tampon)[tab_indices[laid+1]-tab_indices[laid]] = '\0';
        trim_string ((char *)tampon);
        break;
      case 'c' :
      case 'd' :
      case 'e' :
      case 'f' :
      case 'g' :
        chaine[1] = tab_params[i_tab_p];
        tampon = va_arg(liste_params,void *);
        sscanf(tab_indices[laid],chaine,tampon);
        break;
      case 'D' :
      case 'G' :
      case 'F' :
      case 'E' :
        chainel[2] = tolower(tab_params[i_tab_p]);
        tampon = va_arg(liste_params,int *);
        sscanf(tab_indices[laid],chainel,tampon);
        break;
    }
    i_tab_p++;
  }
  return (1);
}

/*@----------------------------------------------------------------------
| TITLE:	CHERCHEMATCH                          			|
| INPUT DATA:   String of char for pattern (CHAR *)   			|
|                  this string can contain golden   			|
|                  characters which are:            			|
|                   s   : matches any string       			|
|					t	: matches a token (string not containing space nor ;) |
|					p	: matches a serie of characters after an opening parenthesis until the corresponding closing parenthesis |
|                   d,D : matches integers         			|
|                   e,E :                          			|
|                   f,F :                          			|
|                   g,G : matches floats           			|
|                   c   : matches one character    			|
|                Characters of this string may be 			|
|                simple characters matching 	  			|
|                themselves.			  			|
|               String to know the character status			|
|                in the previous string. (golden or 			|
|                not) (CHAR *)		          			|
|               String to compare with the pattern (CHAR*)		|
|               Current depth of RECURSION (INT)       			|
|    Output                                       			|
|              This function fills a global array 			|
|              with one way of matching. (if any) 			|
|     This is an array tab_indices of pointers on 			|
|     characters                                  			|
|     tab_indices[0] is left unchanged.           			|
|     tab_indices[i] is filled with the location  			|
|     of the first character of string that       			|
|     matches the ith character of pattern.       			|
| RETURN:                                         			|
|              true if string matches pattern     			|
|              false elsewhere                    			|
| DESCRIPTION:                                    			|
|    This function allows to count all leading    			|
|    characters that can be considered as part of 			|
|    an integer.                                  			|
|    Leading and trailing spaces are taken into   			|
|    account.                                     			|
|    This function is RECURSIVE.                  			|
|.......................................................................|
----------------------------------------------------------------------@*/

static int ChercheMatch(const char *tab_indices[], const char *format,char *gold,const char *ligne,int indice)
{
  int j;

  if(!*format) return(!*ligne);
  if(!*ligne)  return ((*format == 's') && *gold && !*(format+1));
  if(!*gold) {
    /* E021: 1 blanc ou tab match avec une suite non vide de blancs ou tab */
    if (*format == ' ' || *format == '\t') {
      if (*ligne != ' ' && *ligne != '\t') return (0);
      while (*ligne == ' ' || *ligne == '\t') ligne++;
      tab_indices[indice] = ligne;
      return(ChercheMatch(tab_indices, format+1,gold+1,ligne,indice+1));
    }
    else {
    if(*ligne != *format) return(0);
    ligne++;
    tab_indices[indice] = ligne;
    return(ChercheMatch(tab_indices, format+1,gold+1,ligne,indice+1));
    }
  }
  else {
    switch(*format) {
    case 's' :
      return(!*(format+1) ||	/* acelerator */
             (tab_indices[indice] = ligne,
              ChercheMatch(tab_indices, format+1,gold+1,ligne,indice+1)) ||
	     ChercheMatch(tab_indices, format,gold,ligne+1,indice)
	     );
      break;
    case 'd' :
    case 'D' :
      j = taille_entier (ligne);
      if (!j) return(0); else ligne+=j;
      tab_indices[indice] = ligne;
      return(ChercheMatch(tab_indices, format+1,gold+1,ligne,indice+1));
      break;
    case 'e' :
    case 'E' :
    case 'f' :
    case 'F' :
    case 'g' :
    case 'G' :
      j = taille_flottant (ligne);
      if (!j) return(0); else ligne+=j;
      tab_indices[indice] = ligne;
      return(ChercheMatch(tab_indices, format+1,gold+1,ligne,indice+1));
      break;
    case 't' :
      j = taille_token (ligne);
      if (!j) return(0); else ligne+=j;
      tab_indices[indice] = ligne;
      return(ChercheMatch(tab_indices, format+1,gold+1,ligne,indice+1));
      break;
    case 'p' :
      j = taille_expEntreParentheses (ligne);
      if (!j) return(0); else ligne+=j;
      tab_indices[indice] = ligne;
      return(ChercheMatch(tab_indices, format+1,gold+1,ligne,indice+1));
      break;
    case 'c' :
      ligne++;
      tab_indices[indice] = ligne;
      return(ChercheMatch(tab_indices, format+1,gold+1,ligne,indice+1));
      break;
      default :
	/*      stou_errsys(NOK, "Bad format specification", format, NONFATAL);*/
      return (0);
    }
  }
}

/*@----------------------------------------------------------------------
| TITLE:	TYPEDATA                               			|
| INPUT DATA:   Format contained within a char stream (IN CHAR *)	|
|	        One character (OUT CHAR*)              			|
| RETURN:                                         			|
|    Either the news pointer of stream after format     			|
|    or NULL if invalid format.			   			|
| DESCRIPTION:                                    			|
|    This function allows to translate some       			|
|    format specification sequences into single   			|
|    character.                                   			|
|    %d --> d     %D --> D     %ld --> D          			|
|    %f --> f     %F --> F     %lf --> F          			|
|    %e --> e     %E --> E     %le --> E          			|
|    %g --> g     %G --> G     %lg --> G          			|
|    %c --> c     %t --> t                        			|
|    %s --> s                                     			|
|.......................................................................|
----------------------------------------------------------------------@*/

static char* TypeData(char *format,char *type_zone)
{
  format++;
  if(*format == 'l') {
    format++;
    if (!*format) return ((char*)0);
    *format = toupper (*format);
    }

  switch(*format) {
      case 'c' :
      case 'd' :
      case 'D' :
      case 'e' :
      case 'E' :
      case 'f' :
      case 'F' :
      case 'g' :
      case 'G' :
      case 's' :
      case 't' :
      case 'p' :
        *type_zone = *format;
        return(format);
/*      break; */
      default  :
        return ((char*)0);
    }
}

/*@----------------------------------------------------------------------
| TITLE:       TAILLE_ENTIER                           			|
| INPUT DATA:  stream of characters (CHAR *)           			|
| RETURN:      Number of leading characters that  			|
|              can be considered as an integer    			|
|              Zero if leading characters can't be			|
|              interpreted as an integer.         			|
| DESCRIPTION:                                    			|
|    This function allows to count all leading    			|
|    characters that can be considered as part of 			|
|    an integer.                                  			|
|    Leading and trailing spaces are taken into   			|
|    account.                                     			|
|.......................................................................|
----------------------------------------------------------------------@*/

int taille_entier (const char *ligne)
{
  long n1;
  int n,r;
  r = sscanf (ligne,"%ld%n",&n1,&n);
  if (r>0)
    return (n);
  else
    return (0);
}

/*@----------------------------------------------------------------------
| TITLE:       TAILLE_FLOTTANT                         			|
| INPUT DATA:  stream of characters (CHAR *)           			|
| RETURN:      Number of leading characters that  			|
|              can be considered as a float	    			|
|              Zero if leading characters can't be			|
|              interpreted as an integer.         			|
| DESCRIPTION:                                    			|
|    This function allows to count all leading    			|
|    characters that can be considered as part of 			|
|    a float.	                                 			|
|    Leading and trailing spaces are taken into   			|
|    account.                                     			|
|.......................................................................|
----------------------------------------------------------------------@*/

int taille_flottant (const char *ligne)
{
  double f;
  int n,r;
  r = sscanf (ligne,"%lf%n",&f,&n);
  if (r>0) return (n);
  else return (0);
}

/*@----------------------------------------------------------------------
| TITLE:       TAILLE_TOKEN                         			|
| INPUT DATA:  stream of characters (CHAR *)           			|
| RETURN:      Number of leading characters that  			|
|              can be considered as a token	    			|
|              Zero if leading characters can't be			|
|              interpreted as a token.         			|
| DESCRIPTION:                                    			|
|    This function allows to count all leading    			|
|    characters that can be considered as part of 			|
|    a token.	                                 			|
|.......................................................................|
----------------------------------------------------------------------@*/

int taille_token (const char *ligne)
{
  int n=0;
  while (isspace (ligne[n]))  {   // On accepte les blancs en début. Ils seront supprimés lors de l'affectation des variables dans MatchScan
	  n++;
  }
  int nSpace = n;
  while (ligne[n] != ' ' && ligne[n] != ';' && ligne[n] != '\0' && ligne[n] != '\n' && ligne[n] != '\r' && ligne[n] != '\t') {
	  n++;
  }
  if (n == nSpace) {	// Cas ou il n'y avait que des space suivis de la fin de ligne
	  return 0;
  }
  else {
	  return (n);
  }
}


/*@----------------------------------------------------------------------
| TITLE:       TAILLE_EXPENTREPARENTHESES                 	|
| INPUT DATA:  stream of characters (CHAR *)           		|
| RETURN:      Number of leading characters that  			|
|              can be considered as embedded within parenthesis.       |
|			   Considering an open parenthesis just before  |
| DESCRIPTION:                                    			|
|    This function allows to find a string (following an  	|
|	 opening parenthesis until the next closing parenthesis |
|	 AT THE SAME LEVEL										|
|.......................................................................|
----------------------------------------------------------------------@*/

int taille_expEntreParentheses (const char *ligne)
{
  int n=0;
  int niveau = 0;
  while (ligne[n]) {
	  if (ligne[n] == ')') {
		  if (niveau == 0) {
			  return n;
		  }
		  else {
			  niveau--;
		  }
	  }
	  else if (ligne[n]=='(') {
		  niveau++;
	  }
	  n++;
  }
  return 0;
}


/*@----------------------------------------------------------------------
| TITLE:	STOU_UPPERCASE                         			|
| INPUT DATA:   String to uppercase (IN CHAR*)         			|
|		Uppercased string (OUT CHAR*)          			|
| RETURN:                                         			|
| DESCRIPTION:                                   			|
|    This function change a null terminated string into uppercased      |
|    string                                                             |
|    The two strings can be at the same location			|
|.......................................................................|
----------------------------------------------------------------------@*/

int stou_uppercase (char *pc_string, char *pc_uppercase)
{
  while ((*pc_uppercase++ = (char)toupper(*pc_string++)));
  return (0);
}

/*@----------------------------------------------------------------------
| TITLE:	STOU_LOWERCASE                         			|
| INPUT DATA:   String to lowercase (IN CHAR*)         			|
|		Lowercased string (OUT CHAR*)          			|
| RETURN:                                         			|
| DESCRIPTION:                                   			|
|    This function change a null terminated string into lowercased      |
|    string                                                             |
|    The two strings can be at the same location			|
|.......................................................................|
----------------------------------------------------------------------@*/

int stou_lowercase (char *pc_string, char *pc_lowercase)
{
  while ((*pc_lowercase++ = (char)tolower(*pc_string++)));
  return (0);
}



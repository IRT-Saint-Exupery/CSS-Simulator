/*
 * matcher.h
 *
 *  Created on: 19 avr. 2018
 *      Author: Jacques Girard (jb.girard@free.fr)
 */

#ifndef MATCHER_H_
#define MATCHER_H_

#include <stdarg.h>

#define LG_WORKING_STRING 1024

char *format (const char *fmt,...);
char *nformat (int indice,const char *fmt,...);
int eq_string (const char *c1,const char *c2);
int trim_string (char *s1);
int sup_spaces (char *s1);
int sup_crlf (char *s1);
void replace_first (char *str,const char *old,const char *news);
void replace_all (char *str,const char *old,const char *news);
void replace_all_car (char *str,const char old,const char news);
int is_blank (const char *str);
int is_an_integer (const char *chaine);
int is_a_value (const char *chaine);
int taille_entier (const char *ligne);
int taille_flottant (const char *ligne);
int taille_token (const char *ligne);
int taille_expEntreParentheses (const char *ligne);			// Appel: (%p)

int IsMatching (const char *pattern,const char *string);
int MatchScan  (const char *ligne, const char *format,...); // Attention: 1 blanc ou 1 tabulation dans le format match avec une suite
															// non vide de blancs et/ou de tabulations
															// Une suite de plusieurs blancs et/ou tabulations dans le format ne match JAMAIS.

															// Bonne pratique: terminer le format par %s pour matcher les éventuels trailing space


#endif /* MATCHER_H_ */

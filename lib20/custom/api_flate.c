/**************************************************************************
*                                                                        *
* Flate library 1.5 (Fast Template)                                      *
* Copyright (C) 2001 Fabien Menemenlis (nihilist@dead-inside.org)        *
*                                                                        *
* This library is free software; you can redistribute it and/or          *
* modify it under the terms of the GNU Lesser General Public             *
* License as published by the Free Software Foundation; either           *
* version 2.1 of the License, or (at your option) any later version.     *
*                                                                        *
* This library is distributed in the hope that it will be useful,        *
* but WITHOUT ANY WARRANTY; without even the implied warranty of         *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
* Lesser General Public License for more details.                        *
*                                                                        *
* You should have received a copy of the GNU Lesser General Public       *
* License along with this library; if not, write to the Free Software    *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston,                  *
* MA  02111-1307  USA                                                    *
*                                                                        *
**************************************************************************/

/*
* new version breaks compatibility with older ones as an argument is now
* given to each function: this allows you to have multiple templates
* simultanuously
* possible memory leak, some free(curtable) could be missing but I couldn't
* prove this again
* Perl no longer supported
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "api_flate.h"


#define TNAME 100 /* max size of a variable name */


enum {
		TEMP_HTML,
		TEMP_VAR,
		TEMP_BEGINTABLE,
		TEMP_ENDTABLE,
		TEMP_BEGINZONE,
		TEMP_ENDZONE,
		TEMP_BEGINGENTABLE,
		TEMP_ENDGENTABLE
};


int tempcompare(const void *a, const void *b)
{
	return(strcmp(((st_ptr *)a)->nomchamp, ((st_ptr *)b)->nomchamp));
}


int tempcompnom(const void *a, const void *b)
{
	return(strcmp((char *)a, ((st_ptr *)b)->nomchamp));
}


/*
* seeking function: looks for a value in a shortcut table
* (tempVar, tempZone, tempTable)
*/
void *tempsearch(const void *elem, const void *base, size_t nbelem,
				 size_t size, int (*comparefunc)(const void *, const void *),
				 size_t *nb)
{
	void *ptr;
	int pos;
	
	
	*nb = 0;
	ptr = bsearch(elem, base, nbelem, size, comparefunc);
	if (!ptr)
		return(NULL);
	
	pos = ((char *)ptr - (char *)base) / size;
	/* go back to first entry of the sorted table */
	while (pos > 0
		&& comparefunc(elem, (void *)((char *)base + (pos - 1 ) *size)) == 0)
		pos--;
	/* computes number of entries in the table */
	while (pos + *nb < nbelem
		&& comparefunc(elem, (void *)((char *)base + (pos + *nb) * size)) == 0)
		(*nb)++;
	
	return((void *)(((char *)base) + pos * size));
}


Template *tplSetFile(request_rec *r, Template **tmplte, const char *filename)
{
	FILE *fichier;
	int pos, taille, debut;
	char *tempBuf;
	int tempdebut;
	char tmp[TNAME + 1], nom[TNAME + 1];
	int i;
	int nbvar;
	int type;
	tempBloc *Bloc;
	
	//使用apache api 操作文件
	r->filename = ap_psprintf(r->pool, filename);
	
	fichier = ap_pfopen(r->pool, r->filename, "r");
	
	if(!fichier)
	{
		return NULL;
	}
	
	fseek(fichier, 0, SEEK_END);
	taille = ftell(fichier);
	rewind(fichier);
	
	tempBuf = (char *)ap_palloc(r->pool, sizeof(char) * taille);
	tempBuf = (char *)ap_pcalloc(r->pool, sizeof(char) * taille);
	
	fread(tempBuf, sizeof(char), taille, fichier);
	fclose(fichier);
	
	(*tmplte)->tempnbVar = (*tmplte)->tempnbZone
		= (*tmplte)->tempnbTable = 0;
	(*tmplte)->tempVar = (*tmplte)->tempZone = (*tmplte)->tempTable = NULL;
	
	//(*tmplte)->BlocInit = (tempBloc *)malloc(sizeof(tempBloc));
	//memset((*tmplte)->BlocInit, 0, sizeof(tempBloc));
	(*tmplte)->BlocInit = (tempBloc *)ap_palloc(r->pool, sizeof(tempBloc));
	(*tmplte)->BlocInit = (tempBloc *)ap_pcalloc(r->pool, sizeof(tempBloc));
	
	Bloc = (*tmplte)->BlocInit;
	nbvar = 0;
	tempdebut = 0;
	debut = pos = 0;
	
	while (pos < taille) {
		if (tempBuf[pos] == '#') {
			if (tempBuf[pos + 1] == '#') {
				// the encountered code till there is pure html //
				memcpy(tempBuf + tempdebut, tempBuf + debut,
					pos - debut);
				i = 0;
				
				//Bloc->suiv = (tempBloc *)malloc(sizeof(tempBloc));
				//memset(Bloc->suiv, 0, sizeof(tempBloc));
				Bloc->suiv = (tempBloc *)ap_palloc(r->pool, sizeof(tempBloc));
				Bloc->suiv = (tempBloc *)ap_pcalloc(r->pool, sizeof(tempBloc));
				
				Bloc->suiv->prec = Bloc;
				Bloc->data = tempBuf + tempdebut;
				Bloc->tdata = pos - debut;
				Bloc->type = TEMP_HTML;
				Bloc = Bloc->suiv;
				tempdebut += pos - debut;
				
				// from now on we're in the variable name //
				pos += 2;
				while (pos + i < taille
					&& tempBuf[pos + i] != '#'
					&& i < TNAME)
					i++;
				memcpy(tmp, tempBuf + pos, i);
				tmp[i] = '\0';
				
				// saves the bloc containing the variable //
				//Bloc->suiv = (tempBloc *)malloc(sizeof(tempBloc));
				//memset(Bloc->suiv, 0, sizeof(tempBloc));
				Bloc->suiv = (tempBloc *)ap_palloc(r->pool, sizeof(tempBloc));
				Bloc->suiv = (tempBloc *)ap_pcalloc(r->pool, sizeof(tempBloc));
				
				Bloc->suiv->prec = Bloc;
				Bloc->type = TEMP_VAR;
				Bloc->nom = strdup(tmp);
				// adds the variable in the shortcut table //
				(*tmplte)->tempVar =
					(st_ptr *)realloc((*tmplte)->tempVar,
					sizeof(st_ptr) * ((*tmplte)->tempnbVar + 1));
				(*tmplte)->tempVar[(*tmplte)->tempnbVar].nomchamp = strdup(tmp);
				(*tmplte)->tempVar[(*tmplte)->tempnbVar].bloc = Bloc;
				(*tmplte)->tempnbVar++;
				
				Bloc = Bloc->suiv;
				nbvar++;
				pos += i + 1;
				debut = pos + 1;
			}	
		}
		// comments //
		else if (tempBuf[pos] == '<'
			&& tempBuf[pos + 1] == '!'
			&& tempBuf[pos + 2] == '-'
			&& tempBuf[pos + 3] == '-') {
			// the encountered code till there is pure html //
			memcpy(tempBuf + tempdebut, tempBuf + debut, pos - debut);
			i = 0;
			
			//Bloc->suiv = (tempBloc *)malloc(sizeof(tempBloc));
			//memset(Bloc->suiv, 0, sizeof(tempBloc));
			Bloc->suiv = (tempBloc *)ap_palloc(r->pool, sizeof(tempBloc));
			Bloc->suiv = (tempBloc *)ap_pcalloc(r->pool, sizeof(tempBloc));
			
			Bloc->suiv->prec = Bloc;
			Bloc->data = tempBuf + tempdebut;
			Bloc->tdata = pos - debut;
			Bloc->type = TEMP_HTML;
			Bloc = Bloc->suiv;
			tempdebut += pos - debut;
			debut = pos;
			
			pos += 4;
			i = 0;
			while (isspace(tempBuf[pos]))
				pos ++;
			while (pos < taille
				&& tempBuf[pos] != '-'
				&& tempBuf[pos] != '>'
				&& !isspace(tempBuf[pos])
				&& i < TNAME) {
				tmp[i++] = tempBuf[pos];
				pos++;
			}
			tmp[i] = '\0';
			while (isspace(tempBuf[pos]))
				pos++;
			i = 0;
			while (pos < taille
				&& tempBuf[pos] != '-'
				&& tempBuf[pos] != '>'
				&& !isspace(tempBuf[pos])
				&& i < TNAME) {
				nom[i++] = tempBuf[pos];
				pos++;
			}
			nom[i] = '\0';
			while (pos < taille && tempBuf[pos] != '>')
				pos++;
			
			type = -1;
			if (strcmp(tmp, "#BEGINTABLE") == 0)
				type = TEMP_BEGINTABLE;
			else if (strcmp(tmp, "#ENDTABLE") == 0)
				type = TEMP_ENDTABLE;
			else if (strcmp(tmp, "#BEGINZONE") == 0)
				type = TEMP_BEGINZONE;
			else if (strcmp(tmp, "#ENDZONE") == 0)
				type = TEMP_ENDZONE;
			
			if (type != -1) {
				// saves the bloc containing the variable //
				//Bloc->suiv = (tempBloc *)malloc(sizeof(tempBloc));
				//memset(Bloc->suiv, 0, sizeof(tempBloc));
				Bloc->suiv = (tempBloc *)ap_palloc(r->pool, sizeof(tempBloc));
				Bloc->suiv = (tempBloc *)ap_pcalloc(r->pool, sizeof(tempBloc));

				Bloc->type = type;
				Bloc->suiv->prec = Bloc;
				if (Bloc->type != TEMP_HTML)
					Bloc->nom = strdup(nom);
				switch (type) {

				case TEMP_BEGINTABLE:
					(*tmplte)->tempTable
						= (st_ptr *)realloc((*tmplte)->tempTable,
						sizeof(st_ptr)
						* ((*tmplte)->tempnbTable + 1));

					(*tmplte)->tempTable[(*tmplte)->tempnbTable].nomchamp
						= strdup(nom);
					(*tmplte)->tempTable[(*tmplte)->tempnbTable].bloc = Bloc;
					(*tmplte)->tempnbTable++;
					break;
				case TEMP_BEGINZONE:
					(*tmplte)->tempZone
						= (st_ptr *)realloc((*tmplte)->tempZone,
						sizeof(st_ptr)
						* ((*tmplte)->tempnbZone + 1));
					(*tmplte)->tempZone[(*tmplte)->tempnbZone].nomchamp
						= strdup(nom);
					(*tmplte)->tempZone[(*tmplte)->tempnbZone].bloc = Bloc;
					(*tmplte)->tempnbZone++;
					break;
				}
				Bloc = Bloc->suiv;
			}
			else {
				// the whole block is taken for html //
				memcpy(tempBuf + tempdebut, tempBuf + debut, pos + 1 - debut);

				//Bloc->suiv = (tempBloc *)malloc(sizeof(tempBloc));
				//memset(Bloc->suiv, 0, sizeof(tempBloc));
				Bloc->suiv = (tempBloc *)ap_palloc(r->pool, sizeof(tempBloc));
				Bloc->suiv = (tempBloc *)ap_pcalloc(r->pool, sizeof(tempBloc));

				Bloc->suiv->prec = Bloc;
				Bloc->data = tempBuf + tempdebut;
				Bloc->tdata = pos + 1 - debut;
				Bloc->type = TEMP_HTML;
				Bloc = Bloc->suiv;
				tempdebut += pos + 1 - debut;
			}
			debut = pos + 1;
    }
    pos++;
  }
  if (tempdebut < taille) {
	  memcpy(tempBuf + tempdebut, tempBuf + debut, pos - debut);
	  Bloc->suiv = NULL;
	  Bloc->data = tempBuf + tempdebut;
	  Bloc->tdata = pos - debut;
	  Bloc->type = TEMP_HTML;
	  tempdebut += pos - debut;
  }
  
  // free(tempBuf);
  qsort((*tmplte)->tempVar, (*tmplte)->tempnbVar, sizeof(st_ptr),
	  tempcompare);
  qsort((*tmplte)->tempZone, (*tmplte)->tempnbZone, sizeof(st_ptr),
	  tempcompare);
  qsort((*tmplte)->tempTable, (*tmplte)->tempnbTable, sizeof(st_ptr),
	  tempcompare);
  
  return(*tmplte);
}


void tplSetVar(Template *tmplte, const char *champ, const char *val)
{
	tempBloc *Bloc;
	st_ptr *rec;
	size_t i, nb;
	
	if (val == NULL)
		return;
	rec = (st_ptr *)tempsearch(champ, tmplte->tempVar, tmplte->tempnbVar,
			     sizeof(st_ptr), tempcompnom, &nb);
	
	if (rec) {
		
		for (i = 0; i < nb; i++) {
			Bloc = rec[i].bloc;
			if (Bloc->data)
				free(Bloc->data);
			Bloc->data = strdup(val);
			Bloc->tdata = strlen(val);
			Bloc->alloc = 1;
		}
	}
	
	rec = (st_ptr *)tempsearch(champ, tmplte->tempZone, tmplte->tempnbZone,
			     sizeof(st_ptr), tempcompnom, &nb);
	
	if (rec) {
		for (i = 0; i < nb; i++) {
			Bloc = rec[i].bloc;
			Bloc->tdata = 1;
		}
	}
	
}


void templateDumpBloc(request_rec *r, tempBloc *BlocP, tempBloc *Bloc)
{
	tempBloc *BlocN;
	pool *sub_pool;
		
	if (!(Bloc->type == TEMP_HTML || Bloc->type == TEMP_VAR)
		|| Bloc->type == TEMP_VAR && Bloc->data == NULL)
		return;
	
	//BlocN = (tempBloc *)malloc(sizeof(tempBloc));
	//memset(BlocN, 0, sizeof(tempBloc));
	BlocN = (tempBloc *)ap_palloc(r->pool, sizeof(tempBloc)+2);
	BlocN = (tempBloc *)ap_pcalloc(r->pool, sizeof(tempBloc)+2);

	//BlocN->data = (char *)malloc(Bloc->tdata);
	BlocN->data = (char *)ap_palloc(r->pool, Bloc->tdata+2);
	BlocN->data = (char *)ap_pcalloc(r->pool, Bloc->tdata+2);

	memcpy(BlocN->data, Bloc->data, Bloc->tdata);

	BlocN->tdata = Bloc->tdata;
	BlocN->alloc = 1;
	BlocN->type = TEMP_HTML;
	BlocN->prec = BlocP->prec;
	if (BlocP->prec)
	{
		BlocP->prec->suiv = BlocN;
	}
	BlocN->suiv = BlocP;
	BlocP->prec = BlocN;
}


void tplParse(request_rec *r, Template *tmplte, const char *ligne)
{
	tempBloc *Bloc, *BlocS;
	tempBloc *BlocTableS = NULL, *BlocN, *BlocO, *BlocD;
	int pr;
	size_t i, nb;
	st_ptr *rec;
	
	
	rec = (st_ptr *)tempsearch(ligne, tmplte->tempTable, tmplte->tempnbTable,
			     sizeof(st_ptr), tempcompnom, &nb);
	if (rec) {
		pr = 1;
		
		for (i = 0; i < nb; i++) {
			Bloc = rec[i].bloc;
			
			/*
			* insert a mark "beginning of generated code"
			*/
			//BlocN = (tempBloc *)malloc(sizeof(tempBloc));
			//memset(BlocN, 0, sizeof(tempBloc));
			BlocN = (tempBloc *)ap_palloc(r->pool, sizeof(tempBloc));
			BlocN = (tempBloc *)ap_pcalloc(r->pool, sizeof(tempBloc));
			
			BlocN->suiv = Bloc;
			BlocN->prec = Bloc->prec;
			Bloc->prec->suiv = BlocN;
			Bloc->prec = BlocN;
			BlocN->type = TEMP_BEGINGENTABLE;
			
			BlocS = Bloc;
			Bloc = Bloc->suiv;
			
			/*
			* we go through all the blocks till the corresponding ENDTABLE
			*/
			while (Bloc
				&& !(Bloc->type == TEMP_ENDTABLE
				&& strcmp(Bloc->nom, ligne) == 0)) {
				if (Bloc->type == TEMP_BEGINGENTABLE) {
					BlocTableS = Bloc;
				}
				else if (Bloc->type == TEMP_ENDGENTABLE) {
				/*
				* frees the encountered generated blocks
					*/
					BlocD = BlocTableS->prec;
					BlocD->suiv = Bloc->suiv;
					Bloc->suiv->prec = BlocD;
					BlocN = BlocTableS;
					while (BlocN != Bloc) {
						/* freeing the blocks */
						if (BlocN->nom)
							free(BlocN->nom);
						if (BlocN->alloc && BlocN->data)
							free(BlocN->data);
						BlocO = BlocN;
						BlocN = BlocN->suiv;
						free(BlocO);
					}
					/* and we also have to free the ENDGENTABLE block */
					free(BlocN);
					Bloc = BlocD;
				}
				else if (Bloc->type == TEMP_BEGINZONE
					&& !tmplte->curzone
					&& Bloc->tdata != 1) {
					free(tmplte->curzone);
					tmplte->curzone = strdup(Bloc->nom);
					pr = 0;
				}
				else if (Bloc->type == TEMP_ENDZONE
					&& tmplte->curzone
					&& strcmp(Bloc->nom, tmplte->curzone) == 0) {
					pr = 1;
					free(tmplte->curzone);
					tmplte->curzone = NULL;
				}
				else if (Bloc->type == TEMP_BEGINTABLE) {
					tmplte->curtable = strdup(Bloc->nom);
					Bloc = Bloc->suiv;
					while (Bloc
						&& !(Bloc->type == TEMP_ENDTABLE
						&& strcmp(Bloc->nom, tmplte->curtable) == 0)) {
						Bloc = Bloc->suiv;
					}
					free(tmplte->curtable);
					tmplte->curtable = NULL;
				}
				else {
					if (pr) {
						templateDumpBloc(r, BlocS, Bloc);
						/*
						* if the block is a variable, reinitialize it once copied
						*/
						if (Bloc->type == TEMP_VAR) {
							free(Bloc->data);
							Bloc->data = NULL;
							Bloc->tdata = 0;
							Bloc->alloc = 0;
						}
						else if (Bloc->type == TEMP_BEGINZONE) {
							Bloc->tdata = 0;
						}
					}
				}
				if (Bloc)
					Bloc = Bloc->suiv;
			}
			/*
			* insert a mark "end of generated code"
			*/
			BlocO = rec[i].bloc;

			//BlocN = (tempBloc *)malloc(sizeof(tempBloc));
			//memset(BlocN, 0, sizeof(tempBloc));

			BlocN = (tempBloc *)ap_palloc(r->pool, sizeof(tempBloc));
			BlocN = (tempBloc *)ap_pcalloc(r->pool, sizeof(tempBloc));

			BlocN->suiv = BlocO;
			BlocN->prec = BlocO->prec;
			BlocO->prec->suiv = BlocN;
			BlocO->prec = BlocN;
			BlocN->type = TEMP_ENDGENTABLE;
    }
  }
}


void tplPrint(request_rec *r, Template *tmplte)
{
	tempBloc *Bloc;
	char *ptr;
	int pr;	
	
	Bloc = tmplte->BlocInit;
	pr = 1;
	while (Bloc) {
		if (Bloc->type == TEMP_BEGINTABLE) {
			tmplte->curtable = strdup(Bloc->nom);
			Bloc = Bloc->suiv;
			while (Bloc
				&& !(Bloc->type == TEMP_ENDTABLE
				&& strcmp(Bloc->nom, tmplte->curtable) == 0)) {
				Bloc = Bloc->suiv;
			}
			free(tmplte->curtable);
			tmplte->curtable = NULL;
		}
		else if (Bloc->type == TEMP_BEGINZONE && !tmplte->curzone
			&& Bloc->tdata != 1) {
			pr = 0;
			free(tmplte->curzone);
			tmplte->curzone = strdup(Bloc->nom);
		}
		else if (Bloc->type == TEMP_ENDZONE
			&& tmplte->curzone
			&& strcmp(Bloc->nom, tmplte->curzone) == 0) {
			pr = 1;
			free(tmplte->curzone);
			tmplte->curzone = NULL;
		}
		else if (pr && (Bloc->type == TEMP_HTML || Bloc->type == TEMP_VAR)
			&& Bloc->data) {
			//ptr = (char *)malloc(Bloc->tdata + 1);
			ptr = (char *)ap_palloc(r->pool, Bloc->tdata+2);
			ptr = (char *)ap_pcalloc(r->pool, Bloc->tdata+2);

			memcpy(ptr, Bloc->data, Bloc->tdata);
	
			ptr[Bloc->tdata] = '\0';
			ap_rputs(ptr, r);
		}
		if (Bloc)
			Bloc = Bloc->suiv;
	}
}


char *tplPage(Template *tmplte)
{
	char *tBufp;
	int taillep = 0;
	
	tempBloc *Bloc;
	int pr;
	
	
	tBufp = NULL;
	Bloc = tmplte->BlocInit;
	pr = 1;
	while (Bloc) {
		if (Bloc->type == TEMP_BEGINTABLE) {
			tmplte->curtable = strdup(Bloc->nom);
			Bloc = Bloc->suiv;
			while (Bloc
				&& !(Bloc->type == TEMP_ENDTABLE
				&& strcmp(Bloc->nom, tmplte->curtable) == 0)) {
				Bloc = Bloc->suiv;
			}
			free(tmplte->curtable);
			tmplte->curtable = NULL;
		}
		else if (Bloc->type == TEMP_BEGINZONE && !tmplte->curzone
			&& Bloc->tdata != 1) {
			pr = 0;
			free(tmplte->curzone);
			tmplte->curzone = strdup(Bloc->nom);
		}
		else if (Bloc->type == TEMP_ENDZONE
			&& tmplte->curzone
			&& strcmp(Bloc->nom, tmplte->curzone) == 0) {
			pr = 1;
			free(tmplte->curzone);
			tmplte->curzone = NULL;
		}
		else if (pr && (Bloc->type == TEMP_HTML || Bloc->type == TEMP_VAR)
			&& Bloc->data) {
			tBufp = (char *)realloc(tBufp, taillep + Bloc->tdata + 1);
			memcpy(tBufp + taillep, Bloc->data, Bloc->tdata);
			tBufp[taillep + Bloc->tdata] = '\0';
			taillep += Bloc->tdata;
		}
		if (Bloc)
			Bloc = Bloc->suiv;
	}
	return(tBufp);
}


void tplFreeMem(Template *tmplte)
{
	tempBloc *Bloc, *BlocO;
	int i;
	
	
	if (!tmplte)
		return;
	Bloc = tmplte->BlocInit;
	while (Bloc) {
		if (Bloc->nom)
			free(Bloc->nom);
		if (Bloc->alloc && Bloc->data)
			free(Bloc->data);
		BlocO = Bloc;
		Bloc = Bloc->suiv;
		free(BlocO);
	}
	tmplte->BlocInit = NULL;
	for (i = 0; i < tmplte->tempnbVar; i++)
		free(tmplte->tempVar[i].nomchamp);
	for (i = 0; i < tmplte->tempnbZone; i++)
		free(tmplte->tempZone[i].nomchamp);
	for (i = 0; i < tmplte->tempnbTable; i++)
		free(tmplte->tempTable[i].nomchamp);
	if (tmplte->tempVar) {
		free(tmplte->tempVar);
		tmplte->tempVar = NULL;
	}
	if (tmplte->tempZone) {
		free(tmplte->tempZone);
		tmplte->tempZone = NULL;
	}
	if (tmplte->tempTable) {
		free(tmplte->tempTable);
		tmplte->tempTable = NULL;
	}
	tmplte->tempnbVar = tmplte->tempnbZone = tmplte->tempnbTable = 0;
	
	if (tmplte->curzone) {
		free(tmplte->curzone);
		tmplte->curzone = NULL;
	}
	if (tmplte->curtable) {
		free(tmplte->curtable);
		tmplte->curtable = NULL;
	}
	if (tmplte->tempBuf) {
		free(tmplte->tempBuf);
		tmplte->tempBuf = NULL;
	}
	free(tmplte);
}

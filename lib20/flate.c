/**************************************************************************
 *                                                                        *
 * Flate library 1.4.2 (Fast Template)                                    *
 * Copyright (C) 2003 Fabien Menemenlis (nihilist@dead-inside.org)        *
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _USE_FETCH_
#include <sys/param.h>
#include <fetch.h>
#endif

#include "flate.h"


#define TNOM 100


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

typedef struct tempBloc {
  struct tempBloc *suiv;
  struct tempBloc *prec;
  char *data;
  char *nom;
  int tdata;
  int type;
  int alloc;
}
tempBloc;

typedef struct
{
  char *nomchamp;
  tempBloc *bloc;
}
st_ptr;


static tempBloc *BlocInit;
static st_ptr *tempVar, *tempZone, *tempTable;
static int tempnbVar, tempnbZone, tempnbTable;

static char *tempBuf;
static char *curzone;
static char *curtable;


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
		 int *nb)
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
	 && comparefunc(elem, base + (pos - 1 ) *size) == 0)
      pos--;
  /* computes number of entries in the table */
  while (pos + *nb < nbelem
	 && comparefunc(elem, base + (pos + *nb) * size) == 0)
    (*nb)++;

  return((void *)(((char *)base) + pos * size));
}


#ifdef _USE_FETCH_
/*
 * you'd better be sure there's no buffer overflow possible with this
 * library before using this feature...
 */
int templateSetFileURL(char *filename)
{
  FILE *fichier;
  char *ptr;
  int pos, taille, debut;
  int tempdebut;
  char tmp[TNOM + 1], nom[TNOM + 1];
  int i;
  int nbvar;
  int type;
  tempBloc *Bloc;
  struct url_stat stat;


  if ((fichier = fetchXGetURL(filename, &stat, "")) == NULL) {
    return(-1);
  }

  templateFreeMem();
  taille = stat.size;
  tempBuf = (char *)malloc(sizeof(char) * taille);
  fread(tempBuf, 1, taille, fichier);
  fclose(fichier);

  tempnbVar = tempnbZone = tempnbTable = 0;
  tempVar = tempZone = tempTable = NULL;
  BlocInit = (tempBloc *)malloc(sizeof(tempBloc));
  memset(BlocInit, 0, sizeof(tempBloc));
  Bloc = BlocInit;
  nbvar = 0;
  tempdebut = 0;
  debut = pos = 0;
  while (pos < taille) {
    if (tempBuf[pos] == '#') {
      if (tempBuf[pos + 1] == '#') {
	/* the encountered code till there is pure html */
	memcpy(tempBuf + tempdebut, tempBuf + debut, pos - debut);
	i = 0;
	Bloc->suiv = (tempBloc *)malloc(sizeof(tempBloc));
	memset(Bloc->suiv, 0, sizeof(tempBloc));
	Bloc->suiv->prec = Bloc;
	Bloc->data = tempBuf + tempdebut;
	Bloc->tdata = pos - debut;
	Bloc->type = TEMP_HTML;
	Bloc = Bloc->suiv;
	tempdebut += pos - debut;

	/* from now on we're in the variable name */
	pos += 2;
	while (tempBuf[pos + i] != '\0'
	       && tempBuf[pos + i] != '#'
	       && i < TNOM)
	  i++;
	memcpy(tmp, tempBuf + pos, i);
	tmp[i] = '\0';

	/* saves the bloc containing the variable */
	Bloc->suiv = (tempBloc *)malloc(sizeof(tempBloc));
	memset(Bloc->suiv, 0, sizeof(tempBloc));
	Bloc->suiv->prec = Bloc;
	Bloc->type = TEMP_VAR;
	Bloc->nom = strdup(tmp);
	/* adds the variable in the shortcut table */
	tempVar = (st_ptr *)realloc(tempVar, sizeof(st_ptr) * (tempnbVar + 1));
	tempVar[tempnbVar].nomchamp = strdup(tmp);
	tempVar[tempnbVar].bloc = Bloc;
	tempnbVar++;

	Bloc = Bloc->suiv;
	nbvar++;
	pos += i + 1;
	debut = pos + 1;
      }	
    }
    /* comments */
    else if (tempBuf[pos] == '<'
	     && tempBuf[pos + 1] == '!'
	     && tempBuf[pos + 2] == '-'
	     && tempBuf[pos + 3] == '-') {
      /* the encountered code till there is pure html */
      memcpy(tempBuf + tempdebut, tempBuf + debut, pos - debut);
      i = 0;
      Bloc->suiv = (tempBloc *)malloc(sizeof(tempBloc));
      memset(Bloc->suiv, 0, sizeof(tempBloc));
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
      while (tempBuf[pos] != '-'
	     && tempBuf[pos] != '>'
	     && !isspace(tempBuf[pos])
	     && i < TNOM) {
	tmp[i++] = tempBuf[pos];
	pos++;
      }
      tmp[i] = '\0';
      while (isspace(tempBuf[pos]))
	pos++;
      i = 0;
      while (tempBuf[pos] != '-'
	     && tempBuf[pos] != '>'
	     && !isspace(tempBuf[pos])
	     && i < TNOM) {
	nom[i++] = tempBuf[pos];
	pos++;
      }
      nom[i] = '\0';
      while (tempBuf[pos] != '>')
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
	/* saves the bloc containing the variable */
	Bloc->suiv = (tempBloc *)malloc(sizeof(tempBloc));
	memset(Bloc->suiv, 0, sizeof(tempBloc));
	Bloc->type = type;
	Bloc->suiv->prec = Bloc;
	if (Bloc->type != TEMP_HTML)
	  Bloc->nom = strdup(nom);
	switch (type) {
	case TEMP_BEGINTABLE:
	  tempTable = (st_ptr *)realloc(tempTable,
					sizeof(st_ptr) * (tempnbTable + 1));
	  tempTable[tempnbTable].nomchamp = strdup(nom);
	  tempTable[tempnbTable].bloc = Bloc;
	  tempnbTable++;
	  break;
	case TEMP_BEGINZONE:
	  tempZone = (st_ptr *)realloc(tempZone,
				       sizeof(st_ptr) * (tempnbZone + 1));
	  tempZone[tempnbZone].nomchamp = strdup(nom);
	  tempZone[tempnbZone].bloc = Bloc;
	  tempnbZone++;
	  break;
	}
	Bloc = Bloc->suiv;
      }
      else {
	/* the whole block is taken for html */
	memcpy(tempBuf + tempdebut, tempBuf + debut, pos + 1 - debut);
	Bloc->suiv = (tempBloc *)malloc(sizeof(tempBloc));
	memset(Bloc->suiv, 0, sizeof(tempBloc));
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

  qsort(tempVar, tempnbVar, sizeof(st_ptr), tempcompare);
  qsort(tempZone, tempnbZone, sizeof(st_ptr), tempcompare);
  qsort(tempTable, tempnbTable, sizeof(st_ptr), tempcompare);
  
  return(0);
}
#endif


int templateSetFile(char *filename)
{
  FILE *fichier;
  char *ptr;
  int pos, taille, debut;
  int tempdebut;
  char tmp[TNOM + 1], nom[TNOM + 1];
  int i;
  int nbvar;
  int type;
  tempBloc *Bloc;


#ifdef DEBUG
  fprintf(stderr, "templateSetFile: %s\n", filename);
  fflush(stderr);
#endif
  if ((fichier = fopen(filename, "r")) == NULL) {
#ifdef DEBUG
    fprintf(stderr, "*** error opening file\n");
    fflush(stderr);
#endif
    return(-1);
  }

  templateFreeMem();
  fseek(fichier, 0, SEEK_END);
  taille = ftell(fichier);
  rewind(fichier);
  tempBuf = (char *)malloc(sizeof(char) * taille);
  fread(tempBuf, 1, taille, fichier);
  fclose(fichier);

  tempnbVar = tempnbZone = tempnbTable = 0;
  tempVar = tempZone = tempTable = NULL;
  BlocInit = (tempBloc *)malloc(sizeof(tempBloc));
  memset(BlocInit, 0, sizeof(tempBloc));
  Bloc = BlocInit;
  nbvar = 0;
  tempdebut = 0;
  debut = pos = 0;
  while (pos < taille) {
    if (tempBuf[pos] == '#') {
      if (tempBuf[pos + 1] == '#') {
	/* the encountered code till there is pure html */
	memcpy(tempBuf + tempdebut, tempBuf + debut, pos - debut);
	i = 0;
	Bloc->suiv = (tempBloc *)malloc(sizeof(tempBloc));
	memset(Bloc->suiv, 0, sizeof(tempBloc));
	Bloc->suiv->prec = Bloc;
	Bloc->data = tempBuf + tempdebut;
	Bloc->tdata = pos - debut;
	Bloc->type = TEMP_HTML;
	Bloc = Bloc->suiv;
	tempdebut += pos - debut;

	/* from now on we're in the variable name */
	pos += 2;
	while (tempBuf[pos + i] != '\0'
	       && tempBuf[pos + i] != '#'
	       && i < TNOM)
	  i++;
	memcpy(tmp, tempBuf + pos, i);
	tmp[i] = '\0';

	/* saves the bloc containing the variable */
	Bloc->suiv = (tempBloc *)malloc(sizeof(tempBloc));
	memset(Bloc->suiv, 0, sizeof(tempBloc));
	Bloc->suiv->prec = Bloc;
	Bloc->type = TEMP_VAR;
	Bloc->nom = strdup(tmp);
	/* adds the variable in the shortcut table */
	tempVar = (st_ptr *)realloc(tempVar, sizeof(st_ptr) * (tempnbVar + 1));
	tempVar[tempnbVar].nomchamp = strdup(tmp);
	tempVar[tempnbVar].bloc = Bloc;
	tempnbVar++;

	Bloc = Bloc->suiv;
	nbvar++;
	pos += i + 1;
	debut = pos + 1;
      }	
    }
    /* comments */
    else if (tempBuf[pos] == '<'
	     && tempBuf[pos + 1] == '!'
	     && tempBuf[pos + 2] == '-'
	     && tempBuf[pos + 3] == '-') {
      /* the encountered code till there is pure html */
      memcpy(tempBuf + tempdebut, tempBuf + debut, pos - debut);
      i = 0;
      Bloc->suiv = (tempBloc *)malloc(sizeof(tempBloc));
      memset(Bloc->suiv, 0, sizeof(tempBloc));
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
      while (tempBuf[pos] != '-'
	     && tempBuf[pos] != '>'
	     && !isspace(tempBuf[pos])
	     && i < TNOM) {
	tmp[i++] = tempBuf[pos];
	pos++;
      }
      tmp[i] = '\0';
      while (isspace(tempBuf[pos]))
	pos++;
      i = 0;
      while (tempBuf[pos] != '-'
	     && tempBuf[pos] != '>'
	     && !isspace(tempBuf[pos])
	     && i < TNOM) {
	nom[i++] = tempBuf[pos];
	pos++;
      }
      nom[i] = '\0';
      while (tempBuf[pos] != '>')
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
	/* saves the bloc containing the variable */
	Bloc->suiv = (tempBloc *)malloc(sizeof(tempBloc));
	memset(Bloc->suiv, 0, sizeof(tempBloc));
	Bloc->type = type;
	Bloc->suiv->prec = Bloc;
	if (Bloc->type != TEMP_HTML)
	  Bloc->nom = strdup(nom);
	switch (type) {
	case TEMP_BEGINTABLE:
	  tempTable = (st_ptr *)realloc(tempTable,
					sizeof(st_ptr) * (tempnbTable + 1));
	  tempTable[tempnbTable].nomchamp = strdup(nom);
	  tempTable[tempnbTable].bloc = Bloc;
	  tempnbTable++;
	  break;
	case TEMP_BEGINZONE:
	  tempZone = (st_ptr *)realloc(tempZone,
				       sizeof(st_ptr) * (tempnbZone + 1));
	  tempZone[tempnbZone].nomchamp = strdup(nom);
	  tempZone[tempnbZone].bloc = Bloc;
	  tempnbZone++;
	  break;
	}
	Bloc = Bloc->suiv;
      }
      else {
	/* the whole block is taken for html */
	memcpy(tempBuf + tempdebut, tempBuf + debut, pos + 1 - debut);
	Bloc->suiv = (tempBloc *)malloc(sizeof(tempBloc));
	memset(Bloc->suiv, 0, sizeof(tempBloc));
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

  qsort(tempVar, tempnbVar, sizeof(st_ptr), tempcompare);
  qsort(tempZone, tempnbZone, sizeof(st_ptr), tempcompare);
  qsort(tempTable, tempnbTable, sizeof(st_ptr), tempcompare);
  
  return(0);
}


void templateSetVar(char *champ, char *val)
{
  tempBloc *Bloc;
  st_ptr *rec;
  int i, nb;


  if (val == NULL)
    return;
  rec = tempsearch(champ, tempVar, tempnbVar, sizeof(st_ptr), tempcompnom,
		   &nb);
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

  rec = tempsearch(champ, tempZone, tempnbZone, sizeof(st_ptr), tempcompnom,
		   &nb);
  if (rec) {
    for (i = 0; i < nb; i++) {
      Bloc = rec[i].bloc;
      Bloc->tdata = 1;
    }
  }
}


void templateDumpBloc(tempBloc *BlocP, tempBloc *Bloc)
{
  tempBloc *BlocN;


  if (!(Bloc->type == TEMP_HTML || Bloc->type == TEMP_VAR)
      || Bloc->type == TEMP_VAR && Bloc->data == NULL)
    return;

  BlocN = (tempBloc *)malloc(sizeof(tempBloc));
  memset(BlocN, 0, sizeof(tempBloc));
  BlocN->data = (char *)malloc(Bloc->tdata);
  memcpy(BlocN->data, Bloc->data, Bloc->tdata);
  BlocN->tdata = Bloc->tdata;
  BlocN->alloc = 1;
  BlocN->type = TEMP_HTML;
  BlocN->prec = BlocP->prec;
  if (BlocP->prec)
    BlocP->prec->suiv = BlocN;
  BlocN->suiv = BlocP;
  BlocP->prec = BlocN;
}


void templateDumpTableLine(char *ligne)
{
  tempBloc *Bloc, *BlocS;
  tempBloc *BlocTableS, *BlocN, *BlocO, *BlocD;
  int pr;
  int i, nb;
  st_ptr *rec;


  rec = tempsearch(ligne, tempTable, tempnbTable, sizeof(st_ptr), tempcompnom,
		   &nb);
  if (rec) {
    pr = 1;

    for (i = 0; i < nb; i++) {
      Bloc = rec[i].bloc;

      /*
       * insert a mark "beginning of generated code"
       */
      BlocN = (tempBloc *)malloc(sizeof(tempBloc));
      memset(BlocN, 0, sizeof(tempBloc));
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
		 && !curzone
		 && Bloc->tdata != 1) {
	  free(curzone);
	  curzone = strdup(Bloc->nom);
	  pr = 0;
	}
	else if (Bloc->type == TEMP_ENDZONE
		 && curzone
		 && strcmp(Bloc->nom, curzone) == 0) {
	  pr = 1;
	  free(curzone);
	  curzone = NULL;
	}
	else if (Bloc->type == TEMP_BEGINTABLE)	{
	  curtable = strdup(Bloc->nom);
	  Bloc = Bloc->suiv;
	  while (Bloc
		 && !(Bloc->type == TEMP_ENDTABLE
		      && strcmp(Bloc->nom, curtable) == 0)) {
	    Bloc = Bloc->suiv;
	  }
	  free(curtable);
	}
	else {
	  if (pr) {
	    templateDumpBloc(BlocS, Bloc);
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
      BlocN = (tempBloc *)malloc(sizeof(tempBloc));
      memset(BlocN, 0, sizeof(tempBloc));
      BlocN->suiv = BlocO;
      BlocN->prec = BlocO->prec;
      BlocO->prec->suiv = BlocN;
      BlocO->prec = BlocN;
      BlocN->type = TEMP_ENDGENTABLE;
    }
  }
}


void templatePrint()
{
  tempBloc *Bloc;
  char *ptr;
  int pr;


  Bloc = BlocInit;
  pr = 1;
  while (Bloc) {
    if (Bloc->type == TEMP_BEGINTABLE) {
      curtable = strdup(Bloc->nom);
      Bloc = Bloc->suiv;
      while (Bloc
	     && !(Bloc->type == TEMP_ENDTABLE
		  && strcmp(Bloc->nom, curtable) == 0)) {
	Bloc = Bloc->suiv;
      }
      free(curtable);
    }
    else if (Bloc->type == TEMP_BEGINZONE && !curzone && Bloc->tdata != 1) {
      pr = 0;
      free(curzone);
      curzone = strdup(Bloc->nom);
    }
    else if (Bloc->type == TEMP_ENDZONE
	     && curzone
	     && strcmp(Bloc->nom, curzone) == 0) {
      pr = 1;
      free(curzone);
      curzone = NULL;
    }
    else if (pr && (Bloc->type == TEMP_HTML || Bloc->type == TEMP_VAR)
	     && Bloc->data) {
      ptr = (char *)malloc(Bloc->tdata + 1);
      memcpy(ptr, Bloc->data, Bloc->tdata);
      ptr[Bloc->tdata] = '\0';
      fputs(ptr, stdout);
      free(ptr);
    }
    if (Bloc)
      Bloc = Bloc->suiv;
  }
}


char *templatePage()
{
  char *tBufp;
  int taillep = 0;

  tempBloc *Bloc;
  int pr;


  tBufp = NULL;
  Bloc = BlocInit;
  pr = 1;
  while (Bloc) {
    if (Bloc->type == TEMP_BEGINTABLE) {
      curtable = strdup(Bloc->nom);
      Bloc = Bloc->suiv;
      while (Bloc
	     && !(Bloc->type == TEMP_ENDTABLE
		  && strcmp(Bloc->nom, curtable) == 0)) {
	Bloc = Bloc->suiv;
      }
      free(curtable);
    }
    else if (Bloc->type == TEMP_BEGINZONE && !curzone && Bloc->tdata != 1) {
      pr = 0;
      free(curzone);
      curzone = strdup(Bloc->nom);
    }
    else if (Bloc->type == TEMP_ENDZONE
	     && curzone
	     && strcmp(Bloc->nom, curzone) == 0) {
      pr = 1;
      free(curzone);
      curzone = NULL;
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


void templateFreeMem()
{
  tempBloc *Bloc, *BlocO;
  int i;


  Bloc = BlocInit;
  while (Bloc) {
    if (Bloc->nom)
      free(Bloc->nom);
    if (Bloc->alloc && Bloc->data)
      free(Bloc->data);
    BlocO = Bloc;
    Bloc = Bloc->suiv;
    free(BlocO);
  }
  BlocInit = NULL;
  for (i = 0; i < tempnbVar; i++)
    free(tempVar[i].nomchamp);
  for (i = 0; i < tempnbZone; i++)
    free(tempZone[i].nomchamp);
  for (i = 0; i < tempnbTable; i++)
    free(tempTable[i].nomchamp);
  if (tempVar) {
    free(tempVar);
    tempVar = NULL;
  }
  if (tempZone) {
    free(tempZone);
    tempZone = NULL;
  }
  if (tempTable) {
    free(tempTable);
    tempTable = NULL;
  }
  tempnbVar = tempnbZone = tempnbTable = 0;

  if (tempBuf) {
    free(tempBuf);
    tempBuf = NULL;
  }
  if (curzone) {
    free(curzone);
    curzone = NULL;
  }
}

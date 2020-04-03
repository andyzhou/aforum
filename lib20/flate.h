/**************************************************************************
 *                                                                        *
 * Flate library (Fast Template)                                          *
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

#ifndef _LIB_FLATE_H_
#define _LIB_FLATE_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************ģ������FastTemplate�ӿں���***************/

int templateSetFile(char *filename);//����ģ�庯��

void templateSetVar(char *champ, char *val);//����Ԫ�ؼ�ֵ����

void templateDumpTableLine(char *ligne);//BLOCK���������Ӻ���

void templatePrint();//����������

char *templatePage();//���ָ��ģ�����ֵ

void templateFreeMem();//�ڴ��ͷź���

int templateSetFileURL(char *filename);//��Զ��ģ�庯��


#ifdef __cplusplus
}
#endif

#endif

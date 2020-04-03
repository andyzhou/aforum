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

/******************模板引擎FastTemplate接口函数***************/

int templateSetFile(char *filename);//载入模板函数

void templateSetVar(char *champ, char *val);//设置元素及值函数

void templateDumpTableLine(char *ligne);//BLOCK分析、连接函数

void templatePrint();//结果输出函数

char *templatePage();//获得指定模板变量值

void templateFreeMem();//内存释放函数

int templateSetFileURL(char *filename);//打开远程模板函数


#ifdef __cplusplus
}
#endif

#endif

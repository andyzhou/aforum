/*
* Copyright (c) 2007
* All rights reserved.
* 
* �ļ����ƣ�mod_union.h
* �ļ���ʶ��ͳһ��Աģ�麯������
* ��    �ߣ���־��(����)
*/

#ifndef MOD_UNION_H
#define MOD_UNION_H

//�ӽ��̳�ʼ��
static void union_child_init(server_rec *s, pool *p);

///ģ���ʼ��
void union_mod_init(request_rec* r);

//ͳһ��Աģ�����
static int union_handler(request_rec* r);

#endif

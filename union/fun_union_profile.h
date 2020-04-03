/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name: fun_union_profile.h
* file desc: member edit profile
* author:AndyZhou<diudiu8848@163.com>
*/

#ifndef UNION_PROFILE_H
#define UNION_PROFILE_H


//edit profile
void member_profile_edit(request_rec* r, af_app_config *facfg);

//save profile
void member_profile_save(request_rec* r, af_app_config *facfg, ApacheRequest *req);

#endif


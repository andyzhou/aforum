#ifndef FUN_UNION_INTERFACE_H
#define FUN_UNION_INTERFACE_H

//union interface
int union_get_method(request_rec* r, af_app_config* uacfg, apr_table_t* param_list);
int union_post_method(request_rec* r, af_app_config* uacfg, ApacheRequest *req);

#endif


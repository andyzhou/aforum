#ifndef API_LDAP_H
#define API_LDAP_H

#include "ldap.h"

#ifdef __cplusplus
extern "C" {
#endif

//ladp param struct
typedef struct ldapCfg {
	char ldapHost[64];
	int	 ladpPort;//default 389
	char ldapBindDN[128];
	char ldapBindPwd[64];
} ldapCfg;


//ldap attribute struct
typedef struct ldapAttrVal {
	unsigned int	mod_op;
	char	mod_type[32];
	char	*mod_value[2];
} ldapAttrVal;


//ldap signal node value struct
typedef struct sigNodeVal {
	char attrName[32];
	char attrVal[128];
} sigNodeVal;


//ldap record list struct
//typedef struct 

////////////////////////////////////////////////////

//init and connect ldap searver
int ldapInitServer(LDAP **ld, ldapCfg lcfg, char* errMsg);

//unbind ldap server
void ldapClose(LDAP *ld);

//get signal node values
int ldapGetSignalNode(LDAP *ld, char* dnStr, char* filterStr, sigNodeVal **snv);

//get node record list
int ldapGetNodeList(LDAP *ld, char* dnStr, char* filterStr, LDAPMessage **recList);


//add and init signal LDAPMod struct
int ldapInitSigMod(LDAPMod **lmod, int optMethod, char* attrName, char *attrVals[], int attrValNums);

//add signal node record
int ldapAddNode(LDAP *ld, char* dnStr, LDAPMod *listOfAttrs[], char* errMsg);

//update signal node record
int ldapModNode(LDAP *ld, char* dnStr, LDAPMod *listOfAttrs[], char* errMsg);

//delete signal node record
int ldapDelNode(LDAP *ld, char* dnStr, char* errMsg);

#ifdef __cpluplus 
}
#endif

#endif

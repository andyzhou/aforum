/*
 * ldap interface api
 * @author zhirui 
 * @create 2007-03-27
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ldap.h"
#include "api_ldap.h"

#define SCOPE LDAP_SCOPE_SUBTREE

//init and connect ldap server
//return  -1: error param 0:failed 1:success
int ldapInitServer(LDAP **ld, ldapCfg lcfg, char* errMsg)
{
	int version = 0;
	int rc = 0;

	if(lcfg.ladpPort <= 0 || !lcfg.ldapHost)
	{
		return -1;
	}

	if(!lcfg.ldapBindDN || !lcfg.ldapBindPwd)
	{
		return -1;
	}

	//init ldap server
	*ld = ldap_init(lcfg.ldapHost, lcfg.ladpPort);

	if(*ld == NULL)
	{
		sprintf(errMsg, "Init ldap server failed!");
		return 0;
	}

	version = LDAP_VERSION3;

	//set ldap option
	ldap_set_option(*ld, LDAP_OPT_PROTOCOL_VERSION, &version);

	//bind ldap server
	rc = ldap_simple_bind_s(*ld, lcfg.ldapBindDN, lcfg.ldapBindPwd);

	if(rc != LDAP_SUCCESS)
	{
		sprintf(errMsg, "Bind ldap server failed:%s", ldap_err2string(rc));
		return 0;
	}

	return 1;
}


//unbind ldap server
void ldapClose(LDAP *ld)
{
	if(ld)
	{
		ldap_unbind(ld);
	}
}


//get signal node values
//return  -1: error param 0:failed  >=1:suit attribute numbers
int ldapGetSignalNode(LDAP *ld, char* dnStr, char* filterStr, sigNodeVal **snv)
{
	LDAPMessage *result, *e;
	BerElement *ber;
	char *dn;
	char *attrName;
	char **attrVal;
	int rc	 = 0;
	int i	 = 0;
	int cols = 2;

	if(!ld)
	{
		return -1;
	}

	if(!dnStr || !(*dnStr))
	{
		return -1;
	}

	if(!filterStr || !(*filterStr))
	{
		return -1;
	}

	rc = ldap_search_ext_s(ld, dnStr, SCOPE, filterStr, NULL, 0, 
    NULL, NULL, NULL, 0, &result);

	if(rc != LDAP_SUCCESS)
	{
		return 0;
	}

	e = ldap_first_entry(ld, result);

	if(e == NULL)
	{
		return 0;
	}
	
	/*
	if((dn = ldap_get_dn(ld, e)) != NULL)
	{
		printf( "dn: %s\n<br>", dn);
		//ldap_memfree(dn);
	}
	*/

	//init one enum of struct
	*snv = (sigNodeVal*)malloc(sizeof(sigNodeVal));

	for(attrName = ldap_first_attribute(ld, e, &ber), i = 0; attrName != NULL; attrName = ldap_next_attribute(ld, e, ber))
	{
		//get all attribute name and value
		if((attrVal = ldap_get_values(ld, e, attrName)) != NULL)
		{
			//for ( i = 0; attrVal[i] != NULL; i++ )
			//{
			//	printf("attrName:%s: val:%s\n", attrName, attrVal[i]);
			//}
			
			//printf("attrName:%s, val:%s\n", attrName, *attrVal);
			*snv = (sigNodeVal*)realloc(*snv, cols * sizeof(sigNodeVal));
			sprintf((char*)&(*snv)[i].attrName, "%s", attrName);
			sprintf((char*)&(*snv)[i].attrVal, "%s", *attrVal);


			ldap_value_free(attrVal);
			
			i++;
			cols = i;
		}

		ldap_memfree(attrName);
	}

	if(ber != NULL)
	{
		ber_free(ber, 0);
	}

	ldap_msgfree(result);
	
	return cols;
}


//get node record list
//return  -1: error param 0:failed 1:success
int ldapGetNodeList(LDAP *ld, char* dnStr, char* filterStr, LDAPMessage **recList)
{
	int rc = 0;

	if(!ld)
	{
		return -1;
	}

	if(!dnStr || !(*dnStr))
	{
		return -1;
	}

	if(!filterStr || !(*filterStr))
	{
		return -1;
	}

	rc = ldap_search_ext_s(ld, dnStr, SCOPE, filterStr, NULL, 0, 
    NULL, NULL, NULL, 0, recList);

	if(rc != LDAP_SUCCESS)
	{
		return 0;
	}

	return 1;
}



//add and init signal LDAPMod struct
//optMethod 1:LDAP_MOD_ADD 2:LDAP_MOD_REPLACE
//return  -1: error param 0:failed 1:success
int ldapInitSigMod(LDAPMod **lmod, int optMethod, 
				   char* attrName, char *attrVals[], int attrValNums)
{
	int i = 0;

	if(!attrName || !(*attrName))
	{
		return -1;
	}

	if(!attrVals)
	{
		return -1;
	}

	/*
	if(modNum <= 1)
	{
		//*lmod = (LDAPMod*)malloc(sizeof(LDAPMod));
		*lmod = (LDAPMod *)malloc (sizeof(LDAPMod *));
	}
	else
	{
		//*lmod = (LDAPMod*)realloc(*lmod, modNum * sizeof(LDAPMod));
		*lmod = (LDAPMod *)realloc(*lmod, modNum * sizeof(LDAPMod *));
	}
	*/

	*lmod = (LDAPMod *)malloc (sizeof(LDAPMod *));

	(*lmod)->mod_op = optMethod;
	(*lmod)->mod_type = attrName;
	(*lmod)->mod_vals.modv_strvals = (char **)malloc (attrValNums * sizeof (char **));

	if(attrValNums > 1)
	{
		for(i = 0; i < attrValNums - 1; i++)
		{
			(*lmod)->mod_vals.modv_strvals[i] = strdup(attrVals[i]);
		}

		(*lmod)->mod_vals.modv_strvals[attrValNums-1] = NULL;
	}
	//(*lmod)->mod_vals.modv_strvals = attrVals;
	//(*lmod)->mod_vals.modv_strvals[0] = attrVal;
	//(*lmod)->mod_vals.modv_strvals[1] = NULL;
}


/*
 * add signal node record
 *
 * @param	LDAP *ld					handler of ldap connect object
 * @param	char* dnStr					dn string will be add or update
 * @param   LDAPMod *listOfAttrs[]		attribute and values of ldap
 */
//return  -1: error param 0:failed 1:success
int ldapAddNode(LDAP *ld, char* dnStr, LDAPMod *listOfAttrs[], char* errMsg)
{
	int rc	= 0;

	if(!ld)
	{
		return -1;
	}

	if(!dnStr || !(*dnStr))
	{
		return -1;
	}

	if(!listOfAttrs)
	{
		return -1;
	}

	if(!ld)
	{
		return -1;
	}

	/* Add new record */
	rc = ldap_add_s(ld, dnStr, listOfAttrs);

	if(rc != LDAP_SUCCESS )
	{
		sprintf(errMsg, "Add Node failed:%s", ldap_err2string(rc));
		return 0;
	}
	
	return 1;
}


//update signal node record
//return  -1: error param 0:failed 1:success
int ldapModNode(LDAP *ld, char* dnStr, LDAPMod *listOfAttrs[], char* errMsg)
{
	int rc	= 0;

	if(!ld)
	{
		return -1;
	}

	if(!dnStr || !(*dnStr))
	{
		return -1;
	}

	if(!listOfAttrs)
	{
		return -1;
	}

	if(!ld)
	{
		return -1;
	}

	/* modify a record */
	rc = ldap_modify_s(ld, dnStr, listOfAttrs);
	//rc = ldap_modify(ld, dnStr, listOfAttrs);

	if(rc != LDAP_SUCCESS )
	{
		sprintf(errMsg, "Update Node failed:%s", ldap_err2string(rc));
		return 0;
	}
	
	return 1;
}


//delete signal node record
//return  -1: error param 0:failed 1:success
int ldapDelNode(LDAP *ld, char* dnStr, char* errMsg)
{
	int rc	= 0;

	if(!ld)
	{
		return -1;
	}

	if(!dnStr || !(*dnStr))
	{
		return -1;
	}

	//delete a record
	rc = ldap_delete_s(ld, dnStr);

	if(rc != LDAP_SUCCESS )
	{
		sprintf(errMsg, "Delete Node failed:%s", ldap_err2string(rc));
		return 0;
	}
	
	return 1;
}

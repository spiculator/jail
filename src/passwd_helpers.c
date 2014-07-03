///////////////////////////////////////////////////////////////////////////////
//
// Jail
// A chrooted login shell implementation
// Released under GNU Public License v.2.0
// See LICENSE file for details
//
// Juan M. Casillas <assman@gsyc.inf.uc3m.es>
// 3/April/ 2001 09:22
// 
///////////////////////////////////////////////////////////////////////////////
//
// $Id: passwd_helpers.c,v 1.1.1.1 2001/10/26 09:36:09 assman Exp $
//
// $Log: passwd_helpers.c,v $
// Revision 1.1.1.1  2001/10/26 09:36:09  assman
// Added support for new platforms: FreeBSD, Solaris, IRIX. Now some options
// can be selected from the Makefile script: DEBUG on/off, install path,
// install permissions, etc. The perl scripts have been rewritten so they
// support platform-specific code, so port Jail to another platform should
// be an easy task.
//
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "passwd_helpers.h"

void free_passwd_data(passwd_data *p) {

    if (p == NULL)
	return;

    if (p->login!=NULL) free(p->login);
    if (p->pass!=NULL)  free(p->pass);
    if (p->gecos!=NULL) free(p->gecos);
    if (p->dir!=NULL)   free(p->dir);
    if (p->shell!=NULL) free(p->shell);

    free(p);

}

int fill_from_string(char *str, passwd_data *data) {
    char *p = str;
    char buff[LINE_SZ];
    int n = 0;
    int state = 0;

    memset(buff,'\0',LINE_SZ);

    while (*p != '\0') {

	if ((*p == ':') || (*p == '\n')) {
	    switch (state) {
	    case 0: 
		data->login = (char *)malloc(strlen(buff)+1);
		memset(data->login,'\0',strlen(buff)+1);
		strcpy(data->login,buff);
		break;
	    case 1: 
		data->pass = (char *)malloc(strlen(buff)+1);
		memset(data->pass,'\0',strlen(buff)+1);
		strcpy(data->pass,buff);
		break;
	    case 2: 
		data->id = atoi(buff);
		break;
	    case 3: 
		data->gid = atoi(buff);
		break;
	    case 4: 
		data->gecos = (char *)malloc(strlen(buff)+1);
		memset(data->gecos,'\0',strlen(buff)+1);
		strcpy(data->gecos,buff);
		break;
	    case 5: 
		data->dir = (char *)malloc(strlen(buff)+1);
		memset(data->dir,'\0',strlen(buff)+1);
		strcpy(data->dir,buff);
		break;
	    case 6: 
		data->shell = (char *)malloc(strlen(buff)+1);
		memset(data->shell,'\0',strlen(buff)+1);
		strncpy(data->shell,buff,strlen(buff));
		break;
	    }
	    
	    memset(buff,'\0',LINE_SZ);
	    n=0;
	    state++;
	}
	else {
	    buff[n] = *p;
	    n++;
	}
	
	p++;
    }

    //    printf("-B--------\n");
    //    if (state > 0) printf("login  %s\n", data->login);
    //    if (state > 1) printf("pass   %s\n", data->pass);
    //    if (state > 2) printf("uid    %d\n", data->id);
    //    if (state > 3) printf("gid    %d\n", data->gid);
    //    if (state > 4) printf("gecos  %s\n", data->gecos);
    //    if (state > 5) printf("dir    %s\n", data->dir);
    //    if (state > 6) printf("shell  %s\n", data->shell);
    //    printf("-E--------\n");
    
    if (state > 6) 
	return(1);

    return(0);
}


passwd_data *getpasswddata(int uid) {
    char line[LINE_SZ];
    passwd_data *pd = NULL;
    FILE *f = NULL;

    f = fopen(PASSWD_FILE,"r");

    if (!f) 
	return(pd);

    while (!feof(f)) {

	memset(line,'\0',LINE_SZ);

	if (fgets(line,LINE_SZ,f)==NULL) {
	    pd = NULL;
	    break;
	}

	pd = (passwd_data *)malloc(sizeof(passwd_data));
	memset(pd,'\0',sizeof(passwd_data));

	if (!fill_from_string(line,pd)) {
	    free_passwd_data(pd);
	    continue;
	}

	if (pd->id == uid) 
	    break;

	free_passwd_data(pd);
    } 

    fclose(f);
    return(pd);

};

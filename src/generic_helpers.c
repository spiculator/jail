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
// $Id: generic_helpers.c,v 1.1.1.1 2001/10/26 09:36:09 assman Exp $
//
// $Log: generic_helpers.c,v $
// Revision 1.1.1.1  2001/10/26 09:36:09  assman
// Added support for new platforms: FreeBSD, Solaris, IRIX. Now some options
// can be selected from the Makefile script: DEBUG on/off, install path,
// install permissions, etc. The perl scripts have been rewritten so they
// support platform-specific code, so port Jail to another platform should
// be an easy task.
//
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h> // temporal

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

#if defined( __SOLARIS__) || defined(__IRIX__)
     #include <sys/types.h>
     #include <sys/mkdev.h>
#endif

#include "types.h"

//////////////////////////////////////////////////////////////////////////////
//
// init_env()
// Duplicate a string and save it into the pointer pointed by ptr
//
// INPUT args:
//
//       **ptr        the pointer that will point to the dup string
//       *str         a pointer to the string
//
// OUTPUT args:
//
//       noting
//
//////////////////////////////////////////////////////////////////////////////

void init_env(char **ptr, char *str) {

    *ptr = (char *)malloc(strlen(str)+1);
    memset(*ptr,0,strlen(str)+1);
    strcpy(*ptr,str);
}

//////////////////////////////////////////////////////////////////////////////
//
// generate_env()
// Create the terminal device.
//
// INPUT args:
//
//       *chroot:   path to the chrooted environment
//       *data:     the path name of the device, and the major and minor 
//       uid:       user id 
//       gid:       group id
//  
// OUTPUT args:
//
//       noting
//
//////////////////////////////////////////////////////////////////////////////

void generate_dev(char *chroot, struct device_data *data, 
		  int uid, int gid, int perm) {
    char *full_name;
    int sz;

    sz = strlen(chroot)+strlen(data->path)+2;

    full_name = (char *)malloc(sz);
    memset(full_name,'\0',sz);
    strcpy(full_name,chroot);
    strcat(full_name,"/");
    strcat(full_name,data->path);

    mknod(full_name,S_IFCHR, makedev(data->major,data->minor));
    chown(full_name,uid,gid);
    chmod(full_name, perm);

    free(data);
    free(full_name);
}

//////////////////////////////////////////////////////////////////////////////
//
// build_directory()
// create a directory under the chrooted environment with permissions 755
//
// INPUT args:
//
//      
// OUTPUT args:
//
//       nothing
//
//////////////////////////////////////////////////////////////////////////////

void build_directory(char *chroot, char *dir, int num_perm, int perm) {
    char *full_name;
    int sz;
    
    assert(dir != NULL);
    assert(chroot != NULL);

    sz = strlen(chroot)+strlen(dir)+2;
    full_name = (char *)malloc(sz);
    memset(full_name,'\0',sz);

    strcpy(full_name,chroot);
    strcat(full_name,"/");
    strcat(full_name,dir);


    mkdir(full_name,num_perm);
    chmod(full_name,perm);


    free(full_name);
}

//////////////////////////////////////////////////////////////////////////////
//
// extract_dir
//
// INPUT ARGS:
//             char *path: a full path (may include file or directories)
//
// OUTPUT ARGS:
//	       char *: the directory component, or NULL if it's a file
//
//////////////////////////////////////////////////////////////////////////////

char *extract_dir(char *path) {

    char *dir;
    char *tmpdir;
    char *tok;
    char **elem;
    int pos;
    int n;
    int sz;
    int first_bar;

    if (path == NULL) 
	return(NULL);

    tmpdir = (char *)malloc(strlen(path)+1);
    memset(tmpdir,'\0',strlen(path)+1);
    strcpy(tmpdir,path);

    //
    // count elements to do the malloc
    //

    n=0;
    tok=strtok(tmpdir,"/");
    while (tok != NULL) {
	n++;
	tok=strtok(NULL,"/");
    }

    //
    // [note SIZE of a pointer to char]
    // create the array and fill it with ceroes
    //

    elem = malloc(sizeof(char *)*(n+1));

    free(tmpdir);
    tmpdir = (char *)malloc(strlen(path)+1);
    memset(tmpdir,'\0',strlen(path)+1);
    strcpy(tmpdir,path);

    pos=0;
    first_bar=0;
    tok=strtok(tmpdir,"/");

    //
    // build the element array, endded with NULL
    //

    while (tok != NULL) {
	elem[pos] = (char *)malloc(strlen(tok)+1);
	memset(elem[pos],'\0',strlen(tok)+1);
	strcpy(elem[pos],tok);
	pos++;
	tok=strtok(NULL,"/");
	// printf("%d -> %s\n",pos-1,elem[pos-1]);
    }
    elem[pos]=NULL;


    sz = 1; // at least, we have the cero ended string

    for (n=0; n<pos-1; n++)
	sz += strlen(elem[n]);

    if (path[0] == '/') {
	sz++;
	first_bar = 1;
    }
    else 
	sz += 2; // for "./";

    if (pos-1 <= 0)  // for the trailing dot (.)
	sz++;

    dir = (char *)malloc(sz+pos-2+1);
    memset(dir,'\0',sz+pos-2+1);
    
    if (first_bar)
	strcpy(dir,"/");
    else
	strcpy(dir,"./");
    
    for (n=0; n<pos-1; n++) {
	strcat(dir,elem[n]);
	if (n+1 < pos-1)  // if there is another one more, add a trailing /
	    strcat(dir,"/");
    }

    //
    // if no elements, add the dot
    //

    if (pos-1 <= 0) 
	strcat(dir,".");

    //
    // free data
    //

    for(n=0; n< pos; n++)
	free(elem[n]);
    free(elem);


    return(dir);
}

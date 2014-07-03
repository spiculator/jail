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
// $Id: jail.c,v 1.1.1.1 2001/10/26 09:36:09 assman Exp $
//
// $Log: jail.c,v $
// Revision 1.1.1.1  2001/10/26 09:36:09  assman
// Added support for new platforms: FreeBSD, Solaris, IRIX. Now some options
// can be selected from the Makefile script: DEBUG on/off, install path,
// install permissions, etc. The perl scripts have been rewritten so they
// support platform-specific code, so port Jail to another platform should
// be an easy task.
//
// Revision 1.8  2001/05/07 11:29:42  assman
// erased #define SHELL due it not user any more
//
// Revision 1.7  2001/04/16 08:47:04  assman
// fixed a memory leak bug, fixed the directory read bug also
//
// Revision 1.5  2001/04/10 15:42:13  assman
// added documentation, change the Makefile to check de dependencies
// with mkenv.sh added TODO file, removed debug messages in the code
//
// Revision 1.4  2001/04/10 10:33:23  assman
// added CHANGELOG, INSTALL files, moved mkenv.sh for each platform,
// added new info to the documentation
//
// Revision 1.2  2001/04/03 10:28:36  assman
// removed binary file added by error
// added CVS tags
//
// 11/April/2001 --  I lost the cvs in my portable PC :(
//                   We are going to do a major cleanup in the code
//                   and improve some parts of Jail
//
// 13/April/2001 --  The user can choose the shell he wants using the
//                   "chrooted" /etc/passwd file. Major improvements in
//                   mkenv.sh, now it supports guessing the libraries
//                   it will use (we need to fine-tune this process using
//                   strace, this program is not installed on my
//                   portable PC :( )
//
// 05/July/2001  --  Fixed a segmentation fault error when trying to
//                   execute jail form the shell (there are not valid
//                   jail environment created in a valid user account)
//
// 17/Oct/2001   --  Mayor rewrite of some helper applications, and jail.c
//                   code. Now Solaris and FreeBSD (and maybe OpenBSD) are
//                   supported. Better platform configuration support.
//                   Added some path checking, so ./ and /../ should work
//
///////////////////////////////////////////////////////////////////////////////
//
// NOTES
//
// File must be owned by root and have the SUID permissions
// (4755 runs fine) to allow proper operation. Also the chroot
// environment must be setup right. Se documentation files (README, INSTALL)
// for configuration and instalation
//
// ABOUT SSH & SCP
//
// scp passess the following parameters to Jail:
//
// jail -c scp -t /tmp
//
// from the scp's manual
//
// -t exit after reading and executing one command
// -c string       If  the  -c option is present, then commands are
//                 read from string.  If there are arguments  after
//                 the  string, they are assigned to the positional
//                 parameters, starting with $0.
//
//
// the solution is to pass all the parameters that are passing to jail to 
// the subshell, so the programs start to run. Also, we need to manage the
// terminal devices for proper work
//
// realpath() on IRIX requires that the directory exists !
//
///////////////////////////////////////////////////////////////////////////////
//
// BUGS
//
// some curious happen when trying to run jail in an IRIX 6.5
// when it starts, it becomes killed by itself. Tracing it, I
// think is a problem with the execve() system call. 
// Work arrounds for this platform are wellcome.
//
// FIXED !!! NOW jail runs on IRIX !
//
///////////////////////////////////////////////////////////////////////////////

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <grp.h>
#include <sys/stat.h>

#include "globals.h"
#include "types.h"
#include "helpers.h"
#include "types.h"

//////////////////////////////////////////////////////////////////////////////
//
// main()
// main execution loop
// 
// INPUT args:
//
//       argc           an integer that counts the number of arguments
//       *argv[]        an array of pointers pointing to the arguments
//       *env[]         an array of pointers pointing to the enviroment 
//                      variables passed from the caller program (usually, 
//                      a login daemon, just like telnetd, sshd, etc).
//
// OUTPUT args:
// 
//       Jail nevers returns (is overwritten by another process)
//       if some error ocurrs then the following values are returned:
//
//
//            -1        1) bad setuid
//                      2) can't get user info for passwd file
//             
//            -2        In the following cases:
// 
//                      1) can't do a chdir()
//                      2) can't do a chroot()
//             
//            -3        1) can't do execve() (spawn the shell)
//
//            -4        If we reached the last line of jail, that was a 
//                      really big problem, because jail should never 
//                      returns
//
//////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[], char *env[]) {
    int ret;
    char **lenv;
    char **larg;
    int n;
    passwd_data *pwdent,*cpwdent;
    int uid, gid;
    char *home_dir;
    char *shell_path;
    char *ptr_home_dir;
    int argc_cnt;
    int terminal_gid;
    char *tmp_str;

    struct group *tty_group_info;
    struct device_data *dev_tty, *dev_tid;

    //
    // Get the information about the current
    // terminal device (tty or pts)
    //

    dev_tid = get_tid_info();
    dev_tty = get_tty_info();


    #if DEBUG != 0
    if (dev_tid != NULL)
	fprintf(stderr,
		"jail: tid data: %s (M:%d,m:%d)\n",
		dev_tid->path, dev_tid->major, dev_tid->minor);
    if (dev_tty != NULL)
	fprintf(stderr,
		"jail: tty data: %s (M:%d,m:%d)\n",
		dev_tty->path, dev_tty->major, dev_tty->minor);
    #endif

    //
    // Get user id and group id, then try to become root privilegies
    //

    uid = getuid();
    gid = getgid();

    if (setgid(0) || setuid(0) || getuid() != geteuid() || 
	getgid() != getegid()) {
	fprintf(stderr,
		"jail: can't chroot(). "
		"(Is jail owned by root and has setuid permissions?)\n");
	return(-1);
    }

    //
    // Get the user's info from the passwd file
    //

    pwdent = getpasswddata(uid);
    if (pwdent == NULL) {
	fprintf(stderr,"jail: can't get passwd info for uid %d\n",uid);
	exit(-1);
    }

    //
    // create the terminal devices here
    // (platform specific)
    //
    // Works for Linux, Solaris and FreeBSD
    // it should run in another platforms,
    // but it not tested.
    //

    tty_group_info = getgrnam(TERMINAL_GROUP);
    if (tty_group_info == NULL)
	terminal_gid = gid;
    else
	terminal_gid = tty_group_info->gr_gid;

    if (dev_tid != NULL) {
	build_directory(pwdent->dir,extract_dir(dev_tid->path),
			DIR_PERM_NUM, DIR_PERM);
	generate_dev(pwdent->dir,dev_tid,uid,terminal_gid,DEV_PERM);
    }

    if (dev_tty != NULL) {
	build_directory(pwdent->dir,extract_dir(dev_tty->path),
			DIR_PERM_NUM, DIR_PERM);
	generate_dev(pwdent->dir,dev_tty,uid,terminal_gid,DEV_PERM);
    }

    //
    // count elements and generate space for the array
    // then, save all the environment variables. Leave
    // HOME and SHELL entries untouched
    //

    n=0;
    while (env[n]!=NULL) n++;  
    n++;

    lenv = malloc(sizeof(char *)*n);
    memset(lenv,'\0',n);

    n=argc_cnt=0;
    while (env[n]) {
	if ((strstr(env[n],"HOME")==NULL) && 
	    (strstr(env[n],"SHELL")==NULL)) {
	    init_env(&lenv[argc_cnt],env[n]); 

	    argc_cnt++;
	}

        #if DEBUG != 0
	fprintf(stderr,"jail: environment data: %s\n",env[n]);
	#endif

	n++;
    }	
    
    // call realpath so canonize the path, then
    // chdir & chroot to the  user's directory 
    // after chroot, read again the passwd file, 
    // if the two entries are the same, do nothing, else, chdir to the
    // new directory, and save this info in the new environment argument
    // array.
    // 
    // next, get the shell entry from the chrooted /etc/passwd file
    // and set it as shell variable
    //

    tmp_str = (char *)malloc(PATH_LEN);
    if (!realpath(pwdent->dir,tmp_str)) {
	fprintf(stderr,
		"jail: can't canonize path \"%s\". Bad path?\n",
		pwdent->dir);
	return(-2);
    }
    free(pwdent->dir);
    pwdent->dir = tmp_str;


    #if DEBUG != 0
     fprintf(stderr,"jail: doing chdir(%s)\n",pwdent->dir);
    #endif 

    ret = chdir(pwdent->dir);	
    if (ret) {
	perror("jail: chdir() ");
	return(-2);
    }

    #if DEBUG != 0
     fprintf(stderr,"jail: doing chroot(%s)\n",pwdent->dir);
    #endif 
	
    ret = chroot(pwdent->dir);
    if (ret) { 
	perror("jail: chroot() ");
	return(-2);
    }

    //
    // show entries in the chrooted environment passwd
    // some buggy operating systems have badly broken
    // getpwdent() (they don't work under the chrooted
    // environment, e.g. solaris). So use this check
    // for debug.
    //

    #if DEBUG != 0
    {
        FILE *f = NULL;
        char buff[LINE_SZ];
        f = fopen(PASSWD_FILE,"r");
	if (!f) {
	  fprintf(stderr,"jail: can't read chrooted /etc/passwd\n");
	  return(-2);
	}

        while (!feof(f)) {
         memset(buff,'\0',LINE_SZ);
         fgets(buff,LINE_SZ,f);
         fprintf(stderr,"jail: chrooted passwd entry: %s",buff);
        }
       fclose(f);
       fprintf(stderr,"\n");
    } 
    #endif

    cpwdent = getpasswddata(uid);

    if (cpwdent == NULL) {
	fprintf(stderr,
		"jail: chrooted directory %s is not configured"
		"for jail (bad passwd file); bailing out.\n",
		pwdent->dir);
	return(-2);
    }
    
    #if DEBUG != 0
    fprintf(stderr,"jail: original user's dir: %s\n",pwdent->dir);
    fprintf(stderr,"jail: chrooted user's dir: %s\n",cpwdent->dir);
    #endif

    //
    // check the original and chrooted user directories
    // and then canonize the path
    //

    if (strcmp(pwdent->dir,cpwdent->dir)) 
	ptr_home_dir = cpwdent->dir;
    else
	ptr_home_dir = pwdent->dir;

    tmp_str = (char *)malloc(PATH_LEN);
    if (!realpath(ptr_home_dir,tmp_str)) {
	fprintf(stderr,
		"jail: can't canonize path \"%s\". Bad path?\n",
		ptr_home_dir);
	return(-2);
    }
    free(ptr_home_dir);
    ptr_home_dir = tmp_str;

    

    //
    // insert the modified variables inside the environment array
    //

    n = strlen(HOME_VAR)+strlen(ptr_home_dir)+1;
    home_dir = (char *)malloc(n);
    memset(home_dir,'\0',n);
    strcpy(home_dir,HOME_VAR);
    strcat(home_dir,ptr_home_dir);

    n = strlen(SHELL_VAR)+strlen(cpwdent->shell)+1;
    shell_path = (char *)malloc(n);
    memset(shell_path,'\0',n);
    strcpy(shell_path,SHELL_VAR);
    strcat(shell_path,cpwdent->shell);

    init_env(&lenv[argc_cnt],home_dir); argc_cnt++;
    init_env(&lenv[argc_cnt],shell_path); argc_cnt++;	
    lenv[argc_cnt]=NULL;


    #if DEBUG != 0
    for (n=0; n<argc_cnt; n++) {
 	fprintf(stderr,"jail: modified environment data: %s\n",lenv[n]);
    }
    #endif

    //
    // if we have some args when called, pass them to the
    // chrooted shell
    //

    if (argc > 0) {
	n=0;
	while(argv[n]) n++;  
	n++;

	larg = malloc(sizeof(char *)*n);	

	n = 0 ;
	while (argv[n]) {
	    init_env(&larg[n],argv[n]);

	    #if DEBUG != 0
	    fprintf(stderr,"jail: argument data: %s\n",argv[n]);
	    #endif
	
            n++;
	}
	larg[n]=NULL;
    }
    else {
	larg = malloc(sizeof(char *)*4); 
	init_env(&larg[0],argv[0]);
	init_env(&larg[1],"-");
	larg[2]=NULL;

        #if DEBUG != 0
	fprintf(stderr,"jail: no argument data\n");
        #endif

    }
    
    //
    // Finally become the user, 
    // chdir to the user home directory,
    // clean all the structures and
    // call the shell overriding current process space
    //

    setgid(gid);
    setuid(uid);

    #if DEBUG != 0
    fprintf(stderr,"jail: doing chdir(%s)\n",ptr_home_dir);
    #endif
    
    ret = chdir(ptr_home_dir);	
    if (ret) {
	perror("jail: chdir() ");
	return(-2);
    }

    free(home_dir);
    free(shell_path);

    //
    // canonize the shell path
    // 

    tmp_str = (char *)malloc(PATH_LEN);
    if (!realpath(cpwdent->shell,tmp_str)) {
	fprintf(stderr,
		"jail: can't canonize path \"%s\". Bad path?\n",
		cpwdent->shell);
	return(-2);
    }
    free(cpwdent->shell);
    cpwdent->shell = tmp_str;

    #if DEBUG != 0
    fprintf(stderr,"jail: doing execve(%s)\n",cpwdent->shell);
    #endif

    ret = execve(cpwdent->shell,larg,lenv);
    if (ret) { 
	perror("jail: execve() ");
	return(-3);
    }

    //
    // not reached and if so, we are in trouble !
    //
    
    fprintf(stderr,
	    "jail: can't spawn shell. If you read this, you're in trouble.\n");
    return(-4); 
}

    

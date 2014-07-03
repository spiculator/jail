##############################################################################
#
# $Id: Makefile,v 1.1.1.1 2001/10/26 09:36:09 assman Exp $
#
# $Log: Makefile,v $
# Revision 1.1.1.1  2001/10/26 09:36:09  assman
# Added support for new platforms: FreeBSD, Solaris, IRIX. Now some options
# can be selected from the Makefile script: DEBUG on/off, install path,
# install permissions, etc. The perl scripts have been rewritten so they
# support platform-specific code, so port Jail to another platform should
# be an easy task.
#
# Revision 1.1.1.1  2001/08/15 15:50:06  assman
# Added to the production cvs
#
# Revision 1.2  2001/06/24 14:14:26  assman
# added cvs tags
#
#
##############################################################################

ARCH=__LINUX__
#ARCH=__FREEBSD__
#ARCH=__IRIX__
#ARCH=__SOLARIS__

DEBUG = 0
INSTALL_DIR = /usr/local
PERL = /usr/bin/perl
ROOTUSER = root
ROOTGROUP = root

# ----------------------------------------------------------------------
#
# Internal details. Safe leave them untouched
#
# ----------------------------------------------------------------------

VERSION = 1.9

CC = gcc
COPT = -Wall -g
DEFINE = -D$(ARCH) -DDEBUG=$(DEBUG)

INCLUDE = -I .
CFLAGS  = $(COPT) $(DEFINE)
#LIBS = -lefence

OBJ = generic_helpers.o passwd_helpers.o terminal_helpers.o

#
# rule section
#

.SUFFIXES:
.SUFFIXES: .o .c 
.c.o:	
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@ 

all:	jail

jail:	jail.c $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) jail.c -o jail $(OBJ) $(LIBS) && \
	cp jail ../bin

clean:
	@(rm -f jail $(OBJ))

mrproper:	clean
	@(rm -f core *~ ../bin/jail)

install:	jail
	@(./preinstall.sh $(VERSION) $(ARCH) $(DEBUG) $(INSTALL_DIR) \
            	          $(PERL) $(ROOTUSER) $(ROOTGROUP))

#!/bin/sh
##############################################################################
#
# preinstall.sh
# initial startup install file
#
# Juan M. Casillas <assman@gsyc.inf.uc3m.e>
#
# needs:
#
#	cat
#
# $Id: preinstall.sh,v 1.1.1.1 2001/10/26 09:36:09 assman Exp $
#
# $Log: preinstall.sh,v $
# Revision 1.1.1.1  2001/10/26 09:36:09  assman
# Added support for new platforms: FreeBSD, Solaris, IRIX. Now some options
# can be selected from the Makefile script: DEBUG on/off, install path,
# install permissions, etc. The perl scripts have been rewritten so they
# support platform-specific code, so port Jail to another platform should
# be an easy task.
#
#
##############################################################################

TMPFILE=/tmp/jail.tmp


# ----------------------------------------------------------------------------

if [ $# -ne 7 ]; then
  echo "Bad arguments. Are you calling $0 from the src/Makefile?"
  exit;
fi

VERSION=$1
ARCH=$2 
DEBUG=$3
INSTALLDIR=$4
PERLPATH=$5
ROOTID=$6
ROOTGID=$7

SHELLPATH=/bin/sh

case $ARCH in
  __SOLARIS__) SHELLPATH=/usr/bin/ksh
	       ;;
esac

#
# change the shellpath
#

file=../install.sh;
file2=install
tag=__SHELLPATH__;
value=$SHELLPATH;

value=`printf "%s\n" $value | sed -e "s/\\\//\\\\\\\\\//g"`

cat $file | sed -e "s/$tag/$value/" > $TMPFILE
mv $TMPFILE ../$file2

#
# exec it
#

chmod 755 ../$file2
(cd ..; ./$file2 $VERSION $ARCH $DEBUG $INSTALLDIR $PERLPATH $ROOTID $ROOTGID)
rm -f ../$file2

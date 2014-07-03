#!__SHELLPATH__

##############################################################################
#
# install.sh
# Install jail files in the system
# should be called by src/Makefile
#
# Juan M. Casillas <assman@gsyc.inf.uc3m.e>
#
# needs:
#
#	cat
#       echo -e 
#       mkdir
#       basename
#
# $Id: install.sh,v 1.2 2001/10/26 09:36:53 assman Exp $
#
# $Log: install.sh,v $
# Revision 1.2  2001/10/26 09:36:53  assman
#
#
##############################################################################

TMPFILE=/tmp/jail.tmp

# ----------------------------------------------------------------------------

install_file() {

filename=$1
fname=`basename $filename`
dest=$2
perm=$3
own=$4
grp=$5

mkdir -p $dest
cp $filename $dest
chown $own:$grp ${dest}/${fname}
chmod $perm ${dest}/${fname}

}

# ----------------------------------------------------------------------------

parse_tag() {
 
file=$1;
tag=$2;
value=$3;


value=`printf "%s\n" $value | sed -e "s/\\\//\\\\\\\\\//g"`

cat $file | sed -e "s/$tag/$value/" > $TMPFILE
mv $TMPFILE $file 

}

# ----------------------------------------------------------------------------

gen_jail_conf() {

cp etc/jail.conf /tmp/jail.conf.tmp

parse_tag /tmp/jail.conf.tmp __VERSION__    $1
parse_tag /tmp/jail.conf.tmp __ARCH__       $2
parse_tag /tmp/jail.conf.tmp __DEBUG__      $3
parse_tag /tmp/jail.conf.tmp __INSTALLDIR__ $4

mv /tmp/jail.conf.tmp /tmp/jail.conf
install_file /tmp/jail.conf $4/etc $5 $6 $7
rm /tmp/jail.conf

}

# ----------------------------------------------------------------------------

gen_libjail() {

cp lib/libjail.pm /tmp/libjail.pm.tmp

parse_tag /tmp/libjail.pm.tmp __INSTALLDIR__ $1

mv /tmp/libjail.pm.tmp /tmp/libjail.pm
install_file /tmp/libjail.pm $1/lib $2 $3 $4
rm /tmp/libjail.pm

}

# ----------------------------------------------------------------------------

gen_mkenv() {

cp bin/mkjailenv /tmp/mkjailenv.tmp

parse_tag /tmp/mkjailenv.tmp __INSTALLDIR__ $1
parse_tag /tmp/mkjailenv.tmp __PERL__ $2

mv /tmp/mkjailenv.tmp /tmp/mkjailenv
install_file /tmp/mkjailenv $1/bin $3 $4 $5
rm /tmp/mkjailenv

}

# ----------------------------------------------------------------------------

gen_addjailsw() {

cp bin/addjailsw /tmp/addjailsw.tmp

parse_tag /tmp/addjailsw.tmp __INSTALLDIR__ $1
parse_tag /tmp/addjailsw.tmp __PERL__ $2

mv /tmp/addjailsw.tmp /tmp/addjailsw
install_file /tmp/addjailsw $1/bin $3 $4 $5
rm /tmp/addjailsw

}


# ----------------------------------------------------------------------------

gen_addjailuser() {

cp bin/addjailuser /tmp/addjailuser.tmp

parse_tag /tmp/addjailuser.tmp __INSTALLDIR__ $1
parse_tag /tmp/addjailuser.tmp __PERL__ $2

mv /tmp/addjailuser.tmp /tmp/addjailuser
install_file /tmp/addjailuser $1/bin $3 $4 $5
rm /tmp/addjailuser

}

# ----------------------------------------------------------------------------

gen_arch() {

for i in generic linux freebsd irix solaris; do

install_file lib/arch/$i/definitions $1/lib/arch/$i $2 $3 $4
install_file lib/arch/$i/functions $1/lib/arch/$i $2 $3 $4

done

}


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

install_file ./bin/jail $INSTALLDIR/bin 4755 $ROOTID $ROOTGID
gen_jail_conf $VERSION $ARCH $DEBUG $INSTALLDIR 755 $ROOTID $ROOTGID
gen_libjail $INSTALLDIR 755 $ROOTID $ROOTGID  

gen_mkenv $INSTALLDIR $PERLPATH 755 $ROOTID $ROOTGID
gen_addjailsw $INSTALLDIR $PERLPATH 755 $ROOTID $ROOTGID
gen_addjailuser $INSTALLDIR $PERLPATH 755 $ROOTID $ROOTGID

gen_arch $INSTALLDIR 755 $ROOTID $ROOTGID

rm -f $TMPFILE

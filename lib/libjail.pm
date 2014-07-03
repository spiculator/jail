# -----------------------------------------------------------------------------
#
# $Id: libjail.pm,v 1.1.1.1 2001/10/26 09:36:09 assman Exp $
#
# $Log: libjail.pm,v $
# Revision 1.1.1.1  2001/10/26 09:36:09  assman
# Added support for new platforms: FreeBSD, Solaris, IRIX. Now some options
# can be selected from the Makefile script: DEBUG on/off, install path,
# install permissions, etc. The perl scripts have been rewritten so they
# support platform-specific code, so port Jail to another platform should
# be an easy task.
#
#
# -----------------------------------------------------------------------------

$JAIL_DIR= "__INSTALLDIR__/etc";
$CONFIG_FILE=$JAIL_DIR."/"."jail.conf";

# -----------------------------------------------------------------------------
#
# Initial bootstrap
# load the configuration file
#
# -----------------------------------------------------------------------------

if (-e $CONFIG_FILE) {
     require($CONFIG_FILE);
}
else  {
   die("can't load config file \"$CONFIG_FILE\".\n");
}

# -----------------------------------------------------------------------------
#
# load_file()
# a simple function for store the contents of a file in a string
#
# -----------------------------------------------------------------------------

sub load_file {
  local ($file) = @_;
  if (!-e $file) {
    $DEBUG && print("can't open file \"$file\" for read\n");
    return();
  }
  open(F,$file);
  local @elem = <F>;
  close(F);
  return(join("",@elem));
}

# -----------------------------------------------------------------------------
#
# initial_load()
# a function for locate the directory for a given arch
#
# -----------------------------------------------------------------------------

sub initial_load {
  local ($arch) = @_;
  
  local $found = 0;
  
  for $key (keys (%ARCH_DIR_LOOKUP)) {
    if ($key eq $arch) {
      $found = 1;
      last;
    }
  }
  if (!$found) {
    die("arch $arch doesn't exist.\n");
  }
  
  return($ARCH_DIR_LOOKUP{$arch});
}

# -----------------------------------------------------------------------------
#
# INITIALIZATION PHASE
# get the arch, and load the right files
# then eval them
#
# -----------------------------------------------------------------------------

$definitions = load_file($INSTALL_DIR."/lib/arch/generic/definitions");
$functions = load_file($INSTALL_DIR."/lib/arch/generic/functions");

$DEBUG && print("Definition file: $INSTALL_DIR/lib/arch/generic/definitions\n");
$DEBUG && print("Function file: $INSTALL_DIR/lib/arch/generic/functions\n");


if (!$definitions) {
 die("can't open definition file $definitions.\n");
}

if (!$functions) {
 die("can't open functions file \"$functions\".\n");
}

eval($definitions);
eval($functions);

undef $definitions;
undef $functions;

$ARCH_NAME = initial_load($ARCH);
$arch_dir = $INSTALL_DIR."/lib/arch/".$ARCH_NAME;

$DEBUG && print("Arch directory is: \"$arch_dir\".\n");

if (!-e $arch_dir) {
  die("arch directory \"$arch_dir\" doesn't exist.\n");
}

# -----------------------------------------------------------------------------
#
# load the specific arch files
#
# -----------------------------------------------------------------------------

$definitions = load_file($arch_dir."/definitions");
$functions = load_file($arch_dir."/functions");

if (!$definitions) {
  die("can't open definition file \"$definitions\".\n");
}

if (!$functions) {
  die("can't open functions file \"$functions\".\n");
}

eval($definitions);
eval($functions);

undef $definitions;
undef $functions;
undef $arch_dir;

1;

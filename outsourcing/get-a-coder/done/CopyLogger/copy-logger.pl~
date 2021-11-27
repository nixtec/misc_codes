#! /usr/bin/perl -w

use strict;
use warnings;
# rsh user and host information
my $rshUser = "mspds";
my $rshHost = "192.168.10.30";

# destination paths
my $baseDSTpath = "/archive";
my $DSThost = "mspds";


#my $infh = "";
my $fld_uid;
my $filepath;
my $filename;
my $fileyear;
my $filemonth;
my $fileday;

our ($sec, $min, $hour, $mday, $mon, $year) = localtime(time);

my $logfile = "./copy-logger_".($year+1900)."-".($mon+1)."-".$mday."_"."$hour-$min-$sec".".log";
my $infile = "./text_file.txt";



# -----------------------------------------
# open files
open (LOG, ">>$logfile") or die("Couldn't open $logfile for writing");
open (INPUT, "<$infile") || die("Couldn't open $infile for reading!");


# -----------------------------------------
# start loop
my @args = ();
#while (<$infh>)
while (<INPUT>)
{
  $fld_uid = $_;
  chomp $fld_uid;

  $fileyear = substr($fld_uid, 10, 4);
  $filemonth = substr($fld_uid, 14, 2);
  $fileday = substr($fld_uid, 16, 2);


  $filename = $baseDSTpath."/".$DSThost."/".$fileyear."/".$filemonth."/".$fileday."/".$fld_uid.".tar";

  @args = ("ls", "-l", "$filename");
  if (system(@args) == 0) {
    my $UserHostPath = $rshUser . '@' . $rshHost . ':' . $filename;
    my $rcp_output = `rcp -p "$filename" "$UserHostPath"`;
    my $command = "chmod 755 $filename";
    my $rsh_output = `rsh -l $rshUser $rshHost "$command"`;
  } else {
    print LOG "File $filename doesn't exist\n";
  }
}


# -----------------------------------------
# close files

#close($infh);
close(INPUT);
close(LOG);

# -----------------------------------------
# all done
exit(0);

#! /usr/bin/perl -w

# rsh user and host information
my $rshUser = "mspds";
my $rshHost = "192.168.10.30";

# destination paths
my $baseDSTpath = "/archive";
my $DSThost = "mspds";


my $infh = "";
my $fld_uid;
my $filepath;
my $filename;
my $fileyear;
my $filemonth;
my $fileday;

my $logfile = "./copy-logger.log";
my $infile = "./text_file.txt";


# -----------------------------------------
# open files
open (LOG, ">>$logfile") or die "Couldn't open $logfile for writing";
open ($infh, "<".$infile) || die "Couldn't open $infile for reading!";


# -----------------------------------------
# start loop
my @args = ();
while (<$infh>)
{
  $fld_uid = $_;
  chomp $fld_uid;

  $fileyear = substr($fld_uid, 10, 4);
  $filemonth =substr($fld_uid, 14, 2);
  $fileday = substr($fld_uid, 16, 2);


  $filename = $baseDSTpath."/".$DSThost."/".$fileyear."/".$filemonth."/".$fileday."/".$fld_uid.".tar";

  @args = ("ls", "-l", "$filename");
  if (system(@args) == 0) {
    $copy = qx{ rcp -p $filename $rshUser"@"$rshHost":"$filename };
    $chperm = qx{ rsh -l $rshUser $rshHost 'chmod 755 $filename' };
#    print LOG "$copy\n";
  } else {
    print LOG "File $filename doesn't exist\n";
  }
}


# -----------------------------------------
# close files

close($infh);
close(LOG);

# -----------------------------------------
# all done
exit(0);

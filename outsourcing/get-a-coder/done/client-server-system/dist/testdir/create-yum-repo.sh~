#!/bin/sh

# a test attempt to configure yum for io_pigio in his system :D
# wish this will work
# unfortunately there's SL-10.1 folder but no SL-10.2, so attempting to use
# SL-OSS-factory [do it at your own risk]

DIR="/etc/yum.repos.d"
FILE="${DIR}/suse-base-ayub.repo"

mkdir -p "$DIR"
if [ $? -ne 0 ] ; then
  exit $?
fi

echo "
[SuSE Base Reposotory]
name=SUSE Linux \$releaseva - \$basearch
baseurl = http://download.opensuse.org/distribution/SL-OSS-factory/inst-source/suse
#baseurl = http://download.opensuse.org/distribution/SL-\$releasevar/inst-source/suse
enabled = 1
gpgcheck = 1
gpgkey = http://download.opensuse.org/distribution/SL-OSS-factory/inst-source/pubring.gpg
#gpgkey = http://download.opensuse.org/distribution/SL-\$releasever/inst-source/pubring.gpg" > "$FILE"

if [ $? -eq 0 ] ; then
  echo "Yum repository created to $FILE"
else
  echo "Failed to create repository"
fi

exit 0

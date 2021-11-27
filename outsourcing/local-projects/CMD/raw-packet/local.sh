#!/bin/sh
#
# local.sh
# automatically generated by 'make rcscripts'. DO NOT edit
#
# environment variables starts here
# environment variables
IFACE="eth"
RUNUSR="ayub"
NETWORK="192.168.0.0"
MASK="24"
BASEDIR="/var/dse"
VARDIR="/var/dse"
BINDIR="/var/dse/bin"
RCDIR="/var/dse/rc"
CFGDIR="/var/dse/cfg"
LOGDIR="/var/dse/log"
IFCFG="/var/dse/cfg/iface.cfg"
ADDRCFG="/var/dse/cfg/addrs.cfg"
PORTCFG="/var/dse/cfg/ports.cfg"
USRCFG="/var/dse/cfg/user.cfg"
SUBNET="192.168.0.0/24"
BINDADDR="192.168.0.255"
BINDCFG="/var/dse/cfg/bind.cfg"
CONFIG_H="config.h"
ADDRS="0"
PORTS="0"
EXENAME="forwarder"
EXEPATH="/var/dse/bin/forwarder"
EXECFG="/var/dse/cfg/exe.cfg"
INITSCRIPT="/etc/init.d/forwarder"
EXT=".sh"
FORWARDER_RC="forwarder.sh"
UPDIFLIST_RC="update-iflist.sh"
IPUP_RC="ip-up.sh"
IPDOWN_RC="ip-down.sh"
LOCAL_RC="local.sh"
FIREWALL_RC="firewall.sh"
BINFILES="forwarder"
RCFILES="forwarder.sh update-iflist.sh ip-up.sh ip-down.sh local.sh firewall.sh"
TARGETS="forwarder forwarder.debug"
OTHERS=""
ENVFILE="env.source"
LOCKFILE="/var/lock/subsys/forwarder"
# environment variables ends here

# set up a strict firewall
/var/dse/rc/firewall.sh

# there must be a user named $USR [able to login to system]
USR=`cat $USRCFG`

#chown $USR $BASEDIR
chown $USR $LOGDIR
#chmod 644 $LOGDIR/*.log
# remove old log files
rm -f $LOGDIR/*.log
su $USR -c '/var/dse/rc/forwarder.sh start'
touch $LOCKFILE
# clear previous log information
[ -f $LOGDIR/login.log ] && echo "" > $LOGDIR/login.log

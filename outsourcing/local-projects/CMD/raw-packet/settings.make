#
# settings.make
# variables and other settings for Makefile
#
# base directory for forwarder
BASEDIR		:= /var/dse
VARDIR		:= $(BASEDIR)
BINDIR		:= $(VARDIR)/bin
RCDIR		:= $(VARDIR)/rc
CFGDIR		:= $(VARDIR)/cfg
LOGDIR		:= $(VARDIR)/log
# needed by update-iflist.sh
IFCFG		:= $(CFGDIR)/iface.cfg

# runtime forward address information
ADDRCFG		:= $(CFGDIR)/addrs.cfg
PORTCFG		:= $(CFGDIR)/ports.cfg
# which user the program should run as '/etc/rc.local' will use it
USRCFG		:= $(CFGDIR)/user.cfg


SUBNET		:= $(NETWORK)/$(MASK)
# which address should the program bind to [broadcast address of the subnet]
BINDADDR	:= $(shell ipcalc -b $(SUBNET) | cut -d '=' -f 2)
BINDCFG		:= $(CFGDIR)/bind.cfg

CONFIG_H	:= config.h

#
# information about executable file
EXENAME		:= forwarder
EXEPATH		:= $(BINDIR)/$(EXENAME)
EXECFG		:= $(CFGDIR)/exe.cfg

# /etc/init.d/ entry
INITSCRIPT	:= /etc/init.d/$(EXENAME)

EXT		:= .sh
FORWARDER_RC	:= forwarder$(EXT)
UPDIFLIST_RC	:= update-iflist$(EXT)
IPUP_RC		:= ip-up$(EXT)
IPDOWN_RC	:= ip-down$(EXT)
LOCAL_RC	:= local$(EXT)
FIREWALL_RC	:= firewall$(EXT)

# files to be installed in $(BINDIR)
BINFILES	:= $(EXENAME)
# files to be installed in $(VARDIR)
RCFILES		:= $(strip $(FORWARDER_RC) $(UPDIFLIST_RC) \
			$(IPUP_RC) $(IPDOWN_RC) \
			$(LOCAL_RC) $(FIREWALL_RC))


# build targets
TARGETS		:= forwarder forwarder.debug
OTHERS		:=

ENVFILE		:= env.source
LOCKFILE	:= /var/lock/subsys/$(EXENAME)

# NOTE: if you introduce/modify new variables, update the 'writenv' target
# in scripts.make


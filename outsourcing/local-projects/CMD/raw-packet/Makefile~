#
# Makefile for forwarder [ip-helper simulation of cisco]
# You need to change only inside 'CUSTOMIZATION {START|END}' sections
# __NODELAY__ option sets the ip_tos field to IPTOS_LOWDELAY
#
CC		:= gcc
EXTRAFLAGS	:=
CFLAGS		:= -Wall -O3 $(EXTRAFLAGS)
DCFLAGS		:= -g -Wall -D__DEBUG__ $(EXTRAFLAGS)

# load customized settings
include custom.make
# load settings variables
include settings.make

all: $(TARGETS) $(OTHERS) $(RCFILES)

.PHONY: clean install uninstall config

# all global environment variables should be defined above this line
include scripts.make

config:
	@echo -n "Creating $(CONFIG_H) ...";
	@echo "/* generated automatically by 'make config', DONT edit */" > \
	$(CONFIG_H);
	@echo "#ifndef _CONFIG_H_" >> $(CONFIG_H);
	@echo "#define _CONFIG_H_" >> $(CONFIG_H);
	@echo "" >> $(CONFIG_H);
	@echo "/* location of address and port information */" >> $(CONFIG_H);
	@echo "#define ADDRLIST \"$(ADDRCFG)\"" >> $(CONFIG_H);
	@echo "#define PORTLIST \"$(PORTCFG)\"" >> $(CONFIG_H);
	@echo "" >> $(CONFIG_H);
	@echo "#endif /* !_CONFIG_H_ */" >> $(CONFIG_H);
	@echo "DONE";

forwarder: forwarder.c common.h config.h
	$(CC) $(CFLAGS) -o forwarder forwarder.c;
	strip forwarder

forwarder.debug: forwarder.c common.h config.h
	$(CC) $(DCFLAGS) -o forwarder.debug forwarder.c;
	sudo chown root $@;
	sudo chmod u+s $@;

# NOTE: $(RUNUSER) user must exist
# you must be 'root' to run the following target
# if $(RUNUSR) does not exist, install will fail. creat user manually first
# also 'forwarder' is being 'setuid' root [however it will call setuid()] later
install: forwarder
	@echo "Checking if $(RUNUSR) exists...";
	id -un $(RUNUSR);
	mkdir -p $(BASEDIR);
	mkdir -p $(VARDIR);
	mkdir -p $(BINDIR);
	mkdir -p $(CFGDIR);
	mkdir -p $(RCDIR);
	mkdir -p $(LOGDIR);
	install -m 755 $(BINFILES) $(BINDIR);
	chmod u+s $(EXEPATH);
	install -m 755 $(RCFILES) $(RCDIR);
	-ln -s $(RCDIR)/$(FORWARDER_RC) $(INITSCRIPT);
	echo $(RUNUSR) > $(USRCFG);
	chmod 644 $(USRCFG);
	echo $(BINDADDR) > $(BINDCFG);
	chmod 644 $(BINDCFG);
	touch $(ADDRCFG);
	chmod 644 $(ADDRCFG);
	touch $(PORTCFG);
	chmod 644 $(PORTCFG);
	echo $(EXEPATH) > $(EXECFG);
	chmod 644 $(EXECFG);
	echo $(IFACE) > $(IFCFG);
	install -m 644 $(ENVFILE) $(CFGDIR);
	chown -R $(RUNUSR) $(CFGDIR) $(LOGDIR);
	rm -f $(VARDIR)/uninstall.log;
	echo "INSTALLED at `date`" > $(VARDIR)/install.log;
	@echo -n "Performing post-install configuration ...";
	@ENVFILE=$(ENVFILE) ./postconf.sh install;
	@echo "DONE";
	@echo "";
	@echo "^^^^^ INSTALL COMPLETE ^^^^^";
	@echo "";

# i am not removing the '/var/dse' directory because it may contain other
# necessary files
# you must be 'root' to run the following target
uninstall:
	$(RCDIR)/$(FORWARDER_RC) stop;
	rm -f $(addprefix $(BINDIR)/,$(BINFILES));
	rm -f $(INITSCRIPT);
	rm -f $(addprefix $(RCDIR)/,$(RCFILES));
	rm -f $(VARDIR)/install.log;
	echo "UNINSTALLED at `date`" > $(VARDIR)/uninstall.log;
	@echo -n "Performing post-uninstall tasks ...";
	@ENVFILE=$(ENVFILE) ./postconf.sh uninstall;
	@echo "DONE";
	@echo "";
	@echo "^^^^^ UNINSTALL COMPLETE ^^^^^";
	@echo "";

clean:
	rm -f *.o $(TARGETS) $(OTHERS) $(RCFILES)

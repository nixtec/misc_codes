#
# custom.make
# customized settings for Makefile
#
# CUSTOMIZATION START
#
# pattern to be matched for updating list
IFACE		:= eth
# the program will be run as the following user for safety
RUNUSR		:= ayub
# network information
NETWORK		:= 192.168.0.0
MASK		:= 24
#
# by default, no {address,port} will be forwarded
#
ADDRS		:= 0
PORTS		:= 0
#IFACE		:= ppp
#RUNUSR		:= forward
#NETWORK		:= 150.1.93.0
#MASK		:= 27
#
# CUSTOMIZATION END
#


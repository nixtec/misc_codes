#!/bin/sh

# firewall.sh
# firewall script for DSE [subnet:192.168.0.0/24]
# Written by Ayub <mrayub@gmail.com>

# follow strict path
PATH="/bin:/sbin:/usr/bin:/usr/sbin"

SUBNET="192.168.0.0/24"
LOADDR="127.0.0.0/8"

# remove all existing rules belonging to this filter (flush)
iptables -F
iptables -F -t nat

# remove any existing user-defined chains
iptables -X

# set the default policy of the filter to deny
iptables -P INPUT DROP
iptables -P OUTPUT DROP
iptables -P FORWARD DROP

#iptables -A INPUT -i lo -j ACCEPT
#iptables -A OUTPUT -o lo -j ACCEPT
# unlimited traffic on the loopback interface
iptables -A INPUT -s $LOADDR -d $LOADDR -j ACCEPT
iptables -A OUTPUT -s $LOADDR -d $LOADDR -j ACCEPT

# we consider only our network. No packets will go outside the sub-network
iptables -A INPUT -s $SUBNET -d $SUBNET -j ACCEPT
iptables -A OUTPUT -s $SUBNET -d $SUBNET -j ACCEPT
iptables -A FORWARD -s $SUBNET -d $SUBNET -j ACCEPT

#iptables -A INPUT -d 150.1.255.255 -j ACCEPT

# do DNAT if you need to send packets destined to 6000 port to 7860
# so applications need not handle this issue
#DNAT_ADDRS="150.1.93.18 150.1.93.20"
#ORIG_PORT=6000
#DNAT_PORT=7860
#for addr in $DNAT_ADDRS ; do
#  iptables -t nat -A OUTPUT -j DNAT -p udp -d $addr \
#    --destination-port $ORIG_PORT --to ${addr}:${DNAT_PORT}
#done


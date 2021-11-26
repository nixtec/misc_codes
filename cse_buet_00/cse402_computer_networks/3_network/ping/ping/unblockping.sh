#!/bin/sh

# unblockping
# unblocks icmp_echo ping requests

[ $UID -ne 0 ] && echo "Must be root" && exit 1

pingecho="/proc/sys/net/ipv4/icmp_echo_ignore_all"
pingbroad="/proc/sys/net/ipv4/icmp_echo_ignore_broadcasts"

# now discard the ping requests
echo 0 >"$pingecho" && exit 0 || exit 1

exit $?

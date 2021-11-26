Packets may be lost for asynchronous events.
It may happen that datalink thread is busy with processing data and at that
time another event raised, but datalink layer will miss it.

I have used usleep() somewhere in the code. This is to reduce possibility of
missing events for datalink layer. So each event raising threads will wait
some time before raise_event.


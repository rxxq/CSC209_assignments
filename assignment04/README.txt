c4xiaxiu
998269449
Xiuqi Xia

g2wgiang
996824977
Wilfred Giang
grace days used: 3

Notes:
filedata.h and filedata.c are not the same as the starter code.
They have been heavily modified

There is a problem with files being re-sync'ed back and forth due
to small differences in the time a file is written on a client
and the time the file gets written on the server (and vice versa).
This can be fixed by using utime to change the last mod time of a file
to match the time sent in the sync_message, but we did not have time
to implement this fix.

Server does not send to the client files that are not on the client
(did not have time to implement)

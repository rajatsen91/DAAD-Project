^\x01\x01\x05\x0a
user-agent:aim/
^ajprot\x0d\x0a
YCLC_E|CYEL
^\x01\x11\x10\|\xf8\x02\x10\x40\x06
^[a-z][a-z0-9]+@[1-9][0-9]+$
^(\x13bittorrent protocol|azver\x01$|get /scrape\?info_hash=get /announce\?info_hash=|get /client/bitcomet/|GET /data\?fid=)|d1:ad2:id20:|\x08'7P\)[RP]
^CTPv1\.[123] Kamusta.*\x0d\x0a$
\x02[0-4][0-9]:[0-9]+.*\x03$
^\x01\xf4\x01\xf4
\x32\x26\x85\x92\x58
^\xff\xff\xff\xff.*cstrikeCounter-Strike
^BEGIN (AUTH|VERIFICATION|GSSAPI) REQUEST\x0a
^\xff\xff\xff\xff.*dodDay of Defeat
^(longaccoun|qsver2auth|\x35[57]\x30|\+\x10\*)
^[\x01\x02][\x01- ]\x06.*c\x82sc
^(\$mynick |\$lock |\$key )
^\xff\xffchallenge
^get (/.download/[ -~]*|/.supernode[ -~]|/.status[ -~]|/.network[ -~]*|/.files|/.hash=[0-9a-f]*/[ -~]*) http/1.1|user-agent: kazaa|x-kazaa(-username|-network|-ip|-supernodeip|-xferid|-xferuid|tag)|^give [0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]?[0-9]?[0-9]?
^[a-z][a-z0-9\-_]+|login: [\x09-\x0d -~]* name: [\x09-\x0d -~]* Directory:
^\x01[\x08\x09][\x03\x04]
^220[\x09-\x0d -~]*ftp
^gkrellm [23].[0-9].[0-9]\x0a$
gnuclear connect/[\x09-\x0d -~]*user-agent: gnucleus [\x09-\x0d -~]*lan:
<peerplat>|^get /getfilebyhash\.cgi\?|^get /queue_register\.cgi\?|^get /getupdowninfo\.cgi\?
^[\x04\x05]\x0c.i\x01
^\xff\xff\xff\xff.*hl2mpDeathmatch
^\|/dev/[a-z][a-z][a-z]\|[0-9a-z]*\|[0-9][0-9]\|[cfk]\|
^(get[\x09-\x0d -~]* Accept: application/x-rtsp-tunnelled|http/(0\.9|1\.0|1\.1) [1-5][0-9][0-9] [\x09-\x0d -~]*a=control:rtsp://)
http/(0\.9|1\.0|1\.1) [1-5][0-9][0-9] [\x09-\x0d -~]*(connection:|content-type:|content-length:|date:)|post [\x09-\x0d -~]* http/[01]\.[019]
^[1-9][0-9]?[0-9]?[0-9]?[0-9]?[\x09-\x0d]*,[\x09-\x0d]*[1-9][0-9]?[0-9]?[0-9]?[0-9]?(\x0d\x0a|[\x0d\x0a])?$
^(\* ok|a[0-9]+ noop)
 imsh\.download-prod\.musicnet\.com|\x02[\x01\x02]\x83.*\x02[\x01\x02]\x83)
ipp://
^(nick[\x09-\x0d -~]*user[\x09-\x0d -~]*:|user[\x09-\x0d -~]*:[\x02-\x0d -~]*nick[\x09-\x0d -~]*\x0d\x0a)
<stream:stream[\x09-\x0d ][ -~]*[\x09-\x0d ]xmlns=['"]jabber
membername.*session.*player
^\xff\xff\xff\xffgetstatus\x0a
^(ver [ -~]*msnftp\x0d\x0aver msnftp\x0d\x0ausr|method msnmsgr:)
ver [0-9]+ msnp[1-9][0-9]? [\x09-\x0d -~]*cvr0\x0d\x0a$|usr 1 [!-~]+ [0-9. ]+\x0d\x0a$|ans 1 [!-~]+ [0-9. ]+\x0d\x0a$
^(Public|AES)Key: [0-9a-f]*\x0aEnd(Public|AES)Key\x0a$
^(.[\x02\x06][!-~]+ [!-~]+ [0-9][0-9]?[0-9]?[0-9]?[0-9]? "[\x09-\x0d -~]+" ([0-9]|10)|1(send|get)[!-~]+ "[\x09-\x0d -~]+")
\x01\x10\x01|\)\x10\x01\x01|0\x10\x01
^(dmdt.*\x01.*(""|\x11\x11|uu)|tncp.*33)
^(20[01][\x09-\x0d -~]*AUTHINFO USER|20[01][\x09-\x0d -~]*news)
x-openftalias: [-)(0-9a-z ~.]
^(nq|st)$
^(\+ok |-err )
^\xff\xff\xff\xffget(info|challenge)
^\x80\x0c\x01quake\x03
^\x01\x01(\x08\x08|\x1b\x1b)$
rdpdr.*cliprdr.*rdpsnd
^(get /ivs-IVSGetFileChunk|http/(0\.9|1\.0|1\.1) [1-5][0-9][0-9] [\x09-\x0d -~]*\x23\x23\x23\x23\x23REPLAY_CHUNK_START\x23\x23\x23\x23\x23)
^[a-z][a-z0-9][a-z0-9]+/[1-9][0-9]?[0-9]?[0-9]?00
rtsp/1.0 200 ok
^(invite|register|cancel|message|subscribe|notify) sip[\x09-\x0d -~]*sip/[0-2]\.[0-9]
\xffsmb[\x72\x25]
^220[\x09-\x0d -~]* (e?smtp|simple mail)
^notify[\x09-\x0d ]\*[\x09-\x0d ]http/1\.1[\x09-\x0d -~]*ssdp:(alive|byebye)|^m-search[\x09-\x0d ]\*[\x09-\x0d ]http/1\.1[\x09-\x0d -~]*ssdp:discover
^ssh-[12]\.[0-9]
^\xf4\xbe\x03.*teamspeak
^\xff[\xfb-\xfe].\xff[\xfb-\xfe].\xff[\xfb-\xfe]
\x03\x9a\x89\x22\x31\x31\x31\.\x30\x30\x20\x42\x65\x74\x61\x20|\xe2\x3c\x69\x1e\x1c\xe9
^(\x01|\x02)[ -~]*(netascii|octet|mail)
^t\x03ni.?[\x01-\x06]?t[\x01-\x05]s[\x0a\x0b](glob|who are you$|query data)
^(GET /docookie\.php\?uname=|\xfd\xfd\xfd\xfd\x30\x30\x30\x30\x30)
TOR1.*<identity>
^\x10here=
^rfb 00[1-9]\.00[0-9]\x0a$
^[ !-~]+\x0d\x0a$
^\x06\xec\x01
^[lb].?\x0b
^\x1b\xd7\x3b\x48[\x01\x02]\x01?\x01

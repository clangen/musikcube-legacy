You need the mpeg4ip library to compile this. I would include it, but it's 32 freakin' meg. The only bit of it actually being used is the mp4v2 library portion, though, so it compiles small.

Get it using CVS from:

:pserver:anonymous@cvs.sourceforge.net:/cvsroot/mpeg4ip

Repository: mpeg4ip




The projects are probably hardcoded. My directory structure looks like this:

c:\code\musik
- core_m4a
-- mpeg4ip
- musikCube
- musikCore
- etc, etc...





Also, the BASS_AAC.lib file is no longer needed, the BASS_AAC.DLL is loaded in at runtime instead.



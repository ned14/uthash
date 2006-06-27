all: userguide manpage 

userguide:	txt/userguide.txt
	asciidoc --unsafe --out-file=html/userguide.html txt/userguide.txt 

manpage: txt/uthash.3.txt
	asciidoc --unsafe -d manpage --out-file=html/uthash.3.html txt/uthash.3.txt
	asciidoc -d manpage -b docbook --out-file=/tmp/uthash.3.xml txt/uthash.3.txt
	xmlto -o man man /tmp/uthash.3.xml 

docbook: txt/userguide.txt
	asciidoc -b docbook --out-file=/tmp/userguide.xml txt/userguide.txt 
	xmlto -o html html-nochunks /tmp/userguide.xml 

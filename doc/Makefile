all: css userguide manpage 

userguide:	txt/userguide.txt
	asciidoc --unsafe --out-file=html/userguide.html -a linkcss=1 -a theme=tdh txt/userguide.txt 

css: html/toc.css
	cat /etc/asciidoc/stylesheets/xhtml11.css html/toc.css > html/tdh.css
	cp /etc/asciidoc/stylesheets/xhtml11-quirks.css html/tdh-quirks.css 

manpage: txt/uthash.3.txt
	asciidoc --unsafe -d manpage --out-file=html/uthash.3.html txt/uthash.3.txt
	asciidoc -d manpage -b docbook --out-file=/tmp/uthash.3.xml txt/uthash.3.txt
	xmlto -o man man /tmp/uthash.3.xml 

docbook: txt/userguide.txt
	asciidoc -b docbook --out-file=/tmp/userguide.xml txt/userguide.txt 
	xmlto -o html html-nochunks /tmp/userguide.xml 

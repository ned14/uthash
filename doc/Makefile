PREFIX=/opt/local
all: css userguide 

userguide:	txt/userguide.txt
	asciidoc --unsafe --out-file=html/userguide.html -a linkcss=1 -a theme=tdh txt/userguide.txt 

css: html/toc.css
	cat $(PREFIX)/etc/asciidoc/stylesheets/xhtml11.css html/toc.css > html/tdh.css
	cp $(PREFIX)/etc/asciidoc/stylesheets/xhtml11-quirks.css html/tdh-quirks.css 

docbook: txt/userguide.txt
	asciidoc -b docbook --out-file=/tmp/userguide.xml txt/userguide.txt 
	xmlto -o html html-nochunks /tmp/userguide.xml 

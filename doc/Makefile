PREFIX=/opt/local
all: css userguide pdf

userguide:	txt/userguide.txt
	asciidoc --unsafe --out-file=html/userguide.html -a linkcss=1 -a theme=tdh $<

.PHONY: pdf

pdf: txt/userguide.txt
	a2x -f pdf --destination-dir=pdf $<

css: html/toc.css
	cat $(PREFIX)/etc/asciidoc/stylesheets/xhtml11.css $< > html/tdh.css
	cp $(PREFIX)/etc/asciidoc/stylesheets/xhtml11-quirks.css html/tdh-quirks.css 

docbook: txt/userguide.txt
	asciidoc -b docbook --out-file=/tmp/userguide.xml $<
	xmlto -o html html-nochunks /tmp/userguide.xml 

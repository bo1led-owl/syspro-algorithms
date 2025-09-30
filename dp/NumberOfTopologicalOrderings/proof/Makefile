SOURCES = $(wildcard ./*.tex)
OUTDIR = ./build
RESULT = $(OUTDIR)/main.pdf

$(RESULT): $(OUTDIR) $(SOURCES)
	pdflatex --output-directory=$(OUTDIR) main.tex

$(OUTDIR):
	mkdir $(OUTDIR)

.PHONY: open

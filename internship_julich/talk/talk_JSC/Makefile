LATEX_COMPILER		= pdflatex
LATEX_BIB_LINKER	= bibtex

PATH_SLIDES		= slides/
PATH_SPEECH		= speech/

SRC_DIR			= ./

OBJ_FILE_EXTENSION	= .aux .log .out .toc .bbl .blg .vrb .snm .nav .lof
OBJ_FILE_DIR		= ./
OUTPUT_FILE_SLIDES	= $(PATH_SLIDES)mainSlides.pdf
OUTPUT_FILE_SPEECH	= $(PATH_SPEECH)mainSpeech.pdf

all			: compileAndPrintSlides compileAndPrintSpeech





#-----------------------------------------------------------------------------------------------------------
# --------------------------------------- Print results ----------------------------------------------------
#-----------------------------------------------------------------------------------------------------------
printSlides		:
			gnome-open $(OUTPUT_FILE_SLIDES)


printSpeech		:
			gnome-open $(OUTPUT_FILE_SPEECH)


compileAndPrintSlides	: compileSlides printSlides


compileAndPrintSpeech	: compileSpeech printSpeech


#-----------------------------------------------------------------------------------------------------------
# --------------------------------------- Compile ----------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------
compileSlides		: cleanSlides
			$(call compile, $(PATH_SLIDES))


compileSpeech		: cleanSpeech
			$(call compile, $(PATH_SPEECH))


#-----------------------------------------------------------------------------------------------------------
#---------------------------------------- General Methodes -------------------------------------------------
#-----------------------------------------------------------------------------------------------------------
.PHONY			: cleanSlides mrproper

cleanSlides		:
			$(call clean, $(PATH_SLIDES))


cleanSpeech		:
			$(call clean, $(PATH_SPEECH))


clean			: cleanSlides cleanSpeech


mrproper		: cleanSlides cleanSpeech
			rm $(OUTPUT_FILE_SLIDES) $(OUTPUT_FILE_SPEECH)


define clean
			cd $(1);\
			for directory in $(OBJ_FILE_DIR); do\
				for extension in $(OBJ_FILE_EXTENSION); do\
					rm -f $$directory*$$extension;\
				done\
			done
endef

define compile
			cd $(1);\
			$(LATEX_COMPILER) $(SRC_DIR)*.tex;\
			$(LATEX_BIB_LINKER) *.aux; \
			$(LATEX_COMPILER) $(SRC_DIR)*.tex;\
			$(LATEX_COMPILER) $(SRC_DIR)*.tex
endef


LATEX_COMPILER		= pdflatex
LATEX_BIB_LINKER	= bibtex

SRC_DIR			= ./
CHAPTER_DIR		= chapters/

OBJ_FILE_EXTENSION	= .aux .log .out .toc .bbl .blg .lof
OBJ_FILE_DIR		= ./ $(CHAPTER_DIR)
OUTPUT_FILE		= main.pdf

all			: compileAndPrint





#-----------------------------------------------------------------------------------------------------------
# --------------------------------------- Print results ----------------------------------------------------
#-----------------------------------------------------------------------------------------------------------
print			:
			gnome-open $(OUTPUT_FILE)


compileAndPrint		: compile
			gnome-open $(OUTPUT_FILE)


#-----------------------------------------------------------------------------------------------------------
# --------------------------------------- Compile ----------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------
compile			: 
			$(LATEX_COMPILER) $(SRC_DIR)*.tex $(CHAPTER_DIR)*.tex;\
			$(LATEX_BIB_LINKER) *.aux; \
			$(LATEX_COMPILER) $(SRC_DIR)*.tex $(CHAPTER_DIR)*.tex;\
			$(LATEX_COMPILER) $(SRC_DIR)*.tex $(CHAPTER_DIR)*.tex


#-----------------------------------------------------------------------------------------------------------
#---------------------------------------- General Methodes -------------------------------------------------
#-----------------------------------------------------------------------------------------------------------
.PHONY			: clean mrproper

clean			:
			for directory in $(OBJ_FILE_DIR); do\
				for extension in $(OBJ_FILE_EXTENSION); do\
					rm -f $$directory*$$extension;\
				done\
			done


mrproper		: clean
			rm $(OUTPUT_FILE)

CC     	= gcc
EXEC   	= extractNumaArchitecture
CFLAGS 	= -Wall -Werror -g -lnuma
srcDir	= src/
binDir	= bin/
all   	: $(EXEC)










#-----------------------------------------------------------------------------------------------------------
# ----------------------------------------------- Executable -----------------------------------------------
#-----------------------------------------------------------------------------------------------------------
extractNumaArchitecture:		$(binDir)extractNumaArchitecture.o 
								$(CC) $(CFLAGS) -o $@ $^





#-----------------------------------------------------------------------------------------------------------
# ----------------------------------------------- Modules  -------------------------------------------------
#-----------------------------------------------------------------------------------------------------------
$(binDir)%.o:		$(srcDir)%.c
						$(CC) $(CFLAGS) -c -o $@ $^

$(binDir)%.o:		$(srcDir)%.cc
						$(CC) $(CFLAGS) -c -o $@ $^


#-----------------------------------------------------------------------------------------------------------
# ------------------------------------------ General Methods -----------------------------------------------
#-----------------------------------------------------------------------------------------------------------
.PHONY	: clean mrproper
clean:
	rm -f $(binDir)*.o
mrproper: clean
	rm -rf $(EXEC)
# Rates coherence
# 
# - a constant T is defined in the lustre progrram (in seconds)
# - a sampling "period" is defined in the tcl program (in milliseconds)
# 
# THESE VALUES SHOULD BE COHERENT,
# The smallest is the period, the smoother is the simulation,
# T=0.05, period=50 (20 Hz)
# T=0.02, period=20 (50Hz)

all: fake_pendulum.lus
	lus2ec fake_pendulum.lus game
	wish p.tcl

clean:
	rm -f *.ec

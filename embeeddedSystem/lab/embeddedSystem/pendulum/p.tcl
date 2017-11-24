#
# RUN: wish p.tcl
# -> the file "game.ec" must be in the current directory
# (see Makefile)
#

load $::env(LUSTRE_INSTALL)/tcl/ecwish.so

set Size(width) 800
set Size(height) 600
set Size(unit) 50
set Size(xcenter) [expr $Size(width) / 2]
set Size(ycenter) [expr $Size(height) / 2]
set Size(ruler) 4
#
# correspondances X,Y lustre -> x,y tk
#
proc lus2tk_x { X } {
	global Size
	return [expr $Size(unit) * $X + $Size(xcenter)]
}
proc lus2tk_y { Y } {
	global Size
	return [expr -$Size(unit) * $Y - $Size(ycenter)]
}
#
# correspondances x,y tk  -> X,Y lustre
#
proc tk2lus_x { x } {
	global Size
	return [expr ($x - $Size(xcenter)) / $Size(unit)]
}
proc tk2lus_y { y } {
	global Size
	return [expr -($y + $Size(ycenter)) / $Size(unit)]
}

canvas .c -width $Size(width) -height $Size(height) -bg white

pack .c

set Hand(rad) 6 
set Hand(dia) [expr 2 * $Hand(rad)]
set Hand(xc) $Size(xcenter) 
set Hand(yc) $Size(ycenter)

# la periode est en milliseconde
#set Ruler(period) 100
#set Ruler(period) 50
set Ruler(period) 20

set Ruler(l) [expr $Size(ruler) * $Size(unit)] 
set Ruler(x) $Hand(xc) 
set Ruler(y) [expr $Hand(yc) - $Ruler(l)] 

set Hand(item) [.c create oval [expr $Hand(xc)-6] [expr $Hand(yc)-6] \
	[expr $Hand(xc)+6] [expr $Hand(yc)+6] -width 1 -outline black \
	-fill SkyBlue2 \
]
.c addtag dragable withtag $Hand(item)

set Ruler(item) [.c create line $Hand(xc) $Hand(yc) $Ruler(x) $Ruler(y) \
	-width 4  -fill red]
.c lower $Ruler(item) $Hand(item)

.c bind dragable <Any-Enter> ".c itemconfig current -fill red"
.c bind dragable <Any-Leave> ".c itemconfig current -fill SkyBlue2"
.c bind dragable <1> "itemStartDrag .c %x %y" 
.c bind dragable <B1-ButtonRelease> "itemStopDrag .c %x %y" 
#bind .c <B1-Motion> "itemDrag .c %x %y"
.c bind dragable <B1-Motion> "itemDrag .c %x %y"

proc itemStartDrag {c x y} {
    global Hand Ruler
    set Hand(lastX) [$c canvasx $x]
    set Hand(lastY) [$c canvasy $y]
    set Ruler(timer) [add_timer $Ruler(period) updateRulerPeriodic]
#puts "itemStartDrag Ruler(timer) = $Ruler(timer)"
}

proc itemStopDrag {c x y} {
    global Hand Ruler
#puts "itemStopDrag Ruler(timer) = $Ruler(timer)"
    catch [ abort_timer $Ruler(timer)	]
}

proc itemDrag {c x y} {
    global Hand Ruler
#puts "($Hand(xc), $Hand(yc)) -> ($x,$y)"
    set x [$c canvasx $x]
    set y [$c canvasy $y]
    set dx [expr $x-$Hand(lastX)]
    set dy [expr $y-$Hand(lastY)]
    $c move current $dx $dy
    set Hand(lastX) $x
    set Hand(lastY) $y
    set coo [.c coords $Hand(item)]
    set Hand(xc) [expr [lindex $coo 0] + $Hand(rad)]
    set Hand(yc) [expr [lindex $coo 1] + $Hand(rad)]

    #????
    $c move $Ruler(item) $dx $dy
}

#frame .x
#label .x.lbl -text "xpos ="
#label .x.val -textvariable Hand(xc) -bg white -bd 2 -relief sunken
#pack .x.lbl -side left
#pack .x.val -side left -fill x 
#
#frame .y
#label .y.lbl -text "ypos ="
#label .y.val -textvariable Hand(yc) -bg white -bd 2 -relief sunken
#pack .y.lbl -side left
#pack .y.val -side left -fill x
#
#pack .y -side top
#pack .x -side top

#APPEL DU PROGRAMME LUSTRE
set pipe [open {|ecexe game.ec -r} w+]

#puts $pipe

set STEP 0

proc updateRuler { } {
	global Hand Ruler pipe STEP

	#Mise en place des parametres : 	
	#N.B. : changement de coordonnées entre tcl et lustre :
	#Soit X,Y un point Lustre -> 
	puts $pipe [tk2lus_x $Hand(xc)]
	puts $pipe [tk2lus_y $Hand(yc)]

	flush $pipe
	gets $pipe X
	gets $pipe Y

#puts "#step [incr STEP]"
#puts "$Hand(xc) $Hand(yc) #outs $X $Y"

	set Ruler(x) [lus2tk_x $X]
	set Ruler(y) [lus2tk_y $Y]

	.c delete $Ruler(item)
	set Ruler(item) [.c create line $Hand(xc) $Hand(yc) $Ruler(x) $Ruler(y) \
		-width 4  -fill red]
	.c lower $Ruler(item) $Hand(item)
}

proc updateRulerPeriodic { } {
    global Ruler
    updateRuler
    set Ruler(timer) [add_timer $Ruler(period) updateRulerPeriodic]
#puts "updateRulerPeriodic Ruler(timer) = $Ruler(timer)"
}

puts "#program game"
puts "#inputs x0:real y0:real"
puts "#outputs x:real y:real"

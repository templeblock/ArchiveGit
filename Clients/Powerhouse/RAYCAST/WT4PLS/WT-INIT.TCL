create me -mass 70 -size 1 1 3 -drag 0.6 -position 0 0 0 -angle 0
me -attachviewpoint -attachcontrol

create metoo -mass 35 -size 1 1 3 -drag 0.6 -position 10 10 0 -angle 0

proc action_jump {} {me -force 0 0 50}
proc action_1 {} {me -size 1 1 1}
proc action_2 {} {me -size 1 1 3}
proc action_3 {} {me -size 1 1 5}
proc action_4 {} {metoo -attachviewpoint -attachcontrol}
proc action_5 {} {me -attachviewpoint -attachcontrol}

proc start {x y} {me -position $x $y 0}

		  
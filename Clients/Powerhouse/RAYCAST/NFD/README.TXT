		First 3-D Development (FD)


	This is a MFC Windows based adaptation of the Lary Myers
ACK 3-D engine, highly modified, updated and optimized, using
WinG to achieve up to 18 frames per second. Plays .MID or .RMI
music, sound FX using .WAV files, low resolution (160 by 100)
and high resolution (320 by 200) modes, screen sizes from 320 by
200 up to 640 by 400, weapons (fists, pistol, shotgun, machine gun
and rocket laucher), doors, keys, medikits, ammo, saved games,
floor squares, floor maps, God mode (double right click on Help
dialog), vertical motion (walking and jumping), scenic backdrops,
and more. Everything you need for a Wolfenstien style game except
your creativity and graphics. Includes an updated MEDIT map editor.


		Version 1.0

*	Initial release to public domain.


		Version 1.1

What's Changed:

*	Miscellaneous fixes (Major fix for low resolution mode; if
	you got killed, you received the dreaded "General Protection
	Fault." This was due to the screen size parameter passed to
	the TintScreen routine being incorrect).  Other fixes were
	very minor and I term them cosmetic.

*	Fixed God mode so all walls and all objects will be seen in
	map.  Use mouse to move around map.

*	Added notification (via status window) when entering Special
	squares: Start, Up, Down and Goal.

*	Added floor squares.  For full bitmapped floors you'll have to
	wait on Lary Myers new book; I wouldn't spoil it for the world.
	This will give you something to work with and a good feeling
	of down.  It will also let you see how it's done (look at the
	code in ack\ackflrs.cpp).  Floors go especially well with what
	follows...

*	Added vertical movement (Yes, that's right, now you can jog up
	and down while walking.  And you can jump!)  In fact, I added a
	floor level force field object; if you walk through it, it hurts.
	But you can jump over it; with some practice.  Use F2, F3 and F4
	to preview half higher, half lower and normal height, respectively.

*	Added game timing for each level for scoring purposes.  Also,
	saved present time for level with saved game information so you
	can continue a game where you left off.


		Version 1.2

What's Changed:

*	Separated bitmap image information from the Object list information
	in the ACKENG structure.  Only the changeable information is in the
	structure.  Evrything else is in a new structure array.  This
	reduced the size of saved games.

*	Fixed a bug in the draw_object assembly language routine that
	affected light shading over distance.

*	Made the WinG routines always select Top-Down bitmap drawing.  Some
	people saw the screen upside down.  The assembly routines should be
	modified to do either Top-Down or Bottom-Up drawing to maximize
	performance.


		Version 1.3

What's Changed:

*	Fixed 'ghost' monsters after restoring a saved game.  Monsters were
	brought back to life upon restoring a game due to confusion over
	static vs dynamic monster structure information (static monster
	info is stored outside the ACKENG structure, now, for space
	conservation of saved games).

*	Made amount of ammo for pistol and machine gun the same.  It takes
	5 rounds to fire the machine gun vs 1 for the pistol.  Both draw
	from the save bullet reserve.


		Version 1.4

What's Changed:

*	Made several modifications to the assembly routines to increase
	the speed of screen updates.  This also required a couple of
	changes to the calling routines since there was at least one
	assembly routine that had a parameter change.

*	Added scenic backgroung to FD.  Now either of two types of
	"ceilings" are available: 1) solid (as would be the case if the
	player was inside ), and 2) scenic (depicting a scene, like
	mountains, that remains set (relative to player angle) as the
	"ceiling").

*	Fixed a bug that made it where a monster could not "touch"
	the player.  This meant that the player could not fight the
	monster.


John Lundy
71673,1203

PS-	Wanted:  Talented computer artist that can help generate some
	"good" graphics for walls and objects.

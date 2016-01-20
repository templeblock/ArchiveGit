##############################################################################
#                                                                            #
#                         Makefile user preferences                          #
#                                                                            #
##############################################################################

#
# BUILD
#
# You can redefine BUILD to change the default buildtype used when you run
# GO.BAT or NMAKE with no parameters.  Valid settings are:  CV CVL DNL SHIP
# To redefine BUILD, uncomment the following three lines; all three lines 
# must begin in the leftmost column!

# !ifndef BUILD
# BUILD = CVL
# !endif


#
# Overriding REF, DST, and OBJ directories
#
# You can also override the variables by setting DOS environment variables
# but this is cleaner.  By overriding, you can have multiple copies of
# Designer in progress by just using different OBJ and/or REF directories.
#
# You can also set multiple REF directories to search by specifying a
# DOS-style searchpath separated by semi-colons.
#

JIM=1

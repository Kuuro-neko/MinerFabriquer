#!/bin/sh
bindir=$(pwd)
cd /home/mathis/Documents/M1_IMAGINE/MoteurDeJeux/MinecraftProject/TP1/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "xYES" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		/usr/bin/gdb -batch -command=$bindir/gdbscript --return-child-result /home/mathis/Documents/M1_IMAGINE/MoteurDeJeux/MinecraftProject/TP1/MinerFabriquer 
	else
		"/home/mathis/Documents/M1_IMAGINE/MoteurDeJeux/MinecraftProject/TP1/MinerFabriquer"  
	fi
else
	"/home/mathis/Documents/M1_IMAGINE/MoteurDeJeux/MinecraftProject/TP1/MinerFabriquer"  
fi

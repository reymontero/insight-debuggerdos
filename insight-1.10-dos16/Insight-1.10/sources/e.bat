@echo off

tasm /m %1 %2 %3 %4 %5 %6 %7 %8 %9 insight.asm >error.log
if ErrorLevel 1 goto TasmError

tlink /t/3/x insight.obj >>error.log
if ErrorLevel 1 goto TlinkError
type error.log
del insight.obj
del error.log
insight insight.com
goto Exit


:TlinkError
del insight.obj

:TasmError
more < error.log

:Exit

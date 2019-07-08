setlocal

del /Q boot.vfd
SET day=0%1
SET targetPath=%day:~-2%_day

REM ipl build
bin\tolset\z_tools\nask.exe %targetPath%\ipl.nas tmp\ipl.bin

REM asmhead build
SET asmheadPath=%targetPath%\asmhead.nas
bin\tolset\z_tools\nask.exe %asmheadPath% tmp\asmhead.bin tmp\asmhead.lst

REM bootpack build
SET bootpackPath=%targetPath%\bootpack.c
bin\tolset\z_tools\cc1.exe -I bin\tolset\z_tools\haribote\ -Os -Wall -quiet -o tmp\bootpack.gas %bootpackPath%

REM gas 2 nask
bin\tolset\z_tools\gas2nask.exe -a tmp\bootpack.gas tmp\bootpack.nas

REM nas 2 obj
bin\tolset\z_tools\nask.exe tmp\bootpack.nas tmp\bootpack.obj
bin\tolset\z_tools\nask.exe %targetPath%\naskfunc.nas tmp\naskfunc.obj

REM obj 2 bim
bin\tolset\z_tools\obj2bim.exe @bin\tolset\z_tools\haribote\haribote.rul out:tmp\bootpack.bim stack:3136k map:tmp\bootpack.map tmp\bootpack.obj tmp\naskfunc.obj

REM bim 2 hrb
bin\tolset\z_tools\bim2hrb.exe tmp\bootpack.bim tmp\bootpack.hrb 0

REM copy
copy /B tmp\asmhead.bin+tmp\bootpack.hrb tmp\haribote.sys

REM edimg
bin\tolset\z_tools\edimg.exe @edimg_script.txt
setlocal

del /Q boot.vfd
SET day=0%1

REM ipl build
SET iplPath=%day:~-2%_day\ipl.nas
bin\tolset\z_tools\nask.exe %iplPath% ipl.bin

REM haribote build
SET haribotePath=%day:~-2%_day\haribote.nas
bin\tolset\z_tools\nask.exe %haribotePath% haribote.sys

REM edimg
bin\tolset\z_tools\edimg.exe @edimg_script.txt
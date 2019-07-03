setlocal

del /Q boot.vfd
SET day=0%1
SET nasPath=%day:~-2%_day\%2.nas
bin\tolset\z_tools\nask.exe %nasPath% boot.vfd
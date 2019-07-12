param([string]$targetDay)

$targetPath = $targetDay.PadLeft(2, '0') + "_day"

# 掃除
Remove-Item tmp -recurse -force
Remove-Item boot.vfd
New-Item tmp -ItemType Directory

# nas 2 bin
@("ipl","asmhead") | %{bin\tolset\z_tools\nask.exe "$($targetPath)\$($_).nas" "tmp\$($_).bin"}

# c 2 gas
Get-ChildItem $targetPath -recurse -include *.c | %{bin\tolset\z_tools\cc1.exe -I bin\tolset\z_tools\haribote\ -Os -Wall -quiet -o "tmp\$([System.IO.Path]::GetFileNameWithoutExtension($_.Name)).gas" $_.FullName}

# gas 2 nas
Get-ChildItem "tmp\" -recurse -include *.gas | %{bin\tolset\z_tools\gas2nask.exe -a "$($_.FullName)" "tmp\$([System.IO.Path]::GetFileNameWithoutExtension($_.Name)).nas"}

# nas 2 obj
@("naskfunc") | %{bin\tolset\z_tools\nask.exe "$($targetPath)\$($_).nas" "tmp\$($_).obj"}
Get-ChildItem "tmp\" -recurse -include *.nas | %{bin\tolset\z_tools\nask.exe "$($_.FullName)" "tmp\$([System.IO.Path]::GetFileNameWithoutExtension($_.Name)).obj"}

# font
bin\tolset\z_tools\makefont.exe "$($targetPath)\hankaku.txt" tmp\hankaku.bin
bin\tolset\z_tools\bin2obj.exe tmp\hankaku.bin tmp\hankaku.obj _hankaku

# obj 2 bim
bin\tolset\z_tools\obj2bim.exe "@bin\tolset\z_tools\haribote\haribote.rul" out:tmp\bootpack.bim stack:3136k map:tmp\bootpack.map tmp\bootpack.obj tmp\naskfunc.obj tmp\hankaku.obj tmp\dsctbl.obj tmp\graphic.obj tmp\int.obj

# bim 2 hrb
bin\tolset\z_tools\bim2hrb.exe tmp\bootpack.bim tmp\bootpack.hrb 0

# copy
cmd /c copy /B tmp\asmhead.bin+tmp\bootpack.hrb tmp\haribote.sys

# edimg
bin\tolset\z_tools\edimg.exe "@edimg_script.txt"
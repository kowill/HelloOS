param([string]$targetDay)

$targetPath = $targetDay.PadLeft(2, '0') + "_day"

# 掃除
Remove-Item tmp -recurse -force
Remove-Item boot.vfd
New-Item tmp -ItemType Directory
New-Item tmp\app -ItemType Directory

# nas 2 bin
@("ipl", "asmhead") | % { bin\tolset\z_tools\nask.exe "$($targetPath)\$($_).nas" "tmp\$($_).bin" }

# nas(app) 2 hrb
Get-ChildItem ($targetPath + '\app') -depth 0 -include *.nas | 
% {
    $appName = [System.IO.Path]::GetFileNameWithoutExtension($_.Name)
    if ($appName -in @("hello", "hello2")) {
        bin\tolset\z_tools\nask.exe $_.FullName "tmp\app\$($appName).hrb" 
    }
    else {
        bin\tolset\z_tools\nask.exe "$($_.FullName)" "tmp\app\$($appName).obj"
        bin\tolset\z_tools\obj2bim.exe "@bin\tolset\z_tools\haribote\haribote.rul" "out:tmp\app\$($appName).bim" "stack:1k" "map:tmp\app\$($appName).map" "tmp\app\$($appName).obj"
        bin\tolset\z_tools\bim2hrb.exe "tmp\app\$($appName).bim" "tmp\app\$($appName).hrb" 0 
    }
}

# app_c 2 hrb
$appTargest = @(
    @{Name = "a"; Link = @("a", "a_nask") ; HeapSize = "0" },
    @{Name = "hello3"; Link = @("hello3", "a_nask"); HeapSize = "0" },
    @{Name = "hello4"; Link = @("hello4", "a_nask"); HeapSize = "0" },
    @{Name = "winhelo"; Link = @("winhelo", "a_nask"); HeapSize = "0" },
    @{Name = "winhelo2"; Link = @("winhelo2", "a_nask"); HeapSize = "0" },
    @{Name = "winhelo3"; Link = @("winhelo3", "a_nask"); HeapSize = "40k" },
    @{Name = "star1"; Link = @("star1", "a_nask"); HeapSize = "47k" },
    @{Name = "stars"; Link = @("stars", "a_nask"); HeapSize = "47k" },
    @{Name = "stars2"; Link = @("stars2", "a_nask"); HeapSize = "47k" },
    @{Name = "lines"; Link = @("lines", "a_nask"); HeapSize = "47k" },
    @{Name = "walk"; Link = @("walk", "a_nask"); HeapSize = "47k" },
    @{Name = "noodle"; Link = @("noodle", "a_nask"); HeapSize = "40k" },
    @{Name = "beepdown"; Link = @("beepdown", "a_nask"); HeapSize = "40k" },
    @{Name = "beepup"; Link = @("beepup", "a_nask"); HeapSize = "40k" },
    @{Name = "color"; Link = @("color", "a_nask"); HeapSize = "56k" },
    @{Name = "color2"; Link = @("color2", "a_nask"); HeapSize = "56k" }
)
Get-ChildItem "$($targetPath)\app_c" -depth 0 -include *.c | % { bin\tolset\z_tools\cc1.exe -I bin\tolset\z_tools\haribote\ -Os -Wall -quiet -o "tmp\app\$([System.IO.Path]::GetFileNameWithoutExtension($_.Name)).gas" $_.FullName }
Get-ChildItem "tmp\app" -depth 0 -include *.gas | % { bin\tolset\z_tools\gas2nask.exe -a "$($_.FullName)" "tmp\app\$([System.IO.Path]::GetFileNameWithoutExtension($_.Name)).nas" }
Get-ChildItem "$($targetPath)\app_c" -depth 0 -include *.nas | Copy-Item -Destination "tmp\app"
Get-ChildItem "tmp\app" -depth 0 -include *.nas | % { bin\tolset\z_tools\nask.exe "$($_.FullName)" "tmp\app\$([System.IO.Path]::GetFileNameWithoutExtension($_.Name)).obj" }
$appTargest |
% {
    $linkTargets = $_.Link | % { "tmp\app\$($_).obj" }
    bin\tolset\z_tools\obj2bim.exe "@bin\tolset\z_tools\haribote\haribote.rul" "out:tmp\app\$($_.Name).bim" "stack:1k" "map:tmp\app\$($_.Name).map" $linkTargets 
    bin\tolset\z_tools\bim2hrb.exe "tmp\app\$($_.Name).bim" "tmp\app\$($_.Name).hrb" "$($_.HeapSize)"
}

# c 2 gas
Get-ChildItem $targetPath -depth 0 -include *.c | % { bin\tolset\z_tools\cc1.exe -I bin\tolset\z_tools\haribote\ -Os -Wall -quiet -o "tmp\$([System.IO.Path]::GetFileNameWithoutExtension($_.Name)).gas" $_.FullName }

# gas 2 nas
Get-ChildItem "tmp\" -depth 0 -include *.gas | % { bin\tolset\z_tools\gas2nask.exe -a "$($_.FullName)" "tmp\$([System.IO.Path]::GetFileNameWithoutExtension($_.Name)).nas" }

# nas 2 obj
@("naskfunc") | % { bin\tolset\z_tools\nask.exe "$($targetPath)\$($_).nas" "tmp\$($_).obj" }
Get-ChildItem "tmp\" -depth 0 -include *.nas | % { bin\tolset\z_tools\nask.exe "$($_.FullName)" "tmp\$([System.IO.Path]::GetFileNameWithoutExtension($_.Name)).obj" }

# font
bin\tolset\z_tools\makefont.exe "$($targetPath)\hankaku.txt" tmp\hankaku.bin
bin\tolset\z_tools\bin2obj.exe tmp\hankaku.bin tmp\hankaku.obj _hankaku

# obj 2 bim
$obj = Get-ChildItem "tmp\" -depth 0 -include *.obj | % { $_.FullName };
bin\tolset\z_tools\obj2bim.exe "@bin\tolset\z_tools\haribote\haribote.rul" out:tmp\bootpack.bim stack:3136k map:tmp\bootpack.map $obj

# bim 2 hrb
bin\tolset\z_tools\bim2hrb.exe tmp\bootpack.bim tmp\bootpack.hrb 0

# copy
cmd /c copy /B tmp\asmhead.bin+tmp\bootpack.hrb tmp\haribote.sys

# edimg_script
$scriptPath = "edimg_script.txt"
Out-File $scriptPath
Add-Content $scriptPath "imgin:bin\tolset\z_tools\fdimg0at.tek"
Add-Content $scriptPath "wbinimg src:tmp\ipl.bin len:512 from:0 to:0"
Add-Content $scriptPath "copy from:tmp\haribote.sys to:@:"
Add-Content $scriptPath "copy from:readme.md to:@:"
Add-Content $scriptPath "copy from:build.ps1 to:@:"
Get-ChildItem "tmp\app" -depth 0 -include *.hrb | % { Add-Content $scriptPath ("copy from:tmp\app\$($_.Name) to:@:") }
Add-Content $scriptPath "imgout:boot.vfd"

# edimg
bin\tolset\z_tools\edimg.exe "@$($scriptPath)"
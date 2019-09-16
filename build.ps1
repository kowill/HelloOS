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

# create lib
$apis = Get-ChildItem "$($targetPath)\app_c" -depth 0 -include api*.nas | 
% { 
    $path = "tmp\app\$([System.IO.Path]::GetFileNameWithoutExtension($_.Name)).obj"
    bin\tolset\z_tools\nask.exe "$($_.FullName)" $path
    return $path
}
bin\tolset\z_tools\golib00.exe $apis "out:tmp\app\apilib.lib"

# app_c 2 hrb

$appTargest = @(
    @{Name = "a" },
    @{Name = "hello3" },
    @{Name = "hello4" },
    @{Name = "winhelo"; StackSize = "8k" },
    @{Name = "winhelo2"; StackSize = "8k" },
    @{Name = "winhelo3"; HeapSize = "40k" },
    @{Name = "star1"; HeapSize = "47k" },
    @{Name = "stars"; HeapSize = "47k" },
    @{Name = "stars2"; HeapSize = "47k" },
    @{Name = "lines"; HeapSize = "47k" },
    @{Name = "walk"; HeapSize = "47k" },
    @{Name = "noodle"; HeapSize = "40k" },
    @{Name = "beepdown"; HeapSize = "40k" },
    @{Name = "beepup"; HeapSize = "40k" },
    @{Name = "color"; HeapSize = "56k" },
    @{Name = "color2"; HeapSize = "56k" },
    @{Name = "sosu"; HeapSize = "56k"; StackSize = "11k" },
    @{Name = "sosu3"; HeapSize = "56k"; StackSize = "11k" },
    @{Name = "type" },
    @{Name = "iroha" },
    @{Name = "chlang" },
    @{Name = "notrec"; StackSize = "11k" },
    @{Name = "bball"; StackSize = "52k" },
    @{Name = "invader"; StackSize = "90k" },
    @{Name = "calc"; StackSize = "4k" }
)
Get-ChildItem "$($targetPath)\app_c" -depth 0 -include *.c | % { bin\tolset\z_tools\cc1.exe -I bin\tolset\z_tools\haribote\ -Os -Wall -quiet -o "tmp\app\$([System.IO.Path]::GetFileNameWithoutExtension($_.Name)).gas" $_.FullName }
Get-ChildItem "tmp\app" -depth 0 -include *.gas | % { bin\tolset\z_tools\gas2nask.exe -a "$($_.FullName)" "tmp\app\$([System.IO.Path]::GetFileNameWithoutExtension($_.Name)).nas" }
Get-ChildItem "tmp\app" -depth 0 -include *.nas | % { bin\tolset\z_tools\nask.exe "$($_.FullName)" "tmp\app\$([System.IO.Path]::GetFileNameWithoutExtension($_.Name)).obj" }

$appTargest |
% {
    $stack = $_["StackSize"];
    if ($stack -eq $null) {
        $stack = "1k";
    }
    $heap = $_["HeapSize"];
    if ($heap -eq $null) {
        $heap = "0";
    }
    bin\tolset\z_tools\obj2bim.exe "@bin\tolset\z_tools\haribote\haribote.rul" "out:tmp\app\$($_.Name).bim" "stack:$($stack)" "map:tmp\app\$($_.Name).map" "tmp\app\$($_.Name).obj" "tmp\app\apilib.lib"
    bin\tolset\z_tools\bim2hrb.exe "tmp\app\$($_.Name).bim" "tmp\app\$($_.Name).org" "$($heap)"
    bin\tolset\z_tools\bim2bin.exe -osacmp "in:tmp\app\$($_.Name).org" "out:tmp\app\$($_.Name).hrb"
}

# c 2 gas
Get-ChildItem $targetPath -depth 0 -include *.c | % { bin\tolset\z_tools\cc1.exe -I bin\tolset\z_tools\haribote\ -Os -Wall -quiet -o "tmp\$([System.IO.Path]::GetFileNameWithoutExtension($_.Name)).gas" $_.FullName }
Get-ChildItem "bin\" -depth 0 -include *.c | % { bin\tolset\z_tools\cc1.exe -I bin\tolset\z_tools\haribote\ -Os -Wall -quiet -o "tmp\$([System.IO.Path]::GetFileNameWithoutExtension($_.Name)).gas" $_.FullName }

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
Add-Content $scriptPath "copy from:bin\nihongo.fnt to:@:"
Add-Content $scriptPath "imgout:boot.vfd"

# edimg
bin\tolset\z_tools\edimg.exe "@$($scriptPath)"
$ServerName = "HelloOs"
$Vm = Get-VM | ? name -eq $ServerName

if($Vm -eq $null)
{
    NEW-VM $ServerName -Path "F:\vhd\$VMName" -Generation 1 -MemoryStartupBytes 67108864 -NewVHDPath "F:\vhd\$ServerName\$ServerName.vhdx" -NewVHDSizeBytes 3145728 -BootDevice "Floppy"
    Set-VMFloppyDiskDrive $ServerName "$PSScriptRoot\boot.vfd"
}

Start-VM -Name $ServerName
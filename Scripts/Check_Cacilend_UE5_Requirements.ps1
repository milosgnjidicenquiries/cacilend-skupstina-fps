param(
    [string]$ProjectRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = "Continue"
$ProjectRoot = (Resolve-Path -LiteralPath $ProjectRoot).Path
$ReportDir = Join-Path $ProjectRoot "Build"
$ReportPath = Join-Path $ReportDir "UE5-preflight-report.txt"
New-Item -ItemType Directory -Path $ReportDir -Force | Out-Null

$script:Results = New-Object System.Collections.Generic.List[string]
function Add-Check {
    param([string]$Status, [string]$Name, [string]$Detail)
    $line = "{0,-5} | {1}: {2}" -f $Status, $Name, $Detail
    $script:Results.Add($line)
    Write-Host $line
}

Write-Host ""
Write-Host "CACILEND UE5.8 - PROVERA RACUNARA I ALATA"
Write-Host "Projekat: $ProjectRoot"
Write-Host "Izvestaj: $ReportPath"
Write-Host ""

# Project files
$projectFile = Join-Path $ProjectRoot "CacilendUE.uproject"
$buildBat = Join-Path $ProjectRoot "Build_Run_Cacilend_UE5.bat"
if (Test-Path -LiteralPath $projectFile) {
    try {
        $project = Get-Content -LiteralPath $projectFile -Raw | ConvertFrom-Json
        if ($project.EngineAssociation -eq "5.8") {
            Add-Check "PASS" "UE projekat" "CacilendUE.uproject postoji i trazi Unreal Engine 5.8."
        } else {
            Add-Check "FAIL" "UE projekat" "EngineAssociation je '$($project.EngineAssociation)', ocekivano je 5.8."
        }
    } catch {
        Add-Check "FAIL" "UE projekat" "CacilendUE.uproject nije validan JSON: $($_.Exception.Message)"
    }
} else {
    Add-Check "FAIL" "UE projekat" "CacilendUE.uproject nije pronadjen. Pokreni proveru iz otpakovanog foldera projekta."
}
if ((Test-Path -LiteralPath (Join-Path $ProjectRoot "Source\CacilendUE\CacilendUE.Build.cs")) -and (Test-Path -LiteralPath $buildBat)) {
    Add-Check "PASS" "Izvorni kod" "C++ modul i skripta Build_Run_Cacilend_UE5.bat postoje."
} else {
    Add-Check "FAIL" "Izvorni kod" "Nedostaje C++ modul ili Build_Run_Cacilend_UE5.bat."
}

# OS and machine
try {
    $os = Get-CimInstance Win32_OperatingSystem
    $computer = Get-CimInstance Win32_ComputerSystem
    $osCaption = [string]$os.Caption
    $buildNumber = [int]$os.BuildNumber
    $arch = [string]$os.OSArchitecture
    if ($osCaption -match "Windows 11" -and $arch -match "64") {
        Add-Check "PASS" "Windows" "$osCaption, build $buildNumber, $arch."
    } elseif ($osCaption -match "Windows 10" -and $arch -match "64" -and ($buildNumber -ge 19045 -or ($osCaption -match "Enterprise" -and $buildNumber -ge 19044))) {
        Add-Check "PASS" "Windows" "$osCaption, build $buildNumber, $arch."
    } elseif ($osCaption -match "Windows" -and $arch -match "64") {
        Add-Check "WARN" "Windows" "$osCaption, build $buildNumber, $arch. Proveri Epic-ove zahteve za podrzanu verziju."
    } else {
        Add-Check "FAIL" "Windows" "Detektovano: $osCaption / $arch. Potreban je podrzan 64-bitni Windows."
    }

    $ramGB = [math]::Round(([double]$computer.TotalPhysicalMemory / 1GB), 1)
    if ($ramGB -ge 32) {
        Add-Check "PASS" "RAM" "$ramGB GB; ispunjava preporuku od 32 GB."
    } elseif ($ramGB -ge 16) {
        Add-Check "WARN" "RAM" "$ramGB GB; moze raditi, ali UE5 editor/build mogu biti spori. Preporuka je 32 GB."
    } else {
        Add-Check "WARN" "RAM" "$ramGB GB; za UE5.8 je ovo tesno, ocekuj ozbiljna ogranicenja."
    }

    $cpu = Get-CimInstance Win32_Processor | Select-Object -First 1
    if ($cpu) {
        Add-Check "INFO" "Procesor" "$($cpu.Name); logical processors: $([Environment]::ProcessorCount)."
    }
    $gpus = @(Get-CimInstance Win32_VideoController -ErrorAction SilentlyContinue | Where-Object { $_.Name } | Select-Object -ExpandProperty Name -Unique)
    if ($gpus.Count -gt 0) {
        Add-Check "INFO" "Grafika" "$($gpus -join '; '). WMI ne moze pouzdano da potvrdi DX12 podrsku ni tacnu VRAM memoriju."
    } else {
        Add-Check "WARN" "Grafika" "Windows nije vratio naziv graficke kartice."
    }
} catch {
    Add-Check "WARN" "Hardver/Windows" "Automatsko citanje nije uspelo: $($_.Exception.Message)"
}

# Unreal Engine discovery
$engineCandidates = New-Object System.Collections.Generic.List[string]
if ($env:UE_5_8_ROOT) { $engineCandidates.Add($env:UE_5_8_ROOT) }
$engineCandidates.Add((Join-Path $env:ProgramFiles "Epic Games\UE_5.8"))
$engineCandidates.Add("C:\Epic Games\UE_5.8")
$engineCandidates.Add("D:\Epic Games\UE_5.8")
$programData = [Environment]::GetFolderPath("CommonApplicationData")
$manifestDir = Join-Path $programData "Epic\EpicGamesLauncher\Data\Manifests"
if (Test-Path -LiteralPath $manifestDir) {
    foreach ($manifest in (Get-ChildItem -LiteralPath $manifestDir -Filter "*.item" -File -ErrorAction SilentlyContinue)) {
        try {
            $item = Get-Content -LiteralPath $manifest.FullName -Raw | ConvertFrom-Json
            $label = "$($item.AppName) $($item.DisplayName) $($item.InstallLocation)"
            if ($label -match "5[._-]8") {
                if ($item.InstallLocation) { $engineCandidates.Add([string]$item.InstallLocation) }
            }
        } catch { }
    }
}
$engineRoot = $null
$engineBuildVersion = $null
foreach ($candidate in ($engineCandidates | Where-Object { $_ } | Select-Object -Unique)) {
    $buildTool = Join-Path $candidate "Engine\Build\BatchFiles\Build.bat"
    $editorExe = Join-Path $candidate "Engine\Binaries\Win64\UnrealEditor.exe"
    $versionFile = Join-Path $candidate "Engine\Build\Build.version"
    if ((Test-Path -LiteralPath $buildTool) -and (Test-Path -LiteralPath $editorExe)) {
        $versionOkay = $false
        if (Test-Path -LiteralPath $versionFile) {
            try {
                $version = Get-Content -LiteralPath $versionFile -Raw | ConvertFrom-Json
                if ([int]$version.MajorVersion -eq 5 -and [int]$version.MinorVersion -eq 8) {
                    $versionOkay = $true
                    $engineBuildVersion = "$($version.MajorVersion).$($version.MinorVersion) build $($version.Changelist)"
                }
            } catch { }
        }
        if ($versionOkay -or $candidate -match "UE[_ -]?5[._-]8") {
            $engineRoot = $candidate
            break
        }
    }
}
if ($engineRoot) {
    Add-Check "PASS" "Unreal Engine" "Pronadjen UE 5.8: $engineRoot ($engineBuildVersion)."
} else {
    Add-Check "FAIL" "Unreal Engine" "UE 5.8 Editor/Build.bat nije pronadjen na uobicajenim lokacijama ili u Epic Launcher manifestima. Instaliraj ga ili postavi UE_5_8_ROOT."
}

# Visual Studio, MSVC and Windows SDK
$vswhere = Join-Path ([Environment]::GetFolderPath("ProgramFilesX86")) "Microsoft Visual Studio\Installer\vswhere.exe"
$vsInstances = @()
$vsWorkloadPaths = @()
if (Test-Path -LiteralPath $vswhere) {
    try {
        $raw = & $vswhere -all -products '*' -format json 2>$null
        if ($raw) { $vsInstances = @(($raw -join [Environment]::NewLine) | ConvertFrom-Json) }
        $vsWorkloadPaths = @(& $vswhere -all -products '*' -requires Microsoft.VisualStudio.Workload.NativeGame -property installationPath 2>$null | Where-Object { $_ })
    } catch {
        Add-Check "WARN" "Visual Studio discovery" "vswhere je pronadjen, ali nije mogao da procita instalacije."
    }
} else {
    Add-Check "FAIL" "Visual Studio Installer" "vswhere.exe nije pronadjen; instaliraj Visual Studio sa C++ alatima."
}
$validVs = $null
$validMsvc = $false
foreach ($instance in $vsInstances) {
    $versionText = [string]$instance.installationVersion
    $vsPath = [string]$instance.installationPath
    if (-not $versionText -or -not $vsPath) { continue }
    try { $vsVersion = [version]$versionText } catch { continue }
    $versionOkay = ($vsVersion.Major -ge 18) -or ($vsVersion.Major -eq 17 -and $vsVersion -ge [version]"17.14.0")
    $workloadOkay = $vsWorkloadPaths -contains $vsPath
    if ($versionOkay -and $workloadOkay) {
        $validVs = $instance
        $msvcDirs = @(Get-ChildItem -LiteralPath (Join-Path $vsPath "VC\Tools\MSVC") -Directory -ErrorAction SilentlyContinue)
        foreach ($dir in $msvcDirs) {
            try {
                if ([version]$dir.Name -ge [version]"14.38.0") { $validMsvc = $true }
            } catch { }
        }
        if ($validMsvc) { break }
    }
}
if ($validVs) {
    Add-Check "PASS" "Visual Studio" "$($validVs.displayName) $($validVs.installationVersion), workload 'Game development with C++' je detektovan."
} else {
    Add-Check "FAIL" "Visual Studio + C++ workload" "Potreban je Visual Studio 2026 ili Visual Studio 2022 17.14+ sa workload-om 'Game development with C++'."
}
if ($validMsvc) {
    Add-Check "PASS" "MSVC toolset" "Pronadjen toolset 14.38 ili noviji u kompatibilnoj VS instalaciji."
} else {
    Add-Check "FAIL" "MSVC toolset" "Nije pronadjen MSVC 14.38 ili noviji u kompatibilnoj VS instalaciji. Dodaj odgovarajuci MSVC komponent iz Visual Studio Installer-a."
}

$sdkRoot = Join-Path ([Environment]::GetFolderPath("ProgramFilesX86")) "Windows Kits\10\Include"
$sdkVersions = @()
if (Test-Path -LiteralPath $sdkRoot) {
    foreach ($dir in (Get-ChildItem -LiteralPath $sdkRoot -Directory -ErrorAction SilentlyContinue)) {
        try {
            $sdkVersion = [version]$dir.Name
            if ($sdkVersion -ge [version]"10.0.22621.0") { $sdkVersions += $dir.Name }
        } catch { }
    }
}
if ($sdkVersions.Count -gt 0) {
    Add-Check "PASS" "Windows SDK" "Pronadjena kompatibilna verzija: $(($sdkVersions | Sort-Object -Descending | Select-Object -First 3) -join ', ')."
} else {
    Add-Check "FAIL" "Windows SDK" "Nije pronadjen Windows SDK 10.0.22621.0 ili noviji."
}

# Free disk space on project drive; engine drive if detected
try {
    $pathsToCheck = @($ProjectRoot)
    if ($engineRoot) { $pathsToCheck += $engineRoot }
    $driveLetters = @($pathsToCheck | ForEach-Object { [IO.Path]::GetPathRoot($_).TrimEnd('\') } | Select-Object -Unique)
    foreach ($drive in $driveLetters) {
        $logical = Get-CimInstance Win32_LogicalDisk -Filter "DeviceID='$drive'"
        if ($logical) {
            $freeGB = [math]::Round(([double]$logical.FreeSpace / 1GB), 1)
            if ($freeGB -ge 80) { Add-Check "PASS" "Slobodan prostor ($drive)" "$freeGB GB slobodno." }
            else { Add-Check "WARN" "Slobodan prostor ($drive)" "$freeGB GB slobodno; UE kompilacija, shader cache i pakovanje mogu traziti dosta prostora." }
        }
    }
} catch {
    Add-Check "INFO" "Disk" "Slobodan prostor nije mogao biti izmeren."
}

# Optional texture-generation tooling
$pythonCmd = Get-Command python -ErrorAction SilentlyContinue
if ($pythonCmd) {
    $pythonReady = $false
    try {
        & python -c "import numpy, PIL" 2>$null
        if ($LASTEXITCODE -eq 0) { $pythonReady = $true }
    } catch { }
    if ($pythonReady) {
        Add-Check "PASS" "Python teksture (opciono)" "Python i NumPy/Pillow su dostupni."
    } else {
        Add-Check "WARN" "Python teksture (opciono)" "Python postoji, ali NumPy/Pillow nedostaju; UE build nije blokiran, ali automatsko generisanje PBR tekstura moze biti preskoceno."
    }
} else {
    Add-Check "INFO" "Python teksture (opciono)" "Python nije detektovan. Nije potreban za C++ kompajliranje; koristi se za dodatni alat za teksture."
}

$script:Results | Set-Content -LiteralPath $ReportPath -Encoding UTF8
$failCount = @($script:Results | Where-Object { $_ -match "^FAIL" }).Count
$warnCount = @($script:Results | Where-Object { $_ -match "^WARN" }).Count
Write-Host ""
if ($failCount -eq 0 -and $warnCount -eq 0) {
    Write-Host "REZULTAT: Osnovne provere su prosle. Sledeci korak je stvarni C++ build."
} elseif ($failCount -eq 0) {
    Write-Host "REZULTAT: Nema kriticnih blokera u proverama, ali pregledaj upozorenja."
} else {
    Write-Host "REZULTAT: $failCount kriticnih stavki nedostaje i $warnCount upozorenja. Popravi FAIL stavke pre build-a."
}
Write-Host "Izvestaj je sacuvan u: $ReportPath"
Write-Host "Napomena: ovo je provera instalacije, ne kompajlira projekat i ne garantuje da build/gameplay rade."
if ($failCount -gt 0) { exit 2 } else { exit 0 }

param(
    [ValidateSet("Build", "Editor", "Package")]
    [string]$Purpose = "Editor"
)

$ErrorActionPreference = "SilentlyContinue"
$candidates = New-Object System.Collections.Generic.List[string]

if ($env:UE_5_8_ROOT) {
    $candidates.Add($env:UE_5_8_ROOT.Trim().Trim('"'))
}
if ($env:ProgramFiles) {
    $candidates.Add((Join-Path $env:ProgramFiles "Epic Games\UE_5.8"))
}
$candidates.Add("C:\Epic Games\UE_5.8")
$candidates.Add("D:\Epic Games\UE_5.8")

$programData = [Environment]::GetFolderPath("CommonApplicationData")
$manifestDir = Join-Path $programData "Epic\EpicGamesLauncher\Data\Manifests"
if (Test-Path -LiteralPath $manifestDir) {
    foreach ($manifest in (Get-ChildItem -LiteralPath $manifestDir -Filter "*.item" -File)) {
        try {
            $item = Get-Content -LiteralPath $manifest.FullName -Raw | ConvertFrom-Json
            if ($item.InstallLocation) {
                $candidates.Add([string]$item.InstallLocation)
            }
        } catch {}
    }
}

switch ($Purpose) {
    "Build"   { $requiredRelativePath = "Engine\Build\BatchFiles\Build.bat" }
    "Editor"  { $requiredRelativePath = "Engine\Binaries\Win64\UnrealEditor.exe" }
    "Package" { $requiredRelativePath = "Engine\Build\BatchFiles\RunUAT.bat" }
}

$seen = @{}
foreach ($candidate in $candidates) {
    if ([string]::IsNullOrWhiteSpace($candidate)) { continue }
    try {
        $fullPath = [System.IO.Path]::GetFullPath($candidate)
    } catch {
        continue
    }
    $key = $fullPath.TrimEnd('\').ToLowerInvariant()
    if ($seen.ContainsKey($key)) { continue }
    $seen[$key] = $true

    $versionFile = Join-Path $fullPath "Engine\Build\Build.version"
    $toolPath = Join-Path $fullPath $requiredRelativePath
    if (-not (Test-Path -LiteralPath $versionFile) -or -not (Test-Path -LiteralPath $toolPath)) { continue }

    try {
        $version = Get-Content -LiteralPath $versionFile -Raw | ConvertFrom-Json
        if ([int]$version.MajorVersion -ne 5 -or [int]$version.MinorVersion -ne 8) { continue }
    } catch {
        continue
    }

    # Return exactly one line, for use by the Windows .bat launchers.
    [Console]::Out.WriteLine($fullPath)
    exit 0
}

exit 1

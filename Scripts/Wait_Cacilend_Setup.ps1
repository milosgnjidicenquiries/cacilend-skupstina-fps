param(
    [Parameter(Mandatory = $true)]
    [string]$StatusFile,
    [ValidateRange(10, 3600)]
    [int]$TimeoutSeconds = 900
)

$ErrorActionPreference = "Continue"
$deadline = (Get-Date).AddSeconds($TimeoutSeconds)

while ((Get-Date) -lt $deadline) {
    if (Test-Path -LiteralPath $StatusFile) {
        try {
            $lines = @(Get-Content -LiteralPath $StatusFile -ErrorAction Stop)
            if ($lines.Count -gt 0) {
                $status = ([string]$lines[0]).Trim()
                if ($status -eq "SUCCESS") {
                    Write-Host "Unreal Editor setup completed successfully."
                    if ($lines.Count -gt 1) { Write-Host ($lines[1..($lines.Count - 1)] -join " ") }
                    exit 0
                }
                if ($status -eq "FAILED") {
                    Write-Host "Unreal Editor setup reported a failure:"
                    if ($lines.Count -gt 1) { Write-Host ($lines[1..($lines.Count - 1)] -join " ") }
                    exit 1
                }
            }
        } catch {
            # The UE Python script may be writing the marker at this moment.
        }
    }
    Start-Sleep -Seconds 2
}

Write-Error "Timed out waiting for UE setup marker: $StatusFile. Inspect the Unreal Editor log under Saved\Logs."
exit 2

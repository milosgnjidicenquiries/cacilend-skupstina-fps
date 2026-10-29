# Windows UE5.8 build and packaging — Serbian notes

## Recommended path for a first build

Use the local Windows launchers from the root of the repository:

1. `Check_Cacilend_UE5_Requirements.bat` — verifies local prerequisites and writes `Build/UE5-preflight-report.txt`.
2. `Build_Run_Cacilend_UE5.bat` — checks prerequisites again, compiles `CacilendUEEditor`, starts Editor setup and waits for the setup result marker.
3. In Unreal Editor, open `/Game/Maps/L_Cacilend`, press **Play**, and test controls/AI/collision.
4. Only after the Editor gameplay test, run `Package_Windows.bat`. It checks the setup marker and saved `Content/Maps/L_Cacilend.umap` before invoking UAT.

Diagnostics:
- `Build/UE5-preflight-report.txt` — detected machine/toolchain prerequisites.
- `Build/UE5-build.log` — complete C++ compilation output.
- `Build/UE5-setup-status.txt` — `SUCCESS` / `FAILED` marker from the Editor Python bootstrap.
- Unreal Editor logs under `Saved/Logs`.
- `Build/UE5-package.log` — complete UAT packaging output.

## Why the GitHub package workflow is manual

`.github/workflows/validate-source.yml` runs on hosted Windows runners and checks source-level contracts, script syntax and archive creation. It does **not** compile the Unreal C++ module.

`.github/workflows/windows-ue5-build.yml` is a separate manual workflow, because Unreal Engine 5.8 and its required build environment must be installed and authorized on a Windows runner. It uses these labels:

`self-hosted`, `windows`, `x64`, `ue5-8`

Without a self-hosted runner carrying all four labels, the job will stay queued and will not build an EXE.

## Register a self-hosted runner (optional)

Only do this on a Windows machine you control, with UE 5.8, compatible Visual Studio C++ tools and the Windows SDK installed.

1. Open the repository on GitHub and go to **Settings → Actions → Runners → New self-hosted runner**.
2. Select **Windows → x64** and follow GitHub's displayed commands to download, configure and verify the runner.
3. Add the custom label `ue5-8` when configuring the runner, or add it to that runner in the GitHub Settings page.
4. Make sure the runner's user account can read the UE installation, write to the workspace and use the installed Visual Studio toolchain.
5. In **Actions**, select **Package Windows build (self-hosted UE5.8 runner)** and click **Run workflow**.

This workflow is triggered only by manual `workflow_dispatch`; it does not automatically execute for pull requests. A self-hosted runner is still a machine that executes repository workflow code, so do not reuse a sensitive production machine, do not run untrusted workflow changes, and keep runner access limited to trusted repository collaborators.

If the job succeeds, it uploads:
- `Cacilend-Skupstina-Windows-Shipping` — packaged Windows game.
- `Cacilend-UE5-Build-Diagnostics` — logs and preflight report.

## Current limitation

The repository has passed automated static/regression checks, but a successful UnrealBuildTool compilation, Editor gameplay test, Windows cook/package and launched executable still need to be confirmed on an actual UE 5.8 Windows environment. Do not describe an EXE as verified until the self-hosted workflow or local build completes successfully and the packaged game has been launched.

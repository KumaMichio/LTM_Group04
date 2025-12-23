# Qt6 Deployment Issue - Fix

## Problem
When running `MillionaireClient.exe`, you get error:
```
The code execution cannot proceed because Qt6Core.dll was not found.
```

## Solution

### Method 1: Use Deployment Script (Recommended)

Run this script once to deploy all Qt libraries:

```powershell
.\deploy_qt.ps1
```

This will copy all necessary Qt6 DLLs and plugins to the build directory.

### Method 2: Manual Deployment

If the script doesn't work, you can manually deploy:

1. **Find your Qt installation** (usually one of these):
   - `C:\Qt\6.10.1\mingw_64\bin\windeployqt.exe`
   - `C:\Qt\6.10.0\mingw_64\bin\windeployqt.exe`
   - `C:\Qt\6.9.2\mingw_64\bin\windeployqt.exe`

2. **Run windeployqt**:
   ```powershell
   C:\Qt\6.10.1\mingw_64\bin\windeployqt.exe `
     .\build\Desktop_Qt_6_10_1_MinGW_64_bit-Debug\MillionaireClient.exe
   ```

3. **Verify deployment**:
   - Check that `build\Desktop_Qt_6_10_1_MinGW_64_bit-Debug\` now contains:
     - `Qt6Core.dll`
     - `Qt6Gui.dll`
     - `Qt6Widgets.dll`
     - `Qt6Network.dll`
     - `plugins/` directory

### Method 3: Add Qt to System PATH

Alternatively, add Qt bin directory to your Windows PATH:

1. Open Environment Variables (Win+R → `sysdm.cpl`)
2. Click "Environment Variables"
3. Under "System variables", click "Path" → "Edit"
4. Add: `C:\Qt\6.10.1\mingw_64\bin`
5. Click OK and restart your computer

Then the executable should find the DLLs automatically.

---

## After Deployment

Once deployed, you can run:

```powershell
.\test_1vn_2clients.ps1
```

Or run the exe directly:

```powershell
.\build\Desktop_Qt_6_10_1_MinGW_64_bit-Debug\MillionaireClient.exe
```

---

## Troubleshooting

### Still getting DLL errors?

1. **Check if deployment worked**:
   ```powershell
   ls .\build\Desktop_Qt_6_10_1_MinGW_64_bit-Debug\Qt6*.dll
   ```

2. **If files missing, try with debug flag**:
   ```powershell
   C:\Qt\6.10.1\mingw_64\bin\windeployqt.exe `
     --debug `
     .\build\Desktop_Qt_6_10_1_MinGW_64_bit-Debug\MillionaireClient.exe
   ```

3. **Check plugins folder**:
   ```powershell
   ls .\build\Desktop_Qt_6_10_1_MinGW_64_bit-Debug\plugins\
   ```

### Different Qt Version?

If you have a different Qt version, find it:

```powershell
ls C:\Qt\
```

Then update the deployment script path accordingly.

---

## Note for Developers

When building in Qt Creator, you might want to add a post-build step to automatically deploy:

In Qt Creator:
1. Go to Projects → Build Settings
2. Under "Build Steps", find the build configuration
3. Add a post-build step:
   ```
   C:\Qt\6.10.1\mingw_64\bin\windeployqt.exe %{buildDir}/MillionaireClient.exe
   ```

This ensures the executable is always properly deployed after each build.

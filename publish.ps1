$VERSION_MAJOR = "1"
$VERSION_MINOR = "0"
$VERSION_PATCH = "5"
$VERSION_BUILD = "0"

$versionsmall = "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
$platform = "win32, win64"

function fReplace ($filePath, $strold, $strnew) {
    (Get-Content $filePath) `
        -replace $strold, $strnew |
    Set-Content $filePath
}

# Define the path to vswhere.exe
$vswherePath = 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe'

# Check if vswhere.exe exists
if (Test-Path $vswherePath) {
    # Invoke vswhere.exe and get the installation path of Visual Studio 2022 with C++ tools
    $installPath = & $vswherePath -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath

    # Check if vswhere.exe returned a valid installation path
    if ($installPath) {
        # Import the Microsoft.VisualStudio.DevShell module
        Import-Module (Join-Path $installPath 'Common7\Tools\Microsoft.VisualStudio.DevShell.dll')

        # Enter the Visual Studio developer shell
        Enter-VsDevShell -VsInstallPath $installPath -SkipAutomaticLocation
    }
    else {
        # Write an error message and exit
        Write-Error 'Could not find Visual Studio 2022 with C++ tools installed.'
        exit 1
    }
}
else {
    # Write an error message and exit
    Write-Error 'Could not find vswhere.exe in the specified location.'
    exit 1
}

Set-Location -Path $PSScriptRoot

# Build the solution for Win32 and x64 platforms
MSBuild.exe dockwidget_plugin.sln /p:Configuration=Release /p:Platform="Win32" /p:VERSION_MAJOR=${VERSION_MAJOR} /p:VERSION_MINOR=${VERSION_MINOR} /p:VERSION_PATCH=${VERSION_PATCH} /p:VERSION_BUILD=${VERSION_BUILD}
MSBuild.exe dockwidget_plugin.sln /p:Configuration=Release /p:Platform="x64" /p:VERSION_MAJOR=${VERSION_MAJOR} /p:VERSION_MINOR=${VERSION_MINOR} /p:VERSION_PATCH=${VERSION_PATCH} /p:VERSION_BUILD=${VERSION_BUILD}

fReplace ".\Publish\data\package.ini" "__VERSION__" "${versionsmall}"
fReplace ".\Publish\data\package.ini" "__PLATFORM__"  "${platform}"

$pluginpath = ".\Publish\dockwidget-${versionsmall}.ts3_plugin"

Write-Host ""
Write-Host "Checking ${pluginpath}"

if (Test-Path $pluginpath) {
    # Delete the old zip file
    Write-Warning "Removing ${pluginpath}"
    Remove-Item ${pluginpath}
}

# Create a new zip file with 7-Zip
.\Tools\7zip\x64\7za.exe -tzip a .\Publish\dockwidget-${versionsmall}.ts3_plugin .\Publish\data\*
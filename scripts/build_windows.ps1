param(
    [string]$Gcc = 'gcc',
    [string]$SdkRoot = 'build/platform-validation/windows-sdk',
    [string]$BuildDir = 'build/windows-native'
)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
Push-Location $root
try {
    $compiler = (Get-Command $Gcc -ErrorAction Stop).Source
    $sdk = Resolve-Path -LiteralPath $SdkRoot -Relative
    New-Item -ItemType Directory -Force $BuildDir | Out-Null
    # Binutils do MinGW pode corromper acentos em caminhos absolutos.
    # Argumentos relativos mantem a compilacao funcional nesta pasta com acentos.
    $output = Resolve-Path -LiteralPath $BuildDir -Relative
    $packages = @('SDL3-3.4.16', 'SDL3_image-3.4.6', 'SDL3_ttf-3.2.2')
    $flags = @('-std=c99', '-Wall', '-Wextra', '-Isrc')
    $libraries = @()
    foreach ($package in $packages) {
        $prefix = Join-Path $sdk "$package/x86_64-w64-mingw32"
        if (!(Test-Path -LiteralPath "$prefix/include")) { throw "SDK ausente: $prefix" }
        $flags += "-I$prefix/include"
        $libraries += "-L$prefix/lib"
        Copy-Item "$prefix/bin/*.dll" $output
        Copy-Item "$prefix/share/licenses" $output -Recurse -Force
    }
    $libraries += @('-lSDL3_ttf', '-lSDL3_image', '-lSDL3', '-static-libgcc', '-mconsole')
    $sources = @(Get-ChildItem src/image/*.c,src/ui/*.c | Sort-Object FullName | ForEach-Object { Resolve-Path -LiteralPath $_.FullName -Relative })
    $targets = [ordered]@{ programa = @('src/main.c') + $sources }
    foreach ($name in @('histogram', 'ui', 'equalization', 'resolution', 'save')) {
        $targets["test_$name"] = @("tests/test_$name.c") + $sources
    }
    & $compiler --version
    foreach ($target in $targets.Keys) {
        Write-Output "Compilando $target"
        & $compiler @flags @($targets[$target]) @libraries -o "$output/$target.exe"
        if ($LASTEXITCODE -ne 0) { throw "Falha ao compilar $target (exit $LASTEXITCODE)" }
    }
    Copy-Item assets $output -Recurse -Force
    Copy-Item kodim23.png $output -Force
    Copy-Item scripts/VALIDAR_WINDOWS.cmd $output -Force
    Write-Output "Build concluido: $output"
    Write-Output 'Execute VALIDAR_WINDOWS.cmd --no-pause nessa pasta para testar.'
} finally {
    Pop-Location
}

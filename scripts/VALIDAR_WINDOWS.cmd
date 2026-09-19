@echo off
setlocal
chcp 65001 >nul
cd /d "%~dp0"
if not exist logs mkdir logs
echo RUNNING > logs\result.txt
ver > logs\environment.txt
echo PROCESSOR_ARCHITECTURE=%PROCESSOR_ARCHITECTURE% >> logs\environment.txt
if not exist logs\save-test mkdir logs\save-test
pushd logs\save-test
..\..\test_save.exe > ..\save.log 2>&1
set "test_result=%errorlevel%"
popd
if not "%test_result%"=="0" goto fail
test_histogram.exe > logs\histogram.log 2>&1
if not "%errorlevel%"=="0" goto fail
test_ui.exe kodim23.png logs\ui.png > logs\ui.log 2>&1
if not "%errorlevel%"=="0" goto fail
test_equalization.exe kodim23.png logs\equalized.png > logs\equalization.log 2>&1
if not "%errorlevel%"=="0" goto fail
test_resolution.exe > logs\resolution.log 2>&1
if not "%errorlevel%"=="0" goto fail
pushd logs
..\test_ui.exe ..\kodim23.png font-outside-project.png > font.log 2>&1
set "test_result=%errorlevel%"
popd
if not "%test_result%"=="0" goto fail
test_resolution.exe kodim23.png > logs\windows.log 2>&1
if not "%errorlevel%"=="0" goto fail
echo PASS: todos os testes automaticos > logs\result.txt
type logs\result.txt
echo Confira tambem o roteiro manual em docs/VALIDACAO_MULTIPLATAFORMA.md no projeto.
if /i not "%~1"=="--no-pause" pause
exit /b 0
:fail
echo FAIL: consulte os arquivos em logs > logs\result.txt
type logs\result.txt
if /i not "%~1"=="--no-pause" pause
exit /b 1

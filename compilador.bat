@echo off
cd /d "%~dp0"

echo Compilando...
echo.

g++ main.cpp -o sistema_windows.exe -mwindows -static

if errorlevel 1 (
    echo.
    echo Erro ao compilar.
    pause
    exit /b 1
)

echo.
echo Compilado com sucesso!
echo Abrindo sistema...
echo.

start "" sistema_windows.exe

pause
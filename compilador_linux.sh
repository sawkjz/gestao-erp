#!/bin/bash
x86_64-w64-mingw32-g++ main.cpp -o sistema_windows.exe -mwindows -static
if [ $? -eq 0 ]; then
  echo "EXE Windows gerado: sistema_windows.exe"
else
  echo "Nao foi possivel compilar. Este Linux precisa ter x86_64-w64-mingw32-g++ instalado."
fi

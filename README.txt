VERSAO WINDOWS - C++ RAIZ COM JANELA NATIVA

Esta versao usa WinAPI puro.
Nao usa Qt, wxWidgets, SDL, banco instalado nem biblioteca externa.
Depois de compilado, o sistema_windows.exe abre direto no Windows.

Como compilar no Windows, se tiver g++/MinGW:
g++ main.cpp -o sistema_windows.exe -mwindows -static

Ou de dois cliques em:
compilar_windows.bat

Como compilar no Linux para gerar EXE de Windows:
./compilar_no_linux_para_windows.sh

Observacao sincera:
Para gerar o .exe de Windows a partir do Linux, o Linux precisa ter o compilador x86_64-w64-mingw32-g++.
Nesta maquina onde o arquivo foi montado, esse compilador nao estava disponivel, entao deixei a fonte e os scripts prontos.

O arquivo clientes.bin sera criado automaticamente na mesma pasta do .exe.
Ele funciona como o banco de dados local/binario.

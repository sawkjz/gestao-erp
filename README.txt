SISTEMA DE SOLICITACOES POR CATEGORIA

Aplicacao desktop para Windows desenvolvida em C++ puro, com interface grafica nativa feita em WinAPI.

O sistema organiza solicitacoes internas por categoria, permitindo que usuarios registrem pedidos de itens e que um usuario coordenador acompanhe, gerencie e audite as informacoes cadastradas.

FUNCIONALIDADES

- Login por usuario cadastrado.
- Usuario coordenador com acesso administrativo.
- Cadastro de usuarios vinculados a uma categoria.
- Cadastro, abertura e exclusao de categorias.
- Registro de solicitacoes com produto, quantidade, unidade de medida e descricao.
- Listagem de solicitacoes por categoria.
- Exclusao de solicitacoes selecionadas.
- Alteracao de senha de usuarios pelo coordenador.
- Exclusao de usuarios pelo coordenador.
- Auditoria de tentativas incorretas de acesso ao usuario coordenador.
- Manual de uso simples dentro da propria interface.

PERFIS DE ACESSO

O coordenador acessa o painel administrativo, onde pode gerenciar categorias, usuarios, senhas, exclusoes e auditoria.

Usuarios comuns acessam diretamente a tela da propria categoria, onde podem cadastrar, listar e excluir solicitacoes.

ESTRUTURA DE DADOS

O projeto nao utiliza banco de dados instalado. As informacoes sao armazenadas localmente em arquivos dentro da pasta dados.

- dados/usuarios.bin: usuarios cadastrados.
- dados/categorias/<categoria>/solicitacoes.bin: solicitacoes de cada categoria.
- dados/categorias/<categoria>/nextid.txt: controle do proximo ID de solicitacao.
- dados/auditoria_login_coordenador.txt: registros de tentativas incorretas de acesso ao coordenador.

CATEGORIAS PADRAO

O sistema trabalha inicialmente com as categorias Cozinha, Tecnologia e Papelaria. Novas categorias tambem podem ser criadas pelo coordenador.

TECNOLOGIAS UTILIZADAS

- C++
- WinAPI
- MinGW/g++
- Arquivos binarios locais

COMO COMPILAR NO WINDOWS

Com o g++/MinGW instalado, execute:

g++ main.cpp -o sistema_windows.exe -mwindows -static

Ou execute o arquivo:

compilador.bat

COMO COMPILAR NO LINUX PARA GERAR EXE DE WINDOWS

Execute:

./compilador_linux.sh

Para gerar o executavel Windows a partir do Linux, e necessario ter o compilador x86_64-w64-mingw32-g++ instalado.

EXECUCAO

Depois de compilado, abra o arquivo sistema_windows.exe no Windows.

Na primeira execucao, o sistema cria automaticamente as pastas e arquivos necessarios dentro de dados.

AUTORA

@sawkjz

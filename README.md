# <em>Sistema de Solicitações por Categoria</em>

Aplicação desktop para Windows desenvolvida em **C++ puro**, com interface gráfica nativa feita em **WinAPI**.

O sistema organiza solicitações internas por categoria, permitindo que usuários registrem pedidos de itens e que um usuário coordenador acompanhe, gerencie e audite as informações cadastradas.

## Funcionalidades

- Login por usuário cadastrado.
- Usuário coordenador com acesso administrativo.
- Cadastro de usuários vinculados a uma categoria.
- Cadastro, abertura e exclusão de categorias.
- Registro de solicitações com produto, quantidade, unidade de medida e descrição.
- Listagem de solicitações por categoria.
- Exclusão de solicitações selecionadas.
- Alteração de senha de usuários pelo coordenador.
- Exclusão de usuários pelo coordenador.
- Auditoria de tentativas incorretas de acesso ao usuário coordenador.
- Manual de uso simples dentro da própria interface.

## Perfis de Acesso

O **coordenador** acessa o painel administrativo, onde pode gerenciar categorias, usuários, senhas, exclusões e auditoria.

Os **usuários comuns** acessam diretamente a tela da própria categoria, onde podem cadastrar, listar e excluir solicitações.

## Estrutura de Dados

O projeto não utiliza banco de dados instalado. As informações são armazenadas localmente em arquivos dentro da pasta `dados`.

| Arquivo | Finalidade |
|---|---|
| `dados/usuarios.bin` | Armazena os usuários cadastrados. |
| `dados/categorias/<categoria>/solicitacoes.bin` | Armazena as solicitações de cada categoria. |
| `dados/categorias/<categoria>/nextid.txt` | Controla o próximo ID de solicitação. |
| `dados/auditoria_login_coordenador.txt` | Registra tentativas incorretas de acesso ao coordenador. |

## Categorias Padrão

O sistema trabalha inicialmente com as categorias:

- Cozinha
- Tecnologia
- Papelaria

Novas categorias também podem ser criadas pelo coordenador.

## Tecnologias Utilizadas

- C++
- WinAPI
- MinGW/g++
- Arquivos binários locais

## Compatibilidade

O sistema foi desenvolvido para execução no Windows, mas o projeto também possui suporte de compilação pelo Linux para gerar o executável `.exe` de Windows.

## Como Compilar no Windows

Com o `g++`/MinGW instalado, execute:

```bash
g++ main.cpp -o sistema_windows.exe -mwindows -static
```

Ou execute o arquivo:

```text
compilador.bat
```

## Como Compilar no Linux

Execute:

```bash
./compilador_linux.sh
```

Esse script gera o executável `sistema_windows.exe` para Windows a partir do Linux. Para isso, é necessário ter o compilador `x86_64-w64-mingw32-g++` instalado.

## Execução

Depois de compilado, abra o arquivo:

```text
sistema_windows.exe
```

Na primeira execução, o sistema cria automaticamente as pastas e arquivos necessários dentro de `dados`.

## Observação

Este projeto foi desenvolvido como último projeto para a conclusão da faculdade de Análise e Desenvolvimento de Sistemas.

## Autora

[@sawkjz](https://github.com/sawkjz)

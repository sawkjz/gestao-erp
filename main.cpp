#include <windows.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <direct.h>
#include <cstring>

using namespace std;

#define ID_LOGIN_USUARIO 101
#define ID_LOGIN_SENHA 102
#define ID_BTN_ENTRAR 103
#define ID_LISTA_USUARIOS_LOGIN 104
#define ID_LOGIN_IDENTIFICACAO 105

#define ID_LISTA_CATEGORIAS 201
#define ID_BTN_CRIAR_CATEGORIA 202
#define ID_BTN_CRIAR_USUARIO 203
#define ID_BTN_ABRIR_CATEGORIA 204
#define ID_BTN_AUDITORIA 205
#define ID_BTN_SAIR 206

#define ID_PRODUTO 301
#define ID_QUANTIDADE 302
#define ID_DESCRICAO 303
#define ID_BTN_SALVAR_SOLICITACAO 304
#define ID_BTN_LISTAR_SOLICITACOES 305
#define ID_LISTA_SOLICITACOES 306
#define ID_BTN_VOLTAR 307
#define ID_UNIDADE_MEDIDA 308

#define ID_NOVA_CATEGORIA 401
#define ID_BTN_CONFIRMAR_CATEGORIA 402

#define ID_NOVO_LOGIN 501
#define ID_NOVA_SENHA 502
#define ID_CATEGORIA_USUARIO 503
#define ID_BTN_CONFIRMAR_USUARIO 504

struct Usuario {
    char login[50];
    char senha[50];
    char categoria[50];
    int coordenador;
};

struct Solicitacao {
    int id;
    char produto[80];
    int quantidade;
    char unidadeMedida[30];
    char descricao[200];
    char criadoEm[30];
    char criadoPor[50];
};

const char* PASTA_DADOS = "dados";
const char* ARQUIVO_USUARIOS = "dados\\usuarios.bin";
const char* ARQUIVO_AUDITORIA_COORDENADOR = "dados\\auditoria_login_coordenador.txt";

HWND hLoginUsuario, hLoginSenha;
HWND hListaCategorias;
HWND hProduto, hQuantidade, hUnidadeMedida, hDescricao, hListaSolicitacoes;
HWND hNovaCategoria, hNovoLogin, hNovaSenha, hCategoriaUsuario;
HWND hListaUsuariosLogin, hLoginIdentificacao;

Usuario usuarioLogado;
string categoriaAtual = "";
int telaAtual = 0;
// 0 = login
// 1 = painel coordenador
// 2 = tela solicitacoes
// 3 = criar categoria
// 4 = criar usuario
// 5 = auditoria

string getTexto(HWND campo) {
    int tamanho = GetWindowTextLengthA(campo) + 1;
    char* buffer = new char[tamanho];
    GetWindowTextA(campo, buffer, tamanho);
    string texto(buffer);
    delete[] buffer;
    return texto;
}

void setTexto(HWND campo, const char* texto) {
    SetWindowTextA(campo, texto);
}

void limparJanela(HWND hwnd) {
    HWND filho = GetWindow(hwnd, GW_CHILD);

    while (filho) {
        HWND proximo = GetWindow(filho, GW_HWNDNEXT);
        DestroyWindow(filho);
        filho = proximo;
    }
}

void criarPasta(const string& caminho) {
    _mkdir(caminho.c_str());
}

void prepararPastas() {
    criarPasta(PASTA_DADOS);
    criarPasta("dados\\categorias");
}

string caminhoCategoria(const string& categoria) {
    return "dados\\categorias\\" + categoria;
}

string arquivoSolicitacoes(const string& categoria) {
    return caminhoCategoria(categoria) + "\\solicitacoes.bin";
}

string arquivoNextId(const string& categoria) {
    return caminhoCategoria(categoria) + "\\nextid.txt";
}

string dataHoraAtual() {
    time_t agora = time(NULL);
    tm* t = localtime(&agora);

    char buffer[30];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", t);

    return string(buffer);
}

vector<Usuario> carregarUsuariosCadastrados() {
    vector<Usuario> usuarios;
    ifstream arquivo(ARQUIVO_USUARIOS, ios::binary);
    Usuario u;

    while (arquivo.read(reinterpret_cast<char*>(&u), sizeof(Usuario))) {
        usuarios.push_back(u);
    }

    arquivo.close();
    return usuarios;
}

void salvarUsuariosCadastrados(const vector<Usuario>& usuarios) {
    ofstream arquivo(ARQUIVO_USUARIOS, ios::binary | ios::trunc);

    for (size_t i = 0; i < usuarios.size(); i++) {
        arquivo.write(reinterpret_cast<const char*>(&usuarios[i]), sizeof(Usuario));
    }

    arquivo.close();
}

void criarCoordenadorInicial() {
    prepararPastas();
    vector<Usuario> usuarios = carregarUsuariosCadastrados();
    bool encontrouCoordenador = false;

    for (size_t i = 0; i < usuarios.size(); i++) {
        if (string(usuarios[i].login) == "coordenador") {
            memset(usuarios[i].senha, 0, sizeof(usuarios[i].senha));
            memset(usuarios[i].categoria, 0, sizeof(usuarios[i].categoria));
            strncpy(usuarios[i].senha, "26!SenacIM", sizeof(usuarios[i].senha) - 1);
            strncpy(usuarios[i].categoria, "COORDENACAO", sizeof(usuarios[i].categoria) - 1);
            usuarios[i].coordenador = 1;
            encontrouCoordenador = true;
            break;
        }
    }

    if (!encontrouCoordenador) {
        Usuario u;
        memset(&u, 0, sizeof(Usuario));
        strncpy(u.login, "coordenador", sizeof(u.login) - 1);
        strncpy(u.senha, "26!SenacIM", sizeof(u.senha) - 1);
        strncpy(u.categoria, "COORDENACAO", sizeof(u.categoria) - 1);
        u.coordenador = 1;
        usuarios.push_back(u);
    }

    salvarUsuariosCadastrados(usuarios);
}

bool usuarioExiste(const string& login) {
    ifstream arquivo(ARQUIVO_USUARIOS, ios::binary);
    Usuario u;

    while (arquivo.read(reinterpret_cast<char*>(&u), sizeof(Usuario))) {
        if (login == u.login) {
            arquivo.close();
            return true;
        }
    }

    arquivo.close();
    return false;
}

bool validarLogin(const string& login, const string& senha, Usuario& encontrado) {
    ifstream arquivo(ARQUIVO_USUARIOS, ios::binary);
    Usuario u;

    while (arquivo.read(reinterpret_cast<char*>(&u), sizeof(Usuario))) {
        if (login == u.login && senha == u.senha) {
            encontrado = u;
            arquivo.close();
            return true;
        }
    }

    arquivo.close();
    return false;
}

void registrarTentativaCoordenador(const string& loginInformado) {
    ofstream arquivo(ARQUIVO_AUDITORIA_COORDENADOR, ios::app);

    if (!arquivo) {
        return;
    }

    string loginRegistrado = loginInformado.empty() ? "nao_informado" : loginInformado;
    arquivo << dataHoraAtual()
            << " | tentativa de acesso ao coordenador | login informado: "
            << loginRegistrado
            << "\n";
    arquivo.close();
}

vector<string> carregarCategoriasParaUsuario(const Usuario& usuario) {
    vector<string> categorias;

    if (usuario.coordenador) {
        categorias.push_back("Cozinha");
        categorias.push_back("Tecnologia");
        categorias.push_back("Papelaria");
    } else if (strlen(usuario.categoria) > 0) {
        categorias.push_back(usuario.categoria);
    }

    return categorias;
}

void adicionarLista(HWND lista, const string& texto) {
    SendMessageA(lista, LB_ADDSTRING, 0, (LPARAM)texto.c_str());
}

void adicionarCombo(HWND combo, const string& texto) {
    SendMessageA(combo, CB_ADDSTRING, 0, (LPARAM)texto.c_str());
}

string getTextoComboSelecionado(HWND combo) {
    int indice = (int)SendMessageA(combo, CB_GETCURSEL, 0, 0);

    if (indice == CB_ERR) {
        return "";
    }

    char buffer[100] = {0};
    SendMessageA(combo, CB_GETLBTEXT, indice, (LPARAM)buffer);
    return string(buffer);
}

vector<string> carregarUnidadesMedidaPorCategoria(const string& categoria) {
    vector<string> unidades;

    if (categoria == "Cozinha") {
        unidades.push_back("QUILOS");
        unidades.push_back("GRAMAS");
        unidades.push_back("UNIDADE");
        unidades.push_back("MILILITROS");
        unidades.push_back("LITROS");
        unidades.push_back("PACOTE");
        unidades.push_back("CAIXA");
        unidades.push_back("LATA");
        unidades.push_back("GARRAFA");
        unidades.push_back("SACO");
    } else if (categoria == "Tecnologia") {
        unidades.push_back("UNIDADE");
        unidades.push_back("KIT");
        unidades.push_back("CAIXA");
        unidades.push_back("PACOTE");
        unidades.push_back("METRO");
        unidades.push_back("ROLO");
        unidades.push_back("CARTUCHO");
        unidades.push_back("TONER");
        unidades.push_back("CABO");
        unidades.push_back("PECA");
    } else if (categoria == "Papelaria") {
        unidades.push_back("UNIDADE");
        unidades.push_back("PACOTE");
        unidades.push_back("CAIXA");
        unidades.push_back("BLOCO");
        unidades.push_back("CADERNO");
        unidades.push_back("ROLO");
        unidades.push_back("FOLHA");
        unidades.push_back("RESMA");
        unidades.push_back("CARTELA");
        unidades.push_back("TUBO");
        unidades.push_back("FRASCO");
        unidades.push_back("POTE");
        unidades.push_back("KIT");
    }

    return unidades;
}

void carregarListaUsuariosLogin() {
    SendMessageA(hListaUsuariosLogin, LB_RESETCONTENT, 0, 0);

    vector<Usuario> usuarios = carregarUsuariosCadastrados();

    for (size_t i = 0; i < usuarios.size(); i++) {
        stringstream ss;
        ss << (i + 1) << " - " << usuarios[i].login;
        adicionarLista(hListaUsuariosLogin, ss.str());
    }
}

bool obterUsuarioSelecionadoLogin(Usuario& usuarioSelecionado) {
    int indice = (int)SendMessageA(hListaUsuariosLogin, LB_GETCURSEL, 0, 0);

    if (indice == LB_ERR) {
        return false;
    }

    vector<Usuario> usuarios = carregarUsuariosCadastrados();

    if (indice < 0 || indice >= (int)usuarios.size()) {
        return false;
    }

    usuarioSelecionado = usuarios[indice];
    return true;
}

void carregarListaCategorias() {
    SendMessageA(hListaCategorias, LB_RESETCONTENT, 0, 0);

    vector<string> categorias = carregarCategoriasParaUsuario(usuarioLogado);

    if (categorias.empty()) {
        adicionarLista(hListaCategorias, "Nenhuma categoria criada.");
        return;
    }

    for (size_t i = 0; i < categorias.size(); i++) {
        adicionarLista(hListaCategorias, categorias[i]);
    }
}

int proximoIdCategoria(const string& categoria) {
    criarPasta(caminhoCategoria(categoria));

    int id = 1;
    ifstream entrada(arquivoNextId(categoria).c_str());

    if (entrada) {
        entrada >> id;
        entrada.close();
    }

    ofstream saida(arquivoNextId(categoria).c_str());
    saida << (id + 1);
    saida.close();

    return id;
}

void abrirTelaLogin(HWND hwnd);

void abrirPainelCoordenador(HWND hwnd) {
    telaAtual = 1;
    categoriaAtual = "";

    limparJanela(hwnd);

    CreateWindowA("STATIC", "Painel do Coordenador", WS_VISIBLE | WS_CHILD, 20, 20, 250, 20, hwnd, NULL, NULL, NULL);
    CreateWindowA("STATIC", "Categorias:", WS_VISIBLE | WS_CHILD, 20, 60, 120, 20, hwnd, NULL, NULL, NULL);

    hListaCategorias = CreateWindowA("LISTBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL, 20, 85, 350, 250, hwnd, (HMENU)ID_LISTA_CATEGORIAS, NULL, NULL);

    CreateWindowA("BUTTON", "Criar Categoria", WS_VISIBLE | WS_CHILD, 400, 85, 160, 30, hwnd, (HMENU)ID_BTN_CRIAR_CATEGORIA, NULL, NULL);
    CreateWindowA("BUTTON", "Criar Usuario", WS_VISIBLE | WS_CHILD, 400, 125, 160, 30, hwnd, (HMENU)ID_BTN_CRIAR_USUARIO, NULL, NULL);
    CreateWindowA("BUTTON", "Abrir Categoria", WS_VISIBLE | WS_CHILD, 400, 165, 160, 30, hwnd, (HMENU)ID_BTN_ABRIR_CATEGORIA, NULL, NULL);
    CreateWindowA("BUTTON", "Auditoria", WS_VISIBLE | WS_CHILD, 400, 205, 160, 30, hwnd, (HMENU)ID_BTN_AUDITORIA, NULL, NULL);
    CreateWindowA("BUTTON", "Sair", WS_VISIBLE | WS_CHILD, 400, 245, 160, 30, hwnd, (HMENU)ID_BTN_SAIR, NULL, NULL);

    carregarListaCategorias();
}

void abrirTelaSolicitacoes(HWND hwnd, const string& categoria) {
    telaAtual = 2;
    categoriaAtual = categoria;

    limparJanela(hwnd);

    string titulo = "Solicitacoes - Categoria: " + categoria;
    CreateWindowA("STATIC", titulo.c_str(), WS_VISIBLE | WS_CHILD, 20, 20, 400, 20, hwnd, NULL, NULL, NULL);

    CreateWindowA("STATIC", "Produto:", WS_VISIBLE | WS_CHILD, 20, 60, 100, 20, hwnd, NULL, NULL, NULL);
    hProduto = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 60, 300, 22, hwnd, (HMENU)ID_PRODUTO, NULL, NULL);

    CreateWindowA("STATIC", "Quantidade:", WS_VISIBLE | WS_CHILD, 20, 95, 100, 20, hwnd, NULL, NULL, NULL);
    hQuantidade = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 95, 90, 22, hwnd, (HMENU)ID_QUANTIDADE, NULL, NULL);

   CreateWindowA("STATIC", "Medida:", WS_VISIBLE | WS_CHILD, 205, 95, 100, 20, hwnd, NULL, NULL, NULL);
   hUnidadeMedida = CreateWindowA("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWNLIST, 250, 95, 110, 200, hwnd, (HMENU)ID_UNIDADE_MEDIDA, NULL, NULL);

    vector<string> unidades = carregarUnidadesMedidaPorCategoria(categoria);
    for (size_t i = 0; i < unidades.size(); i++) {
        adicionarCombo(hUnidadeMedida, unidades[i]);
    }
    if (!unidades.empty()) {
        SendMessageA(hUnidadeMedida, CB_SETCURSEL, 0, 0);
    }

    CreateWindowA("STATIC", "Descricao:", WS_VISIBLE | WS_CHILD, 20, 130, 100, 20, hwnd, NULL, NULL, NULL);
    hDescricao = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | WS_VSCROLL, 120, 130, 300, 70, hwnd, (HMENU)ID_DESCRICAO, NULL, NULL);

    CreateWindowA("BUTTON", "Salvar Solicitacao", WS_VISIBLE | WS_CHILD, 450, 60, 150, 30, hwnd, (HMENU)ID_BTN_SALVAR_SOLICITACAO, NULL, NULL);
    CreateWindowA("BUTTON", "Listar", WS_VISIBLE | WS_CHILD, 450, 100, 150, 30, hwnd, (HMENU)ID_BTN_LISTAR_SOLICITACOES, NULL, NULL);
    CreateWindowA("BUTTON", "Voltar", WS_VISIBLE | WS_CHILD, 450, 140, 150, 30, hwnd, (HMENU)ID_BTN_VOLTAR, NULL, NULL);

    hListaSolicitacoes = CreateWindowA("LISTBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL, 20, 225, 580, 170, hwnd, (HMENU)ID_LISTA_SOLICITACOES, NULL, NULL);
}

void abrirTelaCriarCategoria(HWND hwnd) {
    telaAtual = 3;

    limparJanela(hwnd);

    CreateWindowA("STATIC", "Criar Categoria", WS_VISIBLE | WS_CHILD, 20, 20, 250, 20, hwnd, NULL, NULL, NULL);

    CreateWindowA("STATIC", "Nome:", WS_VISIBLE | WS_CHILD, 20, 70, 100, 20, hwnd, NULL, NULL, NULL);
    hNovaCategoria = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 70, 250, 22, hwnd, (HMENU)ID_NOVA_CATEGORIA, NULL, NULL);

    CreateWindowA("BUTTON", "Salvar", WS_VISIBLE | WS_CHILD, 120, 115, 100, 30, hwnd, (HMENU)ID_BTN_CONFIRMAR_CATEGORIA, NULL, NULL);
    CreateWindowA("BUTTON", "Voltar", WS_VISIBLE | WS_CHILD, 230, 115, 100, 30, hwnd, (HMENU)ID_BTN_VOLTAR, NULL, NULL);
}

void abrirTelaCriarUsuario(HWND hwnd) {
    telaAtual = 4;

    limparJanela(hwnd);

    CreateWindowA("STATIC", "Criar Usuario por Categoria", WS_VISIBLE | WS_CHILD, 20, 20, 300, 20, hwnd, NULL, NULL, NULL);

    CreateWindowA("STATIC", "Login:", WS_VISIBLE | WS_CHILD, 20, 65, 100, 20, hwnd, NULL, NULL, NULL);
    hNovoLogin = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 130, 65, 220, 22, hwnd, (HMENU)ID_NOVO_LOGIN, NULL, NULL);

    CreateWindowA("STATIC", "Senha:", WS_VISIBLE | WS_CHILD, 20, 100, 100, 20, hwnd, NULL, NULL, NULL);
    hNovaSenha = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD, 130, 100, 220, 22, hwnd, (HMENU)ID_NOVA_SENHA, NULL, NULL);

    CreateWindowA("STATIC", "Categoria:", WS_VISIBLE | WS_CHILD, 20, 135, 100, 20, hwnd, NULL, NULL, NULL);
    hCategoriaUsuario = CreateWindowA("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWNLIST, 130, 135, 220, 200, hwnd, (HMENU)ID_CATEGORIA_USUARIO, NULL, NULL);
    adicionarCombo(hCategoriaUsuario, "Cozinha");
    adicionarCombo(hCategoriaUsuario, "Tecnologia");
    adicionarCombo(hCategoriaUsuario, "Papelaria");
    SendMessageA(hCategoriaUsuario, CB_SETCURSEL, 0, 0);

    CreateWindowA("BUTTON", "Salvar", WS_VISIBLE | WS_CHILD, 130, 180, 100, 30, hwnd, (HMENU)ID_BTN_CONFIRMAR_USUARIO, NULL, NULL);
    CreateWindowA("BUTTON", "Voltar", WS_VISIBLE | WS_CHILD, 240, 180, 100, 30, hwnd, (HMENU)ID_BTN_VOLTAR, NULL, NULL);
}

void abrirTelaAuditoria(HWND hwnd) {
    telaAtual = 5;

    limparJanela(hwnd);

    CreateWindowA("STATIC", "Auditoria de Solicitacoes", WS_VISIBLE | WS_CHILD, 20, 20, 300, 20, hwnd, NULL, NULL, NULL);
    CreateWindowA("BUTTON", "Voltar", WS_VISIBLE | WS_CHILD, 480, 20, 100, 30, hwnd, (HMENU)ID_BTN_VOLTAR, NULL, NULL);

    hListaSolicitacoes = CreateWindowA("LISTBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL, 20, 65, 580, 320, hwnd, (HMENU)ID_LISTA_SOLICITACOES, NULL, NULL);

    bool encontrou = false;
    ifstream auditoriaLogin(ARQUIVO_AUDITORIA_COORDENADOR);
    string linhaAuditoria;

    while (getline(auditoriaLogin, linhaAuditoria)) {
        encontrou = true;
        adicionarLista(hListaSolicitacoes, linhaAuditoria);
    }

    auditoriaLogin.close();

    vector<string> categorias = carregarCategoriasParaUsuario(usuarioLogado);

    for (size_t i = 0; i < categorias.size(); i++) {
        ifstream arquivo(arquivoSolicitacoes(categorias[i]).c_str(), ios::binary);
        Solicitacao s;

        while (arquivo.read(reinterpret_cast<char*>(&s), sizeof(Solicitacao))) {
            encontrou = true;

            stringstream ss;
            ss << "[" << categorias[i] << "] "
               << "ID: " << s.id
               << " | Produto: " << s.produto
               << " | Qtd: " << s.quantidade
               << " | Unid.: " << s.unidadeMedida
               << " | Criado em: " << s.criadoEm
               << " | Criado por: " << s.criadoPor
               << " | Desc: " << s.descricao;

            adicionarLista(hListaSolicitacoes, ss.str());
        }

        arquivo.close();
    }

    if (!encontrou) {
        adicionarLista(hListaSolicitacoes, "Nenhum registro de auditoria encontrado.");
    }
}

void salvarCategoria(HWND hwnd) {
    string categoria = getTexto(hNovaCategoria);

    if (categoria.empty()) {
        MessageBoxA(hwnd, "Informe o nome da categoria.", "Aviso", MB_OK | MB_ICONWARNING);
        return;
    }

    criarPasta(caminhoCategoria(categoria));

    MessageBoxA(hwnd, "Categoria criada com sucesso.", "Sucesso", MB_OK | MB_ICONINFORMATION);
    abrirPainelCoordenador(hwnd);
}

void salvarUsuario(HWND hwnd) {
    string login = getTexto(hNovoLogin);
    string senha = getTexto(hNovaSenha);
    string categoria = getTextoComboSelecionado(hCategoriaUsuario);

    if (login.empty() || senha.empty() || categoria.empty()) {
        MessageBoxA(hwnd, "Preencha login, senha e categoria.", "Aviso", MB_OK | MB_ICONWARNING);
        return;
    }

    if (usuarioExiste(login)) {
        MessageBoxA(hwnd, "Ja existe um usuario com esse login.", "Aviso", MB_OK | MB_ICONWARNING);
        return;
    }

    criarPasta(caminhoCategoria(categoria));

    Usuario u;
    memset(&u, 0, sizeof(Usuario));
    strncpy(u.login, login.c_str(), sizeof(u.login) - 1);
    strncpy(u.senha, senha.c_str(), sizeof(u.senha) - 1);
    strncpy(u.categoria, categoria.c_str(), sizeof(u.categoria) - 1);
    u.coordenador = 0;

    ofstream arquivo(ARQUIVO_USUARIOS, ios::binary | ios::app);
    arquivo.write(reinterpret_cast<char*>(&u), sizeof(Usuario));
    arquivo.close();

    MessageBoxA(hwnd, "Usuario criado com sucesso.", "Sucesso", MB_OK | MB_ICONINFORMATION);
    abrirPainelCoordenador(hwnd);
}

void salvarSolicitacao(HWND hwnd) {
    string produto = getTexto(hProduto);
    string quantidadeTxt = getTexto(hQuantidade);
    string unidadeMedida = getTextoComboSelecionado(hUnidadeMedida);
    string descricao = getTexto(hDescricao);

    if (produto.empty() || quantidadeTxt.empty() || unidadeMedida.empty()) {
        MessageBoxA(hwnd, "Preencha produto, quantidade e unidade de medida.", "Aviso", MB_OK | MB_ICONWARNING);
        return;
    }

    int quantidade = atoi(quantidadeTxt.c_str());

    if (quantidade <= 0) {
        MessageBoxA(hwnd, "Quantidade deve ser maior que zero.", "Aviso", MB_OK | MB_ICONWARNING);
        return;
    }

    criarPasta(caminhoCategoria(categoriaAtual));

    Solicitacao s;
    memset(&s, 0, sizeof(Solicitacao));

    s.id = proximoIdCategoria(categoriaAtual);
    strncpy(s.produto, produto.c_str(), sizeof(s.produto) - 1);
    s.quantidade = quantidade;
    strncpy(s.unidadeMedida, unidadeMedida.c_str(), sizeof(s.unidadeMedida) - 1);
    strncpy(s.descricao, descricao.c_str(), sizeof(s.descricao) - 1);
    strncpy(s.criadoEm, dataHoraAtual().c_str(), sizeof(s.criadoEm) - 1);
    strncpy(s.criadoPor, usuarioLogado.login, sizeof(s.criadoPor) - 1);

    ofstream arquivo(arquivoSolicitacoes(categoriaAtual).c_str(), ios::binary | ios::app);

    if (!arquivo) {
        MessageBoxA(hwnd, "Erro ao salvar solicitacao.", "Erro", MB_OK | MB_ICONERROR);
        return;
    }

    arquivo.write(reinterpret_cast<char*>(&s), sizeof(Solicitacao));
    arquivo.close();

    setTexto(hProduto, "");
    setTexto(hQuantidade, "");
    setTexto(hDescricao, "");
    SendMessageA(hUnidadeMedida, CB_SETCURSEL, 0, 0);

    MessageBoxA(hwnd, "Solicitacao salva com sucesso.", "Sucesso", MB_OK | MB_ICONINFORMATION);
}

void listarSolicitacoes() {
    SendMessageA(hListaSolicitacoes, LB_RESETCONTENT, 0, 0);

    ifstream arquivo(arquivoSolicitacoes(categoriaAtual).c_str(), ios::binary);
    Solicitacao s;

    if (!arquivo) {
        adicionarLista(hListaSolicitacoes, "Nenhuma solicitacao encontrada nesta categoria.");
        return;
    }

    bool encontrou = false;

    while (arquivo.read(reinterpret_cast<char*>(&s), sizeof(Solicitacao))) {
        encontrou = true;

        stringstream ss;

        if (usuarioLogado.coordenador == 1) {
            ss << "ID: " << s.id
               << " | Produto: " << s.produto
               << " | Qtd: " << s.quantidade
               << " | Unid.: " << s.unidadeMedida
               << " | Criado em: " << s.criadoEm
               << " | Criado por: " << s.criadoPor
               << " | Desc: " << s.descricao;
        } else {
            ss << "ID: " << s.id
               << " | Produto: " << s.produto
               << " | Qtd: " << s.quantidade
               << " | Unid.: " << s.unidadeMedida
               << " | Criado em: " << s.criadoEm
               << " | Desc: " << s.descricao;
        }

        adicionarLista(hListaSolicitacoes, ss.str());
    }

    arquivo.close();

    if (!encontrou) {
        adicionarLista(hListaSolicitacoes, "Nenhuma solicitacao encontrada nesta categoria.");
    }
}

void abrirTelaLogin(HWND hwnd) {
    telaAtual = 0;
    categoriaAtual = "";

    limparJanela(hwnd);

    CreateWindowA("STATIC", "Selecione o usuario desejado", WS_VISIBLE | WS_CHILD, 20, 20, 220, 20, hwnd, NULL, NULL, NULL);
    CreateWindowA("STATIC", "Usuarios cadastrados:", WS_VISIBLE | WS_CHILD, 20, 55, 150, 20, hwnd, NULL, NULL, NULL);
    hListaUsuariosLogin = CreateWindowA("LISTBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL, 20, 80, 220, 260, hwnd, (HMENU)ID_LISTA_USUARIOS_LOGIN, NULL, NULL);

    CreateWindowA("STATIC", "Seu login:", WS_VISIBLE | WS_CHILD, 280, 90, 100, 20, hwnd, NULL, NULL, NULL);
    hLoginUsuario = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 390, 90, 180, 22, hwnd, (HMENU)ID_LOGIN_IDENTIFICACAO, NULL, NULL);
    hLoginIdentificacao = hLoginUsuario;

    CreateWindowA("STATIC", "Senha:", WS_VISIBLE | WS_CHILD, 280, 130, 100, 20, hwnd, NULL, NULL, NULL);
    hLoginSenha = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD, 390, 130, 180, 22, hwnd, (HMENU)ID_LOGIN_SENHA, NULL, NULL);

    CreateWindowA("BUTTON", "Entrar", WS_VISIBLE | WS_CHILD, 390, 175, 100, 30, hwnd, (HMENU)ID_BTN_ENTRAR, NULL, NULL);

    CreateWindowA("STATIC", "Login coordenador: coordenador", WS_VISIBLE | WS_CHILD, 280, 225, 220, 20, hwnd, NULL, NULL, NULL);

    carregarListaUsuariosLogin();
}

void selecionarCategoriaCoordenador(HWND hwnd) {
    int indice = (int)SendMessageA(hListaCategorias, LB_GETCURSEL, 0, 0);

    if (indice == LB_ERR) {
        MessageBoxA(hwnd, "Selecione uma categoria.", "Aviso", MB_OK | MB_ICONWARNING);
        return;
    }

    char buffer[100];
    SendMessageA(hListaCategorias, LB_GETTEXT, indice, (LPARAM)buffer);

    string categoria = buffer;

    if (categoria == "Nenhuma categoria criada.") {
        return;
    }

    abrirTelaSolicitacoes(hwnd, categoria);
}

LRESULT CALLBACK JanelaProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            criarCoordenadorInicial();
            abrirTelaLogin(hwnd);
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_BTN_ENTRAR: {
                    Usuario usuarioSelecionado;
                    string loginInformado = getTexto(hLoginIdentificacao);
                    string senha = getTexto(hLoginSenha);

                    if (!obterUsuarioSelecionadoLogin(usuarioSelecionado)) {
                        MessageBoxA(hwnd, "Selecione um usuario na lista para entrar.", "Aviso", MB_OK | MB_ICONWARNING);
                        break;
                    }

                    if (senha.empty()) {
                        MessageBoxA(hwnd, "Informe a senha do usuario selecionado.", "Aviso", MB_OK | MB_ICONWARNING);
                        break;
                    }

                    if (usuarioSelecionado.coordenador == 1) {
                        if (senha == usuarioSelecionado.senha) {
                            usuarioLogado = usuarioSelecionado;
                            abrirPainelCoordenador(hwnd);
                        } else {
                            if (loginInformado != "coordenador") {
                                registrarTentativaCoordenador(loginInformado);
                            }

                            MessageBoxA(hwnd, "Senha incorreta. Verifique sua senha novamente", "Erro", MB_OK | MB_ICONERROR);
                            setTexto(hLoginSenha, "");
                        }
                    } else {
                        if (senha == usuarioSelecionado.senha) {
                            usuarioLogado = usuarioSelecionado;
                            abrirTelaSolicitacoes(hwnd, usuarioLogado.categoria);
                        } else {
                            MessageBoxA(hwnd, "Senha incorreta. Verifique sua senha novamente", "Erro", MB_OK | MB_ICONERROR);
                            setTexto(hLoginSenha, "");
                        }
                    }

                    break;
                }

                case ID_BTN_CRIAR_CATEGORIA:
                    if (usuarioLogado.coordenador == 1) {
                        abrirTelaCriarCategoria(hwnd);
                    }
                    break;

                case ID_BTN_CRIAR_USUARIO:
                    if (usuarioLogado.coordenador == 1) {
                        abrirTelaCriarUsuario(hwnd);
                    }
                    break;

                case ID_BTN_ABRIR_CATEGORIA:
                    if (usuarioLogado.coordenador == 1) {
                        selecionarCategoriaCoordenador(hwnd);
                    }
                    break;

                case ID_BTN_AUDITORIA:
                    if (usuarioLogado.coordenador == 1) {
                        abrirTelaAuditoria(hwnd);
                    }
                    break;

                case ID_BTN_CONFIRMAR_CATEGORIA:
                    if (usuarioLogado.coordenador == 1) {
                        salvarCategoria(hwnd);
                    }
                    break;

                case ID_BTN_CONFIRMAR_USUARIO:
                    if (usuarioLogado.coordenador == 1) {
                        salvarUsuario(hwnd);
                    }
                    break;

                case ID_BTN_SALVAR_SOLICITACAO:
                    salvarSolicitacao(hwnd);
                    break;

                case ID_BTN_LISTAR_SOLICITACOES:
                    listarSolicitacoes();
                    break;

                case ID_BTN_VOLTAR:
                    if (usuarioLogado.coordenador == 1) {
                        abrirPainelCoordenador(hwnd);
                    } else {
                        abrirTelaLogin(hwnd);
                    }
                    break;

                case ID_BTN_SAIR:
                    abrirTelaLogin(hwnd);
                    break;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProcA(hwnd, msg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    const char CLASS_NAME[] = "SistemaSolicitacoesCategorias";

    WNDCLASSA wc = {};
    wc.lpfnWndProc = JanelaProc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "Sistema de Solicitacoes por Categoria",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        650,
        470,
        NULL,
        NULL,
        hInst,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return 0;
}

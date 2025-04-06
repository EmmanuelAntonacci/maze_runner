#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <thread>
#include <chrono>
#include <mutex>

// Representação do labirinto
using Maze = std::vector<std::vector<char>>;

// Estrutura para representar uma posição no labirinto
struct Position {
    int row;
    int col;
};

// Variáveis globais
Maze maze;
int num_rows;
int num_cols;
std::stack<Position> valid_positions;
std::vector<std::thread> threads;
std::mutex mutex;

// Função para carregar o labirinto de um arquivo
Position load_maze(const std::string& file_name) {
    // TODO: Implemente esta função seguindo estes passos:
    // 1. Abra o arquivo especificado por file_name usando std::ifstream
    // 2. Leia o número de linhas e colunas do labirinto
    // 3. Redimensione a matriz 'maze' de acordo (use maze.resize())
    // 4. Leia o conteúdo do labirinto do arquivo, caractere por caractere
    // 5. Encontre e retorne a posição inicial ('e')
    // 6. Trate possíveis erros (arquivo não encontrado, formato inválido, etc.)
    // 7. Feche o arquivo após a leitura

    int space_position, point_position, size;
    int row = 0, column = 0;
    int entrance_found = 0;
    std::string line, extension, aux;
    std::ifstream inputFile;
    Position initial_position;


    point_position = file_name.find_last_of('.'); 
    size = file_name.length();
    extension = std::string(file_name.begin()+point_position, file_name.begin()+size);

    if(extension != ".txt"){
        std::cout << "Erro, extensão não suportada." << std::endl;
        return {-1,-1};
    }

    inputFile.open(file_name);

    if (!inputFile.is_open()){
        std::cout << "Erro, não foi possivel abrir o arquivo." << std::endl;
        return {-1,-1};
    }

    std::getline(inputFile, aux); //Get first line

    size = aux.length();
    space_position = aux.find(' ');

    num_rows = std::stoi(std::string(aux.begin(), aux.begin()+space_position));
    num_cols = std::stoi(std::string(aux.begin()+space_position, aux.begin()+size));


    maze.resize(num_rows);
    for(int i=0; i < num_rows; i++){
        maze[i].resize(num_cols, '#');
    }
    
    while(std::getline(inputFile, line)){
        for(column = 0; column < num_cols; column++){
            if(line[column] != '#'){
                maze[row][column] = line[column];
                if(line[column] == 'e'){
                    entrance_found = 1;
                    initial_position.row = row;
                    initial_position.col = column;
                }
            }
        }
        row++;
    }

    inputFile.close();

    if(!entrance_found) return {-1, -1};
    else return initial_position;
}

// Função para imprimir o labirinto
void print_maze() {
    // TODO: Implemente esta função
    // 1. Percorra a matriz 'maze' usando um loop aninhado
    // 2. Imprima cada caractere usando std::cout
    // 3. Adicione uma quebra de linha (std::cout << '\n') ao final de cada linha do labirinto
    for(int i = 0; i < num_rows; i++){
        for(int j = 0; j < num_cols; j++){
            std::cout << maze[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

// Função para verificar se uma posição é válida
bool is_valid_position(int row, int col) {
    // TODO: Implemente esta função
    // 1. Verifique se a posição está dentro dos limites do labirinto
    //    (row >= 0 && row < num_rows && col >= 0 && col < num_cols)
    // 2. Verifique se a posição é um caminho válido (maze[row][col] == 'x')
    // 3. Retorne true se ambas as condições forem verdadeiras, false caso contrário

    return (row >= 0 && row < num_rows && col >=0 && col < num_cols && (maze[row][col] == 'x' or maze[row][col] == 's'));
}

// Função principal para navegar pelo labirinto
bool walk(Position pos) {
    // TODO: Implemente a lógica de navegação aqui
    // 1. Marque a posição atual como visitada (maze[pos.row][pos.col] = '.')
    // 2. Chame print_maze() para mostrar o estado atual do labirinto
    // 3. Adicione um pequeno atraso para visualização:
    //    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // 4. Verifique se a posição atual é a saída (maze[pos.row][pos.col] == 's')
    //    Se for, retorne true
    // 5. Verifique as posições adjacentes (cima, baixo, esquerda, direita)
    //    Para cada posição adjacente:
    //    a. Se for uma posição válida (use is_valid_position()), adicione-a à pilha valid_positions
    // 6. Enquanto houver posições válidas na pilha (!valid_positions.empty()):
    //    a. Remova a próxima posição da pilha (valid_positions.top() e valid_positions.pop())
    //    b. Chame walk recursivamente para esta posição
    //    c. Se walk retornar true, propague o retorno (retorne true)
    // 7. Se todas as posições foram exploradas sem encontrar a saída, retorne false
    
    Position next_position = {0, 0};
    Position next_move = {0, 0};

    mutex.lock();
    if(maze[pos.row][pos.col] == 's'){
        return true;
    }
    
    maze[pos.row][pos.col] = 'o';
    print_maze();
    maze[pos.row][pos.col] = '.';
    mutex.unlock();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    

    if(is_valid_position(pos.row, pos.col-1)){ //West
        mutex.lock();
        next_position = {pos.row, pos.col-1};
        valid_positions.push(next_position);
        mutex.unlock();
    }

    if(is_valid_position(pos.row-1, pos.col)){ //North
        mutex.lock();
        next_position = {pos.row-1, pos.col};
        valid_positions.push(next_position);
        mutex.unlock();
    }

    if(is_valid_position(pos.row+1, pos.col)){ //South
        mutex.lock();
        next_position = {pos.row+1, pos.col};
        valid_positions.push(next_position);
        mutex.unlock();
    }

    if(is_valid_position(pos.row, pos.col+1)){ //East
        mutex.lock();
        next_position = {pos.row, pos.col+1};
        valid_positions.push(next_position);
        mutex.unlock();
    }    


    while(!valid_positions.empty()){
        std::cout << valid_positions.size() << std::endl;
        while(valid_positions.size() > 1){
            next_move = valid_positions.top();
            valid_positions.pop();
            threads.push_back(std::thread(walk, next_move));
        }
        next_move = valid_positions.top();
        valid_positions.pop();
        if(walk(next_move)){
            return true;
        }
    }

    return false; // Placeholder - substitua pela lógica correta
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_labirinto>" << std::endl;
        return 1;
    }

    Position initial_pos = load_maze(argv[1]);
    if (initial_pos.row == -1 || initial_pos.col == -1) {
        std::cerr << "Posição inicial não encontrada no labirinto." << std::endl;
        return 1;
    }

    bool exit_found = walk(initial_pos);

    if (exit_found) {
        std::cout << "Saída encontrada!" << std::endl;
    } else {
        std::cout << "Não foi possível encontrar a saída." << std::endl;
    }

    return 0;
}

// Nota sobre o uso de std::this_thread::sleep_for:
// 
// A função std::this_thread::sleep_for é parte da biblioteca <thread> do C++11 e posteriores.
// Ela permite que você pause a execução do thread atual por um período especificado.
// 
// Para usar std::this_thread::sleep_for, você precisa:
// 1. Incluir as bibliotecas <thread> e <chrono>
// 2. Usar o namespace std::chrono para as unidades de tempo
// 
// Exemplo de uso:
// std::this_thread::sleep_for(std::chrono::milliseconds(50));
// 
// Isso pausará a execução por 50 milissegundos.
// 
// Você pode ajustar o tempo de pausa conforme necessário para uma melhor visualização
// do processo de exploração do labirinto.

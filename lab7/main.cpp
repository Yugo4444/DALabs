#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <limits.h>
#include <random>
#include <stack>
#include <chrono>
using namespace std;

struct p_node
{
    p_node* _from = nullptr;
    long long cost = LLONG_MAX;
    int pos = -1;
};


std::ostream& operator<<(std::ostream& os, const p_node &p) {
    os << p.cost;
    return os;
}

std::ostream& operator<<(std::ostream& os, const pair<long long, long long> &p) {
    os << "(" << p.first << "," << p.second<< ")";
    return os;
}

template<typename T>
void matrix_print(vector<vector<T>> &matrix){
    for (size_t i = 0; i < matrix.size(); i++)
    {
        for (size_t j = 0; j < matrix[0].size(); j++)
        {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}


void path_init(vector<vector<long long>> &matrix, vector<vector<p_node>> &path){
    for (int j = 0; j < matrix[0].size(); j++)
    {
        p_node first_row_node = {nullptr, matrix[0][j], j};
        path[0][j] = first_row_node;
    }
    
    for (size_t i = 1; i < matrix.size(); i++)
    {
        for (int j = 0; j < matrix[0].size(); j++)
        {
            p_node empty_node = {nullptr, LLONG_MAX, j};
            path[i][j] = empty_node;
        }
    }
}

void path_update(p_node &from_node, p_node &to_node, long long plus){
    long long new_cost = from_node.cost + plus;
    if (new_cost < to_node.cost){
        to_node.cost = new_cost;
        to_node._from = &from_node;
    }
}

void forward(vector<vector<long long>> &matrix, vector<vector<p_node>> &path){
    int n = matrix.size();
    int m = matrix[0].size();
    for (size_t i = 0; i < n - 1; i++)
    {
        for (size_t j = 0; j < m; j++)
        {
            if (j == 0){
                path_update(path[i][0], path[i+1][0], matrix[i+1][0]);
                path_update(path[i][0], path[i+1][1], matrix[i+1][1]);
            } else if (j == (m - 1)){
                path_update(path[i][m - 1], path[i+1][m - 2], matrix[i+1][m - 2]);
                path_update(path[i][m - 1], path[i+1][m - 1], matrix[i+1][m - 1]);
            } else {
                path_update(path[i][j], path[i+1][j - 1], matrix[i+1][j - 1]);
                path_update(path[i][j], path[i+1][j], matrix[i+1][j]);
                path_update(path[i][j], path[i+1][j + 1], matrix[i+1][j + 1]);
            }
        }
    }
}

stack<pair<long long, long long>> backtrack(vector<vector<long long>> &matrix, vector<vector<p_node>> &path){

    p_node min_cost_node;

    for (size_t j = 0; j < path[0].size(); j++)
    {
        p_node check = path[path.size() - 1][j];
        if (check.cost < min_cost_node.cost){
            min_cost_node = check;
        }
    }

    //cout << min_cost_node.cost << endl;
   
    p_node* current = &min_cost_node;
    int row = path.size();
    stack<pair<long long, long long>> result;

    while(row != 0){
        pair<long long, long long> point;
        point.first = row;
        point.second = current->pos + 1;
        result.push(point);
        row -= 1;
        current = current->_from;
    }

    return result;
}



#include <chrono>

vector<vector<long long>> generateTestMatrix(int n, int m) {
    vector<vector<long long>> matrix(n, vector<long long>(m));
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<long long> dis(1, 100);
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            matrix[i][j] = dis(gen);
        }
    }
    return matrix;
}

// Рекурсивная функция для поиска минимального штрафа
void findMinPenalty(const vector<vector<long long>>& matrix, int row, int col, 
                   int currentPenalty, int& minPenalty, 
                   int n, int m) {
    // Добавляем штраф за текущую клетку
    currentPenalty += matrix[row][col];
    
    // Если достигли последней строки, обновляем минимальный штраф
    if (row == n - 1) {
        if (currentPenalty < minPenalty) {
            minPenalty = currentPenalty;
        }
        return;
    }
    
    // Рекурсивно перебираем все возможные переходы в следующей строке
    for (int dc = -1; dc <= 1; dc++) {
        int newCol = col + dc;
        // Проверяем, что новая колонка в пределах матрицы
        if (newCol >= 0 && newCol < m) {
            findMinPenalty(matrix, row + 1, newCol, currentPenalty, minPenalty, n, m);
        }
    }
}

int findMinTotalPenalty(const vector<vector<long long>>& matrix) {
    int n = matrix.size();
    if (n == 0) return 0;
    int m = matrix[0].size();
    if (m == 0) return 0;
    
    int minPenalty = INT_MAX;
    
    // Перебираем все стартовые позиции в первой строке
    for (int startCol = 0; startCol < m; startCol++) {
        findMinPenalty(matrix, 0, startCol, 0, minPenalty, n, m);
    }
    
    return minPenalty;
}


int main(){

    for (int i = 1; i < 7; i++)
    {
        int n = 10 * int(pow(10, i)), m = 10 * int(pow(10, i));
        vector<vector<long long>> matrix = generateTestMatrix(n, m);    
        vector<vector<p_node>> path(n, vector<p_node>(m));
        stack<pair<long long, long long>> result;

        cout << "===>   MATRIX (" << n << "," << m << ")   <===" << endl;


        auto dynamic_start = chrono::high_resolution_clock::now();
        path_init(matrix, path);
        forward(matrix, path);
        result = backtrack(matrix, path);

        auto dynamic_finish = chrono::high_resolution_clock::now();

        chrono::duration<double> dynamic = dynamic_finish - dynamic_start;
        cout << "DYNAMIC: " << dynamic.count() << endl;
        

        
        // auto force_start = chrono::high_resolution_clock::now();

        // int br = findMinTotalPenalty(matrix);
        // //cout << br << endl;

        // auto force_finish = chrono::high_resolution_clock::now();

        // chrono::duration<double> force = force_finish - force_start;
        // cout << "FORCE: " << force.count() << endl;
        cout << endl;


    }
    
    

    cout << endl;
}

#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <filesystem>

// Функция для генерации случайной строки заданной длины
std::string generate_random_string(int length) {
    std::string result;
    result.reserve(length);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis('a', 'd');
    
    for (int i = 0; i < length; ++i) {
        result += static_cast<char>(dis(gen));
    }
    
    return result;
}

int main() {
    // Создаем папку tests если ее нет
    std::filesystem::create_directory("tests");
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Генерация 100 тестов
    for (int i = 1; i <= 1000; ++i) {
        std::string filename = "tests/test_" + std::to_string(i) + ".txt";
        std::ofstream file(filename);
        
        if (!file.is_open()) {
            std::cerr << "Не удалось создать файл: " << filename << std::endl;
            continue;
        }
        
        // Генерируем длины строк
        std::uniform_int_distribution<int> len1_dis(1, 15);    // первая строка: 1-15 символов
        std::uniform_int_distribution<int> len2_dis(1, 100000); // вторая строка: 1-100000 символов
        
        int len1 = len1_dis(gen);
        int len2 = len2_dis(gen);
        
        // Генерируем строки
        std::string str1 = generate_random_string(len1);
        std::string str2 = generate_random_string(len2);
        
        // Записываем в файл
        file << str1 << "\n";
        file << str2 << "\n";
        
        file.close();
        
        std::cout << "Создан тест " << i << ": " << len1 << " и " << len2 << " символов" << std::endl;
    }
    
    std::cout << "Генерация завершена! Создано 100 тестов в папке tests/" << std::endl;
    return 0;
}
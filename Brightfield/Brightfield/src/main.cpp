#include <cctype>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <optional>
#include <ostream>
#include <sstream>
#include <vector>

#include "./tokenization.hpp"


std::vector<Token> tokenize(const std::string& str) {
 
}

std::string tokens_to_asm(const std::vector<Token>& tokens) {
    std::stringstream output;
    output << "global _start\n_start:\n";
    for(int i = 0; i < tokens.size(); i++) {
        const Token& token = tokens.at(i);
        if (token.type == TokenType::exit) {
            if (i + 1 < tokens.size() && tokens.at(i + 1).type == TokenType::int_lit) {
                if(i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::semi) {
                    output << "    mov rax, 60\n";
                    output << "    mov rdi, " << tokens.at(i + 1).value.value() << "\n"; 
                    output << "    syscall";
                }
            }
        }
    }
    return output.str();
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Incorrect usage. Correct usage is..." << std::endl;
        std::cerr << "Brightfield <input.bright>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    std::vector<Token> tokens = tokenize(contents);
    std::cout << tokens_to_asm(tokens) << std::endl;

    {
        std::fstream file("output.asm", std::ios::out);
        file << tokens_to_asm(tokens);
    }

    system("nasm -felf64 output.asm");
    system("ld -o out output.o");

    return EXIT_SUCCESS;
}
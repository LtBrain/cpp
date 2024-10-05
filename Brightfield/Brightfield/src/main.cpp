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

std::string tokens_to_asm(const std::vector<Token>& tokens) {
    std::stringstream output;
    output << "global _start\nsection .text\n_start:\n";
    for(int i = 0; i < tokens.size(); i++) {
        const Token& token = tokens.at(i);
        if (token.type == TokenType::exit) {
            if (i + 1 < tokens.size() && tokens.at(i + 1).type == TokenType::int_lit) {
                if(i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::semi) {
                    output << "    mov rax, 60\n"; // Syscall for exit
                    output << "    mov rdi, " << tokens.at(i + 1).value.value() << "\n"; // Exit code
                    output << "    syscall\n";
                }
            }
        }
    }
    return output.str();
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Incorrect usage. Correct usage is:" << std::endl;
        std::cerr << "./brightfield <input.bright>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        if (!input.is_open()) {
            std::cerr << "Failed to open input file!" << std::endl;
            return EXIT_FAILURE;
        }
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    Tokenizer tokenizer(std::move(contents));
    std::vector<Token> tokens = tokenizer.tokenize();

    // Convert tokens to assembly and print the assembly code
    std::string asm_code = tokens_to_asm(tokens);
    std::cout << "Generated Assembly Code:\n" << asm_code << std::endl; // Print assembly code

    // Save the assembly to a file
    {
        std::fstream file("output.asm", std::ios::out);
        file << asm_code;
    }

    // Run NASM to assemble the file
    int ret_nasm = system("nasm -felf64 output.asm");
    if (ret_nasm != 0) {
        std::cerr << "NASM assembly failed!" << std::endl;
        return EXIT_FAILURE;
    }

    // Link the object file to create an executable
    int ret_ld = system("ld -o out output.o");
    if (ret_ld != 0) {
        std::cerr << "Linking failed!" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Executable 'out' created successfully!" << std::endl;
    return EXIT_SUCCESS;
}

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

#include "./tokenization.hpp"
#include "./parser.hpp"
#include "./generation.hpp"

// Modify the assembly generation to exclude the explicit 'section .text'
std::string tokens_to_asm(const std::vector<Token>& tokens);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Incorrect usage. Correct usage is..." << std::endl;
        std::cerr << "./brightfield <input.bright>" << std::endl;
        return EXIT_FAILURE;
    }

    // Read the input file
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

    // Tokenize the input
    Tokenizer tokenizer(std::move(contents));
    std::vector<Token> tokens = tokenizer.tokenize();

    Parser parser(std::move(tokens));
    std::optional<NodeExit> tree = parser.parse();

    if (!tree.has_value()) {
        std::cerr << "No Exit Statement Found" << std::endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(tree.value());

    // Generate the assembly code
    std::string asm_code = tokens_to_asm(tokens);
    std::cout << "Generated Assembly Code:\n" << asm_code << std::endl;

    // Write the assembly code to output.asm
    {
        std::fstream file("output.asm", std::ios::out);
        file << generator.generate();
    }

    // Assemble with NASM
    int ret_nasm = system("nasm -felf64 output.asm -o output.o");  // Added -o for object file
    if (ret_nasm != 0) {
        std::cerr << "NASM assembly failed!" << std::endl;
        return EXIT_FAILURE;
    }

    // Link object file with ld to create executable
    int ret_ld = system("ld -o out output.o");
    if (ret_ld != 0) {
        std::cerr << "Linking failed!" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Executable 'out' created successfully!" << std::endl;
    return EXIT_SUCCESS;
};

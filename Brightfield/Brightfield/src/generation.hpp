#pragma once
#include "parser.hpp"
#include <algorithm>
#include <sstream>


class Generator {
public:
    inline Generator(NodeExit root)
       : m_root(std::move(root)) 
    {
    }

    [[nodiscard]] std::string generate() const
    {
        std::stringstream output;
        output << "global _start\n_start:\n";
        output << "    mov rax, 60\n";  // sys_exit call
        output << "    mov rdi, " << m_root.expr.int_lit.value.value() << "\n";  // Exit code
        output << "    syscall\n";
        return output.str();
    }

private:
    const NodeExit m_root;
};
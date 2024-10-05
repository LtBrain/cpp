#pragma once

#include <cctype>
#include <optional>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

enum class TokenType { exit, int_lit, semi, eof };

struct Token {
    TokenType type;
    std::optional<std::string> value {};
};

class Tokenizer {
public:
    inline explicit Tokenizer(std::string src)
        : m_src (std::move(src))
    {
    }

    inline std::vector<Token> tokenize() 
    {
        std::vector<Token> tokens;
        std::string buf;

        while(peak().has_value()) {
            if (std::isalpha(peak().value())) {
                buf.push_back(consume());
                while(peak().has_value() && std::isalnum(peak().value())) {
                    buf.push_back(consume());
                }
                if (buf == "exit") {
                    tokens.push_back({.type = TokenType::exit});
                    buf.clear();
                    continue;
                } else {
                    throw std::runtime_error("Unexpected token: " + buf);
                }
            }
            else if (std::isdigit(peak().value())) {
                buf.push_back(consume());
                while (peak().has_value() && std::isdigit(peak().value())) {
                    buf.push_back(consume());
                }
                tokens.push_back({.type = TokenType::int_lit, .value = buf});
                buf.clear();
                continue;
            }
            else if (peak().value() == ';') {
                tokens.push_back({.type = TokenType::semi});
                consume();  // Consume the semicolon
                continue;
            }
            else if (std::isspace(peak().value())) {
                consume();  // Consume the whitespace
                continue;
            } else {
                throw std::runtime_error("Unexpected character: " + std::string(1, peak().value()));
            }
        }

        // Add an EOF token at the end
        tokens.push_back({.type = TokenType::eof});

        m_index = 0;
        return tokens;
    }

private:
    [[nodiscard]] std::optional<char> peak() const 
    {
        if (m_index >= m_src.length()) {
            return {};
        } else {
            return m_src.at(m_index);
        }
    }

    char consume() {
        if (m_index >= m_src.length()) {
            throw std::out_of_range("Consume called on empty source");
        }
        return m_src.at(m_index++);
    }

    const std::string m_src;
    int m_index = 0;
};

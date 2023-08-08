#include "Tokeniser.h"

void Tokeniser::advance() {
    pos++;
    if (pos > (int) line.size() - 1)
        current_char = EOFM;
    else
        current_char = line[pos];
}

void Tokeniser::skip_whitespace() {
    while (isspace(current_char) && current_char != EOFM)
        advance();
}

int Tokeniser::integer() {
    std::string result;
    while (isdigit(current_char)) {
        result += current_char;
        advance();
    }
    return std::stoi(result);
}

std::string Tokeniser::check_string() {
    std::string result;
    advance();
    result += '"';
    while (current_char != '"') {
        result += current_char;
        advance();
    }
    advance();
    result += '"';
    return result;
}

Token Tokeniser::check_operator() {
    std::string operatorr;
    advance();
    operatorr += '.';
    while (isalpha(current_char)) {
        operatorr += current_char;
        advance();
    }
    advance();
    operatorr += '.';

    if (operatorr == ".plus." || operatorr == ".mul." || operatorr == ".div.") {
        return Token{ARITHMETIC_OP, operatorr};
    } else if (operatorr == ".and." || operatorr == ".or") {
        return Token{LOGICAL_OP, operatorr};
    } else if (operatorr == ".eq." || operatorr == ".ne." || operatorr == ".lt." || operatorr == ".gt." ||
               operatorr == ".le." || operatorr == ".ge.") {
        return Token{RELATIONAL_OP, operatorr};
    } else if (operatorr == ".not.") {
        return Token{UNARY_OP, operatorr};
    }
    else if (operatorr == ".minus.")
        return Token{UNARYandARITHMETIC, operatorr};
    else
        throw std::invalid_argument("Error");
}

Token Tokeniser::check_identifier() {
    std::map<std::string, TokenType> keywords{
            {"character", CHARACTER},
            {"do",        DO},
            {"else",      ELSE},
            {"endif",     ENDIF},
            {"endloop",   ENDLOOP},
            {"finish",    FINISH},
            {"if",        IF},
            {"integer",   INTEGER},
            {"logical",   LOGICAL},
            {"loopif",    LOOPIF},
            {"print",     PRINT},
            {"start",     START},
            {"then",      THEN}
    };

    std::string identifier;

    while (isalpha(current_char) || isdigit(current_char) || current_char == '_') {
        identifier += current_char;
        advance();
    }
    auto it = keywords.find(identifier);
    if (it != keywords.end()) {
        return Token{it->second, identifier};
    }

    return Token{IDENTIFIER, identifier};
}

Tokeniser::Tokeniser(const std::string &line)
        : line(line), pos(-1) {
    advance();
}

Token Tokeniser::get_next_token() {
    while (current_char != EOFM) {
        if (isspace(current_char)) {
            skip_whitespace();
            continue;
        }
        if (isdigit(current_char)) {
            return Token{INTEGER_CONSTANT, std::to_string(integer())};
        }
        if (current_char == ';') {
            advance();
            return Token{SEMI_COLON, ";"};
        }
        if (current_char == '"') {
            std::string final_string{check_string()};
            if (final_string.size() > 3) {
                std::cout << "Error" << std::endl;
                exit(0);
            }
            return Token{CHARACTER_CONSTANT, final_string};
        }
        if (current_char == '<') {
            advance();
            advance();
            return Token{ASSIGNMENT, "<-"};
        }
        if (current_char == ',') {
            advance();
            return Token{COMMA, ","};
        }
        if (current_char == '(') {
            advance();
            return Token{LEFT_BRACKET, "("};
        }
        if (current_char == ')') {
            advance();
            return Token{RIGHT_BRACKET, ")"};
        }
        if (current_char == '.') {
            return check_operator();
        }
        if (std::isalpha(current_char)) {
            return check_identifier();
        }
        if (current_char == '\t')
            advance();
        throw std::invalid_argument("Error");
    }
    return Token{EOFM, ""};
}
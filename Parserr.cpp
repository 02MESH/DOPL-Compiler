#include "Parserr.h"

Parserr::Parserr(const std::string &file_name) : Tokeniser(file_name), tokenised_words{run_once(file_name)},
                                                 conditionalFlag(false) {
    parse_file();
}

void Parserr::advance() {
    counter++;
}

int Parserr::currentTokenType() {
    return tokenised_words.at(counter).type;
}

std::string Parserr::currentTokenValue() {
    return tokenised_words.at(counter).value;
}

void Parserr::error() {
    std::cout << "Error" << std::endl;
    exit(0);
}


std::vector<Token> Parserr::run_once(const std::string &file_name) {
    std::vector<Token> tokenised_text{};
    std::ifstream reader{file_name};
    std::string input;

    while (std::getline(reader, input)) {
        Tokeniser tokenizer(input);
        while (true) {
            Token token = tokenizer.get_next_token();
            if (token.type == EOFM) {
                break;
            }
            tokenised_text.push_back(token);
        }
    }
    return tokenised_text;
}

bool Parserr::checkStartFinish() {
    return (tokenised_words.at(0).type == 11 && tokenised_words.at(tokenised_words.size() - 1).type == 5);
}

bool Parserr::checkIdentifierType() {
    bool result = (currentTokenType() == 0 || currentTokenType() == 7 || currentTokenType() == 8);
    if (result)
        advance();
    return result;
}

bool Parserr::checkIdentifier() {
    int identifierType = counter - 1;
    int changeIdentifierType = tokenised_words.at(identifierType).type;
    bool result;
    bool flag = true;
    while (flag) {
        if (tokenised_words.at(counter).type == 14) {
            //tokenised_words.at(counter).type = static_cast<TokenType>(changeIdentifierType);
            identifierTypes.insert(std::make_pair(tokenised_words.at(counter).value, changeIdentifierType));
            identifiers.push_back(tokenised_words.at(counter));
        } else if (tokenised_words.at(counter).type == 18) {
            flag = false;
            result = true;
        } else if (tokenised_words.at(counter).type == 0 || tokenised_words.at(counter).type == 7) {
            flag = false;
            result = false;
        }
        advance();
    }
    return result;
}

bool Parserr::checkIdentifierExistence() {
    for (const auto &x: identifiers) {
        if (currentTokenValue() == x.value) {
            return true;
        }
    }
    error();
    return false;
}

bool Parserr::checkprint() {
    if (currentTokenType() == 10) {
        advance();
        if (expression())
            if (currentTokenType() == 18) {
                advance();
                return true;
            }
    }
    return false;
}

void Parserr::parse_file() {
    bool flag = true;
    const static size_t threshold = tokenised_words.size() - 2;

    if (checkStartFinish()) {
        do {
            if (checkIdentifierType()) {
                checkIdentifier();
            } else if (checkprint()) {
                std::cout << "";
            } else if (conditional()) {
                std::cout << "";
            } else if (assignment()) {
//                if (checkIdentifierExistence()) {
//                    advance();
//                    if (assignment())
                //std::cout<<"Haha"<<std::endl;
                std::cout << "";

            } else if (loop()) {
                std::cout << "";
            } else {
                //std::cout<<counter<<std::endl;
                advance();
            }
            //print_token(counter);

        } while (threshold > counter && flag);
        std::cout << "Success" << std::endl;
    } else {
        std::cout << "Error" << std::endl;
    }
}

bool Parserr::expression() {
    if (term()) {
        while (checkBinaryOp()) {
            if (!term()) {
                error();
            }
        }
        if (currentTokenType() == 18) //semi-colon
            return true;
        else if (currentTokenType() == 21) // ')' bracket
            return true;
        else if (conditionalFlag && currentTokenType() == 12) /*Then*/ {
            conditionalFlag = false;
            return true;
        } else if (loopFlag && currentTokenType() == 1) /*Do*/{
            loopFlag = false;
            return true;
        } else {
            error();
        }
    } else {
        error();
    }
    return false;
}

bool Parserr::term() {
    if (currentTokenType() == 17) {
        advance();
        return true;
    } else if (currentTokenType() == 16) {
        advance();
        return true;
    } else if (currentTokenType() == 14 && checkIdentifierExistence()) {
        advance();
        return true;
    } else if (checkUnaryOp()) {
        if (term())
            return true;
    } else if (currentTokenType() == 20) {
        advance();
        if (expression()) {
            if (currentTokenType() == 21) {
                advance();
                return true;
            } else
                error();
        } else {
            error();
        }
    }
    return false;
}

bool Parserr::assignment() {
    if (currentTokenType() == 14) {
        if (checkIdentifierExistence()) {
            int counterCopy = counter;
            advance();
            if (currentTokenType() == 15) {
                advance();
                //assignment check function here
                if (assignmentIdentifierCheck(counterCopy)) {
                    //ends here
                    if (expression()) {
                        advance();
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool Parserr::conditionalAndLoopTypeChecking(int conditionalCounterCopy) {
    if (typeCheckingForConditionals(conditionalCounterCopy))
        return true;
    error();
    return false;
}

bool Parserr::typeCheckingForConditionals(int conditionalCounterCopy) {
    std::vector<Token> expression;
    while (tokenised_words.at(conditionalCounterCopy).type != 1 &&
           tokenised_words.at(conditionalCounterCopy).type != 12) {
        expression.push_back(tokenised_words.at(conditionalCounterCopy));
        conditionalCounterCopy++;
    }

    for (auto x: expression) {
        if (x.type == 22 || x.type == 24 || x.type == 25)
            return true;
    }

    error();
    return false;
}

bool Parserr::assignmentIdentifierCheck(int identifierCounterCopy) {
    std::string counterCopyIdentifier = tokenised_words.at(identifierCounterCopy).value;
    auto it = identifierTypes.find(counterCopyIdentifier);
    int type = it->second;
    if (type == typeCheck(identifierCounterCopy))
        return true;
    error();
    return false;
}

int Parserr::typeCheck(int identifierCounterCopy) {
    std::vector<Token> expression;
    int counterCopy = identifierCounterCopy + 2;
    while (tokenised_words.at(counterCopy).type != 18) {
        expression.push_back(tokenised_words.at(counterCopy));
        counterCopy++;
    }

    //logical expression check
    for (auto x: expression) {
        if (x.type == 22 || x.type == 24 || x.type == 25)
            return 8;
    }

    //character expression check
    for (auto x: expression) {
        if (x.type == 16 || tokenTypeCheck(0, x.value))
            return 0;
    }

    //integer expression
    return 7;
}

bool Parserr::tokenTypeCheck(int token_type, const std::string &identifier_name) {
    auto it = identifierTypes.find(identifier_name);
    if (it->second == token_type)
        return true;
    return false;
}

bool Parserr::loop() {
    if (currentTokenType() == 9) {
        loopFlag = true;
        advance();
        int counterCopy = counter;
        if (conditionalAndLoopTypeChecking(counterCopy)) {
            if (expression()) {
                if (currentTokenType() == 1) {
                    advance();
                    //expression check
                    while (statements()) {
                        std::cout << "";
                    }
                    if (currentTokenType() == 4) {
                        advance();
                        if (currentTokenType() == 18) {
                            advance();
                            return true;
                        }
                    } else
                        error();
                }
            }
        }
    }
    return false;
}

bool Parserr::conditional() {
    if (currentTokenType() == 6) {
        conditionalFlag = true;
        advance();
        int counterCopy = counter;
        if (conditionalAndLoopTypeChecking(counterCopy)) {
            if (expression()) {
                if (currentTokenType() == 12) {
                    advance();
                    while (statements()) {
                        std::cout << "";
                    }
                    if (currentTokenType() == 2) {
                        advance();
                        while (statements()) {
                            std::cout << "";
                        }
                        if (currentTokenType() == 3) {
                            advance();
                            if (currentTokenType() == 18) {
                                advance();
                                return true;
                            }
                        }
                    } else if (currentTokenType() == 3) {
                        advance();
                        if (currentTokenType() == 18) {
                            advance();
                            return true;
                        }
                    } else {
                        error();
                    }
                }
            }
        }
        error();
    }
    return false;
}

bool Parserr::statements() {
    if (statement()) {
        return true;
    }
    return false;
}

bool Parserr::statement() {
    if (assignment())
        return true;
    else if (conditional())
        return true;
    else if (checkprint())
        return true;
    else if (loop())
        return true;
    return false;
}

bool Parserr::checkBinaryOp() {
    return (checkArithmeticOp() || checkLogicalOp() || checkRelationalOp() || checkUnaryOp());
}

bool Parserr::checkArithmeticOp() {
    bool result = currentTokenType() == 23 || currentTokenType() == 26;
    if (result)
        advance();
    return result;
}

bool Parserr::checkLogicalOp() {
    bool result = currentTokenType() == 22;
    if (result)
        advance();
    return result;
}

bool Parserr::checkRelationalOp() {
    bool result = currentTokenType() == 25;
    if (result)
        advance();
    return result;
}

bool Parserr::checkUnaryOp() {
    bool result = currentTokenType() == 24 || currentTokenType() == 26;
    if (result)
        advance();
    return result;
}




//
// Created by Daniel Posselt on 5/11/22.
//

#ifndef MATH_SQUARES_ML_SOLUTION_SHUNTINGYARD_H
#define MATH_SQUARES_ML_SOLUTION_SHUNTINGYARD_H




#include <deque>
#include <string>

class Token {
public:
    enum class Type {
        Unknown,
        Number,
        Operator,
        LeftParen,
        RightParen,
    };

    Token(Type t, const std::string& s, int prec = -1, bool ra = false)
            : type { t }, str ( s ), precedence { prec }, rightAssociative { ra }
    {}

    const Type type;
    const std::string str;
    const int precedence;
    const bool rightAssociative;
};


template<class T, class U>
void debugReport(const Token& token, const T& queue, const U& stack, const std::string& comment = "");


std::deque<Token> shuntingYard(const std::deque<Token>& tokens);
std::deque<Token> exprToTokens(const std::string& expr);
std::vector<int> calcuate(std::vector<std::string>& expressions);





#endif //MATH_SQUARES_ML_SOLUTION_SHUNTINGYARD_H

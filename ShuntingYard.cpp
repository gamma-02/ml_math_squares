//
// Created by Daniel Posselt on 5/6/22.
//

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <stdio.h>
#include <math.h>
#include "ShuntingYard.h"

static const char* reportFmt = "|%-5s|%-37s|%17s| %s\n";

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << token.str;
    return os;
}



// Debug output
template<class T, class U>
void debugReport(const Token& token, const T& queue, const U& stack, const std::string& comment) {
//    std::ostringstream ossQueue;
//    for(const auto& t : queue) {
//        ossQueue << " " << t;
//    }
//
//    std::ostringstream ossStack;
//    for(const auto& t : stack) {
//        ossStack << " " << t;
//    }
//
//    printf(reportFmt
//            , token.str.c_str()
//            , ossQueue.str().c_str()
//            , ossStack.str().c_str()
//            , comment.c_str()
//    );
}


std::deque<Token> exprToTokens(const std::string& expr) {
    std::deque<Token> tokens;

    for(const auto* p = expr.c_str(); *p; ++p) {
        if(isblank(*p)) {
            // do nothing
        } else if(isdigit(*p)) {
            const auto* b = p;
            while(isdigit(*p)) {
                ++p;
            }
            const auto s = std::string(b, p);
            tokens.emplace_back( Token::Type::Number, s );
            --p;
        } else {
            Token::Type t = Token::Type::Unknown;
            int pr = -1;            // precedence
            bool ra = false;
//            std::cout << *p << std::endl;// rightAssociative
            switch(*p) {
                default:                                    break;
                case '(':   t = Token::Type::LeftParen;     break;
                case ')':   t = Token::Type::RightParen;    break;
                case '^':   t = Token::Type::Operator;      pr = 4; ra = true;  break;
                case '*':   t = Token::Type::Operator;      pr = 3; break;
                case '/':   t = Token::Type::Operator;      pr = 3; break;
                case '+':   t = Token::Type::Operator;      pr = 2; break;
                case '-':   t = Token::Type::Operator;      pr = 2; break;
            }
            const auto s = std::string(1, *p);
            tokens.push_back(Token { t, s, pr, ra });
        }
    }

    return tokens;
}


std::deque<Token> shuntingYard(const std::deque<Token>& tokens) {

    std::deque<Token> queue;
    std::vector<Token> stack;

    // While there are tokens to be read:
    for(auto token : tokens) {
        // Read a token
        switch(token.type) {
            case Token::Type::Number:
                // If the token is a number, then add it to the output queue
                queue.push_back(token);
                break;

            case Token::Type::Operator:
            {
                // If the token is operator, o1, then:
                const auto o1 = token;

                // while there is an operator token,
                while(!stack.empty()) {
                    // o2, at the top of stack, and
                    const auto o2 = stack.back();

                    // either o1 is left-associative and its precedence is
                    // *less than or equal* to that of o2,
                    // or o1 if right associative, and has precedence
                    // *less than* that of o2,
                    if(
                            (! o1.rightAssociative && o1.precedence <= o2.precedence)
                            ||  (  o1.rightAssociative && o1.precedence <  o2.precedence)
                            ) {
                        // then pop o2 off the stack,
                        stack.pop_back();
                        // onto the output queue;
                        queue.push_back(o2);

                        continue;
                    }

                    // @@ otherwise, exit.
                    break;
                }

                // push o1 onto the stack.
                stack.push_back(o1);
            }
                break;

            case Token::Type::LeftParen:
                // If token is left parenthesis, then push it onto the stack
                stack.push_back(token);
                break;

            case Token::Type::RightParen:
                // If token is right parenthesis:
            {
                bool match = false;

                // Until the token at the top of the stack
                // is a left parenthesis,
                while(! stack.empty() && stack.back().type != Token::Type::LeftParen) {
                    // pop operators off the stack
                    // onto the output queue.
                    queue.push_back(stack.back());
                    stack.pop_back();
                    match = true;
                }

                // Pop the left parenthesis from the stack,
                // but not onto the output queue.
                stack.pop_back();

                if(!match && stack.empty()) {
                    // If the stack runs out without finding a left parenthesis,
                    // then there are mismatched parentheses.
                    printf("RightParen error (%s)\n", token.str.c_str());
                    return {};
                }
            }
                break;

            default:
                printf("error (%s)\n", token.str.c_str());
                return {};
        }

        debugReport(token, queue, stack);
    }

    // When there are no more tokens to read:
    //   While there are still operator tokens in the stack:
    while(! stack.empty()) {
        // If the operator token on the top of the stack is a parenthesis,
        // then there are mismatched parentheses.
        if(stack.back().type == Token::Type::LeftParen) {
            printf("Mismatched parentheses error\n");
            return {};
        }

        // Pop the operator onto the output queue.
        queue.push_back(std::move(stack.back()));
        stack.pop_back();
    }

    debugReport(Token { Token::Type::Unknown, "End" }, queue, stack);

    //Exit.
    return queue;
}

std::vector<int> calcuate(std::vector<std::string>& expressions){
    std::vector<int> returnVals;

    for(const auto& expr : expressions) {

        const auto tokens = exprToTokens(expr);
        auto queue = shuntingYard(tokens);
        std::vector<int> stack;


        while(! queue.empty()) {
            std::string op;

            const auto token = queue.front();
            queue.pop_front();
            switch(token.type) {
                case Token::Type::Number:
                    stack.push_back(std::stoi(token.str));
                    op = "Push " + token.str;
                    break;

                case Token::Type::Operator:
                {
                    const auto rhs = stack.back();
                    stack.pop_back();
                    const auto lhs = stack.back();
                    stack.pop_back();

                    switch(token.str[0]) {
                        default:
                            printf("Operator error [%s]\n", token.str.c_str());
                            exit(0);
                            break;
                        case '^':
                            stack.push_back(static_cast<int>(pow(lhs, rhs)));
                            break;
                        case '*':
                            stack.push_back(lhs * rhs);
                            break;
                        case '/':
                            stack.push_back(lhs / rhs);
                            break;
                        case '+':
                            stack.push_back(lhs + rhs);
                            break;
                        case '-':
                            stack.push_back(lhs - rhs);
                            break;
                    }
                    op = "Push " + std::to_string(lhs) + " " + token.str + " " + std::to_string(rhs);
                }
                    break;

                default:
                    printf("Token error\n");
                    exit(0);
            }
            debugReport(token, queue, stack, op);
        }
        returnVals.push_back(stack.back());

    }
    return returnVals;
}

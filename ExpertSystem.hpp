/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExpertSystem.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dborysen <dborysen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/23 13:44:35 by dborysen          #+#    #+#             */
/*   Updated: 2019/05/15 13:22:37 by dborysen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <vector>
#include <regex>
#include <fstream>
#include <stack>

using VecStr = std::vector<std::string>;
using VecChr = std::vector<char>;

enum ExpertSystemAttributes
{
    maxArgNum = 2,
    argId = 1,
    operatorInConclusion = 2
};

class ExpertSystem
{
public:
    enum TokenType
    {
        Fact,
        Operator
    };

    typedef struct  s_token
    {
        TokenType   type;
        std::string name;
        bool        value = false;
    }               t_token;

    using VecToken = std::vector<t_token>;
    using VecVecToken = std::vector<VecToken>;
    using StackToken = std::stack<ExpertSystem::t_token>;
    using MethodPtr = void (ExpertSystem::*)(StackToken& facts) const;
    using PairToken = std::pair<t_token, t_token>;

    ExpertSystem(const std::string& fileName);

    ExpertSystem() = delete;
    ExpertSystem(const ExpertSystem&& other) = delete;
    ExpertSystem(const ExpertSystem& other) = delete;
    ~ExpertSystem() = default;

    ExpertSystem&    operator=(const ExpertSystem& other) = delete;
    ExpertSystem&&   operator=(const ExpertSystem&& other) = delete;

    void        OutputResult() const;

private:
    VecStr      LoadData(const std::string& fileName) const;

    void        OutputFileData() const;

    VecStr      CutAllComments(const VecStr& data, char commentSymbol) const;

    bool        IsDataValid(const VecStr& data) const;
    bool        IsAllSymbolsValid(const VecStr& data) const;
    bool        IsImplicationOnPlace(const VecVecToken& tokens) const;

    VecVecToken Lexer(const VecStr& data);
    t_token     CreateTokenStruct(const std::string& line) const;
    VecToken    SplitOnTokensStructs(std::string line) const;

    VecChr      GetСondition(VecStr& data, const std::string& factType);

    void        MarkAllTrueFacts(VecVecToken& dataInTokens);
    void        MarkKnownFact(VecVecToken& dataInTokens, const t_token& trueFact);

    void        ProcessAllRules(VecVecToken& dataInTokens);
    VecToken    ProcessRule(const VecToken& rule) const;
    void        ProcessPriorityOperator(StackToken& facts,
                                        StackToken& operators) const;
    void        ProcessAndInConclusion(StackToken& facts,
                                        StackToken& operators) const;
    void        ProcessParentheses(StackToken& facts,
                                    StackToken& operators) const;

    /********* Operator handlers *********/
    void        AndOperator(StackToken& facts) const;
    void        OrOperator(StackToken& facts) const;
    void        NegationOperator(StackToken& facts) const;
    void        XorOperator(StackToken& facts) const;
    void        ImplicationOperator(StackToken& facts) const;

    VecStr  _fileData;

    VecChr  _trueFacts {};
    VecChr  _factsToFind {};
    
    VecVecToken _tokens;
};

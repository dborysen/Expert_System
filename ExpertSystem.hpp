/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExpertSystem.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dborysen <dborysen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/23 13:44:35 by dborysen          #+#    #+#             */
/*   Updated: 2019/05/08 15:52:04 by dborysen         ###   ########.fr       */
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
    argId = 1
};

class ExpertSystem
{
public:
    ExpertSystem(const std::string& fileName);

    ExpertSystem() = delete;
    ExpertSystem(const ExpertSystem&& other) = delete;
    ExpertSystem(const ExpertSystem& other) = delete;
    ~ExpertSystem() = default;

    ExpertSystem&    operator=(const ExpertSystem& other) = delete;
    ExpertSystem&&   operator=(const ExpertSystem&& other) = delete;

private:
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
        bool        isChecked = false;
    }               t_token;

    using VecToken = std::vector<t_token>;
    using VecVecToken = std::vector<VecToken>;

    VecStr      LoadData(const std::string& fileName) const;

    VecStr      CutAllComments(const VecStr& data, char commentSymbol) const;
    bool        IsDataValid(const VecStr& data) const;
    bool        IsAllSymbolsValid(const VecStr& data) const;

    VecVecToken Lexer(const VecStr& data);
    t_token     CreateTokenStruct(const std::string& line) const;
    VecToken    SplitOnTokensStructs(std::string line) const;

    VecChr      GetСondition(VecStr& data, const std::string& factType);

    void        ProcessAllRules(VecVecToken& dataInTokens);

    void        MarkAllTrueFacts(VecVecToken& dataInTokens);
    void        MarkKnownFact(VecVecToken& dataInTokens,
                                const t_token& trueFact);
    void        MarkAllFalseFacts(VecVecToken& dataInTokens);

    VecToken    ProcessRule(const VecToken& rule) const;

    VecStr  _fileData;

    VecChr  _trueFacts {};
    VecChr  _factsToFind {};
    
    VecVecToken _tokens;
};

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExpertSystem.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dborysen <dborysen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/23 13:44:35 by dborysen          #+#    #+#             */
/*   Updated: 2019/05/06 18:35:35 by dborysen         ###   ########.fr       */
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

    using VecVecToken = std::vector<std::vector<t_token> >;

    VecStr  LoadData(const std::string& fileName) const;

    VecStr  CutAllComments(const VecStr& data, char commentSymbol) const;
    bool    ValidateData(const VecStr& data) const;
    bool    AllSymbolsValid(const VecStr& data) const;

    void        SaveTokens(VecStr& data);
    VecVecToken Lexer(const VecStr& data);

    VecChr  GetСondition(VecStr& data, const std::string& factType);

    VecStr  _fileData;

    VecChr  _trueFacts {};
    VecChr  _factsToFind {};
    
    VecVecToken _dataInTokens;

    std::stack<t_token> _factStack;
    std::stack<t_token> _operatorStack;
};

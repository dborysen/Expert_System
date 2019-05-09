/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExpertSystem.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dborysen <dborysen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/30 15:05:25 by dborysen          #+#    #+#             */
/*   Updated: 2019/05/09 16:27:33 by dborysen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ExpertSystem.hpp"
#include <map>

static std::map<std::string, size_t> operationPriority
{
    {"(", 6},
    {"!", 5},
    {"+", 4},
    {"|", 3},
    {"^", 2},
    {"=>", 1},
    {")", 0},
};

static VecChr SplitOnSymbols(const std::string& str)
{
    VecChr vecChr;

    for (auto it = str.begin(); it != str.end(); ++it)
    {
        if (*it != ' ' && *it != '\t')
            vecChr.push_back(*it);
    }
    return vecChr;
}

ExpertSystem::ExpertSystem(const std::string& fileName)
{    
    _fileData = CutAllComments(LoadData(fileName), '#');

    if (!IsDataValid(_fileData))
        throw std::logic_error("Validation Failed");

    _trueFacts = GetСondition(_fileData, "=");
    _factsToFind = GetСondition(_fileData, "\\?");

    _tokens = Lexer(_fileData);
    
    ProcessAllRules(_tokens);
}

void    ExpertSystem::ProcessAllRules(VecVecToken& tokens)
{
    std::vector<t_token>    updatedFacts;
    auto                    isAllFactsKnown = false;

    MarkAllTrueFacts(tokens);

    for (auto it = tokens.begin(); it != tokens.end() && !isAllFactsKnown; it++)
    {
        updatedFacts = ProcessRule(*it);

        if (!updatedFacts.empty())
        {
            for (const auto& fact : updatedFacts)
                MarkKnownFact(tokens, fact);

            updatedFacts.clear();
            it = tokens.begin();
            continue;
        }

        if (it == tokens.end() && updatedFacts.empty() && !isAllFactsKnown)
        {
            MarkAllFalseFacts(tokens);

            it = tokens.begin();
            isAllFactsKnown = true;
            continue;
        }

    }
}

static  bool IsOperatorMorePriority(const std::string& op1, const std::string& op2)
{
    return operationPriority.at(op1) > operationPriority.at(op2);
}

void ExpertSystem::AndOperator(StackToken& facts) const
{
    auto fact1 = facts.top();
    facts.pop();
    auto fact2 = facts.top();
    facts.pop();

    fact1.value = fact1.value && fact2.value;

    facts.emplace(fact1);
}

void ExpertSystem::OrOperator(StackToken& facts) const
{
    auto fact1 = facts.top();
    facts.pop();
    auto fact2 = facts.top();
    facts.pop();

    fact1.value = fact1.value || fact2.value;

    facts.emplace(fact1);
}

void ExpertSystem::NegationOperator(StackToken& facts) const
{
    auto fact = facts.top();
    facts.pop();

    fact.value = !fact.value;

    facts.emplace(fact);
}

void ExpertSystem::XorOperator(StackToken& facts) const
{
    auto fact1 = facts.top();
    facts.pop();
    auto fact2 = facts.top();
    facts.pop();

    fact1.value = fact1.value ^ fact2.value;

    facts.emplace(fact1);
}

void ExpertSystem::ImplicationOperator(StackToken& facts) const
{

}

void ExpertSystem::ProcessPriorityOperator(StackToken& facts,
    StackToken& operators) const
{
    std::map<std::string, MethodPtr> funcMap
    {
        {"+", &ExpertSystem::AndOperator},
        {"|", &ExpertSystem::OrOperator},
        {"!", &ExpertSystem::NegationOperator},
        {"^", &ExpertSystem::XorOperator},
        {"=>", &ExpertSystem::ImplicationOperator}
    };

    const auto topOperator = operators.top().name;
    const auto neededFunc = funcMap.at(topOperator);

    (this->*neededFunc)(facts);

    operatorStack.pop();
}

ExpertSystem::VecToken ExpertSystem::ProcessRule(const VecToken& rule) const
{
    VecToken updatedTokens;
    
    StackToken facts;
    StackToken operatorStack;

    for (const auto& token : rule)
    {
        if (token.type == Fact)
        {
            facts.emplace(token);
        }
        else if (token.type == Operator)
        {
            if (operatorStack.empty() ||
                IsOperatorMorePriority(token.name, operatorStack.top().name) ||
                token.name == "(")
                operatorStack.emplace(token);
            else
            {
                while (!IsOperatorMorePriority(token.name, operatorStack.top().name))
                    ProcessPriorityOperator(facts, operatorStack);
                // operatorStack.emplace(token);
            }

        }
    }

    return updatedTokens;
}

void    ExpertSystem::MarkAllFalseFacts(VecVecToken& dataInTokens)
{
    for (auto& vecToken : dataInTokens)
    {
        for (auto& token : vecToken)
        {
            if (!token.isChecked)
            {
                token.value = false;
                token.isChecked = true;
            }
        }
    }
}

void    ExpertSystem::MarkKnownFact(VecVecToken& dataInTokens,
                                    const t_token& trueFact)
{
    for (auto& vecToken : dataInTokens)
    {
        for (auto& token : vecToken)
        {
            if (token.name == trueFact.name)
                token = trueFact;
        }
    }
}

void    ExpertSystem::MarkAllTrueFacts(VecVecToken& dataInTokens)
{
    for (auto& vecToken : dataInTokens)
    {
        for (auto& token : vecToken)
        {
            if (token.type == Operator)
                continue;

            for (const auto trueFact : _trueFacts)
            {
                if (trueFact == *token.name.cbegin())
                {
                    token.value = true;
                    token.isChecked = true;
                }
            }
        }
    }
}

ExpertSystem::t_token ExpertSystem::CreateTokenStruct(const std::string& line) const
{        
    return ExpertSystem::t_token 
    {
        std::regex_match(line, std::regex("[A-Z]")) ? Fact : Operator,
        line,
    };
}

ExpertSystem::VecToken ExpertSystem::SplitOnTokensStructs(std::string line) const
{
    std::cmatch             result;
    std::regex              regular("([A-Z])|(=>)|([+!|^()])");
    std::vector<t_token>    lineTokens;

    while (std::regex_search(line.c_str(), result, regular))
    {
        lineTokens.push_back(CreateTokenStruct(*result.cbegin()));

        line = result.suffix().str();
    }

    return lineTokens;
}

ExpertSystem::VecVecToken ExpertSystem::Lexer(const VecStr& data)
{
    VecVecToken dataInTokens;

    for (const std::string& line : data)
    {
        if (line.empty())
            continue;

        dataInTokens.push_back(SplitOnTokensStructs(line));
    }
    return dataInTokens;
}

VecChr  ExpertSystem::GetСondition(VecStr& data, const std::string& factType)
{
    std::cmatch result;
    std::regex regular(factType + "([A-Z]*)[^>]");

    VecChr      facts;
    std::string logFactType;
    auto        matchFound = false;

    logFactType = factType == "=" ? "initial facts" : "queries";

    for (auto& line : data)
    {
        if (std::regex_search(line.c_str(), result, regular))
        {
            if (matchFound)
                throw std::logic_error(
                    "\033[1;31mError:\033[0m more then one " +
                    logFactType + " line");

            facts = SplitOnSymbols(result[1]);
            line.clear();
            matchFound = true;
        }
    }
   
    if (!matchFound)
        throw std::logic_error("\033[1;31mError:\033[0m no " + logFactType);

    return facts;
}

VecStr  ExpertSystem::CutAllComments(const VecStr& data, char commentSymbol) const
{
    VecStr noCommentsData;

    for (auto& line : data)
    {
        std::string tmp = line.substr(0, line.find(commentSymbol));

        if (!tmp.empty())
            noCommentsData.push_back(tmp);
    }

    return noCommentsData;
}

bool    ExpertSystem::IsDataValid(const VecStr& data) const
{
    if (data.size() == 1 && *data.begin() == "")
    {
        std::cerr << "\033[1;31mError:\033[0m no data" << std::endl;
        return false;
    }

    if (!IsAllSymbolsValid(data))
        return false;

    return true;
}

bool    ExpertSystem::IsAllSymbolsValid(const VecStr& data) const
{
    std::regex reg("[A-Z\\s|+?\\^!=>()]*"); //ADD MORE OPERATORS IF NEEDED
    
    auto isOk = true;

    for(auto i = 0u; i < data.size(); i++)
    {
        if (!std::regex_match(data.at(i).c_str(), reg))
        {
            std::cerr << "[line " << i + 1
            << "]\033[1;31m Error\033[0m wrong input format:"
            << std::endl << "\t" + data.at(i) << std::endl;

            isOk = false;
        }
    }
    return isOk;
}

VecStr ExpertSystem::LoadData(const std::string& fileName) const
{
    std::ifstream   inFile;
    VecStr          fileData;

    inFile.open(fileName);
    if (inFile.fail())
    {
        std::cerr << "\033[1;31mError\033[0m Opening File" << std::endl;
        exit(1);
    }

    for (std::string line; !inFile.eof(); line = "")
    {
        std::getline(inFile, line);
        fileData.push_back(line);
    }
    inFile.close();

    return fileData;
}

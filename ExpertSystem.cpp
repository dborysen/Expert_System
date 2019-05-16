/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExpertSystem.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dborysen <dborysen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/30 15:05:25 by dborysen          #+#    #+#             */
/*   Updated: 2019/05/16 14:08:06 by dborysen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ExpertSystem.hpp"
#include <map>

static std::map<std::string, size_t> operationPriority
{
    {")", 6},
    {"!", 5},
    {"+", 4},
    {"|", 3},
    {"^", 2},
    {"=>", 1},
    {"(", 0},
};

static VecChr SplitOnSymbols(const std::string& str)
{
    VecChr vecChr;

    for (auto it = str.begin(); it != str.end(); ++it)
    {
        if (*it != ' ' && *it != '\t' && *it != '=' && *it != '?')
            vecChr.emplace_back(*it);
    }
    return vecChr;
}

static void OutputNonValidLine(const ExpertSystem::VecToken& tokens)
{
    std::cout << "\t";
    for (const auto& token : tokens)
    {
        std::cerr << token.name + " ";
    }
    std::cerr << std::endl;
}

static  bool IsOperatorMorePriority(const std::string& op1, const std::string& op2)
{
    return operationPriority.at(op1) > operationPriority.at(op2);
}

static ExpertSystem::PairToken GetTopTwoFacts(ExpertSystem::StackToken& facts)
{
    ExpertSystem::PairToken TopTwoFacts;

    if (facts.size() < 2)
        throw std::logic_error("\033[1;31mError:\033[0m wrong input format");

    TopTwoFacts.first = facts.top();
    facts.pop();
    TopTwoFacts.second = facts.top();
    facts.pop();

    return TopTwoFacts;
}

static ExpertSystem::VecToken GetVecFromStack(ExpertSystem::StackToken& stack)
{
    ExpertSystem::VecToken  updatedTokens;

    for (; !stack.empty(); stack.pop())
        updatedTokens.emplace_back(stack.top());
    
    return updatedTokens;
}

static size_t GetReserveNum(const VecStr& data)
{
    auto reserve = 0u;

    for (const auto& line : data)
    {
        if (!line.empty())
            continue;
        reserve++;
    }
    return reserve;
}

ExpertSystem::ExpertSystem(const std::string& fileName)
{    
    _fileData = CutAllComments(LoadData(fileName), '#');

    if (!IsDataValid(_fileData))
        throw std::logic_error("Validation Failed");

    _trueFacts = GetСondition(_fileData, "=");
    _factsToFind = GetСondition(_fileData, "\\?");

    _tokens = Lexer(_fileData);

    if (!IsImplicationOnPlace(_tokens))
        throw std::logic_error("Validation Failed");
    
    ProcessAllRules(_tokens);
}

bool    ExpertSystem::IsImplicationOnPlace(const VecVecToken& tokens) const
{
    auto isOk = true;

    for (const auto& vecToken : tokens)
    {
        auto i = 0u;
        for (const auto& token : vecToken)
        {
            if (token.name == "=>")
                i++;
        }

        if (i != implicationValidNum)
        {
            isOk = false;
            std::cerr << "\033[1;31mError:\033[0m wrong input format\n";
            OutputNonValidLine(vecToken);
        }
    }
    return isOk;
}

void    ExpertSystem::OutputFileData() const
{
    for (const auto& line : _fileData)
    {
        if (!line.empty())
            std::cout << "\033[1;37m" + line + "\033[0m"<< std::endl;
    }

    std::cout << "\n\033[1;32m=\033[0m";

    for (const auto fact : _trueFacts)
        std::cout << "\033[1;32m" << fact << "\033[0m";

    std::cout << "\n\n\033[1;31m?\033[0m";

    for (const auto fact : _factsToFind)
        std::cout << "\033[1;31m" << fact << "\033[0m";

    std::cout << "\n\n";
}

static void PrintFactStatus(const ExpertSystem::t_token& fact)
{
    std::cout << "\033[1;37m" << *fact.name.cbegin() << ":\033[0m ";

    if (fact.value)
        std::cout << "\033[1;32m" << std::boolalpha << fact.value << "\033[0m\n";
    else
        std::cout << "\033[1;31m" << std::boolalpha << fact.value << "\033[0m\n";
}

void    ExpertSystem::OutputResult() const
{
    OutputFileData();

    VecChr shownFacts;

    const auto IsInContainer = [&shownFacts](char fact)
    {
        return std::find(shownFacts.begin(), shownFacts.end(), fact)
            != shownFacts.end();
    };

    for (const auto& vecToken : _tokens)
    {
        for (const auto& token : vecToken)
        {
            for (auto factToFind : _factsToFind)
            {
                const auto isPrintable = factToFind == 
                    *token.name.cbegin() && !IsInContainer(factToFind);

                if (isPrintable)
                {
                    PrintFactStatus(token);
                    shownFacts.emplace_back(factToFind);
                }
            }
        }
    }
}

void    ExpertSystem::ProcessAllRules(VecVecToken& tokens)
{
    MarkAllTrueFacts(tokens);

    for (auto it = tokens.begin(); it != tokens.end();)
    {
        auto updatedFacts = ProcessRule(*it);

        if (!updatedFacts.empty())
        {
            for (const auto& fact : updatedFacts)
                MarkKnownFact(tokens, fact);

            it = tokens.begin();
            continue;
        }
        ++it;
    }
}

ExpertSystem::VecToken ExpertSystem::ProcessRule(const VecToken& rule) const
{
    StackToken  facts;
    StackToken  operators;

    for (const auto& token : rule)
    {
        if (token.type == Fact)
        {
            facts.emplace(token);
        }
        else if (token.type == Operator)
        {
            if (token.name == ")")
            {
                ProcessParentheses(facts, operators);
                continue;
            }
            ProcessOperator(token, facts, operators);
        }
    }

    if (operators.size() == operatorInConclusion)
        ProcessAndInConclusion(facts, operators);

    while (!operators.empty())
         ProcessPriorityOperator(facts, operators);

    return GetVecFromStack(facts);
}

void ExpertSystem::AndOperator(StackToken& facts) const
{
    auto topTwoFacts = GetTopTwoFacts(facts);

    topTwoFacts.first.value = topTwoFacts.first.value &&
                                topTwoFacts.second.value;

    facts.emplace(topTwoFacts.first);
}

void ExpertSystem::OrOperator(StackToken& facts) const
{
    auto topTwoFacts = GetTopTwoFacts(facts);

    topTwoFacts.first.value = topTwoFacts.first.value ||
                                topTwoFacts.second.value;

    facts.emplace(topTwoFacts.first);
}

void ExpertSystem::XorOperator(StackToken& facts) const
{
    auto topTwoFacts = GetTopTwoFacts(facts);

    topTwoFacts.first.value = topTwoFacts.first.value ^
                                topTwoFacts.second.value;

    facts.emplace(topTwoFacts.first);
}

void ExpertSystem::NegationOperator(StackToken& facts) const
{
    auto fact = facts.top();
    facts.pop();

    fact.value = !fact.value;

    facts.emplace(fact);
}

void ExpertSystem::ImplicationOperator(StackToken& facts) const
{
    if (facts.size() != 2)
        throw std::logic_error("\033[1;31mError:\033[0m wrong input format");

    auto fact1 = facts.top();
    facts.pop();
    auto fact2 = facts.top();
    facts.pop();

    if (fact1.value != fact2.value && !fact1.value)
    {
        fact1.value = fact2.value;
        facts.emplace(fact1);
    }
}

void ExpertSystem::ProcessPriorityOperator(StackToken& facts,
    StackToken& operators) const
{
    static std::map<std::string, MethodPtr> factoryMap
    {
        {"+", &ExpertSystem::AndOperator},
        {"|", &ExpertSystem::OrOperator},
        {"!", &ExpertSystem::NegationOperator},
        {"^", &ExpertSystem::XorOperator},
        {"=>", &ExpertSystem::ImplicationOperator}
    };

    const auto topOperator = operators.top().name;
    const auto neededFunc = factoryMap.at(topOperator);

    (this->*neededFunc)(facts);

    operators.pop();
}

void    ExpertSystem::ProcessParentheses(StackToken& facts,
    StackToken& operators) const
{
    while (operators.top().name != "(")
    {
        ProcessPriorityOperator(facts, operators);
        if (operators.empty())
            throw std::logic_error("\033[1;31mError:\033[0m no open parenthes");
    }
    operators.pop();
}

void    ExpertSystem::ProcessOperator(const t_token& token,
    StackToken& facts, StackToken& operators) const
{
    while (!operators.empty() &&
        !IsOperatorMorePriority(token.name, operators.top().name))
        {
            ProcessPriorityOperator(facts, operators);
        }
    
    operators.emplace(token);
}

void   ExpertSystem::ProcessAndInConclusion(StackToken& facts,
    StackToken& operators) const
{
    if (facts.size() < 3)
        throw std::logic_error("\033[1;31mError:\033[0m wrong data format");

    auto factConclusion1 = facts.top();
    facts.pop();
    auto factConclusion2 = facts.top();
    facts.pop();
    auto state = facts.top();
    facts.pop();

    if (state.value && !factConclusion1.value && !factConclusion2.value)
    {
        factConclusion1.value = true;
        factConclusion2.value = true;

        facts.emplace(factConclusion1);
        facts.emplace(factConclusion2);
    }

    operators.pop();
    operators.pop();
}

void    ExpertSystem::MarkKnownFact(VecVecToken& dataInTokens,
    const t_token& trueFact)
{
    for (auto& vecToken : dataInTokens)
    {
        for (auto& token : vecToken)
        {
            auto isUpdatable = token.name == trueFact.name &&
                                token.value != trueFact.value;

            if (isUpdatable)
                token.value = trueFact.value;
        }
    }
}

void    ExpertSystem::MarkAllTrueFacts(VecVecToken& dataInTokens)
{
    for (auto& vecToken : dataInTokens)
    {
        for (auto& token : vecToken)
        {
            if (token.type != Fact)
                continue;

            for (const auto trueFact : _trueFacts)
            {
                if (trueFact == *token.name.cbegin())
                    token.value = true;
            }
        }
    }
}

ExpertSystem::t_token ExpertSystem::CreateTokenStruct(const std::string& line) const
{        
    return ExpertSystem::t_token 
    {
        std::regex_match(line, std::regex{"[A-Z]"}) ? Fact : Operator,
        line
    };
}

ExpertSystem::VecToken ExpertSystem::SplitOnTokensStructs(std::string line) const
{
    std::cmatch result;
    std::regex  regular{"([A-Z])|(=>)|([+!|^()])"};
    VecToken    lineTokens;

    while (std::regex_search(line.c_str(), result, regular))
    {
        lineTokens.emplace_back(CreateTokenStruct(*result.cbegin()));

        line = result.suffix().str();
    }
    return lineTokens;
}

ExpertSystem::VecVecToken ExpertSystem::Lexer(const VecStr& data)
{
    VecVecToken dataInTokens;

    dataInTokens.reserve(GetReserveNum(data));

    for (const auto& line : data)
    {
        if (line.empty())
            continue;

        dataInTokens.emplace_back(SplitOnTokensStructs(line));
    }
    return dataInTokens;
}

VecChr  ExpertSystem::GetСondition(VecStr& data, const std::string& factType)
{
    VecChr              facts;
    auto                matchFound = false;
    const std::string   logFactType{ factType == "=" ?
                                    "initial facts" :
                                    "queries" };

    for (auto& line : data)
    {
        std::cmatch         result;
        const std::regex    regular{ "\\s*" + factType + "\\s*([A-Z])*\\s*" };

        if (std::regex_match(line.c_str(), result, regular))
        {
            if (matchFound)
                throw std::logic_error{ "\033[1;31mError:\033[0m more then one " +
                    logFactType + " line" };

            facts = SplitOnSymbols(*result.begin());
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
        auto noCommentLine = line.substr(0, line.find(commentSymbol));

        noCommentsData.emplace_back(noCommentLine);
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
    auto isOk = true;

    for(auto i = 0u; i < data.size(); i++)
    {
        const std::regex reg{ "[A-Z\\s|+?\\^!=>()]*" };

        if (!std::regex_match(data.at(i).c_str(), reg))
        {
            std::cerr << "[line "
            << i + 1
            << "]\033[1;31m Error\033[0m wrong input format:\n\t"
            << data.at(i) << std::endl;

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
        std::cerr << "\033[1;31mError\033[0m Opening File\n";
        exit(1);
    }

    for (std::string line; !inFile.eof(); line = "")
    {
        std::getline(inFile, line);
        fileData.emplace_back(line);
    }

    inFile.close();

    return fileData;
}

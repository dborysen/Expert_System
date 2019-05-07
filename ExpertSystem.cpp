/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExpertSystem.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dborysen <dborysen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/30 15:05:25 by dborysen          #+#    #+#             */
/*   Updated: 2019/05/06 18:48:38 by dborysen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ExpertSystem.hpp"
#include <map>

static std::map<std::string, size_t> operationPriority = {
    {"(", 6},
    {")", 6},
    {"!", 5},
    {"+", 4},
    {"|", 3},
    {"^", 2},
    {"=>", 1},
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

    if (!ValidateData(_fileData))
        throw std::logic_error("Validation Failed");

    SaveTokens(_fileData);

    // for (const auto& line : _fileData)
    // {
    //     std::cout << line << std::endl;
    // }
}

ExpertSystem::VecVecToken ExpertSystem::Lexer(const VecStr& data)
{
    VecVecToken dataInTokens;
    
    std::cmatch result;
    std::regex regular("([A-Z])|(=>)|([+!|^()])");

    for (std::string line : data)
    {
        while (std::regex_search (line.c_str(), result, regular))
        {
            std::cout << *result.cbegin() << std::endl;
 
            line = result.suffix().str();
        }
        std::cout << std::endl;
    }
    return dataInTokens;
}

void    ExpertSystem::SaveTokens(VecStr& data)
{
    _trueFacts = GetСondition(data, "=");
    _factsToFind = GetСondition(data, "\\?");

    _dataInTokens = Lexer(data);

    // for (auto symb : _trueFacts)
    // {
    //     std::cout << symb << std::endl;
    // }

    // std::cout << std::endl;
    // for (auto symb : _factsToFind)
    // {
    //     std::cout << symb << std::endl;
    // }
    // std::cout << std::endl;

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

bool    ExpertSystem::ValidateData(const VecStr& data) const
{
    if (data.size() == 1 && *data.begin() == "")
    {
        std::cerr << "\033[1;31mError:\033[0m no data" << std::endl;
        return false;
    }

    if (!AllSymbolsValid(data))
        return false;

    return true;
}

bool    ExpertSystem::AllSymbolsValid(const VecStr& data) const
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

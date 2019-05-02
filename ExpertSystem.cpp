/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExpertSystem.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dborysen <dborysen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/30 15:05:25 by dborysen          #+#    #+#             */
/*   Updated: 2019/05/02 12:11:26 by dborysen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ExpertSystem.hpp"

ExpertSystem::ExpertSystem(const std::string& fileName)
{    
    _fileData = CutAllComments(LoadData(fileName));

    for (const auto& line : _fileData)
    {
        std::cout << line << std::endl;
    }

    ValidateData();

}

VecStr  ExpertSystem::CutAllComments(const VecStr& data) const
{
    VecStr noCommentsData = data;

    for (auto& line : data)
    {
        line.substr(0, line.find('#'));
    }

    return noCommentsData;
}


bool ExpertSystem::ValidateData() const
{
    if (_fileData.size() == 1 && *_fileData.begin() == "")
    {
        std::cerr << "\033[1;31mError:\033[0m no data" << std::endl;
        return false;
    }

    

    return true;
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

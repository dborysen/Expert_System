/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExpertSystem.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dborysen <dborysen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/23 13:44:35 by dborysen          #+#    #+#             */
/*   Updated: 2019/05/02 12:03:07 by dborysen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <vector>
#include <regex>
#include <fstream>

using VecStr = std::vector<std::string>;

class ExpertSystem
{
public:

    ExpertSystem() = delete;
    ExpertSystem(const std::string& fileName);
    ~ExpertSystem() = default;

private:
    VecStr LoadData(const std::string& fileName) const;
    VecStr CutAllComments(const VecStr& data) const;
    bool ValidateData() const;


    VecStr    _fileData;
};
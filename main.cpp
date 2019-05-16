/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dborysen <dborysen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/23 13:43:37 by dborysen          #+#    #+#             */
/*   Updated: 2019/05/16 13:05:23 by dborysen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "ExpertSystem.hpp"

int main(int argc, char** argv)
{
    try
    {
        if (argc != maxArgNum)
            throw std::logic_error("\033[1;31mError:\033[0m wrong params num");

        auto expertSystem = std::make_unique<ExpertSystem>(argv[argId]);

        expertSystem->OutputResult();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
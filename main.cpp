/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dborysen <dborysen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/23 13:43:37 by dborysen          #+#    #+#             */
/*   Updated: 2019/05/03 13:32:23 by dborysen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <memory>
#include "ExpertSystem.hpp"

int main(int argc, char** argv)
{
    try
    {
        if (argc != maxArgNum)
            throw std::logic_error("\033[1;31mError:\033[0m wrong params num");

        std::make_unique<ExpertSystem>(argv[argId]);

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
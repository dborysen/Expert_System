/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dborysen <dborysen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/23 13:43:37 by dborysen          #+#    #+#             */
/*   Updated: 2019/05/02 11:56:53 by dborysen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "ExpertSystem.hpp"

int main(int argc, char** argv)
{
    try
    {
        if (argc != 2)
            throw std::logic_error("\033[1;31mError:\033[0m wrong params num");

        std::unique_ptr<ExpertSystem> expertSystem(new ExpertSystem(argv[1]));
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
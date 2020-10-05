//
//  Split.cpp
//  MetaioOpenGLES
//
//  Created by Margrarita Papaefthimiou on 7/24/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#include "PRT/Split.h"
#include <vector>
#include <stdio.h>
#include "PRT/Utils.h"
#include <cassert>
#include <sstream>

std::vector<std::string> split(const std::string s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}
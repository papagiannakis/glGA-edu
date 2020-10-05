//
//  Split.h
//  PRT
//
//  Created by Margrarita Papaefthimiou on 7/24/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#ifndef __MetaioOpenGLES__Split__
#define __MetaioOpenGLES__Split__

#include <stdio.h>
#include "Split.h"
#include <vector>
#include <stdio.h>
#include <cassert>
#include <string>

std::vector<std::string> split(const std::string s, char delim, std::vector<std::string> &elems);

std::vector<std::string> split(const std::string s, char delim);

#endif /* Split.h */

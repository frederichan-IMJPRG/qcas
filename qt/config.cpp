/*  Copyright (C) 2001 Le Coq Loïc
*    This file is part of QCAS.
*
*    QCAS is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    QCAS is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Foobar.  If not, see <http://www.gnu.org/licenses/>
*/

#include "config.h"

Config::Config(){}

//bool Config::ortho=false;
//bool Config::autoscale=true;
int Config::graph_width=400;
bool Config::gridAttraction=true;
int Config::mml_fontsize=16;//default mml size

/* We add a prefix  to automatic names of geometric objects for storing in giac to avoid unexpected 
   non free variables names in formal calculus.
*/
QString Config::GeoVarPrefix="autoG2D\_";

QString Config::QcasVersion="0.3";

Config::LANGUAGES Config::language=Config::FRENCH;
int Config::giaclanguage=1;


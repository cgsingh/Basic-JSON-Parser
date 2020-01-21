/*
Name: Christopher Singh
Course: OOP345A
Date: October 10, 2014
*/

#include <iostream>
#include <string>

#ifndef JSONREADER_H
#define JSONREADER_H

namespace json
{
struct Collection
{
	std::string objectName;
	std::string name[50];
	std::string value[50];
};

int read(const std::string&, std::string*);
int extract(const std::string* buffer, int, int&, Collection&);
int print(const Collection&);
}

#endif
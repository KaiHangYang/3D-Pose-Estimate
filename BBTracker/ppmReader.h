#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdio.h>

class ppmReader {
public:
	std::string type;
	std::pair<int, int> size;
	int range;
	std::vector<unsigned int> data;

	ppmReader(std::string path);
};
#pragma once
#include <fstream>
#include <queue>
#include <string>

#include "Cache.h"

class Core
{
	std::ofstream* outfile_ptr_;

	Cache l1_cache_;
	Cache l2_cache_;
	Cache* l3_cache_ptr_;

public:
	Core();
	Core(unsigned long, unsigned long, const std::string&, Cache*);
	~Core();

	void pass_data(unsigned long, std::vector<std::mutex*>* = nullptr);
	void pass_data_parallel(std::vector<std::queue<unsigned long>*>*, std::vector<std::mutex*>*, std::vector<bool*>*);
};

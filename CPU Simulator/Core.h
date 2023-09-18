#pragma once
#include <fstream>
#include <mutex>
#include <queue>
#include <string>

#include "Cache.h"

class Core
{
	std::ofstream* outfile_ptr_;
	//std::string outfile_name_;

	unsigned long long core_data_entries_;

	Cache l1_cache_;
	Cache l2_cache_;
	Cache* l3_cache_ptr_;

public:
	Core();
	Core(unsigned long long, unsigned long, unsigned long, std::ofstream*, Cache*);

	void pass_data(unsigned long);
	void pass_data_parallel(std::queue<unsigned long>*, std::mutex*);

	//void pass_data_parallel(std::vector<std::queue<unsigned long>*>*, std::vector<std::mutex*>*, std::vector<bool*>*);
};

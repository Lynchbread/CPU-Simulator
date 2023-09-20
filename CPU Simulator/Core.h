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

	Cache l1_cache_;
	Cache l2_cache_;
	Cache* l3_cache_ptr_;

public:
	Core();
	Core(unsigned long, unsigned long, std::ofstream*, Cache*);
	Core(const Core&);
	~Core();

	void pass_data(unsigned long);
	void pass_data_parallel(std::queue<unsigned long>*, std::mutex*);

	//void pass_data_parallel(std::vector<std::queue<unsigned long>*>*, std::vector<std::mutex*>*, std::vector<bool*>*);
};

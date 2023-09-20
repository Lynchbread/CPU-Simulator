#pragma once
#include <fstream>
#include <list>
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
};

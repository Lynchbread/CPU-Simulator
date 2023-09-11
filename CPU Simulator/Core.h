#pragma once
#include <fstream>
#include <mutex>
#include <queue>
#include <string>

#include "Cache.h"
#include "SafeQueue.h"

class Core
{
	std::ofstream* outfile_ptr_;

	Cache l1_cache_;
	Cache l2_cache_;
	Cache* l3_cache_ptr_;

public:
	Core();
	Core(unsigned long, unsigned long, const std::string&, Cache*);
	//Core(unsigned long, unsigned long, const std::string&, Cache*, SafeQueue<unsigned long>*);
	Core(const Core&);
	~Core();

	SafeQueue<unsigned long> data_queue_;

	void pass_data(unsigned long);
	void pass_data_parallel(std::vector<std::queue<unsigned long>*>*, std::vector<std::mutex*>*, std::vector<bool*>*);

	void read_data();
};

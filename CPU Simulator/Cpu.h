#pragma once
#include <list>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "Core.h"

class Cpu
{
	static int cpu_id_;

	const unsigned long mutex_arr_size_;

	int num_core_threads_;
	unsigned long l1_cache_size_;
	unsigned long associativity_;

	Cache l3_cache_;

	std::vector<std::string> filename_vector_;

	static void read_file(const std::string&, Core*);
	void read_file(const std::string&, Core**) const;
	void run_core(const std::vector<std::string>&, int core_num);

public:
	Cpu();
	Cpu(unsigned long, unsigned long, std::vector<std::string>, int = 1);

	void ProcessData();
	void ProcessDataParallel();
};

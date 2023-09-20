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

	const unsigned long mutex_0_size_;

	int num_core_threads_;
	unsigned long l1_cache_size_;
	unsigned long associativity_;

	Cache l3_cache_;

	std::vector<std::string> filename_vector_;
	//std::mutex l3_mutex_;
	// 0 = cache, 1 = compulsory, 2 = bool
	std::mutex* l3_mutex_arr_[3];
	//std::vector<std::mutex*>* l3_mutexes_;

	void run_core(std::vector<std::string>);

public:
	Cpu();
	Cpu(unsigned long, unsigned long, std::vector<std::string>, int = 1);
	~Cpu();

	void ProcessData();
	void ProcessDataParallel();
};

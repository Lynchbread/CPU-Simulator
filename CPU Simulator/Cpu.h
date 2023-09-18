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

	//int num_data_threads_;
	int num_core_threads_;

	Cache l3_cache_;

	std::vector <Core> cores_;
	std::vector<std::thread> threads_;
	std::vector<std::string> filename_vector_;
	std::queue<unsigned long> data_queue_;
	std::mutex data_mutex_;

	void create_threads();


	//static void read_in_data(std::string, std::vector<unsigned long>*);
	//static void read_in_data2(std::queue<std::string>*, std::queue<unsigned long>*, std::mutex*, bool*);

public:
	Cpu();
	Cpu(unsigned long, unsigned long, std::vector<std::string>&, int = 1, int = 1);

	void ProcessData();
	void ProcessDataParallel();

	//void ProcessDataParallel();
	//void ProcessDataParallel2();
};

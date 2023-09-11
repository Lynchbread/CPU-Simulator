#pragma once
#include <list>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

#include "Core.h"

class Cpu
{
	//template <typename T> std::vector<std::vector<T*>> SplitVector(std::vector<T>*, int splits);

	static int cpu_id_;

	int num_data_threads_;
	int num_core_threads_;

	//std::vector<SafeQueue<unsigned long>> safe_queue_vector_;

	Cache l3_cache_;

	//std::vector <Core> cores_;
	Core** cores_;
	std::vector<std::string> filename_vector_;
	static void read_in_data(std::string, std::vector<unsigned long>*);
	static void read_in_data2(std::queue<std::string>*, std::queue<unsigned long>*, std::mutex*, bool*);


public:
	Cpu();
	Cpu(unsigned long, unsigned long, std::vector<std::string>&, int = 1, int = 1);
	~Cpu();

	void ProcessData();
	void ProcessDataParallel();
	void ProcessDataParallel2();

	void ProcessData2();

	//static void generate_threads(int, std::queue<std::string>*, std::queue<unsigned long>*, std::mutex*);
};

/*
template <typename T>
std::vector<std::vector<T*>> Cpu::SplitVector(std::vector<T>* original_vector, int splits)
{
	std::vector<std::vector<T*>> split_vectors;

	for (int i = 0; i < splits; i++)
	{
		split_vectors.emplace_back();
	}

	for (unsigned long long i = 0; i < original_vector->size();)
	{
		for (std::vector<T*>& split_vector : split_vectors)
		{
			if (i < original_vector->size())
			{
				split_vector.push_back(&(*original_vector)[i]);
				i++;
			}
		}
	}

	return split_vectors;
}*/

#include "Cpu.h"

#include <functional>
#include <iostream>
#include <list>
#include <queue>
#include <thread>

int Cpu::cpu_id_ = -1;

Cpu::Cpu() : mutex_arr_size_(0), num_core_threads_(0), l1_cache_size_(0), associativity_(0) {}

Cpu::Cpu(const unsigned long l1_cache_size, const unsigned long associativity, 
	std::vector<std::string> data_filenames, const int num_core_threads)
	: mutex_arr_size_(l1_cache_size * 192 / (associativity * 2)), num_core_threads_(num_core_threads), l1_cache_size_(l1_cache_size), associativity_(associativity),
	l3_cache_(l1_cache_size * 192, associativity * 2, true), filename_vector_(std::move(data_filenames))
{
	cpu_id_++;

	//l3_mutex_arr_ = nullptr;
	//l3_mutex_arr_ = new std::mutex[mutex_arr_size_];
}

void Cpu::run_core(const std::vector<std::string>& filename_vector)
{
	static int core_num = -1;
	core_num++;

	std::ofstream outfile("cpu" + std::to_string(cpu_id_) + "_core" + std::to_string(core_num) + "_output.txt");

	Core core(l1_cache_size_, associativity_, &outfile, &l3_cache_);
	
	for (auto& filename : filename_vector)
	{
		std::ifstream infile(filename, std::ios::binary);
		
		if (infile.is_open())
		{
			unsigned long entry;

			while (!infile.eof())
			{
				infile >> entry;
				core.pass_data(entry);
			}

			infile.close();
		}
	}

	outfile.close();
}

Cpu::~Cpu()
{
	//if (l3_mutex_arr_ != nullptr)
	//	delete[] l3_mutex_arr_;
}


void Cpu::ProcessData()
{
	auto* outfile_arr = new std::ofstream[num_core_threads_];
	auto* cores = new Core* [num_core_threads_];

	for (int i = 0; i < num_core_threads_; i++)
	{
		outfile_arr[i].open("cpu" + std::to_string(cpu_id_) + "_core" + std::to_string(i) + "_output.txt");
		cores[i] = new Core(l1_cache_size_, associativity_, &outfile_arr[i], &l3_cache_);
	}

	for(auto& filename : filename_vector_)
	{
		std::ifstream infile(filename, std::ios::binary);

		if (infile.is_open())
		{
			unsigned long entry;

			while (!infile.eof())
			{
				for (int i = 0; i < num_core_threads_; i++)
				{
					infile >> entry;
					cores[i]->pass_data(entry);
				}
			}

			infile.close();
		}
	}

	for (int i = 0; i < num_core_threads_; i++)
	{
		outfile_arr[i].close();
		delete cores[i];
	}

	delete[] outfile_arr;
	delete[] cores;
}

void Cpu::ProcessDataParallel()
{
	const auto l3_mutex_arr = new std::mutex[mutex_arr_size_];
	l3_cache_.give_mutexes(l3_mutex_arr, mutex_arr_size_);

	auto* threads = new std::thread*[num_core_threads_];
	std::vector<std::vector<std::string>> filename_vectors;

	for (int i = 0; i < num_core_threads_; i++)
		filename_vectors.emplace_back();

	for (unsigned long i = 0; i < filename_vector_.size();)
		for (int j = 0; j < num_core_threads_ && i < filename_vector_.size(); i++, j++)
			filename_vectors[j].push_back(filename_vector_[i]);

	for (int i = 0; i < num_core_threads_; i++)
		threads[i] = new std::thread(&Cpu::run_core, this, filename_vectors[i]);

	for (int i = 0; i < num_core_threads_; i++)
		threads[i]->join();

	delete[] threads;
	delete[] l3_mutex_arr;
}

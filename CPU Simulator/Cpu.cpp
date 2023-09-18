#include "Cpu.h"

#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <queue>
#include <thread>

int Cpu::cpu_id_ = -1;

Cpu::Cpu() : num_core_threads_(1), serial_(true) {}

Cpu::Cpu(const bool serial, const unsigned long l1_cache_size, const unsigned long associativity, 
	std::vector<std::string>& data_filenames, const int num_core_threads)
	: num_core_threads_(num_core_threads), serial_(serial),
	l3_cache_(l1_cache_size * 64, associativity * 2, true), filename_vector_(data_filenames)
{
	cpu_id_++;

	for (int i = 0; i < num_core_threads; i++)
	{
		cores_.emplace_back(l1_cache_size, associativity, "cpu" 
			+ std::to_string(cpu_id_) + "_core" + std::to_string(i) 
			+ "_output.txt", &l3_cache_);
	}
}

void Cpu::ProcessData()
{
	for(auto& filename : filename_vector_)
	{
		std::ifstream infile(filename, std::ios::binary);

		if (infile.is_open())
		{
			unsigned long entry;

			if (serial_)
			{
				while (!infile.eof())
				{
					for (int i = 0; i < num_core_threads_; i++)
					{
						infile >> entry;
						cores_[i].pass_data(entry);
					}
				}
			}
			else
			{
				while (!infile.eof())
				{
					std::vector<std::thread> thread_vector;
					std::vector<std::vector<std::mutex*>> mutex_vectors;

					for (int i = 0; i < num_core_threads_; i++)
					{
						mutex_vectors.emplace_back();

						// 0 for cache, 1 for comp, 2 for bool
						for (int j = 0; j < 3; j++)
						{
							mutex_vectors[i].emplace_back();
							mutex_vectors[i][j] = new std::mutex;
						}

						infile >> entry;
						thread_vector.emplace_back(&Core::pass_data, &cores_[i], entry, &mutex_vectors[i]);
					}

					for (auto& thread : thread_vector)
						thread.join();

					for (auto& vec : mutex_vectors)
					{
						for (const auto& mutex : vec)
						{
							delete mutex;
						}
					}
						

				}
			}

			infile.close();
		}
	}
}
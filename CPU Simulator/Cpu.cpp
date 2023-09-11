#include "Cpu.h"

#include <functional>
#include <iostream>
#include <list>
#include <queue>
#include <thread>

int Cpu::cpu_id_ = -1;

Cpu::Cpu() = default;

Cpu::Cpu(const unsigned long l1_cache_size, const unsigned long associativity, 
	std::vector<std::string>& data_filenames, const int num_data_threads, const int num_core_threads)
	: num_data_threads_(num_data_threads), num_core_threads_(num_core_threads),
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

void Cpu::read_in_data(std::string filename, std::vector<unsigned long>* data_queue)
{
	std::cout << "Thread Created\n";

	std::ifstream infile(filename, std::ios::binary);

	if (infile.is_open())
	{
		unsigned long entry;

		while (!infile.eof())
		{
			infile >> entry;
			data_queue->push_back(entry);
		}

		infile.close();
	}
}

void Cpu::read_in_data2(std::queue<std::string>* file_queue, std::queue<unsigned long>* data_queue, 
	std::mutex* data_mutex, bool* done)
{
	std::cout << "Thread Created2\n";

	while (!file_queue->empty())
	{
		std::ifstream infile(file_queue->front(), std::ios::binary);
		file_queue->pop();

		if (infile.is_open())
		{
			unsigned long entry;

			while (!infile.eof())
			{
				infile >> entry;
				data_mutex->lock();
				data_queue->push(entry);
				data_mutex->unlock();
			}

			infile.close();
			data_mutex->lock();
			*done = true;
			data_mutex->unlock();
		}
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

			while (!infile.eof())
			{
				for (auto& core : cores_)
				{
					infile >> entry;
					core.pass_data(entry); // Replace with thread later
				}
			}

			infile.close();
		}
	}
}

void Cpu::ProcessDataParallel()
{
	//auto mutex_arr = new std::mutex[filename_vector_.size()];
	//auto data_queues_arr = new std::queue<unsigned long, std::list<unsigned long>>[filename_vector_.size()];
	auto data_vector_arr = new std::vector<unsigned long>[filename_vector_.size()];

	int thread_count = 0;
	std::queue<std::thread> data_threads;

	for (; data_threads.size() < num_data_threads_ && data_threads.size() < filename_vector_.size(); thread_count++)
	{
		data_threads.emplace(read_in_data, filename_vector_[thread_count],
			&data_vector_arr[thread_count]);
	}

	for (int i = 0; i < filename_vector_.size(); i++)
	{
		data_threads.front().join();
		data_threads.pop();

		for (; data_threads.size() < num_data_threads_ && thread_count < filename_vector_.size(); thread_count++)
		{
			data_threads.emplace(read_in_data, filename_vector_[thread_count],
				&data_vector_arr[thread_count]);
		}

		for (auto& data : data_vector_arr[i])
		{
			cores_[0].pass_data(data);
		}

		data_vector_arr[i].clear();
	}

	while (!data_threads.empty())
	{
		data_threads.front().join();
		data_threads.pop();
	}
}

void Cpu::ProcessDataParallel2()
{
	// Each data thread gets only 1 data queue.
	// Each core can have more than 1, or even share 1.

	std::vector<std::queue<std::string>> file_queue_vector;
	std::vector<std::queue<unsigned long>> data_queue_vector; // pass to read in
	std::vector<std::vector<std::queue<unsigned long>*>> data_queue_ptr_vector_vector; // pass to cores
	auto mutex_arr = new std::mutex[num_data_threads_];
	std::vector<std::vector<std::mutex*>> mutex_vectors;
	std::vector<std::thread> data_thread_vector;
	std::vector<std::thread> core_thread_vector;

	//std::vector<int> bool_vector (num_data_threads_, 0);
	auto bool_arr = new bool[num_data_threads_];
	std::vector<std::vector<bool*>> bool_vectors;

	// initialize queues
	for (int i = 0; i < num_data_threads_; i++)
	{
		file_queue_vector.emplace_back();
		data_queue_vector.emplace_back();
		bool_arr[i] = false;
	}

	// Combined
	for (int i = 0; i < num_core_threads_; i++)
	{
		data_queue_ptr_vector_vector.emplace_back();
		mutex_vectors.emplace_back();
		bool_vectors.emplace_back();
	}

	for (int i = 0; i < num_data_threads_ || i < num_core_threads_;)
	{
		for (int j = 0; j < num_core_threads_ && i < num_data_threads_; i++, j++)
		{
			data_queue_ptr_vector_vector[j].push_back(&data_queue_vector[i]);
			mutex_vectors[j].push_back(&mutex_arr[i]);
			bool_vectors[j].push_back(&bool_arr[i]);
		}

		for (int j = 0; i < num_core_threads_ && j < num_data_threads_; i++, j++)
		{
			data_queue_ptr_vector_vector[i].push_back(&data_queue_vector[j]);
			mutex_vectors[i].push_back(&mutex_arr[j]);
			bool_vectors[i].push_back(&bool_arr[j]);
		}
	}

	// replace with SplitVector functions later
	for (int i = 0; i < filename_vector_.size();)
	{
		for (int j = 0; i < filename_vector_.size() && j < num_data_threads_; i++, j++)
		{
			file_queue_vector[j].push(filename_vector_[i]);
		}
	}

	for (int i = 0; i < num_data_threads_; i++)
	{
		data_thread_vector.emplace_back(read_in_data2, &file_queue_vector[i],
			&data_queue_vector[i], &mutex_arr[i], &bool_arr[i]);
	}

	for (int i = 0; i < num_core_threads_; i++)
	{
		core_thread_vector.emplace_back(&Core::pass_data_parallel, &cores_[i], 
			&data_queue_ptr_vector_vector[i], &mutex_vectors[i], &bool_vectors[i]);
	}

	for (auto& thread : data_thread_vector)
		thread.join();

	for (auto& thread : core_thread_vector)
		thread.join();

	delete[] mutex_arr;
	delete[] bool_arr;

}
/*
void Cpu::generate_threads(int thread_limit, std::queue<std::string>* file_queue, 
	std::queue<unsigned long>* data_queue_arr, std::mutex* mutex_arr)
{
	std::queue<std::thread> threads;

	while (!file_queue->empty())
	{
		threads.emplace(read_in_data, file_queue->front(), 
			data_queues->front(), mutex_queue->front());
		file_queue->pop();

	}
}*/

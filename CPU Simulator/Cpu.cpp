#include "Cpu.h"

//#include <functional>
#include <iostream>
#include <thread>

int Cpu::cpu_id_ = -1;

Cpu::Cpu() : mutex_arr_size_(0), num_core_threads_(0), l1_cache_size_(0), associativity_(0) {}

Cpu::Cpu(const unsigned long l1_cache_size, const unsigned long associativity, 
	std::vector<std::string> data_filenames, const int num_core_threads)
	: mutex_arr_size_(l1_cache_size * 192 / (associativity * 2)), num_core_threads_(num_core_threads), l1_cache_size_(l1_cache_size), associativity_(associativity),
	l3_cache_(l1_cache_size * 192, associativity * 2, true), filename_vector_(std::move(data_filenames))
{
	if (num_core_threads < 1)
		num_core_threads_ = 1;

	cpu_id_++;
}

void Cpu::read_file(const std::string& filename, Core* core)
{
	std::ifstream infile(filename, std::ios::binary);

	if (infile.is_open())
	{
		// May need to check last line
		int core_index = 0;
		unsigned long lc = 0;
		unsigned long item = 0;
		//char buf[2048];
		constexpr auto buf_size = 1048576 * 11;
		const auto buf = new char[buf_size];
		do
		{
			infile.read(buf, sizeof buf);
			const unsigned long long k = infile.gcount();
			for (unsigned long long i = 0; i < k; ++i)
			{
				switch (buf[i])
				{
				case '\r':
					break;
				case '\n':
					lc++;
				case ' ':
					core->pass_data(item);
					item = 0;
					break;
				case '0': case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
				case '8': case '9':
					item = 10 * item + buf[i] - '0';
					break;
				default:
					std::cerr << "Bad format\n";
				}
			}
		} while (!infile.eof());

		delete[] buf;
		infile.close();
	}
}

void Cpu::read_file(const std::string& filename, Core** cores) const
{
	std::ifstream infile(filename, std::ios::binary);

	if (infile.is_open())
	{
		// May need to check last line
		int core_index = 0;
		unsigned long lc = 0;
		unsigned long item = 0;
		//char buf[2048];
		constexpr auto buf_size = 1048576 * 11;
		const auto buf = new char[buf_size];
		do
		{
			infile.read(buf, sizeof buf);
			const unsigned long long k = infile.gcount();
			for (unsigned long long i = 0; i < k; ++i)
			{
				switch (buf[i])
				{
				case '\r':
					break;
				case '\n':
					lc++;
				case ' ':
					cores[core_index]->pass_data(item);
					core_index++;
					if (core_index == num_core_threads_) core_index = 0;
					item = 0;
					break;
				case '0': case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
				case '8': case '9':
					item = 10 * item + buf[i] - '0';
					break;
				default:
					std::cerr << "Bad format\n";
				}
			}
		} while (!infile.eof());

		delete[] buf;
		infile.close();
	}
}

void Cpu::run_core(const std::vector<std::string>& filename_vector, const int core_num)
{
	std::ofstream outfile("cpu" + std::to_string(cpu_id_) + "_core" + std::to_string(core_num) + "_output.txt");

	//Core core(l1_cache_size_, associativity_, &outfile, &l3_cache_);
	Core core(l1_cache_size_, associativity_, &outfile, nullptr);
	
	for (auto& filename : filename_vector)
		read_file(filename, &core);

	outfile.close();
}

void Cpu::ProcessData()
{
	auto* outfile_arr = new std::ofstream[num_core_threads_];
	auto* cores = new Core* [num_core_threads_];

	for (int i = 0; i < num_core_threads_; i++)
	{
		outfile_arr[i].open("cpu" + std::to_string(cpu_id_) + "_core" + std::to_string(i) + "_output.txt");
		//cores[i] = new Core(l1_cache_size_, associativity_, &outfile_arr[i], &l3_cache_);
		cores[i] = new Core(l1_cache_size_, associativity_, &outfile_arr[i], nullptr);
	}

	for(auto& filename : filename_vector_)
	{
		read_file(filename, cores);
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
	//const auto l3_mutex_arr = new std::mutex[mutex_arr_size_];
	//l3_cache_.give_mutexes(l3_mutex_arr, mutex_arr_size_);

	std::vector<std::vector<std::string>> filename_vectors;

	for (int i = 0; i < num_core_threads_; i++)
		filename_vectors.emplace_back();

	for (unsigned long i = 0; i < filename_vector_.size();)
		for (int j = 0; j < num_core_threads_ && i < filename_vector_.size(); i++, j++)
			filename_vectors[j].push_back(filename_vector_[i]);

	auto* threads = new std::thread * [num_core_threads_ - 1];

	for (int i = 0; i < num_core_threads_ - 1; i++)
		threads[i] = new std::thread(&Cpu::run_core, this, filename_vectors[i + 1], i + 1);

	run_core(filename_vectors[0], 0);

	for (int i = 0; i < num_core_threads_ - 1; i++)
		threads[i]->join();

	delete[] threads;
	//delete[] l3_mutex_arr;
}
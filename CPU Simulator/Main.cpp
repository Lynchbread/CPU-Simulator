#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "Cpu.h"

std::vector<std::string> generate_lists(unsigned long long, unsigned long);
void generate_number_list(const std::string&, unsigned long long, unsigned long, unsigned long);
unsigned long get_random_number(unsigned long min, unsigned long max);
unsigned long long get_runtime(Cpu&, void (Cpu::*)());

int main()
{
	constexpr unsigned long long data_entries = 8388608;
	constexpr unsigned long memory_addresses = 262144;
	constexpr unsigned long l1_cache_size = 256;
	constexpr unsigned long associativity = 8;
	int cpu_cores = 1;

	std::vector<std::string> data_filenames(generate_lists(data_entries, memory_addresses - 1));

	//std::vector<void(Cpu::*)> func_ptrs;
	//func_ptrs.emplace_back(&Cpu::ProcessData);

	{
		Cpu cpu(l1_cache_size, associativity, data_filenames);
		auto process_data_ptr = &Cpu::ProcessData;
		std::cout << get_runtime(cpu, process_data_ptr) / 1000000 << " ms\n";
	}

	{
		//Cpu cpu(l1_cache_size, associativity, data_filenames, 4, 4);
		//auto process_data_ptr = &Cpu::ProcessDataParallel2;
		//std::cout << get_runtime(cpu, process_data_ptr) / 1000000 << " ms\n";
	}

	return 0;
}

std::vector<std::string> generate_lists(const unsigned long long data_entries, const unsigned long max)
{
	constexpr unsigned long long data_entries_per_file = 1048576;

	std::vector<std::string> data_filenames;

	for (unsigned long long i = 0; i < data_entries / data_entries_per_file + 1; i++)
	{
		std::string filename = "uint_data" + std::to_string(i) + ".dat";

		if (i < data_entries / data_entries_per_file)
		{
			data_filenames.push_back(filename);
			generate_number_list(filename, data_entries_per_file, 0, max);
		}
		else if (data_entries % data_entries_per_file != 0)
		{
			data_filenames.push_back(filename);
			generate_number_list(filename, data_entries % data_entries_per_file, 0, max);
		}
	}

	return data_filenames;
}

void generate_number_list(const std::string& filename, const unsigned long long length, const unsigned long min, const unsigned long max)
{
	std::ofstream outfile(filename, std::ios::binary);

	// 11 characters per line
	for (unsigned long long i = 0; i < length; ++i)
		outfile << get_random_number(min, max) << '\n';

	outfile.close();

	std::cout << "Done generating list.\n";
}

unsigned long get_random_number(const unsigned long min, const unsigned long max)
{
	static std::random_device rd;
	static std::mt19937_64 gen(rd());
	std::uniform_int_distribution<unsigned long> distrib(min, max);

	return distrib(gen);
}

unsigned long long get_runtime(Cpu& cpu, void (Cpu::*func_ptr)())
{
	const auto start_time = std::chrono::high_resolution_clock::now();
	//cpu.ProcessData();
	//cpu.ProcessDataParallel();
	(cpu.*func_ptr)();
	const auto stop_time = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time);

	return duration.count();
}

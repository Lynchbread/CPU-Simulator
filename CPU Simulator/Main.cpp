#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "Cpu.h"
#include "RL23.h"

std::vector<std::string> generate_lists(unsigned long long, unsigned long);
void generate_number_list(const std::string&, unsigned long long, unsigned long, unsigned long);
unsigned long get_random_number(unsigned long min, unsigned long max);
unsigned long long get_runtime(Cpu&, void (Cpu::*)() const);

int main()
{
	constexpr unsigned long long data_entries = 4194304;
	constexpr unsigned long memory_addresses = 262144;
	constexpr unsigned long l1_cache_size = 128;
	constexpr unsigned long associativity = 8;
	constexpr int cpu_cores = 4;

	const std::vector<std::string> data_filenames(generate_lists(data_entries, memory_addresses - 1));
	std::vector<std::string> compressed_filenames(data_filenames);

	for (auto& filename : compressed_filenames)
		filename = "compressed_" + filename;

	{
		Cpu cpu(l1_cache_size, associativity, data_filenames, cpu_cores);
		const auto process_data_ptr = &Cpu::ProcessData;
		std::cout << get_runtime(cpu, process_data_ptr) / 1000000 << " ms\n";
	}

	{
		Cpu cpu(l1_cache_size, associativity, compressed_filenames, cpu_cores);
		const auto process_data_ptr = &Cpu::ProcessCompressedData;
		std::cout << get_runtime(cpu, process_data_ptr) / 1000000 << " ms\n";
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
			//generate_number_list(filename, data_entries_per_file, 0, max);
		}
		else if (data_entries % data_entries_per_file != 0)
		{
			data_filenames.push_back(filename);
			//generate_number_list(filename, data_entries % data_entries_per_file, 0, max);
		}

		//RL23::compress(filename, "compressed_" + filename);
	}

	return data_filenames;
}

void generate_number_list(const std::string& filename, const unsigned long long length, const unsigned long min, const unsigned long max)
{
	std::ofstream outfile(filename, std::ios::binary);

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

unsigned long long get_runtime(Cpu& cpu, void (Cpu::*func_ptr)() const)
{
	const auto start_time = std::chrono::high_resolution_clock::now();
	(cpu.*func_ptr)();
	const auto stop_time = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time).count();
}

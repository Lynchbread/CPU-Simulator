#pragma once
#include <list>
#include <string>
#include <vector>

#include "Core.h"

class Cpu
{
	static int cpu_id_;

	int num_core_threads_;
	unsigned long l1_cache_size_;
	unsigned long associativity_;

	Cache l3_cache_;

	std::vector<std::string> filename_vector_;

	void read_file(const std::string&, Core**) const;
	void read_compressed_file(const std::string&, Core**) const;

public:
	Cpu();
	Cpu(unsigned long, unsigned long, std::vector<std::string>, int = 1);

	void ProcessData() const;
	void ProcessCompressedData() const;
};

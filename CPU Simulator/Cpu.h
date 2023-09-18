#pragma once
#include <string>
#include <vector>

#include "Core.h"

class Cpu
{
	static int cpu_id_;

	int num_core_threads_;

	const bool serial_;

	Cache l3_cache_;

	std::vector <Core> cores_;
	std::vector<std::string> filename_vector_;

public:
	Cpu();
	Cpu(bool, unsigned long, unsigned long, std::vector<std::string>&, int = 1);

	void ProcessData();
};

#pragma once
#include <fstream>
#include <mutex>

#include "Cache.h"

class Core
{
	std::ofstream* outfile_ptr_;

	unsigned long long output_buffer_size_;
	char* output_buffer_;

	Cache l1_cache_;
	Cache l2_cache_;
	Cache* l3_cache_ptr_;

	void write_to_buffer(const char*) const;

public:
	Core();
	Core(unsigned long, unsigned long, std::ofstream*, Cache*);
	Core(const Core&);
	~Core();

	void pass_data(unsigned long);
};

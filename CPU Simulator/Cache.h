#pragma once
#include <mutex>
#include <string>
#include <vector>

class Cache
{
	const unsigned long ways_;
	const unsigned long sets_;

	bool full_;

	std::mutex* l3_mutex_ptr_;

	std::vector<unsigned long>* cache_;
	std::vector<bool> compulsory_vector_;

	std::string get_miss_type(unsigned long);

public:
	Cache();
	Cache(unsigned long, unsigned long, bool, std::mutex* = nullptr);

	~Cache();

	unsigned long find(unsigned long);
	unsigned long get_ways() const;
	std::string hit(unsigned long);
	void insert_data(unsigned long, unsigned long);
};

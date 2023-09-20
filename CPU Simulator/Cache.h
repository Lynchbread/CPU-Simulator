#pragma once
#include <mutex>
#include <string>
#include <vector>

class Cache
{
	const unsigned long ways_;
	const unsigned long sets_;

	bool full_;

	std::mutex** l3_mutex_arr_;
	//std::vector<std::mutex*>* l3_mutex_arr_;
	unsigned long l3_mutex_0_size_;
	

	std::vector<unsigned long>* cache_;
	//std::vector<std::vector<unsigned long>> cache_;
	std::vector<bool> compulsory_vector_;

	std::string get_miss_type(unsigned long);

public:
	Cache();
	Cache(unsigned long, unsigned long, bool);
	//Cache(const Cache&);
	~Cache();

	unsigned long find(unsigned long);
	unsigned long get_ways() const;
	std::string hit(unsigned long);
	void insert_data(unsigned long, unsigned long);
	void give_mutexes(std::mutex**, unsigned long);
};

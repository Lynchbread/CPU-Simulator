#include "Cache.h"

Cache::Cache() : ways_(0), sets_(0)
{
	
}

Cache::Cache(const unsigned long cache_size, const unsigned long associativity, const bool need_compulsory_vector)
	: ways_(associativity), sets_(cache_size / associativity)
{
	full_ = false;
	cache_ = new std::vector<unsigned long>[sets_];

	if (need_compulsory_vector)
		compulsory_vector_.assign(ULONG_MAX, false);
}

Cache::~Cache()
{
	delete[] cache_;
}

// Not thread safe
unsigned long Cache::find(const unsigned long data)
{
	const unsigned long set = data % sets_;

	for (auto i = static_cast<unsigned long>(cache_[set].size()); i > 0; --i)
		if (data == cache_[set][i - 1])
			return i - 1;

	return ways_;
}

// Not thread safe
std::string Cache::get_miss_type(const unsigned long data)
{
	if (!compulsory_vector_.empty() && !compulsory_vector_[data])
	{
		compulsory_vector_[data] = true;
		return "Compulsory MISS";
	}

	if (!full_)
	{
		full_ = true;

		for (unsigned long i = 0; full_ && i < sets_; i++)
			if (cache_[i].size() < ways_)
				full_ = false;
	}

	if (full_)
		return "Capacity   MISS";

	return"Conflict   MISS";
}

// Thread safe
unsigned long Cache::get_ways() const { return ways_; }

// Thread safe
std::string Cache::hit(const unsigned long data)
{
	std::string hit_type;
	const auto location = find(data);
	const unsigned long set = data % sets_;

	if (location < ways_)
		hit_type = "HIT";
	else
		hit_type = get_miss_type(data);

	insert_data(data, location);

	return hit_type;
}

// Not thread safe
void Cache::insert_data(const unsigned long data, const unsigned long way)
{
	const unsigned long set = data % sets_;

	if (way < ways_)
	{
		cache_[set].erase(cache_[set].begin() + way);
		cache_[set].push_back(data);
	}
	else
	{
		cache_[set].push_back(data);
	}

	if (cache_[set].size() > ways_)
		cache_[set].erase(cache_[set].begin());
}

#include "Cache.h"

Cache::Cache() : ways_(0), sets_(0)
{
	full_ = false;
	l3_mutex_arr_ = nullptr;
	l3_mutex_0_size_ = 0;
	cache_ = nullptr;
}

Cache::Cache(const unsigned long cache_size, const unsigned long associativity,
	const bool need_compulsory_vector)
	: ways_(associativity), sets_(cache_size / associativity)
{
	full_ = false;
	l3_mutex_arr_ = nullptr;
	l3_mutex_0_size_ = 0;
	cache_ = new std::vector<unsigned long>[sets_];

	if (need_compulsory_vector)
		compulsory_vector_.assign(ULONG_MAX, false);
}
/*
Cache::Cache(const Cache& other) : ways_(other.ways_), sets_(other.sets_)
{
	full_ = other.full_;
	l3_mutex_arr_ = other.l3_mutex_arr_;
	//cache_.assign(other.cache_.begin(), other.cache_.end());


	compulsory_vector_ = other.compulsory_vector_;

	if (!other.compulsory_vector_.empty())
	{
		//compulsory_vector_.assign(other.compulsory_vector_.begin(), other.compulsory_vector_.end());
	}
		
		
}*/

Cache::~Cache()
{
	delete[] cache_;
}

// Not thread safe
unsigned long Cache::find(const unsigned long data)
{
	const unsigned long set = data % sets_;

	if (l3_mutex_arr_ == nullptr)
	{
		for (auto i = static_cast<unsigned long>(cache_[set].size()); i > 0; --i)
			if (data == cache_[set][i - 1])
				return i - 1;
	}
	else
	{
		const unsigned long set_mutex = set % l3_mutex_0_size_;

		l3_mutex_arr_[0][set_mutex].lock();
		for (auto i = static_cast<unsigned long>(cache_[set].size()); i > 0; --i)
			if (data == cache_[set][i - 1])
			{
				l3_mutex_arr_[0][set_mutex].unlock();
				return i - 1;
			}
		l3_mutex_arr_[0][set_mutex].unlock();
	}

	return ways_;
}

// Not thread safe
std::string Cache::get_miss_type(const unsigned long data)
{
	if (l3_mutex_arr_ == nullptr)
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
	}
	else
	{
		//l3_mutex_arr_[1][0].lock();
		if (!compulsory_vector_.empty() && !compulsory_vector_[data])
		{
			compulsory_vector_[data] = true;
			//l3_mutex_arr_[1][0].unlock();
			return "Compulsory MISS";
		}
		//l3_mutex_arr_[1][0].unlock();

		
		l3_mutex_arr_[2][0].lock();
		if (!full_)
		{
			full_ = true;

			// Test inside loop too
			
			for (unsigned long i = 0; full_ && i < sets_; i++)
			{
				const unsigned long set_mutex = i % l3_mutex_0_size_;

				l3_mutex_arr_[0][set_mutex].lock();

				if (cache_[i].size() < ways_)
					full_ = false;

				l3_mutex_arr_[0][set_mutex].unlock();
			}
			
		}

		if (full_)
		{
			l3_mutex_arr_[2][0].unlock();
			return "Capacity   MISS";
		}

		l3_mutex_arr_[2][0].unlock();
	}

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

	if (l3_mutex_arr_ == nullptr)
	{
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
	else
	{
		const unsigned long set_mutex = set % l3_mutex_0_size_;

		if (way < ways_)
		{
			l3_mutex_arr_[0][set_mutex].lock();
			cache_[set].erase(cache_[set].begin() + way);
			cache_[set].push_back(data);
			//l3_mutex_arr_->unlock();
		}
		else
		{
			l3_mutex_arr_[0][set_mutex].lock();
			cache_[set].push_back(data);
			//l3_mutex_arr_->unlock();
		}

		//l3_mutex_arr_->lock();
		if (cache_[set].size() > ways_)
			cache_[set].erase(cache_[set].begin());
		l3_mutex_arr_[0][set_mutex].unlock();
	}
}

void Cache::give_mutexes(std::mutex** l3_mutex_arr, const unsigned long l3_mutex_0_size)
{
	l3_mutex_arr_ = l3_mutex_arr;
	l3_mutex_0_size_ = l3_mutex_0_size;
}

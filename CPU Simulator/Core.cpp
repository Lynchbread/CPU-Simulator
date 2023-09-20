#include "Core.h"

#include <iomanip>
#include <list>

Core::Core()
{
	outfile_ptr_ = nullptr;
	l3_cache_ptr_ = nullptr;
}

Core::Core(const unsigned long l1_cache_size,  const unsigned long associativity, 
	std::ofstream* outfile_ptr, Cache* l3_cache_ptr)
	: outfile_ptr_(outfile_ptr), l1_cache_(l1_cache_size, associativity, true),
	l2_cache_(l1_cache_size * 8, associativity / 2, false), l3_cache_ptr_(l3_cache_ptr)
{
	
}

Core::Core(const Core& other) : l1_cache_(other.l1_cache_), l2_cache_(other.l2_cache_)
{
	outfile_ptr_ = other.outfile_ptr_;
	l3_cache_ptr_ = other.l3_cache_ptr_;
}

Core::~Core()
{
	outfile_ptr_ = nullptr;
	l3_cache_ptr_ = nullptr;
}

void Core::pass_data(const unsigned long data)
{
	constexpr int spacing = 18;
	const std::string l1_result = l1_cache_.hit(data);

	*outfile_ptr_ << "L1:  " << std::left << std::setw(spacing) << l1_result;

	if (l1_result == "HIT")
	{
		l2_cache_.insert_data(data, l2_cache_.find(data));
		l3_cache_ptr_->insert_data(data, l3_cache_ptr_->find(data));
	}
	else if (l1_result == "Compulsory MISS")
	{
		l2_cache_.insert_data(data, l2_cache_.get_ways());
		const std::string l3_result = l3_cache_ptr_->hit(data);

		*outfile_ptr_ << " | L2:  " << std::setw(spacing) << l1_result << " | L3:  " << l3_result;
	}
	else
	{
		const std::string l2_result = l2_cache_.hit(data);

		*outfile_ptr_ << " | L2:  " << std::setw(spacing) << l2_result;

		if (l2_result == "HIT")
		{
			l3_cache_ptr_->insert_data(data, l3_cache_ptr_->find(data));
		}
		else
		{
			const std::string l3_result = l3_cache_ptr_->hit(data);

			*outfile_ptr_ << " | L3:  " << l3_result;
		}
	}

	*outfile_ptr_ << '\n';
}

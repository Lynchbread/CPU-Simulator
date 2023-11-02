#pragma warning(disable : 4996)

#include "Core.h"

#include <iomanip>

void Core::write_to_buffer(const char* str) const
{
	if (std::strlen(output_buffer_) + std::strlen(str) + 1 > output_buffer_size_)
	{
		outfile_ptr_->write(output_buffer_, std::strlen(output_buffer_));
		output_buffer_[0] = '\0';
	}

	std::strcat(output_buffer_, str);
}

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
	output_buffer_size_ = 16384;
	output_buffer_ = new char[output_buffer_size_];
	output_buffer_[0] = '\0';
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
	delete[] output_buffer_;
}

void Core::pass_data(const unsigned long data)
{
	constexpr int spacing = 18;
	const std::string l1_result = l1_cache_.hit(data);

	//*outfile_ptr_ << "L1:  " << std::left << std::setw(spacing) << l1_result;
	//write_to_buffer("L1:  ");
	//write_to_buffer(l1_result.c_str());

	if (l1_result == "HIT")
	{
		l2_cache_.insert_data(data, l2_cache_.find(data));

		if (l3_cache_ptr_)
			l3_cache_ptr_->insert_data(data, l3_cache_ptr_->find(data));
		
	}
	else if (l1_result == "Compulsory MISS")
	{
		l2_cache_.insert_data(data, l2_cache_.get_ways());

		if (l3_cache_ptr_)
		{
			const std::string l3_result = l3_cache_ptr_->hit(data);
			//*outfile_ptr_ << " | L2:  " << std::setw(spacing) << l1_result << " | L3:  " << l3_result;
			//write_to_buffer(" | L2:  ");
			//write_to_buffer(l1_result.c_str());
			//write_to_buffer(" | L3:  ");
			//write_to_buffer(l3_result.c_str());
		}
		else
		{
			//*outfile_ptr_ << " | L2:  " << std::setw(spacing) << l1_result;
			//write_to_buffer(" | L2:  ");
			//write_to_buffer(l1_result.c_str());
		}
	}
	else
	{
		const std::string l2_result = l2_cache_.hit(data);

		//*outfile_ptr_ << " | L2:  " << std::setw(spacing) << l2_result;
		//write_to_buffer(" | L2:  ");
		//write_to_buffer(l2_result.c_str());

		if (l3_cache_ptr_)
		{
			if (l2_result == "HIT")
			{
				l3_cache_ptr_->insert_data(data, l3_cache_ptr_->find(data));
			}
			else
			{
				const std::string l3_result = l3_cache_ptr_->hit(data);
				//*outfile_ptr_ << " | L3:  " << l3_result;
				//write_to_buffer(" | L3:  ");
				//write_to_buffer(l3_result.c_str());
			}
		}
	}

	//*outfile_ptr_ << '\n';
	//write_to_buffer("\n");
}


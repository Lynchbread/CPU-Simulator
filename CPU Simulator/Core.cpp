#include "Core.h"

#include <iomanip>

Core::Core()
{
	
}

Core::Core(const unsigned long long core_data_entries, const unsigned long l1_cache_size, 
	const unsigned long associativity, std::ofstream* outfile_ptr, Cache* l3_cache_ptr)
	: core_data_entries_(core_data_entries), l1_cache_(l1_cache_size, associativity, true),
	l2_cache_(l1_cache_size * 8, associativity / 2, false), l3_cache_ptr_(l3_cache_ptr)
{
	//outfile_ptr_ = new std::ofstream(output_filename);
	//outfile_name_ = output_filename;
	outfile_ptr_ = outfile_ptr;
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

void Core::pass_data_parallel(std::queue<unsigned long>* data_queue, std::mutex* data_mutex)
{
	for (unsigned long long entries = 0; entries < core_data_entries_; entries++)
	{
		data_mutex->lock();
		while (data_queue->empty())
		{
			data_mutex->unlock();
			data_mutex->lock();
		}

		const unsigned long data = data_queue->front();
		data_queue->pop();
		data_mutex->unlock();

		pass_data(data);
	}
}

/*
void Core::pass_data_parallel(std::vector<std::queue<unsigned long>*>* data_queues_ptr, 
	std::vector<std::mutex*>* mutex_vector_ptr, std::vector<bool*>* bool_vector_ptr)
{
	for (int i = 0; i < data_queues_ptr->size() && i < mutex_vector_ptr->size(); i++)
	{
		(*mutex_vector_ptr)[i]->lock();
		while (!(*data_queues_ptr)[i]->empty() || *(*bool_vector_ptr)[i] == false)
		{
			if (!(*data_queues_ptr)[i]->empty())
			{
				const unsigned long data = (*data_queues_ptr)[i]->front();
				(*data_queues_ptr)[i]->pop();
				(*mutex_vector_ptr)[i]->unlock();
				pass_data(data);
			}
			else
			{
				(*mutex_vector_ptr)[i]->unlock();
			}

			(*mutex_vector_ptr)[i]->lock();
		}
		(*mutex_vector_ptr)[i]->unlock();
	}
}
*/



#pragma once
#include <mutex>
#include <queue>

template<class T, class Container = std::deque<T>>
class SafeQueue
{
public:
	using reference = typename Container::reference;
	using size_type = typename Container::size_type;
	using value_type = typename Container::value_type;

protected:
	std::queue<T, Container> queue_;
	std::mutex* queue_mutex_;
	std::mutex* bool_mutex_;
	bool done_ = false;

public:
	SafeQueue();
	SafeQueue(const SafeQueue&);
	~SafeQueue();

	reference& back();
	reference& deque();
	bool empty();
	reference& front();
	void pop();
	void push(const value_type&);
	void push(value_type&&);
	size_type size();

	void set_done(bool);
	bool is_done();
};

template <class T, class Container>
SafeQueue<T, Container>::SafeQueue()
{
	queue_mutex_ = new std::mutex;
	bool_mutex_ = new std::mutex;
}

template <class T, class Container>
SafeQueue<T, Container>::SafeQueue(const SafeQueue& other)
{
	queue_ = other.queue_;
	queue_mutex_ = other.queue_mutex_;
	bool_mutex_ = other.bool_mutex_;
}

template <class T, class Container>
SafeQueue<T, Container>::~SafeQueue()
{
	delete queue_mutex_;
	delete bool_mutex_;
}

template <class T, class Container>
typename SafeQueue<T, Container>::reference& SafeQueue<T, Container>::back()
{
	queue_mutex_->lock();
	reference& val = queue_.back();
	queue_mutex_->unlock();

	return val;
}

template <class T, class Container>
typename SafeQueue<T, Container>::reference& SafeQueue<T, Container>::deque()
{
	queue_mutex_->lock();
	reference& val = queue_.front();
	queue_.pop();
	queue_mutex_->unlock();

	return val;
}

template <class T, class Container>
bool SafeQueue<T, Container>::empty()
{
	queue_mutex_->lock();
	const bool empty = queue_.empty();
	queue_mutex_->unlock();

	return empty;
}

template <class T, class Container>
typename SafeQueue<T, Container>::reference& SafeQueue<T, Container>::front()
{
	queue_mutex_->lock();
	reference& val = queue_.front();
	queue_mutex_->unlock();

	return val;
}

template <class T, class Container>
void SafeQueue<T, Container>::pop()
{
	queue_mutex_->lock();
	queue_.pop();
	queue_mutex_->unlock();
}

template <class T, class Container>
void SafeQueue<T, Container>::push(const value_type& val)
{
	queue_mutex_->lock();
	queue_.push(val);
	queue_mutex_->unlock();
}

template <class T, class Container>
void SafeQueue<T, Container>::push(value_type&& val)
{
	queue_mutex_->lock();
	queue_.push(val);
	queue_mutex_->unlock();
}

template <class T, class Container>
typename SafeQueue<T, Container>::size_type SafeQueue<T, Container>::size()
{
	queue_mutex_->lock();
	size_type size = queue_.size();
	queue_mutex_->unlock();

	return size;
}

template <class T, class Container>
void SafeQueue<T, Container>::set_done(bool status)
{
	bool_mutex_->lock();
	done_ = status;
	bool_mutex_->unlock();
}

template <class T, class Container>
bool SafeQueue<T, Container>::is_done()
{
	bool_mutex_->lock();
	const bool done = done_;
	bool_mutex_->unlock();

	return done;
}


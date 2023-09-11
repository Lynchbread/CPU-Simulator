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
	std::mutex mutex_;

public:
	SafeQueue() = default;

	reference& back();
	reference& deque();
	bool empty();
	reference& front();
	void pop();
	void push(const value_type&);
	void push(value_type&&);
	size_type size();
};

template <class T, class Container>
typename SafeQueue<T, Container>::reference& SafeQueue<T, Container>::back()
{
	mutex_.lock();
	reference& val = queue_.back();
	mutex_.unlock();

	return val;
}

template <class T, class Container>
typename SafeQueue<T, Container>::reference& SafeQueue<T, Container>::deque()
{
	mutex_.lock();
	reference& val = queue_.front();
	queue_.pop();
	mutex_.unlock();

	return val;
}

template <class T, class Container>
bool SafeQueue<T, Container>::empty()
{
	mutex_.lock();
	const bool empty = queue_.empty();
	mutex_.unlock();

	return empty;
}

template <class T, class Container>
typename SafeQueue<T, Container>::reference& SafeQueue<T, Container>::front()
{
	mutex_.lock();
	reference& val = queue_.front();
	mutex_.unlock();

	return val;
}

template <class T, class Container>
void SafeQueue<T, Container>::pop()
{
	mutex_.lock();
	queue_.pop();
	mutex_.unlock();
}

template <class T, class Container>
void SafeQueue<T, Container>::push(const value_type& val)
{
	mutex_.lock();
	queue_.push(val);
	mutex_.unlock();
}

template <class T, class Container>
void SafeQueue<T, Container>::push(value_type&& val)
{
	mutex_.lock();
	queue_.push(val);
	mutex_.unlock();
}

template <class T, class Container>
typename SafeQueue<T, Container>::size_type SafeQueue<T, Container>::size()
{
	mutex_.lock();
	size_type size = queue_.size();
	mutex_.unlock();

	return size;
}


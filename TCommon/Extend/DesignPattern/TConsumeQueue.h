#ifndef _TCONSUMEQUEUE_H_
#define _TCONSUMEQUEUE_H_


#include <functional>
#include <memory>
#include <thread>
#include <queue>
#include <condition_variable>

//消费队列
/*
新建一个线程,以固定的速率处理某项任务
范例: 
	TConsumeQueue<T> comsumeQueue(10);
	comsumeQueue.consume(Fun);
	T t;
	comsumeQueue.publish(t);
*/
template<typename T>
class TConsumeQueue
{
	typedef std::function<void(T)> ConsumeFunction;
public:
	//consumeRate: 消耗速率(消息个数/1ms)
	TConsumeQueue(int consumeRate, 
		int publishQueueMaxSize = USHRT_MAX, 
		ConsumeFunction consumeFunction = nullptr);
	~TConsumeQueue() { stop(); };
	void publish(T data);
	void consume(ConsumeFunction consumeFunction);
	void stop();
	void setConsumeRate(int rate);
	int  getConsumeRate();
	int  getPublishQueueLength();
private:
	void consumeThreadFunction(ConsumeFunction consumeFunction);
private:
	std::thread*   m_pThread = nullptr;
	//生产者队列
	std::queue<T>  m_queuePublish;
	int            m_iConsumeRate;
	int            m_iPublishQueueMaxSize;
	std::condition_variable m_wait;
	std::mutex              m_mutex;
	bool           m_isStop = false;

	std::chrono::microseconds	m_mSleepTime;
};

template<typename T>
int TConsumeQueue<T>::getPublishQueueLength()
{
	return m_queuePublish.size();
}

template<typename T>
int TConsumeQueue<T>::getConsumeRate()
{
	return m_iConsumeRate;
}

template<typename T>
void TConsumeQueue<T>::setConsumeRate(int rate)
{
	if (m_iConsumeRate == rate)
	{
		return;
	}
	m_iConsumeRate = rate;

	m_mSleepTime = std::chrono::microseconds(1000000 / m_iConsumeRate);
}

template<typename T>
void TConsumeQueue<T>::stop()
{
	{
		std::unique_lock<std::mutex> locker(m_mutex);
		m_isStop = true;
		m_wait.notify_all();
	}
	if (m_pThread)
	{
		m_pThread->join();
		delete m_pThread;
		m_pThread = nullptr;
	}
}

template<typename T>
void TConsumeQueue<T>::publish(T data)
{
	if (m_isStop)
	{
		return;
	}
	std::unique_lock<std::mutex> locker(m_mutex);
	m_queuePublish.push(data);
	if (m_queuePublish.size() > m_iPublishQueueMaxSize)
	{
		m_queuePublish.pop();
	}
	m_wait.notify_all();
}

template<typename T>
void TConsumeQueue<T>::consume(ConsumeFunction consumeFunction)
{
	if (m_pThread)
	{
		stop();
	}
	m_pThread = new std::thread([=]() {
		consumeThreadFunction(consumeFunction);
		});
}

template<typename T>
TConsumeQueue<T>::TConsumeQueue(int consumeRate, 
	int publishQueueMaxSize,
	ConsumeFunction consumeFunction /*= nullptr*/)
{
	//m_iConsumeRate = consumeRate;
	setConsumeRate(consumeRate);
	m_iPublishQueueMaxSize = publishQueueMaxSize;
	if (consumeFunction)
		consume(consumeFunction);
}

template<typename T>
void TConsumeQueue<T>::consumeThreadFunction(ConsumeFunction consumeFunction)
{
	while (!m_isStop)
	{
		std::unique_lock<std::mutex> locker(m_mutex);
		if (m_queuePublish.empty())
		{
			m_wait.wait(locker);
		}
		if (m_isStop)
		{
			return;
		}
		std::queue<T> consumeQueue;
		consumeQueue.swap(m_queuePublish);
		locker.unlock();
		while (!consumeQueue.empty() && !m_isStop)
		{
			consumeFunction(consumeQueue.front());
			consumeQueue.pop();
			std::this_thread::sleep_for(m_mSleepTime);
		}
	}
}


#endif // _TCONSUMEQUEUE_H_
/*****************************************************************//**
 * \file   Cache.h
 * \brief  FIFO cache manager template
 * Two containing levels:
 * Cache  -(CacheItem)  -(T)
 *
 * \author Yuefeng
 * \date   Jan 2020
 *********************************************************************/
#pragma once

#include <QList>
#include <QMutex>

template<class T>
struct CacheItem {
	unsigned int		id = -1;		///< incremental, to identify the item. int32_t allows 2.1 billion, enough for any known application senario.
	std::atomic_bool	lock = false;		///< When true, poping is not allowed to be applied to it; meanwhile, counting the limit is not counted into the total sum.
	T content;

	CacheItem<T>() :
		id(-1),		///< to identify invalid
		lock(false),
		content()
	{
	};

	CacheItem<T>(
		int32_t id,
		bool lock,
		T content
	) :
		id(id),
		lock(lock),
		content(content)
	{
	};
};

/*
* \brief FIFO cache. Push newest, pop oldest.
*/
template<class T>
class TThreadSafeQueueCache
{
public:
	TThreadSafeQueueCache(int size);							///< Limit the size
	virtual void push(const T& one);
	virtual bool pop();
	virtual void popAfterMerge();
	virtual void setLimit(int newSize);
	virtual int limit();
	virtual int length();
	virtual int removableCount();
	virtual void lockAt(int index);				///< set the lock flag on item "index", which prevent its removal. Usually, when needing any processing to the item in the list, you have to lock it first.
	virtual void lockOne(uint32_t id);			///< lock by its id.
	virtual void unlockOne(uint32_t id);		///< release the lock, meaning no processing is applied to that item any longer.
	virtual T& lockAndPeekOne(uint32_t id);		///< atom operation of lock and peek
	virtual CacheItem<T>& lockAndPeekOneItem(uint32_t id);

	/**
	 * \brief 注意！！Removal at the "T" level, not the "CacheItem" level. Since T is used in template and not having class properties,
	 * we cannot define remove() in T as the interface. Must be realized in derived class, since pop needs it
	 *
	 */
	virtual bool remove(const T&) = 0;
	virtual bool removeAll(void);					/*remove all elements*/

	/**
	 * @brief peek the latest one, the newest one, i.e. the tail end of list
	 * @param
	 * @return
	*/
	virtual T& peek(void);

	/**
	 * @brief peek the oldest one, i.e. the head end of list
	 * @param
	 * @return
	*/
	virtual T& peekOldest(void);

	/**
	 * @brief peek the one at index. The older the item is in the list, the index is smaller.
	 * @param
	 * @return
	*/
	virtual T& peekAt(int index);
	virtual T& peekOne(uint32_t id);
	virtual CacheItem<T>& peekOneItem(uint32_t id);
	virtual CacheItem<T>& peekItem(void);

	virtual int getStartId();
	virtual void setStartId(int newId);

	virtual int getIndexById(int id);

protected:
	QList<CacheItem<T>>	m_lstCache;					///< all items are stored with incremental "id"
	int					m_iMaxObjectLimit;			///< Max length of list.
	int					m_iUnlockedCount;			///< sum of the unlocked ones, may be different with list.size(), == list.size() - locked ones

private:
	QMutex			m_mutex_push;				///< used for locking for all the operation may cause item added
	QMutex			m_mutex_pop;				///< used for locking for all the operation may cause item removed
	QMutex			m_mutex_setLimit;

	unsigned int	m_iId;
	T				m_invalid;				///< used for return peek with invalid return. Therefore, the T must provide default constructor.
	CacheItem<T>	m_invalidItem;
};

template<class T>
inline TThreadSafeQueueCache<T>::TThreadSafeQueueCache(int size) :
	m_lstCache(),
	m_iMaxObjectLimit(size),
	m_iUnlockedCount(0),
	m_mutex_push(),
	m_mutex_pop(),
	m_mutex_setLimit(),
	m_iId(-1),
	m_invalid(),
	m_invalidItem()
{
}

template<class T>
inline void TThreadSafeQueueCache<T>::push(const T& one)
{
	QMutexLocker lock(&m_mutex_push);
	m_iId++;
	m_lstCache.append({ m_iId, false, one });
	m_iUnlockedCount++;
	while (length() > m_iMaxObjectLimit) {
		if (!pop()) {
			// if pop() failed, meaning all items are locked or some other reason, pop failed, 
			// (length() > maxObjectLimit) will always be true, then, no significance to try to pop again. Lets pop in the future.
			break;
		}
	}
}

/**
* \brief Pop the oldest element
*
*/
template<class T>
inline bool TThreadSafeQueueCache<T>::pop()
{
	QMutexLocker lock(&m_mutex_pop);
	if (m_iUnlockedCount > 0) {
		int i = 0;
		while (m_lstCache.at(i).lock == true) {
			i++;
		}
		if (!remove(m_lstCache.at(i).content)) {
			//assert(0 && "remove step failed.");
			return false;
		}
		m_lstCache.removeAt(i);
		m_iUnlockedCount--;
		return true;
	}
	else {
		return false;
	}
}

template<class T>
inline void TThreadSafeQueueCache<T>::popAfterMerge()
{
	QMutexLocker lock(&m_mutex_pop);
	if (m_iUnlockedCount > 0)
	{
		int i = 0;
		while (m_iUnlockedCount > m_iMaxObjectLimit && i < m_lstCache.size())
		{
			if (!remove(m_lstCache.at(i).content))
			{
				++i;
				continue;
			}
			m_lstCache.removeAt(i);
			m_iUnlockedCount--;
		}
	}
}

template<class T>
inline void TThreadSafeQueueCache<T>::setLimit(int newSize)
{
	QMutexLocker lock(&m_mutex_setLimit);
	while (removableCount() > newSize) {
		pop();
	}
	m_iMaxObjectLimit = newSize;
}

/**
* \brief Maximum size.
*/
template<class T>
inline int TThreadSafeQueueCache<T>::limit()
{
	return m_iMaxObjectLimit;
}

template<class T>
inline int TThreadSafeQueueCache<T>::length()
{
	return m_lstCache.size();
}

/**
* \brief Peek the newest one.
*/
template<class T>
inline T& TThreadSafeQueueCache<T>::peek(void)
{
	return peekAt(length() - 1);
}

/**
* \brief Peek the newest one.
*/
template<class T>
inline T& TThreadSafeQueueCache<T>::peekOldest(void)
{
	return peekAt(0);
}

template<class T>
inline T& TThreadSafeQueueCache<T>::peekAt(int index)
{
	QMutexLocker lock(&m_mutex_pop);
	return m_lstCache[index].content;
}

template<class T>
inline T& TThreadSafeQueueCache<T>::peekOne(uint32_t id)
{
	for (auto& item : m_lstCache) {
		if (item.id == id) {
			return item.content;
		}
	}
	return m_invalid;
}

template<class T>
inline CacheItem<T>& TThreadSafeQueueCache<T>::peekOneItem(uint32_t id)
{
	QMutexLocker lock(&m_mutex_pop);
	for (auto& item : m_lstCache) {
		if (item.id == id) {
			return item;
		}
	}
	return m_invalidItem;
}

template<class T>
inline CacheItem<T>& TThreadSafeQueueCache<T>::peekItem(void)
{
	return m_lstCache.back();
}

template<class T>
inline CacheItem<T>& TThreadSafeQueueCache<T>::lockAndPeekOneItem(uint32_t id)
{
	QMutexLocker lock(&m_mutex_pop);
	for (auto& item : m_lstCache) {
		if (item.id == id) {
			item.lock = true;
			m_iUnlockedCount--;
			return item;
		}
	}
	return m_invalidItem;
}

template<class T>
inline T& TThreadSafeQueueCache<T>::lockAndPeekOne(uint32_t id)
{
	QMutexLocker lock(&m_mutex_pop);
	for (auto& item : m_lstCache) {
		if (item.id == id) {
			item.lock = true;
			m_iUnlockedCount--;
			return item.content;
		}
	}
	return m_invalid;
}

template<class T>
inline int TThreadSafeQueueCache<T>::removableCount()
{
	return m_iUnlockedCount;
}

template<class T>
inline void TThreadSafeQueueCache<T>::lockAt(int index)
{
	QMutexLocker lock(&m_mutex_push);
	m_lstCache[index].lock = true;
	m_iUnlockedCount--;
}

template<class T>
inline void TThreadSafeQueueCache<T>::lockOne(uint32_t id)
{
	for (auto& item : m_lstCache) {
		if (item.id == id) {
			item.lock = true;
			m_iUnlockedCount--;
			break;
		}
	}
}

template<class T>
inline void TThreadSafeQueueCache<T>::unlockOne(uint32_t id)
{
	for (auto& item : m_lstCache) {
		if (item.id == id) {
			if (item.lock == true) {
				item.lock = false;
				m_iUnlockedCount++;
			}
			break;
		}
	}

	if (m_iUnlockedCount > m_iMaxObjectLimit) {
		pop();
	}
}

template<class T>
inline bool TThreadSafeQueueCache<T>::removeAll(void) {
	m_lstCache.clear();
	m_iUnlockedCount = 0;
	return true;
}

template<class T>
inline int TThreadSafeQueueCache<T>::getStartId()
{
	return m_iId;
}

template<class T>
inline void	TThreadSafeQueueCache<T>::setStartId(int newId)
{
	if (newId >= 0)
	{
		QMutexLocker lock(&m_mutex_push);
		m_iId = newId;
	}
}

template<class T>
inline int TThreadSafeQueueCache<T>::getIndexById(int id)
{
	int index = -1;
	for (int i = 0; i < m_lstCache.size(); ++i)
	{
		if (m_lstCache.at(i).id == id)
		{
			index = i;
			break;
		}
	}

	return index;
}
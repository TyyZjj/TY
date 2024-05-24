#ifndef _TFIND_H_
#define _TFIND_H_

//查找相关的算法

//1、QList顺序查找
namespace TListFind {
#pragma region QList-Find
	template<typename T, typename FindFun>
	int Find(const QList<T> &lst, FindFun findFun)
	{
		for (int index = 0; index < lst.count(); ++index)
		{
			if (findFun(lst.at(index)))
				return index;
		}
		return lst.count();
	}

	template<typename T>
	int Find(const QList<T> &lst, const T &t)
	{
		for (int index = 0; index < lst.count(); ++index)
		{
			if (lst.at(index) == t)
				return index;
		}
		return lst.count();
	}

	template<typename T2, typename T1, typename FindFun>
	int Find(const QList<T2> &lst, const T1 &t, FindFun findFun, bool sequence = true)
	{
		if (sequence)
		{
			for (int index = 0; index < lst.count(); ++index)
			{
				if (findFun(lst.at(index), t))
					return index;
			}
		}
		else
		{
			for (int index = lst.count() - 1; index >= 0; index--)
			{
				if (findFun(lst.at(index), t))
					return index;
			}
		}
		return lst.count();
	}

	//************************************
	// 参数: 	QList<T1> & lst : 查找集
	// 参数: 	const QList<T2> & lstFilter : 过滤集
	// 参数: 	FindFun findFun
	// 参数: 	bool bFilterFind: true->过滤找到的; false->过滤未找到的
	// 返回:	void
	// 功能:	过滤lst
	//************************************
	template<typename T1, typename T2 = T1, typename FindFun>
	void Filter(QList<T1> &lst, const QList<T2> &lstFilter, FindFun findFun, bool bFilterFind = true)
	{
		for (int index = 0; index < lst.count();)
		{
			const T1 &t = lst.at(index);
			int iFindIndex = Find(lstFilter, t, findFun);
			if (iFindIndex < lstFilter.count())
			{
				if (bFilterFind)
				{
					lst.removeAt(index);
					continue;
				}
			}
			else
			{
				if (!bFilterFind)
				{
					lst.removeAt(index);
					continue;
				}
			}
			index++;
		}
	}
#pragma endregion


#pragma region QString
	template<typename T>
	bool contains(QString str, QList<T> lst)
	{
		for (auto &t : lst)
		{
			if (str.contains(t))
				return true;
		}
		return false;
	}

	template<typename T>
	int indexOf(QString str, QList<T> lst, int from = 0)
	{
		int res(str.count());
		for (auto &t : lst)
		{
			int index = str.indexOf(t, from);
			if (index >= 0)
				res = qMin(index, res);
		}

		return (res < str.count()) ? res : -1;
	}

	template<typename T>
	int indexOf(QString str, QList<T> lst, T &sub, int from = 0)
	{
		int res(str.count());
		for (auto &t : lst)
		{
			int index = str.indexOf(t, from);
			if (index >= 0)
			{
				if (index < res)
				{
					sub = t;
					res = index;
				}
			}
		}

		return (res < str.count()) ? res : -1;
	}

	template<typename T>
	int lastIndexOf(QString str, QList<T> lst, int from = -1)
	{
		int res(-1);
		for (auto &t : lst)
		{
			int index = str.lastIndexOf(t, from);
			if (index >= 0)
				res = qMax(index, res);
		}
		return res;
	}

	template<typename T>
	int lastIndexOf(QString str, QList<T> lst, T &sub, int from = -1)
	{
		int res(-1);
		for (auto &t : lst)
		{
			int index = str.lastIndexOf(t, from);
			if (index >= 0)
			{
				if (index > res)
				{
					sub = t;
					res = index;
				}
			}
		}
		return res;
	}
#pragma endregion
};




#endif

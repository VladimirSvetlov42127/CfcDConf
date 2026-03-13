#pragma once
#include <vector>
#include <cstdint>
#include <algorithm>

template< class T >
class DcMatrixPool
{
public:
	DcMatrixPool() {};
	virtual ~DcMatrixPool() {};

	bool add(T elem, bool from_db) {
		datavect.push_back(elem);
		if (!from_db)
			elem->insert();
		return true;
	}

	T get(uint32_t position) {
		if (position >= datavect.size())
			return nullptr;
		return datavect[position];
	}

	uint32_t size(void) {
		return datavect.size();
	}

	T get(int32_t src, int32_t dst)
	{
		if ((src < 1)||(dst < 1))
			return nullptr;
		for (T pelement : datavect) {
			if ((pelement->src() == src)&&(pelement->dst() == dst))
				return pelement;
		}
		return nullptr;
	}

	T get(int32_t src)
	{
		if (src < 1)
			return nullptr;
		for (T pelement : datavect) {
			if (pelement->src() == src) {
				return pelement;
			}
		}
		return nullptr;
	}

	T getDst(int32_t id)
	{
		if (id < 1)
			return nullptr;
		for (T pelement : datavect) {
			if (pelement->dst() == id) {
				return pelement;
			}
		}
		return nullptr;
	}

	void remove(int32_t src, int32_t dst) {
		if ((src < 1) || (dst < 1))
			return;
		T pelem = get(src, dst);
		if (pelem == nullptr)
			return;
		pelem->remove();
		auto it = std::find(datavect.begin(), datavect.end(), pelem);
		if (it != datavect.end())
			datavect.erase(it);
	}

	void remove(uint32_t position) {
		if (position >= datavect.size())
			return;
		T pelement = datavect[position];
		if (pelement == nullptr)
			return;
		pelement->remove();
		auto it = std::find(datavect.begin(), datavect.end(), pelement);
		if (it != datavect.end())
			datavect.erase(it);
	}

	void removeBySourse(int32_t src) {
		if (src < 1)
			return;
		T pelem = get(src);
		if (pelem == nullptr)
			return;
		pelem->remove();
		auto it = std::find(datavect.begin(), datavect.end(), pelem);
		if (it != datavect.end())
			datavect.erase(it);
	}

	void removeByDest(int32_t dst) {
		if (dst < 1)
			return;
		T pelem = getDst(dst);
		if (pelem == nullptr)
			return;
		pelem->remove();
		auto it = std::find(datavect.begin(), datavect.end(), pelem);
		if (it != datavect.end())
			datavect.erase(it);
	}

	void removeAll() {
		for (auto ptr : datavect) {
			ptr->remove();
			delete ptr;
		}
		datavect.clear();
	}

	bool exist(int32_t src, int32_t dst) {
		for (T pelement : datavect) {
			if ((pelement->src() == src) && (pelement->dst() == dst))
				return true;
		}
		return false;
	}

	bool existSrc(int32_t src) {
		for (T pelement : datavect) {
			if (pelement->src() == src)
				return true;
		}
		return false;
	}

	bool existDst(int32_t dst) {
		for (T pelement : datavect) {
			if (pelement->dst() == dst)
				return true;
		}
		return false;
	}

	void clear() {
		for (T elem : datavect) {
			if (elem != nullptr) {
				delete elem;
				elem = nullptr;
			}
		}
		datavect.clear();
	}

	uint32_t getCountBySourse(int32_t src) {
		uint32_t count = 0;
		for (T pelement : datavect) {
			if (pelement->src() == src)
				count++;
		}
		return count;
	}

	uint32_t getCountByDest(int32_t dest) {
		uint32_t count = 0;
		for (T pelement : datavect) {
			if (pelement->dst() == dest)
				count++;
		}
		return count;
	}

private:
	std::vector<T> datavect;
};

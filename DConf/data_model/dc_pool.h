#pragma once
#include <vector>
#include <cstdint>
#include <algorithm>

template< class T >
class DcPoolSingleKey
{
public:
	DcPoolSingleKey() {};
	virtual ~DcPoolSingleKey() {};
	
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

	T getById(int32_t index)
	{
		if (index < 1)
			return nullptr;
		for (T pelement : datavect) {
			if (pelement->index() == index)
				return pelement;
		}
		return nullptr;
	}

	void removeById(int32_t index) {
		if (index < 1)
			return;
		T pelem = getById(index);
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

	void removeAll() {
		for (auto ptr : datavect) {
			ptr->remove();
			delete ptr;
		}
		datavect.clear();
	}

	bool exist(int32_t index) {
		for (T pelement : datavect) {
			if (pelement->index() == index)
				return true;
		}
		return false;
	}

	int32_t maxIndex(void) {
		int32_t maxIndex = 0;

		for (T pelement : datavect) {
			if (pelement->index() > maxIndex)
				maxIndex = pelement->index();
		}
		return maxIndex;
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

	void sort() {
		std::sort(datavect.begin(), datavect.end(), [](T a, T b) { return a->index() < b->index(); });
	}

private:
	std::vector<T> datavect;
};

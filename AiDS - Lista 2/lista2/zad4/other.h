#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <stack>

namespace SortType
{
	enum SortType
	{
		NONE, INSERT, MERGE, QUICK, HYBRID, DUAL
	};
}

namespace Comparator
{
	enum Comparator
	{
		NONE, INCREASING, DECREASING
	};
}

typedef struct Stats
{
	size_t size;
	int compares, swaps;
	double time;
} Stats;

typedef struct Range
{
	size_t lo, hi;
} Range;

template <class t>
void swap(std::vector<t> &data, size_t i, size_t j)
{
	std::swap(data[i], data[j]);
}

template <class t>
bool compare(Comparator::Comparator comp, t i, t j, bool strict)
{
	switch (comp)
	{
	case Comparator::INCREASING:
		return (strict) ? (i < j) : (i <= j);
	case Comparator::DECREASING:
		return (strict) ? (i > j) : (i >= j);
	default:
		return false;
	}
}

template <class t>
bool vector_sorted(std::vector<t> &data, Comparator::Comparator comp) {
	for (size_t i = 1; i < data.size(); i++)
	{
		if (!compare(comp, data[i - 1], data[i], false))
		{
			return false;
		}
	}

	return true;
}

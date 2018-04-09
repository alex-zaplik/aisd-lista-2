#pragma once

#include "other.h"

template <class t>
Stats* insertion_sort(std::vector<t> &data, Comparator::Comparator comp)
{
	Stats* stats = new Stats{ data.size(), 0, 0, 0.0 };
	size_t i, j;

	for (i = 1; i < data.size(); i++)
	{
		j = i;

		while (j > 0)
		{
			stats->compares++;

			if (!compare(comp, data[j - 1], data[j], false))
			{
				stats->swaps++;
				swap<t>(data, j, j - 1);
				j--;
			}
			else
			{
				break;
			}
		}
	}

	return stats;
}

template <class t>
Stats* merge_sort(std::vector<t> &data, Comparator::Comparator comp)
{
	Stats* stats = new Stats{ data.size(), 0, 0, 0.0 };
	std::vector<t> secondary(data.size());

	for (size_t jump = 1; jump < data.size(); jump *= 2)
	{
		for (size_t x = 0; x < data.size(); x += jump * 2)
		{
			size_t min = x;
			size_t max = std::min(data.size(), (size_t)x + jump * 2);

			auto left = data.begin() + min;
			auto right = data.begin() + std::min(data.size(), (size_t)x + jump);
			auto end = data.begin() + max;

			auto i = left;
			auto j = right;

			for (size_t it = min; it < max; it++)
			{
				if (i != right && j != end)
				{
					stats->compares++;
				}

				if (i != right && (j == end || compare(comp, *i, *j, false)))
				{
					secondary[it] = *i;
					i++;
				}
				else
				{
					if (i != right)
					{
						stats->swaps++;
					}

					secondary[it] = *j;
					j++;
				}
			}
		}

		data.swap(secondary);
	}

	return stats;
}

template <class t>
size_t partition(std::vector<t> &data, Comparator::Comparator comp, Stats* stats, Range range)
{
	size_t p_index = (rand() % (range.hi - range.lo)) + range.lo;
	t pivot = data[p_index];

	swap(data, range.lo, p_index);
	stats->swaps++;

	size_t i = range.lo;
	size_t j;

	for (j = range.lo + 1; j <= range.hi; j++)
	{
		stats->compares++;

		if (compare(comp, data[j], pivot, true))
		{
			stats->swaps++;
			swap(data, ++i, j);
		}
	}

	swap(data, i, range.lo);

	return i;
}

template <class t>
Stats* quick_sort(std::vector<t> &data, Comparator::Comparator comp)
{
	Stats* stats = new Stats{ data.size(), 0, 0, 0.0 };
	std::stack<Range> sort_ranges;

	sort_ranges.push({ 0, data.size() - 1 });
	while (!sort_ranges.empty())
	{
		Range range = sort_ranges.top();
		sort_ranges.pop();

		if (range.lo < range.hi)
		{
			size_t middle = partition(data, comp, stats, range);
			if (middle > 0) sort_ranges.push({ range.lo, middle - 1 });
			sort_ranges.push({ middle + 1, range.hi });
		}
	}

	return stats;
}

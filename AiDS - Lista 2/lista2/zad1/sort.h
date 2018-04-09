#pragma once

#include "other.h"

/*                           Taks 1                             */

template <class t>
Stats* insertion_sort(std::vector<t> &data, Comparator::Comparator comp)
{
	Stats* stats = new Stats{ data.size(), 0, 0, 0.0 };
	size_t i, j;

	// Starting from the second element because a one element array is always sorted
	for (i = 1; i < data.size(); i++)
	{
		j = i;

		// Looking at the next element in the array (data[j])
		while (j > 0)
		{
			std::cerr << "Compare:\n\tdata[" << (j - 1) << "] (= " << data[j - 1] << ")\n\twith\n\tdata[" << j << "] (= " << data[j] << ")" << std::endl;
			stats->compares++;

			// Putting the next item in the right place of the sorted part of the array ([0, i - 1])
			if (!compare(comp, data[j - 1], data[j], false))
			{
				std::cerr << "Swap:\n\tdata[" << (j - 1) << "] (= " << data[j - 1] << ")\n\twith\n\tdata[" << j << "] (= " << data[j] << ")" << std::endl;
				stats->swaps++;

				swap<t>(data, j, j - 1);
				j--;
			}
			else
			{
				// The element was positioned correctly
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

	// This is a bottom-up implementation of merge sort
	// We start by merging two 1-element arrays into 'secondary'
	// Then we merge two 2-elemet arrays, two 4-element arrays and so on
	for (size_t jump = 1; jump < data.size(); jump *= 2)
	{
		for (size_t x = 0; x < data.size(); x += jump * 2)
		{
			// The two arrays we are about to merge are [min, min + jump - 1] and [min + jump, max]
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
					std::cerr << "Compare:\n\tdata[" << (i - data.begin()) << "] (= " << *i << ")\n\twith\n\tdata[" << (j - data.begin()) << "] (= " << *j << ")" << std::endl;
					stats->compares++;
				}

				if (i != right && (j == end || compare(comp, *i, *j, false)))
				{
					// Taking an element from the left array either
					// if it is smaller than the current element in the right array or if the right array is empty

					secondary[it] = *i;
					i++;
				}
				else
				{
					if (i != right)
					{
						// If we are taking an element from the right array and the left one is not empty yes, we have a swap

						std::cerr << "Swap:\n\tdata[" << (i - data.begin()) << "] (= " << *i << ")\n\twith\n\tdata[" << (j - data.begin()) << "] (= " << *j << ")" << std::endl;
						stats->swaps++;
					}

					// Taking an element from the right array either

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
	// Picking a random pivot
	size_t p_index = (rand() % (range.hi - range.lo)) + range.lo;
	t pivot = data[p_index];

	// Putting the pivot at the beginning of the array
	swap(data, range.lo, p_index);
	stats->swaps++;

	size_t i = range.lo;
	size_t j;

	for (j = range.lo + 1; j <= range.hi; j++)
	{
		std::cerr << "Compare:\n\tdata[" << j << "] (= " << data[j] << ") with the pivot (= " << pivot << ")" << std::endl;
		stats->compares++;

		if (compare(comp, data[j], pivot, true))
		{
			// If the element is less then the pivot, put it on the left size, increasing the "size" of the left side by one
			stats->swaps++;
			swap(data, ++i, j);
			std::cerr << "Swap:\n\tdata[" << j << "] (= " << data[j] << ")\n\twith\n\tdata[" << i << "] (= " << data[i] << ")" << std::endl;
		}
	}

	// Putting the pivot in it's proper place (at the end of the left side)
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

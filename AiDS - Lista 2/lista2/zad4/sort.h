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

template <class t>
bool dual_pivot_partition(std::vector<t> &data, Comparator::Comparator comp, Stats* stats, Range range, size_t &p, size_t &q)
{
	size_t pivot1_index = range.lo;
	size_t pivot2_index = range.hi;

	while (data[pivot1_index] == data[pivot2_index] && pivot1_index < pivot2_index)
	{
		stats->compares++;

		pivot1_index++;
	}

	stats->compares++;

	// All elements are the same
	if (pivot1_index == pivot2_index)
	{
		return false;
	}

	swap(data, pivot1_index, range.lo);

	stats->swaps++;
	stats->compares++;

	if (!compare(comp, data[pivot1_index], data[pivot2_index], true))
	{
		swap(data, range.lo, range.hi);

		stats->swaps++;
	}

	t pivot1 = data[range.lo];
	t pivot2 = data[range.hi];

	size_t i = range.lo + 1, k = range.hi - 1, j = i;
	int d = 0;

	while (j <= k)
	{
		if (d >= 0)
		{
			stats->compares++;

			if (compare(comp, data[j], pivot1, true))
			{
				stats->swaps++;

				swap(data, i, j);
				i++;
				j++;
				d++;
			}
			else
			{
				stats->compares++;

				if (compare(comp, data[j], pivot2, true))
				{
					j++;
				}
				else
				{
					stats->swaps++;

					swap(data, j, k);
					k--;
					d--;
				}
			}
		}
		else
		{
			stats->compares++;

			if (!compare(comp, data[k], pivot2, false))
			{
				k--;
				d--;
			}
			else
			{
				stats->compares++;

				if (compare(comp, data[k], pivot1, true))
				{
					stats->swaps += 3;

					t temp = data[k];
					data[k] = data[j];
					data[j] = data[i];
					data[i] = temp;

					i++;
					d++;
				}
				else
				{
					stats->swaps++;

					swap(data, j, k);
				}

				j++;
			}
		}
	}

	stats->swaps++;
	swap(data, range.lo, i - 1);

	stats->swaps++;
	swap(data, range.hi, k + 1);

	p = i - 1;
	q = k + 1;

	return true;
}

template <class t>
void dual_pivot_quick_sort_inner(std::vector<t> &data, Comparator::Comparator comp, Range range, Stats* stats)
{
	if (range.lo < range.hi)
	{
		size_t p, q;
		if (dual_pivot_partition<t>(data, comp, stats, range, p, q))
		{
			if (p > 0) dual_pivot_quick_sort_inner(data, comp, { range.lo, p - 1 }, stats);
			if (q > 0) dual_pivot_quick_sort_inner(data, comp, { p + 1, q - 1 }, stats);
			dual_pivot_quick_sort_inner(data, comp, { q + 1, range.hi }, stats);
		}
	}
}

template <class t>
Stats* dual_pivot_quick_sort(std::vector<t> &data, Comparator::Comparator comp)
{
	Stats* stats = new Stats{ data.size(), 0, 0, 0.0 };
	dual_pivot_quick_sort_inner(data, comp, { 0, data.size() - 1 }, stats);

	return stats;
}

/*                           Taks 4                             */

template <class t>
Stats* hybrid_sort(std::vector<t> &data, Comparator::Comparator comp)
{
	Stats* stats = new Stats{ data.size(), 0, 0, 0.0 };
	std::vector<t> secondary(data.size());

	size_t jump = 10;

	// For ranges smaller than 10 use insertion sort
	for (size_t x = 0; x < data.size(); x += jump)
	{
		size_t i, j;
		size_t max = std::min(data.size(), x + jump);

		// Starting from the second element because a one element array is always sorted
		for (i = x + 1; i < max; i++)
		{
			j = i;

			// Looking at the next element in the array (data[j])
			while (j > x)
			{
				stats->compares++;

				// Putting the next item in the right place of the sorted part of the array ([0, i - 1])
				if (!compare(comp, data[j - 1], data[j], false))
				{
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
	}

	// For all larger ranges use merge sort
	for (; jump < data.size(); jump *= 2)
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

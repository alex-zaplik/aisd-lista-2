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

/*                           Taks 3                             */

template <class t>
bool dual_pivot_partition(std::vector<t> &data, Comparator::Comparator comp, Stats* stats, Range range, size_t &p, size_t &q)
{
	// Picking two different pivots
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
		// Returning false means the is no more sorting needed for this array
		return false;
	}

	// Putting the first pivot at the beginning
	swap(data, pivot1_index, range.lo);

	stats->swaps++;
	stats->compares++;

	// If the first pivot is less then the second one, switching them
	if (!compare(comp, data[pivot1_index], data[pivot2_index], true))
	{
		swap(data, range.lo, range.hi);

		stats->swaps++;
	}

	// These are out pivot values
	t pivot1 = data[range.lo];
	t pivot2 = data[range.hi];

	// i points at the and of the left side (elements less than pivot1)
	// k at the beginning the the right side (elements greater than pivot2)
	// j is used to iterate
	size_t i = range.lo + 1, k = range.hi - 1, j = i;
	// d is used as a counting value for the count strategy
	int d = 0;

	while (j <= k)
	{
		if (d >= 0)
		{
			// Starting with pivot1
			stats->compares++;

			if (compare(comp, data[j], pivot1, true))
			{
				// data[j] <= pivot1

				// If the current element is less than pivot one, puttin it on the left side,
				// increasing the size of the left side, the iterator and the counting value
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
					// pivot1 < data[j] < pivot2

					// If the current element is less than pivot2 and greater than pivot1
					// Moving on to the next one
					j++;
				}
				else
				{
					// pivot1 < pivot2 <= data[j]

					// If the current element is greater than pivot2 and greater than pivot1,
					// putting it on the right side, increasing the right side and decreasing the counting value
					stats->swaps++;

					swap(data, j, k);
					k--;
					d--;
				}
			}
		}
		else
		{
			// Starting with pivot2
			stats->compares++;

			if (!compare(comp, data[k], pivot2, false))
			{
				// pivot2 < data[k]

				// Skipping any elements at the beginning of the right side that are greater than pivot2
				// and decreasing the counting value
				k--;
				d--;
			}
			else
			{
				stats->compares++;

				if (compare(comp, data[k], pivot1, true))
				{
					// data[k] < pivot1 <= pivot2

					// Putting data[k] on the left side and increasing the size of the left side as well as the counting value
					// Putting data[j] at data[k] for later analysis and data[i] at data[j]
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
					// pivot1 <= data[k] <= pivot2 

					// Putting data[k] at data[j] for later analysis
					stats->swaps++;
					swap(data, j, k);
				}

				j++;
			}
		}
	}

	// Putting both pivots at their proper places in the array
	stats->swaps++;
	swap(data, range.lo, i - 1);

	stats->swaps++;
	swap(data, range.hi, k + 1);

	// Returning the positions of both pivots
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

		// If the elements of data[range.lo, range.hi] are not all the same, sort them recursively
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

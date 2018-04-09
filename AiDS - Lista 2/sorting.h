#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <stack>

#define PRINT_INFO 0

namespace SortType
{
	enum SortType
	{
		NONE, INSERT, MERGE, QUICK, HYBRID, DUAL, RADIX
	};
}

namespace Comparator
{
	enum Comparator
	{
		NONE, INCREASING, DECREASING
	};
}

struct Stats
{
	size_t size;
	long long compares, swaps;
	double time;
};

struct Range
{
	size_t lo, hi;
};

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

			if (PRINT_INFO)
			{
				std::cerr << "Compare:\n\tdata[" << (j - 1) << "] (= " << data[j - 1] << ")\n\twith\n\tdata[" << j << "] (= " << data[j] << ")" << std::endl;
			}

			if (!compare(comp, data[j - 1], data[j], false))
			{
				if (PRINT_INFO)
				{
					std::cerr << "Swap:\n\tdata[" << (j - 1) << "] (= " << data[j - 1] << ")\n\twith\n\tdata[" << j << "] (= " << data[j] << ")" << std::endl;
				}

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
					if (PRINT_INFO)
					{
						std::cerr << "Compare:\n\tdata[" << (i - data.begin()) << "] (= " << *i << ")\n\twith\n\tdata[" << (j - data.begin()) << "] (= " << *j << ")" << std::endl;
					}

					stats->compares++;
				}

				if (i != right && (j == end || compare(comp, *i, *j, false)))
				{
					stats->swaps++;

					secondary[it] = *i;
					i++;
				}
				else
				{
					stats->swaps++;

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
		if (PRINT_INFO)
		{
			std::cerr << "Compare:\n\tdata[" << j << "] (= " << data[j] << ") with the pivot (= " << pivot << ")" << std::endl;
		}

		stats->compares++;

		if (compare(comp, data[j], pivot, true))
		{
			stats->swaps++;

			swap(data, ++i, j);

			if (PRINT_INFO)
			{
				std::cerr << "Swap:\n\tdata[" << j << "] (= " << data[j] << ")\n\twith\n\tdata[" << i << "] (= " << data[i] << ")" << std::endl;
			}
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
void quick_sort_rec_inner(std::vector<t> &data, Comparator::Comparator comp, Range range, Stats* stats)
{
	if (range.lo < range.hi)
	{
		size_t middle = partition<t>(data, comp, stats, range);
		if (middle > 0) quick_sort_rec_inner<t>(data, comp, { range.lo, middle - 1 }, stats);
		quick_sort_rec_inner<t>(data, comp, { middle + 1, range.hi }, stats);
	}
}

template <class t>
Stats* quick_sort_rec(std::vector<t> &data, Comparator::Comparator comp)
{
	Stats* stats = new Stats{ data.size(), 0, 0, 0.0 };
	quick_sort_rec_inner(data, comp, { 0, data.size() - 1 }, stats);

	return stats;
}

template <class t>
bool dual_pivot_partition(std::vector<t> &data, Comparator::Comparator comp, Stats* stats, Range range, size_t &p, size_t &q)
{
	if (PRINT_INFO)
	{
		std::cerr << "Selecting pivots..." << std::endl;
	}

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

	if (PRINT_INFO)
	{
		std::cerr << "Pivots are " << pivot1 << " and " << pivot2 << std::endl;
	}

	size_t i = range.lo + 1, k = range.hi - 1, j = i;
	int d = 0;

	while (j <= k)
	{
		if (d >= 0)
		{
			if (PRINT_INFO)
			{
				std::cerr << "Compare:\n\tdata[" << j << "] (= " << data[j] << ") with the pivot (= " << pivot1 << ")" << std::endl;
			}

			stats->compares++;

			if (compare(comp, data[j], pivot1, true))
			{
				if (PRINT_INFO)
				{
					std::cerr << "Swap:\n\tdata[" << i << "] (= " << data[i] << ")\n\twith\n\tdata[" << j << "] (= " << data[j] << ")" << std::endl;
				}

				stats->swaps++;

				swap(data, i, j);
				i++;
				j++;
				d++;
			}
			else
			{
				if (PRINT_INFO)
				{
					std::cerr << "Compare:\n\tdata[" << j << "] (= " << data[j] << ") with the pivot (= " << pivot2 << ")" << std::endl;
				}

				stats->compares++;

				if (compare(comp, data[j], pivot2, true))
				{
					j++;
				}
				else
				{
					if (PRINT_INFO)
					{
						std::cerr << "Swap:\n\tdata[" << k << "] (= " << data[k] << ")\n\twith\n\tdata[" << j << "] (= " << data[j] << ")" << std::endl;
					}

					stats->swaps++;

					swap(data, j, k);
					k--;
					d--;
				}
			}
		}
		else
		{
			if (PRINT_INFO)
			{
				std::cerr << "Compare:\n\tdata[" << k << "] (= " << data[k] << ") with the pivot (= " << pivot2 << ")" << std::endl;
			}

			stats->compares++;

			if (!compare(comp, data[k], pivot2, false))
			{
				k--;
				d--;
			}
			else
			{
				if (PRINT_INFO)
				{
					std::cerr << "Compare:\n\tdata[" << k << "] (= " << data[k] << ") with the pivot (= " << pivot1 << ")" << std::endl;
				}

				stats->compares++;

				if (compare(comp, data[k], pivot1, true))
				{
					if (PRINT_INFO)
					{
						std::cerr << "Cycling:\n\tdata[" << k << "] (= " << data[k] << ")\n\tdata[" << j << "] (= " << data[j] << ")\n\tdata[" << i << "] (= " << data[i] << ")" << std::endl;
					}

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
					if (PRINT_INFO)
					{
						std::cerr << "Swap:\n\tdata[" << k << "] (= " << data[k] << ")\n\twith\n\tdata[" << j << "] (= " << data[j] << ")" << std::endl;
					}

					stats->swaps++;

					swap(data, j, k);
				}

				j++;
			}
		}
	}

	if (PRINT_INFO)
	{
		std::cerr << "Swap:\n\tdata[" << i - 1 << "] (= " << data[i - 1] << ")\n\twith\n\tdata[" << range.lo << "] (= " << data[range.lo] << ")" << std::endl;
	}

	stats->swaps++;

	swap(data, range.lo, i - 1);

	if (PRINT_INFO)
	{
		std::cerr << "Swap:\n\tdata[" << k + 1 << "] (= " << data[k + 1] << ")\n\twith\n\tdata[" << range.hi << "] (= " << data[range.hi] << ")" << std::endl;
	}

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

template <class t>
Stats* hybrid_sort(std::vector<t> &data, Comparator::Comparator comp)
{
	Stats* stats = new Stats{ data.size(), 0, 0, 0.0 };
	std::vector<t> secondary(data.size());

	size_t jump = 10;

	for (size_t x = 0; x < data.size(); x += jump)
	{
		size_t i, j;
		size_t max = std::min(data.size(), x + jump);

		for (i = x + 1; i < max; i++)
		{
			j = i;

			while (j > x)
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
	}


	for (; jump < data.size(); jump *= 2)
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
					if (PRINT_INFO)
					{
						std::cerr << "Compare:\n\tdata[" << (i - data.begin()) << "] (= " << *i << ")\n\twith\n\tdata[" << (j - data.begin()) << "] (= " << *j << ")" << std::endl;
					}

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
						if (PRINT_INFO)
						{
							std::cerr << "Swap:\n\tdata[" << (i - data.begin()) << "] (= " << *i << ")\n\twith\n\tdata[" << (j - data.begin()) << "] (= " << *j << ")" << std::endl;
						}

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
Stats* hybrid_sort_test(std::vector<t> &data, Comparator::Comparator comp, size_t cut_off)
{
	Stats* stats = new Stats{ data.size(), 0, 0, 0.0 };
	std::vector<t> secondary(data.size());

	size_t jump = cut_off;

	for (size_t x = 0; x < data.size(); x += jump)
	{
		size_t i, j;
		size_t max = std::min(data.size(), x + jump);

		for (i = x + 1; i < max; i++)
		{
			j = i;

			while (j > x)
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
	}


	for (; jump < data.size(); jump *= 2)
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
					if (PRINT_INFO)
					{
						std::cerr << "Compare:\n\tdata[" << (i - data.begin()) << "] (= " << *i << ")\n\twith\n\tdata[" << (j - data.begin()) << "] (= " << *j << ")" << std::endl;
					}

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
						if (PRINT_INFO)
						{
							std::cerr << "Swap:\n\tdata[" << (i - data.begin()) << "] (= " << *i << ")\n\twith\n\tdata[" << (j - data.begin()) << "] (= " << *j << ")" << std::endl;
						}

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

void count_sort(std::vector<int> &data, int exp, int base, Stats* stats)
{
	std::vector<int> secondary(data.size());
	std::vector<int> count(base);

	for (auto it = count.begin(); it != count.end(); it++)
	{
		*it = 0;
	}

	for (auto it = data.begin(); it != data.end(); it++)
	{
		count[(*it / exp) % base]++;
	}

	for (auto it = count.begin() + 1; it != count.end(); it++)
	{
		*it += *(it - 1);
	}

	for (int i = data.size() - 1; i >= 0; i--)
	{
		stats->swaps++;

		secondary[count[(data[i] / exp) % base] - 1] = data[i];
		count[(data[i] / exp) % base]--;
	}

	data.swap(secondary);
}

Stats* radix_sort(std::vector<int> &data, Comparator::Comparator comp)
{
	Stats* stats = new Stats{ data.size(), 0, 0, 0.0 };

	int max = -1;
	for (auto it = data.begin(); it != data.end(); it++)
	{
		stats->compares++;

		if (!compare(comp, *it, max, false)) max = *it;
	}

	for (int exp = 1; max / exp > 0; exp = exp << 4)
	{
		count_sort(data, exp, 16, stats);
	}

	return stats;
}

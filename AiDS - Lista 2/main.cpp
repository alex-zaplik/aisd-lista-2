#include <iostream>
#include <string>
#include <functional>
#include <ctime>
#include <fstream>
#include <sstream>

#include "sorting.h"

#define MAX_VALUE 2100000000

void print_usage() {
	std::cerr << "Usage: --type insert|merge|quick --comp \">=\"|\"<=\"" << std::endl;
}

void parse_params(int argc, char* argv[], SortType::SortType &type, Comparator::Comparator &comp, std::string &file_name, int &k)
{
	for (int i = 1; i < argc; i++)
	{
		if (std::string(argv[i]) == "--type")
		{
			if (++i < argc)
			{
				if (std::string(argv[i]) == "insert") type = SortType::INSERT;
				else if (std::string(argv[i]) == "merge") type = SortType::MERGE;
				else if (std::string(argv[i]) == "quick") type = SortType::QUICK;
				else if (std::string(argv[i]) == "hybrid") type = SortType::HYBRID;
				else if (std::string(argv[i]) == "dual") type = SortType::DUAL;
				else if (std::string(argv[i]) == "radix") type = SortType::RADIX;
			}
			else
			{
				print_usage();
			}
		}
		else if (std::string(argv[i]) == "--comp")
		{
			if (++i < argc)
			{
				if (std::string(argv[i]) == "<=") comp = Comparator::INCREASING;
				else if (std::string(argv[i]) == ">=") comp = Comparator::DECREASING;
			}
		}
		else if (std::string(argv[i]) == "--stat")
		{
			i += 2;
			if (i < argc)
			{
				file_name = argv[i - 1];
				k = std::stoi(argv[i]);
			}
		}
	}
}

std::string time_to_str(double d, int precision)
{
	std::stringstream ss;

	ss << (int)d;
	d -= (int)d;
	
	for (int i = 0; i < precision; i++) d *= 10;

	ss << "," << (int)d;
	d -= (int)d;

	return ss.str();
}

void generate_csv(std::vector<Stats*> &res, std::string file_name)
{
	std::ofstream file;
	file.open(file_name);

	file << "\"size\";\"compares\";\"swaps\";\"time\"\n";

	for (size_t i = 0; i < res.size(); i++)
	{
		file << res[i]->size << ";" << res[i]->compares << ";" << res[i]->swaps << ";" << time_to_str(res[i]->time, 2) << "\n";
	}

	file.close();
}

void generate_data(std::vector<int> &data, int count)
{
	for (int i = 0; i < count; i++)
	{
		data.push_back(rand() % MAX_VALUE);
	}
}

void generate_data(std::vector<std::string> &data, int count)
{
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int j = 0; j < count; j++)
	{
		std::stringstream ss;
		for (int i = 0; i < 50; ++i) {
			ss << alphanum[rand() % (sizeof(alphanum) - 1)];
		}
		data.push_back(ss.str());
	}
}

template <class t>
void get_data(std::vector<t> &data)
{
	int n;
	t num;

	std::cin >> n;

	for (int i = 0; i < n; i++)
	{
		std::cin >> num;
		data.push_back(num);
	}
}

void multiple_test(SortType::SortType &type, Comparator::Comparator &comp, std::string file_name, int &k)
{
	std::clock_t start, end;
	std::vector<Stats*> results;
	std::vector<int> data;
	std::function<Stats* (std::vector<int> &, Comparator::Comparator)> sort_algo = nullptr;

	switch (type)
	{
	case SortType::INSERT:
		sort_algo = insertion_sort<int>;
		break;
	case SortType::MERGE:
		sort_algo = merge_sort<int>;
		break;
	case SortType::QUICK:
		sort_algo = quick_sort_rec<int>;
		break;
	case SortType::HYBRID:
		sort_algo = hybrid_sort<int>;
		break;
	case SortType::DUAL:
		sort_algo = dual_pivot_quick_sort<int>;
		break;
	case SortType::RADIX:
		sort_algo = radix_sort;
		break;
	}

	size_t size = 10;
	for (int i = 0; i < 9; i++)
	{
		std::cerr << "Current size = " << size << std::endl;

		for (int i = 0; i < k; i++)
		{
			generate_data(data, size);

			start = std::clock();
			Stats* s = sort_algo(data, comp);
			end = std::clock();

			data.clear();

			s->time = (end - start) / (double)(CLOCKS_PER_SEC / 1000.0);
			results.push_back(s);
		}

		if (i % 2 == 0)
		{
			size *= 5;
		}
		else
		{
			size *= 2;
		}
	}

	std::vector<Stats*> avg_res(results.size() / k);
	for (int i = 1; i <= results.size() / k; i++)
	{
		Stats* s = new Stats;
		s->size = results[(i - 1) * k]->size;
		s->compares = 0;
		s->swaps = 0;
		s->time = 0;

		for (int j = 0; j < k; j++)
		{
			s->compares += results[(i - 1) * k + j]->compares;
			s->swaps += results[(i - 1) * k + j]->swaps;
			s->time += results[(i - 1) * k + j]->time;
		}

		s->compares /= k;
		s->swaps /= k;
		s->time /= k;

		avg_res[i - 1] = s;
	}

	generate_csv(avg_res, file_name);
}

template <class t>
void single_test(SortType::SortType &type, Comparator::Comparator &comp)
{
	std::clock_t start, end;
	Stats* stats = nullptr;
	std::vector<t> data;

	// get_data<t>(data);
	generate_data(data, 15);

	if (type != SortType::NONE && comp != Comparator::NONE)
	{
		start = std::clock();

		switch (type)
		{
		case SortType::INSERT:
			stats = insertion_sort<t>(data, comp);
			break;
		case SortType::MERGE:
			stats = merge_sort<t>(data, comp);
			break;
		case SortType::QUICK:
			stats = quick_sort<t>(data, comp);
			break;
		case SortType::HYBRID:
			stats = hybrid_sort<t>(data, comp);
			break;
		case SortType::DUAL:
			stats = dual_pivot_quick_sort<t>(data, comp);
			break;
		case SortType::RADIX:
			stats = radix_sort(data, comp);
			break;
		}

		end = std::clock();

		if (!vector_sorted<t>(data, comp))
		{
			std::cerr << "Sorting failed!" << std::endl;
		}
	}
	else
	{
		print_usage();
	}

	if (stats)
	{
		std::cerr << "Compares = " << stats->compares << "; Swaps = " << stats->swaps << "; Time = " << (end - start) / (double)(CLOCKS_PER_SEC / 1000.0) << "ms" << std::endl;
		std::cout << "Size = " << stats->size << std::endl;

		for (auto it = data.begin(); it != data.end(); it++)
		{
			std::cout << *it << " ";
		}
		std::cout << std::endl;
	}
}

void run_test(SortType::SortType &type, Comparator::Comparator &comp, std::string file_name, int &k)
{
	std::srand((unsigned int)time(NULL));

	if (k > 0)
	{
		multiple_test(type, comp, file_name, k);
	}
	else
	{
		single_test<int>(type, comp);
	}
}

template <class t>
void test_cut_off(std::string file_name, int &k)
{
	std::clock_t start, end;
	std::vector<Stats*> results;
	std::vector<t> data;
	auto sort_algo = hybrid_sort_test<t>;
	size_t size = 10000;

	size_t min_cut = 1;
	size_t max_cut = 25;

	for (size_t cut = min_cut; cut <= max_cut; cut++)
	{
		std::cerr << "Current cut = " << cut << std::endl;

		for (int i = 0; i < k; i++)
		{
			generate_data(data, size);

			start = std::clock();
			Stats* s = sort_algo(data, Comparator::Comparator::INCREASING, cut);
			end = std::clock();

			data.clear();

			s->time = (end - start) / (double)(CLOCKS_PER_SEC / 1000.0);
			results.push_back(s);
		}
	}

	std::vector<Stats*> avg_res(max_cut - min_cut);
	for (int i = min_cut; i <= max_cut; i++)
	{
		Stats* s = new Stats;
		s->size = i;
		s->compares = 0;
		s->swaps = 0;
		s->time = 0;

		for (int j = 0; j < k; j++)
		{
			s->compares += results[(i - 1) * k + j]->compares;
			s->swaps += results[(i - 1) * k + j]->swaps;
			s->time += results[(i - 1) * k + j]->time;
		}

		s->compares /= k;
		s->swaps /= k;
		s->time /= k;

		avg_res[i - 1] = s;
	}

	generate_csv(avg_res, file_name);

	for (int i = 0; i < results.size(); i++)
	{
		delete results[i];
	}

	for (int i = 0; i < avg_res.size(); i++)
	{
		delete avg_res[i];
	}
}

int main(int argc, char* argv[])
{
	if (argc < 5)
	{
		print_usage();
	}

	SortType::SortType type = SortType::NONE;
	Comparator::Comparator comp = Comparator::NONE;
	std::string file_name = "";
	int k = -1;

	// parse_params(argc, argv, type, comp, file_name, k);
	// run_test(type, comp, file_name, k);

	comp = Comparator::Comparator::INCREASING;
	k = 500;

	type = SortType::MERGE;
	run_test(type, comp, "merge_500_2100000000.csv", k);
	type = SortType::QUICK;
	run_test(type, comp, "quick_500_2100000000.csv", k);
	type = SortType::DUAL;
	run_test(type, comp, "dual_500_2100000000.csv", k);
	type = SortType::HYBRID;
	run_test(type, comp, "hybrid_500_2100000000.csv", k);
	type = SortType::RADIX;
	run_test(type, comp, "radix_500_2100000000.csv", k);

	// k = 500;
	// test_cut_off<std::string>("cut_off_string.csv", k);
	// test_cut_off<int>("cut_off_int.csv", k);
}
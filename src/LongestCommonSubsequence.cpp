#include <DiffMerge3/LongestCommonSubsequence.hpp>

#include <algorithm>
#include <cassert>
#include <iostream>

namespace DiffMerge3
{
	LongestCommonSubsequence::LongestCommonSubsequence(const char *a, const char *b)
		: orig_a(a)
		, orig_b(b)
	{}

	LongestCommonSubsequence::LongestCommonSubsequence(const char *a, size_t a_len, const char *b, size_t b_len)
		: orig_a(a, a_len)
		, orig_b(b, b_len)
	{}

	LongestCommonSubsequence::LongestCommonSubsequence(const std::string &a, const std::string &b)
		: orig_a(a)
		, orig_b(b)
	{}

	LongestCommonSubsequence::LongestCommonSubsequence(const std::string_view &a, const std::string_view &b)
		: orig_a(a)
		, orig_b(b)
	{}

	size_t LongestCommonSubsequence::Evaluate()
	{
		constexpr const bool take_default_up_arrow = false; // Should take Up arrow if left and upper values equal?

		entries.reserve(std::min(orig_a.size(), orig_b.size()));

		struct CellData
		{
			size_t value;

			enum class Arrow
			{
				LEFT = 0,
				UP,
				DIAGONAL,
			} arrow;
		};

		size_t table_height = orig_b.size() + 1;
		size_t table_width = orig_a.size() + 1;

		std::vector<std::vector<CellData>> table(table_height);
		for (size_t i = 0; i < table_height; i++)
			table[i].resize(table_width);

		for (size_t i = 0; i < orig_b.size(); i++)
		{
			size_t table_i = i + 1;

			for (size_t j = 0; j < orig_a.size(); j++)
			{
				size_t table_j = j + 1;

				if (orig_a[j] == orig_b[i])
				{
					table[table_i][table_j].arrow = CellData::Arrow::DIAGONAL;
					table[table_i][table_j].value = table[table_i - 1][table_j - 1].value + 1;
				}
				else
				{
					if (table[table_i - 1][table_j].value == table[table_i][table_j - 1].value)
					{
						if (take_default_up_arrow)
						{
							table[table_i][table_j].arrow = CellData::Arrow::UP;
							table[table_i][table_j].value = table[table_i - 1][table_j].value;
						}
						else
						{
							table[table_i][table_j].arrow = CellData::Arrow::LEFT;
							table[table_i][table_j].value = table[table_i][table_j - 1].value;
						}
					}
					else if (table[table_i - 1][table_j].value > table[table_i][table_j - 1].value)
					{
						table[table_i][table_j].arrow = CellData::Arrow::UP;
						table[table_i][table_j].value = table[table_i - 1][table_j].value;
					}
					else
					{
						table[table_i][table_j].arrow = CellData::Arrow::LEFT;
						table[table_i][table_j].value = table[table_i][table_j - 1].value;
					}
				}
			}
		}

		// Backtrack
		size_t height_index = table_height - 1;
		size_t width_index = table_width - 1;
		const CellData *current_cell = &table[height_index][width_index];
		while(current_cell != nullptr)
		{
			bool contributes = false;
			switch (current_cell->arrow)
			{
				case CellData::Arrow::LEFT:
				{
					width_index--;
					break;
				}
				case CellData::Arrow::UP:
				{
					height_index--;
					break;
				}
				case CellData::Arrow::DIAGONAL:
				{
					height_index--;
					width_index--;
					contributes = true;
					break;
				}
			}

			if (contributes)
			{
				LongestCommonSubsequenceEntry entry{};
				entry.index_a = width_index;
				entry.index_b = height_index;
				entry.character = orig_a[entry.index_a];
				entries.push_back(entry);
			}

			if (width_index == 0 || height_index == 0)
				break;

			current_cell = &table[height_index][width_index];
		}

		std::reverse(entries.begin(), entries.end());

		return entries.size();
	}
}
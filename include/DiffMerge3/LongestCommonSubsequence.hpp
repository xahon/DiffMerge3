#pragma once

#include <DiffMerge3/Span.hpp>
#include <DiffMerge3/DiffMergeExport.h>

#include <vector>
#include <string>
#include <string_view>

namespace DiffMerge3
{
	struct LongestCommonSubsequenceEntry
	{
		size_t index_a;
		size_t index_b;
		char character;
	};

	class LongestCommonSubsequence
	{
	public:
		LongestCommonSubsequence(const char *a, const char *b);
		LongestCommonSubsequence(const char *a, size_t a_len, const char *b, size_t b_len);
		LongestCommonSubsequence(const std::string &a, const std::string &b);
		LongestCommonSubsequence(const std::string_view &a, const std::string_view &b);

		size_t Evaluate();

		size_t GetNumEntries() const { return entries.size(); }
		const LongestCommonSubsequenceEntry *GetEntry(size_t index) const { return index < entries.size() ? &entries[index] : nullptr; }

	private:
		std::string orig_a;
		std::string orig_b;
		std::vector<LongestCommonSubsequenceEntry> entries;
	};
}
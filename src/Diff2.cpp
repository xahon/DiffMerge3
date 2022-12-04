#include <DiffMerge3/Diff2.hpp>
#include <DiffMerge3/LongestCommonSubsequence.hpp>

#include <cassert>

namespace DiffMerge3
{
	Diff2::Diff2(const char *a, const char *b)
		: orig_a(a)
		, orig_b(b)
	{}

	Diff2::Diff2(const char *a, size_t a_len, const char *b, size_t b_len)
		: orig_a(a, a_len)
		, orig_b(b, b_len)
	{}

	Diff2::Diff2(const std::string &a, const std::string &b)
		: orig_a(a)
		, orig_b(b)
	{}

	Diff2::Diff2(const std::string_view &a, const std::string_view &b)
		: orig_a(a)
		, orig_b(b)
	{}

	size_t Diff2::Evaluate()
	{
		LongestCommonSubsequence lcs(orig_a, orig_b);
		lcs.Evaluate(/*false*/);

		auto makeUnstableBetween = [this](const LongestCommonSubsequenceEntry *first, const LongestCommonSubsequenceEntry *second) -> DiffRegion
		{
			size_t start_a = first == nullptr ? 0 : first->index_a + 1;
			size_t start_b = first == nullptr ? 0 : first->index_b + 1;
			size_t end_a = second == nullptr ? orig_a.size() : second->index_a;
			size_t end_b = second == nullptr ? orig_b.size() : second->index_b;

			DiffRegion unstable_region(false);
			DiffRegion::Unstable *unstable = unstable_region.AsUnstable();
			unstable->span_a.start = start_a;
			unstable->span_a.end = end_a;
			unstable->span_b.start = start_b;
			unstable->span_b.end = end_b;
			unstable->change_a = unstable->span_a.GetView(orig_a);
			unstable->change_b = unstable->span_b.GetView(orig_b);
			unstable->change_type_a = DiffChangeType::ADDED;
			unstable->change_type_b = DiffChangeType::REMOVED;

			return unstable_region;
		};

		const LongestCommonSubsequenceEntry *prev_lcs_entry = nullptr;

		for (size_t i = 0; i < lcs.GetNumEntries(); i++)
		{
			const LongestCommonSubsequenceEntry *current_lcs_entry = lcs.GetEntry(i);

			DiffRegion unstable_between = makeUnstableBetween(prev_lcs_entry, current_lcs_entry);
			if (!unstable_between.IsEmpty())
				regions.push_back(std::move(unstable_between));

			DiffRegion stable_region(true);
			DiffRegion::Stable *stable = stable_region.AsStable();
			stable->span_a = Span(current_lcs_entry->index_a, current_lcs_entry->index_a + 1);
			stable->span_b = Span(current_lcs_entry->index_b, current_lcs_entry->index_b + 1);
			stable->view = stable->span_a.GetView(orig_a);
			regions.push_back(std::move(stable_region));

			prev_lcs_entry = current_lcs_entry;
		}

		DiffRegion unstable_between = makeUnstableBetween(prev_lcs_entry, nullptr);
		if (!unstable_between.IsEmpty())
			regions.push_back(std::move(unstable_between));

		return regions.size();
	}

	size_t Diff2::PrintEntry(FILE *f, const DiffRegion *diff_region)
	{
		size_t bytes_written = 0;

		if (const DiffRegion::Stable *region_stable = diff_region->AsStable())
		{
			bytes_written += fprintf(f, "%.*s", static_cast<int>(region_stable->view.size()), region_stable->view.data());
		}
		else if (const DiffRegion::Unstable *region_unstable = diff_region->AsUnstable())
		{
			static constexpr const std::string_view left_marker    = "\n<<<<<<<\n";
			static constexpr const std::string_view right_marker   = "\n>>>>>>>\n";
			static constexpr const std::string_view middle_marker  = "\n|||||||\n";

			bytes_written += fprintf(f, "%s", left_marker.data());
			bytes_written += fprintf(f, "%.*s", static_cast<int>(region_unstable->change_a.size()), region_unstable->change_a.data());
			bytes_written += fprintf(f, "%s", middle_marker.data());
			bytes_written += fprintf(f, "%.*s", static_cast<int>(region_unstable->change_b.size()), region_unstable->change_b.data());
			bytes_written += fprintf(f, "%s", right_marker.data());
		}

		return bytes_written;
	}
}
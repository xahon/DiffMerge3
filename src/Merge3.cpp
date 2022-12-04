#include <DiffMerge3/Merge3.hpp>
#include <DiffMerge3/LongestCommonSubsequence.hpp>

#include <cassert>
#include <algorithm>

namespace DiffMerge3
{
	// diff3 algorithm
	// https://www.cis.upenn.edu/~bcpierce/papers/diff3-short.pdf

	Merge3::Merge3(const char *base, const char *left, const char *right)
		: orig_base(base)
		, orig_left(left)
		, orig_right(right)
	{}

	Merge3::Merge3(const char *base, size_t base_len, const char *left, size_t left_len, const char *right, size_t right_len)
		: orig_base(base, base_len)
		, orig_left(left, left_len)
		, orig_right(right, right_len)
	{}

	Merge3::Merge3(const std::string &base, const std::string &left, const std::string &right)
		: orig_base(base)
		, orig_left(left)
		, orig_right(right)
	{}

	Merge3::Merge3(const std::string_view &base, const std::string_view &left, const std::string_view &right)
		: orig_base(base)
		, orig_left(left)
		, orig_right(right)
	{}

	bool Merge3::Merge(bool include_false_conflicts)
	{
		struct Chunk
		{
			bool stable {false};
			bool remove {false};

			Span span_base {};
			Span span_left {};
			Span span_right {};

			std::string_view base_view;
			std::string_view left_view;
			std::string_view right_view;
		};

		LongestCommonSubsequence lcs_base_left(orig_base, orig_left);
		LongestCommonSubsequence lcs_base_right(orig_base, orig_right);

		lcs_base_left.Evaluate();
		lcs_base_right.Evaluate();

		std::vector<Chunk> chunks;
		bool have_conflict_chunks = false;

		size_t prev_left_index = 0;
		size_t prev_right_index = 0;
		size_t last_left_lcs_index = 0;
		size_t last_right_lcs_index = 0;

		for (size_t i = 0; i < orig_base.size(); i++)
		{
			Chunk chunk {};

			chunk.span_base = Span(i, i + 1);
			chunk.span_left.start = prev_left_index;
			chunk.span_right.start = prev_right_index;

			for (size_t j = last_left_lcs_index; j < lcs_base_left.GetNumEntries(); j++)
			{
				const LongestCommonSubsequenceEntry *entry = lcs_base_left.GetEntry(j);

				if (entry->index_a == i)
				{
					last_left_lcs_index = j;
					chunk.span_left.end = entry->index_b + 1;

					if (chunk.span_left.size() > 1)
					{
						Chunk intermediate_chunk {};
						intermediate_chunk.stable = false;
						intermediate_chunk.span_left = Span(prev_left_index, chunk.span_left.end - 1);
						intermediate_chunk.left_view = intermediate_chunk.span_left.GetView(orig_left);
						chunks.push_back(intermediate_chunk);
						have_conflict_chunks = true;

						chunk.span_left.start = entry->index_b;
					}

					prev_left_index = chunk.span_left.end;
					break;
				}
			}

			for (size_t j = last_right_lcs_index; j < lcs_base_right.GetNumEntries(); j++)
			{
				const LongestCommonSubsequenceEntry *entry = lcs_base_right.GetEntry(j);

				if (entry->index_a == i)
				{
					last_right_lcs_index = j;
					chunk.span_right.end = entry->index_b + 1;

					if (chunk.span_right.size() > 1)
					{
						Chunk intermediate_chunk {};
						intermediate_chunk.stable = false;
						intermediate_chunk.span_right = Span(prev_right_index, chunk.span_right.end - 1);
						intermediate_chunk.right_view = intermediate_chunk.span_right.GetView(orig_right);
						chunks.push_back(intermediate_chunk);
						have_conflict_chunks = true;

						chunk.span_right.start = entry->index_b;
					}

					prev_right_index = chunk.span_right.end;
					break;
				}
			}

			chunk.base_view = chunk.span_base.GetView(orig_base);
			chunk.left_view = chunk.span_left.GetView(orig_left);
			chunk.right_view = chunk.span_right.GetView(orig_right);
			chunk.stable = (chunk.base_view == chunk.left_view) && (chunk.base_view == chunk.right_view);

			chunks.push_back(chunk);
		}

		// Add trailing chunks for both left and right (Leading are processed in the above loop)
		{
			const LongestCommonSubsequenceEntry *last_lcs_entry = lcs_base_left.GetEntry(lcs_base_left.GetNumEntries() - 1);
			Chunk intermediate_chunk {};
			intermediate_chunk.stable = false;
			intermediate_chunk.span_left = Span(last_lcs_entry->index_b + 1, orig_left.size());
			intermediate_chunk.left_view = intermediate_chunk.span_left.GetView(orig_left);

			if (intermediate_chunk.span_left.size() > 0)
			{
				chunks.push_back(intermediate_chunk);
				have_conflict_chunks = true;
			}
		}

		{
			const LongestCommonSubsequenceEntry *last_lcs_entry = lcs_base_right.GetEntry(lcs_base_right.GetNumEntries() - 1);
			Chunk intermediate_chunk {};
			intermediate_chunk.stable = false;
			intermediate_chunk.span_right = Span(last_lcs_entry->index_b + 1, orig_right.size());
			intermediate_chunk.right_view = intermediate_chunk.span_right.GetView(orig_right);

			if (intermediate_chunk.span_right.size() > 0)
			{
				chunks.push_back(intermediate_chunk);
				have_conflict_chunks = true;
			}
		}

		if (!chunks.empty())
		{
			Chunk *group_begin = &chunks[0];
			Chunk *current_head;
			bool left_started = false;
			bool right_started = false;
			bool base_started = false;

			for (size_t i = 1; i < chunks.size(); i++)
			{
				current_head = &chunks[i];

				if (current_head->stable == group_begin->stable)
				{
					if (!left_started && current_head->span_left.size() > 0)
					{
						group_begin->span_left.start = current_head->span_left.start;
						left_started = true;
					}

					if (!right_started && current_head->span_right.size() > 0)
					{
						group_begin->span_right.start = current_head->span_right.start;
						right_started = true;
					}

					if (!base_started && current_head->span_base.size() > 0)
					{
						group_begin->span_base.start = current_head->span_base.start;
						base_started = true;
					}

					group_begin->span_left.Union(current_head->span_left);
					group_begin->left_view = group_begin->span_left.GetView(orig_left);

					group_begin->span_right.Union(current_head->span_right);
					group_begin->right_view = group_begin->span_right.GetView(orig_right);

					group_begin->span_base.Union(current_head->span_base);
					group_begin->base_view = group_begin->span_base.GetView(orig_base);

					current_head->remove = true;
				}
				else
				{
					group_begin = current_head;

					left_started = group_begin->span_left.size() > 0;
					right_started = group_begin->span_right.size() > 0;
					base_started = group_begin->span_base.size() > 0;
				}
			}
		}

		chunks.erase(std::remove_if(chunks.begin(), chunks.end(), [](const Chunk &c) { return c.remove; }), chunks.end());

		for (const Chunk &chunk : chunks)
		{
			MergeRegion region {};
			region.is_ok = chunk.stable;

			if (chunk.stable)
			{
				region.data.stable.span_base = chunk.span_base;
				region.data.stable.span_left = chunk.span_left;
				region.data.stable.span_right = chunk.span_right;
				region.data.stable.view = chunk.base_view;
			}
			else
			{
				region.data.unstable.span_base = chunk.span_base;
				region.data.unstable.span_left = chunk.span_left;
				region.data.unstable.span_right = chunk.span_right;
				region.data.unstable.base_view = chunk.base_view;
				region.data.unstable.left_view = chunk.left_view;
				region.data.unstable.right_view = chunk.right_view;
				region.data.unstable.is_false_conflict = chunk.left_view == chunk.right_view;
				region.data.unstable.is_auto_solvable = false;

				if (region.data.unstable.is_false_conflict)
				{
					region.data.unstable.false_conflict_view = chunk.left_view;
				}

				if (chunk.base_view == chunk.right_view && chunk.base_view != chunk.left_view)
				{
					region.data.unstable.is_auto_solvable = true;
					region.data.unstable.auto_solve_view = chunk.left_view;
				}
				else if (chunk.base_view != chunk.right_view && chunk.base_view == chunk.left_view)
				{
					region.data.unstable.is_auto_solvable = true;
					region.data.unstable.auto_solve_view = chunk.right_view;
				}
			}

			regions.push_back(region);
		}

		return have_conflict_chunks;
	}

	size_t Merge3::PrintEntry(FILE *f, const MergeRegion *merge_section)
	{
		size_t bytes_written = 0;

		if (const MergeRegion::Stable *ok_section = merge_section->AsOk())
		{
			bytes_written += fwrite(ok_section->view.data(), 1, ok_section->view.size(), f);
		}
		else if (const MergeRegion::Unstable *conflict_section = merge_section->AsConflict())
		{
			if (conflict_section->is_false_conflict)
			{
				bytes_written += fwrite(conflict_section->false_conflict_view.data(), 1, conflict_section->false_conflict_view.size(), f);
			}
			else if (conflict_section->is_auto_solvable)
			{
				bytes_written += fwrite(conflict_section->auto_solve_view.data(), 1, conflict_section->auto_solve_view.size(), f);
			}
			else
			{
				static constexpr const std::string_view left_marker      = "\n<<<<<<<\n";
				static constexpr const std::string_view right_marker     = "\n>>>>>>>\n";
				static constexpr const std::string_view base_marker_top  = "\n|||||||\n";
				static constexpr const std::string_view base_marker_bot  = "\n=======\n";

				bytes_written += fwrite(left_marker.data(), 1, left_marker.size(), f);
				bytes_written += fwrite(conflict_section->left_view.data(), 1, conflict_section->left_view.size(), f);
				bytes_written += fwrite(base_marker_top.data(), 1, base_marker_top.size(), f);
				bytes_written += fwrite(conflict_section->base_view.data(), 1, conflict_section->base_view.size(), f);
				bytes_written += fwrite(base_marker_bot.data(), 1, base_marker_bot.size(), f);
				bytes_written += fwrite(conflict_section->right_view.data(), 1, conflict_section->right_view.size(), f);
				bytes_written += fwrite(right_marker.data(), 1, right_marker.size(), f);
			}
		}

		return bytes_written;
	}
}
#pragma once

#include <DiffMerge3/Span.hpp>
#include <DiffMerge3/DiffMergeExport.h>

#include <cstdint>
#include <cstdio>
#include <string>
#include <string_view>
#include <vector>

namespace DiffMerge3
{
	enum class MergeSource
	{
		LEFT,
		RIGHT,
		BOTH,
		CONFLICT,
	};

	struct MergeRegion
	{
	public:
		struct Stable
		{
			Span span_base;
			Span span_left;
			Span span_right;
			std::string_view view;
			MergeSource merge_source;
		};

		struct Unstable
		{
			Span span_base;
			Span span_left;
			Span span_right;
			std::string_view base_view;
			std::string_view left_view;
			std::string_view right_view;
			MergeSource merge_source;

			bool is_false_conflict {};
			std::string_view false_conflict_view;

			bool is_auto_solvable {};
			std::string_view auto_solve_view;
		};

		union Data
		{
			Stable stable;
			Unstable unstable;
		} data;
		bool is_ok {};

	public:
		const MergeRegion::Stable *AsOk() const { return is_ok ? &data.stable : nullptr; }
		MergeRegion::Stable *AsOk() { return is_ok ? &data.stable : nullptr; }
		const MergeRegion::Unstable *AsConflict() const { return !is_ok ? &data.unstable : nullptr; }
		MergeRegion::Unstable *AsConflict() { return !is_ok ? &data.unstable : nullptr; }

		bool IsOk() const { return is_ok; }
		bool IsFalseConflict() const { return is_ok || data.unstable.is_false_conflict; }
	};

	class Merge3
	{
	public:
		Merge3(const char *base, const char *left, const char *right);
		Merge3(const char *base, size_t base_len, const char *left, size_t left_len, const char *right, size_t right_len);
		Merge3(const std::string &base, const std::string &left, const std::string &right);
		Merge3(const std::string_view &base, const std::string_view &left, const std::string_view &right);

		bool Merge(bool include_false_conflicts = false);

		inline size_t GetNumRegions() const { return regions.size(); }
		inline const MergeRegion *GetRegion(size_t index) const { return index < regions.size() ? &regions[index] : nullptr; }

		static size_t PrintEntry(FILE *f, const MergeRegion *merge_section);

	private:
		std::string orig_base;
		std::string orig_left;
		std::string orig_right;
		std::vector<MergeRegion> regions;
	};
}
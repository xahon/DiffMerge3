#pragma once

#include <DiffMerge3/Span.hpp>
#include <DiffMerge3/DiffMergeExport.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace DiffMerge3
{
	enum class DiffChangeType
	{
		ADDED = 0,
		REMOVED,
	};

	struct DiffRegion
	{
	public:
		struct Unstable
		{
			Span span_a {};
			Span span_b {};
			std::string_view change_a;
			DiffChangeType change_type_a {DiffChangeType::ADDED};
			std::string_view change_b;
			DiffChangeType change_type_b {DiffChangeType::ADDED};
		};

		struct Stable
		{
			Span span_a {};
			Span span_b {};
			std::string_view view;
		};

		union Data
		{
			Stable stable;
			Unstable unstable;
		} data {};
		bool is_stable;

	public:
		explicit DiffRegion(bool is_stable)
			: is_stable(is_stable)
		{}

		bool IsEmpty() const
		{
			if (is_stable)
				return AsStable()->span_a.size() == 0 && AsStable()->span_b.size() == 0;
			return AsUnstable()->span_a.size() == 0 && AsUnstable()->span_b.size() == 0;
		}

		const DiffRegion::Stable *AsStable() const { return is_stable ? &data.stable : nullptr; }
		DiffRegion::Stable *AsStable() { return is_stable ? &data.stable : nullptr; }
		const DiffRegion::Unstable *AsUnstable() const { return !is_stable ? &data.unstable : nullptr; }
		DiffRegion::Unstable *AsUnstable() { return !is_stable ? &data.unstable : nullptr; }
	};

	class Diff2
	{
	public:
		Diff2(const char *a, const char *b);
		Diff2(const char *a, size_t a_len, const char *b, size_t b_len);
		Diff2(const std::string &a, const std::string &b);
		Diff2(const std::string_view &a, const std::string_view &b);

		size_t Evaluate();

		size_t GetNumRegions() const { return regions.size(); }
		const DiffRegion *GetRegion(size_t index) const { return index < regions.size() ? &regions[index] : nullptr; }

		static size_t PrintEntry(FILE *f, const DiffRegion *diff_region);

	private:
		std::string orig_a;
		std::string orig_b;
		std::vector<DiffRegion> regions;
	};
}
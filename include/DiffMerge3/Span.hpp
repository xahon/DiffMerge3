#pragma once

#include <DiffMerge3/DiffMergeExport.h>

#include <cstdint>
#include <string>
#include <string_view>

namespace DiffMerge3
{
	struct Span
	{
		Span() : start(0), end(0) {}
		Span(size_t start, size_t end) : start(start), end(end) {}

		size_t size() const { return end < start ? 0 : end - start; }
		std::string_view GetView(const std::string &string) const { if (size() == 0) return {}; return {string.data() + start, size()}; }

		Span &Union(const Span &other) { if (other.end > end) end = other.end; return *this; }

		size_t start;
		size_t end; // one-past-last
	};
}
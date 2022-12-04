#include "DiffMerge3/DiffMerge3.hpp"

#include <cassert>

int main()
{
	const char *base  = "int Foo(int a) { return 0; }\n void Bar(){}";
	const char *left  = "float Foo(int b) { return b; }\n float Baz(){ return 3.14f; }";
	const char *right = "float Foo(float a) { return 0; }\n bool Xyz(){ return false; }";

	DiffMerge3::Merge3 merger(base, left, right);
	merger.Merge();

	for (size_t i = 0; i < merger.GetNumRegions(); i++)
	{
		const DiffMerge3::MergeRegion *section = merger.GetRegion(i);
		DiffMerge3::Merge3::PrintEntry(stdout, section);
	}

	fflush(stdout);

	return 0;
}
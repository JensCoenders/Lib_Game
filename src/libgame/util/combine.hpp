#include <string>

/* Combiner functions */

template <typename First>
std::string combineStringPath(First& firstString = "")
{
	return firstString;
}

template <typename First, typename ... Rest>
std::string combineStringPath(First firstString, Rest&... rest)
{
	return std::string(firstString) + "\\" + combineStringPath(rest...);
}

template <typename First>
int combineModuleTypes(First firstModule = 0)
{
	return firstModule;
}

template <typename First, typename ... Rest>
int combineModuleTypes(First firstModule, Rest ... restModules)
{
	return (int) firstModule | combineModuleTypes(restModules...);
}

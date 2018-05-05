#pragma once
#include "Common.h"

ref struct Language
{
	System::String^ name;
	List_t< System::String^ > decorations, skills, abilities, tags, components, string_table;
	List_t< List_t< System::String^ >^ > armors;

	Language();
};

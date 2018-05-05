#include "stdafx.h"
#include "Language.h"
#include "Armor.h"

Language::Language()
{
	for( unsigned i = 0; i < int( Armor::ArmorType::NumArmorTypes ); ++i )
		armors.Add( gcnew List_t< System::String^ > );
}

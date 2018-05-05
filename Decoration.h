#pragma once
#include "Common.h"
#include "Skill.h"

ref struct Query;

ref struct Decoration
{
	System::String^ name;
	unsigned hr, slot_level, rarity, difficulty, index, num_owned;
	bool is_event, can_use;

	List_t< AbilityPair^ > abilities;
	List_t< MaterialComponent^ > components;

	bool IsBetterThan( Decoration^ other, List_t< Ability^ >^ rel_abilities );
	int GetSkillAt( Ability^ ability );

	bool MatchesQuery( Query^ query );
	
	static void Load( System::String^ filename );
	static void LoadLanguage( System::String^ filename );
	static void LoadCustom( System::String^ filename );
	static void LoadCustom();
	static void SaveCustom();

	static List_t< Decoration^ > static_decorations;
	static Map_t< Ability^, List_t< Decoration^ >^ > static_decoration_ability_map;
	static Map_t< System::String^, Decoration^ > static_decoration_map;
	static Decoration^ FindDecoration( System::String^ name );
	static Decoration^ FindDecorationFromString( System::String^ line );
	static Decoration^ GetBestDecoration( Ability^ ability, const unsigned max_slot_level, List_t< List_t< Decoration^ >^ >% rel_deco_map );
	static Decoration^ GetBestDecoration( Ability^ ability, const unsigned max_slot_level, const unsigned hr );
};

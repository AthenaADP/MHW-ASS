#pragma once
#include "Common.h"
#include <cliext/map>

ref struct Skill;
ref struct Query;
ref struct SkillTag;

ref struct Ability
{
	typedef System::Collections::Generic::Dictionary< unsigned, Skill^ > SkillMap_t;
	System::String^ name;
	List_t< SkillTag^ > tags;
	bool relevant, related, set_ability, unlocked_by_set_bonus;
	unsigned static_index, max_level, set_bonus_unlocked_at_hr;
	int order;
	SkillMap_t skills;

	Decoration^ decoration;

	Skill^ GetSkill( const unsigned amount );
	Skill^ GetWorstGoodSkill();

	static List_t< Ability^ > static_abilities, ordered_abilities; //for sorting alphabetically
	static Map_t< System::String^, Ability^ > static_ability_map;
	static Ability^ FindAbility( System::String^ name );
	static void UpdateOrdering();
};

ref struct SpecificAbility
{
	static Ability^ fire_res;
	static Ability^ water_res;
	static Ability^ thunder_res;
	static Ability^ ice_res;
	static Ability^ dragon_res;
	static Ability^ defence;
	static Ability^ nonelemental_boost;
	static Ability^ free_element;
};

int CompareAbilitiesByName( Ability^ a1, Ability^ a2 );

ref struct Skill
{
	System::String^ name;
	unsigned order, static_index, level;
	bool best, impossible;
	Ability^ ability;

	System::String^ description;

	static void Load( System::String^ filename );
	static void LoadSetSkills( System::String^ filename );
	static void LoadLanguage( System::String^ filename );
	static void LoadDescriptions( System::String^ filename );
	static Map_t< Skill^, Skill^ > set_skill_map;
	static List_t< Skill^ > static_skills, ordered_skills; //for sorting alphabetically
	static Map_t< System::String^, Skill^ > static_skill_map;
	static Skill^ FindSkill( System::String^ name );
	static void UpdateOrdering();
	static bool UpdateImpossibleSkills( const unsigned hr );
};

ref struct AbilityPair
{
	Ability^ ability;
	unsigned amount;

	AbilityPair( Ability^ ab, const unsigned am )
		: ability( ab ), amount( am ) {}
};

void FindRelatedSkills( List_t< System::Windows::Forms::ComboBox^ >% skills, List_t< System::Windows::Forms::ComboBox^ >% skill_filters, List_t< Map_t< unsigned, unsigned >^ >% index_maps );
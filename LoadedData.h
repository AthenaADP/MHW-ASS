#pragma once
#include <vcclr.h>

#include "Armor.h"
#include "Decoration.h"
#include "Solution.h"

ref struct SkillTag
{
	System::String^ name;

	SkillTag( System::String^ s );

	static List_t< SkillTag^ > tags;
	static SkillTag^ FindTag( System::String^ tag );
	static void Load( System::String^ filename );
	static void LoadLanguage( System::String^ filename );
};

void AddToList( List_t< Charm^ >^ list, Charm^ charm, List_t< Ability^ >^ rel_abilities, List_t< Charm^ >^ inf_charms, const bool adv );
void AddToList( List_t< Armor^ >^ list, Armor^ armor, List_t< Ability^ >^ rel_abilities, List_t< Armor^ >^ inf_armors, const bool adv );

ref class LoadedData
{
public:
	Skill^ FindSkill( const unsigned skill );

	void LoadDataFiles();

	void GetRelevantData( Query^ query );

	void GetRelevantArmors( Query^ query, List_t< Armor^ >^ rel_armor, List_t< Armor^ >^ list, List_t< Armor^ >^ inf_armor, const bool check_disabled );
};

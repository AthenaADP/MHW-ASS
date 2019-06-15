#pragma once
#include "Common.h"
#include <cliext/utility>

ref struct Skill;
ref struct Ability;
ref struct Armor;
ref struct Charm;
ref struct ArmorEquivalence;
ref struct Decoration;
ref struct AbilityPair;

typedef System::Collections::Generic::Dictionary< Ability^, unsigned > AbilityMap;

ref struct Query : public System::Object
{
	List_t< Skill^ > skills;
	Gender gender;
	unsigned hr, total_skill_points_required;
	bool include_arena, allow_lower_tier, allow_fair_wind, no_decos, my_decos, always_search_alpha;

	array< unsigned >^ weapon_slots;
	List_t< List_t< Armor^ >^ > rel_armor, inf_armor;
	List_t< Ability^ > rel_abilities;
	List_t< Decoration^ > rel_decorations;
	List_t< Charm^ > rel_charms, inf_charms;
};


ref struct ThreadSearchData
{
	Query^ query;
	Charm^ charm;
};

ref struct ThreadSearchData2
{
	Query^ query;
	array< Armor^ >^ armors;
	List_t< Charm^ >^ charms;
};

ref struct CalculationData;

ref struct Solution
{
	List_t< Armor^ > armors;
	List_t< Decoration^ > decorations;
	List_t< Skill^ > extra_skills, potential_extra_skills;
	AbilityMap abilities;
	Charm^ charm;
	System::Drawing::Bitmap^ preview_image;

	int fire_res, ice_res, water_res, thunder_res, dragon_res;
	unsigned defence, max_defence, aug_defence, rarity, difficulty, total_slots_spare;
	array< unsigned >^ slots_spare, ^family_score;

	List_t< Solution^ > armor_swaps;

	unsigned long long GetHash();
	
	bool MatchesQuery( Query^ query, const bool find_armor_swaps );
	bool HasDLCDisabledArmor();
	void CalculateData();
	void CalculateFamilyScore();
	void CalculateSkillModifiers();

private:
	CalculationData^ data;

	Solution^ CreateArmorSwap( Query^ query, int armor_type, Armor^ armor );

	bool ImpossibleNoDecorations( Query^ query );
	void CalculateExtraSkills( Query^ query );
	void CalculatePotentialExtraSkills();
	void ReduceCharm();
	void FindPotentialSkillUpgrades();
	void FindArmorSwaps( Query^ query );
};

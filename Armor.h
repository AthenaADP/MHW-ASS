#pragma once
#include "Common.h"
#include "Skill.h"

ref struct Query;
ref struct ArmorDLC;

ref struct Armor : public AdvancedSearchOptions
{
	enum class ArmorType { HEAD = 0, BODY, ARMS, WAIST, LEGS, NumArmorTypes };
	System::String^ name;
	Armor^ alpha_version;
	unsigned hr, defence, max_defence, aug_defence, rarity, difficulty, index, family, male_image, female_image;
	unsigned total_slots, highest_slot_level, total_slot_level, slot_product, total_relevant_skill_points;
	int ice_res, water_res, fire_res, thunder_res, dragon_res;
	Gender gender;
	bool arena, full_set, dlc_disabled, has_nonelemental_boost, has_free_element, has_beta, relevant;
	array< unsigned >^ slots;
	
	List_t< AbilityPair^ > abilities;
	List_t< MaterialComponent^ > components;

	bool IsBetterThan( Armor^ other, List_t< Ability^ >^ rel_abilities );
	bool IsBetterAtNonSkills( Armor^ other );
	bool IsStrictlyBetterThan( Armor^ other );
	unsigned GetSkillAt( Ability^ ability );

	bool MatchesQuery( Query^ query, const bool allow_full );
	bool MeetsRequirements( Query^ query, const bool allow_full );

	bool ContainsAnyAbility( List_t< Ability^ >% to_search );

	static void Load( System::String^ filename, ArmorType armor_type );
	static void LoadLanguage( System::String^ filename, ArmorType armor_type );
	static void LoadDLC( System::String^ filename );
	static void LoadLanguageDLC( System::String^ filename );
	static array< List_t< Armor^ >^ >^ static_armors;
	static List_t< ArmorDLC^ > static_dlc;
	static List_t< array< Armor^ >^ > static_full_sets;
	static void AddFullSetArmor( Armor^ armor, ArmorType armor_type );
};

ref struct ArmorDLC
{
	System::String^ name;
	List_t< Armor^ > armors;
};

ref struct Family
{
	static unsigned count = 0;
	static Map_t< System::String^, unsigned > map;
	static List_t< List_t< Armor^ >^ > families;
};

ref struct Charm : public AdvancedSearchOptions
{
	System::String^ name;
	List_t< AbilityPair^ > abilities;
	List_t< MaterialComponent^ > components;
	unsigned hr, index, difficulty;
	bool fair_wind;

	unsigned GetSkillPointsFor( Ability^ a );
	bool StrictlyBetterThan( Charm^ other );
	bool BetterThan( Charm^ other );

	bool MatchesQuery( Query^ query );
	
	static void AddToOptimalList( List_t< Charm^ >% lst, Charm^ new_charm );
	static List_t< Charm^ > static_charms;
	static Map_t< System::String^, Charm^ > static_charm_map;
	static Map_t< Ability^, List_t< Charm^ >^ > static_charm_ability_map;
	static Charm^ FindCharm( System::String^ name );
	static Charm^ FindExactCharm( Ability^ ability, const unsigned points );
	static void Load( System::String^ filename );
	static void LoadLanguage( System::String^ filename );
};

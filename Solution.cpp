#include "stdafx.h"
#include "Solution.h"
#include "Armor.h"
#include "Decoration.h"
#include "Skill.h"

using namespace System;

ref struct CalculationData
{
	Query^ query;
	Solution^ solution;
	AbilityMap need;
	Map_t< Skill^, int > relevant;
	List_t< List_t< Decoration^ >^ > rel_decoration_map;
	unsigned total_relevant_skill_points;
};

void AddAbilities( AbilityMap% total, List_t< AbilityPair^ >% abilities )
{
	for each( AbilityPair^ ap in abilities )
	{
		if( total.ContainsKey( ap->ability ) )
			total[ ap->ability ] += ap->amount;
		else
			total.Add( ap->ability, ap->amount );
	}
}

void AddDecoration( Solution^ solution, Decoration^ deco )
{
	solution->decorations.Add( deco );
	AddAbilities( solution->abilities, deco->abilities );
}

Decoration^ GetBestAvailableDecoration( CalculationData^ data, Ability^ ability, const unsigned slots_spare )
{
	return ability->relevant
		? Decoration::GetBestDecoration( ability, slots_spare, data->rel_decoration_map )
		: Decoration::GetBestDecoration( ability, slots_spare, data->query->hr );
}

void AddInitialArmor( Armor^ armor, Solution^ solution )
{
	if( armor )
	{
		for( unsigned slot_level = 1; slot_level <= 3; ++slot_level )
			solution->slots_spare[ slot_level ] += armor->slots[ slot_level ];
		
		solution->defence += armor->defence;
		solution->max_defence += armor->max_defence;
		solution->aug_defence += armor->aug_defence;
		solution->ice_res += armor->ice_res;
		solution->fire_res += armor->fire_res;
		solution->water_res += armor->water_res;
		solution->dragon_res += armor->dragon_res;
		solution->thunder_res += armor->thunder_res;
		if( armor->rarity != 99 )
			solution->rarity += armor->rarity;
		solution->difficulty += armor->difficulty;
		AddAbilities( solution->abilities, armor->abilities );
	}
}

void GetInitialData( CalculationData^ data )
{
	for each( Skill^ skill in data->query->skills )
	{
		data->solution->abilities.Add( skill->ability, 0 );
		data->need.Add( skill->ability, skill->level );
		data->relevant.Add( skill, 0 );
	}
	if( data->solution->charm )
	{
		for each( AbilityPair^ ap in data->solution->charm->abilities )
		{
			if( !data->need.ContainsKey( ap->ability ) )
				data->need.Add( ap->ability, 0 );
		}
	}

	data->solution->slots_spare = gcnew array< unsigned >( 4 );
	for( int i = 1; i < 4; ++i )
		data->solution->slots_spare[ i ] = data->query->weapon_slots[ i ];
	
	data->solution->defence = data->solution->max_defence = data->solution->aug_defence = data->solution->fire_res = data->solution->ice_res = data->solution->water_res
		= data->solution->thunder_res = data->solution->dragon_res = data->solution->rarity = data->solution->difficulty = 0;

	AddInitialArmor( data->solution->armors[ 0 ], data->solution );
	AddInitialArmor( data->solution->armors[ 1 ], data->solution );
	AddInitialArmor( data->solution->armors[ 2 ], data->solution );
	AddInitialArmor( data->solution->armors[ 3 ], data->solution );
	AddInitialArmor( data->solution->armors[ 4 ], data->solution );

	data->solution->total_slots_spare = data->solution->slots_spare[ 1 ] + data->solution->slots_spare[ 2 ] + data->solution->slots_spare[ 3 ];

	if( data->solution->charm )
	{
		AddAbilities( data->solution->abilities, data->solution->charm->abilities );
	}

	List_t< Skill^ > activated_set_skills;
	AbilityMap::Enumerator e = data->solution->abilities.GetEnumerator();
	while( e.MoveNext() )
	{
		if( e.Current.Key->set_ability && e.Current.Value > 0 )
		{
			auto e2 = e.Current.Key->skills.GetEnumerator();
			while( e2.MoveNext() )
			{
				if( e2.Current.Key <= e.Current.Value )
					activated_set_skills.Add( e2.Current.Value );
			}
		}
	}
	for each( Skill^ set_skill in activated_set_skills )
	{
		Ability^ bonus = Skill::set_skill_map[ set_skill ]->ability;
		if( !data->solution->abilities.ContainsKey( bonus ) )
			data->solution->abilities.Add( bonus, 1 );
		else
			data->solution->abilities[ bonus ]++;
	}

	data->total_relevant_skill_points = 0;

	e = data->solution->abilities.GetEnumerator();
	while( e.MoveNext() )
	{
		if( e.Current.Key->relevant )
			data->total_relevant_skill_points += e.Current.Value;
	}
}

bool CalculateDecorations( CalculationData^ data )
{
	for each( Skill^ skill in data->query->skills )
	{
		if( !skill->ability->decoration )
			continue;

		const unsigned have = data->solution->abilities[ skill->ability ];
		const unsigned need = data->need[ skill->ability ];
		if( have >= need )
			continue;

		unsigned required = need - have;
		if( data->query->my_decos && required > skill->ability->decoration->num_owned )
			return false;

		for( unsigned slot_level = skill->ability->decoration->slot_level; slot_level <= 3 && required > 0; ++slot_level )
		{
			if( data->solution->slots_spare[ slot_level ] > 0 )
			{
				const unsigned to_use = Math::Min( data->solution->slots_spare[ slot_level ], required );
				data->solution->slots_spare[ slot_level ] -= to_use;
				required -= to_use;

				for( unsigned i = 0; i < to_use; ++i )
					AddDecoration( data->solution, skill->ability->decoration );
			}
		}
		if( required > 0 )
			return false;
	}

	data->solution->total_slots_spare = data->solution->slots_spare[ 1 ] + data->solution->slots_spare[ 2 ] + data->solution->slots_spare[ 3 ];
	return true;
}

void Solution::ReduceCharm()
{
	if( charm->abilities.Count != 1 )
		return;

	AbilityPair^ ap = charm->abilities[ 0 ];
	const unsigned need = data->need[ ap->ability ];
	if( need > 0 )
	{
		const unsigned have = abilities[ ap->ability ];
		if( have > need )
		{
			//if we can cut the charm entirely, do that
			if( have - need >= ap->amount )
			{
				abilities[ ap->ability ] -= ap->amount;
				charm = nullptr;
			}
			else if( have > ap->ability->max_level )
			{
				//otherwise, we'll keep the charm but reduce it down if it exceeds the max level of the ability
				const unsigned excess = have - ap->ability->max_level;
				charm = Charm::FindExactCharm( ap->ability, ap->amount - excess );
			}
		}
	}
}

void Solution::FindPotentialSkillUpgrades()
{
	if( total_slots_spare == 0 )
		return;
	
	//try to upgrade each relevant skill using spare slots
	for each( Collections::Generic::KeyValuePair< Ability^, unsigned >^ ap in abilities )
	{
		Ability^ ab = ap->Key;
		if( !ab->decoration || ap->Value == ab->max_level )
			continue;

		unsigned val = ap->Value;
		Skill^ current_skill = ab->GetSkill( val );
		if( current_skill && current_skill->best )
			continue;

		for( unsigned slot_level = ab->decoration->slot_level; slot_level <= 3; ++slot_level )
		{
			val += slots_spare[ slot_level ];
		}

		val = Math::Min( val, ab->max_level );

		Skill^ skill = ab->GetSkill( val );
		if( !skill || skill == current_skill )
			continue;

		potential_extra_skills.Add( skill );
	}
}

void Solution::FindArmorSwaps( Query^ query )
{
	//for each armor piece we want to optimize
	for( int armor_type = 0; armor_type < (int)Armor::ArmorType::NumArmorTypes; ++armor_type )
	{
		Armor^ armor = armors[ armor_type ];
		if( !armor )
			continue;

		unsigned total_required = 0;
		Ability^ required_set_skill = nullptr;
		Map_t< Ability^, int > required_abilities;
		for each( AbilityPair^ ap in armor->abilities )
		{
			if( ap->ability->relevant )
			{
				if( ap->ability->set_ability )
				{
					required_set_skill = ap->ability;
				}
				else
				{
					Assert( data->need.ContainsKey( ap->ability ), L"need doesn't contain ability" );
					Assert( abilities.ContainsKey( ap->ability ), L"abilities doesn't contain ability" );
					const int required = int( data->need[ ap->ability ] - abilities[ ap->ability ] + ap->amount );
					total_required += (unsigned)Math::Max( 0, required );
					required_abilities.Add( ap->ability, required );
				}
			}
		}
		
		//search unused armors for potential swaps
		for each( Armor^ other_armor in query->inf_armor[ armor_type ] )
		{
			if( other_armor->force_disable || other_armor->relevant || other_armor->dlc_disabled )
				continue;

			if( other_armor->total_slots + other_armor->total_relevant_skill_points < total_required )
				continue;

			if( required_set_skill && other_armor->GetSkillAt( required_set_skill ) == 0 )
				continue;

			Solution^ swap = CreateArmorSwap( query, (int)armor_type, other_armor );
			if( swap->MatchesQuery( query, false ) )
				armor_swaps.Add( swap );
		}
	}
}

Solution^ Solution::CreateArmorSwap( Query^ query, int armor_type, Armor^ armor )
{
	Solution^ swap = gcnew Solution();
	swap->armors.AddRange( %armors );
	swap->armors[ armor_type ] = armor;
	swap->charm = charm;
	return swap;
}

void Solution::CalculatePotentialExtraSkills()
{
	potential_extra_skills.Clear();

	FindPotentialSkillUpgrades();
}

bool Solution::ImpossibleNoDecorations( Query^ query )
{
	for each( Skill^ skill in query->skills )
	{
		const int have = abilities[ skill->ability ],
				  need = data->need[ skill->ability ];
		
		if( have < need )
		{
			if( query->no_decos || !skill->ability->decoration || !skill->ability->decoration->can_use )
				return true;

			bool found = false;
			for each( Decoration^ deco in query->rel_decorations )
			{
				if( deco->abilities.Count > 0 && deco->abilities[0]->ability == skill->ability )
				{
					found = true;
					break;
				}
			}
			if( !found )
				return true;

			const unsigned required = need - have;
			if( total_slots_spare < required ||
				skill->ability->decoration->slot_level == 3 && required > slots_spare[3] ||
				skill->ability->decoration->slot_level == 2 && required > slots_spare[3] + slots_spare[2] )
				return true;
		}
	}
	return false;
}

bool Solution::MatchesQuery( Query^ query, const bool find_armor_swaps )
{
	data = gcnew CalculationData;
	data->query = query;
	data->solution = this;

	for( int i = 0; i < 4; ++i )
		data->rel_decoration_map.Add( gcnew List_t< Decoration^ > );
	for each( Decoration^ deco in query->rel_decorations )
	{
		data->rel_decoration_map[ deco->slot_level ]->Add( deco );
	}
	
	GetInitialData( data );

	if( data->total_relevant_skill_points + total_slots_spare < query->total_skill_points_required )
		return false;

	if( ImpossibleNoDecorations( query ) )
		return false;

	if( !query->no_decos && !CalculateDecorations( data ) )
		return false;
	
	for each( Skill^ skill in query->skills )
	{
		const int have = abilities[ skill->ability ],
				  need = data->need[ skill->ability ];
		if( have < need )
			return false;
	}
	
	//not very useful in MHW
	/*if( charm )
	{
		ReduceCharm();
	}*/

	CalculateExtraSkills( query );
	CalculatePotentialExtraSkills();
	CalculateFamilyScore();
	CalculateSkillModifiers(); //e.g. apply +fire res, etc.
	
	Assert( total_slots_spare < 1000, L"Total slots spare is negative" );
	for each( Decoration^ deco in decorations )
	{
		difficulty += deco->difficulty;
	}
	if( charm )
		difficulty += charm->difficulty;
	
	if( find_armor_swaps )
		FindArmorSwaps( query );
	return true;
}

bool Solution::HasDLCDisabledArmor()
{
	for each( Armor^ armor in armors )
	{
		if( armor && armor->dlc_disabled )
			return true;
	}
	return false;
}

void Solution::CalculateExtraSkills( Query^ query )
{
	extra_skills.Clear();
	for each( Collections::Generic::KeyValuePair< Ability^, unsigned >^ ap in abilities )
	{
		Ability^ ab = ap->Key;

		Skill^ sk = ab->GetSkill( ap->Value );
		if( sk && !sk->ability->set_ability && !data->relevant.ContainsKey( sk ) )
		{
			extra_skills.Add( sk );
		}
	}
}

void Solution::CalculateData()
{
	abilities.Clear();
	fire_res = ice_res = thunder_res = water_res = dragon_res = defence = rarity = difficulty = max_defence = aug_defence = 0;
	
	for each( Armor^ armor in armors )
	{
		if( !armor )
			continue;
		
		fire_res += armor->fire_res;
		ice_res += armor->ice_res;
		thunder_res += armor->thunder_res;
		water_res += armor->water_res;
		dragon_res += armor->dragon_res;
		defence += armor->defence;
		if( armor->rarity != 99 )
			rarity += armor->rarity;
		difficulty += armor->difficulty;
		max_defence += armor->max_defence;
		aug_defence += armor->aug_defence;
	
		Utility::AddAbilitiesToMap( armor->abilities, abilities );
	}
	
	if( charm )
		Utility::AddAbilitiesToMap( charm->abilities, abilities );

	for each( Decoration^ dec in decorations )
		Utility::AddAbilitiesToMap( dec->abilities, abilities );

	CalculateFamilyScore();
	CalculateSkillModifiers();
	CalculatePotentialExtraSkills();
}

void Solution::CalculateSkillModifiers()
{
#define CHECK_RES(X) else if( e.Current.Key == SpecificAbility::X ) \
	{ \
		if( e.Current.Value == 1 ) \
			X += 6; \
		else if( e.Current.Value == 2 ) \
			X += 12; \
		else if( e.Current.Value >= 3 ) \
		{ \
			X += 20; \
			defence += 10; \
			max_defence += 10; \
			aug_defence += 10; \
		} \
	}

	AbilityMap::Enumerator e = abilities.GetEnumerator();
	while( e.MoveNext() )
	{
		if( e.Current.Key == SpecificAbility::defence )
		{
			const unsigned bonus = e.Current.Value * 5; // +5 defence per level
			defence += bonus;
			max_defence += bonus;
			aug_defence += bonus;

			if( e.Current.Value > 3 ) // +3 all res at level 4+
			{
				fire_res += 3;
				water_res += 3;
				thunder_res += 3;
				ice_res += 3;
				dragon_res += 3;
			}
		}
		CHECK_RES( fire_res )
		CHECK_RES( water_res )
		CHECK_RES( thunder_res )
		CHECK_RES( ice_res )
		CHECK_RES( dragon_res )
	}
}

unsigned GetFamilyScore( List_t< Armor^ >% armors, const unsigned family )
{
	array< unsigned >^ rare_count = gcnew array< unsigned >( 11 );
	for each( Armor^ armor in armors )
	{
		if( armor && armor->family == family && (int)armor->rarity < rare_count->Length )
			rare_count[ armor->rarity ]++;
	}
	unsigned res = 0;
	for( int i = 0; i < rare_count->Length; ++i )
		if( rare_count[ i ] > res )
			res = rare_count[ i ];
	return res;
}

void Solution::CalculateFamilyScore()
{
	family_score = gcnew array< unsigned >( 3 );
	array< unsigned >^ count = gcnew array< unsigned >( Family::count );
	for each( Armor^ armor in armors )
	{
		if( armor )
			count[ armor->family ]++;
	}
	array< unsigned >^ count2 = gcnew array< unsigned >( 6 );
	for( int i = 0; i < count2->Length; ++i ) count2[ i ] = Family::count;
	for( int i = 0; i < count->Length; ++i )
		if( count[ i ] > 0 )
			count2[ count[ i ] ] = i;
	array< unsigned >::Sort( count );

	switch( count[ Family::count - 1 ] )
	{
	case 2:
		family_score[ 1 ] = GetFamilyScore( armors, count2[ 2 ] );
		{
			const unsigned max_val = -1;
			array< unsigned >^ other_armor_families = { max_val, max_val, max_val };
			int i = 0;
			for each( Armor^ armor in armors )
			{
				if( armor && armor->family != count2[ 2 ] )
					other_armor_families[ i++ ] = armor->family;
			}
			if( other_armor_families[ 0 ] == other_armor_families[ 1 ] )
			{
				family_score[ 0 ] = 2;
				family_score[ 2 ] = GetFamilyScore( armors, other_armor_families[ 0 ] );
			}
			else if( other_armor_families[ 0 ] == other_armor_families[ 2 ] )
			{
				family_score[ 0 ] = 2;
				family_score[ 2 ] = GetFamilyScore( armors, other_armor_families[ 0 ] );
			}
			else if( other_armor_families[ 1 ] == other_armor_families[ 2 ] )
			{
				family_score[ 0 ] = 2;
				family_score[ 2 ] = GetFamilyScore( armors, other_armor_families[ 1 ] );
			}
			else family_score[ 0 ] = 1;
		}
		break;
	case 3:
		family_score[ 1 ] = GetFamilyScore( armors, count2[ 3 ] );
		if( count2[ 2 ] != Family::count )
		{
			family_score[ 0 ] = 4;
			family_score[ 2 ] = GetFamilyScore( armors, count2[ 2 ] );
		}
		else
		{
			family_score[ 0 ] = 3;
		}
		break;
	case 4:
		family_score[ 0 ] = 5;
		family_score[ 1 ] = GetFamilyScore( armors, count2[ 4 ] );
		break;
	case 5:
		family_score[ 0 ] = 6;
		family_score[ 1 ] = GetFamilyScore( armors, count2[ 5 ] );
		break;
	}
}

int CalculateBonusExtraSkillPoints( Ability^ ability, const unsigned slot_level, int spare_slots, const unsigned hr )
{
	int total = 0;
	while( spare_slots > 0 )
	{
		Decoration^ deco = Decoration::GetBestDecoration( ability, slot_level, hr );
		if( deco == nullptr )
			break;
		total += deco->abilities [ 0 ]->amount;
		spare_slots--;
	}
	return total;
}

unsigned long long Solution::GetHash()
{
	unsigned long long hash = 0;
	for( unsigned i = 0; i < (unsigned)Armor::ArmorType::NumArmorTypes; ++i )
	{
		if( armors[ i ] )
			hash += static_cast< unsigned long long >( armors[ i ]->index + 1 ) << ( i * 8 + 8 );
	}

	if( charm )
		hash += charm->index + 1;

	return hash;
}

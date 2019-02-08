#include "stdafx.h"
#include <fstream>
#include "Skill.h"
#include "Solution.h"
#include "LoadedData.h"

using namespace System;

int CompareAbilities( Ability^ a, Ability^ b )
{
	return String::Compare( a->name, b->name );
}

int CompareSkills( Skill^ a, Skill^ b )
{
	return String::Compare( a->name, b->name );
}

Skill^ Ability::GetSkill( const unsigned amount )
{
	if( amount == 0 )
		return nullptr;

	unsigned best = 0;
	SkillMap_t::Enumerator iter = skills.GetEnumerator();
	if( amount > 0 )
	{
		while( iter.MoveNext() )
			if( iter.Current.Key <= amount && iter.Current.Key > best )
				best = iter.Current.Key;
	}
	else
	{
		while( iter.MoveNext() )
			if( iter.Current.Key >= amount && iter.Current.Key < best )
				best = iter.Current.Key;
	}
	if( best == 0 ) return nullptr;
	Assert( skills.ContainsKey( best ), L"Skill dictionary doesn't contain this skill?" );
	return skills[ best ];
}

Skill^ Ability::GetWorstGoodSkill()
{
	Skill^ best = nullptr;
	SkillMap_t::Enumerator iter = skills.GetEnumerator();
	while( iter.MoveNext() )
	{
		if( iter.Current.Key <= 0 )
			continue;

		if( !best || iter.Current.Key < best->level )
			best = iter.Current.Value;
	}
	return best;
}

Ability^ Ability::FindAbility( System::String^ name )
{
	if( name && static_ability_map.ContainsKey( name ) )
		return static_ability_map[ name ];
	return nullptr;
}

void Ability::UpdateOrdering()
{
	ordered_abilities.Clear();
	ordered_abilities.AddRange( %static_abilities );
	ordered_abilities.Sort( gcnew Comparison< Ability^ >( CompareAbilities ) );
	for( int i = 0; i < ordered_abilities.Count; ++i )
		ordered_abilities[ i ]->order = i;
}

bool ContainsString( List_t< String^ >% vec, String^ item )
{
	for( int i = 0; i < vec.Count; ++i )
		if( vec[ i ] == item ) return true;
	return false;
}

int FindSkillIndex( Skill^ skill, List_t< Skill^ >^ vec )
{
	for( int i = 0; i < vec->Count; ++i )
		if( vec[ i ] == skill )
			return i;
	return -1;
}

void Skill::LoadSetSkills( String^ filename )
{
	IO::StreamReader fin( filename );

	String^ temp;
	while( !fin.EndOfStream && temp != L"" )
	{
		temp = fin.ReadLine();
		
		if( temp == L"" )
			break;
		else if( temp[ 0 ] == L'#' )
			continue;

		List_t< String^ > split;
		Utility::SplitString( %split, temp, L',' );

		if( split.Count < 3 )
			continue;

		Skill^ subskill = FindSkill( split[ 2 ] );
		Assert( subskill, "No such set subskill found: " + split[ 2 ] );

		const unsigned points = Convert::ToInt32( split[ 1 ] );

		Ability^ ability = Ability::FindAbility( split[0] );
		if( ability )
		{
			ability->max_level = Math::Max( ability->max_level, points );
		}
		else
		{
			ability = gcnew Ability();
			ability->name = split[ 0 ];
			ability->max_level = points;
			ability->set_ability = true;
			ability->set_bonus_unlocked_at_hr = 0;
			ability->unlocked_by_set_bonus = false;
			ability->static_index = Ability::static_abilities.Count;
			Ability::static_abilities.Add( ability );
			Ability::static_ability_map.Add( ability->name, ability );
		}

		Skill^ skill = gcnew Skill();
		skill->ability = ability;
		skill->name = ability->name + L" Lv" + Convert::ToString( points );
		skill->level = points;
		skill->best = true;
		skill->static_index = static_skills.Count;
		static_skills.Add( skill );
		static_skill_map.Add( skill->name, skill );

		ability->skills.Add( points, skill );

		Ability::SkillMap_t::Enumerator e = ability->skills.GetEnumerator();
		while( e.MoveNext() )
		{
			if( e.Current.Key < points )
				e.Current.Value->best = false;
		}

		set_skill_map.Add( skill, subskill );
	}
}

void Skill::Load( String^ filename )
{
	IO::StreamReader fin( filename );

	Ability::static_abilities.Clear();
	Ability::static_ability_map.Clear();
	static_skills.Clear();
	static_skill_map.Clear();
	ordered_skills.Clear();
	Ability::static_abilities.Capacity = 150;
	static_skills.Capacity = 150;

	//Name,Max Level,Tags
	String^ temp;
	while( !fin.EndOfStream && temp != L"" )
	{
		temp = fin.ReadLine();
		if( temp == L"" ) break;
		else if( temp[ 0 ] == L'#' ) continue;
		List_t< String^ > split;
		Utility::SplitString( %split, temp, L',' );

		Ability^ ability = gcnew Ability;
		ability->name = split[ 0 ];
		ability->static_index = Ability::static_abilities.Count;
		ability->order = Ability::static_abilities.Count;
		ability->max_level = Convert::ToInt32( split[ 1 ] );
		ability->set_ability = false;
		ability->set_bonus_unlocked_at_hr = 0;
		ability->unlocked_by_set_bonus = false;
		Ability::static_abilities.Add( ability );
		Ability::static_ability_map[ ability->name ] = ability;

		for( int i = 2; i < split.Count; ++i )
		{
			if( split[ i ] != L"" )
			{
				SkillTag^ tag = SkillTag::FindTag( split[ i ] );
				if( !tag )
					throw gcnew Exception( L"Skill Tag '" + split[ i ] + L"' does not exist" );
				
				ability->tags.Add( tag );
			}
		}

		for( unsigned level = 1; level <= ability->max_level; ++level )
		{
			Skill^ skill = gcnew Skill;
			skill->best = false;
			skill->impossible = false;
			if( ability->max_level == 1 )
				skill->name = split[ 0 ];
			else
				skill->name = split[ 0 ] + L" Lv" + level;
			skill->ability = ability;
			skill->level = level;

			skill->ability->skills[ level ] = skill;
			skill->static_index = static_skills.Count;
			skill->order = static_skills.Count;
			static_skills.Add( skill );
			static_skill_map[ skill->name ] = skill;
		}
	}
	
	fin.Close();
	static_skills.TrimExcess();
	Ability::static_abilities.TrimExcess();
	Ability::UpdateOrdering();
	Skill::UpdateOrdering();

	for each( Ability^ a in Ability::static_abilities )
	{
		Skill^ s = a->GetSkill( 1000 );
		if( s )
			s->best = true;
	}

	SpecificAbility::defence = Ability::FindAbility( L"Defense Boost" );
	SpecificAbility::fire_res = Ability::FindAbility( L"Fire Resistance" );
	SpecificAbility::water_res = Ability::FindAbility( L"Water Resistance" );
	SpecificAbility::thunder_res = Ability::FindAbility( L"Thunder Resistance" );
	SpecificAbility::ice_res = Ability::FindAbility( L"Ice Resistance" );
	SpecificAbility::dragon_res = Ability::FindAbility( L"Dragon Resistance" );
	SpecificAbility::free_element = Ability::FindAbility( L"Free Elem/Ammo Up" );
	SpecificAbility::nonelemental_boost = Ability::FindAbility( L"Non-elemental Boost" );
}

Skill^ Skill::FindSkill( System::String^ name )
{
	if( static_skill_map.ContainsKey( name ) )
		return static_skill_map[ name ];
	else return nullptr;
}

void Skill::LoadLanguage( System::String^ filename )
{
	Ability::static_ability_map.Clear();
	IO::StreamReader fin( filename );

	for( int i = 0; i < Ability::static_abilities.Count; )
	{
		String^ line = fin.ReadLine();
		if( line == L"" || line[ 0 ] == L'#' )
			continue;
		
		Ability::static_abilities[ i ]->name = line;
		Ability::static_ability_map.Add( line, Ability::static_abilities[ i ] );
		
		i++;
	}

	static_skill_map.Clear();
	for each( Skill^ skill in static_skills )
	{
		if( skill->ability->max_level == 1 )
			skill->name = skill->ability->name;
		else
			skill->name = FormatString2( SkillLevel, skill->ability->name, skill->level );

		static_skill_map.Add( skill->name, skill );
	}
}

void Skill::LoadDescriptions( System::String^ filename )
{
	IO::StreamReader fin( filename );
	for( int i = 0; i < Skill::static_skills.Count; ++i )
	{
		Skill::static_skills[ i ]->description = fin.ReadLine();
	}
}

void Skill::UpdateOrdering()
{
	ordered_skills.Clear();
	ordered_skills.AddRange( %static_skills );
	ordered_skills.Sort( gcnew Comparison< Skill^ >( CompareSkills ) );
	for( int i = 0; i < ordered_skills.Count; ++i )
		ordered_skills[ i ]->order = i;
}

unsigned GetSkillPointsAt( List_t< AbilityPair^ >% ability_list, Ability^ ab )
{
	for each( AbilityPair^ ap in ability_list )
	{
		if( ap->ability == ab )
			return ap->amount;
	}
	return 0;
}

unsigned GetMostPossible( List_t< Armor^ >^ armors, Ability^ ab, const unsigned hr )
{
	unsigned total = 0;
	for each( Armor^ armor in armors )
	{
		if( armor->hr <= hr && !armor->dlc_disabled )
		{
			unsigned armor_total = GetSkillPointsAt( armor->abilities, ab );
			if( ab->decoration && ab->decoration->hr <= hr )
			{
				for( unsigned i = ab->decoration->slot_level; i <= 3; ++i )
					armor_total += armor->slots[ i ];
			}
			
			total = Math::Max( total, armor_total );
		}
	}
	return total;
}

unsigned GetMostPossible( List_t< Charm^ >% charms, Ability^ ab, const unsigned hr )
{
	unsigned max = 0;
	for each( Charm^ charm in charms )
	{
		if( charm->hr <= hr )
			max = Math::Max( max, GetSkillPointsAt( charm->abilities, ab ) );
	}
	return max;
}

bool Skill::UpdateImpossibleSkills( const unsigned hr )
{
	bool changes = false;
	for each( Ability^ ab in Ability::static_abilities )
	{
		unsigned total = 0;
		total += GetMostPossible( Armor::static_armors[0], ab, hr );
		total += GetMostPossible( Armor::static_armors[1], ab, hr );
		total += GetMostPossible( Armor::static_armors[2], ab, hr );
		total += GetMostPossible( Armor::static_armors[3], ab, hr );
		total += GetMostPossible( Armor::static_armors[4], ab, hr );

		if( Charm::static_charm_ability_map.ContainsKey( ab ) )
			total += GetMostPossible( *Charm::static_charm_ability_map[ ab ], ab, hr );

		//you can have up to 3 level 3 weapon slots
		if( ab->decoration && ab->decoration->hr <= hr )
			total += 3;

		//check if this ability can be unlocked via set bonus
		if( ab->unlocked_by_set_bonus && ab->set_bonus_unlocked_at_hr <= hr )
			total++;

		Ability::SkillMap_t::Enumerator e = ab->skills.GetEnumerator();
		while( e.MoveNext() )
		{
			Skill^ skill = e.Current.Value;
			const bool impossible = skill->level > total;
			changes |= skill->impossible != impossible;
			skill->impossible = impossible;
		}
	}
	return changes;
}

void FindRelatedSkills( List_t< System::Windows::Forms::ComboBox^ >% skills, List_t< System::Windows::Forms::ComboBox^ >% skill_filters, List_t< Map_t< unsigned, unsigned >^ >% index_maps )
{
	for each( Ability^ a in Ability::static_abilities )
	{
		a->related = false;
	}

	List_t< Ability^ > selected_abilities;
	array< unsigned >^ relation_count = gcnew array< unsigned >( Ability::static_abilities.Count );
	for( int i = 0; i < skills.Count; ++i )
	{
		if( skills[ i ]->SelectedIndex <= 0 ||
			skill_filters[ i ]->SelectedIndex == 2 )
			continue;

		Map_t< unsigned, unsigned >^ map = index_maps[ i ];
		Ability^ a = Skill::static_skills[ map[ skills[ i ]->SelectedIndex ] ]->ability ;
		relation_count[ a->static_index ] = 100; //lots because selected skills are related by definition
		selected_abilities.Add( a );
	}

	if( selected_abilities.Count == 0 )
		return;
	
	for each( List_t< Armor^ >^ la in Armor::static_armors )
	{
		for each( Armor^ a in la )
		{
			if( a->ContainsAnyAbility( selected_abilities ) )
			{
				for each( AbilityPair^ ap in a->abilities )
				{
					if( ap->amount > 0 )
						relation_count[ ap->ability->static_index ]++;
				}
			}
		}
	}
	
	for( int i = 0; i < relation_count->Length; ++i )
	{
		if( relation_count[ i ] > 4 )
			Ability::static_abilities[ i ]->related = true;
	}
}

int CompareAbilitiesByName( Ability^ a1, Ability^ a2 )
{
	return System::String::Compare( a1->name, a2->name );
}

void ShowWarningPopup( String^ text )
{
	using namespace System::Windows::Forms;
	MessageBox::Show( text, L"Error loading filters.txt", MessageBoxButtons::OK, MessageBoxIcon::Warning );
}

bool FilterRules::IsRelevant()
{
	if( ability->relevant )
		return false;

	for each( Ability^ a in required )
	{
		if( !a->relevant )
			return false;
	}
	for each( Ability^ a in unrequired )
	{
		if( a->relevant )
			return false;
	}
	return true;
}

void ExtraSkillFilters::Load( System::String^ filename )
{
	IO::StreamReader fin( filename );
	while( !fin.EndOfStream )
	{
		String^ line = fin.ReadLine();
		if( !line || line == L"" || line[ 0 ] == L'#' )
			continue;
		
		List_t< String^ > split;
		Utility::SplitString( %split, line, L',' );
		if( split.Count < 2 )
		{
			ShowWarningPopup( L"Line too short" );
			continue;
		}

		List_t< Ability^ > abilities;
		List_t< bool > plus;

		for( int i = 0; i < split.Count; ++i )
		{
			bool is_plus = false;
			if( split[ i ]->StartsWith( L"-" ) )
			{
				is_plus = false;
			}
			else if( split[ i ]->StartsWith( L"+" ) )
			{
				is_plus = true;
			}
			else
			{
				ShowWarningPopup( L"Token doesn't start with a + or - symbol" );
				continue;
			}

			Ability^ ability = Ability::FindAbility( split[ i ]->Substring( 1 ) );
			if( !ability )
			{
				ShowWarningPopup( L"Unknown skill" );
				continue;
			}
			abilities.Add( ability );
			plus.Add( is_plus );
		}

		if( abilities.Count > 1 )
		{
			FilterRules^ filter = gcnew FilterRules();
			filter->ability = abilities[ 0 ];

			for( int i = 1; i < abilities.Count; ++i )
			{
				if( plus[ i ] )
					filter->required.Add( abilities[ i ] );
				else
					filter->unrequired.Add( abilities[ i ] );
			}

			if( plus[ 0 ] )
				filter_rules.Add( filter->ability, filter );
			else
				extra_filters.Add( filter );
		}
	}
}
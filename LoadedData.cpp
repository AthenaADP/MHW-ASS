#include "stdafx.h"

#include "Skill.h"
#include "Common.h"
#include "LoadedData.h"

using namespace System;

Skill^ LoadedData::FindSkill( const unsigned skill )
{
	Assert( int( skill ) < Skill::static_skills.Count, L"Skill index out of range" );
	return Skill::static_skills[ skill ];
}

SkillTag^ SkillTag::FindTag( String^ tag )
{
	for each( SkillTag^ st in tags )
	{
		if( st->name == tag )
			return st;
	}
	return nullptr;
}

void DumpFamilies( String^ filename )
{
	Map_t< int, List_t< Armor^ >^ > fammap;

	for each( List_t< Armor^ >^ l in Armor::static_armors )
	{
		for each( Armor^ a in l )
		{
			if( !fammap.ContainsKey( a->family ) )
				fammap.Add( a->family, gcnew List_t< Armor^ >() );
			
			fammap[ a->family ]->Add( a );
		}
	}

	IO::StreamWriter fout( filename );
	Map_t< int, List_t< Armor^ >^ >::Enumerator e = fammap.GetEnumerator();
	int last_family = -1;
	while( e.MoveNext() )
	{
		if( e.Current.Key != last_family )
		{
			last_family = e.Current.Key;
			fout.WriteLine( L"\nFamily " + last_family  );
		}
		for each( Armor^ a in e.Current.Value )
		{
			fout.WriteLine( a->name );
		}
	}
}

void LoadedData::LoadDataFiles()
{
	Armor::static_armors = gcnew array< List_t< Armor^ >^ >( int( Armor::ArmorType::NumArmorTypes ) );
	for( int i = 0; i < Armor::static_armors->Length; ++i )
		Armor::static_armors[ i ] = gcnew List_t< Armor^ >;

	Material::LoadMaterials( L"Data/components.txt" );

	SkillTag::Load( L"Data/tags.txt" );
	Skill::Load( L"Data/skills.txt" );
	Skill::LoadSetSkills( L"Data/set_skills.txt" );

	Armor::Load( L"Data/head.txt",  Armor::ArmorType::HEAD );
	Armor::Load( L"Data/body.txt",  Armor::ArmorType::BODY );
	Armor::Load( L"Data/arms.txt",  Armor::ArmorType::ARMS );
	Armor::Load( L"Data/waist.txt", Armor::ArmorType::WAIST );
	Armor::Load( L"Data/legs.txt",  Armor::ArmorType::LEGS );
	Armor::LoadDLC( L"Data/events.txt" );
	
	Decoration::Load( L"Data/decorations.txt" );
	Charm::Load( L"Data/charms.txt" );
	ExtraSkillFilters::Load( L"Data/filters.txt" );
}

void AddToList( List_t< Charm^ >^ list, Charm^ item, List_t< Ability^ >^ rel_abilities, List_t< Charm^ >^ inf_charms, const bool adv )
{
	if( adv && item->force_disable )
		return;
	const bool may_remove_self = !adv || !item->force_enable;
	for( int i = 0; i < list->Count; ++i )
	{
		if( item->BetterThan( list[ i ] ) )
		{
			const bool may_remove = !adv || !list[ i ]->force_enable;
			if( may_remove && !list[ i ]->BetterThan( item ) )
				list->Remove( list[ i-- ] );
		}
		else if( may_remove_self && list[ i ]->BetterThan( item ) )
			return;
	}
	list->Add( item );
	inf_charms->Add( item );
}

void AddToList( List_t< Decoration^ >^ list, Decoration^ item, List_t< Ability^ >^ rel_abilities )
{
	for( int i = 0; i < list->Count; ++i )
	{
		if( item->IsBetterThan( list[ i ], rel_abilities ) )
		{
			if( !list[ i ]->IsBetterThan( item, rel_abilities ) )
				list->Remove( list[ i-- ] );
		}
		else if( list[ i ]->IsBetterThan( item, rel_abilities ) )
				return;
	}
	list->Add( item );
}

void AddToList( List_t< Armor^ >^ list, Armor^ armor, List_t< Ability^ >^ rel_abilities, List_t< Armor^ >^ inf_armor, const bool check_disabled )
{
	if( check_disabled && armor->force_disable )
		return;

//#ifdef _DEBUG
//	const bool test = armor->name->StartsWith( L"Kaiser" );
//#endif

	for( int i = 0; i < list->Count; ++i )
	{
		if( armor->IsBetterThan( list[ i ], rel_abilities ) )
		{
			if( !list[ i ]->IsBetterThan( armor, rel_abilities ) )
			{
				list->Remove( list[ i-- ] );
			}
		}
		else if( list[ i ]->IsBetterThan( armor, rel_abilities ) )
		{
			if( inf_armor )
			{
				inf_armor->Add( armor );
			}
			return;
		}
	}
	list->Add( armor );
	if( inf_armor )
		inf_armor->Add( armor );
}

bool IsBonusArmor( Armor^ a, Query^ query )
{
	return a->MeetsRequirements( query, false ) && a->total_slots >= 2 && a->highest_slot_level > 1 && a->abilities.Count > 0;
}

int CompareArmorByIndex( Armor^ a, Armor^ b )
{
	return a->index < b->index ? -1 : b->index < a->index ? 1 : 0;
}

void LoadedData::GetRelevantArmors( Query^ query, List_t< Armor^ >^ rel_armor, List_t< Armor^ >^ list, List_t< Armor^ >^ inf_armor, const bool check_disabled )
{
	List_t< Armor^ > bonus_armor;

	for each( Armor^ armor in list )
	{
		//const bool test = armor->name->StartsWith( L"Kulve" );

		armor->relevant = false;

		if( check_disabled && armor->force_disable )
			continue;

		if( armor->MatchesQuery( query, false ) )
		{
			AddToList( rel_armor, armor, %query->rel_abilities, inf_armor, false );
		}
		else if( IsBonusArmor( armor, query ) )
			bonus_armor.Add( armor );
	}

	if( query->always_search_alpha )
	{
		for( int i = 0; i < rel_armor->Count; ++i )
		{
			Armor^ alpha = rel_armor[ i ]->alpha_version;
			if( alpha && alpha->total_relevant_skill_points > 0 && !alpha->dlc_disabled && !( check_disabled && alpha->force_disable ) && !Utility::Contains( inf_armor, alpha ) )
			{
				inf_armor->Add( alpha );
			}
		}
	}

	unsigned max_num_slots = 0, max_slot_level = 0, max_total_slot_levels = 0, max_slot_product = 0;
	for each( Armor^ armor in rel_armor )
	{
		armor->relevant = true;

		max_num_slots = Math::Max( max_num_slots, armor->total_slots );
		max_slot_level = Math::Max( max_slot_level, armor->highest_slot_level );
		max_total_slot_levels = Math::Max( max_total_slot_levels, armor->total_slot_level );
		max_slot_product = Math::Max( max_slot_product, armor->slot_product );
	}

	for( int i = 0; i < inf_armor->Count; ++i )
	{
		Armor^ a = inf_armor[ i ];
		if( a->total_relevant_skill_points == 0 &&
			!IsBonusArmor( a, query ) &&
			a->total_slots < 3 &&
			a->total_slots <= max_num_slots &&
			a->highest_slot_level <= max_slot_level &&
			a->slot_product <= max_slot_product &&
			a->total_slot_level <= max_total_slot_levels &&
			!Utility::Contains( rel_armor, a ) )
			inf_armor->RemoveAt( i-- );
	}

	for each( Armor^ armor in rel_armor )
	{
		for( int i = 0; i < inf_armor->Count; ++i )
		{
			if( inf_armor[ i ]->total_relevant_skill_points == 0 && !IsBonusArmor( inf_armor[ i ], query ) && armor->IsStrictlyBetterThan( inf_armor[ i ] ) )
				inf_armor->RemoveAt( i-- );
		}
	}

	if( bonus_armor.Count > 0 )
	{
		inf_armor->AddRange( %bonus_armor );
	}

	inf_armor->Sort( gcnew Comparison< Armor^ >( CompareArmorByIndex ) );
}

void GetRelevantDecorations( Query^ query )
{
	for each( Ability^ ability in query->rel_abilities )
	{
		if( !Decoration::static_decoration_ability_map.ContainsKey( ability ) )
			continue;

		for each( Decoration^ decoration in Decoration::static_decoration_ability_map[ ability ] )
		{
			if( decoration->MatchesQuery( query ) )
			{
				AddToList( %query->rel_decorations, decoration, %query->rel_abilities );
			}
		}
	}
}

void GetRelevantCharms( Query^ query )
{
	for each( Ability^ ability in query->rel_abilities )
	{
		if( !Charm::static_charm_ability_map.ContainsKey( ability ) )
			continue;

		for each( Charm^ charm in Charm::static_charm_ability_map[ ability ] )
		{
			if( charm->MatchesQuery( query ) )
			{
				AddToList( %query->rel_charms, charm, %query->rel_abilities, %query->inf_charms, false );
			}
		}
	}
}

void LoadedData::GetRelevantData( Query^ query )
{
	for each( Ability^ ab in Ability::static_abilities )
	{
		ab->relevant = false;
	}

	List_t< Ability^ > danger_skills;
	//get relevant abilities
	for each( Skill^ skill in query->skills )
	{
		query->rel_abilities.Add( skill->ability );
		skill->ability->relevant = true;
	}

	Map_t< Skill^, Skill^ >::Enumerator e = Skill::set_skill_map.GetEnumerator();
	while( e.MoveNext() )
	{
		if( !e.Current.Key->ability->relevant && e.Current.Value->ability->relevant )
		{
			e.Current.Key->ability->relevant = true;
			query->rel_abilities.Add( e.Current.Key->ability );
		}
	}

	query->rel_abilities.TrimExcess();

	GetRelevantDecorations( query );
	query->rel_decorations.TrimExcess();

	GetRelevantCharms( query );
	query->rel_charms.TrimExcess();

	//get relevant armors
	for( int i = 0; i < int( Armor::ArmorType::NumArmorTypes ); ++i )
	{
		GetRelevantArmors( query, query->rel_armor[ i ], Armor::static_armors[ i ], query->inf_armor[ i ], false );
		
		query->rel_armor[ i ]->TrimExcess();
	}
	query->rel_armor.TrimExcess();
}

void SkillTag::Load( System::String^ filename )
{
	tags.Clear();
	tags.Add( gcnew SkillTag( L"All" ) );
	tags.Add( gcnew SkillTag( L"Misc" ) );
	tags.Add( gcnew SkillTag( L"Related" ) );
	IO::StreamReader fin( filename );
	while( !fin.EndOfStream )
	{
		String^ tag = fin.ReadLine();
		if( tag != L"" )
			tags.Add( gcnew SkillTag( tag ) );
	}
}

void SkillTag::LoadLanguage( String^ filename )
{
	IO::StreamReader fin( filename );
	for( int i = 0; i < tags.Count; )
	{
		String^ line = fin.ReadLine();
		if( line == L"" || line[ 0 ] == L'#' )
			continue;
		tags[ i ]->name = line;
		i++;
	}
}

SkillTag::SkillTag( System::String^ s )
	: name( s )
{

}

#include "stdafx.h"
#include "Armor.h"
#include "Skill.h"
#include "Solution.h"
#include <fstream>

using namespace System;

String^ GetFirst( String^ s, int len )
{
	return s->Length < len ? s : s->Substring( 0, len );
}

String^ GetLast( String^ s, int len )
{
	return s->Length < len ? s : s->Substring( s->Length - len, len );
}

bool ArmorsHaveSameComponents( Armor^ a, Armor^ b )
{
	if( a->components.Count != b->components.Count )
		return false;

	for( int i = 0; i < a->components.Count; ++i )
	{
		if( a->components[ i ]->amount != b->components[ i ]->amount ||
			a->components[ i ]->material != b->components[ i ]->material )
			return false;
	}
	return true;
}

unsigned GetFamily( Armor^ armor )
{
	String^ tag = GetFirst( armor->name, 5 );

	if( armor->name->StartsWith( L"Rath " ) )
		tag = GetFirst( armor->name, 9 );

	if( armor->full_set )
		tag = Convert::ToString( armor->rarity ) + tag;

	if( Family::map.ContainsKey( tag ) )
	{
		const unsigned res = Family::map[ tag ];
		Family::families[ res ]->Add( armor );
		return res;
	}
	Family::map.Add( tag, Family::count );
	List_t< Armor^ >^ list = gcnew List_t< Armor^ >;
	list->Add( armor );
	Family::families.Add( list );
	return Family::count++;
}

int ConvertNum( String^ s )
{
	if( s == L"" )
		return 0;
	else
		return Convert::ToInt32( s );
}

void Armor::Load( String^ filename, ArmorType armor_type )
{
	List_t< Armor^ >^ armors = static_armors[ (int)armor_type ];
	armors->Clear();
	armors->Capacity = 170;

	unsigned num_male = 0, num_female = 0;

//#define DUMP_IMAGE_NUMBERS

#ifdef DUMP_IMAGE_NUMBERS
	IO::StreamWriter^ fout = gcnew IO::StreamWriter( filename + L".images.txt" );
#endif
	
	IO::StreamReader fin( filename );
	String^ temp;
	while( !fin.EndOfStream )
	{
		temp = fin.ReadLine();
		if( temp == L"" ) continue;
		else if( temp[ 0 ] == L'#' ) continue;
		List_t< String^ > split;
		Utility::SplitString( %split, temp, L',' );
		
		//#Name,Gender,Rarity,Acquire,Slots,Min Def,Max Def,Aug Def,Fire,Water,Thunder,Ice,Dragon,Skill 1,Points 1,Skill 2,Points 2,Skill 3,Points 3
		enum Format
		{
			Name = 0,
			Gender,
			Rarity,
			Acquire,
			Slots,
			MinDef,
			MaxDef,
			AugDef,
			FireRes,
			WaterRes,
			ThunderRes,
			IceRes,
			DragonRes,
			Skill1,
			Points1,
			Skill2,
			Points2,
			Skill3,
			Points3,
			Material1,
			Num1,
			Material2,
			Num2,
			Material3,
			Num3,
			Material4,
			Num4,
			NumInfo,
			NumSkills = 3,
			NumMaterials = 4
		};

		if( split.Count < NumInfo || temp->Length < NumInfo )
			continue;
		
		Armor^ armor = gcnew Armor();
		
		armor->name = split[ Format::Name ];

		if( armor->name->EndsWith( L"¦Â" ) )
		{
			Assert( armors->Count > 0, L"First armor shouldn't be beta" );
			armor->alpha_version = armors[ armors->Count - 1 ];
			armor->alpha_version->has_beta = true;
			Assert( armor->alpha_version->name->Length == armor->name->Length && armor->alpha_version->name->Substring( 0, armor->alpha_version->name->Length - 1 ) == armor->name->Substring( 0, armor->name->Length - 1 ), L"Armor names don't match" );
		}
		
		armor->gender = split[ Format::Gender ] == L"M" ? Gender::MALE : split[ Format::Gender ] == L"F" ? Gender::FEMALE : Gender::BOTH_GENDERS;
		armor->full_set = split[ Format::Gender ] == L"Full";
		armor->rarity = Convert::ToInt32( split[ 2 ] );
		armor->arena = split[ Format::Gender ] == L"Arena";
		armor->dlc_disabled = false;
		armor->has_free_element = false;
		armor->has_nonelemental_boost = false;
		armor->has_beta = false;

		armor->slots = gcnew array< unsigned >( 4 );
		String^ slots = split[ Format::Slots ];
		for( int i = 0; i < slots->Length; ++i )
		{
			if( slots[ i ] == L'1' )
				armor->slots[1]++;
			else if( slots[ i ] == L'2' )
				armor->slots[2]++;
			else if( slots[ i ] == L'3' )
				armor->slots[3]++;
		}
		armor->total_slots = armor->slots[1] + armor->slots[2] + armor->slots[3];
		armor->highest_slot_level = armor->slots[3] ? 3 : armor->slots[2] ? 2 : armor->slots[1] ? 1 : 0;
		armor->total_slot_level = armor->slots[ 1 ] + armor->slots[ 2 ] * 2 + armor->slots[ 3 ] * 3;

		if( armor->highest_slot_level > 0 )
		{
			armor->slot_product = 1;
			for( int i = 2; i <= 3; ++i )
			{
				if( armor->slots[ i ] > 0 )
					armor->slot_product *= armor->slots[ i ] * i;
			}
		}
		else armor->slot_product = 0;

		armor->hr = ConvertNum( split[ Format::Acquire ] );
		
		armor->defence = ConvertNum( split[ Format::MinDef ] );
		armor->max_defence = ConvertNum( split[ Format::MaxDef ] );
		armor->aug_defence = ConvertNum( split[ Format::AugDef ] );
		armor->fire_res = ConvertNum( split[ Format::FireRes ] );
		armor->water_res = ConvertNum( split[ Format::WaterRes ] );
		armor->thunder_res = ConvertNum( split[ Format::ThunderRes ] );
		armor->ice_res = ConvertNum( split[ Format::IceRes ] );
		armor->dragon_res = ConvertNum( split[ Format::DragonRes ] );
		armor->difficulty = 0;
		
		for( unsigned i = 0; i < Format::NumMaterials; ++i )
		{
			String^% name = split[ i * 2 + Format::Material1 ];
			
			if( name != L"" )
			{
				MaterialComponent^ comp = gcnew MaterialComponent;
				comp->material = Material::FindMaterial( name );
				if( comp->material )
				{
					comp->amount = Convert::ToInt32( split[ i * 2 + Format::Num1 ] );
					armor->components.Add( comp );

					armor->arena |= comp->material->arena;
					armor->difficulty += comp->material->difficulty * comp->amount;
				}
			}
		}
		
		for( unsigned i = 0; i < Format::NumSkills; ++i )
		{
			const int index = Format::Skill1 + i * 2;
			if( split[ index ] != L"" )
			{
				AbilityPair^ p = gcnew AbilityPair( Ability::FindAbility( split[ index ] ), 0 );
				Assert( p->ability, armor->name + L": No such ability found: " + split[ index ] );
				if( split[ index + 1 ] != L"" )
					p->amount = Convert::ToInt32( split[ index + 1 ] );
				armor->abilities.Add( p );

				if( p->ability->set_ability )
				{
					Map_t< unsigned, Skill^ >::Enumerator e = p->ability->skills.GetEnumerator();
					while( e.MoveNext() )
					{
						Ability^ ab = Skill::set_skill_map[ e.Current.Value ]->ability;
						ab->unlocked_by_set_bonus = true;
						ab->set_bonus_unlocked_at_hr = Math::Max( ab->set_bonus_unlocked_at_hr, armor->hr );
					}
				}
			}
		}

		if( armor->gender == Gender::MALE )
		{
			++num_male;

			armor->male_image = num_male;
			armor->female_image = 0;
		}
		else if( armor->gender == Gender::FEMALE )
		{
			++num_female;

			armor->female_image = num_female;
			armor->male_image = 0;
		}
		else
		{
			++num_male;
			++num_female;

			armor->male_image = num_male;
			armor->female_image = num_female;
		}

#ifdef DUMP_IMAGE_NUMBERS
		if( fout && !IsLevelledDeviantArmor( armor ) )
			fout->WriteLine( armor->jap_name + "," + Convert::ToString( armor->male_image ) + "," + Convert::ToString( armor->female_image ) );
#endif

		armor->family = GetFamily( armor );
		if( armor->full_set )
			AddFullSetArmor( armor, armor_type );

		armor->has_free_element = armor->GetSkillAt( SpecificAbility::free_element ) > 0;
		armor->has_nonelemental_boost = armor->GetSkillAt( SpecificAbility::nonelemental_boost ) > 0;


		armor->index = armors->Count;
		armors->Add( armor );
	}

#ifdef DUMP_IMAGE_NUMBERS
	if( fout )
		fout->Close();
#endif

	fin.Close();
	armors->TrimExcess();
}

void Armor::AddFullSetArmor( Armor^ armor, ArmorType armor_type )
{
	for each( array< Armor^ >^ set in static_full_sets )
	{
		if( set && set->Length > 0 && set[ 0 ] && set[ 0 ]->family == armor->family )
		{
			set[ (int)armor_type ] = armor;
			return;
		}
	}
	array< Armor^ >^ new_set = gcnew array< Armor^ >( (int)Armor::ArmorType::NumArmorTypes );
	new_set[ (int)armor_type ] = armor;
	static_full_sets.Add( new_set );
}

Armor^ FindArmor( String^ name )
{
	for each( List_t< Armor^ >^ list in Armor::static_armors )
	{
		for each( Armor^ armor in list )
		{
			if( armor->name == name )
				return armor;
		}
	}
	return nullptr;
}

void Armor::LoadDLC( System::String^ filename )
{
	IO::StreamReader fin( filename );
	String^ temp;
	while( !fin.EndOfStream )
	{
		temp = fin.ReadLine();
		if( temp == L"" ) continue;
		else if( temp[ 0 ] == L'#' ) continue;
		List_t< String^ > split;
		Utility::SplitString( %split, temp, L',' );
		if( split.Count == 0 )
			continue;

		ArmorDLC^ dlc = gcnew ArmorDLC();
		dlc->name = split[ 0 ];
		for each( String^ name in split )
		{
			if( Material^ mat = Material::FindMaterial( name ) )
			{
				for each( List_t< Armor^ >^ armor_type in Armor::static_armors )
				{
					for each( Armor^ armor in armor_type )
					{
						if( armor->components.Count > 0 && armor->components[ 0 ]->material == mat )
							dlc->armors.Add( armor );
					}
				}
			}
			else
			{
				Assert( false, L"Could not find DLC armor " + name );
			}
		}
		
		Armor::static_dlc.Add( dlc );
	}

	/*IO::StreamWriter fout( L"dlc_dump.txt" );
	for each( ArmorDLC^ dlc in Armor::static_dlc )
	{
		fout.Write( dlc->name );
		for each ( Armor^ armor in dlc->armors )
		{
			fout.Write( L"," + armor->name );
		}
		fout.WriteLine();
	}*/
}

void Armor::LoadLanguageDLC( System::String^ filename )
{
	IO::StreamReader fin( filename );

	for( int i = 0; i < static_dlc.Count; ++i )
	{
		ArmorDLC^ dlc = static_dlc[ i ];
		String^ line = fin.ReadLine();
		if( !line )
		{
			array< wchar_t >^ slashes = { L'/', L'\\' };
			const int slash = filename->LastIndexOfAny( slashes );
			Windows::Forms::MessageBox::Show( L"Unexpected end of file: not enough lines of text?", filename->Substring( slash + 1 ) );
			break;
		}
		line = line->Trim();

		if( line == L"" || line[ 0 ] == L'#' )
		{
			i--;
			continue;
		}

		dlc->name = line;
	}
}

unsigned GetTier( Armor^ a )
{
	return a->rarity / 5;
}

unsigned GetTier( const unsigned hr )
{
	return hr > 5 ? 1 : 0;
}

bool Armor::MeetsRequirements( Query^ query, const bool allow_full )
{
	if( !query->include_arena && this->arena ||
		!query->allow_lower_tier && GetTier( this ) < GetTier( query->hr ) ||
		gender != Gender::BOTH_GENDERS && gender != query->gender ||
		this->dlc_disabled ||
		!allow_full && this->full_set ||
		query->hr < this->hr )
		return false;

	return true;
}

bool Armor::MatchesQuery( Query^ query, const bool allow_full )
{
	if( !MeetsRequirements( query, allow_full ) )
		return false;

	//check for relevant skills
	total_relevant_skill_points = 0;
	for each( AbilityPair^ ap in abilities )
	{
		if( ap->ability->relevant )
			total_relevant_skill_points += ap->amount;
	}
	
	return total_relevant_skill_points > 0 || total_slots > 0;
}

bool Armor::IsBetterAtNonSkills( Armor^ other )
{
	return this->defence != other->defence ? this->defence > other->defence :
		this->aug_defence != other->aug_defence ? this->aug_defence > other->aug_defence :
		this->fire_res != other->fire_res ? this->fire_res > other->fire_res :
		this->dragon_res != other->dragon_res ? this->dragon_res > other->dragon_res :
		this->ice_res != other->ice_res ? this->ice_res > other->ice_res :
		this->thunder_res != other->thunder_res ? this->thunder_res > other->thunder_res :
		this->water_res != other->water_res ? this->water_res > other->water_res :
		this->difficulty < other->difficulty;
}

bool Armor::IsBetterThan( Armor^ other, List_t< Ability^ >^ rel_abilities )
{
	if( this->full_set || other->full_set )
		return false;

	if( this->total_slots > other->total_slots ||
		this->highest_slot_level > other->highest_slot_level ||
		this->total_slot_level > other->total_slot_level ||
		this->slot_product > other->slot_product ||
		this->total_slots >= other->total_slots && highest_slot_level >= other->highest_slot_level && total_slot_level >= other->total_slot_level && slot_product >= other->slot_product && total_relevant_skill_points > 0 && other->total_relevant_skill_points == 0 )
		return true;

	if( SpecificAbility::nonelemental_boost->relevant && other->has_free_element )
		return true;

	bool somewhat_worse = total_slots < other->total_slots || highest_slot_level < other->highest_slot_level || total_slot_level < other->total_slot_level || slot_product < other->slot_product;
	
	if( !somewhat_worse && this->total_relevant_skill_points == 0 && other->total_relevant_skill_points == 0 )
	{
		return this->IsBetterAtNonSkills( other );
	}

	int total_relevant_points = 0, other_total_relevant_points = 0;

	for each( Ability^ ability in rel_abilities )
	{
		int my_skill_at = this->GetSkillAt( ability );
		int other_skill_at = other->GetSkillAt( ability );

		total_relevant_points += my_skill_at;
		other_total_relevant_points += other_skill_at;

		if( my_skill_at > other_skill_at )
			return true;
		else if( my_skill_at < other_skill_at )
			somewhat_worse = true;
	}

	return total_relevant_points > other_total_relevant_points || ( !somewhat_worse && this->IsBetterAtNonSkills( other ) );
}

bool Armor::IsStrictlyBetterThan( Armor^ other )
{
	if( this->full_set || other->full_set )
		return false;

	if( this->total_slots < other->total_slots ||
		this->highest_slot_level < other->highest_slot_level ||
		this->total_slot_level < other->total_slot_level ||
		this->slot_product < other->slot_product )
		return false;

	if( SpecificAbility::nonelemental_boost->relevant && this->has_free_element && !other->has_free_element )
		return false;

	int total_relevant_points = 0, other_total_relevant_points = 0;

	for each( AbilityPair^ ap in abilities )
	{
		int my_skill_at = this->GetSkillAt( ap->ability );
		int other_skill_at = other->GetSkillAt( ap->ability );

		if( my_skill_at < other_skill_at )
			return false;
	}

	for each( AbilityPair^ ap in other->abilities )
	{
		int my_skill_at = this->GetSkillAt( ap->ability );
		int other_skill_at = other->GetSkillAt( ap->ability );

		if( my_skill_at < other_skill_at )
			return false;
	}

	return true;
}

unsigned Armor::GetSkillAt( Ability^ ability )
{
	for each( AbilityPair^ apair in abilities )
	{
		if( apair->ability == ability )
			return apair->amount;
	}
	return 0;
}

void Armor::LoadLanguage( String^ filename, ArmorType armor_type )
{
	List_t< Armor^ >^ armors = static_armors[ (int)armor_type ];
	IO::StreamReader fin( filename );
	for( int i = 0; i < armors->Count; ++i )
	{
		Armor^ armor = armors[ i ];
		String^ line = fin.ReadLine();
		if( !line )
		{
			array< wchar_t >^ slashes = { L'/', L'\\' };
			const int slash = filename->LastIndexOfAny( slashes );
			Windows::Forms::MessageBox::Show( L"Unexpected end of file: not enough lines of text?", filename->Substring( slash + 1 ) );
			break;
		}
		line = line->Trim();

		if( line == L"" || line[ 0 ] == L'#' )
		{
			i--;
			continue;
		}

		armor->name = line;
	}
}

bool Armor::ContainsAnyAbility( List_t< Ability^ >% to_search )
{
	for each( AbilityPair^ ap in abilities )
	{
		if( ap->amount > 0 && Utility::Contains( %to_search, ap->ability ) )
			return true;
	}
	return false;
}

bool Charm::MatchesQuery( Query^ query )
{
	//check requirements
	if( query->hr < this->hr ||
		!query->allow_fair_wind && this->fair_wind )
		return false;

	//check for relevant skills
	for( int i = 0; i < abilities.Count; ++i )
	{
		if( Utility::Contains( %query->rel_abilities, abilities[ i ]->ability ) )
		{
			return true;
		}
	}

	return false;
}

unsigned Charm::GetSkillPointsFor( Ability^ a )
{
	if( abilities.Count > 0 )
	{
		if( abilities[0]->ability == a )
			return abilities[0]->amount;
		else if( abilities.Count > 1 && abilities[1]->ability == a )
			return abilities[1]->amount;
	}
	return 0;
}

bool Charm::BetterThan( Charm^ other )
{
	for each( AbilityPair^ ap in abilities )
	{
		if( ap->amount > other->GetSkillPointsFor( ap->ability ) )
			return true;
	}
	return false;
}

bool Charm::StrictlyBetterThan( Charm^ other )
{
	bool better = false;

	List_t< Ability^ > rel_abilities;
	for each( AbilityPair^ apair in other->abilities )
	{
		if( apair->ability->relevant && !Utility::Contains( %rel_abilities, apair->ability ) )
			rel_abilities.Add( apair->ability );
	}
	for each( AbilityPair^ apair in abilities )
	{
		if( apair->ability->relevant && !Utility::Contains( %rel_abilities, apair->ability ) )
			rel_abilities.Add( apair->ability );
	}

	for each( Ability^ a in rel_abilities )
	{
		const int other_amount = other->GetSkillPointsFor( a );
		const int my_amount = GetSkillPointsFor( a );

		if( my_amount > other_amount )
		{
			better = true;
		}
		else if( my_amount < other_amount )
		{
			return false;
		}
	}
	return better;
}

bool EqualAbilities( List_t< AbilityPair^ >% l1, List_t< AbilityPair^ >% l2 )
{
	Assert( l1.Count == l2.Count, L"Ability lists not equal size" );
	for( int i = 0; i < l1.Count; ++i )
		if( l1[ i ]->ability != l2[ i ]->ability ||
			l1[ i ]->amount != l2[ i ]->amount )
			return false;
	return true;
}

void Charm::AddToOptimalList( List_t< Charm^ >% lst, Charm^ new_charm )
{
	for( int i = 0; i < lst.Count; ++i )
	{
		Charm^ curr_charm = lst[ i ];
		if( new_charm->StrictlyBetterThan( curr_charm ) )
			lst.RemoveAt( i-- );
		else if( curr_charm->StrictlyBetterThan( new_charm ) )
			return;
	}
	lst.Add( new_charm );
}

#define TryCompare( X ) if( a->X > b->X ) return -1;\
						else if( b->X > a->X ) return 1

#define TryCompare2( X ) if( a->X > b->X ) return 1;\
						 else if( b->X > a->X ) return -1

int CompareCharms1( Charm^ a, Charm^ b )
{
	if( a->abilities.Count > 0 )
	{
		if( b->abilities.Count == 0 )
			return 1;
		TryCompare2( abilities[ 0 ]->ability->static_index );
		TryCompare( abilities[ 0 ]->amount );
		if( a->abilities.Count > 1 )
		{
			if( b->abilities.Count < 2 )
				return 1;
			TryCompare2( abilities[ 1 ]->ability->static_index );
			TryCompare( abilities[ 1 ]->amount );
		}
		else if( b->abilities.Count > 1 )
			return -1;
	}
	else if( b->abilities.Count > 0 )
		return -1;

	return 0;
}

int CompareCharms1Alphabetically( Charm^ a, Charm^ b )
{
	if( a->abilities.Count > 0 )
	{
		if( b->abilities.Count == 0 )
			return 1;
		TryCompare2( abilities[ 0 ]->ability->order );
		TryCompare( abilities[ 0 ]->amount );
		if( a->abilities.Count > 1 )
		{
			if( b->abilities.Count < 2 )
				return 1;
			TryCompare2( abilities[ 1 ]->ability->order );
			TryCompare( abilities[ 1 ]->amount );
		}
		else if( b->abilities.Count > 1 )
			return -1;
	}
	else if( b->abilities.Count > 0 )
		return -1;

	return 0;
}

int CompareCharms2( Charm^ a, Charm^ b )
{
	if( a->abilities.Count < 2 )
	{
		if( b->abilities.Count < 2 )
			return CompareCharms1( a, b );
		else
			return 1;
	}
	else
	{
		if( b->abilities.Count < 2 )
			return -1;
		else
		{
			TryCompare2( abilities[ 1 ]->ability->static_index );
			TryCompare( abilities[ 1 ]->amount );
			return 0;
		}
	}
}

int CompareCharms2Alphabetically( Charm^ a, Charm^ b )
{
	if( a->abilities.Count < 2 )
	{
		if( b->abilities.Count < 2 )
			return CompareCharms1Alphabetically( a, b );
		else
			return 1;
	}
	else
	{
		if( b->abilities.Count < 2 )
			return -1;
		else
		{
			TryCompare2( abilities[ 1 ]->ability->order );
			TryCompare( abilities[ 1 ]->amount );
			return 0;
		}
	}
}
#undef TryCompare
#undef TryCompare2

Charm^ Charm::FindCharm( System::String^ name )
{
	if( static_charm_map.ContainsKey( name ) )
		return static_charm_map[ name ];
	else return nullptr;
}

Charm^ Charm::FindExactCharm( Ability^ ability, const unsigned points )
{
	if( !static_charm_ability_map.ContainsKey( ability ) )
		return nullptr;

	List_t< Charm^ >^ possible = static_charm_ability_map[ ability ];
	for each( Charm^ charm in possible )
	{
		if( charm->abilities.Count == 1 && charm->abilities[ 0 ]->ability == ability && charm->abilities[ 0 ]->amount == points )
			return charm;
	}
	return nullptr;
}

void Charm::Load( System::String^ filename )
{
	//Name,Skill1,Points1,Skill2,Points2,Acquire,Material1,Points1,Materi2,Points2,Material3,Points3,Material4,Points4
	static_charm_map.Clear();
	static_charms.Clear();
	static_charms.Capacity = 256;
	IO::StreamReader fin( filename );
	while( !fin.EndOfStream )
	{
		String^ line = fin.ReadLine();
		if( line == L"" || line[ 0 ] == L'#' )
			continue;
		List_t< String^ > split;
		Utility::SplitString( %split, line, L',' );
		enum Format
		{
			Name = 0,
			Skill1,
			Points1,
			Skill2,
			Points2,
			Acquire,
			Material1,
			Required1,
			Material2,
			Required2,
			Material3,
			Required3,
			Material4,
			Required4,
			NumInfo,
			NumMaterials = 4
		};
		if( split.Count < NumInfo )
			continue;

		Charm^ charm = gcnew Charm;
		charm->name = split[ Name ];
		charm->fair_wind = charm->name == L"Fair Wind Charm";

		if( split[ Acquire ] == L"" )
			charm->hr = 0;
		else
			charm->hr = Convert::ToUInt32( split[ Acquire ] );
		charm->difficulty = 0;

		Ability^ ability1 = Ability::FindAbility( split[ Skill1 ] );
		Assert( ability1, L"Cannot find charm ability: " + split[ Skill1 ] );
		charm->abilities.Add( gcnew AbilityPair( ability1, Convert::ToUInt32( split[ Points1 ] ) ) );

		if( !static_charm_ability_map.ContainsKey( ability1 ) )
			static_charm_ability_map.Add( ability1, gcnew List_t< Charm^ >() );
		static_charm_ability_map[ ability1 ]->Add( charm );

		if( split[ Skill2 ] != L"" )
		{
			Ability^ ability2 = Ability::FindAbility( split[ Skill2 ] );
			Assert( ability2, L"Cannot find charm ability: " + split[ Skill2 ] );
			charm->abilities.Add( gcnew AbilityPair( ability2, Convert::ToUInt32( split[ Points2 ] ) ) );

			if( !static_charm_ability_map.ContainsKey( ability2 ) )
				static_charm_ability_map.Add( ability2, gcnew List_t< Charm^ >() );
			static_charm_ability_map[ ability2 ]->Add( charm );
		}

		for( unsigned i = 0; i < NumMaterials; ++i )
		{
			String^% name = split[ Material1 + i * 2 ];
			if( name != L"" )
			{
				MaterialComponent^ mc = gcnew MaterialComponent;
				mc->amount = Convert::ToUInt32( split[ Required1 + i * 2 ] );
				mc->material = Material::FindMaterial( name );
				if( mc->material )
				{
					charm->components.Add( mc );
					charm->difficulty += mc->material->difficulty;
				}
			}
		}

		charm->index = static_charms.Count;
		static_charms.Add( charm );
		static_charm_map.Add( charm->name, charm );
	}
	static_charms.TrimExcess();
}

void Charm::LoadLanguage( System::String^ filename )
{
	IO::StreamReader fin( filename );
	for( int i = 0; i < static_charms.Count; ++i )
	{
		Charm^ charm = static_charms[ i ];
		String^ line = fin.ReadLine();
		if( !line )
		{
			array< wchar_t >^ slashes = { L'/', L'\\' };
			const int slash = filename->LastIndexOfAny( slashes );
			Windows::Forms::MessageBox::Show( L"Unexpected end of file: not enough lines of text?", filename->Substring( slash + 1 ) );
			break;
		}
		line = line->Trim();

		if( line == L"" || line[ 0 ] == L'#' )
			continue;

		charm->name = line;
	}
}
#pragma once

enum class Gender { MALE = 4, FEMALE, BOTH_GENDERS };

#define List_t System::Collections::Generic::List
#define Map_t System::Collections::Generic::Dictionary

//#define CREATE_MATERIALS

ref struct Armor;
ref struct Ability;
ref struct Decoration;
ref struct AbilityPair;
ref struct Skill;
ref struct Charm;

ref struct AdvancedSearchOptions abstract
{
	bool force_enable, force_disable, default_piece;
	unsigned adv_index;
};

namespace Utility
{

	template< class T >
	bool Contains( List_t< T^ >^ cont, const T^ val )
	{
		for( int i = 0; i < cont->Count; ++i )
		{
			if( cont[ i ] == val )
				return true;
		}
		return false;
	}

	bool Contains( List_t< Skill^ >^ cont, const Ability^ val );

	template< class T >
	bool Contains( array< T^ >^ cont, const T^ val )
	{
		for( int i = 0; i < cont->Length; ++i )
		{
			if( cont[ i ] == val )
				return true;
		}
		return false;
	}

	template< class T >
	int GetIndexOf( List_t< T^ >^ cont, const T^ val )
	{
		for( int i = 0; i < cont->Count; ++i )
		{
			if( cont[ i ] == val )
				return i;
		}
		return -1;
	}

	void AddAbilitiesToMap( List_t< AbilityPair^ >% _list, Map_t< Ability^, unsigned >% _map, const int mult = 1 );

	unsigned CountChars( System::String^ str, const wchar_t c );

	bool ContainsString( List_t< System::String^ >% vec, System::String^ item );

	void SplitString( List_t< System::String^ >^ vec, System::String^ str, const wchar_t c );

	void FindLevelReqs( unsigned% available, unsigned% required, System::String^ input );

	System::String^ RemoveQuotes( System::String^ in_out );

	inline unsigned Min( const unsigned a, const unsigned b ) { return a > b ? b : a; }

	void UpdateContextMenu( System::Windows::Forms::ContextMenuStrip^ strip, Decoration^ decoration );
	void UpdateContextMenu( System::Windows::Forms::ContextMenuStrip^ strip, Armor^ armor, const bool use_aug_defence );
	void UpdateContextMenu( System::Windows::Forms::ContextMenuStrip^ strip, Ability^ ability );
	void UpdateContextMenu( System::Windows::Forms::ContextMenuStrip^ strip, Charm^ charm );
}

//bool CanDisplayString( System::String^ text );

void myassert( const bool val, System::String^ message );
void myassert( const bool val, System::String^ message, System::String^ suffix_copy );

#ifdef _DEBUG
#define Assert( CONDITION, MESSAGE ) myassert( !!(CONDITION), MESSAGE )
#define AssertCopy( CONDITION, MESSAGE, COPY_TEXT_TO_CLIPBOARD ) myassert( !!(CONDITION), MESSAGE, COPY_TEXT_TO_CLIPBOARD )
#else
#define Assert( X, Y )
#define AssertCopy( X, Y, Z )
#endif

inline int Min( const int a, const int b )
{
	return a > b ? b : a;
}

inline int Clamp( const int val, const int min, const int max )
{
	return System::Math::Max( System::Math::Min( val, max ), min );
}

ref struct Material
{
	System::String^ name;
	bool event_only, arena;
	unsigned difficulty;

	static List_t< Material^ > static_materials;
	static Material^ FindMaterial( System::String^ name );
	static void LoadMaterials( System::String^ filename );
	static void LoadLanguage( System::String^ filename );
	static Map_t< System::String^, Material^ > static_material_map;
};

ref struct MaterialComponent
{
	Material^ material;
	unsigned amount;
};

ref class SuspendUpdate
{
private:
	const static int WM_SETREDRAW = 0x000B;

public:
	static void Suspend( System::Windows::Forms::Control^ c )
	{
		using namespace System;
		using namespace System::Windows::Forms;

		Message^ m = Message::Create( c->Handle, WM_SETREDRAW, IntPtr::Zero, IntPtr::Zero );
		NativeWindow^ w = NativeWindow::FromHandle( c->Handle );
		w->DefWndProc( *m );

		for each( Control^ child in c->Controls )
		{
			Suspend( child );
		}
	}

	static void Resume( System::Windows::Forms::Control^ c )
	{
		using namespace System;
		using namespace System::Windows::Forms;

		Message^ m = Message::Create( c->Handle, WM_SETREDRAW, IntPtr( 1 ), IntPtr::Zero );
		NativeWindow^ w = NativeWindow::FromHandle( c->Handle );
		w->DefWndProc( *m );

		c->Invalidate();

		for each( Control^ child in c->Controls )
		{
			Resume( child );
		}
	}
};

ref struct StringTable
{
	static array< System::String^ >^ text;
	static void LoadLanguage( System::String^ lang );
	enum class StringIndex
	{ 
		TranslationCredit = 0,
		File,
		Options,
		Language,
		Help,
		LoadData,
		SaveData,
		Update,
		Exit,
		ClearSettings,
		AllowArena,
		AllowEventArmor,
		AllowFairWind,
		AllowLowerTierArmor,
		AlwaysSearchAlpha,
		EnableAll,
		DisableAll,
		SortSkillsAlphabetically,
		Donate,
		SkillHelp,
		SkillHelpForm,
		CheckForUpdates,
		About,
		AboutForm,
		MyDecorationsButton,
		MyDecorationsForm,
		Import,
		QuickSearch,
		AdvancedSearchButton,
		AdvancedSearchForm,
		Cancel,
		Default,
		AdvancedNone,
		SortByNone,
		Search,
		SelectNone,
		SelectBest,
		AddNewDecoration,
		ResetDecoration,
		DeleteDecoration,
		DeleteAllDecorations,
		MoveUp,
		MoveDown,
		Close,
		FindNext,
		HR,
		WeaponSlots,
		Gender,
		Male,
		Female,
		NoExtraSkillFiltering,
		SortFilterResults,
		DragonRes,
		FireRes,
		IceRes,
		ThunderRes,
		WaterRes,
		BaseDefence,
		MaxDefence,
		AugDefence,
		Difficulty,
		Rarity,
		SortSlotsSpare,
		SortFamily,
		SortExtraSkills,
		Decorations,
		UseNoDecorations,
		UseOnlyMyDecorations,
		UseAllDecorations,
		Skill,
		Skills,
		SkillFilters,
		SelectArmor,
		Slots,
		SlotLevel,
		NoneBrackets,
		SlotsSpare,
		SolutionsFound,
		ShowingFirstSolutionsOnly,
		MyDecorationList,
		FileCorrupted,
		Version,
		Find,
		Error,
		To,
		Defence,
		Results,
		DeleteAllDecorationsMessage,
		ASSSettings,
		MaxResults,
		PrintDecoNames,
		PrintMaterials,
		Zoom,
		DefEleAbbrev,
		JewelFormat,
		ItemType,
		SkillPossible,
		Show,
		Description,
		SkillsHelp,
		SkillTrees,
		ArmorsWithSkill,
		ArmorsWithSetSkill,
		HRNum,
		HRAll,
		AreYouSure,
		SkillLevel,
		SlotString,
		NumDecorations,
		ExcludeSkill,
		Completed,
		SlotLevelPrefix,
		TipResults,
		TipDecorations,
		TipFilter,
		TipSort,
		TipQuestLevel,
		TipWeaponSlotLevel,
		TipRemoveSkills,
		TipAddSkills,
		NumStrings
	};
};

void SuspendDrawing( System::Windows::Forms::Control^ control );
void ResumeDrawing( System::Windows::Forms::Control^ control );

bool ConvertInt( int% i, System::String^ str, StringTable::StringIndex err );
bool ConvertUInt( unsigned% i, System::String^ str, StringTable::StringIndex err );

System::String^ StripAmpersands( System::String^ input );

#define StaticString( X ) StringTable::text[ (int)StringTable::StringIndex::X ]
#define StartString( X ) StaticString( X ) + L" "
#define BasicString( X ) StripAmpersands( StaticString( X ) )
#define ColonString( X ) StaticString( X ) + L": "

#define FormatString1( ID, V ) StaticString( ID )->Replace( L"%1", V )
#define FormatString2( ID, V1, V2 ) StaticString( ID )->Replace( L"%1", Convert::ToString( V1 ) )->Replace( L"%2", Convert::ToString( V2 ) )
#define FormatString3( ID, V1, V2, V3 ) StaticString( ID )->Replace( L"%1", Convert::ToString( V1 ) )->Replace( L"%2", Convert::ToString( V2 ) )->Replace( L"%3", Convert::ToString( V3 ) )
#define FormatString7( ID, A, B, C, D, E, F, G ) StaticString( ID )->Replace( L"%1", Convert::ToString( A ) )->Replace( L"%2", Convert::ToString( B ) )->Replace( L"%3", Convert::ToString( C ) )->Replace( L"%4", Convert::ToString( D ) )->Replace( L"%5", Convert::ToString( E ) )->Replace( L"%6", Convert::ToString( F ) )->Replace( L"%7", Convert::ToString( G ) )
#define FormatStringN( ID, V ) StaticString( ID )->Replace( L"%1", Convert::ToString( V ) )
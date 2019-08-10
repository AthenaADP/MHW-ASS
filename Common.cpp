#include "stdafx.h"
#include "Common.h"
#include "Armor.h"
#include "Decoration.h"
#include "LoadedData.h"

using namespace System;
using namespace Windows::Forms;

#using <mscorlib.dll>
using namespace System::Runtime::InteropServices;

value class Win32
{
public:
	[DllImport( "user32.dll", SetLastError = true )]
	static int SendMessage( IntPtr hWnd, UInt32 Msg, Int32 wParam, Int32 lParam );

	/*[DllImport( "gdi32.dll", EntryPoint = "CreateDC", SetLastError = true )]
	static IntPtr CreateDC( const char* lpszDriver, const char* lpszDeviceName, const char* lpszOutput, IntPtr devMode );

	[DllImport( "gdi32.dll", ExactSpelling = true, SetLastError = true )]
	static bool DeleteDC( IntPtr hdc );

	[DllImport( "gdi32.dll" )]
	static IntPtr SelectObject( IntPtr hdc, IntPtr hgdiobj );

	[DllImport( "gdi32.dll", CharSet = CharSet::Unicode )]
	static int GetGlyphIndices( IntPtr hdc, [MarshalAs( UnmanagedType::LPWStr )] String^ lpstr, int c, [MarshalAs( UnmanagedType::LPArray )]array< Int16 >^, int fl );
	*/
};

/*bool CanDisplayString( IntPtr hdc, Drawing::FontFamily^ font_family, String^ text )
{
	try
	{
		Drawing::Font^ font = gcnew Drawing::Font( font_family, 12, Drawing::FontStyle::Regular, Drawing::GraphicsUnit::Point );

		if( Win32::SelectObject( hdc, font->ToHfont() ) == IntPtr::Zero )
			return false;
		
		int count = text->Length;
		array< Int16 >^ rtcode = gcnew array< Int16 >( count );
		int res = Win32::GetGlyphIndices( hdc, text, count, rtcode, 1 );
		if( res == -1 )
			return false;

		for each( Int16 code in rtcode )
		{
			if( code == -1 )
				return false;
		}
		return true;
	}
	catch( Exception^ e )
	{
		MessageBox::Show( e->ToString(), "Font error" );
	}
	return false;
}

bool CanDisplayString( String^ text )
{
	IntPtr hdc = IntPtr::Zero;
	try
	{
		hdc = Win32::CreateDC( "DISPLAY", nullptr, nullptr, IntPtr::Zero );
		if( hdc == IntPtr::Zero )
			return false;

		Drawing::Text::InstalledFontCollection^ fonts = gcnew Drawing::Text::InstalledFontCollection();
		List_t< String^ > valid;
		for each( Drawing::FontFamily^ family  in fonts->Families )
		{
			if( family->Name->Length > 0 && CanDisplayString( hdc, family, text ) )
				valid.Add( family->Name );
		}

		Win32::DeleteDC( hdc );
		return valid.Count > 0;
	}
	catch( Exception^ e )
	{
		MessageBox::Show( e->ToString(), "Font error" );
	}
	if( hdc != IntPtr::Zero )
		Win32::DeleteDC( hdc );
	return false;
}*/

namespace Utility
{
	void AddAbilitiesToMap( List_t< AbilityPair^ >% _list, Map_t< Ability^, unsigned >% _map, const int mult )
	{
		for each( AbilityPair^ ap in _list )
		{
			if( _map.ContainsKey( ap->ability ) )
				_map[ ap->ability ] += ap->amount * mult;
			else _map.Add( ap->ability, ap->amount * mult );
		}
	}

	String^ RemoveQuotes( String^ str )
	{
		if( str == L"" ) return str;
		str = str->Trim();
		if( str[ 0 ] == L'\"' ) str = str->Substring( 1 );
		if( str[ str->Length - 1 ] == L'\"' ) str = str->Substring( 0, str->Length - 1 );
		return str;
	}

	void SplitString( List_t< String^ >^ vec, String^ str, const wchar_t c )
	{
		str = str->Trim();
		int last_non_delim = 0;
		for( int i = 0; i < str->Length; ++i )
		{
			if( str[ i ] == c )
			{
				String^ substr = str->Substring( last_non_delim, i - last_non_delim );
				substr = RemoveQuotes( substr );
				vec->Add( substr );
				last_non_delim = i + 1;
			}
		}
		str = str->Substring( last_non_delim, str->Length - last_non_delim );
		str = RemoveQuotes( str );
		vec->Add( str );
	}

	bool ContainsString( List_t< String^ >% vec, String^ item )
	{
		for( int i = 0; i < vec.Count; ++i )
			if( vec[ i ] == item ) return true;
		return false;
	}

	void FindLevelReqs( unsigned% available, unsigned% required, String^ input )
	{
		if( input->Length > 0 && input[ 0 ] == L'\"' ) input = Utility::RemoveQuotes( input );
		const int exclamation_point = input->LastIndexOf( L'!' );
		if( exclamation_point == -1 )
		{
			required = 0;
			available = Convert::ToUInt32( input );
			return;
		}
		required = Convert::ToUInt32( input->Substring( 0, exclamation_point ) );
		if( exclamation_point < input->Length - 1 )
			available = Convert::ToUInt32( input->Substring( exclamation_point + 1 ) );
	}

	unsigned CountChars( String^ str, const wchar_t c )
	{
		unsigned total = 0;
		for( int i = 0; i < str->Length; ++i )
			if( str[ i ] == c ) ++total;
		return total;
	}

	void CopyTextToClipBoard( String^ data )
	{
		Clipboard::SetText( data );
	}

	void ContextMenuClicked( Object^ obj, EventArgs^ args )
	{
		ToolStripMenuItem^ item = safe_cast< ToolStripMenuItem^ >( obj );
		const int space = item->Text->IndexOf( L' ' );
		String^ material = item->Text->Substring( space + 1 );
		CopyTextToClipBoard( material );
	}

	void ContextMenuClicked2( System::Object^ obj, EventArgs^ args )
	{
		ToolStripMenuItem^ item = safe_cast< ToolStripMenuItem^ >( obj );
		CopyTextToClipBoard( item->Text );
	}

	void AddMenuItem( ToolStripItemCollection^ list, String^ name, const bool context2 )
	{
		ToolStripMenuItem^ item;
		if( context2 )
			item = gcnew ToolStripMenuItem( name, nullptr, gcnew EventHandler( ContextMenuClicked2 ) );
		else
			item = gcnew ToolStripMenuItem( name, nullptr, gcnew EventHandler( ContextMenuClicked ) );

		list->Add( item );
	}

	void GetAbilityMenu( Ability^ ability, ToolStripItemCollection^ strip )
	{
		AddMenuItem( strip, ability->name, true );
		strip->Add( L"-" );
		Ability::SkillMap_t::Enumerator e = ability->skills.GetEnumerator();
		while( e.MoveNext() )
		{
			Skill^ skill = e.Current.Value;
			if( skill->ability->set_ability )
				AddMenuItem( strip, e.Current.Key + L": " + Skill::set_skill_map[ skill ]->name, false );
			else
				AddMenuItem( strip, e.Current.Key + L": " + skill->name, false );
		}
	}

	void UpdateContextMenu( ContextMenuStrip^ strip, List_t< AbilityPair^ >% abilities )
	{
		for each( AbilityPair^ apair in abilities )
		{
			AddMenuItem( strip->Items, Convert::ToString( apair->amount ) + L" " + apair->ability->name, false );
			ToolStripMenuItem^ mi = safe_cast< ToolStripMenuItem^ >( strip->Items[ strip->Items->Count - 1 ] );
			GetAbilityMenu( apair->ability, mi->DropDownItems );
		}
	}

	void UpdateContextMenu( ContextMenuStrip^ strip, Ability^ ability )
	{
		GetAbilityMenu( ability, strip->Items );
	}

	void UpdateContextMenu( ContextMenuStrip^ strip, List_t< MaterialComponent^ >^ components )
	{
		strip->Items->Add( L"-" );
		for each( MaterialComponent^ part in components )
		{
			if( part->amount > 0 )
				AddMenuItem( strip->Items, Convert::ToString( part->amount ) + L"x " + part->material->name, false );
			else
				AddMenuItem( strip->Items, part->material->name, false );
		}
	}

	void UpdateContextMenu( ContextMenuStrip^ strip, Decoration^ decoration )
	{
		AddMenuItem( strip->Items, decoration->name, true );

		strip->Items->Add( L"-" );
		strip->Items->Add( ColonString( SlotLevel ) + Convert::ToString( decoration->slot_level ) );
		strip->Items->Add( L"-" );
		UpdateContextMenu( strip, decoration->abilities );
		UpdateContextMenu( strip, %decoration->components );
	}

	String^ GetSlotString( array< unsigned >^ slots )
	{
		List_t< unsigned > res;
		for( unsigned level = 4; level-- > 1; )
			for( unsigned i = 0; i < slots[ level ]; ++i )
				res.Add( level );

		while( res.Count < 3 )
			res.Add( 0 );

		return FormatString3( SlotString, res[ 0 ], res[ 1 ], res[ 2 ] );
	}

	void UpdateContextMenu( ContextMenuStrip^ strip, Armor^ armor, const bool use_aug_defence )
	{
		AddMenuItem( strip->Items, armor->name, true );

		strip->Items->Add( L"-" );
		strip->Items->Add( ColonString( Slots ) + GetSlotString( armor->slots ) );
		strip->Items->Add( L"-" );
		strip->Items->Add( ColonString( Rarity ) + ( armor->rarity == 11 ? L"X" : Convert::ToString( armor->rarity ) ) );
		strip->Items->Add( ColonString( Defence ) + FormatString2( To, armor->defence, use_aug_defence ? armor->aug_defence : armor->max_defence ) );
		strip->Items->Add( ColonString( FireRes ) + Convert::ToString( armor->fire_res ) );
		strip->Items->Add( ColonString( WaterRes ) + Convert::ToString( armor->water_res ) );
		strip->Items->Add( ColonString( ThunderRes ) + Convert::ToString( armor->thunder_res ) );
		strip->Items->Add( ColonString( IceRes ) + Convert::ToString( armor->ice_res ) );
		strip->Items->Add( ColonString( DragonRes ) + Convert::ToString( armor->dragon_res ) );
		strip->Items->Add( L"-" );
		UpdateContextMenu( strip, armor->abilities );
		UpdateContextMenu( strip, %armor->components );
	}

	void UpdateContextMenu( ContextMenuStrip^ strip, Charm^ charm )
	{
		AddMenuItem( strip->Items, charm->name, true );

		strip->Items->Add( L"-" );
		UpdateContextMenu( strip, charm->abilities );
		UpdateContextMenu( strip, %charm->components );
	}

	bool Contains( List_t< Skill^ >^ cont, const Ability^ val )
	{
		for( int i = 0; i < cont->Count; ++i )
		{
			if( cont[ i ]->ability == val )
				return true;
		}
		return false;
	}

}

void Material::LoadMaterials( String^ filename )
{
	static_materials.Clear();
	IO::StreamReader fin( filename );
	while( !fin.EndOfStream )
	{
		String^ line = fin.ReadLine();
		if( line == L"" ) continue;
		Material^ mat = gcnew Material;
		List_t< String^ > split;
		Utility::SplitString( %split, line, L',' );
		mat->name = split[ 0 ];

		mat->event_only = false;
		mat->difficulty = 0;
		for( int i = 1; i < split.Count; ++i )
		{
			if( split[i] == L"Arena" )
			{
				mat->arena = true;
			}
			else if( split[ i ] != L"" )
			{
				mat->difficulty = Convert::ToUInt32( split[ i ] );
			}
		}
		static_materials.Add( mat );
	}
	fin.Close();
	static_material_map.Clear();
	for each( Material^% mat in static_materials )
	{
		static_material_map.Add( mat->name, mat );
	}
}

Material^ Material::FindMaterial( String^ name )
{
	name = name->Trim();

	if( static_material_map.ContainsKey( name ) )
		return static_material_map[ name ];

#ifdef CREATE_MATERIALS
	Material^ mat = gcnew Material;
	mat->name = name;
	Material::static_materials.Add( mat );
	Material::static_material_map[ mat->name ] = mat;
	return mat;
#endif
	AssertCopy( false, L"Material not found: ", name );
	return nullptr;
}

void Material::LoadLanguage( String^ filename )
{
	static_material_map.Clear();
	IO::StreamReader fin( filename );
	for( int i = 0; i < static_materials.Count; )
	{
		String^ line = fin.ReadLine();
		if( line == L"" || line[ 0 ] == L'#' )
			continue;
		static_materials[ i ]->name = line;
		static_material_map.Add( line, static_materials[ i ] );
		i++;
	}
}

void StringTable::LoadLanguage( String^ lang )
{
	String^ dir = L"Data/Languages/" + lang + L"/";
	//load strings
	{
		text = gcnew array< String^ >( (int)StringTable::StringIndex::NumStrings );
		String^ string_table = dir + L"strings.txt";
		IO::StreamReader fin( IO::File::OpenRead( string_table ) );
		int i;
		for( i = 0; i < text->Length; )
		{
			String^ line = fin.ReadLine();
			if( !line )
			{
				Windows::Forms::MessageBox::Show( nullptr, L"Unexpected end of file: not enough lines of text", string_table );
				break;
			}
			if( line == L"" || line[ 0 ] == L'#' )
				continue;
			text[ i++ ] = line;
		}
		fin.Close();
	}
	//load armor, skills, etc.
	SkillTag::LoadLanguage( dir + L"tags.txt" );
	Material::LoadLanguage( dir + L"components.txt" );

	Armor::LoadLanguage( dir + L"head.txt",  Armor::ArmorType::HEAD );
	Armor::LoadLanguage( dir + L"body.txt",  Armor::ArmorType::BODY );
	Armor::LoadLanguage( dir + L"arms.txt",  Armor::ArmorType::ARMS );
	Armor::LoadLanguage( dir + L"waist.txt", Armor::ArmorType::WAIST );
	Armor::LoadLanguage( dir + L"legs.txt",  Armor::ArmorType::LEGS );

	Armor::LoadLanguageDLC( dir + L"events.txt" );

	Charm::LoadLanguage( dir + L"charms.txt" );

	Decoration::LoadLanguage( dir + L"decorations.txt" );

	Skill::LoadLanguage( dir + L"skills.txt" );
	Skill::LoadDescriptions( dir + L"skill_descriptions.txt" );
	Skill::UpdateOrdering();
	Ability::UpdateOrdering();
}

String^ StripAmpersands( String^ input )
{
	String^ res = L"";
	for each( wchar_t c in input )
	{
		if( c != L'&' )
			res += c;
	}
	return res;
}

void myassert( const bool val, String^ message )
{
	if( !val )
		MessageBox::Show( message );
}

void myassert( const bool val, System::String^ message, System::String^ suffix_copy )
{
	if( !val )
	{
		Clipboard::SetText( suffix_copy );
		MessageBox::Show( message + suffix_copy );
	}
}

bool ConvertInt( int% i, String^ str, StringTable::StringIndex err )
{
	try
	{
		const int limit = ( str[ 0 ] == L'-' ) ? 3 : 2;
		int temp;
		if( str->Length > limit )
			temp = Convert::ToInt32( str->Substring( 0, limit ) );
		else temp = Convert::ToInt32( str );

		i = temp;
		return true;
	}
	catch( FormatException^ )
	{
		MessageBox::Show( StringTable::text[(int)err ], StaticString( Error ), MessageBoxButtons::OK, MessageBoxIcon::Error );
		return false;
	}
}

bool ConvertUInt( unsigned% i, String^ str, StringTable::StringIndex err )
{
	unsigned temp;
	try
	{
		temp = Convert::ToUInt32( str );
	}
	catch( FormatException^ )
	{
		MessageBox::Show( StringTable::text[(int)err ], StaticString( Error ), MessageBoxButtons::OK, MessageBoxIcon::Error );
		return false;
	}
	i = temp;
	return true;
}

void SuspendDrawing( System::Windows::Forms::Control^ control )
{
	Win32::SendMessage( control->Handle, 11, false, 0 );
}

void ResumeDrawing( System::Windows::Forms::Control^ control )
{
	Win32::SendMessage( control->Handle, 11, true, 0 );
	control->Refresh();
}

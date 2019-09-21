#pragma once
#include "LoadedData.h"
#include "About.h"
#include "Advanced.h"
#include "Find.h"
#include "Version.h"
#include "ManageDecorations.h"
#include "SkillHelp.h"
#include "NumericUpDownHR.h"
#include "PreviewImage.h"

namespace MHWASS
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

#pragma region Comparison Functions
	int Compare( const int x, const int y )
	{
		return x < y ? -1 : y < x ? 1 : 0;
	}

	int ReverseCompare( const int x, const int y )
	{
		return x < y ? 1 : y < x ? -1 : 0;
	}
#define TryReverseCompare( X, Y ) X != Y ? ReverseCompare( X, Y )

#define BasicCompare( Name, Var ) \
	int CompareSolutionBy##Name( Solution^ s1, Solution^ s2 )\
	{\
		return s1->Var != s2->Var ? ReverseCompare( s1->Var, s2->Var ) : ReverseCompare( s1->total_slots_spare, s2->total_slots_spare );\
	}

	BasicCompare( Defence, defence );
	BasicCompare( MaxDefence, max_defence );
	BasicCompare( AugDefence, aug_defence );
	BasicCompare( Rarity, rarity );

	BasicCompare( FireRes, fire_res );
	BasicCompare( IceRes, ice_res );
	BasicCompare( WaterRes, water_res );
	BasicCompare( ThunderRes, thunder_res );
	BasicCompare( DragonRes, dragon_res );

#undef BasicCompare
	
	int CompareSolutionByDifficulty( Solution^ s1, Solution^ s2 )
	{
		return Compare( s1->difficulty, s2->difficulty );
	}

	int CompareSolutionBySlotsSpare( Solution^ s1, Solution^ s2 )
	{
		return ReverseCompare( s1->total_slots_spare, s2->total_slots_spare );
	}

	int CompareSolutionByFamily( Solution^ s1, Solution^ s2 )
	{
		return TryReverseCompare( s1->family_score[0], s2->family_score[0] ) :
			   TryReverseCompare( s1->family_score[1], s2->family_score[1] ) :
			   TryReverseCompare( s1->family_score[2], s2->family_score[2] ) :
			   ReverseCompare( s1->total_slots_spare, s2->total_slots_spare );
	}

	int CompareSolutionsByExtraSkills( Solution^ s1, Solution^ s2 )
	{
		return ReverseCompare( s1->extra_skills.Count, s2->extra_skills.Count );
	}

	int CompareSkillsByName( Skill^ s1, Skill^ s2 )
	{
		return s1->name->CompareTo( s2->name );
	}

	int CompareExtraSkills( ExtraSkill^ s1, ExtraSkill^ s2 )
	{
		return ( s1->skill->ability != s2->skill->ability || s1->want == s2->want ) ? s1->skill->name->CompareTo( s2->skill->name ) : s2->want - s1->want;
	}
#pragma endregion

	public ref class Form1 : public System::Windows::Forms::Form
	{
		typedef System::Windows::Forms::DialogResult DialogResult_t;
		const static DialogResult_t OK = DialogResult_t::OK;
		int MAX_LIMIT;
		int NumSkills = 10;
		const static int MaxSolutions = 100000;
		static Threading::Mutex^ progress_mutex = gcnew Threading::Mutex;
		static Threading::Mutex^ results_mutex = gcnew Threading::Mutex;
		static Threading::Mutex^ all_solutions_mutex = gcnew Threading::Mutex;
		static Threading::Mutex^ worker_mutex = gcnew Threading::Mutex;
		String^ CFG_FILE;
		String^ endl;
		String^ last_result;
		bool lock_skills, sort_off, can_save, updating_language, construction_complete, lock_related, search_cancelled, updating_extra_skills;
		LoadedData^ data;
		Query^ query;
		Find^ find_dialog;
		typedef Generic::Dictionary< unsigned, unsigned > IndexMap;
		SkillHelp^ help_window;
		PreviewImage^ preview_pane;
		ToolStripMenuItem^ mnuToggleAllDLC;

		Generic::Dictionary< unsigned long long, unsigned > solution_hashes;

		List_t< Solution^ > final_solutions, all_solutions;
		List_t< ComboBox^ > Skills;
		List_t< ComboBox^ > SkillFilters;
		List_t< IndexMap^ > IndexMaps;
		List_t< String^ > languages;
		List_t< Charm^ > charm_box_charms;
		List_t< int > result_offsets;
		List_t< Ability^ > last_selected_ability;
		List_t< ExtraSkill^ > solutions_extra_skills;
		int language, adv_x, adv_y;
#pragma region Members
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuFile;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuExit;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuHelp;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuAbout;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuLanguage;
	private: System::Windows::Forms::GroupBox^  grpGender;
	private: System::Windows::Forms::RadioButton^  rdoFemale;
	private: System::Windows::Forms::RadioButton^  rdoMale;
	private: System::Windows::Forms::Button^  btnAdvancedSearch;
	private: MHWASS::NumericUpDownHR^  nudHR;
	private: MHWASS::NumericUpDownSlot^  nudWeaponSlots1;
	private: MHWASS::NumericUpDownSlot^  nudWeaponSlots2;
	private: MHWASS::NumericUpDownSlot^  nudWeaponSlots3;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Label^  lblHR;
	private: System::Windows::Forms::Label^  lblSlots;
	private: System::Windows::Forms::Button^  btnCancel;
	private: System::Windows::Forms::GroupBox^  grpSkills;
	private: System::Windows::Forms::Button^  btnSearch;
	private: System::Windows::Forms::ProgressBar^  progressBar1;
	private: System::Windows::Forms::RichTextBox^  txtSolutions;
	private: System::Windows::Forms::GroupBox^  groupBox4;
	private: System::Windows::Forms::GroupBox^  grpResults;
	private: System::Windows::Forms::GroupBox^  grpSkillFilters;
	private: System::Windows::Forms::GroupBox^  grpSortFilter;
	private: System::Windows::Forms::ComboBox^  cmbSort;
	private: System::Windows::Forms::ContextMenuStrip^  cmsSolutions;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuOptions;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuAllowArena;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuAllowEventArmor;
	private: System::Windows::Forms::Button^  btnDecorations;
	private: System::Windows::Forms::GroupBox^  grpDecorations;
	private: System::Windows::Forms::ComboBox^  cmbDecorationSelect;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuCheckForUpdates;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuClearSettings;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
	private: System::Windows::Forms::ContextMenuStrip^  cmsCharms;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuLoadData;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuSaveData;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuSortSkillsAlphabetically;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuPrintDecoNames;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuMaxResults;
	private: System::Windows::Forms::ToolStripTextBox^  mnuNumResults;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuAllowLowerTierArmor;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuSkillHelp;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator2;
	private: System::Windows::Forms::ComboBox^  cmbFilterByExtraSkill;
	private: System::Windows::Forms::ContextMenuStrip^  cmsSkills;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuClearSkill;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuDonate;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuPrintMaterials;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuZoom;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuAllowFairWind;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuAlwaysSearchAlpha;
	private: System::Windows::Forms::Label^  lblAddSkills;
	private: System::Windows::Forms::Label^  lblRemoveSkills;
	private: System::Windows::Forms::ToolTip^  tipWeaponSlots;
	private: System::Windows::Forms::ToolTip^  tipSkills;
	private: System::Windows::Forms::ToolTip^  tipQuestLevel;
	private: System::Windows::Forms::ToolTip^  tipResults;
	private: System::Windows::Forms::ToolTip^  tipSortFilter;
	private: System::Windows::Forms::ToolTip^  tipDecorations;

#pragma endregion

			 List_t< BackgroundWorker^ >  workers;
			 List_t< Object^ > worker_data;
			 unsigned finished_workers, total_progress, worker_start_index, num_updates;

			 void ClearFilters()
			 {
				 for each( ComboBox^ box in SkillFilters )
					 box->Items->Clear();
			 }

			 void AddFilters()
			 {
				 for each( SkillTag^ tag in SkillTag::tags )
				 {
					 for each( ComboBox^ box in SkillFilters )
						 box->Items->Add( tag->name );
				 }
			 }

			 void InitFilters()
			 {
				 ClearFilters();
				 AddFilters();

				 for each( ComboBox^ box in SkillFilters )
					 box->SelectedIndex = 0;
			 }

			 void ResetSkill( ComboBox^ box, IndexMap^ map, Skill^ skill )
			 {
				 if( skill == nullptr )
					 return;

				 IndexMap::Enumerator iter = map->GetEnumerator();
				 while( iter.MoveNext() )
				 {
					 if( Skill::static_skills[ iter.Current.Value ] == skill )
					 {
						 box->SelectedIndex = iter.Current.Key;
						 return;
					 }
				 }
			 }

			 void InitSkills2( ComboBox^ box, IndexMap^ map, const int filter, List_t< Ability^ >^ disallowed )
			 {
				 map->Clear();
				 box->SelectedIndex = -1;
				 box->Items->Clear();
				 if( filter < 0 || !StringTable::text )
					 return;

				 box->Items->Add( StaticString( NoneBrackets ) );
				 List_t< Skill^ >^ the_list = mnuSortSkillsAlphabetically->Checked ? %Skill::ordered_skills : %Skill::static_skills;
				 for each( Skill^ skill in the_list )
				 {
					 if( skill->ability->set_ability )
						 continue;

					 if( Utility::Contains( disallowed, skill->ability ) )
						 continue;

					 if( filter == 0 || filter == 1 && skill->ability->tags.Count == 0 ||
						 filter == 2 && skill->ability->related ||
						 !!Utility::Contains( %skill->ability->tags, SkillTag::tags[ filter ] ) )
					 {
						 map[ box->Items->Count ] = skill->static_index;
						 box->Items->Add( skill->name );
					 }
				 }
			 }

			 void InitSkills( ComboBox^ box, IndexMap^ map, const int filter, List_t< Ability^ >^ disallowed )
			 {
				 unsigned index = filter;
				 InitSkills2( box, map, index, disallowed );
			 }

			 void InitSkills()
			 {
				 for( int i = 0; i < NumSkills; ++i )
				 {
					 InitSkills( Skills[ i ], IndexMaps[ i ], SkillFilters[ i ]->SelectedIndex, gcnew List_t< Ability^ >() );
				 }
			 }

			 ref class MyComboBox : public ComboBox
			 {
				 bool draw_focus;
			 public:
				 MyComboBox() : draw_focus( false ) {}

				 void SetOwnerDraw()
				 {
					 DrawMode = Windows::Forms::DrawMode::OwnerDrawFixed;
					 try
					 {
						 VisualStyles::VisualStyleRenderer renderer( "COMBOBOX", 1, (int)VisualStyles::ComboBoxState::Normal );
					 }
					 catch( Exception^ e )
					 {
						 Assert( false, e->Message );
						 return; //failed to create style renderer. don't set user paint
					 }

					 SetStyle( ControlStyles::UserPaint | ControlStyles::DoubleBuffer, true );
				 }

				 virtual void OnLostFocus( System::EventArgs^ e ) override
				 {
					 draw_focus = false;
					 Refresh();
				 }

				 virtual void OnGotFocus( System::EventArgs^ e ) override
				 {
					 draw_focus = true;
					 Refresh();
				 }

				 virtual void OnDrawItem( DrawItemEventArgs^ e ) override
				 {
					 e->DrawBackground();
					 e->DrawFocusRectangle();

					 if( e->Index < 0 )
						 return;

					 String^ text = Items[ e->Index ]->ToString();
					 Skill^ skill = Skill::FindSkill( text );

					 TextRenderer::DrawText( e->Graphics, text, e->Font, e->Bounds, ( skill && skill->impossible ) ? Color::Red : Color::Black, TextFormatFlags::Left );
				 }

				 virtual void OnDropDown( System::EventArgs^ e ) override
				 {
					 draw_focus = false;
					 Invalidate();
				 }

				 virtual void OnDropDownClosed( System::EventArgs^ e ) override
				 {
					 if( SelectedIndex == 0 && DrawMode == Windows::Forms::DrawMode::OwnerDrawFixed )
					 {
						 SuspendDrawing( this );
						 SelectedIndex = -1;
						 ResumeDrawing( this );
					 }
					 draw_focus = false;
				 }

				 virtual void OnPaint( PaintEventArgs^ e ) override
				 {
					 ButtonRenderer::DrawButton( e->Graphics, Rectangle( -1, -1, Size.Width + 2, Size.Height + 2 ), DroppedDown ? VisualStyles::PushButtonState::Pressed : VisualStyles::PushButtonState::Normal );

					 //See https://www.gittprogram.com/question/479791_comboboxrenderer-does-not-look-like-windows-7-combobox.html
					 VisualStyles::VisualStyleRenderer renderer( "COMBOBOX", 1, (int)( DroppedDown ? VisualStyles::ComboBoxState::Pressed : VisualStyles::ComboBoxState::Normal ) );
					 renderer.DrawBackground( e->Graphics, Rectangle( Size.Width - 20, 0, 20, Size.Height ), Rectangle( Size.Width - 15, 5, 10, Size.Height - 10 ) );

					 if( Focused && !DroppedDown && draw_focus )
						 ControlPaint::DrawFocusRectangle( e->Graphics, Rectangle( 3, 3, Width - 23, Height - 6 ) );

					 if( SelectedIndex < 0 )
						 return;

					 Skill^ skill = Skill::FindSkill( Text );

					 TextRenderer::DrawText( e->Graphics, Text, Font, Point( 2, 4 ), ( skill && skill->impossible ) ? Color::Red : Color::Black );
				 }
			 };

			 ComboBox^ GetNewComboBox( const unsigned width, const unsigned i, const bool is_filter )
			 {
				 MyComboBox^ box = gcnew MyComboBox;
				 box->Location = System::Drawing::Point( 6, 19 + i * 27 );
				 box->Size = System::Drawing::Size( width, box->Size.Height );
				 box->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
				 box->AutoCompleteSource = AutoCompleteSource::ListItems;
				 box->AutoCompleteMode = AutoCompleteMode::Suggest;

				 if( is_filter )
				 {
					 box->SelectedIndexChanged += gcnew System::EventHandler( this, &Form1::cmbSkillFilter_SelectedIndexChanged );
				 }
				 else
				 {
					 box->SelectionChangeCommitted += gcnew System::EventHandler( this, &Form1::cmbSkill_SelectedIndexChanged );
					 box->ContextMenuStrip = cmsSkills;
					 box->SetOwnerDraw();
				 }
				 return box;
			 }

			 void ClearSkillClicked( Object^ sender, EventArgs^ e )
			 {
				 ComboBox^ box = safe_cast<ComboBox^>( cmsSkills->SourceControl );
				 if( box )
				 {
					 box->SelectedIndex = -1;
					 cmbSkill_SelectedIndexChanged( box, nullptr );
				 }
			 }

			 void InitializeComboBox( const int i )
			 {
				 SkillFilters.Add( GetNewComboBox( grpSkillFilters->Width - 13, i, true ) );
				 grpSkillFilters->Controls->Add( SkillFilters[ i ] );

				 Skills.Add( GetNewComboBox( grpSkills->Width - 13, i, false ) );
				 grpSkills->Controls->Add( Skills[ i ] );

				 IndexMaps.Add( gcnew IndexMap );

				 last_selected_ability.Add( nullptr );
			 }

			 void InitializeComboBoxes()
			 {
				 for( int i = 0; i < NumSkills; ++i )
				 {
					 InitializeComboBox( i );
				 }
				 cmbSort->SelectedIndex = 0;
				 cmbDecorationSelect->SelectedIndex = 2;
			 }

			 void AddZoomOption( const int zoom_level )
			 {
				 ToolStripMenuItem^ item = gcnew ToolStripMenuItem( zoom_level + L"%" );
				 item->Tag = zoom_level;
				 item->Checked = zoom_level == PreviewImage::zoom;
				 item->Name = L"mnuZoom" + zoom_level;
				 item->Size = System::Drawing::Size( 152, 22 );
				 item->Click += gcnew EventHandler( this, &Form1::ZoomLevelChanged );

				 mnuZoom->DropDownItems->Add( item );
			 }

	public:

		Form1( void ) : CFG_FILE( L"settings.cfg" ), endl( L"\r\n" ), construction_complete( false ), MAX_LIMIT( 1000 )
		{
			DoubleBuffered = true;
			language = -1;
			sort_off = false;
			updating_language = false;
			can_save = false;
			lock_related = false;
			updating_extra_skills = false;
			InitializeComponent();
			InitializeComboBoxes();

			this->Text += GAMES;
			
			mnuNumResults->KeyPress += gcnew KeyPressEventHandler( this, &Form1::MaxResultsTextBoxKeyPress );
			mnuNumResults->TextChanged += gcnew EventHandler( this, &Form1::MaxResultsTextChanged );
			mnuAllowEventArmor->DropDown->Closing += gcnew ToolStripDropDownClosingEventHandler( this, &Form1::EventArmorMenuClosing );
			cmsSolutions->Closing += gcnew ToolStripDropDownClosingEventHandler( this, &Form1::contextMenuStrip1_Closing );
			cmbFilterByExtraSkill->SelectedIndex = 0;
			mnuClearSkill->Click += gcnew EventHandler( this, &Form1::ClearSkillClicked );

			AddZoomOption( 50 );
			AddZoomOption( 75 );
			AddZoomOption( 100 );
			AddZoomOption( 150 );
			AddZoomOption( 200 );

			can_save = true;
			adv_x = 1031;
			adv_y = 587;

			preview_pane = nullptr;

			this->Size = Drawing::Size( Width, NumSkills * 27 + 307 );

			data = gcnew LoadedData();
			data->LoadDataFiles();
			CreateDLCMenus();
			LoadLanguages();

			InitFilters();
			InitSkills();

			lock_skills = false;
			btnCancel->Enabled = false;

			LoadConfig( true );

			Decoration::LoadCustom();

			LoadConfig( false );

			UpdateImpossibleSkills();

			Text += " v" + STRINGIZE( VERSION_NO );

			Focus(); //without this, the icon won't show in the taskbar, for some reason

			construction_complete = true;
		}

		void CreateDLCMenus()
		{
			mnuToggleAllDLC = gcnew ToolStripMenuItem( L"Disable All" );
			mnuToggleAllDLC->Click += gcnew EventHandler( this, &Form1::ToggleAllDLC_Click );
			mnuAllowEventArmor->DropDownItems->Add( mnuToggleAllDLC );

			mnuAllowEventArmor->DropDownItems->Add( L"-" );

			for each( ArmorDLC^ dlc in Armor::static_dlc )
			{
				ToolStripMenuItem^ item = gcnew ToolStripMenuItem( dlc->name );
				item->Tag = dlc;
				item->Click += gcnew EventHandler( this, &Form1::DLC_Click );
				item->Checked = true;
				item->CheckOnClick = true;
				mnuAllowEventArmor->DropDownItems->Add( item );
			}
		}

		void LoadLanguages()
		{
			array< String^ >^ langs = IO::Directory::GetDirectories( L"Data/Languages" );
			for each( String^% lang in langs )
			{
				//Assert( CanDisplayString( lang ) , "Can't display language" );
				ToolStripMenuItem^ item = gcnew ToolStripMenuItem( lang->Substring( lang->LastIndexOf( L'\\' ) + 1 ) );
				item->Click += gcnew EventHandler( this, &Form1::LanguageSelect_Click );
				mnuLanguage->DropDownItems->Add( item );
			}
		}

		void AddSolution( String^ line, const unsigned version )
		{
			if( all_solutions.Count >= MaxSolutions )
				return;

			List_t< String^ > split;
			Utility::SplitString( %split, line, L' ' );
			Solution^ solution = gcnew Solution;
			for( unsigned i = 0; i < 5; ++i )
			{
				const int index = Convert::ToInt32( split[ i ] );
				if( index < 0 )
					solution->armors.Add( nullptr );
				else solution->armors.Add( Armor::static_armors[ i ][ index ] );
			}
			const int charm_index = Convert::ToInt32( split[ 5 ] );
			if( charm_index >= 0 )
				solution->charm = Charm::static_charms[ charm_index ];
			solution->slots_spare = gcnew array< unsigned >( 4 );
			solution->total_slots_spare = 0;
			for( unsigned i = 1; i <= 3; ++i )
			{
				solution->slots_spare[ i ] = Convert::ToUInt32( split[ 5 + i ] );
				solution->total_slots_spare += solution->slots_spare[ i ];
			}

			const unsigned num_decorations = Convert::ToUInt32( split[ 9 ] );
			for( unsigned i = 0; i < num_decorations; ++i )
				solution->decorations.Add( Decoration::static_decorations[ Convert::ToUInt32( split[ 10 + i ] ) ] );

			const unsigned decorations_end = 10 + num_decorations;

			const unsigned num_skills = Convert::ToUInt32( split[ decorations_end ] );
			for( unsigned i = 0; i < num_skills; ++i )
			{
				const int index = Convert::ToUInt32( split[ 1 + decorations_end + i ] );
				solution->extra_skills.Add( Skill::static_skills[ index ] );
			}

			solution->CalculateData();

			all_solutions.Add( solution );
		}

		void LoadDefaultLanguage()
		{
			language = -1;
			String^ native_lang = System::Globalization::CultureInfo::InstalledUICulture->Parent->NativeName->ToLower();
			for each( ToolStripItem^ item in mnuLanguage->DropDownItems )
			{
				if( item->ToString()->ToLower()->IndexOf( native_lang ) >= 0 )
				{
					LanguageSelect_Click( item, nullptr );
					return;
				}
			}
			if( language == -1 )
			{
				for each( ToolStripItem^ item in mnuLanguage->DropDownItems )
				{
					if( item->ToString()->ToLower()->IndexOf( L"english" ) >= 0 )
					{
						LanguageSelect_Click( item, nullptr );
						return;
					}
				}
			}

			LanguageSelect_Click( mnuLanguage->DropDownItems[ 0 ], nullptr );
		}

		void SetupDefaultConfig()
		{
			if( mnuLanguage->HasDropDownItems )
			{
				LoadDefaultLanguage();
			}
			can_save = true;
			SaveConfig();
		}

		void LoadConfig( String^ file, const bool lang )
		{
			can_save = false;
			if( !IO::File::Exists( file ) )
				return SetupDefaultConfig();

			IO::StreamReader fin( file );
			String^ line = fin.ReadLine();
			if( !line || !line->StartsWith( L"VERSION=" ) )
			{
				fin.Close();
				return SetupDefaultConfig();
			}
			const int version = Convert::ToInt32( line->Substring( 8 ) );
			if( version < 1 || version > 1 )
			{
				fin.Close();
				return SetupDefaultConfig();
			}

			if( lang )
			{
				while( !line->StartsWith( L"LANGUAGE=" ) && !fin.EndOfStream )
				{
					line = fin.ReadLine();
				}
				fin.Close();
				if( line->StartsWith( L"LANGUAGE=" ) )
				{
					String^ lang_name = line->Substring( 9 );
					for( int i = 0; i < mnuLanguage->DropDownItems->Count; ++i )
					{
						if( mnuLanguage->DropDownItems[ i ]->ToString() == lang_name )
						{
							LanguageSelect_Click( mnuLanguage->DropDownItems[ i ], nullptr );
							return;
						}
					}
				}
				SetupDefaultConfig();
				return;
			}

			while( line != L"SOLUTIONS:" && !fin.EndOfStream )
			{
				line = fin.ReadLine();

				if( line->StartsWith( L"RESULTLIMIT=" ) )
				{
					mnuNumResults->Text = line->Substring( 12 );
					MAX_LIMIT = Convert::ToInt32( mnuNumResults->Text );
				}
				else if( line->StartsWith( L"GENDER=" ) )
				{
					if( line->EndsWith( L"Male" ) )
						rdoMale->Checked = true;
					else if( line->EndsWith( L"Female" ) )
						rdoMale->Checked = false;
					rdoFemale->Checked = !rdoMale->Checked;
				}
				else if( line->StartsWith( L"ALLOWARENA=" ) )
				{
					mnuAllowArena->Checked = Convert::ToBoolean( line->Substring( 11 ) );
				}
				else if( line->StartsWith( L"ALLOWLOWTIER=" ) )
				{
					mnuAllowLowerTierArmor->Checked = Convert::ToBoolean( line->Substring( 13 ) );
				}
				else if( line->StartsWith( L"ALLOWEVENTARMOR=" ) )
				{
					String^ checked = line->Substring( 16 );
					const int limit = Math::Min( checked->Length, mnuAllowEventArmor->DropDownItems->Count - 2 );
					for( int i = 0; i < limit; ++i )
					{
						ToolStripMenuItem^ item = safe_cast< ToolStripMenuItem^ >( mnuAllowEventArmor->DropDownItems[ i + 2 ] );
						if( checked[ i ] == L'1' )
							item->Checked = true;
						else if( checked[ i ] == L'0' )
							item->Checked = false;
						
						DLC_Click( item, nullptr );
					}

					if( HaveAllDLCEnabled() )
						mnuToggleAllDLC->Text = StaticString( DisableAll );
					else
						mnuToggleAllDLC->Text = StaticString( EnableAll );
				}
				else if( line->StartsWith( L"SHOWDECONAMES=" ) )
				{
					mnuPrintDecoNames->Checked = Convert::ToBoolean( line->Substring( 14 ) );
				}
				else if( line->StartsWith( L"SHOWMATLIST=" ) )
				{
					mnuPrintMaterials->Checked = Convert::ToBoolean( line->Substring( 12 ) );
				}
				else if( line->StartsWith( L"SORTALPHA=" ) )
				{
					mnuSortSkillsAlphabetically->Checked = Convert::ToBoolean( line->Substring( 10 ) );
				}
				else if( line->StartsWith( L"ALLOWFAIRWIND=" ) )
				{
					mnuAllowFairWind->Checked = Convert::ToBoolean( line->Substring( 14 ) );
				}
				else if( line->StartsWith( L"ALWAYSUSEALPHA=" ) )
				{
					mnuAlwaysSearchAlpha->Checked = Convert::ToBoolean( line->Substring( 15 ) );
				}
				else if( line->StartsWith( L"PREVIEWZOOM=" ) )
				{
					const int zoom = Convert::ToInt32( line->Substring( 12 ) );
					for each( ToolStripMenuItem^ item in mnuZoom->DropDownItems )
					{
						if( zoom == (Int32)item->Tag )
						{
							ZoomLevelChanged( item, nullptr );
							break;
						}
					}
				}
				else if( line->StartsWith( L"SORTTYPE=" ) )
				{
					cmbSort->SelectedIndex = Convert::ToInt32( line->Substring( 9 ) );
				}
				else if( line->StartsWith( L"DECOTYPE=" ) )
				{
					cmbDecorationSelect->SelectedIndex = Convert::ToInt32( line->Substring( 9 ) );
				}
				else if( line->StartsWith( L"QUESTLEVEL=" ) )
				{
					nudHR->Value = Clamp( Convert::ToInt32( line->Substring( 11 ) ), (int)nudHR->Minimum, (int)nudHR->Maximum );
				}
				else if( line->StartsWith( L"WEAPONSLOTS=" ) )
				{
					nudWeaponSlots1->Value = Convert::ToInt32( line->Substring( 12, 1 ) );
					nudWeaponSlots2->Value = Convert::ToInt32( line->Substring( 13, 1 ) );
					nudWeaponSlots3->Value = Convert::ToInt32( line->Substring( 14, 1 ) );
				}
				else if( line->StartsWith( L"WEAPONSLOTSSHOWN=" ) )
				{
					//ignore
				}
				else if( line->StartsWith( L"ADVWIDTH=" ) )
				{
					adv_x = Convert::ToInt32( line->Substring( 9 ) );
				}
				else if( line->StartsWith( L"ADVHEIGHT=" ) )
				{
					adv_y = Convert::ToInt32( line->Substring( 10 ) );
				}
				else if( line->StartsWith( L"NUMCOMBOBOXES=" ) )
				{
					const int num = Convert::ToInt32( line->Substring( 14 ) );
					for( int i = NumSkills; i < num; ++i )
					{
						lblAddSkills_Click( lblAddSkills, nullptr );
					}
				}
				else if( line->StartsWith( L"NUMSKILLS=" ) )
				{
					for each( ComboBox^ box in SkillFilters )
						box->SelectedIndex = -1;

					for each( ComboBox^ box in Skills )
						box->SelectedIndex = -1;

					const int num_skills = Convert::ToInt32( line->Substring( 10 ) );
					for( int i = 0; i < num_skills; ++i )
					{
						String^ filter = fin.ReadLine();
						String^ index = fin.ReadLine();
						if( i < NumSkills && filter->StartsWith( L"FILTER=" ) && index->StartsWith( L"INDEX=" ) )
						{
							const int filter_index = Convert::ToInt32( filter->Substring( 7 ) );
							if( filter_index < 0 )
								SkillFilters[ i ]->SelectedIndex = 0;
							else
								SkillFilters[ i ]->SelectedIndex = filter_index;

							if( SkillFilters[ i ]->SelectedIndex == 2 ) //related
							{
								SkillFilters[ i ]->SelectedIndex = 0;
								Skills[ i ]->SelectedIndex = SearchIndexMap( IndexMaps[ i ], Convert::ToInt32( index->Substring( 6 ) ) );
								SkillFilters[ i ]->SelectedIndex = 2;
							}
							else Skills[ i ]->SelectedIndex = SearchIndexMap( IndexMaps[ i ], Convert::ToInt32( index->Substring( 6 ) ) );
						}
					}

					for each( ComboBox^ box in SkillFilters )
					{
						if( box->SelectedIndex == -1 )
							box->SelectedIndex = 0;
						else
							cmbSkillFilter_SelectedIndexChanged( box, nullptr );
					}
				}
				else if( line->StartsWith( L"NUMSEARCHED=" ) )
				{
					const unsigned num_searched_skills = Convert::ToUInt32( line->Substring( 12 ) );
					List_t< Skill^ > searched_skills;
					for( unsigned i = 0; i < num_searched_skills; ++i )
					{
						String^ s = fin.ReadLine();
						if( s->StartsWith( L"SKILL=" ) )
							searched_skills.Add( Skill::static_skills[ Convert::ToUInt32( s->Substring( 6  ) ) ] );
					}

					FormulateQuery( %searched_skills );
				}
				else if( !line->StartsWith( L"LANGUAGE=" ) && line != L"SOLUTIONS:" )
				{
					Assert( false, L"Unknown config setting: " + line );
				}
			}
			if( line == L"SOLUTIONS:" )
			{
				all_solutions.Clear();
				do
				{
					line = fin.ReadLine();
					if( line )
						AddSolution( line, version );
					else break;
				}
				while( !fin.EndOfStream );
			}
			
			last_result = nullptr;
			fin.Close();

			UpdateResultString();
			UpdateExtraSkillCombo( false );
			
			can_save = true;
			SaveConfig();
		}

		void LoadConfig( const bool lang )
		{
			LoadConfig( CFG_FILE, lang );
		}

		String^ GetEventString()
		{
			String^ res = L"";
			for( int i = 2; i < mnuAllowEventArmor->DropDownItems->Count; ++i )
			{
				ToolStripMenuItem^ item = safe_cast<ToolStripMenuItem^ >( mnuAllowEventArmor->DropDownItems[ i ] );
				if( item->Tag )
					res += item->Checked ? L"1" : L"0";
			}
			return res;
		}

		void SaveConfig( String^ file )
		{
			if( !can_save )
				return;

			IO::StreamWriter fout( file );
			fout.WriteLine( L"VERSION=1" );
			fout.WriteLine( L"LANGUAGE=" + mnuLanguage->DropDownItems[ language ]->ToString() );
			fout.WriteLine( L"RESULTLIMIT=" + MAX_LIMIT );
			fout.WriteLine( L"NUMCOMBOBOXES=" + NumSkills );
			fout.WriteLine( L"GENDER=" + gcnew String( rdoMale->Checked ? L"Male" : L"Female" ) );
			fout.WriteLine( L"ALLOWARENA=" + mnuAllowArena->Checked );
			fout.WriteLine( L"ALLOWLOWTIER=" + mnuAllowLowerTierArmor->Checked );
			fout.WriteLine( L"ALLOWEVENTARMOR=" + GetEventString() );
			fout.WriteLine( L"SHOWDECONAMES=" + mnuPrintDecoNames->Checked );
			fout.WriteLine( L"SHOWMATLIST=" + mnuPrintMaterials->Checked );
			fout.WriteLine( L"SORTALPHA=" + mnuSortSkillsAlphabetically->Checked );
			fout.WriteLine( L"ALLOWFAIRWIND=" + mnuAllowFairWind->Checked );
			fout.WriteLine( L"ALWAYSUSEALPHA=" + mnuAlwaysSearchAlpha->Checked );
			fout.WriteLine( L"PREVIEWZOOM=" + PreviewImage::zoom );
			fout.WriteLine( L"SORTTYPE=" + cmbSort->SelectedIndex );
			fout.WriteLine( L"DECOTYPE=" + cmbDecorationSelect->SelectedIndex );
			fout.WriteLine( L"QUESTLEVEL=" + nudHR->Value );
			fout.WriteLine( L"WEAPONSLOTS=" + nudWeaponSlots1->Value.ToString() + nudWeaponSlots2->Value.ToString() + nudWeaponSlots3->Value.ToString() );
			fout.WriteLine( L"ADVWIDTH=" + adv_x );
			fout.WriteLine( L"ADVHEIGHT=" + adv_y );
			fout.WriteLine( L"NUMSKILLS=" + NumSkills );
			for( int i = 0; i < NumSkills; ++i )
			{
				fout.WriteLine( L"FILTER=" + SkillFilters[ i ]->SelectedIndex );
				if( Skills[ i ]->SelectedIndex <= 0 )
					fout.WriteLine( L"INDEX=-1" );
				else
				{
					IndexMap^ imap = IndexMaps[ i ];
					fout.WriteLine( L"INDEX=" + imap[ Skills[ i ]->SelectedIndex ] );
				}
			}

			const unsigned num_searched_skills = query ? query->skills.Count : 0;
			fout.WriteLine( L"NUMSEARCHED=" + num_searched_skills );
			for( unsigned i = 0; i < num_searched_skills; ++i )
				fout.WriteLine( L"SKILL=" + query->skills[ i ]->static_index );

			fout.WriteLine( L"SOLUTIONS:" );
			
			for each( Solution^ solution in all_solutions )
			{
				for( unsigned i = 0; i < 5; ++i )
				{
					Armor^ a = solution->armors[ i ];
					if( a )
						fout.Write( Convert::ToString( a->index ) );
					else
						fout.Write( L"-1" );
					fout.Write( L" " );
				}
				if( solution->charm )
				{
					fout.Write( Convert::ToString( solution->charm->index ) );
					fout.Write( L" " );
				}
				else fout.Write( L"-1 " );
				for( unsigned i = 1; i <= 3; ++i )
				{
					fout.Write( Convert::ToString( solution->slots_spare[ i ] ) );
					fout.Write( L" " );
				}
				fout.Write( Convert::ToString( solution->decorations.Count ) );
				fout.Write( L" " );
				for each( Decoration^ decoration in solution->decorations )
				{
					fout.Write( Convert::ToString( decoration->index ) );
					fout.Write( L" " );
				}
				fout.Write( Convert::ToString( solution->extra_skills.Count ) );
				fout.Write( L" " );
				for each( Skill^ skill in solution->extra_skills )
				{
					fout.Write( Convert::ToString( skill->static_index ) );
					fout.Write( L" " );
				}

				fout.WriteLine();
			}
			fout.Close();
		}

		void SaveConfig()
		{
			SaveConfig( CFG_FILE );
		}

		int SearchIndexMap( IndexMap^ imap, int skill_index )
		{
			for each( Generic::KeyValuePair< unsigned, unsigned > kvp in imap )
			{
				if( kvp.Value == skill_index )
					return kvp.Key;
			}
			return -1;
		}

	protected:

		~Form1()
		{
			SaveConfig();
		}

	private: System::ComponentModel::IContainer^  components;


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = ( gcnew System::ComponentModel::Container() );
			System::ComponentModel::ComponentResourceManager^  resources = ( gcnew System::ComponentModel::ComponentResourceManager( Form1::typeid ) );
			this->groupBox1 = ( gcnew System::Windows::Forms::GroupBox() );
			this->lblSlots = ( gcnew System::Windows::Forms::Label() );
			this->lblHR = ( gcnew System::Windows::Forms::Label() );
			this->grpSkills = ( gcnew System::Windows::Forms::GroupBox() );
			this->lblRemoveSkills = ( gcnew System::Windows::Forms::Label() );
			this->lblAddSkills = ( gcnew System::Windows::Forms::Label() );
			this->btnSearch = ( gcnew System::Windows::Forms::Button() );
			this->progressBar1 = ( gcnew System::Windows::Forms::ProgressBar() );
			this->txtSolutions = ( gcnew System::Windows::Forms::RichTextBox() );
			this->cmsSolutions = ( gcnew System::Windows::Forms::ContextMenuStrip( this->components ) );
			this->groupBox4 = ( gcnew System::Windows::Forms::GroupBox() );
			this->btnAdvancedSearch = ( gcnew System::Windows::Forms::Button() );
			this->btnCancel = ( gcnew System::Windows::Forms::Button() );
			this->grpResults = ( gcnew System::Windows::Forms::GroupBox() );
			this->btnDecorations = ( gcnew System::Windows::Forms::Button() );
			this->grpSkillFilters = ( gcnew System::Windows::Forms::GroupBox() );
			this->menuStrip1 = ( gcnew System::Windows::Forms::MenuStrip() );
			this->mnuFile = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuLoadData = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuSaveData = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuExit = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuOptions = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuClearSettings = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->toolStripSeparator1 = ( gcnew System::Windows::Forms::ToolStripSeparator() );
			this->mnuAllowEventArmor = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuAllowArena = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuAllowLowerTierArmor = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuAllowFairWind = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuAlwaysSearchAlpha = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->toolStripSeparator2 = ( gcnew System::Windows::Forms::ToolStripSeparator() );
			this->mnuMaxResults = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuNumResults = ( gcnew System::Windows::Forms::ToolStripTextBox() );
			this->mnuZoom = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuPrintDecoNames = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuPrintMaterials = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuSortSkillsAlphabetically = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuLanguage = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuHelp = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuSkillHelp = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuDonate = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuCheckForUpdates = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuAbout = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->grpGender = ( gcnew System::Windows::Forms::GroupBox() );
			this->rdoFemale = ( gcnew System::Windows::Forms::RadioButton() );
			this->rdoMale = ( gcnew System::Windows::Forms::RadioButton() );
			this->grpSortFilter = ( gcnew System::Windows::Forms::GroupBox() );
			this->cmbFilterByExtraSkill = ( gcnew System::Windows::Forms::ComboBox() );
			this->cmbSort = ( gcnew System::Windows::Forms::ComboBox() );
			this->grpDecorations = ( gcnew System::Windows::Forms::GroupBox() );
			this->cmbDecorationSelect = ( gcnew System::Windows::Forms::ComboBox() );
			this->cmsCharms = ( gcnew System::Windows::Forms::ContextMenuStrip( this->components ) );
			this->cmsSkills = ( gcnew System::Windows::Forms::ContextMenuStrip( this->components ) );
			this->mnuClearSkill = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->tipWeaponSlots = ( gcnew System::Windows::Forms::ToolTip( this->components ) );
			this->tipSkills = ( gcnew System::Windows::Forms::ToolTip( this->components ) );
			this->tipQuestLevel = ( gcnew System::Windows::Forms::ToolTip( this->components ) );
			this->tipResults = ( gcnew System::Windows::Forms::ToolTip( this->components ) );
			this->tipSortFilter = ( gcnew System::Windows::Forms::ToolTip( this->components ) );
			this->tipDecorations = ( gcnew System::Windows::Forms::ToolTip( this->components ) );
			this->nudWeaponSlots1 = ( gcnew MHWASS::NumericUpDownSlot() );
			this->nudWeaponSlots2 = ( gcnew MHWASS::NumericUpDownSlot() );
			this->nudWeaponSlots3 = ( gcnew MHWASS::NumericUpDownSlot() );
			this->nudHR = ( gcnew MHWASS::NumericUpDownHR() );
			this->groupBox1->SuspendLayout();
			this->grpSkills->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->grpResults->SuspendLayout();
			this->menuStrip1->SuspendLayout();
			this->grpGender->SuspendLayout();
			this->grpSortFilter->SuspendLayout();
			this->grpDecorations->SuspendLayout();
			this->cmsSkills->SuspendLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->nudWeaponSlots1 ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->nudWeaponSlots2 ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->nudWeaponSlots3 ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->nudHR ) )->BeginInit();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add( this->nudWeaponSlots1 );
			this->groupBox1->Controls->Add( this->nudWeaponSlots2 );
			this->groupBox1->Controls->Add( this->lblSlots );
			this->groupBox1->Controls->Add( this->nudWeaponSlots3 );
			this->groupBox1->Controls->Add( this->nudHR );
			this->groupBox1->Controls->Add( this->lblHR );
			this->groupBox1->Location = System::Drawing::Point( 6, 27 );
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size( 206, 81 );
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			// 
			// lblSlots
			// 
			this->lblSlots->AutoSize = true;
			this->lblSlots->Location = System::Drawing::Point( 4, 48 );
			this->lblSlots->Name = L"lblSlots";
			this->lblSlots->Size = System::Drawing::Size( 74, 13 );
			this->lblSlots->TabIndex = 4;
			this->lblSlots->Text = L"Weapon Slots";
			// 
			// lblHR
			// 
			this->lblHR->AutoSize = true;
			this->lblHR->Location = System::Drawing::Point( 4, 22 );
			this->lblHR->Name = L"lblHR";
			this->lblHR->Size = System::Drawing::Size( 64, 13 );
			this->lblHR->TabIndex = 0;
			this->lblHR->Text = L"Quest Level";
			// 
			// grpSkills
			// 
			this->grpSkills->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom )
				| System::Windows::Forms::AnchorStyles::Left ) );
			this->grpSkills->Controls->Add( this->lblRemoveSkills );
			this->grpSkills->Controls->Add( this->lblAddSkills );
			this->grpSkills->Location = System::Drawing::Point( 6, 169 );
			this->grpSkills->Name = L"grpSkills";
			this->grpSkills->Size = System::Drawing::Size( 200, 210 );
			this->grpSkills->TabIndex = 0;
			this->grpSkills->TabStop = false;
			this->grpSkills->Text = L"Skills";
			// 
			// lblRemoveSkills
			// 
			this->lblRemoveSkills->AutoSize = true;
			this->lblRemoveSkills->Font = ( gcnew System::Drawing::Font( L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>( 0 ) ) );
			this->lblRemoveSkills->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->lblRemoveSkills->Location = System::Drawing::Point( 49, 0 );
			this->lblRemoveSkills->Name = L"lblRemoveSkills";
			this->lblRemoveSkills->Size = System::Drawing::Size( 20, 13 );
			this->lblRemoveSkills->TabIndex = 1;
			this->lblRemoveSkills->Text = L"－";
			this->tipSkills->SetToolTip( this->lblRemoveSkills, L"Click to remove Skill options" );
			this->lblRemoveSkills->Click += gcnew System::EventHandler( this, &Form1::lblRemoveSkills_Click );
			// 
			// lblAddSkills
			// 
			this->lblAddSkills->AutoSize = true;
			this->lblAddSkills->Font = ( gcnew System::Drawing::Font( L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>( 0 ) ) );
			this->lblAddSkills->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->lblAddSkills->Location = System::Drawing::Point( 34, 0 );
			this->lblAddSkills->Name = L"lblAddSkills";
			this->lblAddSkills->Size = System::Drawing::Size( 20, 13 );
			this->lblAddSkills->TabIndex = 0;
			this->lblAddSkills->Text = L"＋";
			this->tipSkills->SetToolTip( this->lblAddSkills, L"Click to add more Skill options" );
			this->lblAddSkills->Click += gcnew System::EventHandler( this, &Form1::lblAddSkills_Click );
			// 
			// btnSearch
			// 
			this->btnSearch->Location = System::Drawing::Point( 6, 12 );
			this->btnSearch->Name = L"btnSearch";
			this->btnSearch->Size = System::Drawing::Size( 107, 27 );
			this->btnSearch->TabIndex = 0;
			this->btnSearch->Text = L"&Quick Search";
			this->btnSearch->UseVisualStyleBackColor = true;
			this->btnSearch->Click += gcnew System::EventHandler( this, &Form1::btnSearch_Click );
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point( 6, 45 );
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size( 350, 10 );
			this->progressBar1->Step = 1;
			this->progressBar1->TabIndex = 6;
			// 
			// txtSolutions
			// 
			this->txtSolutions->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom )
				| System::Windows::Forms::AnchorStyles::Left )
				| System::Windows::Forms::AnchorStyles::Right ) );
			this->txtSolutions->ContextMenuStrip = this->cmsSolutions;
			this->txtSolutions->Font = ( gcnew System::Drawing::Font( L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>( 0 ) ) );
			this->txtSolutions->Location = System::Drawing::Point( 6, 16 );
			this->txtSolutions->Name = L"txtSolutions";
			this->txtSolutions->ReadOnly = true;
			this->txtSolutions->Size = System::Drawing::Size( 321, 400 );
			this->txtSolutions->TabIndex = 0;
			this->txtSolutions->Text = L"";
			this->tipResults->SetToolTip( this->txtSolutions, L"Right-click skills or armor for more info" );
			this->txtSolutions->WordWrap = false;
			this->txtSolutions->KeyDown += gcnew System::Windows::Forms::KeyEventHandler( this, &Form1::KeyDown );
			// 
			// cmsSolutions
			// 
			this->cmsSolutions->Name = L"contextMenuStrip1";
			this->cmsSolutions->Size = System::Drawing::Size( 61, 4 );
			this->cmsSolutions->Opening += gcnew System::ComponentModel::CancelEventHandler( this, &Form1::contextMenuStrip1_Opening );
			// 
			// groupBox4
			// 
			this->groupBox4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left ) );
			this->groupBox4->Controls->Add( this->btnAdvancedSearch );
			this->groupBox4->Controls->Add( this->btnCancel );
			this->groupBox4->Controls->Add( this->btnSearch );
			this->groupBox4->Controls->Add( this->progressBar1 );
			this->groupBox4->Location = System::Drawing::Point( 6, 385 );
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size( 363, 63 );
			this->groupBox4->TabIndex = 5;
			this->groupBox4->TabStop = false;
			// 
			// btnAdvancedSearch
			// 
			this->btnAdvancedSearch->Location = System::Drawing::Point( 119, 12 );
			this->btnAdvancedSearch->Name = L"btnAdvancedSearch";
			this->btnAdvancedSearch->Size = System::Drawing::Size( 137, 27 );
			this->btnAdvancedSearch->TabIndex = 1;
			this->btnAdvancedSearch->Text = L"&Advanced Search...";
			this->btnAdvancedSearch->UseVisualStyleBackColor = true;
			this->btnAdvancedSearch->Click += gcnew System::EventHandler( this, &Form1::btnAdvancedSearch_Click );
			// 
			// btnCancel
			// 
			this->btnCancel->Location = System::Drawing::Point( 262, 12 );
			this->btnCancel->Name = L"btnCancel";
			this->btnCancel->Size = System::Drawing::Size( 94, 27 );
			this->btnCancel->TabIndex = 2;
			this->btnCancel->Text = L"&Cancel";
			this->btnCancel->UseVisualStyleBackColor = true;
			this->btnCancel->Click += gcnew System::EventHandler( this, &Form1::btnCancel_Click );
			// 
			// grpResults
			// 
			this->grpResults->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom )
				| System::Windows::Forms::AnchorStyles::Left )
				| System::Windows::Forms::AnchorStyles::Right ) );
			this->grpResults->Controls->Add( this->txtSolutions );
			this->grpResults->Location = System::Drawing::Point( 373, 27 );
			this->grpResults->Name = L"grpResults";
			this->grpResults->Size = System::Drawing::Size( 333, 422 );
			this->grpResults->TabIndex = 7;
			this->grpResults->TabStop = false;
			this->grpResults->Text = L"Results";
			// 
			// btnDecorations
			// 
			this->btnDecorations->Location = System::Drawing::Point( 111, 18 );
			this->btnDecorations->Name = L"btnDecorations";
			this->btnDecorations->Size = System::Drawing::Size( 89, 23 );
			this->btnDecorations->TabIndex = 1;
			this->btnDecorations->Text = L"My &Decos...";
			this->tipDecorations->SetToolTip( this->btnDecorations, L"Manage your personal collection of Decorations to use when searching" );
			this->btnDecorations->UseVisualStyleBackColor = true;
			this->btnDecorations->Click += gcnew System::EventHandler( this, &Form1::btnDecorations_Click );
			// 
			// grpSkillFilters
			// 
			this->grpSkillFilters->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom )
				| System::Windows::Forms::AnchorStyles::Left ) );
			this->grpSkillFilters->Location = System::Drawing::Point( 210, 169 );
			this->grpSkillFilters->Name = L"grpSkillFilters";
			this->grpSkillFilters->Size = System::Drawing::Size( 159, 210 );
			this->grpSkillFilters->TabIndex = 1;
			this->grpSkillFilters->TabStop = false;
			this->grpSkillFilters->Text = L"Skill Filters";
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange( gcnew cli::array< System::Windows::Forms::ToolStripItem^  >( 4 )
			{
				this->mnuFile, this->mnuOptions,
					this->mnuLanguage, this->mnuHelp
			} );
			this->menuStrip1->Location = System::Drawing::Point( 0, 0 );
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size( 712, 24 );
			this->menuStrip1->TabIndex = 16;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// mnuFile
			// 
			this->mnuFile->DropDownItems->AddRange( gcnew cli::array< System::Windows::Forms::ToolStripItem^  >( 3 )
			{
				this->mnuLoadData,
					this->mnuSaveData, this->mnuExit
			} );
			this->mnuFile->Name = L"mnuFile";
			this->mnuFile->Size = System::Drawing::Size( 37, 20 );
			this->mnuFile->Text = L"&File";
			// 
			// mnuLoadData
			// 
			this->mnuLoadData->Name = L"mnuLoadData";
			this->mnuLoadData->Size = System::Drawing::Size( 100, 22 );
			this->mnuLoadData->Text = L"&Load";
			this->mnuLoadData->Click += gcnew System::EventHandler( this, &Form1::mnuLoad_Click );
			// 
			// mnuSaveData
			// 
			this->mnuSaveData->Name = L"mnuSaveData";
			this->mnuSaveData->Size = System::Drawing::Size( 100, 22 );
			this->mnuSaveData->Text = L"&Save";
			this->mnuSaveData->Click += gcnew System::EventHandler( this, &Form1::mnuSave_Click );
			// 
			// mnuExit
			// 
			this->mnuExit->Name = L"mnuExit";
			this->mnuExit->Size = System::Drawing::Size( 100, 22 );
			this->mnuExit->Text = L"E&xit";
			this->mnuExit->Click += gcnew System::EventHandler( this, &Form1::exitToolStripMenuItem_Click );
			// 
			// mnuOptions
			// 
			this->mnuOptions->DropDownItems->AddRange( gcnew cli::array< System::Windows::Forms::ToolStripItem^  >( 13 )
			{
				this->mnuClearSettings,
					this->toolStripSeparator1, this->mnuAllowEventArmor, this->mnuAllowArena, this->mnuAllowLowerTierArmor, this->mnuAllowFairWind,
					this->mnuAlwaysSearchAlpha, this->toolStripSeparator2, this->mnuMaxResults, this->mnuZoom, this->mnuPrintDecoNames, this->mnuPrintMaterials,
					this->mnuSortSkillsAlphabetically
			} );
			this->mnuOptions->Name = L"mnuOptions";
			this->mnuOptions->Size = System::Drawing::Size( 61, 20 );
			this->mnuOptions->Text = L"&Options";
			// 
			// mnuClearSettings
			// 
			this->mnuClearSettings->Name = L"mnuClearSettings";
			this->mnuClearSettings->Size = System::Drawing::Size( 220, 22 );
			this->mnuClearSettings->Text = L"&Clear Settings";
			this->mnuClearSettings->Click += gcnew System::EventHandler( this, &Form1::mnuClearSettings_Click );
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size( 217, 6 );
			// 
			// mnuAllowEventArmor
			// 
			this->mnuAllowEventArmor->Name = L"mnuAllowEventArmor";
			this->mnuAllowEventArmor->Size = System::Drawing::Size( 220, 22 );
			this->mnuAllowEventArmor->Text = L"Allow Event Armor";
			// 
			// mnuAllowArena
			// 
			this->mnuAllowArena->Checked = true;
			this->mnuAllowArena->CheckOnClick = true;
			this->mnuAllowArena->CheckState = System::Windows::Forms::CheckState::Checked;
			this->mnuAllowArena->Name = L"mnuAllowArena";
			this->mnuAllowArena->Size = System::Drawing::Size( 220, 22 );
			this->mnuAllowArena->Text = L"Allow &Arena Armor";
			this->mnuAllowArena->Click += gcnew System::EventHandler( this, &Form1::OptionsChanged );
			// 
			// mnuAllowLowerTierArmor
			// 
			this->mnuAllowLowerTierArmor->Checked = true;
			this->mnuAllowLowerTierArmor->CheckOnClick = true;
			this->mnuAllowLowerTierArmor->CheckState = System::Windows::Forms::CheckState::Checked;
			this->mnuAllowLowerTierArmor->Name = L"mnuAllowLowerTierArmor";
			this->mnuAllowLowerTierArmor->Size = System::Drawing::Size( 220, 22 );
			this->mnuAllowLowerTierArmor->Text = L"Allow &Lower Tier Armor";
			this->mnuAllowLowerTierArmor->Click += gcnew System::EventHandler( this, &Form1::OptionsChanged );
			// 
			// mnuAllowFairWind
			// 
			this->mnuAllowFairWind->Checked = true;
			this->mnuAllowFairWind->CheckOnClick = true;
			this->mnuAllowFairWind->CheckState = System::Windows::Forms::CheckState::Checked;
			this->mnuAllowFairWind->Name = L"mnuAllowFairWind";
			this->mnuAllowFairWind->Size = System::Drawing::Size( 220, 22 );
			this->mnuAllowFairWind->Text = L"Allow Fair &Wind Charm";
			this->mnuAllowFairWind->Click += gcnew System::EventHandler( this, &Form1::OptionsChanged );
			// 
			// mnuAlwaysSearchAlpha
			// 
			this->mnuAlwaysSearchAlpha->CheckOnClick = true;
			this->mnuAlwaysSearchAlpha->Name = L"mnuAlwaysSearchAlpha";
			this->mnuAlwaysSearchAlpha->Size = System::Drawing::Size( 220, 22 );
			this->mnuAlwaysSearchAlpha->Text = L"Always Search Alpha Armor";
			this->mnuAlwaysSearchAlpha->Click += gcnew System::EventHandler( this, &Form1::OptionsChanged );
			// 
			// toolStripSeparator2
			// 
			this->toolStripSeparator2->Name = L"toolStripSeparator2";
			this->toolStripSeparator2->Size = System::Drawing::Size( 217, 6 );
			// 
			// mnuMaxResults
			// 
			this->mnuMaxResults->DropDownItems->AddRange( gcnew cli::array< System::Windows::Forms::ToolStripItem^  >( 1 ) { this->mnuNumResults } );
			this->mnuMaxResults->Name = L"mnuMaxResults";
			this->mnuMaxResults->Size = System::Drawing::Size( 220, 22 );
			this->mnuMaxResults->Text = L"&Max Results";
			// 
			// mnuNumResults
			// 
			this->mnuNumResults->Name = L"mnuNumResults";
			this->mnuNumResults->Size = System::Drawing::Size( 100, 23 );
			this->mnuNumResults->Text = L"1000";
			// 
			// mnuZoom
			// 
			this->mnuZoom->Enabled = false;
			this->mnuZoom->Name = L"mnuZoom";
			this->mnuZoom->Size = System::Drawing::Size( 220, 22 );
			this->mnuZoom->Text = L"Preview Image &Zoom";
			// 
			// mnuPrintDecoNames
			// 
			this->mnuPrintDecoNames->Checked = true;
			this->mnuPrintDecoNames->CheckOnClick = true;
			this->mnuPrintDecoNames->CheckState = System::Windows::Forms::CheckState::Checked;
			this->mnuPrintDecoNames->Name = L"mnuPrintDecoNames";
			this->mnuPrintDecoNames->Size = System::Drawing::Size( 220, 22 );
			this->mnuPrintDecoNames->Text = L"Print &Decoration Names";
			this->mnuPrintDecoNames->Click += gcnew System::EventHandler( this, &Form1::OptionsChanged );
			// 
			// mnuPrintMaterials
			// 
			this->mnuPrintMaterials->CheckOnClick = true;
			this->mnuPrintMaterials->Name = L"mnuPrintMaterials";
			this->mnuPrintMaterials->Size = System::Drawing::Size( 220, 22 );
			this->mnuPrintMaterials->Text = L"Print &Materials";
			this->mnuPrintMaterials->Click += gcnew System::EventHandler( this, &Form1::OptionsChanged );
			// 
			// mnuSortSkillsAlphabetically
			// 
			this->mnuSortSkillsAlphabetically->Checked = true;
			this->mnuSortSkillsAlphabetically->CheckOnClick = true;
			this->mnuSortSkillsAlphabetically->CheckState = System::Windows::Forms::CheckState::Checked;
			this->mnuSortSkillsAlphabetically->Name = L"mnuSortSkillsAlphabetically";
			this->mnuSortSkillsAlphabetically->Size = System::Drawing::Size( 220, 22 );
			this->mnuSortSkillsAlphabetically->Text = L"Sort Skills &Alphabetically";
			this->mnuSortSkillsAlphabetically->Click += gcnew System::EventHandler( this, &Form1::OptionsChanged );
			// 
			// mnuLanguage
			// 
			this->mnuLanguage->Name = L"mnuLanguage";
			this->mnuLanguage->Size = System::Drawing::Size( 71, 20 );
			this->mnuLanguage->Text = L"&Language";
			// 
			// mnuHelp
			// 
			this->mnuHelp->DropDownItems->AddRange( gcnew cli::array< System::Windows::Forms::ToolStripItem^  >( 4 )
			{
				this->mnuSkillHelp,
					this->mnuDonate, this->mnuCheckForUpdates, this->mnuAbout
			} );
			this->mnuHelp->Name = L"mnuHelp";
			this->mnuHelp->Size = System::Drawing::Size( 44, 20 );
			this->mnuHelp->Text = L"&Help";
			// 
			// mnuSkillHelp
			// 
			this->mnuSkillHelp->Name = L"mnuSkillHelp";
			this->mnuSkillHelp->Size = System::Drawing::Size( 171, 22 );
			this->mnuSkillHelp->Text = L"&Skill Information";
			this->mnuSkillHelp->Click += gcnew System::EventHandler( this, &Form1::mnuSkillHelp_Click );
			// 
			// mnuDonate
			// 
			this->mnuDonate->Font = ( gcnew System::Drawing::Font( L"Segoe UI", 9, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>( 0 ) ) );
			this->mnuDonate->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->mnuDonate->Name = L"mnuDonate";
			this->mnuDonate->Size = System::Drawing::Size( 171, 22 );
			this->mnuDonate->Text = L"&Donate";
			this->mnuDonate->Click += gcnew System::EventHandler( this, &Form1::mnuDonate_Click );
			// 
			// mnuCheckForUpdates
			// 
			this->mnuCheckForUpdates->Font = ( gcnew System::Drawing::Font( L"Segoe UI", 9, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>( 0 ) ) );
			this->mnuCheckForUpdates->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->mnuCheckForUpdates->Name = L"mnuCheckForUpdates";
			this->mnuCheckForUpdates->Size = System::Drawing::Size( 171, 22 );
			this->mnuCheckForUpdates->Text = L"Check for &Updates";
			this->mnuCheckForUpdates->Click += gcnew System::EventHandler( this, &Form1::UpdateMenuItem_Click );
			// 
			// mnuAbout
			// 
			this->mnuAbout->Name = L"mnuAbout";
			this->mnuAbout->Size = System::Drawing::Size( 171, 22 );
			this->mnuAbout->Text = L"&About";
			this->mnuAbout->Click += gcnew System::EventHandler( this, &Form1::aboutToolStripMenuItem_Click );
			// 
			// grpGender
			// 
			this->grpGender->Controls->Add( this->rdoFemale );
			this->grpGender->Controls->Add( this->rdoMale );
			this->grpGender->Location = System::Drawing::Point( 6, 114 );
			this->grpGender->Name = L"grpGender";
			this->grpGender->Size = System::Drawing::Size( 154, 49 );
			this->grpGender->TabIndex = 2;
			this->grpGender->TabStop = false;
			this->grpGender->Text = L"Gender";
			// 
			// rdoFemale
			// 
			this->rdoFemale->AutoSize = true;
			this->rdoFemale->Checked = true;
			this->rdoFemale->Location = System::Drawing::Point( 83, 21 );
			this->rdoFemale->Name = L"rdoFemale";
			this->rdoFemale->Size = System::Drawing::Size( 59, 17 );
			this->rdoFemale->TabIndex = 1;
			this->rdoFemale->TabStop = true;
			this->rdoFemale->Text = L"Female";
			this->rdoFemale->UseVisualStyleBackColor = true;
			// 
			// rdoMale
			// 
			this->rdoMale->AutoSize = true;
			this->rdoMale->Location = System::Drawing::Point( 13, 21 );
			this->rdoMale->Name = L"rdoMale";
			this->rdoMale->Size = System::Drawing::Size( 48, 17 );
			this->rdoMale->TabIndex = 0;
			this->rdoMale->TabStop = true;
			this->rdoMale->Text = L"Male";
			this->rdoMale->UseVisualStyleBackColor = true;
			// 
			// grpSortFilter
			// 
			this->grpSortFilter->Controls->Add( this->cmbFilterByExtraSkill );
			this->grpSortFilter->Controls->Add( this->cmbSort );
			this->grpSortFilter->Location = System::Drawing::Point( 216, 27 );
			this->grpSortFilter->Name = L"grpSortFilter";
			this->grpSortFilter->Size = System::Drawing::Size( 153, 81 );
			this->grpSortFilter->TabIndex = 1;
			this->grpSortFilter->TabStop = false;
			this->grpSortFilter->Text = L"Sort/Filter Results";
			// 
			// cmbFilterByExtraSkill
			// 
			this->cmbFilterByExtraSkill->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left )
				| System::Windows::Forms::AnchorStyles::Right ) );
			this->cmbFilterByExtraSkill->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbFilterByExtraSkill->FormattingEnabled = true;
			this->cmbFilterByExtraSkill->Items->AddRange( gcnew cli::array< System::Object^  >( 1 ) { L"No extra skill filtering" } );
			this->cmbFilterByExtraSkill->Location = System::Drawing::Point( 6, 46 );
			this->cmbFilterByExtraSkill->Name = L"cmbFilterByExtraSkill";
			this->cmbFilterByExtraSkill->Size = System::Drawing::Size( 141, 21 );
			this->cmbFilterByExtraSkill->TabIndex = 2;
			this->tipSortFilter->SetToolTip( this->cmbFilterByExtraSkill, L"Select an extra skill to filter out results which lack that skill" );
			this->cmbFilterByExtraSkill->SelectedIndexChanged += gcnew System::EventHandler( this, &Form1::cmbFilterByExtraSkill_SelectedIndexChanged );
			// 
			// cmbSort
			// 
			this->cmbSort->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left )
				| System::Windows::Forms::AnchorStyles::Right ) );
			this->cmbSort->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbSort->FormattingEnabled = true;
			this->cmbSort->Items->AddRange( gcnew cli::array< System::Object^  >( 14 )
			{
				L"None", L"Dragon res", L"Fire res", L"Ice res",
					L"Thunder res", L"Water res", L"Base defence", L"Max defence", L"Aug defence", L"Difficulty", L"Rarity", L"Slots spare", L"Family",
					L"Extra Skills"
			} );
			this->cmbSort->Location = System::Drawing::Point( 6, 20 );
			this->cmbSort->Name = L"cmbSort";
			this->cmbSort->Size = System::Drawing::Size( 141, 21 );
			this->cmbSort->TabIndex = 0;
			this->tipSortFilter->SetToolTip( this->cmbSort, L"Select criteria to sort results by" );
			this->cmbSort->SelectedIndexChanged += gcnew System::EventHandler( this, &Form1::cmbSort_SelectedIndexChanged );
			// 
			// grpDecorations
			// 
			this->grpDecorations->Controls->Add( this->cmbDecorationSelect );
			this->grpDecorations->Controls->Add( this->btnDecorations );
			this->grpDecorations->Location = System::Drawing::Point( 164, 114 );
			this->grpDecorations->Name = L"grpDecorations";
			this->grpDecorations->Size = System::Drawing::Size( 205, 49 );
			this->grpDecorations->TabIndex = 3;
			this->grpDecorations->TabStop = false;
			this->grpDecorations->Text = L"Decorations";
			// 
			// cmbDecorationSelect
			// 
			this->cmbDecorationSelect->ContextMenuStrip = this->cmsCharms;
			this->cmbDecorationSelect->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbDecorationSelect->FormattingEnabled = true;
			this->cmbDecorationSelect->Items->AddRange( gcnew cli::array< System::Object^  >( 3 ) { L"Use none", L"Use mine", L"Use all" } );
			this->cmbDecorationSelect->Location = System::Drawing::Point( 6, 19 );
			this->cmbDecorationSelect->Name = L"cmbDecorationSelect";
			this->cmbDecorationSelect->Size = System::Drawing::Size( 101, 21 );
			this->cmbDecorationSelect->TabIndex = 0;
			this->tipDecorations->SetToolTip( this->cmbDecorationSelect, L"Select which Decorations to use when searching" );
			// 
			// cmsCharms
			// 
			this->cmsCharms->Name = L"cmsCharms";
			this->cmsCharms->Size = System::Drawing::Size( 61, 4 );
			// 
			// cmsSkills
			// 
			this->cmsSkills->Items->AddRange( gcnew cli::array< System::Windows::Forms::ToolStripItem^  >( 1 ) { this->mnuClearSkill } );
			this->cmsSkills->Name = L"cmsSkills";
			this->cmsSkills->Size = System::Drawing::Size( 102, 26 );
			// 
			// mnuClearSkill
			// 
			this->mnuClearSkill->Name = L"mnuClearSkill";
			this->mnuClearSkill->Size = System::Drawing::Size( 101, 22 );
			this->mnuClearSkill->Text = L"&Clear";
			// 
			// tipWeaponSlots
			// 
			this->tipWeaponSlots->ToolTipTitle = L"Weapon Slots";
			// 
			// tipSkills
			// 
			this->tipSkills->ToolTipTitle = L"Skills";
			// 
			// tipQuestLevel
			// 
			this->tipQuestLevel->ToolTipTitle = L"Quest Level";
			// 
			// tipResults
			// 
			this->tipResults->ToolTipTitle = L"Results";
			// 
			// tipSortFilter
			// 
			this->tipSortFilter->ToolTipTitle = L"Sort/Filter Results";
			// 
			// tipDecorations
			// 
			this->tipDecorations->ToolTipTitle = L"Decorations";
			// 
			// nudWeaponSlots1
			// 
			this->nudWeaponSlots1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right ) );
			this->nudWeaponSlots1->BackColor = System::Drawing::SystemColors::Control;
			this->nudWeaponSlots1->Location = System::Drawing::Point( 79, 46 );
			this->nudWeaponSlots1->Maximum = System::Decimal( gcnew cli::array< System::Int32 >( 4 ) { 3, 0, 0, 0 } );
			this->nudWeaponSlots1->Name = L"nudWeaponSlots1";
			this->nudWeaponSlots1->Size = System::Drawing::Size( 40, 20 );
			this->nudWeaponSlots1->TabIndex = 7;
			this->nudWeaponSlots1->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tipWeaponSlots->SetToolTip( this->nudWeaponSlots1, L"Set the level of Weapon Slot 1" );
			this->nudWeaponSlots1->ValueChanged += gcnew System::EventHandler( this, &Form1::nudWeaponSlots1_ValueChanged );
			// 
			// nudWeaponSlots2
			// 
			this->nudWeaponSlots2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right ) );
			this->nudWeaponSlots2->BackColor = System::Drawing::SystemColors::Control;
			this->nudWeaponSlots2->Enabled = false;
			this->nudWeaponSlots2->Location = System::Drawing::Point( 120, 46 );
			this->nudWeaponSlots2->Maximum = System::Decimal( gcnew cli::array< System::Int32 >( 4 ) { 3, 0, 0, 0 } );
			this->nudWeaponSlots2->Name = L"nudWeaponSlots2";
			this->nudWeaponSlots2->Size = System::Drawing::Size( 40, 20 );
			this->nudWeaponSlots2->TabIndex = 6;
			this->nudWeaponSlots2->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tipWeaponSlots->SetToolTip( this->nudWeaponSlots2, L"Set the level of Weapon Slot 2" );
			this->nudWeaponSlots2->ValueChanged += gcnew System::EventHandler( this, &Form1::nudWeaponSlots2_ValueChanged );
			// 
			// nudWeaponSlots3
			// 
			this->nudWeaponSlots3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right ) );
			this->nudWeaponSlots3->BackColor = System::Drawing::SystemColors::Control;
			this->nudWeaponSlots3->Enabled = false;
			this->nudWeaponSlots3->Location = System::Drawing::Point( 161, 46 );
			this->nudWeaponSlots3->Maximum = System::Decimal( gcnew cli::array< System::Int32 >( 4 ) { 3, 0, 0, 0 } );
			this->nudWeaponSlots3->Name = L"nudWeaponSlots3";
			this->nudWeaponSlots3->Size = System::Drawing::Size( 40, 20 );
			this->nudWeaponSlots3->TabIndex = 5;
			this->nudWeaponSlots3->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tipWeaponSlots->SetToolTip( this->nudWeaponSlots3, L"Set the level of Weapon Slot 3" );
			// 
			// nudHR
			// 
			this->nudHR->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right ) );
			this->nudHR->BackColor = System::Drawing::SystemColors::Control;
			this->nudHR->Location = System::Drawing::Point( 161, 20 );
			this->nudHR->Maximum = System::Decimal( gcnew cli::array< System::Int32 >( 4 ) { 9, 0, 0, 0 } );
			this->nudHR->Minimum = System::Decimal( gcnew cli::array< System::Int32 >( 4 ) { 1, 0, 0, 0 } );
			this->nudHR->Name = L"nudHR";
			this->nudHR->Size = System::Drawing::Size( 40, 20 );
			this->nudHR->TabIndex = 1;
			this->nudHR->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tipQuestLevel->SetToolTip( this->nudHR, L"Set level of quests you have access to" );
			this->nudHR->Value = System::Decimal( gcnew cli::array< System::Int32 >( 4 ) { 9, 0, 0, 0 } );
			this->nudHR->ValueChanged += gcnew System::EventHandler( this, &Form1::nudHR_ValueChanged );
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF( 6, 13 );
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size( 712, 457 );
			this->Controls->Add( this->grpSkills );
			this->Controls->Add( this->grpSkillFilters );
			this->Controls->Add( this->grpSortFilter );
			this->Controls->Add( this->grpResults );
			this->Controls->Add( this->groupBox1 );
			this->Controls->Add( this->menuStrip1 );
			this->Controls->Add( this->grpDecorations );
			this->Controls->Add( this->grpGender );
			this->Controls->Add( this->groupBox4 );
			this->Icon = ( cli::safe_cast<System::Drawing::Icon^>( resources->GetObject( L"$this.Icon" ) ) );
			this->Name = L"Form1";
			this->Text = L"Athena\'s ASS for ";
			this->groupBox1->ResumeLayout( false );
			this->groupBox1->PerformLayout();
			this->grpSkills->ResumeLayout( false );
			this->grpSkills->PerformLayout();
			this->groupBox4->ResumeLayout( false );
			this->grpResults->ResumeLayout( false );
			this->menuStrip1->ResumeLayout( false );
			this->menuStrip1->PerformLayout();
			this->grpGender->ResumeLayout( false );
			this->grpGender->PerformLayout();
			this->grpSortFilter->ResumeLayout( false );
			this->grpDecorations->ResumeLayout( false );
			this->cmsSkills->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->nudWeaponSlots1 ) )->EndInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->nudWeaponSlots2 ) )->EndInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->nudWeaponSlots3 ) )->EndInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->nudHR ) )->EndInit();
			this->ResumeLayout( false );
			this->PerformLayout();

		}
#pragma endregion

private: 
	System::Void FormulateQuery( List_t< Skill^ >^ skills )
	{
		query = gcnew Query;
		for( int i = 0; i < int( Armor::ArmorType::NumArmorTypes ); ++i )
		{
			query->rel_armor.Add( gcnew List_t< Armor^ >() );
			query->inf_armor.Add( gcnew List_t< Armor^ >() );
		}
		
		query->weapon_slots = gcnew array< unsigned >( 4 );
		query->weapon_slots[ (int)nudWeaponSlots1->Value ]++;
		query->weapon_slots[ (int)nudWeaponSlots2->Value ]++;
		query->weapon_slots[ (int)nudWeaponSlots3->Value ]++;
		query->weapon_slots[ 0 ] = 0;

		query->hr = int( nudHR->Value );
		query->gender = rdoMale->Checked ? Gender::MALE : Gender::FEMALE;
		query->include_arena = mnuAllowArena->Checked;
		query->allow_lower_tier = mnuAllowLowerTierArmor->Checked;
		query->allow_fair_wind = mnuAllowFairWind->Checked;
		query->always_search_alpha = mnuAlwaysSearchAlpha->Checked;
		query->no_decos = cmbDecorationSelect->SelectedIndex == 0;
		query->my_decos = cmbDecorationSelect->SelectedIndex == 1;
		
		query->skills.AddRange( skills );
		query->total_skill_points_required = 0;
		for each( Skill^ skill in skills )
		{
			query->total_skill_points_required += skill->level;
		}

		data->GetRelevantData( query );
	}

	System::Void FormulateQuery()
	{
		List_t< Skill^ > skills;

		for( int i = 0; i < NumSkills; ++i )
			if( Skills[ i ]->SelectedIndex >= 0 )
			{
				IndexMap^ imap = IndexMaps[ i ];
				skills.Add( data->FindSkill( imap[ Skills[ i ]->SelectedIndex ] ) );
			}

		FormulateQuery( %skills );
	}

	System::Void UpdateImpossibleSkills()
	{
		if( Skill::UpdateImpossibleSkills( (int)nudHR->Value ) )
		{
			for each( ComboBox^ cb in SkillFilters )
				cmbSkillFilter_SelectedIndexChanged( cb, nullptr );
		}
	}

	System::Void EventArmorMenuClosing( System::Object^ sender, ToolStripDropDownClosingEventArgs^ e )
	{
		if( e->CloseReason == ToolStripDropDownCloseReason::ItemClicked )
			e->Cancel = true;
	}

	System::Void MaxResultsTextBoxKeyPress( System::Object^ sender, KeyPressEventArgs^ e )
	{
		//ignore all keypresses that aren't numeric digits or control chars
		if( !Char::IsControl( e->KeyChar ) &&
			!Char::IsDigit( e->KeyChar ) )
			e->Handled = true;
	}

	System::Void MaxResultsTextChanged( System::Object^ sender, EventArgs^ e )
	{
		try
		{
			MAX_LIMIT = Convert::ToUInt32( mnuNumResults->Text );
		}
		catch( Exception^ )
		{
			return;
		}
		SaveConfig();
	}

	System::Void OptionsChanged( System::Object^ sender, System::EventArgs^ e )
	{
		SaveConfig();
		
		if( sender == mnuPrintDecoNames ||
			sender == mnuPrintMaterials )
			UpdateResultString();

		else if( sender == mnuSortSkillsAlphabetically )
		{
			for each( ComboBox^ cb in SkillFilters )
				cmbSkillFilter_SelectedIndexChanged( cb, nullptr );
		}
	}

	System::Void QueueTask( Query^ query, Charm^ ct )
	{
		BackgroundWorker^ new_thread = gcnew BackgroundWorker;
		new_thread->WorkerSupportsCancellation = true;
		new_thread->WorkerReportsProgress = true;
		new_thread->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &Form1::backgroundWorker1_DoWork);
		new_thread->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &Form1::backgroundWorker1_RunWorkerCompleted);
		new_thread->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(this, &Form1::backgroundWorker1_ProgressChanged);

		ThreadSearchData^ tsd = gcnew ThreadSearchData;
		tsd->charm = ct;
		tsd->query = query;
		worker_data.Add( tsd );

		workers.Add( new_thread );
	}

	System::Void QueueTask2( Query^ query, array< Armor^ >^ armors, List_t< Charm^ >^ charms )
	{
		BackgroundWorker^ new_thread = gcnew BackgroundWorker;
		new_thread->WorkerSupportsCancellation = true;
		new_thread->WorkerReportsProgress = true;
		new_thread->DoWork += gcnew System::ComponentModel::DoWorkEventHandler( this, &Form1::backgroundWorker1_DoWork2 );
		new_thread->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler( this, &Form1::backgroundWorker1_RunWorkerCompleted );
		new_thread->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler( this, &Form1::backgroundWorker1_ProgressChanged );

		ThreadSearchData2^ tsd = gcnew ThreadSearchData2;
		tsd->charms = charms;
		tsd->query = query;
		tsd->armors = armors;
		worker_data.Add( tsd );

		workers.Add( new_thread );
	}

	void EnableSearchControls( const bool enabled )
	{
		btnSearch->Enabled = enabled;
		btnAdvancedSearch->Enabled = enabled;
		btnCancel->Enabled = !enabled;
		btnDecorations->Enabled = enabled;
		mnuMaxResults->Enabled = enabled;
		mnuNumResults->Enabled = enabled;
		cmbSort->Enabled = enabled;
		cmbFilterByExtraSkill->Enabled = enabled;
	}

	void StartTasks()
	{
		worker_mutex->WaitOne();

		worker_start_index = Math::Max( 1, Math::Min( Environment::ProcessorCount, workers.Count ) );

		if( workers.Count > 0 )
			for( unsigned i = 0; i < worker_start_index; ++i )
				workers[ i ]->RunWorkerAsync( worker_data[ i ] );

		worker_mutex->ReleaseMutex();
	}

	System::Void StartSearch()
	{
		progressBar1->Value = 0;
		total_progress = 0;
		num_updates = 0;
		search_cancelled = false;
		if( query->skills.Count > 0 )
		{
			EnableSearchControls( false );
			txtSolutions->Text = StartString( SolutionsFound ) + L"0";
			final_solutions.Clear();
			all_solutions.Clear();
			solution_hashes.Clear();
			workers.Clear();
			worker_data.Clear();
			last_result = nullptr;
			finished_workers = 0;

			for each( Charm^ charm in query->rel_charms )
			{
				QueueTask( query, charm );
			}

			QueueTask( query, nullptr );

			for each( array< Armor^ >^ set in Armor::static_full_sets )
			{
				if( set && set[0] && set[0]->MatchesQuery( query, true ) )
					QueueTask2( query, set, %query->rel_charms );
			}

			StartTasks();
		}
	}

	System::Void btnAdvancedSearch_Click( System::Object^ sender, System::EventArgs^ e )
	{
		FormulateQuery();

		Advanced advanced_search( query );
		advanced_search.Width = adv_x;
		advanced_search.Height = adv_y;
		advanced_search.ShowDialog( this );

		adv_x = advanced_search.Width;
		adv_y = advanced_search.Height;
		SaveConfig();

		if( advanced_search.DialogResult != System::Windows::Forms::DialogResult::OK )
			return;

		for( int p = 0; p < int( Armor::ArmorType::NumArmorTypes ); ++p )
		{
/*#ifdef _DEBUG
			List_t< String^ > old, nnu;
			for each( Armor^ a in query->rel_armor[ p ] )
				old.Add( a->name );
#endif*/

			query->rel_armor[ p ]->Clear();
			List_t< Armor^ >^ ilist = query->inf_armor[ p ];
			query->inf_armor[ p ] = gcnew List_t< Armor^ >();
			/*for( int i = 0; i < ilist->Count; ++i )
			{
				if( !ilist[ i ]->force_disable )
					AddToList( query->rel_armor[ p ], ilist[ i ], %query->rel_abilities, query->inf_armor[ p ], false );
			}*/

			data->GetRelevantArmors( query, query->rel_armor[ p ], Armor::static_armors[ p ], query->inf_armor[ p ], true );

/*#ifdef _DEBUG
			for each( Armor^ a in query->rel_armor[ p ] )
				nnu.Add( a->name );

			for( int i = 0; i < ilist->Count; ++i )
			{
				const bool checked = advanced_search.boxes[ p ]->Items[ i ]->Checked;
				const bool used = Utility::Contains( query->rel_armor[ p ], ilist[ i ] ) || Utility::Contains( query->inf_armor[ p ], ilist[ i ] );
				Assert( checked == used, L"Advanced search armor usage not as expected" );
			}
#endif*/
		}
		query->rel_charms.Clear();
		
		for( int i = 0; i < query->inf_charms.Count; ++i )
		{
			if( advanced_search.boxes[ int( Armor::ArmorType::NumArmorTypes ) ]->Items[ i ]->Checked )
				query->rel_charms.Add( query->inf_charms[ i ] );
		}

		StartSearch();
	}

	System::Void btnSearch_Click( System::Object^ sender, System::EventArgs^ e )
	{		
		FormulateQuery();
		
		StartSearch();
	}

	System::Void btnCancel_Click( System::Object^ sender, System::EventArgs^ e )
	{
		search_cancelled = true;
		for each( BackgroundWorker^ worker in workers )
			worker->CancelAsync();
		EnableSearchControls( true );
		progressBar1->Value = 0;
	}

	System::Void cmbSkillFilter_SelectedIndexChanged( System::Object^ sender, List_t< ComboBox^ >% skills, List_t< ComboBox^ >% skill_filters, List_t< IndexMap^ >% index_maps )
	{
		List_t< Ability^ > old_skills;
		int index = -1;
		Skill^ selected_skill = nullptr;
		for( int i = 0; i < NumSkills; ++i )
		{
			if( sender == skill_filters[ i ] )
				index = i;
			//Assert( skills[ i ]->SelectedIndex != 0, L"What happen" );
			if( skills[ i ]->SelectedIndex < 1 )
				continue;

			IndexMap^ im = index_maps[ i ];
			Skill^ skill = Skill::static_skills[ im[ skills[ i ]->SelectedIndex ] ];
			if( sender == skill_filters[ i ] )
				selected_skill = skill;
			else old_skills.Add( skill->ability );
		}
		if( index == -1 )
			return;

		if( skill_filters[ index ]->SelectedIndex == 2 )
			FindRelatedSkills( skills, skill_filters, index_maps );

		skills[ index ]->BeginUpdate();
		lock_related = true;
		InitSkills( skills[ index ], index_maps[ index ], skill_filters[ index ]->SelectedIndex, %old_skills );
		ResetSkill( skills[ index ], index_maps[ index ], selected_skill );
		lock_related = false;
		skills[ index ]->EndUpdate();
	}

	System::Void cmbSkillFilter_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ )
	{
		cmbSkillFilter_SelectedIndexChanged( sender, Skills, SkillFilters, IndexMaps );
	}

	void cmbSkill_SelectedIndexChanged( System::Object^ sender, List_t< ComboBox^ >% skills, List_t< ComboBox^ >% skill_filters, List_t< IndexMap^ >% index_maps, List_t< Ability^ >% last_abilities )
	{
		int index = -1;
		for( int i = 0; i < NumSkills; ++i )
		{
			if( sender == skills[ i ] )
			{
				index = i;
				break;
			}
		}
		if( index == -1 )
			return;

		Skill^ selected_skill = nullptr;
		if( skills[ index ]->SelectedIndex < 1 )
		{
			last_abilities[ index ] = nullptr;
		}
		else
		{
			IndexMap^ imap = index_maps[ index ];
			selected_skill = Skill::static_skills[ imap[ skills[ index ]->SelectedIndex ] ];
			
			//if( last_abilities[ index ] == selected_skill->ability )
			//	return;

			last_abilities[ index ] = selected_skill->ability;
		}

		lock_skills = true;

		if( skills[ index ]->SelectedIndex == 0 && !skills[ index ]->DroppedDown )
		{
			SuspendDrawing( skills[ index ] );
			skills[ index ]->SelectedIndex = -1;
			ResumeDrawing( skills[ index ] );
		}

		if( !lock_related )
			FindRelatedSkills( skills, skill_filters, index_maps );
		
		for( int i = 0; i < NumSkills; ++i )
		{
			if( i == index && skill_filters[ i ]->SelectedIndex != 2 )
				continue;

			IndexMap^ imap = index_maps[ i ];
			Skill^ skill = skills[ i ]->SelectedIndex <= 0 ? nullptr : Skill::static_skills[ imap[ skills[ i ]->SelectedIndex ] ];
			List_t< Ability^ > old_skills;
			for( int j = 0; j < NumSkills; ++j )
			{
				if( skills[ j ]->SelectedIndex <= 0 )
					continue;

				IndexMap^ jmap = index_maps[ j ];
				Skill^ os = Skill::static_skills[ jmap[ skills[ j ]->SelectedIndex ] ];
				if( j != i )
					old_skills.Add( os->ability );


				if( i == index && i == j )
					continue;
			}

			skills[ i ]->BeginUpdate();
			InitSkills( skills[ i ], index_maps[ i ], skill_filters[ i ]->SelectedIndex, %old_skills );
			ResetSkill( skills[ i ], index_maps[ i ], skill );
			skills[ i ]->EndUpdate();
		}
		lock_skills = false;
		return;
	}

	System::Void cmbSkill_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e ) 
	{
		if( lock_skills )
			return;

		cmbSkill_SelectedIndexChanged( sender, Skills, SkillFilters, IndexMaps, last_selected_ability );
	}

	unsigned UpdateResultDecorations( System::Text::StringBuilder% sb, List_t< Decoration^ >% decorations, String^ suffix )
	{
		Generic::Dictionary< Decoration^, unsigned > deco_dict;
		for each( Decoration^ decoration in decorations )
		{
			if( !deco_dict.ContainsKey( decoration ) )
				deco_dict.Add( decoration, 1 );
			else deco_dict[ decoration ]++;
		}
		
		Generic::Dictionary< Decoration^, unsigned >::Enumerator iter = deco_dict.GetEnumerator();
		while( iter.MoveNext() )
		{
			sb.Append( Convert::ToString( iter.Current.Value ) )->Append( L"x " );
			if( mnuPrintDecoNames->Checked )
				sb.AppendLine( iter.Current.Key->name + suffix );
			else
			{
				sb.AppendLine( FormatString1( JewelFormat, iter.Current.Key->abilities[ 0 ]->ability->name ) + suffix );
			}
		}
		return deco_dict.Count;
	}

	System::Void UpdateResultString( List_t< Solution^ >^ solutions )
	{
		if( !StringTable::text )
			return;

		result_offsets.Clear();
		unsigned offset = 1;
		if( solutions->Count > MAX_LIMIT )
			offset++;

		System::Text::StringBuilder sb( solutions->Count * 1024 );

		if( last_result )
			sb.Append( last_result );
		
		System::String^ dash = L"-----------------";
		int count = 0;
		for each( Solution^ solution in solutions )
		{
			if( solution->HasDLCDisabledArmor() )
				continue;

			if( ++count > MAX_LIMIT )
				break;

			sb.Append( endl );
			result_offsets.Add( ++offset );
		
			for each( Armor^ armor in solution->armors )
			{
				if( armor )
				{
					sb.Append( armor->name );
					if( armor->total_relevant_skill_points == 0 )
					{
						//sb.Append( StaticString( JustForTheSlots ) );
					}

					sb.Append( endl );
					offset++;
				}
			}
			if( solution->charm )
			{
				sb.AppendLine( dash );
				sb.AppendLine( solution->charm->name );
				offset += 2;
			}
			if( solution->decorations.Count > 0 )
			{
				sb.AppendLine( dash );
				offset++;

				offset += UpdateResultDecorations( sb, solution->decorations, "" );
			}
			if( solution->total_slots_spare > 0 || cmbSort->SelectedIndex == 11 )
			{
				if( solution->total_slots_spare == 0 )
				{
					sb.AppendLine( FormatString1( SlotsSpare, L"0" ) );
				}
				else
				{
					sb.Append( FormatStringN( SlotsSpare, solution->total_slots_spare ) );
					sb.Append( L" (" );
					bool first = true;
					for( unsigned i = 1; i <= 3; ++i )
						for( unsigned j = 0; j < solution->slots_spare[ i ]; ++j )
						{
							sb.Append( ( first ? L"" : L"+" ) + Convert::ToString( i ) );
							first = false;
						}
					sb.AppendLine( L")" );
				}
				offset++;
			}
			if( cmbSort->SelectedIndex > 0 && cmbSort->SelectedIndex < 10 )
			{
				if( cmbSort->SelectedIndex < 9 )
				{
					sb.AppendLine( dash );
					if( int( nudHR->Value ) >= 9 )
						sb.AppendLine( FormatString7( DefEleAbbrev, solution->defence, solution->aug_defence, solution->fire_res, solution->water_res, solution->ice_res, solution->thunder_res, solution->dragon_res ) );
					else
						sb.AppendLine( FormatString7( DefEleAbbrev, solution->defence, solution->max_defence, solution->fire_res, solution->water_res, solution->ice_res, solution->thunder_res, solution->dragon_res ) );
					offset += 2;
				}
				else
				{
					if( cmbSort->SelectedIndex == 8 ) sb.Append( solution->difficulty );
					else if( cmbSort->SelectedIndex == 9 ) sb.Append( solution->rarity );
					sb.Append( L" " )->AppendLine( (String^)cmbSort->SelectedItem );
					offset++;
				}
			}
			if( solution->extra_skills.Count > 0 )
			{
				sb.AppendLine( dash );
				offset++;
				for each( Skill^ skill in solution->extra_skills )
				{
					if( !skill->ability->set_ability && !Utility::Contains( %query->skills, skill ) )
					{
						sb.AppendLine( skill->name );
						offset++;
					}
				}
			}
			if( cmbFilterByExtraSkill->SelectedIndex > 0 && cmbFilterByExtraSkill->SelectedIndex - 1 < solutions_extra_skills.Count )
			{
				Skill^ selected_extra_skill = solutions_extra_skills[ cmbFilterByExtraSkill->SelectedIndex - 1 ]->skill;
				if( !Utility::Contains( %solution->extra_skills, selected_extra_skill ) && Utility::Contains( %solution->potential_extra_skills, selected_extra_skill ) )
				{
					sb.AppendLine( FormatString1( SkillPossible, selected_extra_skill->name ) );
					offset++;
				}
			}

			if( mnuPrintMaterials->Checked )
			{
				sb.AppendLine( dash );
				offset++;
				Generic::SortedDictionary< String^, int > materials;
				for each( Armor^ armor in solution->armors )
					for each( MaterialComponent^ mat in armor->components )
					{
						if( materials.ContainsKey( mat->material->name ) )
							materials[ mat->material->name ] += mat->amount;
						else
							materials.Add( mat->material->name, mat->amount );
					}
				for each( Decoration^ deco in solution->decorations )
					for each( MaterialComponent^ mat in deco->components )
					{
						if( materials.ContainsKey( mat->material->name ) )
							materials[ mat->material->name ] += mat->amount;
						else
							materials.Add( mat->material->name, mat->amount );
					}

				Generic::SortedDictionary< String^, int >::Enumerator e = materials.GetEnumerator();
				while( e.MoveNext() )
				{
					sb.Append( e.Current.Value )->Append( L"x " )->AppendLine( e.Current.Key );
					offset++;
				}
			}
		}

		if( %final_solutions != solutions )
			final_solutions.AddRange( solutions );

		System::Text::StringBuilder sb2;
		sb2.Append( StartString( SolutionsFound ) )->AppendLine( Convert::ToString( count ) );

		if( solutions->Count > MAX_LIMIT )
		{
			sb2.AppendLine( FormatStringN( ShowingFirstSolutionsOnly, MAX_LIMIT ) );
		}

		sb2.Append( last_result = sb.ToString() );
		
		results_mutex->WaitOne();
		{
			//this is ensure that entire text box is the correct font (if you change language)
			SuspendDrawing( txtSolutions );
			txtSolutions->Text = sb2.ToString();
			txtSolutions->SelectionStart = 0;
			txtSolutions->SelectionLength = txtSolutions->Text->Length;
			txtSolutions->SelectionFont = gcnew Drawing::Font( L"Microsoft Sans Serif", txtSolutions->Font->Size );
			txtSolutions->SelectionLength = 0;
			ResumeDrawing( txtSolutions );
		}
		results_mutex->ReleaseMutex();
	}

#pragma region Worker Thread Stuff

	System::Void AddSolution( Solution^ sol )
	{
		const auto hash = sol->GetHash();
		if( !solution_hashes.ContainsKey( hash ) )
		{
			solution_hashes.Add( hash, 0 );
			all_solutions.Add( sol );
		}

		for each( Solution^ swap in sol->armor_swaps )
		{
			AddSolution( swap );
		}
	}

	System::Void AddSolutions( List_t< Solution^ >^ solutions )
	{
		all_solutions_mutex->WaitOne();
		for each( Solution^ sol in solutions )
		{
			if( all_solutions.Count >= MaxSolutions )
			{
				all_solutions_mutex->ReleaseMutex();
				search_cancelled = true;
				for each( BackgroundWorker^ worker in workers )
					worker->CancelAsync();
				return;
			}
			else AddSolution( sol );
		}
		all_solutions_mutex->ReleaseMutex();
	}

	System::Void cmbFilterByExtraSkill_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
	{
		if( solutions_extra_skills.Count == 0 || updating_extra_skills || updating_language )
			return;

		UpdateResultString();
	}

	System::Void UpdateExtraSkillCombo( const bool preserve_selected_index )
	{
		updating_extra_skills = true;

		Skill^ prev_selected = nullptr;
		if( preserve_selected_index && cmbFilterByExtraSkill->SelectedIndex > 0 && cmbFilterByExtraSkill->SelectedIndex - 1 < solutions_extra_skills.Count )
		{
			prev_selected = solutions_extra_skills[ cmbFilterByExtraSkill->SelectedIndex - 1 ]->skill;
		}

		for each( Skill^ skill in Skill::static_skills )
		{
			skill->extra = false;
		}

		cmbFilterByExtraSkill->BeginUpdate();
		
		solutions_extra_skills.Clear();
		cmbFilterByExtraSkill->Items->Clear();
		cmbFilterByExtraSkill->Items->Add( StaticString( NoExtraSkillFiltering ) );

		for each( Solution^ sol in all_solutions )
		{
			for each( Skill^ sk in sol->extra_skills )
			{
				if( !sk->ability->set_ability && !sk->extra )
				{
					if( ExtraSkillFilters::filter_rules.ContainsKey( sk->ability ) && !ExtraSkillFilters::filter_rules[ sk->ability ]->IsRelevant() )
						continue;
					
					solutions_extra_skills.Add( gcnew ExtraSkill( sk, true ) );
					sk->extra = true;
				}
			}
			for each( Skill^ sk in sol->potential_extra_skills )
			{
				if( !sk->ability->set_ability && !sk->extra )
				{
					if( ExtraSkillFilters::filter_rules.ContainsKey( sk->ability ) && !ExtraSkillFilters::filter_rules[ sk->ability ]->IsRelevant() )
						continue;

					solutions_extra_skills.Add( gcnew ExtraSkill( sk, true ) );
					sk->extra = true;
				}
			}
		}

		for each( FilterRules^ filter in ExtraSkillFilters::extra_filters )
		{
			if( filter->IsRelevant() )
			{
				Skill^ lowest_skill = filter->ability->GetWorstGoodSkill();
				solutions_extra_skills.Add( gcnew ExtraSkill( lowest_skill, false ) );
				lowest_skill->extra = true;
			}
		}

		solutions_extra_skills.Sort( gcnew Comparison< ExtraSkill^ >( CompareExtraSkills ) );

		for( int i = 0; i < solutions_extra_skills.Count; ++i )
		{
			ExtraSkill^ sk = solutions_extra_skills[ i ];
			if( sk->want )
				cmbFilterByExtraSkill->Items->Add( sk->skill->name );
			else
				cmbFilterByExtraSkill->Items->Add( FormatString1( ExcludeSkill , sk->skill->ability->name ) );
		}

		if( prev_selected )
		{
			for( int i = 0; i < solutions_extra_skills.Count; ++i )
			{
				if( solutions_extra_skills[ i ]->skill == prev_selected )
				{
					cmbFilterByExtraSkill->SelectedIndex = i + 1;
					break;
				}
			}
		}
		else cmbFilterByExtraSkill->SelectedIndex = 0;

		cmbFilterByExtraSkill->EndUpdate();

		updating_extra_skills = false;
	}

	System::Void backgroundWorker1_RunWorkerCompleted( Object^ /*sender*/, RunWorkerCompletedEventArgs^ e )
	{
		if( e->Error != nullptr )
		{
			MessageBox::Show( e->Error->Message );
			progressBar1->Value = 0;
			return;
		}
		else if( e->Cancelled )
		{
			progressBar1->Value = 0;
			return;
		}
		else if( e->Result )
		{
			if( !search_cancelled )
			{
				worker_mutex->WaitOne();
				if( worker_start_index < unsigned( workers.Count ) )
				{
					workers[ worker_start_index ]->RunWorkerAsync( worker_data[ worker_start_index ] );
					worker_start_index++;
				}
				worker_mutex->ReleaseMutex();
			}

			AddSolutions( static_cast< List_t< Solution^ >^ >( e->Result ) );
			progress_mutex->WaitOne();
			if( ++finished_workers >= worker_start_index )
			{
				EnableSearchControls( true );
				progressBar1->Value = 100;
				UpdateExtraSkillCombo( false );
				SaveConfig();
				UpdateResultString();
			}
			else txtSolutions->Text = StartString( SolutionsFound ) + Convert::ToString( all_solutions.Count );
			progress_mutex->ReleaseMutex();
		}
	}		
	

	System::Void backgroundWorker1_ProgressChanged( Object^ /*sender*/, ProgressChangedEventArgs^ e )
	{
		progress_mutex->WaitOne();
		num_updates++;
		total_progress += e->ProgressPercentage;
		progressBar1->Value = total_progress / workers.Count;
		progress_mutex->ReleaseMutex();
		if( e->UserState )
		{
			AddSolutions( static_cast< List_t< Solution^ >^ >( e->UserState ) );
			txtSolutions->Text = StartString( SolutionsFound ) + Convert::ToString( all_solutions.Count );
		}
	}

	Solution^ backgroundWorker1_Search( BackgroundWorker^ worker, System::ComponentModel::DoWorkEventArgs^ e, Query^ query, Charm^ charm, array< Armor^ >^ armors )
	{
		Solution^ job = gcnew Solution();
		job->armors.AddRange( armors );
		job->charm = charm;

		try
		{
			if( job->MatchesQuery( query, true ) )
				return job;
		}
		catch( Exception^ e )
		{
			MessageBox::Show( e->ToString() );
		}
		return nullptr;
	}

	System::Void backgroundWorker1_DoWork( System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e )
	{
		try
		{
			BackgroundWorker^ worker = static_cast< BackgroundWorker^ >( sender );
			ThreadSearchData^ qc = static_cast< ThreadSearchData^ >( e->Argument );
			List_t< Solution ^ >^ solutions = gcnew List_t< Solution^ >;
			List_t< Armor^ >^ head  = query->rel_armor[ int( Armor::ArmorType::HEAD ) ];
			List_t< Armor^ >^ body  = query->rel_armor[ int( Armor::ArmorType::BODY ) ];
			List_t< Armor^ >^ arms  = query->rel_armor[ int( Armor::ArmorType::ARMS ) ];
			List_t< Armor^ >^ waist = query->rel_armor[ int( Armor::ArmorType::WAIST ) ];
			List_t< Armor^ >^ legs  = query->rel_armor[ int( Armor::ArmorType::LEGS ) ];
	 
			if( head->Count  == 0 ) head->Add( nullptr );
			if( body->Count  == 0 ) body->Add( nullptr );
			if( arms->Count  == 0 ) arms->Add( nullptr );
			if( waist->Count == 0 ) waist->Add( nullptr );
			if( legs->Count  == 0 ) legs->Add( nullptr );
			
			//int solutions_found = 0;
			unsigned last_percent = 0;
			bool new_stuff = false;
			for( int i = 0; i < head->Count; ++i )
			{
				for( int j = 0; j < body->Count; ++j )
				{
					const int progress = ( 100 * ( i * body->Count + j ) ) / ( head->Count * body->Count );
				
					if( !new_stuff )
					{
						worker->ReportProgress( progress - last_percent );
						last_percent = progress;
					}
					else
					{
						List_t< Solution ^ >^ temp = solutions; //handle race-condition: shouldn't modify solution list while iterating through it
						solutions = gcnew List_t< Solution^ >;
						worker->ReportProgress( progress - last_percent, temp );
						last_percent = progress;
						new_stuff = false;
					}
					for( int k = 0; k < arms->Count; ++k )
					{
						for( int l = 0; l < waist->Count; ++l )
						{
							for( int m = 0; m < legs->Count; ++m )
							{
								if( worker->CancellationPending )
								{
									e->Result = solutions;
									return;
								}
								array< Armor^ >^ search_armors = { head[i], body[j], arms[k], waist[l], legs[m] };
								Solution^ good = backgroundWorker1_Search( worker, e, query, qc->charm, search_armors );
								if( good )
								{
									solutions->Add( good );
									new_stuff = true;
									/*if( ++solutions_found >= MAX_LIMIT )
									{
										e->Result = solutions;
										worker->ReportProgress( 100 - last_percent );
										return;
									}*/
								}
							}
						}
					}
				}
			}
			worker->ReportProgress( 100 - last_percent );
			e->Result = solutions;
		}
		catch( Exception^ e )
		{
			Assert( false, L"Worker thread exception: " + e->ToString() );
		}
	}

	System::Void backgroundWorker1_DoWork2( System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e )
	{
		try
		{
			BackgroundWorker^ worker = static_cast< BackgroundWorker^ >( sender );
			ThreadSearchData2^ qc2 = static_cast< ThreadSearchData2^ >( e->Argument );
			List_t< Solution ^ >^ solutions = gcnew List_t< Solution^ >;
			if( qc2->charms && qc2->charms->Count > 0 )
			{
				for each( Charm^ charm in qc2->charms )
				{
					Solution^ good = backgroundWorker1_Search( worker, e, qc2->query, charm, qc2->armors );
					if( good )
						solutions->Add( good );
				}
			}
			
			Solution^ good = backgroundWorker1_Search( worker, e, qc2->query, nullptr, qc2->armors );
			if( good )
				solutions->Add( good );

			worker->ReportProgress( 100 );
			e->Result = solutions;
		}
		catch( Exception^ e )
		{
			Assert( false, L"Worker thread exception2: " + e->ToString() );
		}
	}
#pragma endregion

	System::Void FindDialogClosed( System::Object^ sender, System::EventArgs^ e )
	{
		find_dialog = nullptr;
	}

	System::Void FindDialogFoundText( System::Object^ sender, System::EventArgs^ e )
	{
		Find^ find = (Find^)sender;
		if( find->index == -1 )
		{
			txtSolutions->SelectionStart = txtSolutions->Text->Length;
			txtSolutions->SelectionLength = 0;
		}
		else
		{
			txtSolutions->SelectionStart = find->index;
			txtSolutions->SelectionLength = find->length;
		}
		txtSolutions->ScrollToCaret();
		txtSolutions->Focus();
	}

	System::Void KeyDown( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
	{
		if( sender == txtSolutions && e->Control )
		{
			if( e->KeyValue == L'A' )
			{
				txtSolutions->SelectAll();
				e->Handled = true;
				e->SuppressKeyPress = true;
			}
			else if( e->KeyValue == L'F' && !find_dialog )
			{
				find_dialog = gcnew Find( txtSolutions );
				find_dialog->DialogClosing += gcnew EventHandler( this, &Form1::FindDialogClosed );
				find_dialog->TextFound += gcnew EventHandler( this, &Form1::FindDialogFoundText );
				find_dialog->Show( this );
				e->Handled = true;
				e->SuppressKeyPress = true;
			}
			else if( e->KeyValue == L'X' )
			{
				
			}
		}
	}

	System::Void cmbSort_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		static int last_index = -1;
		if( cmbSort->SelectedIndex == last_index ) return;
		last_index = cmbSort->SelectedIndex;
		if( data )
		{
			SaveConfig();
			if( final_solutions.Count > 0 )
			{
				SortResults();
				last_result = nullptr;
				UpdateResultString( %final_solutions );
			}
		}
	}

	System::Void UpdateComboBoxLanguage( ComboBox^ cb )
	{
		cb->BeginUpdate();
		for( int i = 1; i < cb->Items->Count; ++i )
		{
			String^ name = (String^)cb->Items[ i ];
			Skill^ skill = Skill::FindSkill( name );
			Assert( skill, L"No such skill!" );
			cb->Items[ i ] = skill->name;
		}
		cb->EndUpdate();
	}

	bool HaveAllDLCEnabled()
	{
		for( int i = 2; i < mnuAllowEventArmor->DropDownItems->Count; ++i )
		{
			ToolStripMenuItem^ item = safe_cast<ToolStripMenuItem^>( mnuAllowEventArmor->DropDownItems[ i ] );
			if( !item->Checked )
				return false;
		}
		return true;
	}

	void SetAllDLC( const bool enabled )
	{
		for( int i = 2; i < mnuAllowEventArmor->DropDownItems->Count; ++i )
		{
			ToolStripMenuItem^ item = safe_cast<ToolStripMenuItem^>( mnuAllowEventArmor->DropDownItems[ i ] );
			item->Checked = enabled;
			DLC_Click( item, nullptr );
		}
	}

	System::Void ToggleAllDLC_Click( System::Object^ sender, System::EventArgs^ e )
	{
		can_save = false;

		if( HaveAllDLCEnabled() )
		{
			mnuToggleAllDLC->Text = StaticString( EnableAll );
			SetAllDLC( false );
		}
		else
		{
			mnuToggleAllDLC->Text = StaticString( DisableAll );
			SetAllDLC( true );
		}

		can_save = true;
		UpdateImpossibleSkills();
		UpdateResultString();
		SaveConfig();
	}

	System::Void DLC_Click( System::Object^ sender, System::EventArgs^ e )
	{
		ToolStripMenuItem^ item = safe_cast< ToolStripMenuItem^ >( sender );
		if( !item->Tag )
			return;

		ArmorDLC^ dlc = safe_cast< ArmorDLC^ >( item->Tag );
		for each( Armor^ armor in dlc->armors )
		{
			armor->dlc_disabled = !item->Checked;
		}

		if( can_save )
		{
			if( HaveAllDLCEnabled() )
				mnuToggleAllDLC->Text = StaticString( DisableAll );
			else
				mnuToggleAllDLC->Text = StaticString( EnableAll );

			UpdateImpossibleSkills();
			UpdateResultString();
			SaveConfig();
		}
	}

	System::Void LanguageSelect_Click( System::Object^ sender, System::EventArgs^ e )
	{
		const int old_language = language;
		for( int index = 0; index < mnuLanguage->DropDownItems->Count; ++index )
		{
			ToolStripMenuItem^ item = static_cast< ToolStripMenuItem^ >( mnuLanguage->DropDownItems[ index ] );
			if( mnuLanguage->DropDownItems[ index ] == sender )
			{
				if( index == language )
					return;
				language = index;
				item->Checked = true;
			}
			else item->Checked = false;
		}
		if( old_language == language )
			return;

		updating_language = true;

		SuspendUpdate::Suspend( this );

		StringTable::LoadLanguage( static_cast< ToolStripMenuItem^ >( sender )->ToString() );

		for each( ComboBox^ box in SkillFilters )
		{
			box->BeginUpdate();
			for( int i = 0, j = 0; i < SkillTag::tags.Count; ++i )
				box->Items[ j++ ] = SkillTag::tags[ i ]->name;
			box->EndUpdate();
		}

		if( HaveAllDLCEnabled() )
			mnuToggleAllDLC->Text = StaticString( DisableAll );
		else
			mnuToggleAllDLC->Text = StaticString( EnableAll );

		for( int i = 0; i < Armor::static_dlc.Count; ++i )
		{
			mnuAllowEventArmor->DropDownItems[ i + 2 ]->Text = Armor::static_dlc[ i ]->name;
		}

#define UpdateMenu( X ) mnu##X->Text = StaticString( X )
		UpdateMenu( File );
		UpdateMenu( Options );
		UpdateMenu( Language );
		UpdateMenu( Help );
		UpdateMenu( LoadData );
		UpdateMenu( SaveData );
		UpdateMenu( Exit );
		UpdateMenu( ClearSettings );
		UpdateMenu( AllowEventArmor );
		UpdateMenu( AllowLowerTierArmor );
		UpdateMenu( AlwaysSearchAlpha );
		UpdateMenu( AllowFairWind );
		UpdateMenu( Donate );
		UpdateMenu( AllowArena );
		UpdateMenu( CheckForUpdates );
		UpdateMenu( About );
		UpdateMenu( MaxResults );
		UpdateMenu( PrintDecoNames );
		UpdateMenu( SortSkillsAlphabetically );
		UpdateMenu( SkillHelp );
		UpdateMenu( Zoom );
		UpdateMenu( PrintMaterials );
#undef UpdateMenu
		nudHR->UpdateEditText();
		lblHR->Text = StaticString( HR );
		lblSlots->Text = StaticString( WeaponSlots );
		grpSortFilter->Text = StaticString( SortFilterResults );
		grpDecorations->Text = StaticString( Decorations );
		grpResults->Text = StaticString( Results );
		rdoMale->Text = StaticString( Male );
		rdoFemale->Text = StaticString( Female );
		grpSkills->Text = StaticString( Skills );
		grpSkillFilters->Text = StaticString( SkillFilters );
		grpGender->Text = StaticString( Gender );
		btnDecorations->Text = StaticString( MyDecorationsButton );
		btnSearch->Text = StaticString( QuickSearch );
		btnAdvancedSearch->Text = StaticString( AdvancedSearchButton );
		btnCancel->Text = StaticString( Cancel );
		cmbDecorationSelect->Items[ 0 ] = StaticString( UseNoDecorations );
		cmbDecorationSelect->Items[ 1 ] = StaticString( UseOnlyMyDecorations );
		cmbDecorationSelect->Items[ 2 ] = StaticString( UseAllDecorations );
		cmbSort->Items[ 0 ] = BasicString( SortByNone );
		cmbSort->Items[ 1 ] = StaticString( DragonRes );
		cmbSort->Items[ 2 ] = StaticString( FireRes );
		cmbSort->Items[ 3 ] = StaticString( IceRes );
		cmbSort->Items[ 4 ] = StaticString( ThunderRes );
		cmbSort->Items[ 5 ] = StaticString( WaterRes );
		cmbSort->Items[ 6 ] = StaticString( BaseDefence );
		cmbSort->Items[ 7 ] = StaticString( MaxDefence );
		cmbSort->Items[ 8 ] = StaticString( AugDefence );
		cmbSort->Items[ 9 ] = StaticString( Difficulty );
		cmbSort->Items[ 10 ] = StaticString( Rarity );
		cmbSort->Items[ 11 ] = StaticString( SortSlotsSpare );
		cmbSort->Items[ 12 ] = StaticString( SortFamily );
		cmbSort->Items[ 13 ] = StaticString( SortExtraSkills );

		tipResults->ToolTipTitle = StaticString( Results );
		tipResults->SetToolTip( txtSolutions, StaticString( TipResults ) );
		tipDecorations->ToolTipTitle = StaticString( Decorations );
		tipDecorations->SetToolTip( btnDecorations, StaticString( TipDecorations ) );
		tipDecorations->SetToolTip( cmbDecorationSelect, StaticString( TipDecorations ) );
		tipSortFilter->ToolTipTitle = StaticString( SortFilterResults );
		tipSortFilter->SetToolTip( cmbFilterByExtraSkill, StaticString( TipFilter ) );
		tipSortFilter->SetToolTip( cmbSort, StaticString( TipSort ) );
		tipQuestLevel->ToolTipTitle = StaticString( HR );
		tipQuestLevel->SetToolTip( nudHR, StaticString( TipQuestLevel ) );
		tipWeaponSlots->ToolTipTitle = StaticString( WeaponSlots );
		tipWeaponSlots->SetToolTip( nudWeaponSlots1, FormatString1( TipWeaponSlotLevel, L"1" ) );
		tipWeaponSlots->SetToolTip( nudWeaponSlots2, FormatString1( TipWeaponSlotLevel, L"2" ) );
		tipWeaponSlots->SetToolTip( nudWeaponSlots3, FormatString1( TipWeaponSlotLevel, L"3" ) );
		tipSkills->ToolTipTitle = StaticString( Skills );
		tipSkills->SetToolTip( lblRemoveSkills, StaticString( TipRemoveSkills ) );
		tipSkills->SetToolTip( lblAddSkills, StaticString( TipAddSkills ) );

		for( int i = 0; i < NumSkills; ++i )
		{
			Skills[ i ]->BeginUpdate();
			cmbSkillFilter_SelectedIndexChanged( SkillFilters[ i ], Skills, SkillFilters, IndexMaps );
			Skills[ i ]->EndUpdate();
		}

		lblAddSkills->Location = Point( TextRenderer::MeasureText( grpSkills->Text, grpSkills->Font ).Width + 2, lblAddSkills->Location.Y );
		lblRemoveSkills->Location = Point( lblAddSkills->Location.X + lblAddSkills->Width - 5, lblRemoveSkills->Location.Y );
		
		nudWeaponSlots1->UpdateEditText();
		nudWeaponSlots2->UpdateEditText();
		nudWeaponSlots3->UpdateEditText();

		UpdateResultString();
		UpdateExtraSkillCombo( true );

		if( construction_complete )
		{
			try
			{
				Decoration::SaveCustom();
			}
			catch( Exception^ e )
			{
				Assert( e, "Failed to save decorations: " + e->ToString() );
			}
		}
		
		updating_language = false;
		UpdateResultString();

		SuspendUpdate::Resume( this );
	}

	System::Void aboutToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		About about_form;
		about_form.ShowDialog( this );
	}

	System::Void SortResults()
	{
		if( cmbSort->SelectedIndex < 1 || sort_off ) return;
		else if( cmbSort->SelectedIndex == 1 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByDragonRes ) );
		else if( cmbSort->SelectedIndex == 2 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByFireRes ) );
		else if( cmbSort->SelectedIndex == 3 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByIceRes ) );
		else if( cmbSort->SelectedIndex == 4 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByThunderRes ) );
		else if( cmbSort->SelectedIndex == 5 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByWaterRes ) );
		else if( cmbSort->SelectedIndex == 6 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByDefence ) );
		else if( cmbSort->SelectedIndex == 7 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByMaxDefence ) );
		else if( cmbSort->SelectedIndex == 8 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByAugDefence ) );
		else if( cmbSort->SelectedIndex == 9 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByDifficulty ) );
		else if( cmbSort->SelectedIndex == 10 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByRarity ) );
		else if( cmbSort->SelectedIndex == 11 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionBySlotsSpare ) );
		else if( cmbSort->SelectedIndex == 12 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByFamily ) );
		else if( cmbSort->SelectedIndex == 13 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionsByExtraSkills ) );
	}

	void contextMenuStrip1_Closing( System::Object^ sender, ToolStripDropDownClosingEventArgs^ e )
	{
		if( preview_pane )
		{
			preview_pane->Close();
			preview_pane = nullptr;
		}
	}

	System::Void contextMenuStrip1_Opening( System::Object^ sender, CancelEventArgs^ e )
	{
		if( txtSolutions->Text == L"" )
			return;

		cmsSolutions->Items->Clear();
		e->Cancel = true;

		Point pos = txtSolutions->PointToClient( txtSolutions->MousePosition );
		int chr = txtSolutions->GetCharIndexFromPosition( pos );
		int line = txtSolutions->GetLineFromCharIndex( chr );
		String^ str = txtSolutions->Lines[ line ];
		if( str == L"" )
			return;
		
		//check for decorations
		if( str->Length >= 4 )
		{
			if( str->Substring( 1, 2 ) == L"x " || str->Substring( 2, 2 ) == L"x " )
			{
				String^ deco_name = str->Substring( str->IndexOf( L' ' ) + 1 );
				Decoration^ decoration = Decoration::FindDecoration( deco_name );
				if( decoration )
				{
					Utility::UpdateContextMenu( cmsSolutions, decoration );
					e->Cancel = false;
				}
				else 
				{
					Decoration^ decoration = Decoration::FindDecorationFromString( deco_name );
					if( decoration )
					{
						Utility::UpdateContextMenu( cmsSolutions, decoration );
						e->Cancel = false;
					}
				}
				return;
			}
		}
		//check for armors
		for( int i = 0; i < result_offsets.Count; ++i )
		{
			if( result_offsets[ i ] > line )
				break;

			int offset = result_offsets[ i ];

			int num_armors = 0;
			for each( Armor^ a in final_solutions[ i ]->armors )
			{
				if( a )
					num_armors++;
			}
			
			if( line >= offset &&
				line < offset + num_armors )
			{
				for( int a = 0, c = 0; a < final_solutions[ i ]->armors.Count; ++a )
				{
					if( final_solutions[ i ]->armors[ a ] && c++ == line - offset )
					{
						Utility::UpdateContextMenu( cmsSolutions, final_solutions[ i ]->armors[ a ], int( nudHR->Value ) >= 9 );
						break;
					}
				}

				if( preview_pane )
					preview_pane->Close();

				preview_pane = gcnew PreviewImage( rdoFemale->Checked );
				
				final_solutions[ i ]->preview_image = preview_pane->SetData( final_solutions[ i ]->armors, final_solutions[ i ]->preview_image );
				if( final_solutions[ i ]->preview_image )
				{
					preview_pane->Show( this );

					preview_pane->SetLocation( cmsSolutions );
				}
				else preview_pane = nullptr;

				e->Cancel = false;
				return;
			}
		}

		//check for charms
		Charm^ charm = Charm::FindCharm( str );
		if( charm )
		{
			Utility::UpdateContextMenu( cmsSolutions, charm );
			e->Cancel = false;
			return;
		}

		//check for extra skills
		{
			//full skills
			Skill^ extra = Skill::FindSkill( str );
			if( extra )
			{
				Utility::UpdateContextMenu( cmsSolutions, extra->ability );
				e->Cancel = false;
				return;
			}
			
			//check potential extra skills
			{
				String^ pots =  StaticString( SkillPossible );
				const int i = pots->IndexOf( "%1" );
				if( i >= 0 )
				{
					const unsigned prefix = pots->Substring( 0, i )->Length;
					const unsigned suffix = pots->Substring( i + 2 )->Length;
					const int len = str->Length - prefix - suffix;
					if( len > 0 )
					{
						String^ skillname = str->Substring( prefix, len );
						extra = Skill::FindSkill( skillname );
						if( extra )
						{
							Utility::UpdateContextMenu( cmsSolutions, extra->ability );
							e->Cancel = false;
						}
					}
				}
			}
		}
	}

	System::Void UpdateResultString()
	{
		if( updating_language )
			return;
		last_result = L"";
		final_solutions.Clear();

		if( cmbFilterByExtraSkill->SelectedIndex < 1 || cmbFilterByExtraSkill->SelectedIndex - 1 >= solutions_extra_skills.Count )
		{
			final_solutions.AddRange( %all_solutions );
		}
		else
		{
			ExtraSkill^ selected = solutions_extra_skills[ cmbFilterByExtraSkill->SelectedIndex - 1 ];
			for each( Solution^ s in all_solutions )
			{
				if( selected->want && ( Utility::Contains( %s->extra_skills, selected->skill ) || Utility::Contains( %s->potential_extra_skills, selected->skill ) ) ||
					!selected->want && !Utility::Contains( %s->extra_skills, selected->skill->ability ) )
					final_solutions.Add( s );
			}
		}
		SortResults();
		UpdateResultString( %final_solutions );
	}

	System::Void btnDecorations_Click( System::Object^ sender, System::EventArgs^ e )
	{
		ManageDecorations mydecos( language, mnuSortSkillsAlphabetically->Checked );
		mydecos.ShowDialog( this );
		UpdateImpossibleSkills();
	}
	
	System::Void UpdateMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		System::Diagnostics::Process::Start( "https://www.facebook.com/AthenasArmorSetSearch/" );
	}

	System::Void mnuDonate_Click(System::Object^  sender, System::EventArgs^  e)
	{
		System::Diagnostics::Process::Start( "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=8A7BDLKN7ZK6W&lc=NZ&item_name=Athena%27s%20Armor%20Set%20Search&currency_code=NZD&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted" );
	}

	System::Void mnuClearSettings_Click( System::Object^ sender, System::EventArgs^ e )
	{
		lock_skills = true;

		cmbSort->SelectedIndex = 0;
		for each( ComboBox^ cb in Skills )
		{
			cb->SelectedIndex = -1;
		}

		for each( ComboBox^ cb in SkillFilters )
		{
			cb->SelectedIndex = 0;
		}

		lock_skills = false;

		for each( ComboBox^ cb in Skills )
		{
			cmbSkill_SelectedIndexChanged( cb, nullptr );
		}
	}

	System::Void nudHR_ValueChanged(System::Object^  sender, System::EventArgs^  e)
	{
		UpdateImpossibleSkills();
	}
	
	System::Void mnuLoad_Click( System::Object^ sender, System::EventArgs^ e )
	{
		OpenFileDialog dlg;
		dlg.InitialDirectory = System::Environment::CurrentDirectory;
		dlg.DefaultExt = L".ass";
		dlg.AddExtension = true;
		dlg.Filter = StartString( ASSSettings ) + L"(*.ass)|*.ass";
		dlg.FilterIndex = 0;
		Windows::Forms::DialogResult res = dlg.ShowDialog();
		if( res == Windows::Forms::DialogResult::OK )
		{
			LoadConfig( dlg.FileName, true );
			LoadConfig( dlg.FileName, false );
		}
	}
	
	System::Void mnuSave_Click( System::Object^ sender, System::EventArgs^ e )
	{
		SaveFileDialog dlg;
		dlg.InitialDirectory = System::Environment::CurrentDirectory;
		dlg.DefaultExt = L".ass";
		dlg.AddExtension = true;
		dlg.Filter = StartString( ASSSettings ) + L"(*.ass)|*.ass";
		dlg.FilterIndex = 0;
		dlg.FileName = L"results.ass";
		Windows::Forms::DialogResult res = dlg.ShowDialog();
		if( res == Windows::Forms::DialogResult::OK )
			SaveConfig( dlg.FileName );
	}

	void SkillHelp_FormClosed( System::Object^ sender, FormClosedEventArgs^ e )
	{
		help_window = nullptr;
	}

	System::Void mnuSkillHelp_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if( help_window != nullptr )
		{
			help_window->Focus();
			return;
		}

		help_window = gcnew SkillHelp( -1, mnuSortSkillsAlphabetically->Checked, rdoFemale->Checked );
		help_window->FormClosed += gcnew FormClosedEventHandler( this, &Form1::SkillHelp_FormClosed );
		help_window->Show( this );
	}

	System::Void mnuShowRequiredSkillPoints_Click(System::Object^  sender, System::EventArgs^  e)
	{
		for( int i = 0; i < NumSkills; ++i )
		{
			cmbSkillFilter_SelectedIndexChanged( SkillFilters[ i ], Skills, SkillFilters, IndexMaps );
		}

		OptionsChanged( sender, e );
	}

	System::Void ZoomLevelChanged( System::Object^ sender, System::EventArgs^ e )
	{
		ToolStripMenuItem^ item = safe_cast< ToolStripMenuItem^ >( sender );
		if( !item )
			return;

		for each( ToolStripMenuItem^ m in mnuZoom->DropDownItems )
			if( m != sender )
				m->Checked = false;
		item->Checked = true;
	
		PreviewImage::zoom = (int)(Int32)item->Tag;
		SaveConfig();
	}

	System::Void lblAddSkills_Click( System::Object^  sender, System::EventArgs^  e )
	{
		InitializeComboBox( NumSkills );
		ComboBox^ filter = SkillFilters[ NumSkills++ ];

		for each( SkillTag^ tag in SkillTag::tags )
		{
			filter->Items->Add( tag->name );
		}
		filter->SelectedIndex = 0;

		this->Size = Drawing::Size( Width, NumSkills * 27 + 307 );

		SaveConfig();
	}

	System::Void lblRemoveSkills_Click( System::Object^  sender, System::EventArgs^  e )
	{
		if( NumSkills <= 4 )
			return;

		ComboBox^ skill = Skills[ NumSkills - 1 ];
		skill->SelectedIndex = -1;
		cmbSkill_SelectedIndexChanged( skill, nullptr );

		--NumSkills;

		ComboBox^ filter = SkillFilters[ NumSkills ];
		grpSkillFilters->Controls->Remove( filter );
		SkillFilters.RemoveAt( NumSkills );
		delete filter;

		grpSkills->Controls->Remove( skill );
		Skills.RemoveAt( NumSkills );
		delete skill;

		IndexMaps.RemoveAt( NumSkills );

		last_selected_ability.RemoveAt( NumSkills );

		this->Size = Drawing::Size( Width, NumSkills * 27 + 307 );

		SaveConfig();
	}

	System::Void nudWeaponSlots1_ValueChanged( System::Object^  sender, System::EventArgs^  e )
	{
		if( (int)nudWeaponSlots1->Value == 0 )
		{
			nudWeaponSlots2->Enabled = false;
			nudWeaponSlots3->Enabled = false;
			nudWeaponSlots2->Value = 0;
			nudWeaponSlots3->Value = 0;
		}
		else
		{
			nudWeaponSlots2->Enabled = true;
		}
	}

	System::Void nudWeaponSlots2_ValueChanged( System::Object^  sender, System::EventArgs^  e )
	{
		if( (int)nudWeaponSlots2->Value == 0 )
		{
			nudWeaponSlots3->Enabled = false;
			nudWeaponSlots3->Value = 0;
		}
		else
		{
			nudWeaponSlots3->Enabled = true;
		}
	}

	System::Void exitToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		this->Close();
	}

};
}

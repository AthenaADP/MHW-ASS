#pragma once
#include "Skill.h"
#include "Armor.h"
#include "PreviewImage.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace MHWASS
{
	inline int SortByOrder( Ability^ a, Ability^ b )
	{
		return a->order - b->order;
	}

	public ref class SkillHelp : public System::Windows::Forms::Form
	{
		bool alphabetic, default_female;
		int armor_list_begin;
		Generic::List< Armor^ > armors;
		Generic::List< unsigned > armor_types;
		Generic::List< Ability^ > abilities;
		PreviewImage^ preview_pane;

		ref struct SubSkill
		{
			Skill^ skill;
			int char_start, char_end;
		};
		Generic::List< SubSkill^ > subskills;

	public:
		SkillHelp( const int static_index, const bool alphabetic, const bool default_female )
		{
			InitializeComponent();

			txtDescription->Text = StaticString( Description );

			grpSkills->Text = StaticString( Skills );
			
			grpDescription->Text = StaticString( Description );
			grpDescription->Location = Point( 6 + grpSkills->Location.X + grpSkills->Size.Width, grpDescription->Location.Y );
			grpDescription->Size = Drawing::Size( this->Width - grpDescription->Location.X - 28, grpDescription->Size.Height );

			tipDescription->ToolTipTitle = StaticString( Description );
			tipDescription->SetToolTip( txtDescription, StaticString( TipResults ) );

			this->Text = BasicString( SkillHelpForm );

			this->alphabetic = alphabetic;
			this->default_female = default_female;
			
			//Setup abilities
			abilities.AddRange( %Ability::static_abilities );
			if( alphabetic )
				abilities.Sort( gcnew Comparison< Ability^ >( SortByOrder ) );

			for( int i = 0; i < abilities.Count; ++i )
				listBox1->Items->Add( abilities[ i ]->name );

			if( static_index >= 0 && static_index < listBox1->Items->Count )
				SelectInfo( static_index );
		}

		System::Void SelectInfo( const int static_index )
		{
			listBox1->SelectedIndex = abilities.IndexOf( Ability::static_abilities[ static_index ] );
		}

	protected:

		~SkillHelp()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListBox^  listBox1;
	private: System::Windows::Forms::GroupBox^  grpSkills;
	private: System::Windows::Forms::GroupBox^  grpDescription;
	private: System::Windows::Forms::RichTextBox^  txtDescription;
	private: System::ComponentModel::IContainer^  components;
	private: System::Windows::Forms::ContextMenuStrip^  cmsDescription;
	private: System::Windows::Forms::ToolTip^ tipDescription;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->listBox1 = (gcnew System::Windows::Forms::ListBox());
			this->grpSkills = (gcnew System::Windows::Forms::GroupBox());
			this->grpDescription = (gcnew System::Windows::Forms::GroupBox());
			this->txtDescription = (gcnew System::Windows::Forms::RichTextBox());
			this->cmsDescription = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->grpSkills->SuspendLayout();
			this->grpDescription->SuspendLayout();
			this->SuspendLayout();
			// 
			// listBox1
			// 
			this->listBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->listBox1->FormattingEnabled = true;
			this->listBox1->Location = System::Drawing::Point(6, 19);
			this->listBox1->Name = L"listBox1";
			this->listBox1->Size = System::Drawing::Size(158, 415);
			this->listBox1->TabIndex = 0;
			this->listBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &SkillHelp::listBox1_SelectedIndexChanged);
			// 
			// grpSkills
			// 
			this->grpSkills->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			this->grpSkills->Controls->Add(this->listBox1);
			this->grpSkills->Location = System::Drawing::Point(12, 12);
			this->grpSkills->Name = L"grpSkills";
			this->grpSkills->Size = System::Drawing::Size(170, 433);
			this->grpSkills->TabIndex = 1;
			this->grpSkills->TabStop = false;
			this->grpSkills->Text = L"Skills";
			// 
			// grpDescription
			// 
			this->grpDescription->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->grpDescription->Controls->Add(this->txtDescription);
			this->grpDescription->Location = System::Drawing::Point(188, 12);
			this->grpDescription->Name = L"grpDescription";
			this->grpDescription->Size = System::Drawing::Size(720, 433);
			this->grpDescription->TabIndex = 2;
			this->grpDescription->TabStop = false;
			this->grpDescription->Text = L"Description";
			// 
			// txtDescription
			// 
			this->txtDescription->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->txtDescription->ContextMenuStrip = this->cmsDescription;
			this->txtDescription->Location = System::Drawing::Point(6, 19);
			this->txtDescription->Name = L"txtDescription";
			this->txtDescription->ReadOnly = true;
			this->txtDescription->Size = System::Drawing::Size(708, 408);
			this->txtDescription->TabIndex = 0;
			this->txtDescription->Text = L"";
			this->txtDescription->WordWrap = false;
			// 
			// cmsDescription
			// 
			this->cmsDescription->Name = L"cmsDescription";
			this->cmsDescription->Size = System::Drawing::Size(61, 4);
			this->cmsDescription->Opening += gcnew System::ComponentModel::CancelEventHandler(this, &SkillHelp::cmsDescription_Opening);
			this->cmsDescription->Closing += gcnew ToolStripDropDownClosingEventHandler( this, &SkillHelp::cmsDescription_Closing );
			// 
			// SkillHelp
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(920, 453);
			this->Controls->Add(this->grpDescription);
			this->Controls->Add(this->grpSkills);
			this->Name = L"SkillHelp";
			this->Text = L"Skill Help";
			this->grpSkills->ResumeLayout(false);
			this->grpDescription->ResumeLayout(false);
			this->ResumeLayout(false);
			//
			// tipDescription
			//
			tipDescription = gcnew System::Windows::Forms::ToolTip();
		}
#pragma endregion

		System::Void SelectInfoInternal( String^ name )
		{
			Ability^ ability = Ability::FindAbility( name );
			if( ability == nullptr )
				return;

			armors.Clear();
			armor_types.Clear();
			armor_list_begin = -1;
			subskills.Clear();

			SuspendDrawing( txtDescription );

			txtDescription->Text = ability->name;

			txtDescription->AppendText( Environment::NewLine );
			txtDescription->AppendText( Environment::NewLine );

			txtDescription->SelectionStart = 0;
			txtDescription->SelectionLength = ability->name->Length;
			txtDescription->SelectionColor = Drawing::Color::Blue;

			
			{
				Ability::SkillMap_t::Enumerator e = ability->skills.GetEnumerator();
				while( e.MoveNext() )
				{
					Generic::List< SubSkill^ > subskills2;

					const int sel_start = txtDescription->TextLength;
					const int name_start = txtDescription->TextLength;
					txtDescription->AppendText( e.Current.Value->name );
					const int name_end = txtDescription->TextLength;

					String^ desc = e.Current.Value->description;
					if( !desc )
						desc = L"(no description)";

					int subskill_start = desc->IndexOf( L"<<" );
					while( subskill_start != -1 )
					{
						int subskill_end = desc->IndexOf( L">>", subskill_start + 2 );
						if( subskill_end == -1 )
							break;

						SubSkill^ ss = gcnew SubSkill();

						String^ skill_name = desc->Substring( subskill_start + 2, subskill_end - subskill_start - 2 );
						ss->skill = Skill::FindSkill( skill_name );
						if( ss->skill == nullptr )
							break;

						ss->char_start = subskill_start;
						ss->char_end = subskill_end - 2;
						
						subskills2.Add( ss );

						desc = desc->Substring( 0, subskill_start ) + skill_name + desc->Substring( subskill_end + 2 );
						subskill_start = desc->IndexOf( L"<<" );
					}

					txtDescription->AppendText( L".  " );

					const int desc_start = txtDescription->TextLength;
					
					txtDescription->AppendText( desc );
					txtDescription->AppendText( Environment::NewLine );

					txtDescription->SelectionStart = sel_start;
					txtDescription->SelectionLength = txtDescription->TextLength - sel_start;
					txtDescription->SelectionColor = ( e.Current.Key >= 0 ) ? Drawing::Color::Black : Drawing::Color::Red;

					txtDescription->SelectionStart = name_start;
					txtDescription->SelectionLength = name_end - name_start;
					txtDescription->SelectionColor = Drawing::Color::Blue;

					for each( SubSkill^ ss in subskills2 )
					{
						ss->char_end += desc_start;
						ss->char_start += desc_start;

						txtDescription->SelectionStart = ss->char_start;
						txtDescription->SelectionLength = ss->char_end - ss->char_start;
						txtDescription->SelectionColor = Drawing::Color::Blue;
					}
					subskills.AddRange( %subskills2 );
				}
			}

			const int armor_section_start = txtDescription->Text->Length;

			for( int i = 0; i < (int)Armor::ArmorType::NumArmorTypes; ++i )
			{
				for( int j = 0; j < Armor::static_armors[ i ]->Count; ++j )
				{
					Armor^ armor = Armor::static_armors[ i ][ j ];
					for each( AbilityPair^ ap in armor->abilities )
					{
						if( ap->ability == ability || ap->ability->set_ability && SetAbilityCanGrant( ap->ability, ability ) )
						{
							armors.Add( armor );
							armor_types.Add( i );
							break;
						}
					}
				}
			}

			if( armors.Count > 0 )
				txtDescription->AppendText( Environment::NewLine + StaticString( ArmorsWithSkill ) + Environment::NewLine + Environment::NewLine );

			armor_list_begin = txtDescription->Lines->Length - 1;

			for each( Armor^ armor in armors )
			{
				txtDescription->AppendText( armor->name + Environment::NewLine );
			}

			txtDescription->Select( armor_section_start, txtDescription->Text->Length - armor_section_start );
			txtDescription->SelectionColor = Drawing::Color::Black;
			txtDescription->Select( 0, 0 );

			ResumeDrawing( txtDescription );
		}

		bool SetAbilityCanGrant( Ability^ set_ability, Ability^ desired )
		{
			Assert( set_ability->set_ability, L"Set Ability not flagged as such" );
			
			Ability::SkillMap_t::Enumerator e = set_ability->skills.GetEnumerator();
			while( e.MoveNext() )
			{
				Assert( Skill::set_skill_map.ContainsKey( e.Current.Value ), L"Set skill not in map" );
				if( Skill::set_skill_map[ e.Current.Value ]->ability == desired )
					return true;
			}
			return false;
		}

		System::Void listBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		{
			if( listBox1->SelectedIndex < 0 )
			{
				txtDescription->Clear();
				return;
			}

			SelectInfoInternal( listBox1->Items[ listBox1->SelectedIndex ]->ToString() );
		}

		System::Void cmsDescription_Opening(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e)
		{
			cmsDescription->Items->Clear();
			e->Cancel = true;

			Point pos = txtDescription->PointToClient( txtDescription->MousePosition );
			int chr = txtDescription->GetCharIndexFromPosition( pos );
			int line = txtDescription->GetLineFromCharIndex( chr );
			if( line < 0 || line >= txtDescription->Lines->Length )
				return;
			String^ str = txtDescription->Lines[ line ];
			if( str == L"" )
				return;

			if( line < armor_list_begin )
			{
				for each( SubSkill^ ss in subskills )
				{
					if( chr >= ss->char_start && chr < ss->char_end )
					{
						listBox1->SelectedIndex = listBox1->FindString( ss->skill->ability->name );
						break;
					}
				}
			}
			else if( line - armor_list_begin < armors.Count )
			{
				Armor^ armor = armors[ line - armor_list_begin ];
				Utility::UpdateContextMenu( cmsDescription, armor, true );

				if( preview_pane )
					preview_pane->Close();

				if( armor->gender == Gender::MALE )
					preview_pane = gcnew PreviewImage( false );
				else if( armor->gender == Gender::FEMALE )
					preview_pane = gcnew PreviewImage( true );
				else
					preview_pane = gcnew PreviewImage( default_female );

				Generic::List< Armor^ > fake_list;
				for( unsigned i = 0; i < (unsigned)Armor::ArmorType::NumArmorTypes; ++i )
					fake_list.Add( nullptr );
				fake_list[ armor_types[ line - armor_list_begin ] ] = armor;
			
				if( preview_pane->SetData( fake_list ) )
				{
					preview_pane->Show( this );

					preview_pane->SetLocation( cmsDescription );
				}
				else preview_pane = nullptr;

				e->Cancel = false;
			}
		}

		void cmsDescription_Closing( System::Object^ sender, ToolStripDropDownClosingEventArgs^ e )
		{
			if( preview_pane )
			{
				preview_pane->Close();
				preview_pane = nullptr;
			}
		}
};
}

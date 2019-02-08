#pragma once
#include "Armor.h"
#include "LoadedData.h"
#include "Common.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#pragma warning( disable: 4677 ) //warning for passing my own class into this form

namespace MHWASS {

	inline int CompareDecorationsByName( Decoration^ d1, Decoration^ d2 )
	{
		return d1->name->CompareTo( d2->name );
	}

	public ref class ManageDecorations : public System::Windows::Forms::Form
	{
		const int language;
		bool update_skills, updating_decorations, sort_alphabetically;
		List_t< Decoration^ > owned_decorations;
	
	public:

		ManageDecorations( const int language, const bool sort_alphabetically )
			: language( language ), update_skills( true ), updating_decorations( false ), sort_alphabetically( sort_alphabetically )
		{
			InitializeComponent();
			
			RefreshList( -1 );
			
			InitFilter( cmbSkillFilters );
			
			Text = BasicString( MyDecorationsForm );
			btnAdd->Text = StaticString( AddNewDecoration );
			btnDelete->Text = StaticString( DeleteDecoration );
			btnDeleteAll->Text = StaticString( DeleteAllDecorations );
			btnImport->Text = StaticString( Import );
			
			//MatchWidthToText( btnAdd );

			btnAdd->Enabled = false;
			btnDelete->Enabled = false;
		}

		void MatchWidthToText( Control^ b )
		{
			b->Width = 10 + TextRenderer::MeasureText( b->Text, b->Font ).Width;
		}

		void PlaceToTheRightOf( Control^ to_move, Control^ left, const int extra )
		{
			to_move->Location = Point( left->Location.X + left->Width + extra, to_move->Location.Y );
		}

		void InitFilter( ComboBox^ cb )
		{
			cb->BeginUpdate();
			cb->Items->Clear();
			for each( SkillTag^ tag in SkillTag::tags )
			{
				cb->Items->Add( tag->name );
			}

			if( cb->Items->Count > 2 ) //replace "Related" with "Complete"
				cb->Items[ 2 ] = StaticString( Completed );

			cb->SelectedIndex = 0;
			cb->EndUpdate();
		}

	protected:
		~ManageDecorations()
		{
			if( components )
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListBox^  lstDecorations;
	private: System::Windows::Forms::Button^  btnAdd;
	private: System::Windows::Forms::ComboBox^  cmbSkills;
	private: System::Windows::Forms::Button^  btnDelete;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::ComboBox^  cmbSkillFilters;
	private: System::Windows::Forms::Button^  btnDeleteAll;
	private: System::Windows::Forms::Button^  btnImport;

	private: System::ComponentModel::IContainer^  components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->lstDecorations = ( gcnew System::Windows::Forms::ListBox() );
			this->btnAdd = ( gcnew System::Windows::Forms::Button() );
			this->cmbSkills = ( gcnew System::Windows::Forms::ComboBox() );
			this->btnDelete = ( gcnew System::Windows::Forms::Button() );
			this->groupBox1 = ( gcnew System::Windows::Forms::GroupBox() );
			this->cmbSkillFilters = ( gcnew System::Windows::Forms::ComboBox() );
			this->groupBox2 = ( gcnew System::Windows::Forms::GroupBox() );
			this->btnImport = ( gcnew System::Windows::Forms::Button() );
			this->btnDeleteAll = ( gcnew System::Windows::Forms::Button() );
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->SuspendLayout();
			// 
			// lstDecorations
			// 
			this->lstDecorations->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom )
				| System::Windows::Forms::AnchorStyles::Left )
				| System::Windows::Forms::AnchorStyles::Right ) );
			this->lstDecorations->FormattingEnabled = true;
			this->lstDecorations->IntegralHeight = false;
			this->lstDecorations->Location = System::Drawing::Point( 6, 19 );
			this->lstDecorations->Name = L"lstDecorations";
			this->lstDecorations->Size = System::Drawing::Size( 231, 204 );
			this->lstDecorations->TabIndex = 12;
			this->lstDecorations->SelectedIndexChanged += gcnew System::EventHandler( this, &ManageDecorations::lstDecorations_SelectedIndexChanged );
			// 
			// btnAdd
			// 
			this->btnAdd->Location = System::Drawing::Point( 266, 18 );
			this->btnAdd->Name = L"btnAdd";
			this->btnAdd->Size = System::Drawing::Size( 88, 23 );
			this->btnAdd->TabIndex = 9;
			this->btnAdd->Text = L"&Add";
			this->btnAdd->UseVisualStyleBackColor = true;
			this->btnAdd->Click += gcnew System::EventHandler( this, &ManageDecorations::btnAdd_Click );
			// 
			// cmbSkills
			// 
			this->cmbSkills->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::Suggest;
			this->cmbSkills->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
			this->cmbSkills->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbSkills->FormattingEnabled = true;
			this->cmbSkills->Location = System::Drawing::Point( 12, 19 );
			this->cmbSkills->Name = L"cmbSkills";
			this->cmbSkills->Size = System::Drawing::Size( 132, 21 );
			this->cmbSkills->TabIndex = 1;
			this->cmbSkills->SelectedIndexChanged += gcnew System::EventHandler( this, &ManageDecorations::cmbSkills_SelectedIndexChanged );
			// 
			// btnDelete
			// 
			this->btnDelete->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right ) );
			this->btnDelete->Location = System::Drawing::Point( 243, 19 );
			this->btnDelete->Name = L"btnDelete";
			this->btnDelete->Size = System::Drawing::Size( 111, 25 );
			this->btnDelete->TabIndex = 13;
			this->btnDelete->Text = L"&Remove";
			this->btnDelete->UseVisualStyleBackColor = true;
			this->btnDelete->Click += gcnew System::EventHandler( this, &ManageDecorations::btnDelete_Click );
			// 
			// groupBox1
			// 
			this->groupBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left )
				| System::Windows::Forms::AnchorStyles::Right ) );
			this->groupBox1->Controls->Add( this->cmbSkillFilters );
			this->groupBox1->Controls->Add( this->btnAdd );
			this->groupBox1->Controls->Add( this->cmbSkills );
			this->groupBox1->Location = System::Drawing::Point( 12, 12 );
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size( 366, 53 );
			this->groupBox1->TabIndex = 14;
			this->groupBox1->TabStop = false;
			// 
			// cmbSkillFilters
			// 
			this->cmbSkillFilters->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::Suggest;
			this->cmbSkillFilters->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
			this->cmbSkillFilters->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbSkillFilters->FormattingEnabled = true;
			this->cmbSkillFilters->Location = System::Drawing::Point( 150, 19 );
			this->cmbSkillFilters->Name = L"cmbSkillFilters";
			this->cmbSkillFilters->Size = System::Drawing::Size( 110, 21 );
			this->cmbSkillFilters->TabIndex = 2;
			this->cmbSkillFilters->SelectedIndexChanged += gcnew System::EventHandler( this, &ManageDecorations::cmbSkillFilters_SelectedIndexChanged );
			// 
			// groupBox2
			// 
			this->groupBox2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom )
				| System::Windows::Forms::AnchorStyles::Left )
				| System::Windows::Forms::AnchorStyles::Right ) );
			this->groupBox2->Controls->Add( this->btnImport );
			this->groupBox2->Controls->Add( this->btnDeleteAll );
			this->groupBox2->Controls->Add( this->lstDecorations );
			this->groupBox2->Controls->Add( this->btnDelete );
			this->groupBox2->Location = System::Drawing::Point( 12, 71 );
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size( 366, 231 );
			this->groupBox2->TabIndex = 15;
			this->groupBox2->TabStop = false;
			// 
			// btnImport
			// 
			this->btnImport->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right ) );
			this->btnImport->Location = System::Drawing::Point( 243, 199 );
			this->btnImport->Name = L"btnImport";
			this->btnImport->Size = System::Drawing::Size( 111, 25 );
			this->btnImport->TabIndex = 20;
			this->btnImport->Text = L"&Import...";
			this->btnImport->UseVisualStyleBackColor = true;
			this->btnImport->Click += gcnew System::EventHandler( this, &ManageDecorations::btnImport_Click );
			// 
			// btnDeleteAll
			// 
			this->btnDeleteAll->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right ) );
			this->btnDeleteAll->Location = System::Drawing::Point( 243, 44 );
			this->btnDeleteAll->Name = L"btnDeleteAll";
			this->btnDeleteAll->Size = System::Drawing::Size( 111, 25 );
			this->btnDeleteAll->TabIndex = 14;
			this->btnDeleteAll->Text = L"Delete &All";
			this->btnDeleteAll->UseVisualStyleBackColor = true;
			this->btnDeleteAll->Click += gcnew System::EventHandler( this, &ManageDecorations::btnDeleteAll_Click );
			// 
			// ManageDecorations
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF( 6, 13 );
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size( 390, 314 );
			this->Controls->Add( this->groupBox2 );
			this->Controls->Add( this->groupBox1 );
			this->Name = L"ManageDecorations";
			this->Text = L"My Decorations";
			this->groupBox1->ResumeLayout( false );
			this->groupBox2->ResumeLayout( false );
			this->ResumeLayout( false );

		}
#pragma endregion
	private:

		System::Void RefreshList( const int new_index )
		{
			lstDecorations->BeginUpdate();
			lstDecorations->Items->Clear();
			owned_decorations.Clear();
			for each( Decoration^ deco in Decoration::static_decorations )
			{
				if( deco->num_owned > 0 )
					owned_decorations.Add( deco );
			}

			owned_decorations.Sort( gcnew Comparison< Decoration^ >( CompareDecorationsByName ) );

			for each( Decoration^ deco in owned_decorations )
			{
				lstDecorations->Items->Add( FormatString2( NumDecorations, deco->num_owned, deco->name ) );
			}

			lstDecorations->SelectedIndex = new_index;
			lstDecorations->EndUpdate();
		}

		System::Void btnDelete_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if( lstDecorations->SelectedIndex < 0 )
				return;
			
			Decoration^ deco = owned_decorations[ lstDecorations->SelectedIndex ];
			if( deco->num_owned == 0 )
				return;

			deco->num_owned--;
			Decoration::SaveCustom();

			if( cmbSkillFilters->SelectedIndex == 2 )
				cmbSkillFilters->SelectedIndex = 0;

			if( deco->num_owned > 0 )
			{
				updating_decorations = true;
				RefreshList( lstDecorations->SelectedIndex );
				updating_decorations = false;
			}
			else
				RefreshList( Math::Min( lstDecorations->SelectedIndex, owned_decorations.Count - 2 ) );
		}

		System::Void btnAdd_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if( cmbSkills->SelectedIndex < 0 )
				return;

			Ability^ ab = Ability::FindAbility( cmbSkills->SelectedItem->ToString() );
			if( !ab )
				return;

			ab->decoration->num_owned++;
			Decoration::SaveCustom();

			for( int i = 0; i < owned_decorations.Count; ++i )
			{
				if( owned_decorations[ i ] == ab->decoration )
				{
					updating_decorations = true;
					RefreshList( i );
					updating_decorations = false;
					if( ab->decoration->num_owned >= ab->max_level )
						cmbSkillFilters->SelectedIndex = 2;
					return;
				}
			}
			
			updating_decorations = true;
			RefreshList( -1 );
			for( int i = 0; i < owned_decorations.Count; ++i )
			{
				if( owned_decorations[ i ] == ab->decoration )
				{
					updating_decorations = true;
					lstDecorations->SelectedIndex = i;
					updating_decorations = false;
					if( ab->decoration->num_owned >= ab->max_level )
						cmbSkillFilters->SelectedIndex = 2;
					return;
				}
			}
			updating_decorations = false;
		}

		System::Void UpdateFilter( ComboBox^ skill, ComboBox^ filter, Ability^ banned )
		{
			if( !update_skills )
				return;
			List_t< Ability^ >^ the_list = sort_alphabetically ? %Ability::ordered_abilities : %Ability::static_abilities;

			update_skills = false;
			String^ old_item = (String^)skill->SelectedItem;
			skill->BeginUpdate();
			skill->Items->Clear();
			skill->Items->Add( StaticString( NoneBrackets ) );
			
			if( filter->SelectedIndex == 0 ) //All
			{
				for each( Ability^ ab in the_list )
				{
					if( ab != banned && ab->decoration && ab->decoration->num_owned < ab->max_level )
						skill->Items->Add( ab->name );
				}
			}
			else if( filter->SelectedIndex == 1 ) //Misc
			{
				for each( Ability^ ab in the_list )
				{
					if( ab->tags.Count == 0 && ab != banned && ab->decoration && ab->decoration->num_owned < ab->max_level )
						skill->Items->Add( ab->name );
				}
			}
			else if( filter->SelectedIndex == 2 ) //Complete
			{
				for each( Ability^ ab in the_list )
				{
					if( ab != banned && ab->decoration && ab->decoration->num_owned >= ab->max_level )
						skill->Items->Add( ab->name );
				}
			}
			else
			{
				String^ tag = (String^)filter->SelectedItem;
				for each( Ability^ ab in the_list )
				{
					if( ab == banned || !ab->decoration || ab->decoration->num_owned >= ab->max_level )
						continue;
					for each( SkillTag^ st in ab->tags )
					{
						if( st->name == tag )
						{
							skill->Items->Add( ab->name );
							break;
						}
					}
				}
			}
			//reselect if possible
			for( int i = 0; i < skill->Items->Count; ++i )
			{
				if( (String^)skill->Items[ i ] == old_item )
				{
					skill->SelectedIndex = i;
					break;
				}
			}

			skill->EndUpdate();
			update_skills = true;
		}

		System::Void cmbSkillFilters_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		{
			UpdateFilter( cmbSkills, cmbSkillFilters, nullptr );

			btnAdd->Enabled = cmbSkills->SelectedIndex > 0 && cmbSkillFilters->SelectedIndex != 2;
		}

		System::Void cmbSkills_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		{
			if( cmbSkills->SelectedIndex == 0 )
				cmbSkills->SelectedIndex = -1;
			//else
			//	UpdateFilter( cmbSkills2, cmbSkillFilters2, Ability::FindAbility( (String^)cmbSkills1->SelectedItem ) );

			btnAdd->Enabled = cmbSkills->SelectedIndex > 0 && cmbSkillFilters->SelectedIndex != 2;
		}

		System::Void SwitchToAbility( ComboBox^ cb, Ability^ ab )
		{
			for( int i = 0; i < cb->Items->Count; ++i )
			{
				if( Ability::FindAbility( (String^)cb->Items[ i ] ) == ab )
				{
					cb->BeginUpdate();
					cb->SelectedIndex = i;
					cb->EndUpdate();
					break;
				}
			}
		}

		System::Void lstDecorations_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		{
			if( updating_decorations )
				return;

			btnDelete->Enabled = lstDecorations->SelectedIndex >= 0;

			if( lstDecorations->SelectedIndex < 0 )
				return;

			cmbSkills->BeginUpdate();
			cmbSkills->SelectedIndex = 0;

			if( lstDecorations->SelectedIndex < owned_decorations.Count )
			{
				Ability^ ability = owned_decorations[ lstDecorations->SelectedIndex ]->abilities[ 0 ]->ability;
				cmbSkillFilters->SelectedIndex = ability->decoration->num_owned < ability->max_level ? 0 : 2;
				SwitchToAbility( cmbSkills, ability );
			}
			else
			{
				cmbSkillFilters->SelectedIndex = 0;
			}
			cmbSkills->EndUpdate();
		}

		System::Void btnDeleteAll_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if( System::Windows::Forms::DialogResult::OK != MessageBox::Show( StaticString( AreYouSure ), StaticString( DeleteAllDecorationsMessage ), MessageBoxButtons::OKCancel, MessageBoxIcon::Exclamation, MessageBoxDefaultButton::Button2 ) )
				return;
			
			for each( Decoration^ deco in Decoration::static_decorations )
			{
				deco->num_owned = 0;
			}

			RefreshList( -1 );
			
			Decoration::SaveCustom();
		}

		System::Void btnImport_Click(System::Object^  sender, System::EventArgs^  e)
		{
			OpenFileDialog dlg;
			dlg.InitialDirectory = System::Environment::CurrentDirectory;
			dlg.Filter = StaticString( MyDecorationList ) + L"|mydecorations.txt";
			dlg.FilterIndex = 0;
			System::Windows::Forms::DialogResult res = dlg.ShowDialog();
			
			if( res == System::Windows::Forms::DialogResult::OK )
			{
				for each( Decoration^ deco in Decoration::static_decorations )
				{
					deco->num_owned = 0;
				}

				Decoration::LoadCustom( dlg.FileName );
				RefreshList( -1 );
				Decoration::SaveCustom();
			}
		}
		
};
}

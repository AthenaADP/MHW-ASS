#pragma once
#include "Solution.h"
#include "Armor.h"
#include "Common.h"
#include "Decoration.h"
#include "LoadedData.h"
#include "PreviewImage.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace MHWASS {

	public ref class Advanced : public System::Windows::Forms::Form
	{
	private:
		Generic::Dictionary< Button^, int > button_dict;
		System::Windows::Forms::ContextMenuStrip^  contextMenuStrip1;
		Query^ query;
		PreviewImage^ preview_pane;
		const static Color color_default = Color::Black;
		const static Color color_enabled = Color::Green;
		const static Color color_disabled = Color::Gray;
		bool manual_checking;
	public:

#pragma warning( disable : 4677 )
		Advanced( Query^ query )
		{
			manual_checking = false;
			InitializeComponent();
			this->query = query;
			this->DialogResult = System::Windows::Forms::DialogResult::Cancel;

			grpArmors->Layout += gcnew LayoutEventHandler( this, &Advanced::ArmorGroupResized);

			boxes.Add( lvHead );
			boxes.Add( lvBody );
			boxes.Add( lvArms );
			boxes.Add( lvWaist );
			boxes.Add( lvLegs );
			boxes.Add( lvCharms );

			def_buttons.Add( btnDefaultHead );
			def_buttons.Add( btnDefaultBody );
			def_buttons.Add( btnDefaultArms );
			def_buttons.Add( btnDefaultWaist );
			def_buttons.Add( btnDefaultLegs );
			def_buttons.Add( btnDefaultCharms );

			none_buttons.Add( btnNoneHead );
			none_buttons.Add( btnNoneBody );
			none_buttons.Add( btnNoneArms );
			none_buttons.Add( btnNoneWaist );
			none_buttons.Add( btnNoneLegs );
			none_buttons.Add( btnNoneCharms );

			button_dict.Add( btnDefaultHead, 0 );
			button_dict.Add( btnDefaultBody, 1 );
			button_dict.Add( btnDefaultArms, 2 );
			button_dict.Add( btnDefaultWaist, 3 );
			button_dict.Add( btnDefaultLegs, 4 );
			button_dict.Add( btnDefaultCharms, 5 );

			button_dict.Add( btnNoneHead, 0 );
			button_dict.Add( btnNoneBody, 1 );
			button_dict.Add( btnNoneArms, 2 );
			button_dict.Add( btnNoneWaist, 3 );
			button_dict.Add( btnNoneLegs, 4 );
			button_dict.Add( btnNoneCharms, 5 );

			if( query == nullptr )
				return;

			for( int i = 0; i < int( Armor::ArmorType::NumArmorTypes ); ++i )
			{
				boxes[ i ]->SuspendLayout();
				for each( Armor^ armor in query->inf_armor[ i ] )
				{
					armor->adv_index = boxes[ i ]->Items->Count;
					AddCheckedItem( boxes[ i ], armor->name, Utility::Contains( query->rel_armor[ i ], armor ), armor );
				}
				FixColumnWidth( boxes[ i ] );
				boxes[ i ]->ResumeLayout();
				boxes[ i ]->ItemChecked += gcnew ItemCheckedEventHandler( this, &Advanced::ArmorCheckBoxClicked );

				RecheckDefaultItems( boxes[ i ] );
			}

			lvCharms->SuspendLayout();
			for each( Charm^ charm in query->inf_charms )
			{
				charm->adv_index = lvCharms->Items->Count;
				AddCheckedItem( lvCharms, charm->name, Utility::Contains( %query->rel_charms, charm ), charm );
			}
			RecheckDefaultItems( lvCharms );
			FixColumnWidth( lvCharms );
			lvCharms->ResumeLayout();
			lvCharms->ItemChecked += gcnew ItemCheckedEventHandler( this, &Advanced::CharmCheckBoxClicked );

			Text = BasicString( AdvancedSearchForm );
			btnSearch->Text = StaticString( Search );
			btnCancel->Text = StaticString( Cancel );
			grpArmors->Text = StaticString( SelectArmor );
			for each( Button^ btn in none_buttons )
				btn->Text = StaticString( AdvancedNone );
			for each( Button^ btn in def_buttons )
				btn->Text = StaticString( Default );
		}

		virtual void OnShown( EventArgs^ e ) override
		{
			Form::OnShown( e );
			//only enable checkbox watching after here since
			//Windows will trigger the event after construction
			//and the logic will screw up :P
			manual_checking = true;

			//apparently this needs to happen here, not in the constructor
			for( int i = 0; i < int( Armor::ArmorType::NumArmorTypes ); ++i )
			{
				if( boxes[ i ]->Items->Count > 0 )
					boxes[ i ]->Items[ boxes[ i ]->Items->Count - 1 ]->EnsureVisible();
			}
			if( lvCharms->Items->Count > 0 )
				lvCharms->Items[ lvCharms->Items->Count - 1 ]->EnsureVisible();
		}

		void FixColumnWidth( ListView^ lv )
		{
			int max_width = 0;
			for each( ListViewItem^ item in lv->Items )
				max_width = Math::Max( max_width, TextRenderer::MeasureText( item->Text, item->Font ).Width );
			lv->Columns[ 0 ]->Width = max_width + 20;
		}

		property System::Drawing::Size DefaultSize 
		{
			virtual System::Drawing::Size get() override
			{
				return System::Drawing::Size( 1031, 587 );
			}
		}

	private: System::Windows::Forms::GroupBox^  grpArmors;
	private: System::Windows::Forms::ListView^  lvHead;
	private: System::Windows::Forms::ListView^  lvLegs;
	private: System::Windows::Forms::ListView^  lvWaist;
	private: System::Windows::Forms::ListView^  lvArms;
	private: System::Windows::Forms::ListView^  lvBody;
	private: System::Windows::Forms::ColumnHeader^  columnHead;
	private: System::Windows::Forms::ColumnHeader^  columnLegs;
	private: System::Windows::Forms::ColumnHeader^  columnWaist;
	private: System::Windows::Forms::ColumnHeader^  columnArms;
	private: System::Windows::Forms::ColumnHeader^  columnBody;
	private: System::Windows::Forms::Button^  btnNoneHead;
	private: System::Windows::Forms::Button^  btnDefaultHead;
	private: System::Windows::Forms::Button^  btnNoneBody;
	private: System::Windows::Forms::Button^  btnDefaultBody;
	private: System::Windows::Forms::Button^  btnNoneLegs;
	private: System::Windows::Forms::Button^  btnDefaultLegs;
	private: System::Windows::Forms::Button^  btnNoneWaist;
	private: System::Windows::Forms::Button^  btnDefaultWaist;
	private: System::Windows::Forms::Button^  btnNoneArms;
	private: System::Windows::Forms::Button^  btnDefaultArms;
	private: System::Windows::Forms::Button^  btnNoneCharms;
	private: System::Windows::Forms::Button^  btnDefaultCharms;
	private: System::Windows::Forms::ListView^  lvCharms;
	private: System::Windows::Forms::ColumnHeader^  columnHeader1;
	private: System::Windows::Forms::Button^  btnSearch;
	private: System::Windows::Forms::Button^  btnCancel;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	public: 
		List_t< ListView^ > boxes;
		List_t< Button^ > def_buttons, none_buttons;

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Advanced()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = ( gcnew System::ComponentModel::Container() );
			this->btnSearch = ( gcnew System::Windows::Forms::Button() );
			this->btnCancel = ( gcnew System::Windows::Forms::Button() );
			this->groupBox1 = ( gcnew System::Windows::Forms::GroupBox() );
			this->lvHead = ( gcnew System::Windows::Forms::ListView() );
			this->columnHead = ( gcnew System::Windows::Forms::ColumnHeader() );
			this->contextMenuStrip1 = ( gcnew System::Windows::Forms::ContextMenuStrip( this->components ) );
			this->grpArmors = ( gcnew System::Windows::Forms::GroupBox() );
			this->btnNoneCharms = ( gcnew System::Windows::Forms::Button() );
			this->btnDefaultCharms = ( gcnew System::Windows::Forms::Button() );
			this->lvCharms = ( gcnew System::Windows::Forms::ListView() );
			this->columnHeader1 = ( gcnew System::Windows::Forms::ColumnHeader() );
			this->btnNoneLegs = ( gcnew System::Windows::Forms::Button() );
			this->btnDefaultLegs = ( gcnew System::Windows::Forms::Button() );
			this->btnNoneWaist = ( gcnew System::Windows::Forms::Button() );
			this->btnDefaultWaist = ( gcnew System::Windows::Forms::Button() );
			this->btnNoneArms = ( gcnew System::Windows::Forms::Button() );
			this->btnDefaultArms = ( gcnew System::Windows::Forms::Button() );
			this->btnNoneBody = ( gcnew System::Windows::Forms::Button() );
			this->btnDefaultBody = ( gcnew System::Windows::Forms::Button() );
			this->btnNoneHead = ( gcnew System::Windows::Forms::Button() );
			this->btnDefaultHead = ( gcnew System::Windows::Forms::Button() );
			this->lvLegs = ( gcnew System::Windows::Forms::ListView() );
			this->columnLegs = ( gcnew System::Windows::Forms::ColumnHeader() );
			this->lvWaist = ( gcnew System::Windows::Forms::ListView() );
			this->columnWaist = ( gcnew System::Windows::Forms::ColumnHeader() );
			this->lvArms = ( gcnew System::Windows::Forms::ListView() );
			this->columnArms = ( gcnew System::Windows::Forms::ColumnHeader() );
			this->lvBody = ( gcnew System::Windows::Forms::ListView() );
			this->columnBody = ( gcnew System::Windows::Forms::ColumnHeader() );
			this->groupBox1->SuspendLayout();
			this->grpArmors->SuspendLayout();
			this->SuspendLayout();
			// 
			// btnSearch
			// 
			this->btnSearch->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom )
				| System::Windows::Forms::AnchorStyles::Left ) );
			this->btnSearch->Location = System::Drawing::Point( 6, 13 );
			this->btnSearch->Name = L"btnSearch";
			this->btnSearch->Size = System::Drawing::Size( 75, 26 );
			this->btnSearch->TabIndex = 10;
			this->btnSearch->Text = L"&Search";
			this->btnSearch->UseVisualStyleBackColor = true;
			this->btnSearch->Click += gcnew System::EventHandler( this, &Advanced::btnSearch_Click );
			// 
			// btnCancel
			// 
			this->btnCancel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom )
				| System::Windows::Forms::AnchorStyles::Left )
				| System::Windows::Forms::AnchorStyles::Right ) );
			this->btnCancel->Location = System::Drawing::Point( 87, 13 );
			this->btnCancel->Name = L"btnCancel";
			this->btnCancel->Size = System::Drawing::Size( 75, 26 );
			this->btnCancel->TabIndex = 11;
			this->btnCancel->Text = L"&Cancel";
			this->btnCancel->UseVisualStyleBackColor = true;
			this->btnCancel->Click += gcnew System::EventHandler( this, &Advanced::btnCancel_Click );
			// 
			// groupBox1
			// 
			this->groupBox1->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->groupBox1->Controls->Add( this->btnSearch );
			this->groupBox1->Controls->Add( this->btnCancel );
			this->groupBox1->Location = System::Drawing::Point( 433, 491 );
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size( 168, 47 );
			this->groupBox1->TabIndex = 13;
			this->groupBox1->TabStop = false;
			// 
			// lvHead
			// 
			this->lvHead->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom ) );
			this->lvHead->CheckBoxes = true;
			this->lvHead->Columns->AddRange( gcnew cli::array< System::Windows::Forms::ColumnHeader^  >( 1 ) { this->columnHead } );
			this->lvHead->ContextMenuStrip = this->contextMenuStrip1;
			this->lvHead->HeaderStyle = System::Windows::Forms::ColumnHeaderStyle::None;
			this->lvHead->LabelWrap = false;
			this->lvHead->Location = System::Drawing::Point( 6, 19 );
			this->lvHead->MultiSelect = false;
			this->lvHead->Name = L"lvHead";
			this->lvHead->Size = System::Drawing::Size( 158, 405 );
			this->lvHead->TabIndex = 0;
			this->lvHead->UseCompatibleStateImageBehavior = false;
			this->lvHead->View = System::Windows::Forms::View::Details;
			// 
			// columnHead
			// 
			this->columnHead->Text = L"";
			this->columnHead->Width = 137;
			// 
			// contextMenuStrip1
			// 
			this->contextMenuStrip1->Name = L"contextMenuStrip1";
			this->contextMenuStrip1->Size = System::Drawing::Size( 61, 4 );
			this->contextMenuStrip1->Text = L"asdfasdf";
			this->contextMenuStrip1->Opening += gcnew System::ComponentModel::CancelEventHandler( this, &Advanced::contextMenuStrip1_Opening );
			// 
			// grpArmors
			// 
			this->grpArmors->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom )
				| System::Windows::Forms::AnchorStyles::Left )
				| System::Windows::Forms::AnchorStyles::Right ) );
			this->grpArmors->Controls->Add( this->btnNoneCharms );
			this->grpArmors->Controls->Add( this->btnDefaultCharms );
			this->grpArmors->Controls->Add( this->lvCharms );
			this->grpArmors->Controls->Add( this->btnNoneLegs );
			this->grpArmors->Controls->Add( this->btnDefaultLegs );
			this->grpArmors->Controls->Add( this->btnNoneWaist );
			this->grpArmors->Controls->Add( this->btnDefaultWaist );
			this->grpArmors->Controls->Add( this->btnNoneArms );
			this->grpArmors->Controls->Add( this->btnDefaultArms );
			this->grpArmors->Controls->Add( this->btnNoneBody );
			this->grpArmors->Controls->Add( this->btnDefaultBody );
			this->grpArmors->Controls->Add( this->btnNoneHead );
			this->grpArmors->Controls->Add( this->btnDefaultHead );
			this->grpArmors->Controls->Add( this->lvLegs );
			this->grpArmors->Controls->Add( this->lvWaist );
			this->grpArmors->Controls->Add( this->lvArms );
			this->grpArmors->Controls->Add( this->lvBody );
			this->grpArmors->Controls->Add( this->lvHead );
			this->grpArmors->Location = System::Drawing::Point( 12, 12 );
			this->grpArmors->Name = L"grpArmors";
			this->grpArmors->Size = System::Drawing::Size( 992, 476 );
			this->grpArmors->TabIndex = 12;
			this->grpArmors->TabStop = false;
			this->grpArmors->Text = L"Select Armor";
			// 
			// btnNoneCharms
			// 
			this->btnNoneCharms->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->btnNoneCharms->Location = System::Drawing::Point( 908, 430 );
			this->btnNoneCharms->Name = L"btnNoneCharms";
			this->btnNoneCharms->Size = System::Drawing::Size( 76, 26 );
			this->btnNoneCharms->TabIndex = 17;
			this->btnNoneCharms->Text = L"&None";
			this->btnNoneCharms->UseVisualStyleBackColor = true;
			this->btnNoneCharms->Click += gcnew System::EventHandler( this, &Advanced::ClearChecked );
			// 
			// btnDefaultCharms
			// 
			this->btnDefaultCharms->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->btnDefaultCharms->Location = System::Drawing::Point( 826, 430 );
			this->btnDefaultCharms->Name = L"btnDefaultCharms";
			this->btnDefaultCharms->Size = System::Drawing::Size( 76, 26 );
			this->btnDefaultCharms->TabIndex = 16;
			this->btnDefaultCharms->Text = L"&Default";
			this->btnDefaultCharms->UseVisualStyleBackColor = true;
			this->btnDefaultCharms->Click += gcnew System::EventHandler( this, &Advanced::DefaultChecked );
			// 
			// lvCharms
			// 
			this->lvCharms->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom ) );
			this->lvCharms->CheckBoxes = true;
			this->lvCharms->Columns->AddRange( gcnew cli::array< System::Windows::Forms::ColumnHeader^  >( 1 ) { this->columnHeader1 } );
			this->lvCharms->ContextMenuStrip = this->contextMenuStrip1;
			this->lvCharms->HeaderStyle = System::Windows::Forms::ColumnHeaderStyle::None;
			this->lvCharms->LabelWrap = false;
			this->lvCharms->Location = System::Drawing::Point( 826, 19 );
			this->lvCharms->MultiSelect = false;
			this->lvCharms->Name = L"lvCharms";
			this->lvCharms->Size = System::Drawing::Size( 158, 405 );
			this->lvCharms->TabIndex = 15;
			this->lvCharms->UseCompatibleStateImageBehavior = false;
			this->lvCharms->View = System::Windows::Forms::View::Details;
			// 
			// columnHeader1
			// 
			this->columnHeader1->Width = 137;
			// 
			// btnNoneLegs
			// 
			this->btnNoneLegs->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->btnNoneLegs->Location = System::Drawing::Point( 744, 430 );
			this->btnNoneLegs->Name = L"btnNoneLegs";
			this->btnNoneLegs->Size = System::Drawing::Size( 76, 26 );
			this->btnNoneLegs->TabIndex = 14;
			this->btnNoneLegs->Text = L"&None";
			this->btnNoneLegs->UseVisualStyleBackColor = true;
			this->btnNoneLegs->Click += gcnew System::EventHandler( this, &Advanced::ClearChecked );
			// 
			// btnDefaultLegs
			// 
			this->btnDefaultLegs->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->btnDefaultLegs->Location = System::Drawing::Point( 662, 430 );
			this->btnDefaultLegs->Name = L"btnDefaultLegs";
			this->btnDefaultLegs->Size = System::Drawing::Size( 76, 26 );
			this->btnDefaultLegs->TabIndex = 13;
			this->btnDefaultLegs->Text = L"&Default";
			this->btnDefaultLegs->UseVisualStyleBackColor = true;
			this->btnDefaultLegs->Click += gcnew System::EventHandler( this, &Advanced::DefaultChecked );
			// 
			// btnNoneWaist
			// 
			this->btnNoneWaist->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->btnNoneWaist->Location = System::Drawing::Point( 580, 430 );
			this->btnNoneWaist->Name = L"btnNoneWaist";
			this->btnNoneWaist->Size = System::Drawing::Size( 76, 26 );
			this->btnNoneWaist->TabIndex = 12;
			this->btnNoneWaist->Text = L"&None";
			this->btnNoneWaist->UseVisualStyleBackColor = true;
			this->btnNoneWaist->Click += gcnew System::EventHandler( this, &Advanced::ClearChecked );
			// 
			// btnDefaultWaist
			// 
			this->btnDefaultWaist->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->btnDefaultWaist->Location = System::Drawing::Point( 498, 430 );
			this->btnDefaultWaist->Name = L"btnDefaultWaist";
			this->btnDefaultWaist->Size = System::Drawing::Size( 76, 26 );
			this->btnDefaultWaist->TabIndex = 11;
			this->btnDefaultWaist->Text = L"&Default";
			this->btnDefaultWaist->UseVisualStyleBackColor = true;
			this->btnDefaultWaist->Click += gcnew System::EventHandler( this, &Advanced::DefaultChecked );
			// 
			// btnNoneArms
			// 
			this->btnNoneArms->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->btnNoneArms->Location = System::Drawing::Point( 416, 430 );
			this->btnNoneArms->Name = L"btnNoneArms";
			this->btnNoneArms->Size = System::Drawing::Size( 76, 26 );
			this->btnNoneArms->TabIndex = 10;
			this->btnNoneArms->Text = L"&None";
			this->btnNoneArms->UseVisualStyleBackColor = true;
			this->btnNoneArms->Click += gcnew System::EventHandler( this, &Advanced::ClearChecked );
			// 
			// btnDefaultArms
			// 
			this->btnDefaultArms->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->btnDefaultArms->Location = System::Drawing::Point( 334, 430 );
			this->btnDefaultArms->Name = L"btnDefaultArms";
			this->btnDefaultArms->Size = System::Drawing::Size( 76, 26 );
			this->btnDefaultArms->TabIndex = 9;
			this->btnDefaultArms->Text = L"&Default";
			this->btnDefaultArms->UseVisualStyleBackColor = true;
			this->btnDefaultArms->Click += gcnew System::EventHandler( this, &Advanced::DefaultChecked );
			// 
			// btnNoneBody
			// 
			this->btnNoneBody->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->btnNoneBody->Location = System::Drawing::Point( 252, 430 );
			this->btnNoneBody->Name = L"btnNoneBody";
			this->btnNoneBody->Size = System::Drawing::Size( 76, 26 );
			this->btnNoneBody->TabIndex = 8;
			this->btnNoneBody->Text = L"&None";
			this->btnNoneBody->UseVisualStyleBackColor = true;
			this->btnNoneBody->Click += gcnew System::EventHandler( this, &Advanced::ClearChecked );
			// 
			// btnDefaultBody
			// 
			this->btnDefaultBody->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->btnDefaultBody->Location = System::Drawing::Point( 170, 430 );
			this->btnDefaultBody->Name = L"btnDefaultBody";
			this->btnDefaultBody->Size = System::Drawing::Size( 76, 26 );
			this->btnDefaultBody->TabIndex = 7;
			this->btnDefaultBody->Text = L"&Default";
			this->btnDefaultBody->UseVisualStyleBackColor = true;
			this->btnDefaultBody->Click += gcnew System::EventHandler( this, &Advanced::DefaultChecked );
			// 
			// btnNoneHead
			// 
			this->btnNoneHead->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->btnNoneHead->Location = System::Drawing::Point( 88, 430 );
			this->btnNoneHead->Name = L"btnNoneHead";
			this->btnNoneHead->Size = System::Drawing::Size( 76, 26 );
			this->btnNoneHead->TabIndex = 6;
			this->btnNoneHead->Text = L"&None";
			this->btnNoneHead->UseVisualStyleBackColor = true;
			this->btnNoneHead->Click += gcnew System::EventHandler( this, &Advanced::ClearChecked );
			// 
			// btnDefaultHead
			// 
			this->btnDefaultHead->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->btnDefaultHead->Location = System::Drawing::Point( 6, 430 );
			this->btnDefaultHead->Name = L"btnDefaultHead";
			this->btnDefaultHead->Size = System::Drawing::Size( 76, 26 );
			this->btnDefaultHead->TabIndex = 5;
			this->btnDefaultHead->Text = L"&Default";
			this->btnDefaultHead->UseVisualStyleBackColor = true;
			this->btnDefaultHead->Click += gcnew System::EventHandler( this, &Advanced::DefaultChecked );
			// 
			// lvLegs
			// 
			this->lvLegs->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom ) );
			this->lvLegs->CheckBoxes = true;
			this->lvLegs->Columns->AddRange( gcnew cli::array< System::Windows::Forms::ColumnHeader^  >( 1 ) { this->columnLegs } );
			this->lvLegs->ContextMenuStrip = this->contextMenuStrip1;
			this->lvLegs->HeaderStyle = System::Windows::Forms::ColumnHeaderStyle::None;
			this->lvLegs->LabelWrap = false;
			this->lvLegs->Location = System::Drawing::Point( 662, 19 );
			this->lvLegs->MultiSelect = false;
			this->lvLegs->Name = L"lvLegs";
			this->lvLegs->Size = System::Drawing::Size( 158, 405 );
			this->lvLegs->TabIndex = 4;
			this->lvLegs->UseCompatibleStateImageBehavior = false;
			this->lvLegs->View = System::Windows::Forms::View::Details;
			// 
			// columnLegs
			// 
			this->columnLegs->Width = 137;
			// 
			// lvWaist
			// 
			this->lvWaist->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom ) );
			this->lvWaist->CheckBoxes = true;
			this->lvWaist->Columns->AddRange( gcnew cli::array< System::Windows::Forms::ColumnHeader^  >( 1 ) { this->columnWaist } );
			this->lvWaist->ContextMenuStrip = this->contextMenuStrip1;
			this->lvWaist->HeaderStyle = System::Windows::Forms::ColumnHeaderStyle::None;
			this->lvWaist->LabelWrap = false;
			this->lvWaist->Location = System::Drawing::Point( 498, 19 );
			this->lvWaist->MultiSelect = false;
			this->lvWaist->Name = L"lvWaist";
			this->lvWaist->Size = System::Drawing::Size( 158, 405 );
			this->lvWaist->TabIndex = 3;
			this->lvWaist->UseCompatibleStateImageBehavior = false;
			this->lvWaist->View = System::Windows::Forms::View::Details;
			// 
			// columnWaist
			// 
			this->columnWaist->Width = 137;
			// 
			// lvArms
			// 
			this->lvArms->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom ) );
			this->lvArms->CheckBoxes = true;
			this->lvArms->Columns->AddRange( gcnew cli::array< System::Windows::Forms::ColumnHeader^  >( 1 ) { this->columnArms } );
			this->lvArms->ContextMenuStrip = this->contextMenuStrip1;
			this->lvArms->HeaderStyle = System::Windows::Forms::ColumnHeaderStyle::None;
			this->lvArms->LabelWrap = false;
			this->lvArms->Location = System::Drawing::Point( 334, 19 );
			this->lvArms->MultiSelect = false;
			this->lvArms->Name = L"lvArms";
			this->lvArms->Size = System::Drawing::Size( 158, 405 );
			this->lvArms->TabIndex = 2;
			this->lvArms->UseCompatibleStateImageBehavior = false;
			this->lvArms->View = System::Windows::Forms::View::Details;
			// 
			// columnArms
			// 
			this->columnArms->Width = 137;
			// 
			// lvBody
			// 
			this->lvBody->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom ) );
			this->lvBody->CheckBoxes = true;
			this->lvBody->Columns->AddRange( gcnew cli::array< System::Windows::Forms::ColumnHeader^  >( 1 ) { this->columnBody } );
			this->lvBody->ContextMenuStrip = this->contextMenuStrip1;
			this->lvBody->HeaderStyle = System::Windows::Forms::ColumnHeaderStyle::None;
			this->lvBody->LabelWrap = false;
			this->lvBody->Location = System::Drawing::Point( 170, 19 );
			this->lvBody->MultiSelect = false;
			this->lvBody->Name = L"lvBody";
			this->lvBody->Size = System::Drawing::Size( 158, 405 );
			this->lvBody->TabIndex = 1;
			this->lvBody->UseCompatibleStateImageBehavior = false;
			this->lvBody->View = System::Windows::Forms::View::Details;
			// 
			// columnBody
			// 
			this->columnBody->Width = 137;
			// 
			// Advanced
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF( 6, 13 );
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size( 1015, 549 );
			this->Controls->Add( this->groupBox1 );
			this->Controls->Add( this->grpArmors );
			this->Name = L"Advanced";
			this->Text = L"Advanced Search";
			this->groupBox1->ResumeLayout( false );
			this->grpArmors->ResumeLayout( false );
			this->ResumeLayout( false );

		}
#pragma endregion

		System::Void ArmorGroupResized( System::Object^ sender, LayoutEventArgs^ e )
		{
			if( grpArmors->Size.Width <= 36 ) return;
			const int new_column_width = ( grpArmors->Size.Width - 42 ) / 6;
			grpArmors->SuspendLayout();
			for( int i = 0; i < 6; ++i )
			{
				const int new_x = 6 + ( 6 + new_column_width ) * i;
				boxes[ i ]->SuspendLayout();
				boxes[ i ]->SetBounds( new_x, boxes[ i ]->Location.Y, new_column_width, grpArmors->Size.Height - 72 );

				def_buttons[ i ]->Location.X = new_x;
				none_buttons[ i ]->Location.X = new_x + new_column_width / 2 + 3;
				def_buttons[ i ]->SetBounds( new_x, grpArmors->Size.Height - 48, new_column_width / 2 - 3, def_buttons[ i ]->Size.Height );
				none_buttons[ i ]->SetBounds( new_x + new_column_width / 2 + 3, grpArmors->Size.Height - 48, new_column_width / 2 - 3, none_buttons[ i ]->Size.Height );
				boxes[ i ]->ResumeLayout();
			}
			grpArmors->ResumeLayout();
		}

		System::Void btnSearch_Click(System::Object^  sender, System::EventArgs^  e)
		{
			this->DialogResult = System::Windows::Forms::DialogResult::OK;
			Close();
		}

		System::Void btnCancel_Click(System::Object^  sender, System::EventArgs^  e)
		{
			this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			Close();
		}

		System::Void AddCheckedItem( ListView^ lv, String^ name, const bool checked, AdvancedSearchOptions^ tag )
		{
			tag->default_piece = checked;

			ListViewItem^ item = gcnew ListViewItem( name );
			item->Checked = ( checked || tag->force_enable ) && !tag->force_disable;
			item->Tag = tag;
			item->ForeColor = tag->force_enable ? color_enabled : tag->force_disable ? color_disabled : color_default;
			if( checked )
				item->Font = gcnew Drawing::Font( item->Font->Name, item->Font->Size, item->Font->Style | Drawing::FontStyle::Bold );
			lv->Items->Add( item );
		}

		System::Void ClearChecked( System::Object^ sender, System::EventArgs^ e )
		{
			manual_checking = false;
			ListView^ lv = boxes[ button_dict[ (Button^)sender ] ];
			lv->BeginUpdate();
			for each( ListViewItem^ item in lv->Items )
			{
				item->Checked = false;
				item->ForeColor = color_disabled;
				AdvancedSearchOptions^ op = safe_cast< AdvancedSearchOptions^ >( item->Tag );
				op->force_disable = true;
				op->force_enable = false;

			}
			lv->EndUpdate();
			manual_checking = true;
		}

		System::Void DefaultChecked( System::Object^ sender, System::EventArgs^ e )
		{
			manual_checking = false;
			ListView^ lv = boxes[ button_dict[ (Button^)sender ] ];
			lv->BeginUpdate();
			for each( ListViewItem^ item in lv->Items )
			{
				item->Checked = true;
				item->ForeColor = color_default;
				AdvancedSearchOptions^ op = safe_cast< AdvancedSearchOptions^ >( item->Tag );
				op->force_disable = false;
				op->force_enable = false;
			}
			lv->EndUpdate();
			manual_checking = true;
		}

	private:

		System::Void RecalculateDefaults( ListView^ lv )
		{
			List_t< Charm^ > inf, rel;
			for( int i = 0; i < lv->Items->Count; ++i )
			{
				Charm^ item = safe_cast< Charm^ >( lv->Items[ i ]->Tag );
				AddToList( %rel, item, %query->rel_abilities, %inf, true );
			}

			//check for anything to enable
			for each( Charm^ item in rel )
			{
				if( !item->force_enable && !item->force_disable && !lv->Items[ item->adv_index ]->Checked )
					lv->Items[ item->adv_index ]->Checked = true;
			}
			//check for anything to disable
			for each( Charm^ item in inf )
			{
				if( !item->force_enable && !item->force_disable && lv->Items[ item->adv_index ]->Checked && !Utility::Contains( %rel, item ) )
					lv->Items[ item->adv_index ]->Checked = false;
			}
		}

		System::Void RecheckDefaultItems( System::Object^ sender )
		{
			if( sender == lvCharms )
			{
				RecalculateDefaults( lvCharms );
			}
			else if( sender != nullptr )
			{
				ListView^ lv = safe_cast< ListView^ >( sender );
				for each( ListViewItem^ item in lv->Items )
				{
					AdvancedSearchOptions^ ad = safe_cast< AdvancedSearchOptions^ >( item->Tag );
					item->Checked = !ad->force_disable;
				}
			}
		}

		System::Void CharmCheckBoxClicked( System::Object^ sender, ItemCheckedEventArgs^ e )
		{
			if( !manual_checking )
				return;
			manual_checking = false;

			AdvancedSearchOptions^ op = safe_cast< AdvancedSearchOptions^ >( e->Item->Tag );
			if( e->Item->Checked )
			{
				if( op->force_disable )
				{
					e->Item->ForeColor = color_default;
				}
				else
				{
					op->force_enable = true;
					e->Item->ForeColor = color_enabled;
				}
				op->force_disable = false;
			}
			else
			{
				if( op->force_enable )
				{
					e->Item->ForeColor = color_default;
				}
				else
				{
					op->force_disable = true;
					e->Item->ForeColor = color_disabled;
				}
				op->force_enable = false;
			}

			RecheckDefaultItems( sender );

			manual_checking = true;
		}

		System::Void ArmorCheckBoxClicked( System::Object^ sender, ItemCheckedEventArgs^ e )
		{
			if( !manual_checking )
				return;
			manual_checking = false;

			AdvancedSearchOptions^ op = safe_cast<AdvancedSearchOptions^>( e->Item->Tag );
			if( e->Item->Checked )
			{
				e->Item->ForeColor = color_default;
				op->force_disable = false;
				op->force_enable = false;
			}
			else
			{
				e->Item->ForeColor = color_disabled;
				op->force_disable = true;
				op->force_enable = false;
			}

			RecheckDefaultItems( sender );

			manual_checking = true;
		}

		System::Void contextMenuStrip1_Opening(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e)
		{
			contextMenuStrip1->Items->Clear();
			e->Cancel = true;
			if( lvCharms->Focused && lvCharms->SelectedIndices->Count == 1 )
			{
				Utility::UpdateContextMenu( contextMenuStrip1, query->inf_charms[ lvCharms->SelectedIndices[ 0 ] ] );
				e->Cancel = false;
			}
			else
			{
				List_t< Armor^ > preview_armors;
				for( unsigned i = 0; i < 5; ++i )
				{
					if( boxes[ i ]->Focused && boxes[ i ]->SelectedIndices->Count == 1 )
					{
						List_t< Armor^ >^ list = query->inf_armor[ i ];
						Armor^ armor = list[ boxes[ i ]->SelectedIndices[ 0 ] ];
						Utility::UpdateContextMenu( contextMenuStrip1, armor, query->hr >= 9 );
						e->Cancel = false;

						if( preview_pane )
							preview_pane->Close();

						preview_pane = gcnew PreviewImage( query->gender == Gender::FEMALE );

						preview_armors.Add( armor );

						while( preview_armors.Count < 5 )
							preview_armors.Add( nullptr );

						if( preview_pane->SetData( preview_armors ) )
						{
							preview_pane->Show( this );

							preview_pane->SetLocation( contextMenuStrip1 );
						}
						else preview_pane = nullptr;

						break;
					}
					preview_armors.Add( nullptr );
				}
			}

		}

		void contextMenuStrip1_Closing( System::Object^ sender, ToolStripDropDownClosingEventArgs^ e )
		{
			if( preview_pane )
			{
				preview_pane->Close();
				preview_pane = nullptr;
			}
		}
	};
}

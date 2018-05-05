#pragma once
#include "Common.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace MHWASS {

	public ref class Find : public System::Windows::Forms::Form
	{
	public:
		int index, length;
		event EventHandler^ TextFound;
		event EventHandler^ DialogClosing;
		RichTextBox^ text_box;

		Find( RichTextBox^ text_box )
		{
			InitializeComponent();
			
			this->text_box = text_box;
			txtFind->KeyDown += gcnew KeyEventHandler( this, &Find::KeyDown );

			Text = BasicString( Find );
			btnFind->Text = StaticString( FindNext );
			btnClose->Text = StaticString( Close );
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Find()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  txtFind;
	private: System::Windows::Forms::Button^  btnFind;
	private: System::Windows::Forms::Button^  btnClose;


	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->txtFind = (gcnew System::Windows::Forms::TextBox());
			this->btnFind = (gcnew System::Windows::Forms::Button());
			this->btnClose = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// txtFind
			// 
			this->txtFind->Location = System::Drawing::Point(12, 12);
			this->txtFind->Name = L"txtFind";
			this->txtFind->Size = System::Drawing::Size(207, 20);
			this->txtFind->TabIndex = 0;
			// 
			// btnFind
			// 
			this->btnFind->Location = System::Drawing::Point(225, 10);
			this->btnFind->Name = L"btnFind";
			this->btnFind->Size = System::Drawing::Size(75, 23);
			this->btnFind->TabIndex = 1;
			this->btnFind->Text = L"&Find Next";
			this->btnFind->UseVisualStyleBackColor = true;
			this->btnFind->Click += gcnew System::EventHandler(this, &Find::btnFind_Click);
			// 
			// btnClose
			// 
			this->btnClose->Location = System::Drawing::Point(306, 10);
			this->btnClose->Name = L"btnClose";
			this->btnClose->Size = System::Drawing::Size(75, 23);
			this->btnClose->TabIndex = 2;
			this->btnClose->Text = L"&Close";
			this->btnClose->UseVisualStyleBackColor = true;
			this->btnClose->Click += gcnew System::EventHandler(this, &Find::btnClose_Click);
			// 
			// Find
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(393, 41);
			this->ControlBox = false;
			this->Controls->Add(this->btnClose);
			this->Controls->Add(this->btnFind);
			this->Controls->Add(this->txtFind);
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"Find";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->Text = L"Find";
			this->TopMost = true;
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		System::Void btnClose_Click(System::Object^  sender, System::EventArgs^  e)
		{
			DialogClosing( this, EventArgs::Empty );
			Close();
		}

		System::Void btnFind_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if( txtFind->Text == L"" ) return;
			int start = text_box->SelectionStart + text_box->SelectionLength;
			if( start == text_box->Text->Length ) start = 0;
			index = text_box->Text->IndexOf( txtFind->Text, start, StringComparison::CurrentCultureIgnoreCase );
			length = txtFind->Text->Length;

			TextFound( this, EventArgs::Empty );
		}

		System::Void KeyDown( System::Object^ sender, KeyEventArgs^ e )
		{
			if( e->KeyCode == Keys::Enter )
			{
				e->Handled = true;
				e->SuppressKeyPress = true;
				btnFind_Click( this, EventArgs::Empty );
			}
		}
	};
}

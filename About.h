#pragma once
#include "Version.h"
#include "Common.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace MHWASS {

	public ref class About : public System::Windows::Forms::Form
	{
	public:
		About(void)
		{
			InitializeComponent();

			lblVersion->Text = StartString( Version ) + STRINGIZE( VERSION_NO );
			label1->Text += GAMES;
			btnClose->Text = StaticString( Close );
			Text = BasicString( AboutForm );

			String^ credit = StaticString( TranslationCredit );
			if( credit != L"-" )
			{
				this->Height += 20;

				Label^ l = gcnew Label();
				l->AutoSize = true;
				l->Name = L"lblTranslation";
				l->Text = credit;
				l->Size = TextRenderer::MeasureText( l->Text, l->Font );
				l->Location = System::Drawing::Point( ( this->Width - l->Width ) / 2, 260 );

				this->Controls->Add( l );
			}

			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~About()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  lblVersion;
	private: System::Windows::Forms::Button^  btnClose;
	private: System::Windows::Forms::PictureBox^  pictureBox1;

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(About::typeid));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->lblVersion = (gcnew System::Windows::Forms::Label());
			this->btnClose = (gcnew System::Windows::Forms::Button());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->label1->Location = System::Drawing::Point(25, 213);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(244, 22);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Athena\'s Armor Set Search for ";
			this->label1->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// lblVersion
			// 
			this->lblVersion->Location = System::Drawing::Point(25, 235);
			this->lblVersion->Name = L"lblVersion";
			this->lblVersion->Size = System::Drawing::Size(244, 14);
			this->lblVersion->TabIndex = 1;
			this->lblVersion->Text = L"Version";
			this->lblVersion->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// btnClose
			// 
			this->btnClose->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->btnClose->Location = System::Drawing::Point(108, 263);
			this->btnClose->Name = L"btnClose";
			this->btnClose->Size = System::Drawing::Size(75, 23);
			this->btnClose->TabIndex = 2;
			this->btnClose->Text = L"&Close";
			this->btnClose->UseVisualStyleBackColor = true;
			this->btnClose->Click += gcnew System::EventHandler(this, &About::btnClose_Click);
			// 
			// pictureBox1
			// 
			this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox1.Image")));
			this->pictureBox1->Location = System::Drawing::Point(25, 10);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(244, 191);
			this->pictureBox1->TabIndex = 3;
			this->pictureBox1->TabStop = false;
			// 
			// About
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(293, 298);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->btnClose);
			this->Controls->Add(this->lblVersion);
			this->Controls->Add(this->label1);
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"About";
			this->Text = L"About";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: 
		System::Void btnClose_Click(System::Object^  sender, System::EventArgs^  e)
		{
			Close();
		}
	};
}

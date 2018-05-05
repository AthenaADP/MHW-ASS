#pragma once
#include "Common.h"
#include "Armor.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace MHWASS
{

	public ref class PreviewImage : public System::Windows::Forms::Form
	{
		Bitmap^ image;
		String^ path;
		bool female;
		int current_zoom;
		Control^ parent_control;

	public:
		static int zoom = 100;

		PreviewImage( const bool female )
			: female( female )
		{
			InitializeComponent();
			this->FormBorderStyle = Windows::Forms::FormBorderStyle::SizableToolWindow;
			this->MouseWheel += gcnew MouseEventHandler( this, &PreviewImage::OnMouseWheelInput );
			this->pictureBox1->MouseWheel += gcnew MouseEventHandler( this, &PreviewImage::OnMouseWheelInput );
			current_zoom = zoom;

			if( female )
				path = L"Data/Images/f_";
			else
				path = L"Data/Images/m_";
			
			String^ base_filename = path + L"base.png";
			if( IO::File::Exists( base_filename ) )
			{
				image = gcnew Bitmap( base_filename );

				this->ClientSize = System::Drawing::Size( image->Width * zoom / 200, image->Height * zoom / 200 );
			}
		}

		void SetLocation( Control^ parent_control )
		{
			this->parent_control = parent_control;
			UpdateLocation();
		}

#pragma warning( disable: 4677 )

		Bitmap^ SetData( List_t< Armor^ >% armors ) { return SetData( armors, nullptr ); }
		Bitmap^ SetData( List_t< Armor^ >% armors, Bitmap^ cached_image )
		{
			if( !image )
				return nullptr;

			if( cached_image )
			{
				pictureBox1->Image = cached_image;
				return cached_image;
			}

			Drawing::Rectangle r( 0, 0, image->Width, image->Height );
			Imaging::BitmapData^ bd = image->LockBits( r, Imaging::ImageLockMode::ReadWrite, Imaging::PixelFormat::Format32bppArgb );
			array< int >^ pix = gcnew array< int >( image->Width * image->Height );
			Runtime::InteropServices::Marshal::Copy( bd->Scan0, pix, 0, pix->Length );

			array< Byte >^ head_info  = LoadImagePack( L"head" );
			array< Byte >^ body_info  = LoadImagePack( L"body" );
			array< Byte >^ arms_info  = LoadImagePack( L"arms" );
			array< Byte >^ waist_info = LoadImagePack( L"waist" );
			array< Byte >^ legs_info  = LoadImagePack( L"legs" );

			MergeImage( head_info,  pix, armors[ (int)Armor::ArmorType::HEAD ], true );
			MergeImage( body_info,  pix, armors[ (int)Armor::ArmorType::BODY ], true );
			MergeImage( waist_info, pix, armors[ (int)Armor::ArmorType::WAIST ], true );

			MergeImage( legs_info,  pix, armors[ (int)Armor::ArmorType::LEGS ], false );
			if( !IsGammothMail( armors[ (int)Armor::ArmorType::BODY ] ) )
				MergeImage( body_info,  pix, armors[ (int)Armor::ArmorType::BODY ], false );
			MergeImage( waist_info, pix, armors[ (int)Armor::ArmorType::WAIST ], false );
			MergeImage( arms_info,  pix, armors[ (int)Armor::ArmorType::ARMS ], false );

			if( IsGammothMail( armors[ (int)Armor::ArmorType::BODY ] ) )
				MergeImage( body_info,  pix, armors[ (int)Armor::ArmorType::BODY ], false );

			MergeImage( head_info,  pix, armors[ (int)Armor::ArmorType::HEAD ], false );

			Runtime::InteropServices::Marshal::Copy( pix, 0, bd->Scan0, pix->Length );
			image->UnlockBits( bd );

			pictureBox1->Image = image;

			return image;
		}

		bool CanShow()
		{
			return !!image;
		}

	protected:

		void UpdateLocation()
		{
			if( !parent_control )
				return;

			Screen^ screen = Screen::FromControl( this );
			if( parent_control->Location.Y + this->Size.Height >= screen->WorkingArea.Bottom )
				this->Location = Point( parent_control->Location.X + parent_control->Size.Width, Math::Max( 0, screen->WorkingArea.Bottom - this->Size.Height ) );
			else
				this->Location = Point( parent_control->Location.X + parent_control->Size.Width, parent_control->Location.Y );
		}

		void OnMouseWheelInput( System::Object^ sender, MouseEventArgs^ e )
		{
			const int dz = 5 * e->Delta / 120;
			const int new_zoom = Math::Max( 50, Math::Min( 200, current_zoom + dz ) );
			if( new_zoom == current_zoom )
				return;

			current_zoom = new_zoom;
			
			this->ClientSize = System::Drawing::Size( image->Width * current_zoom / 200, image->Height * current_zoom / 200 );
			UpdateLocation();
		}

		bool IsGammothMail( Armor^ armor )
		{
			if( !armor )
				return false;

			if( female )
			{
				return  armor->female_image == 135 ||
						armor->female_image == 136 ||
						armor->female_image == 401 ||
						armor->female_image == 402;
			}
			else
			{
				return false;
			}
		}

		unsigned ReadUInt( array< Byte >^ data, const unsigned offset )
		{
			unsigned n = int(data[offset]) << 24;
			n |= int(data[offset + 1]) << 16;
			n |= int(data[offset + 2]) << 8;
			n |= int(data[offset + 3]);
			return n;
		}

		unsigned short ReadUShort( array< Byte >^ data, const unsigned offset )
		{
			unsigned short n = (unsigned short)data[offset] << 8;
			n |= (unsigned short)data[offset + 1];
			return n;
		}

		array< Byte >^ LoadImagePack( String^ subpath )
		{
			String^ filename = path + subpath + L".pak";
			if( IO::File::Exists( filename ) )
				return IO::File::ReadAllBytes( filename );
			else
				return nullptr;
		}

		Bitmap^ LoadPackedImage( array< Byte >^ data, const unsigned index, int% dx, int% dy, const bool need_alpha )
		{
			const unsigned images_offset = ReadUInt( data, 0 );
			const unsigned index_offset = index * 4;
			if( index_offset + 4 > images_offset )
				return nullptr;

			const unsigned offset = ReadUInt( data, index_offset );
			if( offset == 0xffffffff )
				return nullptr;

			dx = ReadUShort( data, images_offset + offset );
			dy = ReadUShort( data, images_offset + offset + 2 );

			if( dx & 0x8000 )
			{
				dx = dx & 0x7fff;
			}
			else if( need_alpha )
				return nullptr;

			return gcnew Bitmap( gcnew IO::MemoryStream( data, images_offset + offset + 4, data->Length - offset - images_offset - 4, false, true ) );
		}

		void MergeImage( array< Byte >^ data, array< int >^ pix, Armor^ armor, bool use_mid_alpha_only )
		{
			if( !armor || !data )
				return;

			const int index = female ? armor->female_image : armor->male_image;
			if( index < 1 )
				return;

			int dx, dy;
			Bitmap^ b = LoadPackedImage( data, index, dx, dy, use_mid_alpha_only );
			if( !b )
				return;

			Drawing::Rectangle r( 0, 0, b->Width, b->Height );
			Imaging::BitmapData^ bd = b->LockBits( r, Imaging::ImageLockMode::ReadWrite, Imaging::PixelFormat::Format32bppArgb );
			array< int >^ bpix = gcnew array< int >( b->Width * b->Height );
			Runtime::InteropServices::Marshal::Copy( bd->Scan0, bpix, 0, bpix->Length );
			b->UnlockBits( bd );

			for( int x = 0; x < b->Width; ++x )
			{
				for( int y = 0; y < b->Height; ++y )
				{
					const int c = bpix[ x + b->Width * y ];
					const unsigned char alpha = (c >> 24) & 0xff;
					if( alpha == 0 )
						continue;

					if( ( alpha < 255 ) == use_mid_alpha_only )
					{
						pix[ x + dx + image->Width * (y + dy) ] = c | 0xff000000;
					}
				}
			}
		}
		
		~PreviewImage()
		{
			if (components)
			{
				delete components;
			}
		}

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
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// pictureBox1
			// 
			this->pictureBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->pictureBox1->Location = System::Drawing::Point(0, 0);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(190, 500);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			// 
			// PreviewImage
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(190, 500);
			this->ControlBox = false;
			this->Controls->Add(this->pictureBox1);
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"PreviewImage";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->TopMost = true;
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	};
}

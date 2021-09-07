#pragma once
#include <string.h>
#include "calculate.h"

namespace Calculator {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// —водка дл€ CalculatorUI
	/// </summary>
	public ref class CalculatorUI : public System::Windows::Forms::Form
	{
	public:
		CalculatorUI(void)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
		}

	protected:
		/// <summary>
		/// ќсвободить все используемые ресурсы.
		/// </summary>
		~CalculatorUI()
		{
			if (components)
			{
				delete components;
			}
		}
  private: System::Windows::Forms::TextBox^ Expression;
  protected:
  private: System::Windows::Forms::Button^ Equal;
  private: System::Windows::Forms::TextBox^ Answer;


	private:
		/// <summary>
		/// ќб€зательна€ переменна€ конструктора.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// “ребуемый метод дл€ поддержки конструктора Ч не измен€йте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(CalculatorUI::typeid));
			this->Expression = (gcnew System::Windows::Forms::TextBox());
			this->Equal = (gcnew System::Windows::Forms::Button());
			this->Answer = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// Expression
			// 
			this->Expression->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->Expression->Location = System::Drawing::Point(13, 83);
			this->Expression->Margin = System::Windows::Forms::Padding(4);
			this->Expression->Name = L"Expression";
			this->Expression->Size = System::Drawing::Size(834, 37);
			this->Expression->TabIndex = 0;
			this->Expression->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CalculatorUI::KeyPress);
			// 
			// Equal
			// 
			this->Equal->Cursor = System::Windows::Forms::Cursors::Default;
			this->Equal->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->Equal->Location = System::Drawing::Point(855, 83);
			this->Equal->Margin = System::Windows::Forms::Padding(4);
			this->Equal->Name = L"Equal";
			this->Equal->Size = System::Drawing::Size(89, 37);
			this->Equal->TabIndex = 1;
			this->Equal->Text = L"=";
			this->Equal->UseVisualStyleBackColor = true;
			this->Equal->Click += gcnew System::EventHandler(this, &CalculatorUI::Equal_Click);
			// 
			// Answer
			// 
			this->Answer->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->Answer->Location = System::Drawing::Point(952, 83);
			this->Answer->Margin = System::Windows::Forms::Padding(4);
			this->Answer->Name = L"Answer";
			this->Answer->ReadOnly = true;
			this->Answer->Size = System::Drawing::Size(315, 37);
			this->Answer->TabIndex = 2;
			// 
			// CalculatorUI
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::GradientInactiveCaption;
			this->ClientSize = System::Drawing::Size(1306, 206);
			this->Controls->Add(this->Answer);
			this->Controls->Add(this->Equal);
			this->Controls->Add(this->Expression);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Margin = System::Windows::Forms::Padding(4);
			this->Name = L"CalculatorUI";
			this->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->Text = L"Calculator";
			this->ResumeLayout(false);
			this->PerformLayout();

		}


#pragma endregion
  private: System::Void Equal_Click(System::Object^ sender, System::EventArgs^ e) {
    error_t error = NO_ERROR;
    String^ expression = this->Expression->Text;
    using namespace Runtime::InteropServices;
    const char* c_expression = (const char*)(Marshal::StringToHGlobalAnsi(expression)).ToPointer();
    double ans = Calculate(c_expression, &error);
    if (error)
      MessageBox::Show("Error!");
    else
      this->Answer->Text = Convert::ToString(ans);
    return System::Void();
  }

	private: void KeyPress(System::Object^ sender, KeyEventArgs^ e) {
		if (e->KeyCode == Keys::Enter)
			Equal->PerformClick();
	}
};

}

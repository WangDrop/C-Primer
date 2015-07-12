//
// MainPage.xaml.cpp
// MainPage 类的实现。
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace test;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// “空白页”项模板在 http://go.microsoft.com/fwlink/?LinkId=234238 上提供

MainPage::MainPage()
{
	InitializeComponent();
}

/// <summary>
/// 在此页将要在 Frame 中显示时进行调用。
/// </summary>
/// <param name="e">描述如何访问此页的事件数据。Parameter
/// 属性通常用于配置页。</param>
void MainPage::OnNavigatedTo(NavigationEventArgs^ e)
{
	(void) e;	// 未使用的参数

	// TODO: 准备此处显示的页面。

	// TODO: 如果您的应用程序包含多个页面，请确保
	// 通过注册以下事件来处理硬件“后退”按钮:
	// Windows::Phone::UI::Input::HardwareButtons.BackPressed 事件。
	// 如果使用由某些模板提供的 NavigationHelper，
	// 则系统会为您处理该事件。
}

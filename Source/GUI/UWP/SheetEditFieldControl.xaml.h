//
// SheetViewEditFieldControl.xaml.h
// Declaration of the SheetViewEditFieldControl class
//

#pragma once

#include "SheetEditFieldControl.g.h"

namespace MediaInfo
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class SheetEditFieldControl sealed
    {
    public:
        SheetEditFieldControl(Platform::String^ Name, size_t Width, size_t StreamKind, Platform::String^ Parameter);
        Platform::String^ Get_Name();
        size_t Get_Width();
        size_t Get_StreamKind();
        Platform::String^ Get_Parameter();

    private:
        void Get_Streams();
        void Get_Parameters();

        static Platform::Collections::Map<size_t, Platform::String^>^ _Streams;
        static Platform::Collections::Map<size_t, Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^>^ _Parameters;
        void StreamBox_SelectionChanged(Platform::Object^ Sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ Event);
        void ParameterBox_SelectionChanged(Platform::Object^ Sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ Event);
    };
}

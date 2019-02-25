//
// SheetEditDialog.xaml.h
// Déclaration de la classe SheetEditDialog
//

#pragma once

#include "SheetEditDialog.g.h"

namespace MediaInfo
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class SheetEditDialog sealed
    {
    public:
        SheetEditDialog(Platform::String^ Title, Sheet^ Content);
    private:
        void Add_Field(Platform::String^ Name, size_t Width, size_t StreamKind, Platform::String^ Parameter);
        void ContentDialog_PrimaryButtonClick(Windows::UI::Xaml::Controls::ContentDialog^ Sender, Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs^ Event);
        void ContentDialog_SecondaryButtonClick(Windows::UI::Xaml::Controls::ContentDialog^ Sender, Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs^ Event);

        Sheet^ _Sheet;
    };
}

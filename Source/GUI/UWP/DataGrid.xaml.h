//
// DataGrid.xaml.h
// Declaration of the DataGrid class
//

#pragma once

#include "DataGrid.g.h"

namespace MediaInfo
{
    ref class DataGrid;
    [Windows::Foundation::Metadata::WebHostHidden]
    public delegate void SelectionChangedEventHandler(DataGrid^ Sender, unsigned int Pos);

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class DataGrid sealed
    {
    public:
        DataGrid();
        void Reset(); // Delete cell content and rows source
        void Clear(); // Delete only cell content
        void Add_Column(Windows::UI::Xaml::GridLength Width, Platform::String^ Header);
        void Set_Content(unsigned int Row, unsigned int Column, Platform::String^ Content);

        property Windows::Foundation::Collections::IObservableVector<Windows::Foundation::Collections::IVectorView<Platform::String^>^>^ Rows
        {
            void set(Windows::Foundation::Collections::IObservableVector<Windows::Foundation::Collections::IVectorView<Platform::String^>^>^ Value);
        }

        event SelectionChangedEventHandler^ SelectionChangedEvent; //then fire the event: SelectionChangedEvent(this, 0);
    private:
        void Display();
        Windows::UI::Xaml::Controls::RelativePanel^ Add_Cell(unsigned int Row, unsigned int Column, Platform::String^ Content);
        void Cell_Clicked(Platform::Object^ Sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ Event);
        void ResizeHandle_PointerEntered(Platform::Object^ Sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ Event);
        void ResizeHandle_PointerPressed(Platform::Object^ Sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ Event);
        void ResizeHandle_PointerReleased(Platform::Object^ Sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ Event);
        void ResizeHandle_PointerMoved(Platform::Object^ Sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ Event);
        void ResizeHandle_PointerExited(Platform::Object^ Sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ Event);

        Platform::Collections::Vector<Windows::UI::Xaml::Controls::ColumnDefinition^>^ _Columns;
        Platform::Collections::Vector<Platform::String^>^ _ColumnsTitles;
        Platform::Collections::Vector<Windows::UI::Xaml::Shapes::Rectangle^>^ _RowsBackgrounds;
        unsigned int _SelectedRow;
        Platform::Collections::Vector<Windows::Foundation::Collections::IVectorView<Platform::String^>^>^ _Rows;
        Platform::Collections::Vector<Windows::Foundation::Collections::IVectorView<Platform::String^>^>^ _SortedRows;
        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::Collections::IVectorView<Platform::String^>^>^ _OriginalRows; // keep this reference to unregister the VectorChanged token later
        bool _Resizing;
        uint16 _Pointer_Count;
        Windows::UI::Core::CoreCursor^ _Old_Pointer;
        Windows::Foundation::EventRegistrationToken _RowsVectorChangedToken;

    };
}
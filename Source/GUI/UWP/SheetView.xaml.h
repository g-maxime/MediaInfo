/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license that can
*  be found in the License.html file in the root of the source tree.
*/

#pragma once

#include "SheetView.g.h"

//---------------------------------------------------------------------------
namespace MediaInfo
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class SheetView sealed
    {
    public:
        SheetView(ReportViewModel^ CurrentReport);

    private:
        ~SheetView();
        void Configure();
        void Save_Sheets();
        void Get_Saved_Sheets();
        void Get_Streams();
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ Get_Values(ReportViewModel^ Source);
        void ReportDataSource_VectorChanged(Windows::Foundation::Collections::IObservableVector<Report^>^ Sender, Windows::Foundation::Collections::IVectorChangedEventArgs^ Event);
        void StreamBox_SelectionChanged(Platform::Object^ Sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ Event);
        void FormatBox_SelectionChanged(Platform::Object^ Sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ Event);
        void EditSheet_Click(Platform::Object^ Sender, Windows::UI::Xaml::RoutedEventArgs^ Event);
        void AddSheet_Click(Platform::Object^ Sender, Windows::UI::Xaml::RoutedEventArgs^ Event);
        void DeleteSheet_Click(Platform::Object^ Sender, Windows::UI::Xaml::RoutedEventArgs^ Event);

        ReportViewModel^ _CurrentReport;
        Sheet^ _CurrentSheet;
        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::Collections::IVectorView<Platform::String^>^>^ _Reports;
        Windows::Foundation::Collections::IObservableVector<Sheet^>^ _Sheets;

        Windows::Foundation::EventRegistrationToken _ReportVectorChangedToken;
        void MainGrid_SelectionChangedEvent(DataGrid^ Sender, uint32 Pos);
    };
}

//
// SheetView.xaml.cpp
// Implementation of the SheetView class
//

#include "pch.h"
#include "SheetView.xaml.h"

#include <MediaInfo/MediaInfo_Internal.h>
#include <ZenLib/Ztring.h>
#include <ZenLib/ZtringList.h>
#include <ZenLib/ZtringListList.h>

using namespace MediaInfoLib;
using namespace MediaInfo;
using namespace ZenLib;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::Storage;
using namespace Windows::Storage::AccessCache;
using namespace Windows::Storage::Streams;


SheetView::SheetView(ReportViewModel^ CurrentReport) : _CurrentReport(CurrentReport), _Sheets(ref new Vector<Sheet^>()), _Reports(ref new Vector<IVectorView<Platform::String^>^>()), _ReportVectorChangedToken(EventRegistrationToken())
{
    InitializeComponent();

    Get_Saved_Sheets();

    _ReportVectorChangedToken=ReportDataSource::GetAllReports()->VectorChanged+=ref new VectorChangedEventHandler<Report^>(this, &SheetView::ReportDataSource_VectorChanged);
}

//---------------------------------------------------------------------------
SheetView::~SheetView()
{
    ReportDataSource::GetAllReports()->VectorChanged-=_ReportVectorChangedToken;
}

//---------------------------------------------------------------------------
void SheetView::ReportDataSource_VectorChanged(IObservableVector<Report^>^ Sender, IVectorChangedEventArgs^ Event)
{
    switch (Event->CollectionChange)
    {
    case CollectionChange::ItemInserted:
        if (Event->Index<=_Reports->Size)
            _Reports->InsertAt(Event->Index, Get_Values(ref new ReportViewModel(ReportDataSource::GetReportAt(Event->Index))));
        break;
    case CollectionChange::ItemRemoved:
        if (Event->Index<_Reports->Size)
            _Reports->RemoveAt(Event->Index);
        break;
    case CollectionChange::Reset:
        _Reports->Clear();
        for (Report^ Cur:ReportDataSource::GetAllReports())
            _Reports->Append(Get_Values(ref new ReportViewModel(Cur)));
        break;
    }
}

//---------------------------------------------------------------------------
void SheetView::Configure()
{
    MainGrid->Reset();

    _Reports->Clear();
    for (Report^ Cur:ReportDataSource::GetAllReports())
        _Reports->Append(Get_Values(ref new ReportViewModel(Cur)));

    for (Field^ Cur : _CurrentSheet->Fields)
    {
        GridLength Width=GridLengthHelper::Auto;
        if (Cur->Length!=0)
            Width=Cur->Length;

        MainGrid->Add_Column(Width, Cur->Title);
    }
    MainGrid->Rows=_Reports;
}

//---------------------------------------------------------------------------
void SheetView::Save_Sheets()
{
      ApplicationDataContainer^ LocalSettings=ApplicationData::Current->LocalSettings;
      ApplicationDataCompositeValue^ Composite=ref new ApplicationDataCompositeValue();

      for (Sheet^ Cur : _Sheets)
      {
          Platform::String^ Name=Cur->Name;

          ZtringListList Fields;
          for (Field^ Cur2 : Cur->Fields)
          {
              ZtringList Z1;
              Z1.Insert(Cur2->Title->Data(), 0);
              Z1.Insert(Ztring().From_Number(Cur2->StreamKind), 1);
              Z1.Insert(Cur2->Parameter->Data(), 2);
              Z1.Insert(Ztring().From_Number(Cur2->Length), 3);

              Fields.Insert(Z1, Fields.size());
          }

          Composite->Insert(Cur->Name, ref new Platform::String(Fields.Read().c_str()));
      }

      LocalSettings->Values->Insert(L"Sheets", Composite);
}

//---------------------------------------------------------------------------
void SheetView::Get_Saved_Sheets()
{
    ApplicationDataContainer^ LocalSettings=ApplicationData::Current->LocalSettings;
    Object^ Obj=LocalSettings->Values->Lookup(L"Sheets");

    if (!Obj)
    {
        // Default sheet
        Vector<Field^>^ Fields=ref new Vector<Field^>();
        Fields->Append(ref new Field("File Name", 0, "FileName", 0));
        Fields->Append(ref new Field("File Size", 0, "FileSize", 0));
        Fields->Append(ref new Field("Video Streams", 0, "VideoCount", 0));
        Fields->Append(ref new Field("Audio Streams", 0, "AudioCount", 0));
        _Sheets->Append(ref new Sheet("Default", Fields));
    }
    else {
        ApplicationDataCompositeValue^ Composite=safe_cast<ApplicationDataCompositeValue^>(Obj);
        for (IKeyValuePair<Platform::String^, Platform::Object^>^ Cur : Composite)
        {
            Vector<Field^>^ Fields=ref new Vector<Field^>();

            for (ZtringList CurrentField : ZtringListList((safe_cast<Platform::String^>(Cur->Value))->Data()))
            {
                if (CurrentField.size()<4)
                    continue;

                Fields->Append(ref new Field(ref new Platform::String(CurrentField[0].c_str()), CurrentField[1].To_int32u(), ref new Platform::String(CurrentField[2].c_str()), CurrentField[3].To_int32u()));
            }
            _Sheets->Append(ref new Sheet(Cur->Key, Fields));
        }
    }

    FormatBox->ItemsSource=_Sheets;
    FormatBox->SelectedIndex=0;
}

//---------------------------------------------------------------------------
void SheetView::Get_Streams()
{
    StreamBox->Items->Clear();
    StreamBlock->Text=L"";

    for (size_t StreamKind=0; StreamKind<static_cast<size_t>(Stream_Max); StreamKind++)
    {
        size_t StreamCount=_CurrentReport->Count_Get(StreamKind);
        for (size_t StreamPos=0; StreamPos<StreamCount; StreamPos++)
        {
            TextBlock^ Content=ref new TextBlock();
            Content->Text=AppCore::Get_Stream_Name(StreamKind)+": "+_CurrentReport->Summary(StreamKind, StreamPos);

            ComboBoxItem^ Item=ref new ComboBoxItem();
            Array<size_t>^ Pos=ref new Array<size_t>(2);
            Pos[0]=StreamKind;
            Pos[1]=StreamPos;
            Item->Tag=Pos;
            Item->Content=Content;

            StreamBox->Items->Append(Item);
        }
    }
}

IVectorView<Platform::String^>^ SheetView::Get_Values(ReportViewModel ^ Source)
{
    Vector<Platform::String^>^ Fields=ref new Vector<Platform::String^>;
    for (Field^ Cur : _CurrentSheet->Fields)
        Fields->Append(Source->Get(Cur->StreamKind, 0, Cur->Parameter));

    return Fields->GetView();
}

void SheetView::StreamBox_SelectionChanged(Object^ Sender, SelectionChangedEventArgs^ Event)
{
    Object^ Obj=StreamBox->SelectedItem;
    if (!Obj || Obj->GetType()->FullName!=TypeName(ComboBoxItem::typeid).Name)
        return;

    ComboBoxItem^ Item=safe_cast<ComboBoxItem^>(Obj);
    Array<size_t>^ Pos=safe_cast<Array<size_t>^>(Item->Tag);

    StreamBlock->Text=_CurrentReport->Inform_Get(Pos[0], Pos[1]);
}

void SheetView::FormatBox_SelectionChanged(Object^ Sender, SelectionChangedEventArgs^ Event)
{
    if (FormatBox->SelectedIndex==0)
        DeleteSheet->IsEnabled=false;
    else
        DeleteSheet->IsEnabled=true;

    if (!FormatBox->SelectedItem)
        return;

    _CurrentSheet=safe_cast<Sheet^>(FormatBox->SelectedItem);

    Configure();
}

void SheetView::EditSheet_Click(Object^ Sender, RoutedEventArgs^ Event)
{
    if (!FormatBox->SelectedItem)
        return;

    SheetEditDialog^ Dialog=ref new SheetEditDialog(L"Edit Sheet", safe_cast<Sheet^>(FormatBox->SelectedItem));
    create_task(Dialog->ShowAsync()).then([this](ContentDialogResult Result) {
        if (Result==ContentDialogResult::Primary)
            Save_Sheets();
            Configure();
    });
}

void SheetView::AddSheet_Click(Object^ Sender, RoutedEventArgs^ Event)
{
    Sheet^ NewSheet=ref new Sheet(L"New Sheet", ref new Vector<Field^>());
    SheetEditDialog^ Dialog=ref new SheetEditDialog(L"New Sheet", NewSheet);
    create_task(Dialog->ShowAsync()).then([this, NewSheet](ContentDialogResult Result) {
        if (Result==ContentDialogResult::Primary)
        {
            _Sheets->Append(NewSheet);
            Save_Sheets();
        }
    });
}

void SheetView::DeleteSheet_Click(Object^ Sender, RoutedEventArgs^ Event)
{
    if (!FormatBox->SelectedItem || FormatBox->SelectedIndex==0)
        return;

    unsigned int Index=0;
    if (_Sheets->IndexOf(safe_cast<Sheet^>(FormatBox->SelectedItem), &Index))
    {
        FormatBox->SelectedIndex=0;
        _Sheets->RemoveAt(Index);
        Save_Sheets();
    }
}


void SheetView::MainGrid_SelectionChangedEvent(DataGrid^ Sender, uint32 Pos)
{
    if (Pos>=ReportDataSource::GetAllReports()->Size)
        return;

    _CurrentReport->Source=ReportDataSource::GetReportAt(Pos);
    Get_Streams();
}

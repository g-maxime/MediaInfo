/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license that can
*  be found in the License.html file in the root of the source tree.
*/

//---------------------------------------------------------------------------
#include "pch.h"
#include "EasyView.xaml.h"

#include <MediaInfo\MediaInfo.h>

//---------------------------------------------------------------------------
using namespace MediaInfo;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

const size_t Title_Pos=94;
//---------------------------------------------------------------------------
// Constructor/Destructor
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
EasyView::EasyView(ReportViewModel^ CurrentReport) : _CurrentReport(CurrentReport)
{
    InitializeComponent();

    LayoutRoot->ColumnDefinitions->Append(ref new ColumnDefinition());

    VectorView<Box^>^ Boxes=Create_Boxes();

    Grid^ Row=nullptr;
    for (Box^ Current : Boxes)
    {
        if (!LayoutRoot->FindName(Current->Kind))
        {
            LayoutRoot->RowDefinitions->Append(ref new RowDefinition());
            LayoutRoot->RowDefinitions->GetAt(LayoutRoot->RowDefinitions->Size-1)->Height=GridLength::Auto;

            Row=ref new Grid();
            Row->Padding=ThicknessHelper::FromLengths(0, 0, 0, 15);
            Row->Name=Current->Kind;
            Row->RowDefinitions->Append(ref new RowDefinition());
            Row->ColumnDefinitions->Append(ref new ColumnDefinition());
            Row->RowDefinitions->GetAt(Row->RowDefinitions->Size-1)->Height=GridLength::Auto;

            LayoutRoot->Children->Append(Row);
            LayoutRoot->SetColumn(Row, 0);
            LayoutRoot->SetRow(Row, LayoutRoot->RowDefinitions->Size-1);
        }
        else
        {
            Row=safe_cast<Grid^>(LayoutRoot->FindName(Current->Kind));
            if (!Row)
                continue;

            Row->ColumnDefinitions->Append(ref new ColumnDefinition());
        }

        Grid^ Block=ref new Grid();
        Block->RowDefinitions->Append(ref new RowDefinition());
        Block->RowDefinitions->Append(ref new RowDefinition());

        for (int Pos=0; Pos<Current->Content->Size; Pos++)
            Block->ColumnDefinitions->Append(ref new ColumnDefinition());

        Block->RowDefinitions->GetAt(0)->Height=GridLength::Auto;
        Block->RowDefinitions->GetAt(1)->Height=GridLength::Auto;

        RichTextBlock^ TitleBlock=ref new RichTextBlock();
        Paragraph^ TitleParagraph=ref new Paragraph();
        Run^ Title=ref new Run();
        Title->Text=Current->Kind;
        Title->FontWeight=Windows::UI::Text::FontWeights::Bold;
        TitleParagraph->Inlines->Append(Title);
        TitleBlock->Blocks->Append(TitleParagraph);

        Block->Children->Append(TitleBlock);
        Block->SetRow(TitleBlock, 0);
        Block->SetColumn(TitleBlock, 0);
        Block->SetColumnSpan(TitleBlock, Block->ColumnDefinitions->Size);

        for (int Pos=0; Pos<Current->Content->Size; Pos++)
        {
            RichTextBlock^ ContentBlock=ref new RichTextBlock();
            Paragraph^ ContentParagraph=ref new Paragraph();
            Run^ Text=ref new Run();

            Text->Text=Current->Content->GetAt(Pos);
            ContentParagraph->Inlines->Append(Text);
            ContentBlock->Blocks->Append(ContentParagraph);

            Block->Children->Append(ContentBlock);
            Block->SetRow(ContentBlock, 1);
            Block->SetColumn(ContentBlock, Pos);
        }

        Row->Children->Append(Block);
        Row->SetRow(Block, 0);
        Row->SetColumn(Block, Row->ColumnDefinitions->Size-1);
    }

    return;
}

//---------------------------------------------------------------------------
VectorView<MediaInfo::Box^>^ EasyView::Create_Boxes()
{
    Vector<MediaInfo::Box^>^ ToReturn=ref new Vector<MediaInfo::Box^>;

    for (size_t StreamPos=0; StreamPos<MediaInfoLib::Stream_Max; StreamPos++)
    {
        size_t Count=Boxes_Count_Get(StreamPos);

        if (Count==0)
            continue;

        for (size_t Pos=0; Pos<Count; Pos++)
        {
            Box^ Current=Create_Box(StreamPos, Pos);

            if (Current)
                ToReturn->Append(Current);
        }
    }

    return safe_cast<VectorView<MediaInfo::Box^>^>(ToReturn->GetView());
}

//---------------------------------------------------------------------------
MediaInfo::Box^ EasyView::Create_Box(size_t StreamKind, int StreamPos)
{
    Platform::String^ Kind=Title_Get(StreamKind);
    Platform::String^ Title=ref new Platform::String();

    Vector<Platform::String^>^ Content=ref new Vector<Platform::String^>();

    Platform::String^ Text=_CurrentReport->Summary(StreamKind, StreamPos);

    if (StreamKind==static_cast<size_t>(MediaInfoLib::Stream_General))
    {
        for (size_t KindNo=1; KindNo<static_cast<size_t>(MediaInfoLib::Stream_Max); KindNo++)
        {
            if (_CurrentReport->Count_Get(KindNo)<1)
                continue;

            Platform::String^ Z1=_CurrentReport->Get(KindNo, 0, L"StreamKind", static_cast<size_t>(MediaInfoLib::Info_Measure))+L"StreamCount";
            Platform::String^ Z2=L" "+_CurrentReport->Get(KindNo, 0, L"StreamKind", static_cast<size_t>(MediaInfoLib::Info_Text))+L" stream";
            Platform::String^ Z3=_CurrentReport->Get(KindNo, 0, L"StreamKind", static_cast<size_t>(MediaInfoLib::Info_Text))+L"_Format_WithHint_List";
            Text+=L"\r\n";
            Text+=_CurrentReport->Get(KindNo, 0, Z1)+Z2+L": "+_CurrentReport->Get(0, 0, Z3);

            if (KindNo>Lines_Count_Get(StreamKind))
                break;
        }

        Content->Append(Text);
        Content->Append(Tags_Get_General());
    }
    else if (Lines_Count_Get(StreamKind)>1)
    {
        if (Text->Length()==0)
            return nullptr;

        Title=_CurrentReport->Get(StreamKind, StreamPos, L"Title");
        Content->Append(Text);
    }

    return ref new MediaInfo::Box(Kind, Title, Content);
}

//---------------------------------------------------------------------------
Platform::String^ EasyView::Tags_Get_General()
{
    Platform::String^ ToReturn=ref new Platform::String;

    size_t Pos=Title_Pos; //Should begin with "Title"
    size_t Max=_CurrentReport->Count_Get(static_cast<size_t>(MediaInfoLib::Stream_General), 0);
    size_t Lines=0;

   
    while (Pos<Max)
    {
        Platform::String^ Z1=_CurrentReport->Get(static_cast<size_t>(MediaInfoLib::Stream_General), 0, Pos);
        Platform::String^ Options=_CurrentReport->Get(static_cast<size_t>(MediaInfoLib::Stream_General), 0, Pos, static_cast<size_t>(MediaInfoLib::Info_Options));
        if (Options->Length()>MediaInfoLib::InfoOption_ShowInInform && Options->Data()[MediaInfoLib::InfoOption_ShowInInform]==L'Y' && Z1->Length()>0)
        {
            Platform::String^ Z2=_CurrentReport->Get(static_cast<size_t>(MediaInfoLib::Stream_General), 0, Pos, static_cast<size_t>(MediaInfoLib::Info_Name_Text));
            if (Z2->Length()==0)
                Z2=_CurrentReport->Get(static_cast<size_t>(MediaInfoLib::Stream_General), 0, Pos, static_cast<size_t>(MediaInfoLib::Info_Name));

            Z2+=L": ";
            Z2+=Z1;
            if (Lines>0)
                ToReturn+="\n";

            ToReturn+=Z2;

            Lines++;
            if (Lines>=Lines_Count_Get(static_cast<size_t>(MediaInfoLib::Stream_General)))
                break;
        }
        Pos++;
    }

    return ToReturn;
}

//---------------------------------------------------------------------------
size_t MediaInfo::EasyView::Lines_Count_Get(size_t StreamKind)
{
    switch (StreamKind)
    {
        case 0: return 5;
        case 1: return 2;
        case 2: return 2;
        case 3: return 2;
        default: return 0;
    }
}

//---------------------------------------------------------------------------
size_t MediaInfo::EasyView::Boxes_Count_Get(size_t StreamKind)
{
    switch (StreamKind)
    {
        case 0: return 1;
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        default: return 0;
    }
}

//---------------------------------------------------------------------------
Platform::String^ MediaInfo::EasyView::Title_Get(size_t StreamKind)
{
    switch (StreamKind)
    {
        case 0: return L"General";
        case 1: return L"Video";
        case 2: return L"Audio";
        case 3: return L"Text";
        default: return L"";
    }
}
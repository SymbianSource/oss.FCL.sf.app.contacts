/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Phonebook 2 memory entry defaults assign dialog.
*
*/


#include <CPbk2MemoryEntryDefaultsAssignDlg.h>

// Phonebook 2
#include "MPbk2ClipListBoxText.h"
#include "CPbk2FieldListBoxModel.h"
#include <CPbk2IconInfo.h>
#include <CPbk2IconArray.h>
#include <Pbk2UIControls.rsg>
#include <CPbk2PresentationContact.h>
#include <CPbk2PresentationContactField.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2ApplicationServices.h>
#include <CPbk2ServiceManager.h>
#include <MPbk2ApplicationServices.h>

// Virtual Phonebook
#include <CVPbkFieldTypeSelector.h>
#include <CVPbkContactManager.h>
#include <CVPbkFieldFilter.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactFieldData.h>

// System includes
#include <aknlists.h>
#include <StringLoader.h>
#include <aknPopup.h>
#include <barsread.h>
#include <aknlayoutscalable_avkon.cdl.h>

// Debugging headers
#include <Pbk2Config.hrh>
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

static const TInt KMaxIntLength( 15 );

} /// namespace

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsAssignDlg::CPbk2MemoryEntryDefaultsAssignDlg
// --------------------------------------------------------------------------
//
CPbk2MemoryEntryDefaultsAssignDlg::CPbk2MemoryEntryDefaultsAssignDlg()
    {
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsAssignDlg::~CPbk2MemoryEntryDefaultsAssignDlg
// --------------------------------------------------------------------------
//
CPbk2MemoryEntryDefaultsAssignDlg::~CPbk2MemoryEntryDefaultsAssignDlg()
    {
    Release( iAppServices );
    
    if ( iDestroyedPtr )
        {
        *iDestroyedPtr = ETrue;
        }
    if ( iPopupList )
        {
        iPopupList->CancelPopup();
        }
    delete iListBox;
    delete iModel;
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsAssignDlg::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2MemoryEntryDefaultsAssignDlg*
        CPbk2MemoryEntryDefaultsAssignDlg::NewL()
    {
    CPbk2MemoryEntryDefaultsAssignDlg* self =
        new ( ELeave ) CPbk2MemoryEntryDefaultsAssignDlg();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsAssignDlg::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryDefaultsAssignDlg::ConstructL()
    {
    iAppServices = CPbk2ApplicationServices::InstanceL();
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsAssignDlg::ExecuteLD
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2MemoryEntryDefaultsAssignDlg::ExecuteLD
        ( const CPbk2PresentationContactFieldCollection& aFields,
          const CVPbkContactManager& aManager,
          const MPbk2FieldPropertyArray& aFieldProperties,
          const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorID )
    {
    CleanupStack::PushL( this );
    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;
    TInt index = KErrCancel;
    iSelectorId = aSelectorID;

    PreparePopupL( aFields, aManager, aFieldProperties );

    TBool result = EFalse;
    TRAPD( err, result = iPopupList->ExecuteLD() );
    // Reset iPopupList member
    if ( !thisDestroyed )
        {
        iPopupList = NULL;
        }
    // Rethrow any error
    if ( err != KErrNone )
        {
        User::Leave( err );
        }

    if ( thisDestroyed )
        {
        index = KErrNotFound;
        CleanupStack::Pop(); // this
        }
    else
        {
        if ( result )
            {
            index = iModel->GetModelIndex( iListBox->CurrentItemIndex() );
            }
        CleanupStack::PopAndDestroy(); // this
        }

    return index;
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsAssignDlg::ClipFromBeginning
// --------------------------------------------------------------------------
//
TBool CPbk2MemoryEntryDefaultsAssignDlg::ClipFromBeginning
        ( TDes& aBuffer, TInt aItemIndex, TInt aSubCellNumber )
    {
    return AknTextUtils::ClipToFit(
        aBuffer,
        AknTextUtils::EClipFromBeginning,
        iListBox,
        aItemIndex,
        aSubCellNumber );
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsAssignDlg::PreparePopupL
// --------------------------------------------------------------------------
//
inline void CPbk2MemoryEntryDefaultsAssignDlg::PreparePopupL
        ( const CPbk2PresentationContactFieldCollection& aFields,
          const CVPbkContactManager& aManager,
          const MPbk2FieldPropertyArray& aFieldProperties )
    {
    // Create a list box
    iListBox = static_cast<CEikFormattedCellListBox*>(
        EikControlFactory::CreateByTypeL(
            EAknCtDoubleGraphicPopupMenuListBox).iControl );

    // Create a popup list

    CAknPopupList* popupList = CAknPopupList::NewL(
        iListBox,
        R_AVKON_SOFTKEYS_OK_CANCEL__OK,
        AknPopupLayouts::EMenuDoubleLargeGraphicWindow );


    CleanupStack::PushL( popupList );

    // Init list box
    iListBox->ConstructL( popupList, CEikListBox::ELeftDownInViewRect );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    // Set title of popup list
    HBufC* heading = StringLoader::LoadLC
        ( R_QTN_PHOB_TITLE_SELECT_DEFAULT );
    popupList->SetTitleL(*heading);
    CleanupStack::PopAndDestroy(heading);

    // Set icons
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC
        ( reader, R_PBK2_FIELDTYPE_ICONS );
    CPbk2IconArray* iconArray = CPbk2IconArray::NewL( reader );
    
    //Calculate preferred size for xsp service icons 
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect(
        AknLayoutUtils::EMainPane, mainPane );
    TAknLayoutRect listLayoutRect;
    listLayoutRect.LayoutRect(
        mainPane,
        AknLayoutScalable_Avkon::list_single_graphic_pane_g1(0).LayoutLine() );
    TSize size(listLayoutRect.Rect().Size());
    
    // Add xsp service icons
    CPbk2ServiceManager& servMan(iAppServices->ServiceManager()); 
    
    const CPbk2ServiceManager::RServicesArray& services = servMan.Services();
    TUid uid;
    uid.iUid = KPbk2ServManId;
    for ( TInt i = 0; i < services.Count(); i++ )
        {
        const CPbk2ServiceManager::TService& service = services[i];
        if ( service.iBitmapId && service.iBitmap )
            {
        	AknIconUtils::SetSize(
        	           service.iBitmap,
        	           size );
        	AknIconUtils::SetSize(
        	           service.iMask,
        	           size );            
            TPbk2IconId id = TPbk2IconId( uid, services[i].iBitmapId );
            CPbk2IconInfo* info = CPbk2IconInfo::NewLC(
                id, services[i].iBitmap, services[i].iMask, size );
            iconArray->AppendIconL(info);
            CleanupStack::Pop(info);
            }
        }    
    
    CleanupStack::PopAndDestroy(); // reader
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( iconArray );

    // Create model
    CreateModelL( aFields, aManager, aFieldProperties, *iconArray );

    CleanupStack::Pop(); // popupList
    iPopupList = popupList;
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsAssignDlg::CreateModelL
// --------------------------------------------------------------------------
//
inline void CPbk2MemoryEntryDefaultsAssignDlg::CreateModelL
        ( const CPbk2PresentationContactFieldCollection& aFields,
          const CVPbkContactManager& aManager,
          const MPbk2FieldPropertyArray& aFieldProperties,
          const CPbk2IconArray& aIconArray )
    {
    HBufC* timeFormat = CCoeEnv::Static()->AllocReadResourceLC
        ( R_QTN_DATE_USUAL );

    CPbk2FieldListBoxModel::TParams params(
        aFields,
        aManager,
        aFieldProperties,
        *timeFormat,
        aIconArray,
        NULL,   // analyzer not used
        NULL,   // dynamic properties are not used
        ETrue );// the list items will be duplicated if their content are the same 

    iModel = CPbk2FieldListBoxModel::NewL( params );    
    iModel->FormatFieldsL();            
    CleanupStack::PopAndDestroy( timeFormat );
    iModel->SetClipper( *this );

    // Insert TEXT_NO_DEFAULT as the last item to the array
    CPbk2FieldListBoxRow* row = CPbk2FieldListBoxRow::NewL();
	CleanupStack::PushL( row );
	TBuf<KMaxIntLength> iconBuffer;
	_LIT( KIcon, "%d" );
	iconBuffer.Format( KIcon, aIconArray.FindIcon(
	    TPbk2AppIconId( EPbk2qgn_prop_nrtyp_empty )));
	row->AppendColumnL( iconBuffer );
	HBufC* text = StringLoader::LoadLC( R_QTN_PHOB_SETI_NO_DEFAULT );
	row->AppendColumnL( *text );
	CleanupStack::PopAndDestroy( text );
	row->AppendColumnL( KNullDesC ); // empty label column
	iModel->AppendRowL( row );
	CleanupStack::Pop( row );

    iListBox->Model()->SetItemTextArray( iModel );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    iListBox->Reset();
    }

// End of File

/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 contact editor synchronisation field.
*
*/


#include "CPbk2ContactEditorSyncField.h"

// Phonebook 2
#include "MPbk2ContactEditorUiBuilder.h"
#include "MPbk2ContactEditorFieldVisitor.h"
#include <MPbk2FieldProperty.h>
#include <CPbk2PresentationContactField.h>
#include <pbk2uicontrols.rsg>
#include "Pbk2UIControls.hrh"

// Virtual Phonebook
#include <MVPbkContactFieldTextData.h>
#include <VPbkSyncConstants.h>

// System includes
#include <eikcapc.h>
#include <eikedwin.h>
#include <AknUtils.h>
#include <badesca.h>
#include <barsread.h>
#include <AknPopupField.h>
#include <AknQueryValueText.h>

/// Unnamed namespace for local definitions
namespace {
const TInt KInitialValue = -1;

/**
 * Synchronisation ordering.
 */
enum TPbkSyncronizationConf
    {
    /// Private
    EPbk2SyncConfPrivate = 0,
    /// Public
    EPbk2SyncConfPublic,
    /// Disabled
    EPbk2SyncConfNoSync
    };

} /// namespace

// --------------------------------------------------------------------------
// CPbk2ContactEditorSyncField::CPbk2ContactEditorSyncField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorSyncField::CPbk2ContactEditorSyncField
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer ) :
            CPbk2ContactEditorFieldBase( aContactField, aUiBuilder,
                aIconInfoContainer ),
            iIsUsingDefaultSyncValue( EFalse ),
            iInitialSyncConfSelection( KInitialValue )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorSyncField::~CPbk2ContactEditorSyncField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorSyncField::~CPbk2ContactEditorSyncField()
    {
    delete iSyncArray;
    delete iSyncTextValues;
    delete iSyncArrayText;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorSyncField::NewLC
// --------------------------------------------------------------------------
//
CPbk2ContactEditorSyncField* CPbk2ContactEditorSyncField::NewLC
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer )
    {
    CPbk2ContactEditorSyncField* self =
        new ( ELeave ) CPbk2ContactEditorSyncField( aContactField,
            aUiBuilder, aIconInfoContainer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorSyncField::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorSyncField::ConstructL()
    {
    // Create text array
    iSyncArrayText = CEikonEnv::Static()->ReadDesCArrayResourceL
        ( R_PBK2_SYNC_POPUP_ITEMS );
    iSyncArray = CAknQueryValueTextArray::NewL();
    iSyncArray->SetArray(*iSyncArrayText);
    iSyncTextValues = CAknQueryValueText::NewL();
    iSyncTextValues->SetArrayL(iSyncArray);

    // Set selection to correct item, if field contains some
    DoSetFieldValueAsCurrentSelection();

    // Create and insert a line in the dialog
    iControl = static_cast<CAknPopupField*>(iUiBuilder.CreateLineL(
            FieldLabel(), ControlId(), EAknCtPopupField));

    // Construct the CAknPopupField from resources
    TResourceReader reader;
    CEikonEnv::Static()->CreateResourceReaderLC(
            reader, R_PBK2_SYNCRONIZATION_POPUP_DLG);
    iControl->ConstructFromResourceL(reader);
    CleanupStack::PopAndDestroy(1); // R_PBK2_SYNCRONIZATION_POPUP_DLG

    // Control is now owned by the dialog
    iControl->SetQueryValueL(iSyncTextValues);

    iCaptionedCtrl = iUiBuilder.LineControl(ControlId());
    iCaptionedCtrl->SetTakesEnterKey(ETrue);
    iCaptionedCtrl->SetOfferHotKeys(ETrue);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorSyncField::SaveFieldL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorSyncField::SaveFieldL()
    {
    iContactDataHasChanged = EFalse;

    // Retrieve the selected index
    const TInt selectedIndex = iSyncTextValues->CurrentValueIndex();
    // Retrieve the sync setting text corresponding to the selection
    const TDesC* syncSetting = &KVPbkContactSyncPrivate;
    // Map setting indexes to sync settings
    switch(selectedIndex)
        {
        case EPbk2SyncConfPrivate:
            {
            syncSetting = &KVPbkContactSyncPrivate;
            break;
            }
        case EPbk2SyncConfPublic:
            {
            syncSetting = &KVPbkContactSyncPublic;
            break;
            }
        case EPbk2SyncConfNoSync:
            {
            syncSetting = &KVPbkContactSyncNoSync;
            break;
            }
        default:
            {
            syncSetting = &KVPbkContactSyncPrivate;
            break;
            }
        }

    // compare sync setting to existing setting in field
    MVPbkContactFieldTextData& data = MVPbkContactFieldTextData::Cast(
            iContactField.FieldData());
    TPtrC fieldText(data.Text());
    if (fieldText.CompareF(*syncSetting))
        {
        // store the text in the contact item
        data.SetTextL(*syncSetting);
        iContactDataHasChanged = ETrue;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorSyncField::FieldDataChanged
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorSyncField::FieldDataChanged() const
    {
    TBool result = EFalse;
    if (iContactDataHasChanged)
        {
        if ( iIsUsingDefaultSyncValue &&
              iSyncTextValues->CurrentValueIndex() ==
                iInitialSyncConfSelection )
            {
            // Default value has not been changed
            result = EFalse;
            }
        else
            {
            result = ETrue;
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorSyncField::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorSyncField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorSyncField::Control
// --------------------------------------------------------------------------
//
HBufC* CPbk2ContactEditorSyncField::ControlTextL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorSyncField::Control
// --------------------------------------------------------------------------
//
CEikEdwin* CPbk2ContactEditorSyncField::Control() const
    {
    // Not called by the visitor, doesnt contain a edwin
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorSyncField::AcceptL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorSyncField::AcceptL
        ( MPbk2ContactEditorFieldVisitor& aVisitor )
    {
    aVisitor.VisitL(*this);
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorSyncField::ConsumesKeyEvent
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactEditorSyncField::ConsumesKeyEvent
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TBool ret = EFalse;

    if (aType == EEventKey && aKeyEvent.iCode == EKeyOK)
        {
        ret = ETrue;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorSyncField::DoSetFieldValueAsCurrentSelection
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorSyncField::DoSetFieldValueAsCurrentSelection()
    {
    TInt selectedIndex = EPbk2SyncConfPrivate;
    iIsUsingDefaultSyncValue = ETrue;

    // get the text from the field
    MVPbkContactFieldTextData& data = MVPbkContactFieldTextData::Cast(
            iContactField.FieldData());
    TPtrC fieldText(data.Text());
    // safely detect whether theres syncronization setting in the field
    if (!fieldText.CompareF(KVPbkContactSyncPrivate))
        {
        selectedIndex = EPbk2SyncConfPrivate;
        iIsUsingDefaultSyncValue = EFalse;
        }
    else if (!fieldText.CompareF(KVPbkContactSyncPublic))
        {
        selectedIndex = EPbk2SyncConfPublic;
        iIsUsingDefaultSyncValue = EFalse;
        }
    else if (!fieldText.CompareF(KVPbkContactSyncNoSync))
        {
        selectedIndex = EPbk2SyncConfNoSync;
        iIsUsingDefaultSyncValue = EFalse;
        }

    // set default value
    iSyncTextValues->SetCurrentValueIndex(selectedIndex);
    // store initial sync selection index
    iInitialSyncConfSelection = selectedIndex;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorSyncField::HandleCustomFieldCommandL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorSyncField::HandleCustomFieldCommandL(TInt aCommand )
    {
    TBool ret(EFalse);
    
    if( aCommand == EPbk2CmdEditorChangeSync )  
        {
        iControl->ActivateSelectionListL();
        ret = ETrue;
        }

    return ret;
    }

//  End of File

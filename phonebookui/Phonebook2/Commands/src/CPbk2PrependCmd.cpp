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
* Description:  Prepend command object.
*
*/


// INCLUDE FILES
#include "CPbk2PrependCmd.h"

// From Phonebook2
#include "CPbk2CallCmd.h"
#include "CPbk2AiwInterestArray.h"
#include <pbk2uicontrols.rsg>
#include <CPbk2PrependDlg.h>
#include <Pbk2Commands.hrh>
#include <CPbk2ContactUiControlSubstitute.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <CPbk2PresentationContactField.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>

// From VirtualPhonebook
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactStore.h>

/// Unnamed namespace for local definitions
namespace {

// Definition for buffer size
const TInt KBufferSize( 50 );

#ifdef _DEBUG

enum TPanicCode
    {
    EMaxLengthOfEditorL_OOB = 1
    };

void Panic( TPanicCode aPanic )
    {
    _LIT( KPanicCat, "CPbk2PrependCmd" );
    User::Panic( KPanicCat, aPanic );
    }

#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPbk2PrependCmd::CPbk2PrependCmd
// --------------------------------------------------------------------------
//
CPbk2PrependCmd::CPbk2PrependCmd(
        TInt aMenuCommandId,
        MPbk2ContactUiControl& aUiControl,
        CAiwServiceHandler& aServiceHandler,
        CPbk2CallTypeSelector& aSelector,
        MVPbkStoreContact*& aStoreContact,
        CPbk2FieldPropertyArray& aFieldProperties) :
    iUiControl(&aUiControl), 
    iServiceHandler(aServiceHandler),
    iSelector(aSelector),
    iMenuCommandId( aMenuCommandId),
    iStoreContact( aStoreContact ),
    iFieldProperties( aFieldProperties )
    {
    }

// --------------------------------------------------------------------------
// CPbk2PrependCmd::~CPbk2PrependCmd
// --------------------------------------------------------------------------
//
CPbk2PrependCmd::~CPbk2PrependCmd()
    {
    delete iUiControlSubstitute;
    delete iTempContact;
    }

// --------------------------------------------------------------------------
// CPbk2PrependCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2PrependCmd* CPbk2PrependCmd::NewL(
        TInt aMenuCommandId,
        MPbk2ContactUiControl& aUiControl,
        CAiwServiceHandler& aServiceHandler,
        CPbk2CallTypeSelector& aSelector,
        MVPbkStoreContact*& aStoreContact,
        CPbk2FieldPropertyArray& aFieldProperties)
    {
    CPbk2PrependCmd* self =
        new(ELeave) CPbk2PrependCmd(
            aMenuCommandId, aUiControl, 
            aServiceHandler, aSelector, 
            aStoreContact,
            aFieldProperties);
            
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2PrependCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2PrependCmd::ConstructL()
    {
    iUiControlSubstitute = CPbk2ContactUiControlSubstitute::NewL();
    }

// --------------------------------------------------------------------------
// CPbk2PrependCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2PrependCmd::ExecuteLD()
    {
    CleanupStack::PushL( this );

    ExecuteDlgL();
    
    CleanupStack::PopAndDestroy( this );
    }

// --------------------------------------------------------------------------
// CPbk2PrependCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2PrependCmd::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iUiControl == &aUiControl)
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2PrependCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2PrependCmd::AddObserver( MPbk2CommandObserver& /*aObserver*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2PrependCmd::ExecuteDlgL
// --------------------------------------------------------------------------
//
void CPbk2PrependCmd::ExecuteDlgL()
    {
    if (iStoreContact && iUiControl)
        {
        // get field text (phone no)
        TInt fieldIndex( iUiControl->FocusedFieldIndex() );
        MVPbkStoreContactField* field =
            iStoreContact->Fields().FieldAtLC( fieldIndex );
        TPtrC phoneNo =
            MVPbkContactFieldTextData::Cast( field->FieldData() ).Text();
        HBufC* number = HBufC::NewLC( KBufferSize );
        number->Des().Copy( phoneNo );
        TPtr numberPtr = number->Des();
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( numberPtr );
        // show dialog with that no
        CPbk2PrependDlg* dlg = CPbk2PrependDlg::NewL( numberPtr );
        CleanupStack::PushL( dlg );
        TInt maxLength( MaxLengthOfEditorL( fieldIndex ) );
        CleanupStack::Pop( dlg );
        if ( maxLength != KErrNotFound )
            {
            // The length of prepend dialog's editor should be same as
            // field's maxlength. If maxLength is KErrNotFound use resource's
            // length.
            dlg->SetMaxLength( maxLength );
            }
        // Get the contact store and field type from the iStoreContact before the prepend dialog is poped up,
        // since it might be changed When the dialog is poped up. 
        MVPbkContactStore& store = iStoreContact->ParentStore();
        const MVPbkFieldType* fieldType = field->BestMatchingFieldType();
        if ( dlg->ExecuteLD(R_PBK2_PREPEND_QUERY) )
            {
            // Create temporary contact for phone call
            delete iTempContact;
            iTempContact = NULL;
            iTempContact = store.CreateNewContactLC();
            CleanupStack::Pop(); // iTempContact
            if( fieldType )
                {
                // User has pressed ok, execute call                
                MVPbkStoreContactField* tempField =
                    iTempContact->CreateFieldLC( *fieldType );
                
                MVPbkContactFieldTextData::Cast( tempField->FieldData() ).
                    SetTextL( numberPtr );
                // Takes ownership
                TInt tempFieldIndex = iTempContact->AddFieldL( tempField );
                CleanupStack::Pop(); // tempField
                tempField = NULL;                
                tempField = iTempContact->Fields().FieldAtLC( 
                    tempFieldIndex );
                CleanupStack::Pop(); // tempField

                iUiControlSubstitute->SetFocusedStoreContact( *iTempContact );
                iUiControlSubstitute->SetFocusedField( *tempField );
                iUiControlSubstitute->SetFocusedFieldIndex( tempFieldIndex );

                // Create call command to execute phone call                
                CPbk2CallCmd* cmd = CPbk2CallCmd::NewL(
                    iTempContact,
                    tempField,
                    *iUiControlSubstitute,
                    EPbk2CmdCall,
                    iServiceHandler,
                    iSelector );

                cmd->ExecuteLD();                
                }

            iUiControl->UpdateAfterCommandExecution();
            }
        CleanupStack::PopAndDestroy( 2 ); // field, number
        }
    }

// --------------------------------------------------------------------------
// CPbk2PrependCmd::ToStoreFieldIndexL
// --------------------------------------------------------------------------
//
TInt CPbk2PrependCmd::ToStoreFieldIndexL
    ( TInt aPresIndex )
    {
    TInt index( KErrNotFound );
    CPbk2PresentationContact* presentationContact =
        CreatePresentationContactLC();
    index = presentationContact->PresentationFields().StoreIndexOfField
                ( aPresIndex );

    CleanupStack::PopAndDestroy( presentationContact );

    return index;
    }

// --------------------------------------------------------------------------
// CPbk2PrependCmd::MaxLengthOfEditorL
// --------------------------------------------------------------------------
//
TInt CPbk2PrependCmd::MaxLengthOfEditorL( TInt aFieldStoreIndex )
    {
    TInt maxLength( KErrNotFound );
    CPbk2PresentationContact* presentationContact =
        CreatePresentationContactLC();
    
    TInt fieldIndex = KErrNotFound;
     
    fieldIndex = presentationContact->PresentationFields().FieldIndexOfStoreField( aFieldStoreIndex );

    __ASSERT_DEBUG( presentationContact->PresentationFields().FieldCount() >
                       fieldIndex, Panic( EMaxLengthOfEditorL_OOB ) );
    
    __ASSERT_DEBUG( fieldIndex > KErrNotFound, Panic( EMaxLengthOfEditorL_OOB ) );

    maxLength = presentationContact->PresentationFields().At( fieldIndex ).
                MaxDataLength();

    CleanupStack::PopAndDestroy( presentationContact );

    return maxLength;
    }

// --------------------------------------------------------------------------
// CPbk2PrependCmd::CreatePresenationContactLC
// --------------------------------------------------------------------------
//
CPbk2PresentationContact* CPbk2PrependCmd::CreatePresentationContactLC()
    {
    CPbk2PresentationContact* presentationContact =
        CPbk2PresentationContact::NewL(*iStoreContact, iFieldProperties);
    CleanupStack::PushL( presentationContact );
    return presentationContact;
    }

// End of File

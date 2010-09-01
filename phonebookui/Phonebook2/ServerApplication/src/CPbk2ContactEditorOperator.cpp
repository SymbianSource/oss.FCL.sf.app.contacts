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
* Description:  Phonebook 2 server app contact editor operator.
*
*/

#include "CPbk2ContactEditorOperator.h"

// Phonebook 2
#include "CPbk2ServerAppAppUi.h"
#include <CPbk2ContactEditorDlg.h>
#include <MPbk2ApplicationServices.h>

// Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <CVPbkContactManager.h>
#include <VPbkUtils.h>

// System includes
#include <AiwContactAssignDataTypes.h>


using namespace AiwContactAssign;

// --------------------------------------------------------------------------
// CPbk2ContactEditorOperator::CPbk2ContactEditorOperator
// --------------------------------------------------------------------------
//
inline CPbk2ContactEditorOperator::CPbk2ContactEditorOperator
        ( MPbk2ExitCallback& aExitCallback,
          MPbk2EditedContactObserver& aContactObserver ) :
            iExitCallback( aExitCallback ),
            iContactObserver( aContactObserver )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorOperator::~CPbk2ContactEditorOperator
// --------------------------------------------------------------------------
//
CPbk2ContactEditorOperator::~CPbk2ContactEditorOperator()
    {
    if ( iSelfPtr )
        {
        *iSelfPtr = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorOperator::NewLC
// --------------------------------------------------------------------------
//
CPbk2ContactEditorOperator* CPbk2ContactEditorOperator::NewLC
        ( MPbk2ExitCallback& aExitCallback,
          MPbk2EditedContactObserver& aContactObserver )
    {
    CPbk2ContactEditorOperator* self =
        new ( ELeave ) CPbk2ContactEditorOperator
            ( aExitCallback, aContactObserver );
    CleanupStack::PushL( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorOperator::OpenEditorL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorOperator::OpenEditorL
        ( MVPbkStoreContact*& aStoreContact, TInt& aIndex,
          TUint& aFlags, TCoeHelpContext& aHelpContext )
    {
    // Set editor flags
    TUint32 editorFlags = 0;
    if ( aFlags & EHideEditorExit )
        {
        editorFlags |= TPbk2ContactEditorParams::EHideExit;
        }
    if ( aFlags & ECreateNewContact )
        {
        editorFlags |= TPbk2ContactEditorParams::ENewContact;
        }
    else
        {
        editorFlags |= TPbk2ContactEditorParams::EModified;
        }

    TCoeHelpContext* helpContext = NULL;
    if ( !aHelpContext.IsNull() )
        {
        helpContext = &aHelpContext;
        }

    // Verify syncronization field existance and content
    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );

    VPbkUtils::VerifySyncronizationFieldL(
            appUi.ApplicationServices().ContactManager().FsSession(),
            aStoreContact->ParentStore().StoreProperties().SupportedFields(),
            *aStoreContact );

    MVPbkBaseContactField* field = NULL;
    if ( aIndex >= KErrNone )
        {
        field = aStoreContact->Fields().FieldAtLC( aIndex );
        }
    else
        {
        // We must keep cleanup stack level the same
        CleanupStack::PushL( field );
        }

    TPbk2ContactEditorParams params
        ( editorFlags, field, helpContext, this );
    
    CPbk2ContactEditorDlg* editor = CPbk2ContactEditorDlg::NewL
        ( params, aStoreContact, iContactObserver );
    aStoreContact = NULL; // ownership was taken away
    iContactEditorEliminator = editor;
    editor->ResetWhenDestroyed( &iContactEditorEliminator );
    editor->ExecuteLD();

    aIndex = params.iFocusedIndex;

    CleanupStack::PopAndDestroy( field );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorOperator::RequestExitL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorOperator::RequestExitL( TInt aCommandId )
    {
    if ( iContactEditorEliminator )
        {
        iContactEditorEliminator->RequestExitL( aCommandId );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorOperator::ForceExitL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorOperator::ForceExit()
    {
    if ( iContactEditorEliminator )
        {
        iContactEditorEliminator->ForceExit();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorOperator::ResetWhenDestroyed
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorOperator::ResetWhenDestroyed(
                MPbk2DialogEliminator** aSelfPtr )
    {
    iSelfPtr = aSelfPtr;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorOperator::OkToExitL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorOperator::OkToExitL( TInt aCommandId )
    {
    // Deny editor exit and pass an asynchronous query to the consumer,
    // which may later approve exit
    iExitCallback.OkToExitL( aCommandId );
    return EFalse;
    }

// End of File

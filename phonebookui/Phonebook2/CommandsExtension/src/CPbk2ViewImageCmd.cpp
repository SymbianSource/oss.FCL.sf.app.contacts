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
* Description: 
*           View image command event handling class.
*
*/


// INCLUDE FILES
#include "CPbk2ViewImageCmd.h"

// Phonebook 2
#include "Pbk2DataCaging.hrh"
#include <pbk2uicontrols.rsg>
#include <MPbk2CommandObserver.h>
#include <TPbk2StoreContactAnalyzer.h>
#include <pbk2commands.rsg>
#include <pbk2cmdextres.rsg>

// Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactFieldTextData.h>

// System includes
#include <DocumentHandler.h>            // CDocumentHandler
#include <apmstd.h>                     // TDataType
#include <StringLoader.h>
#include <aknnotewrappers.h>

// Debugging headers
#include <Pbk2Debug.h>


// --------------------------------------------------------------------------
// CPbk2ViewImageCmd::CPbk2ViewImageCmd
// --------------------------------------------------------------------------
//
CPbk2ViewImageCmd::CPbk2ViewImageCmd( MPbk2ContactUiControl& aUiControl )
    : CPbk2ImageCmdBase( aUiControl )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ViewImageCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ViewImageCmd::ConstructL()
    {
    CPbk2ImageCmdBase::BaseConstructL();
    iDocHandler = CDocumentHandler::NewL();
    iDocHandler->SetExitObserver( this );
    }

// --------------------------------------------------------------------------
// CPbk2ViewImageCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2ViewImageCmd* CPbk2ViewImageCmd::NewL(
        MPbk2ContactUiControl& aUiControl )
    {
    CPbk2ViewImageCmd* self =
        new (ELeave) CPbk2ViewImageCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ViewImageCmd::~CPbk2ViewImageCmd
// --------------------------------------------------------------------------
//
CPbk2ViewImageCmd::~CPbk2ViewImageCmd()
    {
    delete iDocHandler;
    }

// --------------------------------------------------------------------------
// CPbk2ViewImageCmd::ExecuteCommandL
// --------------------------------------------------------------------------
//
TBool CPbk2ViewImageCmd::ExecuteCommandL()
    {
    TBool result = EFalse;

    // fetch image from contact's image field
    TInt fieldIndex( KErrNotFound );
    TPbk2StoreContactAnalyzer analyzer( *iContactManager, iStoreContact );
    fieldIndex = analyzer.HasFieldL( R_PHONEBOOK2_IMAGE_SELECTOR );

    if( fieldIndex != KErrNotFound )
        {
        // retrieve field pointer and its current text for call
        MVPbkStoreContactField& field =
            iStoreContact->Fields().FieldAt( fieldIndex );
        iFileName.Copy(
            MVPbkContactFieldTextData::Cast( field.FieldData() ).Text() );

        // empty datatype for automatic recognization
        TDataType dataType;

        // open file using document handler
        TInt status = KErrGeneral;
        TRAPD( error, status = iDocHandler->
            OpenFileEmbeddedL( iFileName, dataType ) );
        
        // KErrNotReady, when no mmc inserted
        // KErrNotFound, when image is deleted
        if ( error == KErrNotFound || error == KErrUnderflow
                                   || error == KErrCorrupt 
                                   || error == KErrNotReady )
            {
            // if the image has been deleted from Image Gallery
            // or it is corrupted, show a note instead of leave
            ShowImageNotOpenedNoteL();
            }
        result = ( status == KErrNone );
        }
    else
        {
        // When viewing image on the business card, show "can't open" note for it
        ShowImageNotOpenedNoteL();
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ViewImageCmd::HandleServerAppExit
// --------------------------------------------------------------------------
//
void CPbk2ViewImageCmd::HandleServerAppExit( TInt aReason )
    {
    // make base call first
    MAknServerAppExitObserver::HandleServerAppExit( aReason );
    // finish the command (treat KUserCancel as KErrNone too)
    ProcessDismissed( KErrNone );
    }

// --------------------------------------------------------------------------
// CPbk2ViewImageCmd::ShowImageNotOpenedNoteL
// --------------------------------------------------------------------------
//
void CPbk2ViewImageCmd::ShowImageNotOpenedNoteL()
    {
    HBufC* prompt = StringLoader::LoadLC( R_QTN_ALBUM_ERR_FORMAT_UNKNOWN );
    CAknInformationNote* dlg = new ( ELeave ) CAknInformationNote( ETrue );
    dlg->ExecuteLD( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    }

//  End of File

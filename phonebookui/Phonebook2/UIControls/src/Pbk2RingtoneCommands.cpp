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
* Description:  Phonebook 2 ringing tone commands.
*
*/


// INCLUDES
#include <e32base.h>
#include "Pbk2RingtoneCommands.h"

// Phonebook 2
#include <CPbk2RingingToneFetch.h>
#include "MPbk2BaseCommand.h"

#include <MVPbkContactFieldTextData.h>
#include <CPbk2AppUiBase.h>
#include <MPbk2ApplicationServices.h>
#include <TPbk2StoreContactAnalyzer.h>

#include <pbk2commonui.rsg>
#include <pbk2uicontrols.rsg>
#include <pbk2commands.rsg>

// VPbk includes
#include <MVPbkStoreContact.h>
#include <MVPbkStoreContactField.h>
#include <CVPbkContactManager.h>
#include <TVPbkFieldTypeMapping.h>
#include <MVPbkFieldType.h>
#include <vpbkeng.rsg>

// System includes
#include <AknQueryDialog.h>
#include <barsread.h>
#include <coemain.h>
#include <sysutil.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicPreCond_NULL_Contact = 1
    };

void Panic( TPanicCode aPanic )
    {
    _LIT( KPanicCat, "MPbk2RingtoneCommands" );
    User::Panic( KPanicCat, aPanic );
    }
#endif // _DEBUG    

// local functions

// --------------------------------------------------------------------------
// ReadFieldTypeL
// --------------------------------------------------------------------------
const MVPbkFieldType* ReadFieldTypeL()
    {
    return Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
    		FieldTypes().Find( R_VPBK_FIELD_TYPE_RINGTONE );
    }

// --------------------------------------------------------------------------
// Copies to aFilename ringtone field content if the field exists
// in the store contact.
// --------------------------------------------------------------------------
inline void RetrieveExistingRingingToneFilenameL( 
        TFileName& aFilename,
        const MVPbkStoreContact& aContact )
    {
    TPbk2StoreContactAnalyzer analyzer(
        Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager(),
        &aContact );
    TInt fieldIndex = analyzer.HasFieldL( R_PHONEBOOK2_RINGTONE_SELECTOR );
    if ( fieldIndex != KErrNotFound )
    	{
    	const MVPbkStoreContactField& field =
    		aContact.Fields().FieldAt( fieldIndex );
    	aFilename.Copy(
    			MVPbkContactFieldTextData::Cast( field.FieldData() ).Text() );    	
    	}
    }

// --------------------------------------------------------------------------
// Shows select ringtone dialog. aContact is used to preselect assigned
// ringtone file.
// Returns ETrue if user selected a ringtone file.
// It is returned in aToneFilename. If user selects default ringtone,
// than aToneFilename will be empty
// --------------------------------------------------------------------------
//
inline TBool FetchRingingToneL( TFileName& aToneFilename,
		const MVPbkStoreContact& aContact )
    {
    TBool retval = EFalse;
    RetrieveExistingRingingToneFilenameL( aToneFilename, aContact );
    CPbk2RingingToneFetch* toneFetch = CPbk2RingingToneFetch::NewL();
    CleanupStack::PushL( toneFetch );
    // Run select ringtone dialog
    if ( toneFetch->FetchRingingToneL( aToneFilename ) )
        {
        retval = ETrue;
        }
    CleanupStack::PopAndDestroy( toneFetch );
    return retval;
    }

} /// namespace

// --------------------------------------------------------------------------
// MPbk2RingtoneCommands::SetRingtoneL
// --------------------------------------------------------------------------
//
MPbk2BaseCommand* Pbk2RingtoneCommands::SetRingtoneL(
		MVPbkStoreContact* aContact )
	{
	__ASSERT_DEBUG( aContact, Panic( EPanicPreCond_NULL_Contact ) );
	
    TBool retval =  EFalse;
    TFileName ringtoneFilename;

    RFs fs = CCoeEnv::Static()->FsSession();
    if ( SysUtil::FFSSpaceBelowCriticalLevelL( &fs ) )
        {
        User::Leave( KErrDiskFull );
        }
    
    retval = FetchRingingToneL( ringtoneFilename, *aContact );
    if ( retval )
        {
        MVPbkStoreContactField* field = NULL;
        // First check if the field is available already
        TPbk2StoreContactAnalyzer analyzer(
            Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager(),
            aContact );
        TInt fieldIndex = analyzer.HasFieldL( R_PHONEBOOK2_RINGTONE_SELECTOR );

        if ( fieldIndex != KErrNotFound)
            {
            // Check if the iToneFilename is set
            if (ringtoneFilename.Length() <= 0)
                {
                // If filename is not set and the contact has the field
                // field can be removed.
                //aContact->RemoveField( fieldIndex );
                //Instead of above we don't remove the field, just zero it's data
                MVPbkContactFieldData& data(aContact->Fields().
                    FieldAt( fieldIndex ).FieldData());
                MVPbkContactFieldTextData::Cast( data ).SetTextL(KNullDesC);
                }
            else
                {
                // If filename has been set and the contact has the field
                // field should be updated.
                field = &aContact->Fields().FieldAt( fieldIndex );
                MVPbkContactFieldTextData::Cast( field->FieldData() ).
                    SetTextL( ringtoneFilename );
                }
            }
        else
            {
            // Contact does not have the field, so create new one.
            if ( ringtoneFilename.Length() > 0 )
                {
                const MVPbkFieldType* fieldType = ReadFieldTypeL();
                if ( !fieldType )
                    {
                    ReadFieldTypeL();
                    }
                User::LeaveIfNull( fieldType );
                // iToneFilename is set
                field = aContact->CreateFieldLC( *fieldType );
                MVPbkContactFieldTextData::Cast( field->FieldData() ).
                    SetTextL( ringtoneFilename );
                aContact->AddFieldL( field );
                CleanupStack::Pop(); // field
                }
            }
        }
    return NULL; // synchronous operation
	}

// --------------------------------------------------------------------------
// MPbk2RingtoneCommands::RemoveRingtoneL
// --------------------------------------------------------------------------
//
MPbk2BaseCommand* Pbk2RingtoneCommands::RemoveRingtoneL(
		MVPbkStoreContact* aContact )
	{
	__ASSERT_DEBUG( aContact, Panic( EPanicPreCond_NULL_Contact ) );
	
    if( CAknQueryDialog::NewL()->ExecuteLD(
            R_PHONEBOOK2_REMOVE_RINGTONE_CONFIRMATION_DIALOG ) )
        {
        TPbk2StoreContactAnalyzer analyzer
        	( Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager(),
        			aContact );
        TInt fieldIndex( KErrNotFound );
        fieldIndex = analyzer.HasFieldL( R_PHONEBOOK2_RINGTONE_SELECTOR );
	    if ( fieldIndex != KErrNotFound )
			{
		    // aContact->RemoveField (fieldIndex);
			//Instead of above we don't remove the field, just zero it's data
			MVPbkContactFieldData& data(aContact->Fields().
			    FieldAt( fieldIndex ).FieldData());
            MVPbkContactFieldTextData::Cast( data ).SetTextL(KNullDesC);
			}
        }
    return NULL; // synchronous operation
	}

// --------------------------------------------------------------------------
// MPbk2RingtoneCommands::IsRingtoneFieldL
// --------------------------------------------------------------------------
//
TBool Pbk2RingtoneCommands::IsRingtoneFieldL(
		const MVPbkStoreContactField& aField )
	{
    TPbk2StoreContactAnalyzer analyzer(
    		Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager(),
    		NULL );
    return analyzer.IsFieldTypeIncludedL(
    		aField, R_PHONEBOOK2_RINGTONE_SELECTOR );    
	}

// --------------------------------------------------------------------------
// MPbk2RingtoneCommands::IsRingtoneFieldValidL
// --------------------------------------------------------------------------
//
TBool Pbk2RingtoneCommands::IsRingtoneFieldValidL(
		const MVPbkStoreContactField& aField )
	{
    const MVPbkContactFieldTextData& textData = 
        MVPbkContactFieldTextData::Cast( aField.FieldData() );
    TEntry ignore;
    return CCoeEnv::Static()->FsSession().Entry( textData.Text(), ignore ) == KErrNone;
	}

// End of File

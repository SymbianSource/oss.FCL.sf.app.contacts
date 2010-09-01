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
* Description:  Phonebook 2 default attribute modification processor.
*
*/


#include "CPbk2DefaultAttributeProcess.h"

// Phonebook 2
#include <MPbk2DefaultAttributeProcessObserver.h>

// Virtual Phonebook
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContactField.h>
#include <CVPbkDefaultAttribute.h>
#include <CVPbkContactManager.h>
#include <MVPbkStoreContact.h>

// Debugging headers
#include <Pbk2Debug.h>


/// Unnamed namespace for local definitions
namespace {

const TInt KGranularity = 4;

#ifdef _DEBUG

enum TPanicCode_Process
    {
    EPanicLogic_AttributeOperationComplete,
    EPanicLogic_AttributeOperationFailed,
    EPanicPreCond_SetDefaultsL,
    EPanicPreCond_RemoveDefaultsL,
    };

static void Panic( TPanicCode_Process aReason )
    {
    _LIT(KPanicText, "CPbk2DefaultAttributeProcess");
    User::Panic(KPanicText, aReason);
    }

#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPbk2DefaultAttributeProcess::CPbk2DefaultAttributeProcess
// --------------------------------------------------------------------------
//
CPbk2DefaultAttributeProcess::CPbk2DefaultAttributeProcess
        ( CVPbkContactManager& aManager, MVPbkStoreContact& aContact,
          MPbk2DefaultAttributeProcessObserver& aObserver ):
            iManager( aManager ), iContact( aContact ),
            iObserver( aObserver )
    {
    }

// --------------------------------------------------------------------------
// CPbk2DefaultAttributeProcess::CPbk2DefaultAttributeProcess
// --------------------------------------------------------------------------
//
CPbk2DefaultAttributeProcess::~CPbk2DefaultAttributeProcess()
    {
    delete iDefaultAttributes;
    delete iSetAttributeOperation;
    delete iRemoveAttributeOperation;
    delete iRemovePreviousAttributeOperation;
    delete iRemoveAttributeField;
    }

// --------------------------------------------------------------------------
// CPbk2DefaultAttributeProcess::NewL
// --------------------------------------------------------------------------
//
EXPORT_C  CPbk2DefaultAttributeProcess* CPbk2DefaultAttributeProcess::NewL
        ( CVPbkContactManager& aManager, MVPbkStoreContact& aContact,
        MPbk2DefaultAttributeProcessObserver& aObserver )
    {
    CPbk2DefaultAttributeProcess* self =
        new ( ELeave ) CPbk2DefaultAttributeProcess
            ( aManager, aContact, aObserver );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2DefaultAttributeProcess::SetDefaultL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2DefaultAttributeProcess::SetDefaultL
        ( TVPbkDefaultType aDefaultType, MVPbkStoreContactField& aField )
    {
    iAttributeType = aDefaultType;
    iSetAttributeField = &aField;

    delete iDefaultAttributes;
    iDefaultAttributes = NULL;
    iDefaultAttributes =
        new( ELeave ) CArrayFixFlat<TVPbkDefaultType>( KGranularity );
    iDefaultAttributes->AppendL( aDefaultType );

    SetNextL();
    }

// --------------------------------------------------------------------------
// CPbk2DefaultAttributeProcess::RemoveDefaultL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2DefaultAttributeProcess::RemoveDefaultL
        ( TVPbkDefaultType aDefaultType  )
    {
    iAttributeType = aDefaultType;

    delete iDefaultAttributes;
    iDefaultAttributes = NULL;
    iDefaultAttributes =
        new( ELeave ) CArrayFixFlat<TVPbkDefaultType>( KGranularity );
    iDefaultAttributes->AppendL( aDefaultType );


    RemoveNextL();
    }

// --------------------------------------------------------------------------
// CPbk2DefaultAttributeProcess::SetDefaultsL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2DefaultAttributeProcess::SetDefaultsL
        ( CArrayFixFlat<TVPbkDefaultType>* aDefaultProperties,
          MVPbkStoreContactField& aField )
    {
    __ASSERT_DEBUG( aDefaultProperties->Count() > 0,
        Panic( EPanicPreCond_SetDefaultsL ) );

    iSetAttributeField = &aField;

    // Take ownership
    delete iDefaultAttributes;
    iDefaultAttributes = aDefaultProperties;

    SetNextL();
    }

// --------------------------------------------------------------------------
// CPbk2DefaultAttributeProcess::RemoveDefaultsL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2DefaultAttributeProcess::RemoveDefaultsL
        ( CArrayFixFlat<TVPbkDefaultType>* aDefaultProperties )
    {
    __ASSERT_DEBUG( aDefaultProperties->Count() > 0,
        Panic( EPanicPreCond_RemoveDefaultsL ) );

    // Take ownership
    delete iDefaultAttributes;
    iDefaultAttributes = aDefaultProperties;

    RemoveNextL();
    }

// --------------------------------------------------------------------------
// CPbk2DefaultAttributeProcess::AttributeOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2DefaultAttributeProcess::AttributeOperationComplete
        ( MVPbkContactOperationBase& aOperation )
    {
    __ASSERT_DEBUG( &aOperation == iSetAttributeOperation ||
        &aOperation == iRemoveAttributeOperation ||
        &aOperation == iRemovePreviousAttributeOperation,
            Panic( EPanicLogic_AttributeOperationComplete ) );

    TInt err = KErrNone;

    if ( &aOperation == iSetAttributeOperation )
        {
        // Move to next attribute
        TRAP( err, SetNextL() );
        }
    else if ( &aOperation == iRemoveAttributeOperation )
        {
        // Move to next attribute
        TRAP( err, RemoveNextL() );
        }
    else if ( &aOperation == iRemovePreviousAttributeOperation )
        {
        // Previous attribute was removed, set the new attribute
        TRAP( err, DoSetL( iAttributeType ) );
        }

    // Handle error
    if ( err != KErrNone )
        {
        iObserver.AttributeProcessFailed( err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2DefaultAttributeProcess::AttributeOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2DefaultAttributeProcess::AttributeOperationFailed
        ( MVPbkContactOperationBase& aOperation, TInt aError )
    {
    __ASSERT_DEBUG( &aOperation == iSetAttributeOperation ||
        &aOperation == iRemoveAttributeOperation ||
        &aOperation == iRemovePreviousAttributeOperation,
            Panic( EPanicLogic_AttributeOperationFailed ) );

    iObserver.AttributeProcessFailed( aError );
    }

// --------------------------------------------------------------------------
// CPbk2DefaultAttributeProcess::FindContactFieldWithAttributeL
// --------------------------------------------------------------------------
//
MVPbkStoreContactField*
        CPbk2DefaultAttributeProcess::FindContactFieldWithAttributeL
            ( TVPbkDefaultType aDefaultType )
    {
    MVPbkStoreContactField* field = NULL;

    // Loop through contact's fields and find the specified field
    const TInt fieldCount = iContact.Fields().FieldCount();
    CVPbkDefaultAttribute* attr =
        CVPbkDefaultAttribute::NewL( aDefaultType );
    CleanupStack::PushL( attr );

    for ( TInt i=0; i < fieldCount; ++i )
        {
        MVPbkStoreContactField* candidate =
            iContact.Fields().FieldAtLC( i );

        // Check if field has default attribute defaultType
        if ( iManager.ContactAttributeManagerL().HasFieldAttributeL
                ( *attr, *candidate ) )
            {
            field = candidate;
            CleanupStack::Pop(); // candicate
            break;
            }
        else
            {
            CleanupStack::PopAndDestroy(); // candidate
            }
        }

    CleanupStack::PopAndDestroy( attr );
    return field;
    }

// --------------------------------------------------------------------------
// CPbk2DefaultAttributeProcess::DoSetL
// --------------------------------------------------------------------------
//
void CPbk2DefaultAttributeProcess::DoSetL( TVPbkDefaultType aDefaultType )
    {
    // Set given attribute to given field
    CVPbkDefaultAttribute* attr =
        CVPbkDefaultAttribute::NewL( aDefaultType );
    CleanupStack::PushL( attr );

    delete iSetAttributeOperation;
    iSetAttributeOperation = NULL;
    iSetAttributeOperation =
        iManager.ContactAttributeManagerL().SetFieldAttributeL
            ( *iSetAttributeField, *attr, *this );

    CleanupStack::PopAndDestroy( attr );
    }

// --------------------------------------------------------------------------
// CPbk2DefaultAttributeProcess::DoRemoveL
// --------------------------------------------------------------------------
//
TBool CPbk2DefaultAttributeProcess::DoRemoveL
        ( TVPbkDefaultType aDefaultType )
    {
    TBool found = EFalse;

    delete iRemoveAttributeField;
    iRemoveAttributeField = NULL;
    iRemoveAttributeField = FindContactFieldWithAttributeL( aDefaultType );

    if ( iRemoveAttributeField )
        {
        found = ETrue;

        CVPbkDefaultAttribute* attr =
            CVPbkDefaultAttribute::NewL( aDefaultType );
        CleanupStack::PushL( attr );

        delete iRemoveAttributeOperation;
        iRemoveAttributeOperation = NULL;
        iRemoveAttributeOperation = iManager.ContactAttributeManagerL().
            RemoveFieldAttributeL( *iRemoveAttributeField, *attr, *this );

        CleanupStack::PopAndDestroy( attr );
        }

    return found;
    }

// --------------------------------------------------------------------------
// CPbk2DefaultAttributeProcess::DoRemovePreviousL
// --------------------------------------------------------------------------
//
TBool CPbk2DefaultAttributeProcess::DoRemovePreviousL
        ( TVPbkDefaultType aDefaultType )
    {
    TBool found = EFalse;

    delete iRemoveAttributeField;
    iRemoveAttributeField = NULL;
    iRemoveAttributeField = FindContactFieldWithAttributeL( aDefaultType );

    if ( iRemoveAttributeField )
        {
        found = ETrue;

        CVPbkDefaultAttribute* attr =
            CVPbkDefaultAttribute::NewL( aDefaultType );
        CleanupStack::PushL( attr );

        delete iRemovePreviousAttributeOperation;
        iRemovePreviousAttributeOperation = NULL;
        iRemovePreviousAttributeOperation =
            iManager.ContactAttributeManagerL().RemoveFieldAttributeL
                ( *iRemoveAttributeField, *attr, *this );

        CleanupStack::PopAndDestroy( attr );
        }

    return found;
    }

// --------------------------------------------------------------------------
// CPbk2DefaultAttributeProcess::SetNextL
// --------------------------------------------------------------------------
//
void CPbk2DefaultAttributeProcess::SetNextL()
    {
    TVPbkDefaultType attributeType = NextAttribute();
    if ( attributeType != EVPbkDefaultTypeUndefined )
        {
        iAttributeType = attributeType;
        // Remove previous, if any
        if ( !DoRemovePreviousL( attributeType ) )
            {
            DoSetL( attributeType );
            }
        }
    else
        {
        // Finished
        iObserver.AttributeProcessCompleted();
        }
    }

// --------------------------------------------------------------------------
// CPbk2DefaultAttributeProcess::RemoveNextL
// --------------------------------------------------------------------------
//
void CPbk2DefaultAttributeProcess::RemoveNextL()
    {
    TVPbkDefaultType attributeType = NextAttribute();
    if ( attributeType != EVPbkDefaultTypeUndefined )
        {
        if ( !DoRemoveL( attributeType ) )
            {
            RemoveNextL();
            }
        }
    else
        {
        // Finished
        iObserver.AttributeProcessCompleted();
        }
    }

// --------------------------------------------------------------------------
// CPbk2DefaultAttributeProcess::NextAttribute
// Gets next attribute from the array. The array is processed backwards.
// --------------------------------------------------------------------------
//
inline TVPbkDefaultType CPbk2DefaultAttributeProcess::NextAttribute()
    {
    TVPbkDefaultType attribute = EVPbkDefaultTypeUndefined;
    TInt count = 0;
    if ( iDefaultAttributes )
        {
        count = iDefaultAttributes->Count();
        }

    if ( count > 0 )
        {
        attribute = iDefaultAttributes->At( count - 1 ); // zero-based
        iDefaultAttributes->Delete( count -1 ); // zero-based
        }
    return attribute;
    }

// End of File

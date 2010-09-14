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
* Description:  Phonebook 2 contact editor reading field visitor.
*
*/


#include "CPbk2ReadingFieldEditorVisitor.h"

// Phonebook 2
#include <MPbk2ContactEditorField.h>
#include <CPbk2PresentationContactField.h>
#include <pbk2uicontrols.rsg>

// Virtual Phonebook
#include <MVPbkFieldType.h>
#include <MVPbkBaseContactField.h>
#include <CVPbkContactManager.h>
#include <CVPbkFieldTypeSelector.h>

// System includes
#include <AknReadingConverter.h>
#include <barsread.h>

// Debugging headers
#include <Pbk2Debug.h>


/// Unnamed namespace for local definitions
namespace {

/**
 * Matches field type.
 *
 * @param aFieldTypeList    The field type list to use.
 * @param aField            The field whose type to match.
 * @return  Matched field type.
 */
const MVPbkFieldType* MatchFieldType
        ( const MVPbkFieldTypeList& aFieldTypeList,
          const MVPbkBaseContactField& aField )
    {
    const MVPbkFieldType* result = NULL;

    for ( TInt matchPriority = 0; 
          matchPriority <= aFieldTypeList.MaxMatchPriority() && !result;
          ++matchPriority )
        {
        result = aField.MatchFieldType( matchPriority );
        }

    return result;
    }

/**
 * Checks is the given field type included in
 * the given selection.
 *
 * @param aField        The field whose type to check.
 * @param aManager      Contact manager.
 * @param aResourceId   Selector's resource id.
 * @return  ETrue if field type is included.
 */
TBool IsFieldTypeIncludedL(
        const MVPbkBaseContactField& aField,
        const CVPbkContactManager& aManager,
        const TInt aResourceId )
    {
    // Get the field type
    const MVPbkFieldType* fieldType = MatchFieldType
        ( aManager.FieldTypes(), aField );
    
    TResourceReader resReader;
    CCoeEnv::Static()->CreateResourceReaderLC
        ( resReader, aResourceId );
    
    CVPbkFieldTypeSelector* selector =
        CVPbkFieldTypeSelector::NewL( resReader, aManager.FieldTypes() );
    CleanupStack::PopAndDestroy(); // resReader

    TBool ret = selector->IsFieldTypeIncluded( *fieldType );
    delete selector;
    return ret;
    }

} /// namespace


// --------------------------------------------------------------------------
// CPbk2ReadingFieldEditorVisitor::CPbk2ReadingFieldEditorVisitor
// --------------------------------------------------------------------------
//
inline CPbk2ReadingFieldEditorVisitor::CPbk2ReadingFieldEditorVisitor
        ( const CVPbkContactManager& aContactManager ):
            iContactManager( aContactManager )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ReadingFieldEditorVisitor::~CPbk2ReadingFieldEditorVisitor
// --------------------------------------------------------------------------
//
CPbk2ReadingFieldEditorVisitor::~CPbk2ReadingFieldEditorVisitor()
    {
    delete iFirstNameConverter;
    delete iLastNameConverter;
    }

// --------------------------------------------------------------------------
// CPbk2ReadingFieldEditorVisitor::NewL
// --------------------------------------------------------------------------
//
CPbk2ReadingFieldEditorVisitor* CPbk2ReadingFieldEditorVisitor::NewL
        ( const CVPbkContactManager& aContactManager )
    {
    return new ( ELeave )
        CPbk2ReadingFieldEditorVisitor( aContactManager );
    }

// --------------------------------------------------------------------------
// CPbk2ReadingFieldEditorVisitor::VisitL
// --------------------------------------------------------------------------
//
void CPbk2ReadingFieldEditorVisitor::VisitL
        ( MPbk2ContactEditorField& /* aThis */ )
    {
    }
    
// --------------------------------------------------------------------------
// CPbk2ReadingFieldEditorVisitor::SetEditorsL
// --------------------------------------------------------------------------
//
inline void CPbk2ReadingFieldEditorVisitor::SetEditorsL
        ( MPbk2ContactEditorField& aThis )
    {
    const MVPbkStoreContactField& field = aThis.ContactField();
    
    if ( IsFieldTypeIncludedL( field, iContactManager,
         R_PHONEBOOK2_LAST_NAME_SELECTOR ) )
        {
        iLastNameConverter->SetMainEditor( *aThis.Control() );
        }
    else if ( IsFieldTypeIncludedL( field, iContactManager,
               R_PHONEBOOK2_FIRST_NAME_SELECTOR ) )
        {
        iFirstNameConverter->SetMainEditor( *aThis.Control() );
        }
    else if ( IsFieldTypeIncludedL( field, iContactManager,
               R_PHONEBOOK2_LAST_NAME_PRONUNCIATION_SELECTOR ) )
        {
        iLastNameConverter->SetReadingEditor( *aThis.Control() );
        }
    else if ( IsFieldTypeIncludedL( field, iContactManager,
               R_PHONEBOOK2_FIRST_NAME_PRONUNCIATION_SELECTOR ) )
        {
        iFirstNameConverter->SetReadingEditor( *aThis.Control() );
        }
    }

// End of File

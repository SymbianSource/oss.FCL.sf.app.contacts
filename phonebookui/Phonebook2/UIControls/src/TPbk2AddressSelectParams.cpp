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
* Description:  Phonebook 2 address selection parameters.
*
*/


#include <TPbk2AddressSelectParams.h>

// Phonebook 2
#include <MPbk2ContactNameFormatter.h>
#include <CPbk2FieldPropertyArray.h>

// Virtual Phonebook
#include <MVPbkStoreContactField.h>
#include <MVPbkStoreContact.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <CVPbkContactManager.h>


// --------------------------------------------------------------------------
// TPbk2AddressSelectParams::TPbk2AddressSelectParams
// --------------------------------------------------------------------------
//
EXPORT_C TPbk2AddressSelectParams::TPbk2AddressSelectParams
        ( MVPbkStoreContact& aContact,
          const CVPbkContactManager& aContactManager,
          MPbk2ContactNameFormatter& aNameFormatter,
          const CPbk2FieldPropertyArray& aFieldPropertyArray,
          TResourceReader& aReader, TInt aTitleResId ) :
            iContact( aContact ),
            iContactManager( aContactManager ),
            iNameFormatter( aNameFormatter ),
            iFieldPropertyArray( aFieldPropertyArray ),
            iResReader( aReader ),
            iTitleResId( aTitleResId ),
            iFocusedField( NULL ),
            iDefaultPriorities( NULL ),
            iUseDefaultDirectly( EFalse ),
            iQueryAlways( EFalse ),
            iIncludeContactNameInPrompt( ETrue ),
            iSuppressWarnings( EFalse ),
            iCommMethod( VPbkFieldTypeSelectorFactory::EEmptySelector )
    {
    }

// --------------------------------------------------------------------------
// TPbk2AddressSelectParams::SetFocusedField
// --------------------------------------------------------------------------
//
EXPORT_C void TPbk2AddressSelectParams::SetFocusedField
        ( const MVPbkStoreContactField* aFocusedField )
    {
    iFocusedField = aFocusedField;
    }

// --------------------------------------------------------------------------
// TPbk2AddressSelectParams::SetDefaultPriorities
// --------------------------------------------------------------------------
//
EXPORT_C void TPbk2AddressSelectParams::SetDefaultPriorities
        ( const RVPbkContactFieldDefaultPriorities& aDefaultPriorities )
    {
    iDefaultPriorities = &aDefaultPriorities;
    }

// --------------------------------------------------------------------------
// TPbk2AddressSelectParams::SetUseDefaultDirectly
// --------------------------------------------------------------------------
//
EXPORT_C void TPbk2AddressSelectParams::SetUseDefaultDirectly
        ( TBool aUseDefaultDirectly )
    {
    iUseDefaultDirectly = aUseDefaultDirectly;
    }

// --------------------------------------------------------------------------
// TPbk2AddressSelectParams::SetQueryAlways
// --------------------------------------------------------------------------
//
EXPORT_C void TPbk2AddressSelectParams::SetQueryAlways
        ( TBool aQueryAlways )
    {
    iQueryAlways = aQueryAlways;
    }

// --------------------------------------------------------------------------
// TPbk2AddressSelectParams::SetQueryAlways
// --------------------------------------------------------------------------
//
EXPORT_C void TPbk2AddressSelectParams::SetTitleToIncludeContactName
        ( TBool aIncludeContactName )
    {
    iIncludeContactNameInPrompt = aIncludeContactName;
    }

// --------------------------------------------------------------------------
// TPbk2AddressSelectParams::SetSuppressWarnings
// --------------------------------------------------------------------------
//
EXPORT_C void TPbk2AddressSelectParams::SetSuppressWarnings
        ( TBool aSuppressWarnings )
    {
    iSuppressWarnings = aSuppressWarnings;
    }

// End of File

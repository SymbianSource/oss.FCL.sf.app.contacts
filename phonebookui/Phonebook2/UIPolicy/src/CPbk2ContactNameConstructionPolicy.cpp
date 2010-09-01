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
* Description:  Phonebook 2 contact name construction policy.
*
*/


#include "CPbk2ContactNameConstructionPolicy.h"

// Phonebook 2
#include <CPbk2SortOrderManager.h>
#include <Pbk2ContactNameFormatterFactory.h>
#include <MPbk2ContactNameFormatter.h>
#include <MPbk2ContactNameFormatter3.h>
#include <CVPbkContactFieldIterator.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <CVPbkFieldTypeRefsList.h>

#include <featmgr.h>

// --------------------------------------------------------------------------
// CPbk2ContactNameConstructionPolicy::CPbk2ContactNameConstructionPolicy
// --------------------------------------------------------------------------
//
CPbk2ContactNameConstructionPolicy::CPbk2ContactNameConstructionPolicy
        ( const MVPbkFieldTypeList& aMasterFieldTypeList ) :
            iMasterFieldTypeList( aMasterFieldTypeList )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactNameConstructionPolicy::~CPbk2ContactNameConstructionPolicy
// --------------------------------------------------------------------------
//
CPbk2ContactNameConstructionPolicy::~CPbk2ContactNameConstructionPolicy()
    {
    delete iSortOrderManager;
    delete iNameFormatter;
    FeatureManager::UnInitializeLib();
    }

// --------------------------------------------------------------------------
// CPbk2ContactNameConstructionPolicy::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactNameConstructionPolicy* CPbk2ContactNameConstructionPolicy::NewL
        ( TParam* aParam )
    {
    CPbk2ContactNameConstructionPolicy* self =
        new ( ELeave ) CPbk2ContactNameConstructionPolicy
            ( aParam->iMasterFieldTypeList );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactNameConstructionPolicy::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactNameConstructionPolicy::ConstructL()
    {
    iSortOrderManager = CPbk2SortOrderManager::NewL( iMasterFieldTypeList );

    iNameFormatter = Pbk2ContactNameFormatterFactory::CreateL
        ( iMasterFieldTypeList, *iSortOrderManager );
    FeatureManager::InitializeLibL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactNameConstructionPolicy::NameConstructionFieldsLC
// --------------------------------------------------------------------------
//
MVPbkBaseContactFieldIterator*
    CPbk2ContactNameConstructionPolicy::NameConstructionFieldsLC
        ( const MVPbkBaseContactFieldCollection& aFieldCollection,
          CVPbkFieldTypeRefsList& aFieldTypeRefsList )
    {
    if( FeatureManager::FeatureSupported( KFeatureIdFfContactsCompanyNames ) )
        {
        MPbk2ContactNameFormatter3* nameformatterExtension =
                reinterpret_cast<MPbk2ContactNameFormatter3*>( iNameFormatter->
                ContactNameFormatterExtension( MPbk2ContactNameFormatterExtension3Uid ) );
        
        if ( nameformatterExtension )
            {
            return nameformatterExtension->TitleWithCompanyNameFieldsLC( aFieldTypeRefsList, aFieldCollection );
            }
        }
    return iNameFormatter->ActualTitleFieldsLC( aFieldTypeRefsList, aFieldCollection );
    }

// End of File

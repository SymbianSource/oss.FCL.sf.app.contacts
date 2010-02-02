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
*     Phonebook 2 contact view builder.
*
*/


#include "CPbk2ContactViewBuilder.h"

// Phonebook 2
#include "Pbk2PresentationUtils.h"
#include "CPbk2StoreConfiguration.h"
#include "CPbk2StoreProperty.h"
#include "CPbk2StorePropertyArray.h"
#include "CPbk2StoreViewDefinition.h"
#include <RPbk2LocalizedResourceFile.h>
#include <Pbk2Presentation.rsg>
#include <Pbk2DataCaging.hrh>

 #include <CVPbkFieldTypeSelector.h>
 #include <VPbkContactViewFilterBuilder.h>
    
// Virtual Phonebook
#include <CVPbkContactStoreUriArray.h>
#include <CVPbkContactViewDefinition.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactViewBase.h>
#include <MVPbkContactSelector.h>
#include <VPbkContactView.hrh>

// System includes
#include <coemain.h>
#include <barsread.h>


// --------------------------------------------------------------------------
// CPbk2ContactViewBuilder::CPbk2ContactViewBuilder
// --------------------------------------------------------------------------
//
CPbk2ContactViewBuilder::CPbk2ContactViewBuilder(
            CVPbkContactManager& aContactManager,
            CPbk2StorePropertyArray& aStoreProperties ) :
        iContactManager( aContactManager ),
        iStoreProperties( aStoreProperties )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewBuilder::~CPbk2ContactViewBuilder
// --------------------------------------------------------------------------
//
CPbk2ContactViewBuilder::~CPbk2ContactViewBuilder()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewBuilder::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ContactViewBuilder* CPbk2ContactViewBuilder::NewLC(
        CVPbkContactManager& aContactManager,
        CPbk2StorePropertyArray& aStoreProperties )
    {
    CPbk2ContactViewBuilder* self = new ( ELeave ) CPbk2ContactViewBuilder(
        aContactManager, aStoreProperties );
    CleanupStack::PushL( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewBuilder::CreateContactViewForStoresLC
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactViewBase*
    CPbk2ContactViewBuilder::CreateContactViewForStoresLC(
        const CVPbkContactStoreUriArray& aUriArray,
        MVPbkContactViewObserver& aObserver,
        const MVPbkFieldTypeList& aSortOrder,
        CVPbkFieldTypeSelector* aFilter,
        TUint32 aFlags ) const
    {
    return CreateViewForStoresLC
        ( EPbk2MainNamesList, aUriArray, aObserver, aSortOrder, aFilter, aFlags );
    }


// --------------------------------------------------------------------------
// CPbk2ContactViewBuilder::CreateTopNonTopContactViewForStoresLC
//
// Similar as CreateContactViewForStoresLC but TopContacts are filtered out
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactViewBase*
    CPbk2ContactViewBuilder::CreateTopNonTopContactViewForStoresLC(
        const CVPbkContactStoreUriArray& aUriArray,
        MVPbkContactViewObserver& aObserver,
        const MVPbkFieldTypeList& aSortOrder,
        CVPbkFieldTypeSelector* aFilter,
        TUint32 aFlags,
        MVPbkContactSelector& aContactSelector ) const
    {
    iContactSelector = &aContactSelector;

    return CreateViewForStoresLC(
        EPbk2MainTopContactsList, // This param has no effect if aTopContacts=ETrue
        aUriArray,
        aObserver,
        aSortOrder,
        aFilter,
        aFlags,
        ETrue ); // aTopContacts = ETrue
    }
    
// --------------------------------------------------------------------------
// CPbk2ContactViewBuilder::CreateGroupViewForStoresLC
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactViewBase*
    CPbk2ContactViewBuilder::CreateGroupViewForStoresLC(
        const CVPbkContactStoreUriArray& aUriArray,
        MVPbkContactViewObserver& aObserver,
        const MVPbkFieldTypeList& aSortOrder,
        CVPbkFieldTypeSelector* aFilter,
        TUint32 aFlags ) const
    {
    return CreateViewForStoresLC
        ( EPbk2MainGroupsList, aUriArray, aObserver, aSortOrder, aFilter, aFlags );
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewBuilder::BuildMainViewStructureLC
// --------------------------------------------------------------------------
//
CVPbkContactViewDefinition*
        CPbk2ContactViewBuilder::BuildMainViewStructureLC(
        		CVPbkFieldTypeSelector* aFilter,
        		TUint32 aFlags ) const
    {
    RPbk2LocalizedResourceFile resFile( *CCoeEnv::Static() );
    resFile.OpenLC( KPbk2RomFileDrive, 
        KDC_RESOURCE_FILES_DIR, 
        Pbk2PresentationUtils::PresentationResourceFile() );
    
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC
        ( reader, R_PHONEBOOK2_MAIN_CONTACT_VIEW_STRUCTURE );

    CVPbkContactViewDefinition* viewDef =
        CVPbkContactViewDefinition::NewL( reader );
    CleanupStack::PopAndDestroy(2); // reader, resFile
    CleanupStack::PushL( viewDef );
    
    if( aFlags & EVPbkExcludeEmptyGroups )
    	{
    	viewDef->SetFlag ( EVPbkExcludeEmptyGroups, ETrue );
    	}
    
    viewDef->SetFieldTypeFilterL( aFilter );
    return viewDef;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewBuilder::CreateViewForStoresLC
// Creates a sub-view for all matching view types, from the listed stores. 
// --------------------------------------------------------------------------
//
/**
 * Creates a sub-view for all matching view types from the listed stores. 
 * @param aViewType The view type that should match.
 * @param aUriArray The Stores from which to look for views.
 * @param aObserver An observer
 * @param aSortOrder
 * @param aFilter An object that is able to discard contacts based on
 *                  field collection.
 * @param aTopContacts Whether or not to construct a top/non-top-contact style
 *                  of list, or a normal legacy list with no top contacts.
 * @return The actual new view. 
 */
MVPbkContactViewBase* CPbk2ContactViewBuilder::CreateViewForStoresLC(
        TPbk2ContactViewType aViewType,
        const CVPbkContactStoreUriArray& aUriArray,
        MVPbkContactViewObserver& aObserver,
        const MVPbkFieldTypeList& aSortOrder,
        CVPbkFieldTypeSelector* aFilter,
        TUint32 aFlags,
        TBool aTopContacts ) const
    {
    // Main view is a composite view that contains several sub composite
    // views. The subviews are identified by TPbk2ContactViewLocation. 
    CVPbkContactViewDefinition* viewDef = BuildMainViewStructureLC( aFilter, aFlags );
    
    const TInt count = aUriArray.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        // Get store property for each URI
        const CPbk2StoreProperty* storeProperty =
            iStoreProperties.FindProperty( aUriArray[i] );
        if ( storeProperty )
            {
            // Loop all view definitions of the store and
            // add the view definitions to the correct sub view of 
            // the main structure.
            TArray<const CPbk2StoreViewDefinition*> viewDefs = 
                storeProperty->Views();
            const TInt viewDefCount = viewDefs.Count();
            for ( TInt j = 0; j < viewDefCount; ++j )
                {
                const CPbk2StoreViewDefinition* pb2ViewDef = viewDefs[j];
                // Check that UI type of the view is the given type.
                // Meaning that does the view belong to all-contacts-view
                // or all-groups-view.
                
                TBool includeThisView = pb2ViewDef->ViewType() == aViewType;
                if ( aTopContacts )
                    {
                    includeThisView = IncludeThisViewInTopContactMode( pb2ViewDef->ViewType() );
                    }
                if ( includeThisView )
                    {
                    // Get the correct sub view of the main view structure.
                    CVPbkContactViewDefinition& subView = viewDef->SubViewAt( 
                        TInt( pb2ViewDef->ViewLocation() ) );
                    // Add store's view to composite.
                    CVPbkContactViewDefinition* newSubView =
                        CVPbkContactViewDefinition::NewL( 
                            pb2ViewDef->ViewDefinition() );
                    CleanupStack::PushL( newSubView );
                    if ( aTopContacts )
                        {
                        SetTopRelatedFilteringL(
                            *newSubView, pb2ViewDef->ViewType(), aFilter );
                        }
                    else
                        {
                        newSubView->SetFieldTypeFilterL( aFilter );
                        }
                    if( aFlags & EVPbkExcludeEmptyGroups )
                        {
                        newSubView->SetFlag ( EVPbkExcludeEmptyGroups, ETrue );
                        }
                    
                    if( aFlags & EVPbkExcludeEmptyGroups )
                    	{
                    	newSubView->SetFlag ( EVPbkExcludeEmptyGroups, ETrue );
                    	}
                    
                    subView.AddSubViewL( newSubView );
                    CleanupStack::Pop();
                    }
                }
            }
        }

    MVPbkContactViewBase* result = iContactManager.CreateContactViewLC
        ( aObserver, *viewDef, aSortOrder );
    CleanupStack::Pop(); // result

    CleanupStack::PopAndDestroy(); // viewDef
    CleanupDeletePushL( result );
    return result;
    }
                    
                    
TBool CPbk2ContactViewBuilder::IncludeThisViewInTopContactMode(
    TPbk2ContactViewType aViewType )
    {
    return aViewType == EPbk2MainTopContactsList || aViewType == EPbk2MainNamesList;
    }
    
void CPbk2ContactViewBuilder::SetTopRelatedFilteringL(
    CVPbkContactViewDefinition& aNewSubViewDef,
    TPbk2ContactViewType aViewType,
    CVPbkFieldTypeSelector* aFilter ) const
    {
    if ( aViewType == EPbk2MainTopContactsList )
        {
        //Override original filter with top contact filter.
        CVPbkFieldTypeSelector* filter = CVPbkFieldTypeSelector::NewL(
            iContactManager.FieldTypes());
        CleanupStack::PushL(filter);         

        //Set the filter to retrieve Top contact items (VPbkContactView.hrh)
        VPbkContactViewFilterBuilder::BuildContactViewFilterL( *filter, 
            TVPbkContactViewFilter(EVPbkContactViewFilterTopContact), iContactManager );
        aNewSubViewDef.SetFieldTypeFilterL( filter ); 
        CleanupStack::PopAndDestroy(); //filter  
        }
    else if ( aViewType == EPbk2MainNamesList )
        {
        //Use original filter, if any
        aNewSubViewDef.SetFieldTypeFilterL( aFilter );  
        if ( iContactSelector )
            {
            // Add the contact selector
            // (usually the Non-Top Contacts selector)
            aNewSubViewDef.SetContactSelector( iContactSelector ); // ownership not transferred
            }
        }
    else
        {
        //Use original filter, if any
        aNewSubViewDef.SetFieldTypeFilterL( aFilter );          
        }
    }

// End of File

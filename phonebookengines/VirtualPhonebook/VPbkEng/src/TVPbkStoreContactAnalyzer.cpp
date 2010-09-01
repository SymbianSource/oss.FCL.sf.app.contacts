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
* Description:  VPbk store contact analyzer.
*
*/


#include "TVPbkStoreContactAnalyzer.h"

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkUtils.h>

// System includes
#include <barsread.h>
#include <coemain.h>

// non-class function declarations
template<typename T> TInt t_HasFieldL( T aSelector,
                           TInt aStartIndex,
                           const MVPbkStoreContact* aContact,
                           const MVPbkStoreContact* aClassContact,
                           const CVPbkContactManager& aClassContactManager ); 

template<typename T> TBool t_IsFieldTypeSupportedL( T aSelector, 
                           const MVPbkStoreContact* aContact,
                           const MVPbkStoreContact* aClassContact,
                           const CVPbkContactManager& aClassContactManager );

template<typename T> TBool t_IsFieldTypeIncludedL(
                           const MVPbkFieldType& aFieldType,
                           T aSelector,
                           const CVPbkContactManager& aClassContactManager );

template<typename T> TBool t_IsFieldTypeIncludedL(
                           const MVPbkBaseContactField& aField,
                           T aSelector,
                           const CVPbkContactManager& aClassContactManager );

CVPbkFieldTypeSelector* CreateSelectorLC(
             VPbkFieldTypeSelectorFactory::TVPbkFieldTypeSelector aType,
             const CVPbkContactManager& aContactManager );

CVPbkFieldTypeSelector* CreateSelectorLC(
             VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aType,
             const CVPbkContactManager& aContactManager );

TBool IsFieldTypeIncluded
        ( const MVPbkFieldTypeList& aFieldTypes,
          const MVPbkFieldTypeSelector& aFieldTypeSelector );

TInt IsFieldIncluded
        (const MVPbkBaseContactFieldCollection& aFields,
        TInt aStartIndex,
        const MVPbkContactFieldSelector& aFieldTypeSelector);


// --------------------------------------------------------------------------
// TVPbkStoreContactAnalyzer::TVPbkStoreContactAnalyzer
// --------------------------------------------------------------------------
//
EXPORT_C TVPbkStoreContactAnalyzer::TVPbkStoreContactAnalyzer
        ( const CVPbkContactManager& aContactManager,
        const MVPbkStoreContact* aContact ) :
            iContactManager( aContactManager ),
            iContact( aContact )
    {
    }

// --------------------------------------------------------------------------
// TVPbkStoreContactAnalyzer::HasFieldL
// --------------------------------------------------------------------------
//
EXPORT_C TInt TVPbkStoreContactAnalyzer::HasFieldL
        ( VPbkFieldTypeSelectorFactory::TVPbkFieldTypeSelector aFieldTypeSelector,
         TInt aStartIndex/* = 0*/,
         const MVPbkStoreContact* aContact/* = NULL*/ ) const
    { 
    return t_HasFieldL<VPbkFieldTypeSelectorFactory::TVPbkFieldTypeSelector>
                            (aFieldTypeSelector,aStartIndex,aContact,
                                  iContact, iContactManager);
    }

// --------------------------------------------------------------------------
// TVPbkStoreContactAnalyzer::HasFieldL
// --------------------------------------------------------------------------
//
EXPORT_C TInt TVPbkStoreContactAnalyzer::HasFieldL
        ( VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aActionTypeSelector,
         TInt aStartIndex/* = 0*/,
         const MVPbkStoreContact* aContact/* = NULL*/ ) const
    {
    return t_HasFieldL<VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector>
                               (aActionTypeSelector,aStartIndex,aContact,
                                     iContact, iContactManager);
    }

// --------------------------------------------------------------------------
// TVPbkStoreContactAnalyzer::IsFieldTypeSupportedL
// --------------------------------------------------------------------------
//
EXPORT_C TBool TVPbkStoreContactAnalyzer::IsFieldTypeSupportedL
        ( VPbkFieldTypeSelectorFactory::TVPbkFieldTypeSelector aFieldTypeSelector, 
          const MVPbkStoreContact* aContact /*= NULL*/ ) const
    {
    return t_IsFieldTypeSupportedL<VPbkFieldTypeSelectorFactory::TVPbkFieldTypeSelector>
                ( aFieldTypeSelector, aContact, iContact, iContactManager);
    }

// --------------------------------------------------------------------------
// TVPbkStoreContactAnalyzer::IsFieldTypeSupportedL
// --------------------------------------------------------------------------
//
EXPORT_C TBool TVPbkStoreContactAnalyzer::IsFieldTypeSupportedL
        ( VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aActionTypeSelector, 
          const MVPbkStoreContact* aContact /*= NULL*/ ) const
    {
    return t_IsFieldTypeSupportedL<VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector>
                ( aActionTypeSelector, aContact, iContact, iContactManager);
    }

// --------------------------------------------------------------------------
// TVPbkStoreContactAnalyzer::IsFieldTypeIncludedL
// --------------------------------------------------------------------------
//
EXPORT_C TBool TVPbkStoreContactAnalyzer::IsFieldTypeIncludedL
        ( const MVPbkFieldType& aFieldType,
          VPbkFieldTypeSelectorFactory::TVPbkFieldTypeSelector aFieldTypeSelector ) const
    {
    return t_IsFieldTypeIncludedL<VPbkFieldTypeSelectorFactory::TVPbkFieldTypeSelector>
               ( aFieldType, aFieldTypeSelector, iContactManager );
    }

// --------------------------------------------------------------------------
// TVPbkStoreContactAnalyzer::IsFieldTypeIncludedL
// --------------------------------------------------------------------------
//
EXPORT_C TBool TVPbkStoreContactAnalyzer::IsFieldTypeIncludedL
        ( const MVPbkFieldType& aFieldType,
          VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aActionTypeSelector ) const
    {
    return t_IsFieldTypeIncludedL<VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector>
               ( aFieldType, aActionTypeSelector, iContactManager );
    }

// --------------------------------------------------------------------------
// TVPbkStoreContactAnalyzer::IsFieldTypeIncludedL
// --------------------------------------------------------------------------
//
EXPORT_C TBool TVPbkStoreContactAnalyzer::IsFieldTypeIncludedL
        ( const MVPbkBaseContactField& aField,
          VPbkFieldTypeSelectorFactory::TVPbkFieldTypeSelector aFieldTypeSelector ) const
    {
    return t_IsFieldTypeIncludedL<VPbkFieldTypeSelectorFactory::TVPbkFieldTypeSelector>
                   ( aField, aFieldTypeSelector, iContactManager );
    }

// --------------------------------------------------------------------------
// TVPbkStoreContactAnalyzer::IsFieldTypeIncludedL
// --------------------------------------------------------------------------
//
EXPORT_C TBool TVPbkStoreContactAnalyzer::IsFieldTypeIncludedL
        ( const MVPbkBaseContactField& aField,
          VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aActionTypeSelector ) const
    {
    return t_IsFieldTypeIncludedL<VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector>
                   ( aField, aActionTypeSelector, iContactManager );
    }

// --------------------------------------------------------------------------
// t_HasFieldL
// --------------------------------------------------------------------------
//
template<class T> TInt t_HasFieldL( T aSelector, TInt aStartIndex,
                       const MVPbkStoreContact* aContact,
                       const MVPbkStoreContact* aClassContact,
                       const CVPbkContactManager& aClassContactManager )
    {
    TInt ret = KErrNotFound;
    const MVPbkStoreContact* tempContact = aClassContact;

    if ( aContact )
        {
        tempContact = aContact;
        }

    // Check that there is contact to analyze
    if ( tempContact )
        {
        // Read the contact field selector from resource
        MVPbkContactFieldSelector* fieldTypeSelector =
            static_cast<MVPbkContactFieldSelector*>
                ( CreateSelectorLC( aSelector, aClassContactManager ) );

        // Check if the specified field is included to contact
        const MVPbkStoreContactFieldCollection& fields =
            tempContact->Fields();
        ret = IsFieldIncluded( fields, aStartIndex, *fieldTypeSelector );
        CleanupStack::PopAndDestroy( ); // fieldTypeSelector;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// t_IsFieldTypeSupportedL
// --------------------------------------------------------------------------
//
template<class T> TBool t_IsFieldTypeSupportedL( T aSelector, 
                             const MVPbkStoreContact* aContact,
                             const MVPbkStoreContact* aClassContact,
                             const CVPbkContactManager& aClassContactManager )
     {
     TBool ret = EFalse;
     const MVPbkStoreContact* tempContact = aClassContact;
    
     if (aContact)
         {
         tempContact = aContact;
         }
    
     // Check that there is contact to analyze
     if ( tempContact )
         {
         // Read the field type selector from resource
         MVPbkFieldTypeSelector* fieldTypeSelector =
             static_cast<MVPbkFieldTypeSelector*>
                 ( CreateSelectorLC( aSelector, aClassContactManager ) );
        
         // Check if the specified field type is supported by contact
         const MVPbkFieldTypeList& fieldTypes =
             tempContact->ParentStore().StoreProperties().SupportedFields();
         ret = IsFieldTypeIncluded( fieldTypes, *fieldTypeSelector );
         CleanupStack::PopAndDestroy( ); // fieldTypeSelector;
         }
    
     return ret;
     }

// --------------------------------------------------------------------------
// t_IsFieldTypeIncludedL
// --------------------------------------------------------------------------
//
template<class T> TBool t_IsFieldTypeIncludedL(
                           const MVPbkFieldType& aFieldType,
                           T aSelector,
                           const CVPbkContactManager& aClassContactManager )
    {
    CVPbkFieldTypeSelector* selector = CreateSelectorLC( aSelector, aClassContactManager );
    TBool ret = selector->IsFieldTypeIncluded( aFieldType );
    CleanupStack::PopAndDestroy( selector );
    return ret;
    }

template<class T> TBool t_IsFieldTypeIncludedL(
                           const MVPbkBaseContactField& aField,
                           T aSelector,
                           const CVPbkContactManager& aClassContactManager )
    {
    TBool ret = EFalse;
    // Get the field type
    const MVPbkFieldType* fieldType =
        VPbkUtils::MatchFieldType( aClassContactManager.FieldTypes(), aField );
  
    if ( fieldType )
        {
        CVPbkFieldTypeSelector* selector = CreateSelectorLC( aSelector,
            aClassContactManager );
        ret = selector->IsFieldTypeIncluded( *fieldType );
        CleanupStack::PopAndDestroy();
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CreateSelectorLC
// --------------------------------------------------------------------------
//
CVPbkFieldTypeSelector* CreateSelectorLC(
             VPbkFieldTypeSelectorFactory::TVPbkFieldTypeSelector aType,
             const CVPbkContactManager& aContactManager )
    {
    CVPbkFieldTypeSelector* fieldTypeSelector = VPbkFieldTypeSelectorFactory::BuildFieldTypeSelectorL(
                    aType, aContactManager.FieldTypes() );
  
    CleanupStack::PushL( fieldTypeSelector );
    return fieldTypeSelector;
    }

// --------------------------------------------------------------------------
// CreateSelectorLC
// --------------------------------------------------------------------------
//
CVPbkFieldTypeSelector* CreateSelectorLC(
             VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aType,
             const CVPbkContactManager& aContactManager )
    {
    CVPbkFieldTypeSelector* fieldTypeSelector = VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                    aType, aContactManager.FieldTypes() );
  
    CleanupStack::PushL( fieldTypeSelector );
    return fieldTypeSelector;
    }

// --------------------------------------------------------------------------
// IsFieldTypeIncluded
// --------------------------------------------------------------------------
//
TBool IsFieldTypeIncluded
        ( const MVPbkFieldTypeList& aFieldTypes,
          const MVPbkFieldTypeSelector& aFieldTypeSelector ) 
    {
    TBool ret( EFalse );
    TInt fieldTypeCount( aFieldTypes.FieldTypeCount() );

    for ( TInt i = 0; i < fieldTypeCount && !ret; ++i )
        {
        const MVPbkFieldType& fieldType = aFieldTypes.FieldTypeAt( i );
        if ( aFieldTypeSelector.IsFieldTypeIncluded( fieldType ) )
            {
            ret = ETrue;
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// IsFieldTypeIncluded
// --------------------------------------------------------------------------
//
TInt IsFieldIncluded
        (const MVPbkBaseContactFieldCollection& aFields,
        TInt aStartIndex,
        const MVPbkContactFieldSelector& aFieldTypeSelector)
    {
    TInt ret( KErrNotFound );
    TInt fieldCount( aFields.FieldCount() );

    for ( TInt i = aStartIndex; i < fieldCount && ret == KErrNotFound; ++i )
        {
        const MVPbkBaseContactField& field = aFields.FieldAt( i );
        if ( aFieldTypeSelector.IsFieldIncluded( field ) )
            {
            ret = i;
            }
        }

    return ret;
    }

// End of File

/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility to help manage the remote view definitions.
*
*/


// INCLUDES
#include "NamedRemoteViewViewDefinitionStoreUtility.h"
#include <cntviewstore.h>


namespace VPbkCntModel {

TBool IsViewDefinitionContentSame(
        const CContactDefaultViewDefinition& aLhsViewDef, 
        const CContactDefaultViewDefinition& aRhsViewDef)
    {
    TBool ret = ETrue;
    // view exists in the view definition
    // check views configuration - compare sort orders
    if ( !( aLhsViewDef.SortOrder() == aRhsViewDef.SortOrder() ) )
        {
        return EFalse;
        }
    if ( aLhsViewDef.ViewPreferences() != 
            aRhsViewDef.ViewPreferences() )
        {
        return EFalse;
        }
    if ( aLhsViewDef.SortPluginNameL().CompareF( 
            aRhsViewDef.SortPluginNameL() ) )
        {
        return EFalse;
        }
    return ret;
    }

/// Sort plugin name
_LIT8(KSortPluginName, "/default");

void CleanupContent(TAny* aArray)
    {
    RPointerArray<CContactDefaultViewDefinition>* array = 
        static_cast<RPointerArray<CContactDefaultViewDefinition>* >(aArray);
    for (TInt i = 0; i < array->Count(); ++i)
        {
        CContactDefaultViewDefinition* elem = (*array)[i];
        delete elem;
        }
    }

// --------------------------------------------------------------------------
// NamedRemoteViewViewDefinitionStoreUtility::SetNamedRemoteViewViewDefinitionL
// --------------------------------------------------------------------------
//
void NamedRemoteViewViewDefinitionStoreUtility::SetNamedRemoteViewViewDefinitionL(
        const TDesC& aViewName, 
        const RContactViewSortOrder& aSortOrder, 
        TContactViewPreferences aContactTypes)
    {
    DoSetNamedRemoteViewViewDefinitionL( aViewName, aSortOrder, 
            aContactTypes );
    }

// --------------------------------------------------------------------------
// NamedRemoteViewViewDefinitionStoreUtility::DoSetNamedRemoteViewViewDefinitionL
// --------------------------------------------------------------------------
//
void NamedRemoteViewViewDefinitionStoreUtility::DoSetNamedRemoteViewViewDefinitionL(
        const TDesC& aViewName, 
        const RContactViewSortOrder& aSortOrder, 
        TContactViewPreferences aContactTypes)
    {
    // first get the already saved named remote view settings
    RPointerArray<CContactDefaultViewDefinition> defaultViews;
    CleanupClosePushL( defaultViews );
    TContactViewStore::GetDefaultViewsL(defaultViews);
    CleanupStack::PushL(TCleanupItem(CleanupContent, &defaultViews));

    CContactDefaultViewDefinition* newViewDef = CContactDefaultViewDefinition::NewLC(
        CContactDefaultViewDefinition::ENamedRemoteView, aViewName, 
        aSortOrder, aContactTypes, KSortPluginName );

    // Check whether the view exist in the settings
    TBool overwriteDefinition = EFalse;
    TInt index = -1;
    TBool addNewViewDef = ETrue;
    for ( TInt i = 0; i < defaultViews.Count(); ++i )
        {
        CContactDefaultViewDefinition* viewDef = defaultViews[i];
        if ( viewDef && !viewDef->ViewNameL().CompareF( aViewName ) )
            {
            addNewViewDef = EFalse;
            if ( !IsViewDefinitionContentSame(*viewDef, *newViewDef) )
                {
                overwriteDefinition = ETrue;
                index = i;
                break;
                }
            }
        }

    // change default views according to modifications
    if (overwriteDefinition)
        {
        // remove and delete previous definition
        CContactDefaultViewDefinition* def = defaultViews[index];
        defaultViews.Remove(index);
        delete def;
        }
    if (overwriteDefinition || addNewViewDef)
        {
        defaultViews.AppendL(newViewDef);
        CleanupStack::Pop( newViewDef );
        }
    else
        {
        CleanupStack::PopAndDestroy( newViewDef );
        }

    // write modified view definition
    if (overwriteDefinition || addNewViewDef)
        {
        TContactViewStore::SetDefaultViewsL( defaultViews );
        }

    CleanupStack::PopAndDestroy(); // Clean array elements
    CleanupStack::PopAndDestroy(); // close pointer array
    }


}  // namespace VPbkCntModel

// End of file

/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  STIF test module for testing VirtualPhonebook View Definition API
*
*/


#include "T_VPbkViewDefinitionApi.h"

#include <CVPbkContactViewDefinition.h>

// -----------------------------------------------------------------------------
// CVPbkViewDefinitionApi::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CT_VPbkViewDefinitionApi::Delete() 
    {
    if(iViewDefinition)
        {
    	delete iViewDefinition;
        iViewDefinition = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::RunMethodL
// Run specified method. Contains also table of test methods and their names.
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::RunMethodL( CStifItemParser& aItem ) 
    {
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "CreateContactViewDefinition", 
        		CT_VPbkViewDefinitionApi::CreateContactViewDefinition ),
        ENTRY( "DeleteContactViewDefinition", 
        		CT_VPbkViewDefinitionApi::DeleteContactViewDefinition ),
        ENTRY( "CountSubViews", 
        		CT_VPbkViewDefinitionApi::CountSubViews ),
        ENTRY( "ReturnSubViewAt",
        		CT_VPbkViewDefinitionApi::ReturnSubViewAt ),
        ENTRY( "ViewType",
        		CT_VPbkViewDefinitionApi::ViewType ),
        ENTRY( "FlagIsOn",
        		CT_VPbkViewDefinitionApi::FlagIsOn ),
        ENTRY( "Uri",
        		CT_VPbkViewDefinitionApi::Uri ),
        ENTRY( "SortPolicy",
        		CT_VPbkViewDefinitionApi::SortPolicy ),
        ENTRY( "Name",
        		CT_VPbkViewDefinitionApi::Name ),
        ENTRY( "Sharing",
        		CT_VPbkViewDefinitionApi::Sharing ),
        ENTRY( "FieldTypeFilter",
        		CT_VPbkViewDefinitionApi::FieldTypeFilter ),
        ENTRY( "AddSubView",
        		CT_VPbkViewDefinitionApi::AddSubView ),
	    ENTRY( "SetType",
	    		CT_VPbkViewDefinitionApi::SetType ),
	    ENTRY( "SetFlag",
	    		CT_VPbkViewDefinitionApi::SetFlag ),
	    ENTRY( "SetUri",
	    		CT_VPbkViewDefinitionApi::SetUri ),
	    ENTRY( "SetSortPolicy",
	    		CT_VPbkViewDefinitionApi::SetSortPolicy ),
	    ENTRY( "SetName",
	    		CT_VPbkViewDefinitionApi::SetName ),
	    ENTRY( "SetSharing",
	    		CT_VPbkViewDefinitionApi::SetSharing ),
	    ENTRY( "SetFieldTypeFilter",
	    		CT_VPbkViewDefinitionApi::SetFieldTypeFilter )
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );    
    }

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::CreateContactViewDefinition
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::CreateContactViewDefinition()
	{
	TRAPD( err, CreateContactViewDefinitionL() );
	return err;
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::CreateContactViewDefinitionL
// -----------------------------------------------------------------------------
//
void CT_VPbkViewDefinitionApi::CreateContactViewDefinitionL()
	{
	iViewDefinition = CVPbkContactViewDefinition::NewL();
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::DeleteContactViewDefinition
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::DeleteContactViewDefinition()
	{
	delete iViewDefinition;
    iViewDefinition = NULL;
	
	return 0;
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::CountSubViews
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::CountSubViews()
	{
	iViewDefinition->SubViewCount();
	
	return 0;
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::ReturnSubViewAt
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::ReturnSubViewAt()
	{
	iViewDefinition->SubViewAt( 0 );
	
	return 0;
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::ViewType
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::ViewType()
	{
	iViewDefinition->Type();
	
	return 0;
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::FlagIsOn
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::FlagIsOn()
	{
	iViewDefinition->FlagIsOn( EVPbkExcludeEmptyGroups );
	
	return 0;
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::Uri
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::Uri()
	{
	iViewDefinition->Uri();
	
	return 0;
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::SortPolicy
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::SortPolicy()
	{
	iViewDefinition->SortPolicy();
	
	return 0;
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::Name
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::Name()
	{
	iViewDefinition->Name();
	
	return 0;
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::Sharing
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::Sharing()
	{
	iViewDefinition->Sharing();
	
	return 0;
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::FieldTypeFilter
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::FieldTypeFilter()
	{
	iViewDefinition->FieldTypeFilter();
	
	return 0;
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::AddSubView
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::AddSubView()
	{
	TRAPD( err, AddSubViewL() )
	return err;
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::AddSubViewL
// -----------------------------------------------------------------------------
//
void CT_VPbkViewDefinitionApi::AddSubViewL()
	{
	CVPbkContactViewDefinition *aViewDefinition = CVPbkContactViewDefinition::NewL();
	iViewDefinition->AddSubViewL( aViewDefinition );
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::SetType
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::SetType()
	{
	iViewDefinition->SetType( EVPbkGroupsView );
	
	return 0;
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::SetFlag
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::SetFlag()
	{
	iViewDefinition->SetFlag( EVPbkExcludeEmptyGroups, ETrue );
	
	return 0;
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::SetUri
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::SetUri()
	{
	TRAPD( err, SetUriL() );
	return err;
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::SetUriL
// -----------------------------------------------------------------------------
//
void CT_VPbkViewDefinitionApi::SetUriL()
	{
	_LIT( KUri, "C://" );
	iViewDefinition->SetUriL( KUri );
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::SetSortPolicy
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::SetSortPolicy()
	{
    iViewDefinition->SetSortPolicy( EVPbkOrderedContactView );
	
	return 0;
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::SetName
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::SetName()
	{
    TRAPD( err, SetNameL() );
	
	return err;
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::SetNameL
// -----------------------------------------------------------------------------
//
void CT_VPbkViewDefinitionApi::SetNameL()
	{
    _LIT( KViewName, "ViewName" );
    iViewDefinition->SetNameL( KViewName );
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::SetSharing
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::SetSharing()
	{
	iViewDefinition->SetSharing( EVPbkLocalView );
	
	return 0;
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::SetFieldTypeFilter
// -----------------------------------------------------------------------------
//
TInt CT_VPbkViewDefinitionApi::SetFieldTypeFilter()
	{
	TRAPD( err, SetFieldTypeFilterL() );
	
	return err;
	}

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::SetFieldTypeFilterL
// -----------------------------------------------------------------------------
//
void CT_VPbkViewDefinitionApi::SetFieldTypeFilterL()
	{
	iViewDefinition->SetFieldTypeFilterL( 0 );
	}

// End of File

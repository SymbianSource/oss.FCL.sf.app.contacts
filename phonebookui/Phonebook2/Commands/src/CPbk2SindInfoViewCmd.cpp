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
* Description:  Phonebook 2 SIND InfoView command.
*
*/


// INCLUDE FILES
#include "CPbk2SindInfoViewCmd.h"

// System includes
#include <AiwGenericParam.hrh>
#include <AiwServiceHandler.h>      // AIW
#include <coemain.h>

// Debugging headers
#include <Pbk2Debug.h>


// --------------------------------------------------------------------------
// CPbk2SindInfoViewCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2SindInfoViewCmd* CPbk2SindInfoViewCmd::NewL(        
        TInt aCommandId,
        CAiwServiceHandler& aServiceHandler )
    {
    CPbk2SindInfoViewCmd* self = new( ELeave ) CPbk2SindInfoViewCmd(        
        aCommandId,
        aServiceHandler );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SindInfoViewCmd::CPbk2SindInfoViewCmd
// --------------------------------------------------------------------------
//
CPbk2SindInfoViewCmd::CPbk2SindInfoViewCmd(        
        TInt aCommandId,
        CAiwServiceHandler& aServiceHandler ) :			
	iCommandId( aCommandId ),
    iServiceHandler( aServiceHandler )
	{
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2SindInfoViewCmd::CPbk2SindInfoViewCmd(0x%x)"), this);    
    }

// --------------------------------------------------------------------------
// CPbk2SindInfoViewCmd::~CPbk2SindInfoViewCmd
// --------------------------------------------------------------------------
//
CPbk2SindInfoViewCmd::~CPbk2SindInfoViewCmd()
    {
    delete iPackedLinkArray;
    
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2SindInfoViewCmd::~CPbk2SindInfoViewCmd(0x%x)"), this);
    }

// --------------------------------------------------------------------------
// CPbk2SindInfoViewCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2SindInfoViewCmd::ExecuteLD()
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2SindInfoViewCmd::ExecuteLD(0x%x)"), this);

   	CleanupStack::PushL( this );
   
    // Set parameters for the AIW InfoView
    CAiwGenericParamList& paramList = iServiceHandler.InParamListL();
    
    paramList.AppendL( TAiwGenericParam( EGenericParamContactLinkArray,
                                         TAiwVariant( *iPackedLinkArray ) ) );
                                         
    // Relay the command to AIW for handling
    iServiceHandler.ExecuteMenuCmdL(
        iCommandId,
        paramList,
        iServiceHandler.OutParamListL(),
        0,      // No options used.
        NULL ); // No need for callback
        
	// Destroy itself as promised
    CleanupStack::PopAndDestroy(); // this
    }
    
// --------------------------------------------------------------------------
// CPbk2SindInfoViewCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2SindInfoViewCmd::ResetUiControl
        ( MPbk2ContactUiControl& /*aUiControl*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2SindInfoViewCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2SindInfoViewCmd::AddObserver
        ( MPbk2CommandObserver& /*aObserver*/ )
    {
    // Do nothing
    }


// --------------------------------------------------------------------------
// CPbk2SindInfoViewCmd::SetPackedLinkArray
// --------------------------------------------------------------------------
//
void CPbk2SindInfoViewCmd::SetPackedLinkArray( TDesC8* aPackedLinkArray )
    {
    delete iPackedLinkArray;
    // takes ownership of the object
    iPackedLinkArray = aPackedLinkArray;
    }

//  End of File

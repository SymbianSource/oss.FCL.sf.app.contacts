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
* Description:  Phonebook 2 AIW interest item factory.
*
*/


#include "CPbk2AiwInterestItemFactory.h"

// Phonebook 2
#include "MPbk2AiwInterestItem.h"
#include "CPbk2AiwInterestItemCall.h"
#include "CPbk2AiwInterestItemPoc.h"
#include "CPbk2AiwInterestItemSyncMl.h"
#include "CPbk2AiwInterestItemOviSync.h"
#include "CPbk2AiwInterestItemSindInfoView.h"
#include "Pbk2InternalCommands.hrh"
#include <CPbk2UIExtensionManager.h>
#include <MPbk2UIExtensionFactory.h>

// System includes
#include <AiwCommon.hrh>


// --------------------------------------------------------------------------
// CPbk2AiwInterestItemFactory::CPbk2AiwInterestItemFactory
// --------------------------------------------------------------------------
//
CPbk2AiwInterestItemFactory::CPbk2AiwInterestItemFactory
        ( CAiwServiceHandler& aServiceHandler ) :
        iServiceHandler( aServiceHandler )
    {
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemFactory::~CPbk2AiwInterestItemFactory
// --------------------------------------------------------------------------
//
CPbk2AiwInterestItemFactory::~CPbk2AiwInterestItemFactory()
    {
    Release( iExtensionManager );
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemFactory::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2AiwInterestItemFactory::ConstructL()
    {
    iExtensionManager = CPbk2UIExtensionManager::InstanceL();
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemFactory::NewL
// --------------------------------------------------------------------------
//
CPbk2AiwInterestItemFactory* CPbk2AiwInterestItemFactory::NewL
        ( CAiwServiceHandler& aServiceHandler )
    {
    CPbk2AiwInterestItemFactory* self =
        new ( ELeave ) CPbk2AiwInterestItemFactory( aServiceHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemFactory::CreateInterestItemL
// --------------------------------------------------------------------------
//
MPbk2AiwInterestItem* CPbk2AiwInterestItemFactory::CreateInterestItemL
        ( TInt aInterestId )
    {
    MPbk2AiwInterestItem* interest = iExtensionManager->FactoryL()->
            CreatePbk2AiwInterestForIdL( aInterestId, iServiceHandler );

    if ( !interest )
        {
        switch ( aInterestId )
            {
            case KAiwCmdCall:
                {
                interest = CPbk2AiwInterestItemCall::NewL
                    ( aInterestId, iServiceHandler );
                break;
                }
            case KAiwCmdPoC:
                {
                interest = CPbk2AiwInterestItemPoc::NewL
                    ( aInterestId, iServiceHandler );
                break;
                }
            case KAiwCmdSynchronize:
                {
                interest = CPbk2AiwInterestItemSyncMl::NewL
                    ( aInterestId, iServiceHandler );
                break;
                }
    
            case KAiwCmdInfoView:
                {
                interest = CPbk2AiwInterestItemSindInfoView::NewL
                    ( aInterestId, iServiceHandler );
                break;
                }
            case KOtasAiwCmdSynchronize:
                {
                interest = CPbk2AiwInterestItemOviSync::NewL
                    ( aInterestId, iServiceHandler );
                break;
                }
            default:
                {
                break;
                }
            }
        }

    return interest;
    }

// End of File

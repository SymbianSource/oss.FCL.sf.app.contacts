/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


// INCLUDE FILES
#include "CPbkAppGlobals.h"

#include "CPbkCommandFactory.h"
#include "CPbkAiwInterestItemFactory.h"
#include <CPbkAiwInterestArray.h>
#include <Phonebook.hrh>

#include <BCardEng.h>
#include <sendui.h>
#include <CMessageData.h>

// ================= MEMBER FUNCTIONS =======================

inline CPbkAppGlobals::CPbkAppGlobals()
    {
    }

inline void CPbkAppGlobals::ConstructL()
    {
    iCommandFactory = CPbkCommandFactory::NewL();
    iAiwInterestFactory = CPbkAiwInterestItemFactory::NewL();
    }

CPbkAppGlobals* CPbkAppGlobals::InstanceL()
    {
    // This upcast is safe because in this classes context (Phonebook.app) the
    // app globals object is always of this class type.
    return static_cast<CPbkAppGlobals*>(CPbkAppGlobalsBase::InstanceL());
    }

CPbkAppGlobals* CPbkAppGlobals::NewL()
    {
    CPbkAppGlobals* self = new (ELeave) CPbkAppGlobals;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkAppGlobals::~CPbkAppGlobals()
    {
    delete iAiwInterestFactory;
	delete iCommandFactory;
    delete iSendUi;
    delete iBCardEngine;
    delete iAiwInterestArray;
    }

MPbkCommandFactory& CPbkAppGlobals::CommandFactory()
    {
    return *iCommandFactory;
    }

MPbkAiwInterestItemFactory& CPbkAppGlobals::AiwInterestItemFactory()
    {
    return *iAiwInterestFactory;
    }

CSendUi* CPbkAppGlobals::SendUiL()
    {
    if (!iSendUi)
        {
        iSendUi = CSendUi::NewL();
        }
    return iSendUi;
    }

CBCardEngine& CPbkAppGlobals::BCardEngL(CPbkContactEngine& aContactEngine)
    {
	if (!iBCardEngine)
		{
	    iBCardEngine = CBCardEngine::NewL(&aContactEngine);
		}
	return *iBCardEngine;
    }

CPbkAiwInterestArray* CPbkAppGlobals::AiwInterestArrayL
        (CPbkContactEngine& aContactEngine)
    {
    if (!iAiwInterestArray)
        {
        iAiwInterestArray = CPbkAiwInterestArray::NewL(aContactEngine);
        }
    return iAiwInterestArray;
    }

void CPbkAppGlobals::DoRelease()
    {
    }

// End of File

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
*       Contact editor strategy factory.
*
*/


// INCLUDE FILES
#include "PbkContactEditorStrategyFactory.h"  // Class declarations
#include "CPbkContactEditorNewContact.h"
#include "CPbkContactEditorEditContact.h"

// LOCAL CONSTANTS AND MACROS


// MODULE DATA STRUCTURES


// ==================== LOCAL FUNCTIONS ====================


// ================= MEMBER FUNCTIONS =======================

MPbkContactEditorStrategy* PbkContactEditorStrategyFactory::CreateL
		(CPbkContactEngine& aEngine,
		CPbkContactItem& aContactItem,
        TBool aIsNewContact)
	{
	MPbkContactEditorStrategy* self = NULL;
	if (aIsNewContact)
		{
		self = CPbkContactEditorNewContact::NewL(aEngine, aContactItem);
		}
	else
		{
		self = CPbkContactEditorEditContact::NewL(aEngine, aContactItem);
		}

	return self;
	}

//  End of File  

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
*       Factory to create Phonebook 2 process decorators.
*
*/


#include <Pbk2ProcessDecoratorFactory.h>

// Phonebook 2
#include "CPbk2ProgressDialogDecorator.h"
#include "CPbk2WaitNoteDecorator.h"

EXPORT_C MPbk2ProcessDecorator*
    Pbk2ProcessDecoratorFactory::CreateProgressDialogDecoratorL
        ( TInt aResourceId, TBool aVisibilityDelay )

    {
    return CPbk2ProgressDialogDecorator::NewL
        ( aResourceId, aVisibilityDelay );
    }


EXPORT_C MPbk2ProcessDecorator*
    Pbk2ProcessDecoratorFactory::CreateWaitNoteDecoratorL
        ( TInt aResourceId, TBool aVisibilityDelay )
    {
    return CPbk2WaitNoteDecorator::NewL
        ( aResourceId, aVisibilityDelay );
    }

// End of File

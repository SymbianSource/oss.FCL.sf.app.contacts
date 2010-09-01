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
* Description:  Phonebook 2 process decorator factory.
*
*/


#ifndef PBK2PROCESSDECORATORFACTORY_H
#define PBK2PROCESSDECORATORFACTORY_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MPbk2ProcessDecorator;

/**
 * Phonebook 2 process decorator factory.
 */
class Pbk2ProcessDecoratorFactory
    {
    public: // Factory interface

        /**
         * Creates and returns a progress dialog process decorator.
         *
         * @param aResourceId       Dialog resource id.
         * @param aVisibilityDelay  ETrue if the delay is used,
         *                          EFalse otherwise.
         * @return  A progress dialog process decorator.
         */
        IMPORT_C static MPbk2ProcessDecorator* CreateProgressDialogDecoratorL(
                TInt aResourceId,
                TBool aVisibilityDelay );

        /**
         * Creates and returns a wait note process decorator.
         *
         * @param aResourceId       Dialog resource id.
         * @param aVisibilityDelay  ETrue if the delay is used,
         *                          EFalse otherwise.
         * @return  A wait note process decorator.
         */
        IMPORT_C static MPbk2ProcessDecorator* CreateWaitNoteDecoratorL(
                TInt aResourceId,
                TBool aVisibilityDelay );
    };

#endif // PBK2PROCESSDECORATORFACTORY_H

// End of File

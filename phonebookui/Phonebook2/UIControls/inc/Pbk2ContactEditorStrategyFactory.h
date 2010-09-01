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
* Description:  A factory for creating contact editor strategy
*
*/



#ifndef PBK2CONTACTEDITORSTRATEGYFACTORY_H
#define PBK2CONTACTEDITORSTRATEGYFACTORY_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class TPbk2ContactEditorParams;
class MPbk2ContactEditorStrategy;
class CPbk2PresentationContact;

// CLASS DECLARATION

/**
 *  A factory for creating contact editor strategy.
 *
 */
NONSHARABLE_CLASS(Pbk2ContactEditorStrategyFactory)
    {
    public: // New functions
        
        /**
         * A factory function for creating contact editor strategy.
         *
         * @param aParams   Contact editor parameters.
         * @param aContact  The presentation level contact.
         * @return A the strategy implementation.
         */
        static MPbk2ContactEditorStrategy* CreateL(
            TPbk2ContactEditorParams& aParams,
            CPbk2PresentationContact* aContact);
    };

#endif // PBK2CONTACTEDITORSTRATEGYFACTORY_H
            
// End of File

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
* Description:  
*
*/


#ifndef CPBK2NONTOPCONTACTSELECTOR_H
#define CPBK2NONTOPCONTACTSELECTOR_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkContactSelector.h>

//  FORWARD DECLARATIONS

//  CLASS DECLARATION

/**
 */
class CPbk2NonTopContactSelector : public CBase,
                                   public MVPbkContactSelector
    {
    public:

        static CPbk2NonTopContactSelector* NewL();


        ~CPbk2NonTopContactSelector();

    public: // From MVPbkContactSelector

        TBool IsContactIncluded(
            const MVPbkBaseContact& aContact );
    
    private:
        CPbk2NonTopContactSelector();
        void ConstructL();
    private:
        TBool iMyCardSupported;
    };


#endif // CPBK2NONTOPCONTACTSELECTOR_H

// End of File

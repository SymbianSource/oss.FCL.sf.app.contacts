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


#ifndef CVCARDIDNAMEMAPPING_H
#define CVCARDIDNAMEMAPPING_H

// INCLUDES
#include "vpbkfieldtype.hrh"

// FORWARD DECLARATIONS
class TResourceReader;

// CLASS DECLARATIONS

NONSHARABLE_CLASS( CVPbkVCardEng::CVCardIdNameMapping ): public CBase
	{
	public: 
        static CVCardIdNameMapping* NewLC( TResourceReader& aResReader );
        
        ~CVCardIdNameMapping();

    public: 

        TInt Id() const;

        const TDesC8& Name() const;

    private:
        CVCardIdNameMapping( TResourceReader& aResReader );
        void ConstructL( TResourceReader& aResReader );

    private:
        TInt     iId;
        /// Own: 
        HBufC8*                     iName;
	};
#endif // CVCARDIDNAMEMAPPING_H

// End of file

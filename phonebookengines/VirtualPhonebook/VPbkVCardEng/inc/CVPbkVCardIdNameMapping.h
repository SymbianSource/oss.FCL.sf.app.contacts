/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CVPBKVCARDIDNAMEMAPPING_H
#define CVPBKVCARDIDNAMEMAPPING_H

// INCLUDES
#include "VPbkFieldType.hrh"

// FORWARD DECLARATIONS
class TResourceReader;

// CLASS DECLARATIONS

NONSHARABLE_CLASS(CVPbkVCardIdNameMapping) : public CBase
	{
	public: 
        static CVPbkVCardIdNameMapping* NewLC( TResourceReader& aResReader );
        CVPbkVCardIdNameMapping();
        
        ~CVPbkVCardIdNameMapping();

    public: 

        TInt Id() const;

        const TDesC8& Name() const;

    private:
        CVPbkVCardIdNameMapping( TResourceReader& aResReader );
        void ConstructL( TResourceReader& aResReader );

    private:
        TInt    iId;
        /// Own: 
        HBufC8* iName;
	};
#endif // CVPBKVCARDIDNAMEMAPPING_H

// End of file

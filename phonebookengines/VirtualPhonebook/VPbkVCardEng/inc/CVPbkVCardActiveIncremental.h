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
* Description:  Virtual Phonebook VCard active incremental
*
*/



#ifndef CVPBKVCARDACTIVEINCREMENTAL_H
#define CVPBKVCARDACTIVEINCREMENTAL_H

// INCLUDES
#include <MVPbkContactOperation.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class MVPbkSingleContactOperationObserver;
class MVPbkFieldTypeList;
class CVPbkVCardFieldTypeProperty;
class CVPbkVCardIdNameMapping;
class CVPbkVCardData;

// CLASS DECLARATIONS

NONSHARABLE_CLASS( MVPbkVCardIncremental )
    {
    public:
        virtual TInt Next( TInt& aRemainingSteps ) = 0;
        virtual void CloseL() = 0;
        virtual void HandleLeave( TInt aError ) = 0;
        virtual void HandleCancel() = 0;
    };

NONSHARABLE_CLASS( CVPbkVCardActiveIncremental )
:   public CActive,
    public MVPbkContactOperation
	{
    public:
        CVPbkVCardActiveIncremental(
            TInt aPriority, 
            MVPbkVCardIncremental& aIncremental,
            CVPbkVCardData& aData,
            MVPbkSingleContactOperationObserver& aObserver );
        ~CVPbkVCardActiveIncremental();
        void Start();
        void StartWaiting();

    public: // From MVPbkContactOperation

        virtual void StartL();
        virtual void Cancel();

	protected: // From CActive

        virtual void RunL();
        virtual void DoCancel();
        virtual TInt RunError( TInt aError );

    protected: // New functions
        
        void Complete( TInt aError );

	protected:
        /// Ref: 
        MVPbkVCardIncremental& iIncremental;
        /// Ref:
        CVPbkVCardData& iData;
        /// Ref:
        MVPbkSingleContactOperationObserver& iObserver;
	};

#endif // CVPBKVCARDACTIVEINCREMENTAL_H

// End of file

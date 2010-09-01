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
*     Sub view to a CContactViewBase.
*
*/


#ifndef __CPBKCONTACTFINDVIEWIMPL_H__
#define __CPBKCONTACTFINDVIEWIMPL_H__


// CONSTANTS
enum TPanicCode
    {
    ECntPanicViewNotReady = 1,
    EPanicInvalidIndex,
    EPanicInvalidStateChange,
    EPanicInvalidContactType,
    EPanicPreCond_HandleContactViewEvent,
    EPanicPreCond_HandleItemAdded,
    EPanicPostCond_HandleItemAddedL,
    EPanicPreCond_HandleItemRemoved,
    EPanicPostCond_HandleItemRemoved,
    EPanicPreCond_MakeInitialMatchL,
    EPanicPreCond_RefineFindL,
    EPanicLogic_RefineFindL
    };


// CLASS DECLARATION

/**
 * Interface for CPbkContactFindView states.
 */
NONSHARABLE_CLASS(CPbkContactFindView::MState)
    {
    public: // Interface

        /**
         * Creates initial state.
		 * @param aParent parent view
         */
        static MState* CreateInitialStateL(CPbkContactFindView& aParent);

        /**
         * Destructor. 
         */
        virtual ~MState()
			{
			}

    public:  // From CPbkContactFindView API
        virtual TBool SetFindTextL
            (const TDesC& aFindText, const CContactIdArray* aIncludeAlways) = 0;
        virtual const TDesC& FindText() const = 0;
        virtual TBool ResetFind() = 0;
        virtual TInt IndexOfFirstFindMatchL() const = 0;

    public: // From CContactViewBase API
	    virtual TContactItemId AtL(TInt aIndex) const = 0;
	    virtual const CViewContact& ContactAtL(TInt aIndex) const = 0;
	    virtual TInt CountL() const = 0;
	    virtual TInt FindL(TContactItemId aId) const = 0;
	    virtual HBufC* AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const = 0;
        virtual TAny* CContactViewBase_Reserved_1
            (TFunction aFunction,TAny* aParams) = 0;
    public:  // From MContactViewObserver API
	    virtual void HandleContactViewEvent
            (const CContactViewBase& aView, const TContactViewEvent& aEvent) = 0;
    };


#endif // __CPBKCONTACTFINDVIEWIMPL_H__

// End of File

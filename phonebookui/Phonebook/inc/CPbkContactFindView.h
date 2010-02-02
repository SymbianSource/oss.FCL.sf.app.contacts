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


#ifndef __CPbkContactFindView_H__
#define __CPbkContactFindView_H__

// INCLUDES
#include <cntviewbase.h>
#include "MPbkFindPrimitives.h"


// FORWARD DECLARATIONS
class CPbkContactEngine;
class MPbkContactNameFormat;
/// For testing only
class CPbkContactFindView_TestAccess;


// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported.
 *
 * Specialized Contact Find View for Phonebook. Implements features needed
 * by the Find UI.
 */
class CPbkContactFindView :
        public CContactViewBase, 
        private MContactViewObserver
	{
    public:  // Constructors and destructors
        /**
         * Creates a new instance of this class.
         *
         * @param aEngine   Phonebook engine.
         * @param aBaseView the view this view is based on.
         * @param aObserver observer for this contact view.
         * @param aNameFormat   contact name formatting interface. Used by
         *                      find to filter contacts by name.
         * @param aFindPrimitives   find matching primitives to use
         * @return a new instance of this class.
         */
	    IMPORT_C static CPbkContactFindView* NewL
            (CPbkContactEngine& aEngine,
            CContactViewBase& aBaseView,
            MContactViewObserver& aObserver,
            MPbkFindPrimitives& aFindPrimitives);

    public:  // New functions
        /**
         * Filters this view to contain only entries which match a find text.
         *
         * @param aFindText         text to match.
         * @param aIncludeAlways    contacts to include always in this view
         *                          even if they don't match aFindText.
         * @return ETrue if this view's contents changed.
         */
        IMPORT_C TBool SetFindTextL
            (const TDesC& aFindText,
			const CContactIdArray* aIncludeAlways = NULL);

        /**
         * Returns the current find text.
         */
        IMPORT_C const TDesC& FindText() const;

        /**
         * Stops find filtering of this view.
         * @return ETrue if this view's contents changed.
         */
        IMPORT_C TBool ResetFind();

        /**
         * Returns the index of first contact in this view matching FindText().
         * Returns -1 if there are no matches or FindText() is empty.
         */
        IMPORT_C TInt IndexOfFirstFindMatchL() const;
            
        /**
         * Pointer to a find matching function.
         * @param aTargetText   text to search for aFindText
         * @param aFindText     text to search from aTargetText
         * @return ETrue if aFindText matches aTargetText, EFalse otherwise.
         */
        typedef TBool (*TFindMatcher)(const TDesC& aTargetText,
			const TDesC& aFindText);

    public: // From CContactViewBase.
	    TContactItemId AtL(TInt aIndex) const;
	    const CViewContact& ContactAtL(TInt aIndex) const;
	    TInt CountL() const;
	    TInt FindL(TContactItemId aId) const;
	    HBufC* AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const;
	    TContactViewPreferences ContactViewPreferences();
	    const RContactViewSortOrder& SortOrderL() const;
        TAny* CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams);

    protected:  // Constructors and destructor
	    CPbkContactFindView
            (CPbkContactEngine& aEngine, 
            CContactViewBase& aBaseView, 
            MPbkFindPrimitives& aFindPrimitives);
	    void ConstructL(MContactViewObserver& aObserver);
	    ~CPbkContactFindView();

    private: // From MContactViewObserver.
	    void HandleContactViewEvent
            (const CContactViewBase& aView, const TContactViewEvent& aEvent);

   private:  // Implementation
	    void HandleContactViewEventL(const TContactViewEvent& aEvent);
        class MState;
        friend class MState;
        class CStateBase;
        friend class CStateBase;
        class CNotReadyState;
        friend class CNotReadyState;
        class CReadyState;
        friend class CReadyState;
        class CFindState;
        friend class CFindState;
        inline void SetStateReady();
        inline void SetStateNotReady();
        /// For testing only
        friend class CPbkContactFindView_TestAccess;

    private:  // Data
        /// Own: current state
        MState* iCurrentState;
        /// Ref: the contact view this view is based on
	    CContactViewBase& iBaseView;
        /// Ref: contact name formatter
        MPbkContactNameFormat& iNameFormatter;
        /// Ref: find matching primitives
        MPbkFindPrimitives* iFindPrimitives;
	};


#endif // __CPbkContactFindView_H__

// End of File

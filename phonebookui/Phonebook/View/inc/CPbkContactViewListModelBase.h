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
*    Phonebook contact view listbox model.
*
*/


#ifndef __CPbkContactViewListModelBase_H__
#define __CPbkContactViewListModelBase_H__

//  INCLUDES
#include <e32base.h>    // CBase
#include <bamdesca.h>   // MDesCArray

//  FORWARD DECLARATIONS
class CContactViewBase;
class CViewContact;


// CLASS DECLARATION

/**
 * Base class CContactViewBase-attached listbox models.
 */
NONSHARABLE_CLASS(CPbkContactViewListModelBase) : 
        public CBase, 
        public MDesCArray
    {
    protected:  // Constructors and destructor
        /**
         * Constructor.
         * @param aView contact view to which this listbox model is 
         *        attached
         * @param aBuffer buffer where to format listbox model texts
         */
        CPbkContactViewListModelBase(CContactViewBase& aView, TDes& aBuffer);

        /**
         * Destructor.
         */ 
        ~CPbkContactViewListModelBase();

    public:  // Interface
        /**
         * Returns the group label's field index in iView's sort order.
         * Returns -1 if iView doesn't contain groups.
         */
        TInt GroupLabelFieldIndexL() const;

    public:  // from MDesCArray
        TInt MdcaCount() const;
        TPtrC MdcaPoint(TInt aIndex) const;

    private:  // Implementation interface
        /**
         * Formats aBuffer to contain listbox model text for a contact.
         * @param aViewContact  view's data for the contact.
         * @precond aViewContact.ContactType()==CViewContact::EContactItem
         */
        virtual void FormatBufferForContactL
            (const CViewContact& aViewContact) const = 0;

        /**
         * Formats iBuffer to contain listbox model text for a group.
         * @param aViewContact  view's data for the group.
         * @precond aViewContact.ContactType()==CViewContact::EContactGroup
         */
        virtual void FormatBufferForGroupL
            (const CViewContact& aViewContact) const = 0;

        /**
         * Adds dynamic icon index to iBuffer. Default implementation does
         * nothing.
         * @param aViewContact  view's data for the contact.
         * @precond aViewContact.ContactType()==CViewContact::EContactItem
         */
        virtual void AddDynamicIconL
            (const CViewContact& aViewContact) const;

        /**
         * Formats an empty listbox model text in case of an error.
         * Default implementation makes the buffer empty.
         */
        virtual void FormatEmptyBuffer() const;

        /**
         * Handles any leave occuring in MdcaCount() or MdcaPoint().
         * Default implementation calls
         * CEikonEnv::NotifyIdleErrorWhileRedrawing(aError).
         * @param aError @see CEikonEnv::NotifyIdleErrorWhileRedrawing(aError)
         */
        virtual void HandleError(TInt aError) const;

    private:  // Implementation
        void FormatBufferL(TInt aIndex) const;

    protected:  // data
        /// Ref: contact view this listbox model is attached to
        CContactViewBase& iView;
        /// Ref: buffer where to format listbox texts
        TDes& iBuffer;
        /// Own: previous iView.CountL()
        mutable TInt iPreviousCount;
    };

#endif // __CPbkContactViewListModelBase_H__

// End of File

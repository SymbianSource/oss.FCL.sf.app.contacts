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
* Description:  Event array item class.
*
*/


#ifndef CVPBKEVENTARRAYITEM_H    
#define CVPBKEVENTARRAYITEM_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS    
class MVPbkContactLink;    
    
/**
 * Virtual phonebook event array item class
 */
NONSHARABLE_CLASS( CVPbkEventArrayItem ) : public CBase
    {
    public:
        /// Event types
        enum TViewEventType 
            {
            ERemoved = 1,
            EAdded
            }; 
    
    public:
        /**
         * Creates new instance of this class
         * @param aContactIndex Contact index
         * @param aLink Link to contact
         * @param aEvent Item's event type
         * @return A new instance of this class
         */
        static CVPbkEventArrayItem* NewLC
            ( TInt aContactIndex, 
              const MVPbkContactLink& aLink,
              TViewEventType aEvent );
        
        ~CVPbkEventArrayItem();
        
    public: // interface
        /**
         * Getter for contact link
         * @return a link to the contact
         */
        MVPbkContactLink* Link();
        
        /** 
         * Getter for contact index
         * @return a index of the contact
         */
        TInt Index();
        
        /**
         * Return item's event type
         *
         * return event type
         */
        TInt Event();
        
    private:
        CVPbkEventArrayItem( 
            TInt aContactIndex, 
            TViewEventType aEvent );
        void ConstructL( const MVPbkContactLink& aLink );
        
    private:
        /// Own: contact index
        TInt iContactIndex;
        /// Own: link to contact
        MVPbkContactLink* iLink;
        /// Own: View event type
        TViewEventType iEvent;
        
    };   
    
#endif // CVPBKEVENTARRAYITEM_H    

// End of file

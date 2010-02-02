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
* Description:  A bookmark for folding contact
*
*/



#ifndef TVPBKFOLDINGCONTACTBOOKMARK_H
#define TVPBKFOLDINGCONTACTBOOKMARK_H

//  INCLUDES
#include <MVPbkContactBookmark.h>

// FORWARD DECLARATIONS
class CVPbkFoldingViewContact;

// CLASS DECLARATION

/**
*  A bookmark for folding contact
*
*/
NONSHARABLE_CLASS(TVPbkFoldingContactBookmark) : public MVPbkContactBookmark
    {
    public:  // Constructors and destructor
        
		/**
        * C++ constructor.
        * @param aContact contact which bookmark is needed
        */
        inline TVPbkFoldingContactBookmark(const CVPbkFoldingViewContact& aContact);
        
    public: // New functions
        
        /**
        * Returns the reference to the folding contact.
        * @return the reference to the folding contact.
        */
        inline const CVPbkFoldingViewContact& Contact() const;

    private:    // Data
        const CVPbkFoldingViewContact& iContact;
    };

inline TVPbkFoldingContactBookmark::TVPbkFoldingContactBookmark(
        const CVPbkFoldingViewContact& aContact) :   
        iContact(aContact) 
    {
    }
                
inline const CVPbkFoldingViewContact& TVPbkFoldingContactBookmark::Contact() const
    {
    return iContact;
    }
    
#endif      // TVPBKFOLDINGCONTACTBOOKMARK_H
            
// End of File

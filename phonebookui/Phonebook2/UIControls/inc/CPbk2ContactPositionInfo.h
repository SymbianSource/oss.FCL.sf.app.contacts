/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Range that is allocated for a plug-in.
*
*/


#ifndef PBK2_CONTACT_POSITION_INFO_H
#define PBK2_CONTACT_POSITION_INFO_H


// INCLUDES
#include <e32std.h>
#include <MVPbkContactLink.h>

// FORWARD DECLARATIONS


/**
 * stores contact referred link and it's position in view
 */
class CPbk2ContactPositionInfo : public CBase
    { 

    public:
        static inline CPbk2ContactPositionInfo* NewLC( MVPbkContactLink* aContactLink, TInt aPos )
            {
            CPbk2ContactPositionInfo* self =
                new ( ELeave ) CPbk2ContactPositionInfo( aContactLink, aPos );
            CleanupStack::PushL( self );
            return self;
            }
        
        static TInt CompareByPosition( const CPbk2ContactPositionInfo& aFirst, 
                        const CPbk2ContactPositionInfo& aSecond );
     
    public:
        inline ~CPbk2ContactPositionInfo()
    	  	{
    	    delete iContactLink;
    	  	}
        
        /*
         * @return pointer of MVPbkContactLink, ownership is transferred
         */
        inline MVPbkContactLink* ContactLink()
            {
            MVPbkContactLink* link = iContactLink;
            iContactLink = NULL;
            return link;
            }
        inline TInt Position()
            {
            return iPos;
            }
    private: // Construction
       
        inline CPbk2ContactPositionInfo( MVPbkContactLink* aContactLink, TInt aPos ) :
            iContactLink( aContactLink ),
            iPos( aPos )
            {
            }
      
    private: // Data
        
        // contact referred link
        MVPbkContactLink* iContactLink;
        
        // the position of contact in the view
        TInt iPos; 
        
    };
    
#endif // PBK2_TOPCONTACT_ORDER_INFO_H

//End of file

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


#ifndef MPBK2ADDFAVORITESVISIBILITY_H_
#define MPBK2ADDFAVORITESVISIBILITY_H_

namespace
    {
    enum TVariationFlag
        {
	    EVisibilityUndefined = -1,
	    EVisibilityAlwaysOFF, // 0
	    EVisibilityUntilFirstFavorite, // 1
	    EVisibilityAlwaysON	           // 2 
	    };    
    }

// forward declarations
class MPbk2CmdItemVisibilityObserver;


class MPbk2AddFavoritesVisibilityImpl
    {
    public:
    
        /* Implements decesion logic on whether 
           "Add to Top Contacts" promotion is visible or not
           
           Callbacks to client should be implemented in ASYNC
           manner, as this impl may run during client's construction etc
           
           @param aTopCount: Number of Top Contacts
           @param aContactCount: Number of Top/NonTop contacts
        */   
		virtual void NotifyVisibilityChange(const TInt aTopCount, const TInt aContactCount) = 0;

        /* @return a reference to TVariationFlag */
        virtual TVariationFlag Variation() = 0;    
    
        /*
        * @param aObserver: (i.e. nameslistcontrol's ready state obj)
        * obj who is interested in visibility on/off events
        */        
        virtual void SetVisibilityObserver(MPbk2CmdItemVisibilityObserver* aObserver) = 0;
    
            
    };


#endif /* MPBK2ADDFAVORITESVISIBILITY_H_*/

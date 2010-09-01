/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef CMSNOTIFYEVENT_H_
#define CMSNOTIFYEVENT_H_

#include <e32base.h>

enum TCmsNotifyType
{
        EcmsNotifyPresence,
        ECmsNotifyPbk
};

class CCmsServerContact;

class CCmsNotifyEvent : public CBase
    {
    public:
        
        static CCmsNotifyEvent* NewL( const TDesC& aMessage,
                                      CCmsServerContact& aContact );
        
        static CCmsNotifyEvent* NewL( const TDesC8& aMessage,
                                      CCmsServerContact& aContact );
        
        TInt Complete( TInt aMessageSlot, const RMessage2& aMessage ) const;
        
        virtual ~CCmsNotifyEvent();
        
    private:
        
        CCmsNotifyEvent( CCmsServerContact& aContact );
        
        void ConstructL( const TDesC& aMessage );
        
        void ConstructL( const TDesC8& aMessage );
        
        TInt Complete8BitL( TInt aMessageSlot,
                            TInt aClientDesLength,
                            const RMessage2& aMessage ) const;
    
        TInt Complete16BitL( TInt aMessageSlot,
                             TInt aClientDesLength,
                             const RMessage2& aMessage ) const;
        
    private:  // Data
        
        TBool                               iSent; 
        HBufC*                              iData16;
        HBufC8*                             iData8;
        CCmsServerContact&                  iContact;
    };

#endif /*CMSNOTIFYEVENT_H_*/

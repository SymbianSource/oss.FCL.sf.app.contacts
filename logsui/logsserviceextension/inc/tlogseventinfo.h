/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Encapsulates log event information
*
*/



#ifndef T_TLOGSEVENTINFO_H
#define T_TLOGSEVENTINFO_H


#include <logwrap.h>


/**
 *  Encapsulates log event information
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(TLogsEventInfo) 
    {

public:

    /**
     * Constructor
     */
    TLogsEventInfo();
    
    
    /**
     * Constructor
     *
     * @since S60 v3.2
     * @param aServiceId a service id
     */
    TLogsEventInfo( TUint32 aServiceId );
    

    /**
     * Sets the branding icon index of this event.
     *
     * @since S60 v3.2
     * @param aBrandingIconIndex The branding icon index of this event. 
     * The index is the absolute position of the branding icon in the icon array.
     */
    void SetBrandingIconIndex( TInt aBrandingIconIndex );


    /**
     * Gets the Service Id
     *
     * @since S60 v3.2
     * @return The Service Id of this event
     */
    TUint32 ServiceId();


    /**
     * Gets the branding icon index
     *
     * @since S60 v3.2
     * @return The branding icon index of this event
     */
    TInt BrandingIconIndex();


private: // data

    /**
     * Service Id
     */
    TUint32 iServiceId;

    /**
     * Index of branding icon of the service 'iServiceId' in the 
     * icon array.
     */
    TInt iBrandingIconIndex;

    };


#endif // T_TLOGSEVENTINFO_H

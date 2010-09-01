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
* Description:  Presence in pbonebook
*
*/


#ifndef CONTACTPRESENCE_H
#define CONTACTPRESENCE_H

#include <e32std.h>
#include <badesca.h>

#include <mcontactpresence.h>

class MContactPresenceObs;
class CPresenceIconInfo;
class CPresenceBrandedIcon;
class CBSFactory;
class CVPbkContactManager;

class CPresenceIconInfoListener;

/**
 * CContactPresence
 *
 * Presence in phonebook
 *
 * @lib ececontactpresence
 * @since s60 v5.0
 */
NONSHARABLE_CLASS( CContactPresence ) : public CBase, public MContactPresence
    {
public:

    /**
     * Constructor.
     * @param aObs callback observer
     */
    static CContactPresence* NewL(
        MContactPresenceObs& aObs );

    /**
     * Constructor.
     * @param aObs callback observer
     * @param aContactManager contact manager with open store
     */
    static CContactPresence* NewL(
        CVPbkContactManager& aContactManager,
        MContactPresenceObs& aObs );

    virtual ~CContactPresence();

public:


// From base class MContactPresence

    /**
     * Defined in a base class
     */
    void Close();

    /**
     * Defined in a base class
     */
    void SubscribePresenceInfoL(
        const MDesC8Array& aLinkArray );

    /**
     * Defined in a base class
     */
    TInt GetPresenceIconFileL(
        const TDesC8& aBrandId,
        const TDesC8& aElementId);

    /**
     * Defined in a base class
     */    
    TInt GetPresenceInfoL( const TDesC8& aPackedLink );      

    /**
     * Defined in a base class
     */
    void SetPresenceIconSize(
        const TSize aSize  );

    /**
     * Defined in a base class
     */
    void CancelSubscribePresenceInfo( const MDesC8Array& aLinkArray );

    /**
     * Defined in a base class
     */
    void CancelOperation( TInt aOpId );

    /**
     * Defined in a base class
     */
    void CancelAll();

#ifdef _DEBUG
    static void WriteToLog( TRefByValue<const TDesC8> aFmt,... );
#endif


protected:


private:

    CContactPresence( MContactPresenceObs& aObs );

    /**
     * 2 pahe constructor
     * @param aManager contact manager, may be NULL
     */
    void ConstructL( CVPbkContactManager* aManager );

    /**
     * Search branded icon handler
     * @return handler or NULL
     */
    CPresenceBrandedIcon* SearchBrandedIcon( TInt aOpId );

    /**
     * Delete all handler queues
     */
    void DeleteAllRequests();


private: // Data

    /**
     * callback observer
     */
    MContactPresenceObs& iObs;

    /**
     * latest operation id
     */
    TInt iOpId;


    /**
     * Queue for icon requests
     */
    TDblQue<CPresenceBrandedIcon> iIconReqList;

    /**
     * Branding factory not owned
     */
    CBSFactory* iBrandingFactory;

    /**
     * size of the bitmap
     */
    TSize iSize;

    /**
     * OWN
     */
    CPresenceIconInfoListener* iListener;

    };


#endif // CONTACTPRESENCE_H

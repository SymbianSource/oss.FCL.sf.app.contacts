/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef CPBK2ICONCUSTOMAPITESTPLUGIN_H
#define CPBK2ICONCUSTOMAPITESTPLUGIN_H

// INCLUDES
#include <CContactCustomIconPluginBase.h>

// FORWARD DECLARATIONS
class CCustomIconDecisionData;
class CCustomIconIdMap;
class CPeriodic;

// CONSTANTS

// Implementation UID of this plug-in.
const TInt KCPbk2IconCustomApiTestPluginImplValue = 0x10208413;
const TUid KCPbk2IconCustomApiTestPluginImpl =
    { KCPbk2IconCustomApiTestPluginImplValue };


// CLASS DECLARATION
/**
*  This is example implementation of UI customization plugin.
*/
class CPbk2IconCustomApiTestPlugin :
    public CContactCustomIconPluginBase
    {

    public:
        /**
        * Two-phased constructor.
        */
        static CPbk2IconCustomApiTestPlugin* NewL();
        ~CPbk2IconCustomApiTestPlugin();

    public: // API implementation from base class

        void AddObserverL( MCustomIconChangeObserver* aObserver );

        void RemoveObserver( MCustomIconChangeObserver* aObserver );

        CCustomIconIdMap* IconsL( TUid aContext, TIconSizeHint aSizeHint );

        TBool IconChoiceL( const CCustomIconDecisionData& aHint,
            TCustomIconId& aIdResult );

        CGulIcon* IconL( const CCustomIconDecisionData& aHint,
            TIconSizeHint aSizeHint );

        CGulIcon* IconL( TCustomIconId aIconId, TIconSizeHint aSizeHint );

        CGulIcon* IconL( TUid aContext,
            TCustomIconId aIconId,
            TIconSizeHint aSizeHint );

    private: // Implementation

        static TInt Tick( TAny* aObject );
        void NotifyObserversL();

    private: // Construction and destruction

        /**
        * C++ default constructor.
        */
        CPbk2IconCustomApiTestPlugin();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // data

        TInt64 iSeed;
        RArray<MCustomIconChangeObserver*> iObservers;
        CPeriodic* iTimer;
    };


#endif // CPBK2ICONCUSTOMAPITESTPLUGIN_H

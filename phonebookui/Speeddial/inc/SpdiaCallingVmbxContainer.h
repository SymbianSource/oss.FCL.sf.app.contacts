/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     Declares container control for Calling Vmbx view.
*
*/






#ifndef CSPDIACALLINGVMBXCONTAINER_H
#define CSPDIACALLINGVMBXCONTAINER_H

// INCLUDES
#include <eiklbo.h>
#include <aknview.h>
#include <AknUtils.h>

// FORWARD DECLARATIONS
class CSpeedDialPrivate;
class CSpdiaCallingVmbxView;
class CVPbkContactManager;

// CLASS DECLARATION
/**
*  Calling vmbx container class.
*/
class CSpdiaCallingVmbxContainer : public CCoeControl
    {
    public: // Constructors and destructor

        /**
        * Symbian OS two-phased constructor.
        */
        static CSpdiaCallingVmbxContainer* NewL();

        /**
        * Symbian OS two-phased constructor.
        */
        static CSpdiaCallingVmbxContainer* NewLC();

        /**
        * C++ default constructor.
        */
        CSpdiaCallingVmbxContainer();
        
        /**
        * Virtual C++ destructor.
        */
        virtual ~CSpdiaCallingVmbxContainer();


    private: // Functions from base classes
        /**
        * Symbian second phase Constructor.
        * @param aView      Frame pointer.
        */
        void ConstructL();
        /**
        * From CoeControl,SizeChanged.
        */
        void SizeChanged();

        /**
        * From CoeControl,CountComponentControls.
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl,ComponentControl.
        */
        CCoeControl* ComponentControl( TInt /*aIndex*/ ) const;

    private: //data
        CSpdiaCallingVmbxView* iView;
        CVPbkContactManager* iContactManager;
    public:
        CSpeedDialPrivate*          iControl;

    };

#endif        // CSpdiaCallingVmbxContainer_H

// End of File

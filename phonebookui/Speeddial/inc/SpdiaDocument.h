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
* Description:     Declares document for application
*
*/






#ifndef SPDIADOCUMENT_H
#define SPDIADOCUMENT_H

// INCLUDES
#include <AknDoc.h>

// FORWARD DECLARATIONS
class   CEikAppUi;
class   CPbkContactEngine;

// CLASS DECLARATION

/**
*  CSpdiaDocument
*  Speeddial document class. Required by the Symbian application framework.
*
*  @since 
*/
class CSpdiaDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CSpdiaDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CSpdiaDocument();

    public: // New functions
        
    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:
        /**
        * Symbian default constructor.
        */
        CSpdiaDocument(CEikApplication& aApp);
        void ConstructL();

    private:
        /**
        * From CEikDocument, create CSpeedDialAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();

    };

#endif        // SPDIADOCUMENT_H

// End of File


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
* Description: Declares document class for application.
*
*/

#ifndef __NEWCONTACTLAUNCHERDOCUMENT_h__
#define __NEWCONTACTLAUNCHERDOCUMENT_h__

// INCLUDES
#include <AknDoc.h>
#include <apgwgnam.h>
// FORWARD DECLARATIONS
class CNewContactLauncherAppUi;
class CEikApplication;

// CLASS DECLARATION

/**
 * CNewContactLauncherDocument application class.
 * An instance of class CNewContactLauncherDocument is the Document part of the
 * AVKON application framework for the NewContactLauncher example application.
 */
class CNewContactLauncherDocument : public CAknDocument
    {
public:
    // Constructors and destructor

    /**
     * NewL.
     * Two-phased constructor.
     * Construct a CNewContactLauncherDocument for the AVKON application aApp
     * using two phase construction, and return a pointer
     * to the created object.
     * @param aApp Application creating this document.
     * @return A pointer to the created instance of CNewContactLauncherDocument.
     */
    static CNewContactLauncherDocument* NewL(CEikApplication& aApp);

    /**
     * NewLC.
     * Two-phased constructor.
     * Construct a CNewContactLauncherDocument for the AVKON application aApp
     * using two phase construction, and return a pointer
     * to the created object.
     * @param aApp Application creating this document.
     * @return A pointer to the created instance of CNewContactLauncherDocument.
     */
    static CNewContactLauncherDocument* NewLC(CEikApplication& aApp);

    /**
     * ~CNewContactLauncherDocument
     * Virtual Destructor.
     */
    virtual ~CNewContactLauncherDocument();

public:
    // Functions from base classes

    /**
     * CreateAppUiL
     * From CEikDocument, CreateAppUiL.
     * Create a CNewContactLauncherAppUi object and return a pointer to it.
     * The object returned is owned by the Uikon framework.
     * @return Pointer to created instance of AppUi.
     */
    CEikAppUi* CreateAppUiL();
public: // new methods

     /**
      * Updates the task in fast swap windows
      *
      * @param aWgName the window group name
      */
     void UpdateTaskNameL( CApaWindowGroupName* aWgName );


private:
    // Constructors

    /**
     * ConstructL
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * CNewContactLauncherDocument.
     * C++ default constructor.
     * @param aApp Application creating this document.
     */
    CNewContactLauncherDocument(CEikApplication& aApp);

    };

#endif // __NEWCONTACTLAUNCHERDOCUMENT_h__
// End of File

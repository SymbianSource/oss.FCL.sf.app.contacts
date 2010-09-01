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
* Description:  An instance of CCCAAppApplication is the 
*                application part of the AVKON application framework
*
*/


#ifndef __CCAAPPLICATION_H__
#define __CCAAPPLICATION_H__

#include <aknapp.h>

/**
 *  An instance of CCCAAppApplication is the application part 
 *  of the AVKON application framework for the CCAApp 
 *  application
 *
 *  @lib ccaapp.exe
 *  @since S60 v5.0
 */  
class CCCAAppApplication : public CAknApplication
    {

public:

// from base class CApaApplication

    /**
     * From CApaApplication
     * (see details from header)
     *
     * @since S60 v5.0
     */
    TUid AppDllUid() const;

protected:

// from base class CApaApplication

    /**
     * From CApaApplication
     * (see details from header)
     *
     * @since S60 v5.0
     */
	void NewAppServerL(CApaAppServer*& aAppServer);

// from base class CEikApplication

    /**
     * From CEikApplication
     * (see details from header)
     *
     * @since S60 v5.0
     */
    CApaDocument* CreateDocumentL();

    };

#endif // __CCAAPPLICATION_H__

//End Of File

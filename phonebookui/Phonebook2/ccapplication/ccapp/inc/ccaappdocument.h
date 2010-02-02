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
* Description:  An instance of class CCCAAppDocument is the 
*                Document part of the AVKON application 
*                framework for the CCAApp 
*
*/


#ifndef __CCAAPPDOCUMENT_H__
#define __CCAAPPDOCUMENT_H__

#include <AknDoc.h>

class CCCAAppAppUi;
class CEikApplication;
 
/**
 *  An instance of class CCCAAppDocument is the Document part of 
 *  the AVKON application framework for the CCAApp
 *
 *  @lib ccaapp.exe
 *  @since S60 v5.0
 */   
class CCCAAppDocument : public CAknDocument
    {

public:

    static CCCAAppDocument* NewL( CEikApplication& aApp );
    static CCCAAppDocument* NewLC( CEikApplication& aApp );

    /**
    * Destructor.
    */
    ~CCCAAppDocument();

private: 

// from base class CEikDocument

    /**
     * From CEikDocument
     * (see details from header)
     *
     * @since S60 v5.0
     */
    CEikAppUi* CreateAppUiL();

private:

    CCCAAppDocument( CEikApplication& aApp );
    void ConstructL();

    };

#endif // __CCAAPPDOCUMENT_H__

//End Of File

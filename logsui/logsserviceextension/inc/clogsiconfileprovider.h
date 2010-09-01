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
* Description:  Icon file provider.
*
*/



#ifndef C_CLOGSICONFILEPROVIDER_H
#define C_CLOGSICONFILEPROVIDER_H

#include <e32base.h>
#include <AknsItemID.h>

/**
 *  Icon file provider.
 *  See MAknIconFileProvider.h for further information.
 *
 * @since S60 v3.2
 */
NONSHARABLE_CLASS(CLogsIconFileProvider) : public CBase, 
                                           public MAknIconFileProvider
    {

public:

    /**
     * Two-phased constructor.
     * @param aFile     
     */
    static CLogsIconFileProvider* NewL( RFile& aFile );
    
    /**
     * Destructor.
     */
    virtual ~CLogsIconFileProvider();

public: 

// from base class MAknIconFileProvider

    /**
     * From MAknIconFileProvider
     *
     * @see MAknIconFileProvider
     */
    void RetrieveIconFileHandleL(
        RFile& aFile, 
        const TIconFileType aType );

    /**
     * From MAknIconFileProvider
     */
    void Finished();

private:

    /**
     * C++ default constructor.
     *
     * @see MAknIconFileProvider
     */
    CLogsIconFileProvider();
      
    /**
     * Symbian second phase constructor
     */
    void ConstructL( RFile& aFile );

private: // data

    /**
     * Handle to the icon file
     * Own.
     */
    RFile iFile;

    };

#endif // C_CLOGSICONFILEPROVIDER_H

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
* Description:  Server-side service implementation
*
*/

#ifndef CCAAPPSERVICE_H
#define CCAAPPSERVICE_H

#include <f32file.h>
#include <eikdoc.h>
#include <AknServerApp.h>
#include "ccaappappui.h"

// for logging purposes
#define KCCAppLogFile CCA_L("ccaapp.txt")

 /**
 *  Server-side message editor/viewer service implementations
 *
 *  @lib ccaapp.exe
 *  @since S60 v5.0
 */
class CCAAppService : public CAknAppServiceBase
	{

public:

	/**
	* Constructor
	*/
	CCAAppService();

	/**
	* Destructor
	*/
	~CCAAppService();

private:

    /**
     * Reads message and transfers commands to app UI class
     *
     * @since S60 v5.0
     * @param aMessage contains CCCAParameter
     */
    void ReadMsgL( const RMessage2& aMessage );

protected:

// from base class CAknAppServiceBase

    /**
     * From CAknAppServiceBase
     * (see details from header)
     *
     * @since S60 v5.0
     */
	void CreateL();

    /**
     * From CAknAppServiceBase
     * (see details from header)
     *
     * @since S60 v5.0
     */
     void ServiceL(const RMessage2& aMessage);

    /**
     * From CAknAppServiceBase
     * (see details from header)
     *
     * @since S60 v5.0
     */
     void ServiceError(const RMessage2& aMessage,TInt aError);

private:

    /**
     * Pointer to app UI class
     * Not own.
     */
    CCCAAppAppUi* iAppUi;

	};

#endif //CCAAPPSERVICE_H


// End of file

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
* Description:  Class for handling connections over application server.
*
*/

#ifndef R_CCACLIENTUISERVICE_H
#define R_CCACLIENTUISERVICE_H

class RAknAppServiceBase; 

/**
 *  RCCAClientUIService
 *
 *  For handling IPC traffic over the application server.
 *  @code
 *  RCCAClientUIService iAppService;
 *  iAppService.DoConnectL();
 *  iAppService.DoLaunchL(iStatus, iMessagePtr);
 *  @endcode
 *
 *  @lib ccaclient
 *  @since S60 v5.0
 */
class RCCAClientUIService : public RAknAppServiceBase
    {
public:

    RCCAClientUIService();

    /**
     * Launches application service.
     *
     * @since S60 v5.0
     * @param aStatus                   Request status.
     * @param aMessagePtr               Connection parameters.
     */        
    void DoLaunch(
            TRequestStatus& aStatus,
            TPtr8& aMessagePtr );
    
    /**
     * Closes application service.
     *
     * @since S60 v5.0
     */        	
    void DoCloseApplication();
        
    /**
     * Connects to application server.
     * @since S60 v5.0
     */        
    void DoConnectL();
        
private: 
    
// from base class RAknAppServiceBase

    /**
     * From RAknAppServiceBase.
     * (see baseclass for details)
     *
     * @since S60 5.0
     */
    TUid ServiceUid() const;
    
private:
    
    /**
     * Gets launched applications window group id.
     */
    TInt GetAppWindowGroupId();
        
    };
    
#endif // R_CCACLIENTUISERVICE_H
// End of File

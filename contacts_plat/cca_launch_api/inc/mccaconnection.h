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
* Description:  This is a class for launching CCA application. 
 *
*/


#ifndef M_MCCACONNECTION_H
#define M_MCCACONNECTION_H

#include <e32std.h>

class MCCAParameter;

/** @file mccaconnection.h
 *
 *  MCCAObserver is used to get notifications from CCA application.
 *
 *  @lib ccaclient
 *  @since S60 v5.0
 */
class MCCAObserver
    {
public:

    enum TNotifyType
        {
        /* EExitEvent is used to indicate that the CCA application has been
         * closed. Accompanied parameter aReason will either be an error code 
         * or the command id that was used to close the CCA application.
         */
        EExitEvent
        };

    /**
     * This callback is to get simple notifications from CCA application.
     *
     * @since S60 5.0
     * @param aType describes the notify type (see TNotifyType)
     * @param aReason describes the reason (see events in TNotifyType)
     */
    virtual void CCASimpleNotifyL( TNotifyType aType, TInt aReason ) = 0;

    /**
     * Returns an extension point for this interface or NULL.
     * @param aExtensionUid Uid of extension
     * @return Extension point or NULL
     */
    virtual TAny* CcaObserverExtension( 
            TUid /*aExtensionUid*/ ) { return NULL; }

protected:  // Destructor
    /**
     * Destructor.
     */
    virtual ~MCCAObserver() { }

    };

/** @file mccaconnection.h
 *  MCCAConnection is used to start a actual launching of CCA application over the AppServer framwork. 
 *  Before starting the launching, the parameter class have to be instantiated and set to ready 
 *  with the proper contact data. 
 *  @see MCCAParameter
 *
 *  Example usage of the API:  
 *  @code
 *  Header file:
 *   
 *  // Launching can be done without MCCAObserver-interface,
 *  // then the iConnection can be closed in destructor etc when
 *  // CCApplication is already closed
 *  class CMyClass : public CBase, public MCCAObserver
 *  {
 *  ...
 *  MCCAConnection* iConnection;
 *  };
 *
 *  
 *  Source file:
 *  ...
 *  // Launch the CCApplication  
 *  MCCAParameter* parameter = TCCAFactory::NewParameterL();
 *  CleanupClosePushL( *parameter );
 *  // prepare parameter here (see details from the header)
 *  if ( !iConnection )
 *      {// get connection
 *      iConnection = TCCAFactory::NewConnectionL();
 *      }
 *  // note; there can be only one CCApplication instance
 *  // running simultaneously per iConnection
 *  iConnection->LaunchAppL( *parameter );
 *  OR
 *  iConnection->LaunchAppL( *parameter, this );
 *  CleanupStack::Pop();// parameter is taken care by iConnection
 *  ...
 *  ~CMyClass::CMyClass()
 *  {
 *  ...
 *  if ( iConnection )
 *      {// close connection
 *      iConnection->Close();
 *      }
 *  } 
 *  ...
 *  // If MCCAObserver-interface is used, it could be implemented like this
 *  void CMyClass::CCASimpleNotifyL( TNotifyType aType, TInt aReason )
 *  {
 *  // check the condition of aReason here if needed
 *  if ( MCCAObserver::EExitEvent == aType )
 *      {
 *      // Calling Close() for iConnection will close the running 
 *      // CCApplication, so be careful when using it
 *      if ( iConnection )
 *          {
 *          iConnection->Close();
 *          iConnection = NULL;
 *          }
 *      }
 *  }
 *  ...
 *
 *  @endcode
 *
 *  @lib ccaclient
 *  @since S60 v5.0
 */
class MCCAConnection
    {

public:

    /**
     * Destroy this entity
     *
     * @since S60 5.0
     */
    virtual void Close() = 0;

    /**
     * DEPRECATED - USE LaunchAppL() instead!
     * Ownership of MCCAParameter is NOT transferred.
     */
    virtual void LaunchApplicationL( MCCAParameter& aParameter ) = 0;

    /**
     * Launches CCA application with the given parameter.
     * See above for example instructions.
     *
     * Ownership of MCCAParameter is transferred.
     *
     * Leaves KErrArgument if there is no contact data in the
     *        given parameter. 
     * Leaves KErrAlreadyExists if there is already CCApplication
     * running simultaneously.
     * 
     * @see MCCAParameter::SetContactDataL      
     * @since S60 5.0
     * @param aParameter to launch application
     * @param aObserver to get notifications (see MCCAObserver)
     */    
    virtual void LaunchAppL( 
        MCCAParameter& aParameter, 
        MCCAObserver* aObserver = NULL ) = 0;
    };

#endif //M_MCCACONNECTION_H
// End of File

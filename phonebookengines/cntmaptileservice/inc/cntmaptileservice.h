/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*     Contact maptile service interface
*
*/

#ifndef _CNTMAPTILESERVICE_H_
#define _CNTMAPTILESERVICE_H_


#include <QString>
#include <QtGlobal>


#ifdef  CNTMAPTILESERVICEDLL
#define CNTMAPTILESERVICE_EXPORT Q_DECL_EXPORT
#else
#define CNTMAPTILESERVICE_EXPORT Q_DECL_IMPORT
#endif



// CLASS DECLARATION

/**
*  Maptile service class,  provides interface to get map tile image associated with 
*  contact. Also provides interface to check whether location feature is enabled or disabled.
*
*  Note: Location feature can be enabled or disabled by modifying conf\cntmaptileservice.confml file.
*/
class CNTMAPTILESERVICE_EXPORT CntMapTileService 
{

public:

    /** 
     * Contact address types
     */
    enum ContactAddressType      
    {
        /** Address Type Pref */
        AddressPreference,
        /** Address type Work */
        AddressWork,
        /** Address type Home */
        AddressHome
    };
      
   /**
    * Checks whether location feature enabled or disabled.
    * 
    * @return Returns true or false based on location feature setting.
    */
    static bool isLocationFeatureEnabled();
            
   /**
    * Gets a maptile image associated with a contact id. Returns a maptile 
    * image path if it is available otherwise returns NULL.
    * 
    * @param contactId  Contact id     
    * @param sourceType Source address type( Preferred, Home , Work address )
    *      
    * @return Returns maptile image path if it is available, else NULL.
    */
    static QString getMapTileImage( int contactId, ContactAddressType sourceType  );  
     	
};

#endif //_CNTMAPTILESERVICE_H_


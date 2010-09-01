/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Service information holder
*
*/



#ifndef C_BPASSERVICEINFO_H
#define C_BPASSERVICEINFO_H


#include <spdefinitions.h>


/**
 * bpas service info class. It should be only created if presence info is available.
 *
 *  @lib bpas.lib
 *  @since S60 v5.0
 */
class CBPASServiceInfo: public CBase
    {

public: 

    /**
     * Symbian constructors. Not exported since only used inside bpas
     *
     * @param aServiceId service id for this service
     * @return CBPASServiceInfo* newly created object
     */
     static CBPASServiceInfo* NewL(TServiceId aServiceId);
     
     static CBPASServiceInfo* NewLC(TServiceId aServiceId);

    /**
     * Public destructor. 
     */
     virtual ~CBPASServiceInfo();
    

    /**
     * Get service id from service info
     *
     * @return service id
     */    
     IMPORT_C TServiceId ServiceId() const;
    
    
    /**
     * Get brand id from service info
     *
     * @return brand id
     */
     IMPORT_C const TPtrC8 BrandId( ) const;
        
     /**
     * Get language id from service info
     *
     * @return language id
     */
     IMPORT_C TInt LanguageId( ) const;
    
      
    /**
     * Get service type from service info
     *
     * @return service type
     */
     IMPORT_C TInt ServiceType( ) const;

public:

     void SetLanguageId(TInt aLanguageId);
     void SetBrandIdL( const TDesC8& aBrandId );
     void SetServiceType(TInt aServiceType);
          
     
private:
 
    CBPASServiceInfo(TServiceId aServiceId);
    void ConstructL ();
    
private:
    
    TServiceId	iServiceId;//mandatory
    HBufC8*     iBrandId;//optional
    TInt        iLanguageId;//optional -1
    TInt        iServiceType;// optional, 0


    };


#endif // C_BPASSERVICEINFO_H

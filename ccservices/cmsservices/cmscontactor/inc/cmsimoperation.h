/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IM operation.
 *
*/


#ifndef C_CMSIMOPERATION_H
#define C_CMSIMOPERATION_H

#include <e32base.h>

#include "cmsoperationhandlerbase.h"
#include "cmscontactfield.h"

class CCmsContactorImPluginBase;
class CSPSettings;


/**
 *  CCMSIMOperation
 *  Call operation. 
 *
 *  @code
 *   
 *  @endcode
 *
 *  @since S60 v5.0
 */
class CCMSIMOperation : public CCMSOperationHandlerBase
    {

public:

    /**
     * Two-phased constructor.
     */
    static CCMSIMOperation* NewL(
        const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
        const RPointerArray<CCmsContactField>& aItemArray,
        const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag );
	/**
	 * Destructor.
	 */
	virtual ~CCMSIMOperation();                    

public:

    // from base class CCMSOperationHandlerBase

    /**
     * Operation for starting executing the operation.
     *
     * @since S60 v5.0
     */
    void ExecuteLD();

private:

    // from base class CCMSOperationHandlerBase
    
	/**
	 * Initiaties operation.
	 *
	 * @since S60 v5.0
	 * @param aContactFieldItem: Selected field item to complete operation.
	 */
	void StartOperationL( const CCmsContactFieldItem& aContactFieldItem);    

private:

    void ConstructL();
    
    CCMSIMOperation(
        const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
		const RPointerArray<CCmsContactField>& aItemArray,
		const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag );
              
    /**
     * Resolve IM Launcher ECom UID
     * 
     * @since S60 v5.0     
     * @param aServiceId service id 
     * @return ECom UID
     */ 
    TUid ResolveEcomUidL( const TDesC& aServiceId );     
    
    /**
     * Resolve IM Launcher ECom UID
     * 
     * @since S60 v5.0     
     * @param aServiceId service id 
     * @return Ecom UId in interegr
     */ 
    TInt DoResolveEcomUidL( const TDesC& aServiceId );     
                    
    /**
     * Load the ECom plugin
     * 
     * @since S60 v5.0       
     * @param aUidImp implementation UID
     * @param aXspId service provider id
     */
    void LoadEcomL( TUid aUidImp, const TDesC& aXspId );
    
    /**
     * Extracts service part from XspId
     * 
     * @since S60 v5.0       
     * @param aXspId service provider id   
     * @return service part (uri scheme part)
     */
    TPtrC ExtractService( const TDesC& aXspId );
    
private: 
    // data
    
    /**
     * Im Plugin. 
     * OWN
     */
    CCmsContactorImPluginBase* iPlugin;        

    /**
     * Service Provider Settings
     * OWN
     */
    CSPSettings* iSPSettings;
    
    };

#endif // C_CMSIMOPERATION_H

// End of File

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
* Description: Service icon manager
*
*/

#ifndef SPBSERVICEICONPROVIDER_H
#define SPBSERVICEICONPROVIDER_H

#include <e32base.h>

// FORWARD DECLARATION 
class TPbk2IconId;
class CBSFactory;
class CSpbIconItem;
class CGulIcon;

/**
 * Service icon information 
 */
NONSHARABLE_CLASS( MSpbServiceIcon )
    {
    public:
        /**
         * @return Brand id for this icon 
         */
        virtual const TPtrC8 BrandId() const = 0;
        
        /**
         * @return Brand icon id for this icon 
         */
        virtual const TPtrC8 BrandIconId() const = 0;

        /**
         * @return Unique phonebook2 icon id for the icon 
         */
        virtual const TPbk2IconId& IconId() const = 0;
        
        /**
         * @return wheter icon is valid or not. Icon can get invalid if e.g. 
         * loading fails.
         */
        virtual TBool IsValid() const = 0;

    protected:
        
        /// virtual destructor
        virtual ~MSpbServiceIcon() {}
    };

/**
 *  Status icon manager
 *
 *  @since S60 9.2
 */
NONSHARABLE_CLASS( CSpbServiceIconProvider ) : public CBase
    {
public: // constructor and destructor

    /**
     * Constructor
     */
    static CSpbServiceIconProvider* NewL();
    
    /**
     * Destructor
     */
    ~CSpbServiceIconProvider();

    
public: // Interface
    
    /**
     * Get or create a unique icon id for branded icon. Ownership is not 
     * transferred. ServiceIcon is valid until the icon manager is deleted.
     * 
     * @param aBrandId brand id for icon  
     * @param aIconId brand element id for icon
     * @return service icon  
     */
    MSpbServiceIcon* GetBrandedIconL( 
        const TDesC8& aBrandId, 
        const TDesC8& aIconId );

    /**
     * Create concrete icon for aIconId. Ownership transferred to caller.
     * 
     * @param aIconId icon ID
     * @return Icon bitmap
     */
    CGulIcon* CreateIconLC( const TPbk2IconId& aIconId );
    
    
private: // constructors

    /**
     * Constructor
     */
    inline CSpbServiceIconProvider();

    /**
     * Constructor
     */
    inline void ConstructL();

    
private: // data

    /// Own. Branding factory instance
    CBSFactory* iBrandFactory;
    
    /// Own. Array of icon details
    RPointerArray<CSpbIconItem> iIconArray;
    
    /// Index for generating unique icons
    TInt iNextFreeIconIndex;
    };

#endif // SPBSERVICEICONPROVIDER_H

/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Classes for storing contact field data of different types.
*
*/


#ifndef CVPBKVCARDCONTACTFIELDDATA_H
#define CVPBKVCARDCONTACTFIELDDATA_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MVPbkContactAttribute;
class MVPbkFieldType;
class TVersitDateTime;

// CLASS DECLARATIONS

/** 
 * Base class for all concrete contact field data classes.
 * Managing data from parsed contact field data.
 */    
NONSHARABLE_CLASS( CVPbkVCardContactFieldData ): public CBase
    {
    public:
        ~CVPbkVCardContactFieldData();
    
    protected:
        CVPbkVCardContactFieldData( const MVPbkFieldType& aFieldType );
        
    public:
        virtual TUid Uid() =0;
        
        virtual const MVPbkFieldType& FieldType();
        virtual MVPbkContactAttribute* Attribute();
        virtual void AppendAttribute( MVPbkContactAttribute* aAttribute );

    private:
        /// Ref:
        const MVPbkFieldType& iFieldType;
        /// Ref:
        MVPbkContactAttribute* iAttribute;
    };

/** 
 * Concrete contact field data class for TDesC type data.
 *
 */    
NONSHARABLE_CLASS( CVPbkVCardContactFieldDesCData ): 
    public CVPbkVCardContactFieldData
    {
    public:
        static CVPbkVCardContactFieldDesCData* NewLC
            ( const MVPbkFieldType& aFieldType, const TDesC& aValue );
        ~CVPbkVCardContactFieldDesCData();
        
    public:
        const TDesC& Value() const;        
        TUid Uid();
        
    private:
        CVPbkVCardContactFieldDesCData
            ( const MVPbkFieldType& aFieldType );
        void ConstructL( const TDesC& aValue );
            
    private:
        HBufC* iValue;
    };
    
/** 
 * Concrete contact field data class for TDesC8 type data.
 *
 */    
NONSHARABLE_CLASS( CVPbkVCardContactFieldBinaryData ): 
    public CVPbkVCardContactFieldData
    {
    public:
        static CVPbkVCardContactFieldBinaryData* NewLC
            ( const MVPbkFieldType& aFieldType, const TDesC8& aValue );
        ~CVPbkVCardContactFieldBinaryData();
        
    public:
        const TDesC8& Value() const;  
        TUid Uid();      
        
    private: 
        CVPbkVCardContactFieldBinaryData    
            ( const MVPbkFieldType& aFieldType );
        void ConstructL( const TDesC8& aValue );
        
    private:
        HBufC8* iValue;
    }; 
    
/** 
 * Concrete contact field data class for TVersitDateTime type data.
 *
 */    
NONSHARABLE_CLASS( CVPbkVCardContactFieldDateTimeData ): 
    public CVPbkVCardContactFieldData
    {
    public:
        static CVPbkVCardContactFieldDateTimeData* NewLC
            ( const MVPbkFieldType& aFieldType, const TVersitDateTime& aValue );
        ~CVPbkVCardContactFieldDateTimeData();
        
    public:
        const TVersitDateTime& Value() const;        
        TUid Uid();

    private:
        CVPbkVCardContactFieldDateTimeData
            ( const MVPbkFieldType& aFieldType, const TVersitDateTime& aValue );
    private:
        const TVersitDateTime& iValue;
    };            

#endif // CVPBKVCARDCONTACTFIELDDATA_H    
// End of file
    

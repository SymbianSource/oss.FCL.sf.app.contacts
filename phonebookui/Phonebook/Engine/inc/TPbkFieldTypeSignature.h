/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      Phonebook field type signature.
*
*/


#ifndef __TPbkFieldTypeSignature_H__
#define __TPbkFieldTypeSignature_H__

// INCLUDES
#include <e32def.h>
#include <e32std.h>


// FORWARD DECLARATIONS
class TUid;
class CPbkUidMap;
class MPbkVcardParameters;


// CLASS DECLARATION
const TInt KArraySize( 4 );

/**
 * @internal Only Phonebook internal use supported!
 *
 * Phonebook field type signature.
 */
NONSHARABLE_CLASS(TPbkFieldTypeSignature) 
    {
    public: // Constructors
        /**
         * Default constructor.
         */
        TPbkFieldTypeSignature();

        // Default copy constructor, assignment and destructor are ok for
        // this class.

    public:  // New functions
        /**
         * Returns ETrue if this signature is empty.
		 * @return ETrue if empty
         */
        TBool IsEmpty() const;

        /**
         * Returns ETrue if this signature is a proper subset of aSignature.
		 * @param aSignature field type signature
		 * @return ETrue is signature is proper subset of signature given
		 * as parameter
         */
        TBool IsProperSubsetOf(const TPbkFieldTypeSignature& aSignature) const;
        
        /**
         * Adds type aUid to this signature.
		 * @param aUid UID
		 * @param aUidMap UID map
         */
        void AddType(const TUid& aUid, const CPbkUidMap& aUidMap);

        /**
         * Adds all types in aParameters to this signature.
		 * @param aParameters parameters to add
		 * @param aUidMap UID map
         */
        void AddParameters(const MPbkVcardParameters& aParameters, 
            const CPbkUidMap& aUidMap);

    private:  // Implementation    
        TPbkFieldTypeSignature( const TFixedArray<TUint32,4>& aSignatures );        
        
    private:  // Data        
        //Own: fields type signature array
        TFixedArray<TUint32,KArraySize> iSignatures;       

    private:  // friend declarations
        friend TBool operator==
            (const TPbkFieldTypeSignature& aLhs, 
            const TPbkFieldTypeSignature& aRhs);

        friend TBool operator!=
            (const TPbkFieldTypeSignature& aLhs, 
            const TPbkFieldTypeSignature& aRhs);

        friend TPbkFieldTypeSignature Intersection
            (const TPbkFieldTypeSignature& aSig1,
            const TPbkFieldTypeSignature& aSig2);
    };


// INLINE FUNCTIONS

inline TPbkFieldTypeSignature::TPbkFieldTypeSignature()    
    { 
    const TInt count( iSignatures.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        iSignatures[i] = TUint32(0);
        }    
    }

inline TPbkFieldTypeSignature::TPbkFieldTypeSignature( const TFixedArray<TUint32,4>& aSignatures )        
    {
    const TInt count( iSignatures.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        iSignatures[i] = aSignatures[i];
        }    
    }

inline TBool TPbkFieldTypeSignature::IsEmpty() const
    {
    const TInt count( iSignatures.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        if ( iSignatures[i] != 0 )            
            {
            return EFalse;
            }
        }
    return ETrue;    
    }

inline TBool operator==
        (const TPbkFieldTypeSignature& aLhs, 
        const TPbkFieldTypeSignature& aRhs)
    {    
    for ( TInt i( 0 ); i < KArraySize; ++i )
        {
        if ( aLhs.iSignatures[i] != aRhs.iSignatures[i] )            
            {
            return EFalse;
            }
        }
    return ETrue;
    }

inline TBool operator!=
        (const TPbkFieldTypeSignature& aLhs, 
        const TPbkFieldTypeSignature& aRhs)
    {
    for ( TInt i( 0 ); i < KArraySize; ++i )
        {
        if ( aLhs.iSignatures[i] != aRhs.iSignatures[i] )            
            {
            return ETrue;
            }
        }
    return EFalse;    
    }

inline TBool TPbkFieldTypeSignature::IsProperSubsetOf
        (const TPbkFieldTypeSignature& aSignature) const
    {        
    const TInt count( iSignatures.Count() );
    for( TInt i( 0 ); i < count; ++i )
        {        
        if ( (iSignatures[i] & aSignature.iSignatures[i] ) != 
              iSignatures[i] )
            {
            return EFalse;
            }        
        }

    return ETrue;    
    }

inline TPbkFieldTypeSignature Intersection
        (const TPbkFieldTypeSignature& aSig1,
        const TPbkFieldTypeSignature& aSig2)
    {        
    TFixedArray<TUint32,KArraySize> signatures;    
    for( TInt i( 0 ); i < KArraySize; ++i )
        {        
        signatures[i] = TUint32( aSig1.iSignatures[i] & aSig2.iSignatures[i] );
        }

    return TPbkFieldTypeSignature(signatures);        
    }
#endif // __TPbkFieldTypeSignature_H__

// End of File

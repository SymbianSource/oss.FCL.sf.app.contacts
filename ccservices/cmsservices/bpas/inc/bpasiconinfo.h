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
* Description:  Icon information holder
*
*/



#ifndef C_BPASICONINFO_H
#define C_BPASICONINFO_H


/**
 * Bpas icon info class. Image element id must be valid to create this object.
 *
 *  @lib bpas.lib
 *  @since S60 v5.0
 */
class CBPASIconInfo: public CBase
    {

public: 

    /**
     * Symbian constructors. Not exported since only used inside bpas
     *
     * @param iImageElementId image element id for this service
     * @return CBPASIconInfo* newly created object
     */
     static CBPASIconInfo* NewL(const TDesC8& iImageElementId);
     
     static CBPASIconInfo* NewLC(const TDesC8& iImageElementId);
    

    /**
     * Text element id. KNullDesC if not found
     *
     * @return TPtrC8 text element id
     */
     IMPORT_C TPtrC8 TextElementId() const;
     

    /**
     * Image element id.
     *
     * @return TPtrC8 image element id
     */
     IMPORT_C TPtrC8 ImageElementId() const;


    /**
     * Public destructor. 
     */
     virtual ~CBPASIconInfo();
     
    /**
     * Sets Text element id
     *
     * @param TDesC8 text element id
     */
     void SetTextElementIdL(const TDesC8& aTextElementId);

    
private:
    
    CBPASIconInfo();
    void ConstructL(const TDesC8& iImageElementId);
    
private:
    
    // own
    HBufC8*     iTextElementId;//optional
    
    //own
    HBufC8*     iImageElementId;//mandatory
    };


#endif // C_BPASICONINFO_H

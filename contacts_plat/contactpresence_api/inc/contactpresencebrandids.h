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
* Description:  Factory class for contactpresence.dll
*
*/

#ifndef CONTACTPRESENCEBRANDIDS_H
#define CONTACTPRESENCEBRANDIDS_H


/*
 * contactpresencebrandids.h contains the literals and constants used 
 * by Meco, PhoneBook, VOIP for  
 * creating a Brand Element Id which can be used in the BrandPackage and 
 * used to retreive the same brand either Image/Icon from the Branding Server  
 *
 * @since s60 v5.0
 */

// Literals to be used within element id creation

/*
 * KCPBrandAppId stands for the application id for fetching the brand icons/texts
 * from the branding server.
 * @since s60 v5.0 
 */
_LIT8( KCPBrandAppId, "xsp" );

/*
 * KCPBrandDefaultId stands for the default brand id. 
 * if the brandpackages for the service is not installed or not available,
 * then the icons/texts installed with this brand id will be used
 * PreRequisite : Pkg with this brand id needs to be installed to the h/w
 * By default this brand pkg must be made available in the h/w
 * @since s60 v5.0
 */
_LIT8( KCPBrandDefaultId, "xsp" ); 


/*
 * KCPBrandElementIdLocalizedServiceName stands for BrandID which can be
 * used for getting the localised service name from the Branding Server.
 * @since s60 v5.0
 */
_LIT8( KCPBrandElementIdLocalizedServiceName, "service_name" );


/*
 * KCPBrandElementIdServiceIcon stands for BrandID which can be
 * used for getting the branded icon for a service from the Branding Server.
 * @since s60 v5.0
 */
_LIT8( KCPBrandElementIdServiceIcon, "default_brand_image" );


/* The following Literals below helps us in forming the brand element ids which can be used 
 * for denoting diff states of presence
 * KCPBrandElementIdPerson  - the first part of the brand element id used to denote the 
 *                              Presence of a contact
 * KCPBrandElementIdDevice  - the first part of the brand element id used to denote the 
 *                              Presence of a device
 * KCPBrandElementIdService - the first part of the brand element id used to denote the 
 *                              Presence of a service
 * KCPBrandElementIdImage   - Stands for the brand element id which is of type Image/Icon
 * KCPBrandElementIdText    - Stands for the brand element id which is of type Text 
 *  
 * Sample:
 * BrandId Element for Presence of any contact must be formed as below:
 * "person:available:image" - This is the ElementId for the Presence of a contact,
 * whose presence status is "available"
 * and this element id stands for a Image(Icon).
 * 
 * How to use the below literals to form/create a Brand element Id? 
 * If the Presence is for a Contact, and you want a Brand Icon - 
 *     Element Id = KCPBrandElementIdPerson + Status from Presence Cache + KCPBrandElementIdImage
 * 
 * If the Presence is for a Contact, and you want a Brand text(Presence Status) - 
 *     Element Id = KCPBrandElementIdPerson + Status from Presence Cache + KCPBrandElementIdText
 *                
 * @since s60 v5.0                             
 */
_LIT8( KCPBrandElementIdPerson,  "person:" );
_LIT8( KCPBrandElementIdDevice,  "device:" ); 
_LIT8( KCPBrandElementIdService, "service:" );
_LIT8( KCPBrandElementIdImage,   ":image" );
_LIT8( KCPBrandElementIdText,    ":text" );


/*
 * KCPBrandElementIdMaxLength stands for Brand Element Ids Max length
 * @since s60 v5.0
 */
const TInt KCPBrandElementIdMaxLength = 100; //Brand Element Ids Max length


/*
 * Sample Brand Package is available below
 * 
 * 
 * 
    <branding>
        <brand>
            <application_id>    xsp</application_id>      
            <brand_id>          xsp</brand_id>            
            <brand_language_id> 01</brand_language_id>
            <brand_version>     01</brand_version>
            <element type="file" id="xxx_mif">
                <element_value>
                    <file_name>c:\private\102828DD\import\xxx.mif</file_name>
                </element_value>
            </element>
    
            <element id="person:available:image" type="bitmap">  
                <element_value>
                    <bitmap_file_id> vimpstui_mif</bitmap_file_id>
                    <bitmap_id>      163xx </bitmap_id>
                    <mask_id>        163xx </mask_id>
                    <skin_id_major> 2705016xx </skin_id_major>
                    <skin_id_minor>   44xx </skin_id_minor>
                </element_value>
            </element>
            
            <element id="person:available:text" type="text">
                <element_value>"online"</element_value>
            </element>
    
            <element id="service_name" type="text">
                <element_value>my localised string</element_value>
            </element>
            
        </brand>
    </branding>
 * 
 */

#endif // CONTACTPRESENCEBRANDIDS_H

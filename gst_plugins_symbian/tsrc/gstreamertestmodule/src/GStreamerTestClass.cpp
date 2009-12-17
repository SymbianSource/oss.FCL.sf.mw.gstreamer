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
* Description: 
*
*/




// INCLUDE FILES
#include <StifTestInterface.h>
//#include "GStreamerTestClass.h"
#include "GStreamerTestClass.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGStreamerTestClass::CGStreamerTestClass
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CGStreamerTestClass::CGStreamerTestClass( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf ), 
        iTestModuleIf(aTestModuleIf)
    {
    //myClassPtr =  this;
    }

// -----------------------------------------------------------------------------
// CGStreamerTestClass::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGStreamerTestClass::ConstructL()
    {
    iLog = CStifLogger::NewL( KGStreamerTestClassLogPath, 
                          KGStreamerTestClassLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
                          
	iTimeoutController = CSimpleTimeout::NewL (this, iLog);
	iLog->Log(_L(">>CGStreamerTestClass::ConstructL "));
	
    }

// -----------------------------------------------------------------------------
// CGStreamerTestClass::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGStreamerTestClass* CGStreamerTestClass::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CGStreamerTestClass* self = new (ELeave) CGStreamerTestClass( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CGStreamerTestClass::~CGStreamerTestClass()
    { 

    // Delete resources allocated from test methods
    Delete();
    iLog->Log(_L(">>CGStreamerTestClass::~CGStreamerTestClass "));
    // Delete logger
    delete iLog; 
	delete iTimeoutController;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) CGStreamerTestClass::NewL( aTestModuleIf );

    }

// -----------------------------------------------------------------------------
// E32Dll is a DLL entry point function.
// Returns: KErrNone
// -----------------------------------------------------------------------------
//
#ifndef EKA2 // Hide Dll entry point to EKA2
GLDEF_C TInt E32Dll(
    TDllReason /*aReason*/) // Reason code
    {
    return(KErrNone);

    }
#endif // EKA2

//  End of File

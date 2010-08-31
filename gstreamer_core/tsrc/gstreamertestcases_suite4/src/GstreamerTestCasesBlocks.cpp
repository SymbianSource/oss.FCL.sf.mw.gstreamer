/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA 02111-1307, USA.
*
* Description: This file contains testclass implementation.
*
*/

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>
#include "GstreamerTestCases.h"

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
// CGstreamerTestCases::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CGstreamerTestCases::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CGstreamerTestCases::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CGstreamerTestCases::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", CGstreamerTestCases::ExampleL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CGstreamerTestCases::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CGstreamerTestCases::ExampleL( CStifItemParser& aItem )
{
    TInt err = KErrNone;
    RTimer timeoutTimer;
    // Print to UI
//    _LIT( KGstreamerTestCases, "GstreamerTestCases" );
//    _LIT( KExample, "Test case: " );
//    TestModuleIf().Printf( 0, KGstreamerTestCases, KExample );
    // Print to log file

    TPtrC string[2];
    TFileName arg;
    //_LIT( KParam, "Param[%i]: %S" );
    
    if( aItem.GetNextString ( string[0] ) != KErrNone)
    {
        iLog->Log( _L(">> Test cases Not Found") );
        return -1;
    }
    iLog->Log( _L( "Test case Name is : " ) );
    iLog->Log( string[0] );
    
    while ( aItem.GetNextString ( string[1] ) == KErrNone )
    {
        iLog->Log( string[1] );
        arg.Append( string[1] );
        arg.Append( TChar(' ') );
    }
    
    
    _LIT( KArg, "Arg: " );
    iLog->Log( KArg );
    iLog->Log( arg );
    RProcess process;
    err = process.Create( string[0],arg );
    
    if( !err )
    {

        if( timeoutTimer.CreateLocal() != KErrNone )
        {
            iLog->Log( _L(">> timeoutTimer.CreateLocal failed") );
        }
    
        iLog->Log( _L(">> Test cases Started") );
        TRequestStatus processStatus = KRequestPending;
        TRequestStatus timerStatus = KRequestPending;
        process.Logon( processStatus );
        
        process.Resume();
        TTimeIntervalMicroSeconds32 timeout = 1 * 60 * 1000000;
        timeoutTimer.After( timerStatus, timeout );
        
        while( processStatus == KRequestPending && timerStatus == KRequestPending )
        {
            User::WaitForAnyRequest();
            
        }
        
        err = process.ExitReason();
        if( timerStatus != KRequestPending )
        {
            err = KErrTimedOut;
        }
        
        process.LogonCancel( processStatus );
        timeoutTimer.Cancel();
        /// required to complete one of the pending request..
        User::WaitForAnyRequest();
        
        process.Kill( KErrNone );
        process.Close();
        timeoutTimer.Close();
        iLog->Log( _L(">> test cases completed") );
    }
    else
    {
        iLog->Log( _L(">> test cases exe not found") );
    }


    if( err != KErrNone )
    {
        iLog->Log( CStifLogger::ERed, _L( "Test cases FAILED!" ) );
    }
    else
    {
        iLog->Log( CStifLogger::EGreen, _L( "Test cases PASSED!" ) );
    }
    

    return err;
}

// -----------------------------------------------------------------------------
// CGstreamerTestCases::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CGstreamerTestCases::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove

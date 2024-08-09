// ----[DYNAMICALLY CREATING STATIC STRINGS]----- 
// a library to define static strings, where the pointer address does not change
// required for libraries where it is good to programmatically define strings
// this library creates a string based on a dynamic input. and returns a pointer to 
// the static string held in the string library from there on in

#pragma once

#define RESERVE_STRINGS    8   // reserve 8 strings for UIDs
#define FIXED_STR_LENGTH   64  // max string length is 64 chars
int NEXT_ID = 0;               // let's count how many we use

const char* staticStrings[RESERVE_STRINGS]; // Array to store C-style strings (const char*)

// Save a string in the array and return a pointer to the saved string
const char* storeStaticString(const String& s_data) {
    // Check for available space
    if (NEXT_ID >= RESERVE_STRINGS) {
        logSuspend("Allocation full. Not enough space reserved for static strings");
        return nullptr; // Indicate an error
    }

    // Convert the Arduino String to a C-style string
    char c_data[FIXED_STR_LENGTH]; 
    s_data.toCharArray(c_data, FIXED_STR_LENGTH); // Copy String to char array

    // Allocate memory and copy the string
    staticStrings[NEXT_ID] = strdup(c_data); 
    NEXT_ID++;

    return staticStrings[NEXT_ID - 1];
}
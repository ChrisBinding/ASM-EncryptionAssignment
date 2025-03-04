// Filename: "FoCA Assignment 2 - Milestone 2.cpp"
//
// Chris Binding, last update: 29th March 2014

#include <conio.h>		// for kbhit
#include <iostream>		// for cin >> and cout <<
#include <iomanip>		// for fancy output
using namespace std;

#define MAXCHARS 6		// feel free to alter this, but 6 is the minimum
#define dollarchar '$'  // string terminator

char OChars[MAXCHARS], EChars[MAXCHARS], DChars[MAXCHARS];	// Global Original, Encrypted, Decrypted character strings

//----------------------------- C++ Functions ----------------------------------------------------------


void get_char(char& a_character)
{
	cin >> a_character;
	while (((a_character < '0') | (a_character > 'z')) && (a_character != dollarchar))
	{
		cout << "Alphanumeric characters only, please try again > ";
		cin >> a_character;
	}
}
//-------------------------------------------------------------------------------------------------------------

void get_original_chars(int& length)
{
	char next_char;
	length = 0;
	get_char(next_char);

	while ((length < MAXCHARS) && (next_char != dollarchar))
	{
		OChars[length++] = next_char;
		get_char(next_char);
	}
}

//---------------------------------------------------------------------------------------------------------------
//----------------- ENCRYPTION ROUTINES -------------------------------------------------------------------------

void encrypt_chars(int length, char EKey)
{
	char temp_char;							// char temporary store

	for (int i = 0; i < length; i++)		// Encrypt characters one at a time
	{
		temp_char = OChars[i];
		__asm {
				push   eax					// Save register values on stack to be safe
				push   ecx					//			 
				lea    eax, EKey			// 32-bit memory address of EKey is loaded into EAX, as a pointer, to be passed as a parameter
				movzx  ecx, temp_char		// Moves 8-bit value of 'temp_char' to the LSB of the ECX register, remaining 24 bits of ECX cleared to zero
				push   eax					// Save 32-bit EKey memory address pointer to the stack 
				push   ecx					// Save 8-bit value of 'temp_char' to the stack
				call   encrypt4				// The 'encrypt4' subroutine is called to encrypt the current character
				add	   esp, 8				// CDECL stack clean-up by caller, clears used parameters from stack  
				mov    temp_char, al		// Encrypted character, stored in the LSB of EAX, AL, is copied into the 'temp_char' variable to save
				pop    ecx					// Restore original register values from stack
				pop    eax					//	
			}
			EChars[i] = temp_char;			// Store encrypted char in the encrypted chars array
	}
	return;

	// --- Start of Assembly code
	__asm {
		// Encrypt subroutine.
		// Inputs: parameter [EBP+12] = 32-bit address of Ekey, 
		//	     			 [EBP+8] = the character to be encrypted.
		// Output: register EAX = the encrypted value of the source character (in the low 8-bit field, AL).


	encrypt4:	push	ebp						// Push the EBP register on to the stack to save its present value
				mov		ebp, esp				// Stack Pointer copied into Base Pointer, fixed reference for accessing parameters
				mov		ecx, [ebp + 8]			// temp_char parameter copied via EBP offset from stack, placed into the LSB of the ECX register
				mov		eax, [ebp + 0xc]		// EKey 32-bit address pointer copied into the EAX register from the stack
				push	edi						// Save EDI register value on stack for safe keeping
				push	ecx						// Save character to be encrypted on the stack for later retrieval
				not	byte ptr[eax]				// LSB value of 'EKey' set to logical negative of itself 
				add	byte ptr[eax], 0x04			// Increment the LSB value of 'EKey' by hex value 04
				movzx	edi, byte ptr[eax]		// LSB of 'EKey' copied into the LSB of the EDI register, remaining 24 bits of EDI cleared to zero
				pop	eax							// Character to be encrypted removed from stack into the EAX register
				xor	eax, edi					// Character is encrypted using the XOR function and the modified encryption key stored in the EDI register
				pop	edi							// Modified encryption key is cleared from EDI register, original value of EDI restored from stack
				rol	al, 1						// Encrypted character is further encrypted, it's LSB value is rotated left, by 1 bit
				rol	al, 1						// Encrypted character is further encrypted, it's LSB value is rotated left, by 1 bit
				add	al, 0x04					// Encrypted character is further encrypted, Hex value 04 is added to its LSB value 
				pop	ebp							// Restore the caller's original EBP value
				ret								// CDECL subroutine return instruction
	}

	//--- End of Assembly code
}
// end of encrypt_chars function
//---------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------
//----------------- DECRYPTION ROUTINES -------------------------------------------------------------------------

void decrypt_chars(int length, char EKey)
{   
	char temp_char;							// char temporary store

	for (int i = 0; i < length; i++)		// Decrypt characters one at a time
	{
		temp_char = EChars[i];
		__asm {
				push	eax					// Save register values on stack to be safe
				push   ecx					//			 
				lea    eax, EKey			// 32-bit memory address of EKey is loaded into EAX, as a pointer, to be passed as a parameter
				movzx  ecx, temp_char		// Moves 8-bit value of 'temp_char' to the LSB of the ECX register, remaining 24 bits of ECX cleared to zero
				push   eax					// Save 32-bit EKey memory address pointer to the stack 
				push   ecx					// Save 8-bit value of 'temp_char' to the stack	
				call   decrypt4				// The 'decrypt4' subroutine is called to decrypt the current character
				add	   esp, 8				// CDECL stack clean-up by caller, clears used parameters from stack  
				mov    temp_char, al		// Decrypted character, stored in the LSB of EAX, AL, is copied into the 'temp_char' variable to save
				pop    ecx					// Restore original register values from stack
				pop    eax					//	
		}
		DChars[i] = temp_char;				// Store decrypted char in the decrypted chars array
	}

		return;

		// --- Start of Assembly code
		__asm {
			// Decrypt subroutine.
			// Inputs: parameter [EBP+12] = 32-bit address of Ekey, 
			//	     			 [EBP+8] = the character to be decrypted.
			// Output: register EAX = the decrypted value of the source character (in the low 8-bit field, AL).


		decrypt4:	push	ebp							// Push the EBP register on to the stack to save its present value
					mov		ebp, esp					// Stack Pointer copied into Base Pointer, fixed reference for accessing parameters
					mov		ecx, [ebp + 8]				// temp_char parameter copied via EBP offset from stack, placed into the LSB of the ECX register
					mov		eax, [ebp + 0xc]			// EKey 32-bit address pointer copied into the EAX register from the stack
					push	edi							// Save EDI register value on stack for safe keeping
					push	ecx							// Save character to be decrypted on the stack for later retrieval
					not		byte ptr[eax]				// LSB value of 'EKey' set to logical negative of itself, same modified EKey must be obtained again to decrypt
					add		byte ptr[eax], 0x04			// Increment LSB value of 'EKey' by hex value 04
					movzx	edi, byte ptr[eax]			// LSB value of 'EKey' copied into the LSB of the EDI register, remaining 24 bits of EDI cleared to zero
					pop		eax							// Character to be decrypted removed from top of stack into the EAX register
					sub		al, 0x04					// Character decrypted, by reversing the encryption routine, subtracting hex value 04 first
					ror		al, 2						// Further decryption, right rotation reverses encryption left rotations
					xor		eax, edi					// Decryption, reverses the encryption XOR operation, storing the result in the EAX register
					pop		edi							// Modified en/decryption key is cleared from EDI register, original value of EDI restored from stack
					pop		ebp							// Restore the caller's original EBP value
					ret
		}

		//--- End of Assembly code

}
// end of decrypt_chars function
//---------------------------------------------------------------------------------------------------------------


int main(void)
{
	int  char_count;	// The number of actual characters entered (upto MAXCHARS limit).
	char EKey;			// Encryption key.

	cout << "\nPlease enter your Encryption Key (EKey) letter: "; get_char(EKey);

	cout << "\nNow enter upto " << MAXCHARS << " alphanumeric characters:\n";
	get_original_chars(char_count);
	cout << "\n\nOriginal source string = " << OChars << "\tHex = ";
	for (int i = 0; i<char_count; i++) cout << hex << setw(2) << setfill('0') << ((int(OChars[i])) & 0xFF) << "  ";

	encrypt_chars(char_count, EKey);
	cout << "\n\nEncrypted string       = " << EChars << "\tHex = ";
	for (int i = 0; i<char_count; i++) cout << ((int(EChars[i])) & 0xFF) << "  ";

	decrypt_chars(char_count, EKey);
	cout << "\n\nDecrypted string       = " << DChars << "\tHex = ";
	for (int i = 0; i<char_count; i++) cout << ((int(DChars[i])) & 0xFF) << "  ";

	cout << "\n\nPress a key to end...";
	while (!_kbhit());					//hold the screen until a key is pressed
	return (0);


} // end of whole encryption/decryption program --------------------------------------------------------------------



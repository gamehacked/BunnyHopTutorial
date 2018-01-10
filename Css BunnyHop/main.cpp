/*
This code allows us to create a bunny hop hack for CSS,
in theory this will work for most games. 
Especially source based like TF2 all Counter-strikes etc.

Credits to ExcidiumDubstep for external base tutorial and Nubtik for certain code sections

Written by Fleep at http://guidedhacking.com/, feel free to check us out for many tutorials 
on cheating and join our friendly hacking community
If you end up releasing any hacks from this code they would be greatly appreciated on our GuidedHacking.
*/

#include <Windows.h>  
#include <iostream> 
#include "HackProcess.h" 

//Create our 'hooking' and process managing object
CHackProcess fProcess;  
using namespace std;  

//The player base is VERY important so we know where our player info is at
//including current jump status so we can use force jumping making our bHop
const DWORD Player_Base = 0x53FB04;//0x00574560;
//BY WRITTING 1 BYTE TO JUMP WE ARE PERFORMING THE ACTION + JUMP
//FOUND BY SEARCHING FOR 5 WHEN ENTERING + JUMP AND 4 WHEN - JUMP
const DWORD dw_Jump = 0x56C5CC;//client.dll
//Read the current jump status
const DWORD dw_JumpOffset = 0x34C;

//Variables required for bunny hop
//is our player on the ground?
#define FL_ONGROUND 257
//0x20 represents space see http://msdn.microsoft.com/en-us/library/ms927178.aspx
#define SPACE_BAR 0x20
//We use F6 to exit the hack
#define F6_Key 0x75
//We write this into Dw_Jump this value to say +Jump
bool b_True = true;
//Or if false -Jump
bool b_False = false;
//Used to set bHop on/off
bool BunnyHopStatus = false;

//We will use this struct throughout all other tutorials adding more variables every time
struct MyPlayer_t  
{ 
	DWORD CLocalPlayer; 
	int m_fFlags;
	void ReadInformation() 
	{
		// Reading CLocalPlayer Pointer to our "CLocalPlayer" DWORD. 
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordClient + Player_Base), &CLocalPlayer, sizeof(DWORD), 0);
		//Read BHop status
		//Got this by scanning for 0 jumping 1 as a BYTE on ground
		//also checking for different states i n integers, scanned them all and with the results
		//saw the closest one to our base address and added the offset different after comparing in data dissect 
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(CLocalPlayer + dw_JumpOffset), &m_fFlags, sizeof(int), NULL);  //CLocalPlayer + 0x22C
	}
}MyPlayer;  


void BunnyHop()
{
	if(GetAsyncKeyState(SPACE_BAR))
	{
		BunnyHopStatus = !BunnyHopStatus;
		//Normally I would use a timer but because this is very simple we put Sleep for 250ms here
		//so that once we press SPACE then it turns bunny hop On or Off just once
		//If you don't have this here it will switch off/on BHop many times in a second 
		Sleep(250);
	}

	//If bunny hop is off leave function
	if(!BunnyHopStatus)
		return;

	//if space is pressed and we are on the ground then JUMP
	if(MyPlayer.m_fFlags ==  FL_ONGROUND)
	{
		//bHOP bay bay
		WriteProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordClient + dw_Jump), &b_True, sizeof(bool), NULL);
	}
	//If we are in the air reset the flag so we can jump again after
	else if(MyPlayer.m_fFlags != FL_ONGROUND)
	{
		//bHOP bay bay, SWITCH off jump
		WriteProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordClient + dw_Jump), &b_False, sizeof(bool), NULL);
	}
}


//Our main loop
int main() 
{   
	fProcess.RunProcess(); // Waiting for CSS......
	cout << "Game found! Running Bunny Hop..." << endl; 

	//Exit if the F6 key is pressed
	while (!GetAsyncKeyState(F6_Key)) // or for(;;)
	{
		//Read player information into our struct so we know the player 
		//base and the bunny hop addresses
		MyPlayer.ReadInformation(); 
		//std::cout << (int)MyPlayer.f_Flags << std::endl;

		//Perform our bunny hop
		BunnyHop();
	}

}
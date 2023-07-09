/*
        This file is part of VirtualBiosMod, a virtual bios setup interface
        Copyright (C) 2020 Alexandru Marc Serdeliuc

        https://github.com/serdeliuk/VirtualBiosMod

        VirtualBiosMod is free software: you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation, either version 3 of the License, or
        (at your option) any later version.

        VirtualBiosMod is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with VirtualBiosMod.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <efi.h>
#include <efilib.h>
#include "functions.c"

EFI_STATUS efi_main (EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
    int changes = 0;
    int vmajor = 1;
    int vminor = 0;
    int vpatch = 8;
    int params = 0;

    EFI_STATUS status;
    EFI_GUID guid = { 0x3A997502, 0x647A, 0x4C82, { 0x99, 0x8E, 0x52, 0xEF, 0x94, 0x86, 0xA2, 0xA7 } };

    CHAR8 *data;
    UINTN data_size;
    UINT32 *attr = 0;
    EFI_INPUT_KEY efi_input_key;
    EFI_INPUT_KEY KeyReset = {0};

    EFI_STATUS efi_status;
    EFI_LOADED_IMAGE *loaded_image = NULL;
    BOOLEAN exit = FALSE;

    InitializeLib(image, systab);

    status = uefi_call_wrapper(systab->BootServices->HandleProtocol, 3, image, &LoadedImageProtocol, (void **) &loaded_image);
    if (EFI_ERROR(status)) {
        Print(L"[VirtualBiosMod] err: %r\n", status);
    } else {
		params = loaded_image->LoadOptionsSize;
    }

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    if ( params == 0 ){
    	Print(L"\n\n\n[VirtualBiosMod v%d.%d.%d] Press ",vmajor,vminor,vpatch);
    	uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLUE);
    	Print(L"del");
    	uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLACK);
    	Print(L" or any other key to enter Setup...");

    	WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second

    	while (!exit) {

    	efi_status = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &efi_input_key);

		if (efi_status != EFI_SUCCESS) {
		    Print(L" Exiting\n\n\n");
		    return EFI_SUCCESS;
		} else {
		    break;
		}
    	}
    }

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLUE);

/////////--DRAW MAIN BLUE BOX--/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Print(L"             SERDELIUK - VirtualBiosMod v%d.%d.%d CMOS Setup Utility               ",vmajor,vminor,vpatch);
    draw_box_simple(80, 8, 0, 1);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 2); // h, v ;pos
    Print(L"Firmware version:       %d.%02d", ST->FirmwareRevision >> 16, ST->FirmwareRevision & 0xffff);
    Print(L" (%s)", ST->FirmwareVendor,L"%c"); // firmware vendoor
    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 3); // h, v ;pos
    Print(L"UEFI version:           %d.%02d", ST->Hdr.Revision >> 16, ST->Hdr.Revision & 0xffff);

    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 0, 12);

    status = get_bios_variables( &guid, L"AmdSetup", &data, &data_size, attr);
    if (status != EFI_SUCCESS) {
	uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_RED|EFI_BACKGROUND_BLACK);
        Print(L"Unsupported B.I.O.S.\n" , status);
	WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
	    if ( params == 0){ 
    		return EFI_SUCCESS;
	    } else {
		uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    }
    }

	int offset_uma_frame_buffer = 0xC4;

// UI DRAW
redraw:
    WaitForSingleEvent(ST->ConIn->WaitForKey, 10); // 10000000 = one second
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLUE);

	switch(data[offset_uma_frame_buffer]) {
		case 0x200:
			uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
			Print(L"UMA Frame Buffer Size:        512M        ");
			break;
		case 0x400:
			uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
			Print(L"UMA Frame Buffer Size:        1G        ");
			break;
		case 0x800:
			uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
			Print(L"UMA Frame Buffer Size:        2G        ");
			break;
		case 0xC00:
			uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
			Print(L"UMA Frame Buffer Size:        3G        ");
			break;
		case 0x1000:
			uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
			Print(L"UMA Frame Buffer Size:        4G        ");
			break;
		case 0xFFFFFFFF:
			uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
			Print(L"UMA Frame Buffer Size:        AUTO        ");
			break;
		default:
			uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
			Print(L"UMA Frame Buffer Size:        ???        ");
			break;
	}

    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLACK);
    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 0, 11);

    Print(L" Press 0 to set UMA Buffer to 512M\n");
    Print(L" Press 1 to set UMA Buffer to 1G\n");
    Print(L" Press 2 to set UMA Buffer to 2G\n");
    Print(L" Press 3 to set UMA Buffer to 3G\n");
    Print(L" Press 4 to set UMA Buffer to 4G\n");
    Print(L" Press 5 to set UMA Buffer to AUTO\n");


    Print(L" Press ENTER to save new settings\n");
    if ( changes == 0 ) {
		Print(L" Press any other key or wait to boot without any mods\n");
    } else {
		Print(L" Press E to boot without any mods                    \n");
    }

    WaitForSingleEvent(ST->ConIn->WaitForKey, 30000000); // 10000000 = one second

    while (!exit) {

    efi_status = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &efi_input_key);

    switch (efi_input_key.UnicodeChar) {
        case '0':
			changes=1;
	    	if ( data[offset_uma_frame_buffer] != 0x200) 
				data[offset_uma_frame_buffer] = 0x200;

	    	efi_input_key = KeyReset;
	    	goto redraw;
        case '1':
			changes=1;
	    	if ( data[offset_uma_frame_buffer] != 0x400) 
				data[offset_uma_frame_buffer] = 0x400;

	    	efi_input_key = KeyReset;
	    	goto redraw;
        case '2':
			changes=1;
	    	if ( data[offset_uma_frame_buffer] != 0x800)
				data[offset_uma_frame_buffer] = 0x800;

	    	efi_input_key = KeyReset;
	    	goto redraw;
        case '3':
			changes=1;
	    	if ( data[offset_uma_frame_buffer] != 0xC00)
				data[offset_uma_frame_buffer] = 0xC00;

	    	efi_input_key = KeyReset;
	    	goto redraw;
        case '4':
			changes=1;
	    	if ( data[offset_uma_frame_buffer] != 0x1000)
				data[offset_uma_frame_buffer] = 0x1000;

	    	efi_input_key = KeyReset;
	    	goto redraw;
        case '5':
			changes=1;
	    	if ( data[offset_uma_frame_buffer] != 0xFFFFFFFF)
				data[offset_uma_frame_buffer] = 0xFFFFFFFF;

	    	efi_input_key = KeyReset;
	    	goto redraw;

        case 'e':
    	    Print(L" Exiting......\n");
    	    WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
	    	if ( params == 0){ 
    			return EFI_SUCCESS;
	    	} else {
				uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    	}

        case CHAR_CARRIAGE_RETURN:
	    	if ( changes == 0 ) {
    			Print(L" Nothing to save, booting......\n");
    			WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
	    	if ( params == 0){ 
    			return EFI_SUCCESS;
	    	} else {
				uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    	}
	    	}

	    	status = set_bios_variables(L"AmdSetup", &guid, data_size, data);
	    	if (status != EFI_SUCCESS) {
				uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_RED|EFI_BACKGROUND_BLACK);
				Print(L" ERROR saving data %r\n" , status);
				WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
	    		if ( params == 0){ 
    				return EFI_SUCCESS;
	    		} else {
					uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    		}
	    	}
			uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_GREEN|EFI_BACKGROUND_BLACK);
	    	Print(L" Write data OK, rebooting...          \n");
    		WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
	    	uefi_call_wrapper(RT->ResetSystem, 4, EfiResetCold, EFI_SUCCESS, 0, NULL);
    		return EFI_SUCCESS;

        default: // continue boot
	    	if ( changes == 1 ) {
				efi_input_key = KeyReset;
				goto redraw;
	    	}

    	    Print(L" Exiting......\n");
    	    WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
	    	if ( params == 0){ 
    			return EFI_SUCCESS;
	    	} else {
				uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    	}
        }
    }
    
	return EFI_SUCCESS;
}

#pragma once

#define ERR(x) if(EFI_ERROR((x))) return (x)
#define efi_panic(msg, code) { st->ConOut->OutputString(st->ConOut, (char16_t *) (msg)); st->BootServices->Stall(1000 * 1000 * 5); return (code); }

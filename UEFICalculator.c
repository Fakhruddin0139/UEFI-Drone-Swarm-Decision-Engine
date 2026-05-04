#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/SerialIo.h>

void *memcpy(void *dest, const void *src, UINTN count)
{
    UINT8 *d = (UINT8 *)dest;
    const UINT8 *s = (const UINT8 *)src;
    while (count--)
        *d++ = *s++;
    return dest;
}

void *memset(void *dest, int val, UINTN count)
{
    UINT8 *d = (UINT8 *)dest;
    while (count--)
        *d++ = (UINT8)val;
    return dest;
}

// Get a single digit choice from user (1-9)
UINTN GetChoice()
{
    EFI_INPUT_KEY Key;
    gST->ConIn->Reset(gST->ConIn, FALSE);
    while (gST->ConIn->ReadKeyStroke(gST->ConIn, &Key) != EFI_SUCCESS)
        ;
    if (Key.UnicodeChar >= L'1' && Key.UnicodeChar <= L'9')
    {
        return (UINTN)(Key.UnicodeChar - L'0');
    }
    return 0;
}

// Get a multi-digit number from user
INTN GetNumber()
{
    CHAR16 Buffer[20];
    UINTN Index = 0;
    EFI_INPUT_KEY Key;
    BOOLEAN IsNegative = FALSE;

    gST->ConIn->Reset(gST->ConIn, FALSE);

    while (TRUE)
    {
        while (gST->ConIn->ReadKeyStroke(gST->ConIn, &Key) != EFI_SUCCESS)
            ;

        if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN)
        {
            Buffer[Index] = L'\0';
            Print(L"\n");
            break;
        }

        if (Key.UnicodeChar == CHAR_BACKSPACE && Index > 0)
        {
            Index--;
            Print(L"\b \b");
            continue;
        }

        if (Key.UnicodeChar == L'-' && Index == 0)
        {
            IsNegative = TRUE;
            Print(L"-");
            Buffer[Index++] = L'-';
            continue;
        }

        if (Key.UnicodeChar >= L'0' && Key.UnicodeChar <= L'9' && Index < 19)
        {
            Buffer[Index++] = Key.UnicodeChar;
            Print(L"%c", Key.UnicodeChar);
        }
    }

    INTN Result = 0;
    UINTN Start = IsNegative ? 1 : 0;

    for (UINTN i = Start; i < Index; i++)
    {
        Result = Result * 10 + (Buffer[i] - L'0');
    }

    return IsNegative ? -Result : Result;
}

// Wait for any key press
VOID WaitForKey()
{
    EFI_INPUT_KEY Key;
    Print(L"\nPress any key to continue...");
    gST->ConIn->Reset(gST->ConIn, FALSE);
    while (gST->ConIn->ReadKeyStroke(gST->ConIn, &Key) != EFI_SUCCESS)
        ;
}

// Simple power function
INTN Power(INTN base, INTN exp)
{
    INTN result = 1;
    for (INTN i = 0; i < exp; i++)
    {
        result *= base;
    }
    return result;
}

// Square root
INTN SquareRoot(INTN num)
{
    if (num < 0)
        return -1;
    if (num == 0)
        return 0;

    INTN x = num;
    INTN y = (x + 1) / 2;

    while (y < x)
    {
        x = y;
        y = (x + num / x) / 2;
    }
    return x;
}

// Factorial
INTN Factorial(INTN n)
{
    if (n < 0)
        return -1;
    if (n == 0 || n == 1)
        return 1;

    INTN result = 1;
    for (INTN i = 2; i <= n; i++)
    {
        result *= i;
    }
    return result;
}
// Get a string input from user (letters, numbers, spaces)
VOID GetString(CHAR16 *Buffer, UINTN MaxLen)
{
    UINTN Index = 0;
    EFI_INPUT_KEY Key;

    gST->ConIn->Reset(gST->ConIn, FALSE);

    while (TRUE)
    {
        while (gST->ConIn->ReadKeyStroke(gST->ConIn, &Key) != EFI_SUCCESS)
            ;

        if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN)
        {
            Buffer[Index] = L'\0';
            Print(L"\n");
            break;
        }

        if (Key.UnicodeChar == CHAR_BACKSPACE && Index > 0)
        {
            Index--;
            Print(L"\b \b");
            continue;
        }

        if (Index < MaxLen - 1 && Key.UnicodeChar >= L' ')
        {
            Buffer[Index++] = Key.UnicodeChar;
            Print(L"%c", Key.UnicodeChar);
        }
    }
}

VOID Triangle()
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"╔═══════════════════════════════════╗\n");
    Print(L"║            Triangle               ║\n");
    Print(L"╚═══════════════════════════════════╝\n\n");

    Print(L"  Enter size (1-9): ");
    INTN Size = GetNumber();
    if (Size < 1 || Size > 9)
        Size = 5;

    Print(L"\n");
    for (INTN i = 1; i <= Size; i++)
    {
        Print(L"  ");
        for (INTN j = 1; j <= i; j++)
            Print(L"* ");
        Print(L"\n");
    }

    WaitForKey();
}

VOID Diamond()
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"╔═══════════════════════════════════╗\n");
    Print(L"║             Diamond               ║\n");
    Print(L"╚═══════════════════════════════════╝\n\n");

    Print(L"  Enter size (1-9): ");
    INTN Size = GetNumber();
    if (Size < 1 || Size > 9)
        Size = 5;

    Print(L"\n");
    // Upper half
    for (INTN i = 1; i <= Size; i++)
    {
        Print(L"  ");
        for (INTN j = i; j < Size; j++)
            Print(L" ");
        for (INTN j = 1; j <= (2 * i - 1); j++)
            Print(L"*");
        Print(L"\n");
    }
    // Lower half
    for (INTN i = Size - 1; i >= 1; i--)
    {
        Print(L"  ");
        for (INTN j = i; j < Size; j++)
            Print(L" ");
        for (INTN j = 1; j <= (2 * i - 1); j++)
            Print(L"*");
        Print(L"\n");
    }

    WaitForKey();
}

VOID Checkerboard()
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"╔═══════════════════════════════════╗\n");
    Print(L"║           Checkerboard            ║\n");
    Print(L"╚═══════════════════════════════════╝\n\n");

    Print(L"  Enter size (1-9): ");
    INTN Size = GetNumber();
    if (Size < 1 || Size > 9)
        Size = 5;

    Print(L"\n");
    for (INTN i = 0; i < Size; i++)
    {
        Print(L"  ");
        for (INTN j = 0; j < Size; j++)
        {
            if ((i + j) % 2 == 0)
                Print(L"# ");
            else
                Print(L". ");
        }
        Print(L"\n");
    }

    WaitForKey();
}

VOID Pyramid()
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"╔═══════════════════════════════════╗\n");
    Print(L"║             Pyramid               ║\n");
    Print(L"╚═══════════════════════════════════╝\n\n");

    Print(L"  Enter size (1-9): ");
    INTN Size = GetNumber();
    if (Size < 1 || Size > 9)
        Size = 5;

    Print(L"\n");
    for (INTN i = 1; i <= Size; i++)
    {
        Print(L"  ");
        // Spaces for centering
        for (INTN j = i; j < Size; j++)
            Print(L" ");
        // Stars
        for (INTN j = 1; j <= (2 * i - 1); j++)
            Print(L"*");
        Print(L"\n");
    }

    WaitForKey();
}

VOID Spiral()
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"╔═══════════════════════════════════╗\n");
    Print(L"║              Spiral               ║\n");
    Print(L"╚═══════════════════════════════════╝\n\n");

    // Fixed 5x5 spiral — size input makes spiral logic too complex
    INTN Grid[9][9];
    INTN Size = 0;

    Print(L"  Enter size (1-9): ");
    Size = GetNumber();
    if (Size < 1 || Size > 9)
        Size = 5;

    // Initialize grid to 0
    for (INTN i = 0; i < Size; i++)
        for (INTN j = 0; j < Size; j++)
            Grid[i][j] = 0;

    // Fill spiral
    INTN Top = 0, Bottom = Size - 1;
    INTN Left = 0, Right = Size - 1;
    INTN Num = 1;

    while (Top <= Bottom && Left <= Right)
    {
        for (INTN i = Left; i <= Right; i++)
            Grid[Top][i] = Num++;
        Top++;

        for (INTN i = Top; i <= Bottom; i++)
            Grid[i][Right] = Num++;
        Right--;

        if (Top <= Bottom)
        {
            for (INTN i = Right; i >= Left; i--)
                Grid[Bottom][i] = Num++;
            Bottom--;
        }

        if (Left <= Right)
        {
            for (INTN i = Bottom; i >= Top; i--)
                Grid[i][Left] = Num++;
            Left++;
        }
    }

    // Print grid
    Print(L"\n");
    for (INTN i = 0; i < Size; i++)
    {
        Print(L"  ");
        for (INTN j = 0; j < Size; j++)
            Print(L"%3d", Grid[i][j]);
        Print(L"\n");
    }

    Print(L"\n  Numbers fill outward-in spiral path\n");
    WaitForKey();
}

// Pattern Renderer
VOID PatternRenderer()
{
    BOOLEAN Running = TRUE;

    while (Running)
    {
        gST->ConOut->ClearScreen(gST->ConOut);

        Print(L"╔═══════════════════════════════════╗\n");
        Print(L"║       Pattern Renderer            ║\n");
        Print(L"╚═══════════════════════════════════╝\n\n");

        Print(L"  [1] Triangle\n");
        Print(L"  [2] Diamond\n");
        Print(L"  [3] Checkerboard\n");
        Print(L"  [4] Pyramid\n");
        Print(L"  [5] Spiral\n");
        Print(L"  [6] Back to Main Menu\n\n");

        Print(L"Select option: ");

        UINTN choice = GetChoice();
        Print(L"%d\n", choice);

        switch (choice)
        {
        case 1:
            Triangle();
            break;
        case 2:
            Diamond();
            break;
        case 3:
            Checkerboard();
            break;
        case 4:
            Pyramid();
            break;
        case 5:
            Spiral();
            break;
        case 6:
            Running = FALSE;
            break;
        default:
            Print(L"\nInvalid choice!\n");
            WaitForKey();
        }
    }
}

VOID CaesarEncryption()
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"╔═══════════════════════════════════╗\n");
    Print(L"║     Caesar Cipher - Encrypt       ║\n");
    Print(L"╚═══════════════════════════════════╝\n\n");

    CHAR16 Text[100];
    Print(L"  Enter text : ");
    GetString(Text, 100);

    Print(L"  Enter shift (1-9): ");
    INTN Shift = GetNumber();
    Shift = Shift % 26;
    if (Shift < 0)
        Shift += 26;

    Print(L"\n  Encrypted  : ");

    for (UINTN i = 0; Text[i] != L'\0'; i++)
    {
        CHAR16 c = Text[i];
        if (c >= L'A' && c <= L'Z')
            Print(L"%c", (CHAR16)((c - L'A' + Shift) % 26 + L'A'));
        else if (c >= L'a' && c <= L'z')
            Print(L"%c", (CHAR16)((c - L'a' + Shift) % 26 + L'a'));
        else
            Print(L"%c", c);
    }

    Print(L"\n");
    WaitForKey();
}

VOID CaesarDecryption()
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"╔═══════════════════════════════════╗\n");
    Print(L"║     Caesar Cipher - Decrypt       ║\n");
    Print(L"╚═══════════════════════════════════╝\n\n");

    CHAR16 Text[100];
    Print(L"  Enter encrypted text : ");
    GetString(Text, 100);

    Print(L"  Enter shift (1-9): ");
    INTN Shift = GetNumber();
    Shift = Shift % 26;
    if (Shift < 0)
        Shift += 26;

    Print(L"\n  Decrypted  : ");

    for (UINTN i = 0; Text[i] != L'\0'; i++)
    {
        CHAR16 c = Text[i];
        if (c >= L'A' && c <= L'Z')
            Print(L"%c", (CHAR16)((c - L'A' - Shift + 26) % 26 + L'A'));
        else if (c >= L'a' && c <= L'z')
            Print(L"%c", (CHAR16)((c - L'a' - Shift + 26) % 26 + L'a'));
        else
            Print(L"%c", c);
    }

    Print(L"\n");
    WaitForKey();
}

VOID XORCipher()
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"╔═══════════════════════════════════╗\n");
    Print(L"║           XOR Cipher              ║\n");
    Print(L"╚═══════════════════════════════════╝\n\n");

    CHAR16 Text[100];
    Print(L"  Enter text : ");
    GetString(Text, 100);

    Print(L"  Enter key (1-9): ");
    INTN Key = GetNumber();

    Print(L"\n  XOR Result : ");

    for (UINTN i = 0; Text[i] != L'\0'; i++)
    {
        CHAR16 c = (CHAR16)(Text[i] ^ (CHAR16)Key);
        // Only print if printable, else print hex
        if (c >= L' ' && c <= L'~')
            Print(L"%c", c);
        else
            Print(L"[%02X]", (UINT32)c);
    }

    Print(L"\n\n  Note: Apply same key again to decrypt\n");
    WaitForKey();
}

VOID ROT13()
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"╔═══════════════════════════════════╗\n");
    Print(L"║             ROT13                 ║\n");
    Print(L"╚═══════════════════════════════════╝\n\n");

    CHAR16 Text[100];
    Print(L"  Enter text : ");
    GetString(Text, 100);

    Print(L"\n  ROT13 Result : ");

    for (UINTN i = 0; Text[i] != L'\0'; i++)
    {
        CHAR16 c = Text[i];
        if (c >= L'A' && c <= L'Z')
            Print(L"%c", (CHAR16)((c - L'A' + 13) % 26 + L'A'));
        else if (c >= L'a' && c <= L'z')
            Print(L"%c", (CHAR16)((c - L'a' + 13) % 26 + L'a'));
        else
            Print(L"%c", c);
    }

    Print(L"\n\n  Note: Apply ROT13 again to get original\n");
    WaitForKey();
}

// Encryption Tools
VOID EncryptionTools()
{
    BOOLEAN Running = TRUE;

    while (Running)
    {
        gST->ConOut->ClearScreen(gST->ConOut);

        Print(L"╔═══════════════════════════════════╗\n");
        Print(L"║      Encryption Tools             ║\n");
        Print(L"╚═══════════════════════════════════╝\n\n");

        Print(L"  [1] Caesar Cipher (Encrypt)\n");
        Print(L"  [2] Caesar Cipher (Decrypt)\n");
        Print(L"  [3] XOR Cipher\n");
        Print(L"  [4] ROT13\n");
        Print(L"  [5] Back to Main Menu\n\n");

        Print(L"Select option: ");

        UINTN choice = GetChoice();
        Print(L"%d\n", choice);

        switch (choice)
        {
        case 1:
            CaesarEncryption();
            break;
        case 2:
            CaesarDecryption();
            break;
        case 3:
            XORCipher();
            break;
        case 4:
            ROT13();
            break;

        case 5:
            Running = FALSE;
            break;
        default:
            Print(L"\nInvalid choice!\n");
            WaitForKey();
        }
    }
}

// CPU Information
VOID CPUInformation()
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"╔═══════════════════════════════════╗\n");
    Print(L"║         CPU Information           ║\n");
    Print(L"╚═══════════════════════════════════╝\n\n");

    UINT32 Eax, Ebx, Ecx, Edx;

    // Get CPU Vendor String
    AsmCpuid(0x00, &Eax, &Ebx, &Ecx, &Edx);

    // Vendor string is split across EBX, EDX, ECX (in that order)
    CHAR8 Vendor[13];
    *(UINT32 *)(Vendor) = Ebx;
    *(UINT32 *)(Vendor + 4) = Edx;
    *(UINT32 *)(Vendor + 8) = Ecx;
    Vendor[12] = '\0';

    Print(L"  CPU Vendor        : %a\n", Vendor);
    Print(L"  Max CPUID Level   : %d\n", Eax);

    // Get CPU Family, Model, Stepping
    AsmCpuid(0x01, &Eax, &Ebx, &Ecx, &Edx);

    UINT32 Stepping = Eax & 0xF;
    UINT32 Model = (Eax >> 4) & 0xF;
    UINT32 Family = (Eax >> 8) & 0xF;
    UINT32 ExtModel = (Eax >> 16) & 0xF;
    UINT32 ExtFamily = (Eax >> 20) & 0xFF;

    // Actual family and model calculation per Intel spec
    UINT32 DisplayFamily = Family;
    UINT32 DisplayModel = Model;

    if (Family == 0xF)
    {
        DisplayFamily = Family + ExtFamily;
    }
    if (Family == 0x6 || Family == 0xF)
    {
        DisplayModel = (ExtModel << 4) + Model;
    }

    Print(L"  CPU Family        : %d\n", DisplayFamily);
    Print(L"  CPU Model         : %d\n", DisplayModel);
    Print(L"  Stepping          : %d\n", Stepping);

    // Logical processor count
    UINT32 LogicalCPUs = (Ebx >> 16) & 0xFF;
    Print(L"  Logical CPUs      : %d\n", LogicalCPUs);

    // Check for key features via EDX flags
    Print(L"\n  --- CPU Features ---\n");
    Print(L"  FPU               : %s\n", (Edx & (1 << 0)) ? L"Yes" : L"No");
    Print(L"  SSE               : %s\n", (Edx & (1 << 25)) ? L"Yes" : L"No");
    Print(L"  SSE2              : %s\n", (Edx & (1 << 26)) ? L"Yes" : L"No");
    Print(L"  HTT               : %s\n", (Edx & (1 << 28)) ? L"Yes" : L"No");

    // Check SSE3, SSE4 via ECX
    Print(L"  SSE3              : %s\n", (Ecx & (1 << 0)) ? L"Yes" : L"No");
    Print(L"  SSE4.1            : %s\n", (Ecx & (1 << 19)) ? L"Yes" : L"No");
    Print(L"  SSE4.2            : %s\n", (Ecx & (1 << 20)) ? L"Yes" : L"No");
    Print(L"  AES               : %s\n", (Ecx & (1 << 25)) ? L"Yes" : L"No");
    Print(L"  AVX               : %s\n", (Ecx & (1 << 28)) ? L"Yes" : L"No");

    WaitForKey();
}

// Memory Information
VOID MemoryInformation()
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"╔═══════════════════════════════════╗\n");
    Print(L"║       Memory Information          ║\n");
    Print(L"╚═══════════════════════════════════╝\n\n");

    EFI_STATUS Status;
    UINTN MemoryMapSize = 0;
    EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;
    UINTN MapKey;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;

    // First call to get required buffer size
    Status = gBS->GetMemoryMap(
        &MemoryMapSize, MemoryMap, &MapKey,
        &DescriptorSize, &DescriptorVersion);

    // Add extra space for safety
    MemoryMapSize += 2 * DescriptorSize;

    // Allocate pool
    Status = gBS->AllocatePool(
        EfiLoaderData, MemoryMapSize, (VOID **)&MemoryMap);

    if (EFI_ERROR(Status))
    {
        Print(L"  Failed to allocate memory.\n");
        WaitForKey();
        return;
    }

    // Second call to actually get the map
    Status = gBS->GetMemoryMap(
        &MemoryMapSize, MemoryMap, &MapKey,
        &DescriptorSize, &DescriptorVersion);

    if (EFI_ERROR(Status))
    {
        Print(L"  Failed to get memory map.\n");
        gBS->FreePool(MemoryMap);
        WaitForKey();
        return;
    }

    // Count entries and calculate totals
    UINTN EntryCount = MemoryMapSize / DescriptorSize;
    UINTN ConventionalPages = 0;
    UINTN ReservedPages = 0;
    UINTN RuntimePages = 0;
    UINTN ACPIPages = 0;
    UINTN OtherPages = 0;
    UINTN TotalPages = 0;

    for (UINTN i = 0; i < EntryCount; i++)
    {
        EFI_MEMORY_DESCRIPTOR *Desc = (EFI_MEMORY_DESCRIPTOR *)((UINT8 *)MemoryMap + i * DescriptorSize);

        TotalPages += Desc->NumberOfPages;

        switch (Desc->Type)
        {
        case EfiConventionalMemory:
            ConventionalPages += Desc->NumberOfPages;
            break;
        case EfiReservedMemoryType:
            ReservedPages += Desc->NumberOfPages;
            break;
        case EfiRuntimeServicesCode:
        case EfiRuntimeServicesData:
            RuntimePages += Desc->NumberOfPages;
            break;
        case EfiACPIReclaimMemory:
        case EfiACPIMemoryNVS:
            ACPIPages += Desc->NumberOfPages;
            break;
        default:
            OtherPages += Desc->NumberOfPages;
            break;
        }
    }

    // 1 page = 4096 bytes = 4KB
    Print(L"  Memory Map Entries  : %d\n", EntryCount);
    Print(L"  Total Memory        : %d MB\n",
          (TotalPages * 4096) / (1024 * 1024));
    Print(L"  Conventional (Free) : %d MB\n",
          (ConventionalPages * 4096) / (1024 * 1024));
    Print(L"  Reserved            : %d KB\n",
          (ReservedPages * 4096) / 1024);
    Print(L"  Runtime Services    : %d KB\n",
          (RuntimePages * 4096) / 1024);
    Print(L"  ACPI Memory         : %d KB\n",
          (ACPIPages * 4096) / 1024);
    Print(L"  Other/System Used   : %d MB\n",
          (OtherPages * 4096) / (1024 * 1024));
    Print(L"  Descriptor Version  : %d\n", DescriptorVersion);
    Print(L"  Descriptor Size     : %d bytes\n", DescriptorSize);

    gBS->FreePool(MemoryMap);

    WaitForKey();
}

// UEFI firmware information
VOID UEFIFirmwareInformation()
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"╔═══════════════════════════════════╗\n");
    Print(L"║     UEFI Firmware Information     ║\n");
    Print(L"╚═══════════════════════════════════╝\n\n");

    // Firmware Vendor
    Print(L"  Firmware Vendor   : %s\n", gST->FirmwareVendor);

    // Firmware Revision
    UINT32 Rev = gST->FirmwareRevision;
    Print(L"  Firmware Revision : %d.%d\n", (Rev >> 16), (Rev & 0xFFFF));

    // UEFI Spec Version
    UINT32 Spec = gST->Hdr.Revision;
    Print(L"  UEFI Spec Version : %d.%d\n", (Spec >> 16), (Spec & 0xFFFF));

    // Header Size
    Print(L"  Header Size       : %d bytes\n", gST->Hdr.HeaderSize);

    WaitForKey();
}

// system time & date function
VOID SystemTimeDate()
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"╔═══════════════════════════════════╗\n");
    Print(L"║       System Time & Date          ║\n");
    Print(L"╚═══════════════════════════════════╝\n\n");

    EFI_TIME Time;
    EFI_TIME_CAPABILITIES Capabilities;

    EFI_STATUS Status = gRT->GetTime(&Time, &Capabilities);

    if (EFI_ERROR(Status))
    {
        Print(L"  Failed to get time. Status: %d\n", Status);
    }
    else
    {
        Print(L"  Date     : %02d/%02d/%04d\n",
              Time.Day, Time.Month, Time.Year);
        Print(L"  Time     : %02d:%02d:%02d\n",
              Time.Hour, Time.Minute, Time.Second);
        Print(L"  Timezone : UTC%+d minutes\n", Time.TimeZone);
    }

    WaitForKey();
}

// system information menu
VOID SystemInformation()
{
    BOOLEAN Running = TRUE;

    while (Running)
    {
        gST->ConOut->ClearScreen(gST->ConOut);

        Print(L"╔═══════════════════════════════════╗\n");
        Print(L"║      System Information           ║\n");
        Print(L"╚═══════════════════════════════════╝\n\n");

        Print(L"  [1] CPU Information\n");
        Print(L"  [2] Memory Information\n");
        Print(L"  [3] UEFI Firmware Information\n");
        Print(L"  [4] System Time & Date\n");
        Print(L"  [5] Back to Main Menu\n\n");

        Print(L"Select option: ");

        UINTN choice = GetChoice();
        Print(L"%d\n", choice);

        switch (choice)
        {
        case 1:
            CPUInformation();
            break;
        case 2:
            MemoryInformation();
            break;
        case 3:
            UEFIFirmwareInformation();
            break;
        case 4:
            SystemTimeDate();
            break;
        case 5:
            Running = FALSE;
            break;
        default:
            Print(L"\nInvalid choice!\n");
            WaitForKey();
        }
    }
}

// Bubble Sort
VOID BubbleSort()
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"╔═══════════════════════════════════╗\n");
    Print(L"║    BUBBLE SORT ALGORITHM          ║\n");
    Print(L"╚═══════════════════════════════════╝\n\n");

    Print(L"Enter number of elements (max 20): ");
    INTN n = GetNumber();

    if (n <= 0 || n > 20)
    {
        Print(L"Invalid size! Must be between 1 and 20.\n");
        WaitForKey();
        return;
    }

    INTN arr[20];
    Print(L"\nEnter %d numbers:\n", n);
    for (INTN i = 0; i < n; i++)
    {
        Print(L"Element %d: ", i + 1);
        arr[i] = GetNumber();
    }

    Print(L"\n=== Bubble Sort Process ===\n\n");
    Print(L"Starting array: [ ");
    for (INTN i = 0; i < n; i++)
    {
        Print(L"%d", arr[i]);
        if (i < n - 1)
            Print(L", ");
    }
    Print(L" ]\n");

    INTN step = 1;
    for (INTN i = 0; i < n - 1; i++)
    {
        BOOLEAN swapped = FALSE;
        Print(L"\nPass %d:\n", step++);

        for (INTN j = 0; j < n - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                Print(L"  Swapping %d and %d\n", arr[j], arr[j + 1]);
                INTN temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = TRUE;
            }
        }

        if (!swapped)
        {
            Print(L"  No swaps - array is sorted!\n");
            break;
        }

        Print(L"  After pass: [ ");
        for (INTN k = 0; k < n; k++)
        {
            Print(L"%d", arr[k]);
            if (k < n - 1)
                Print(L", ");
        }
        Print(L" ]\n");
    }

    Print(L"\nFinal sorted array: [ ");
    for (INTN i = 0; i < n; i++)
    {
        Print(L"%d", arr[i]);
        if (i < n - 1)
            Print(L", ");
    }
    Print(L" ]\n");

    WaitForKey();
}

// Selection Sort
VOID SelectionSort()
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"╔═══════════════════════════════════╗\n");
    Print(L"║    SELECTION SORT ALGORITHM       ║\n");
    Print(L"╚═══════════════════════════════════╝\n\n");

    Print(L"Enter number of elements (max 20): ");
    INTN n = GetNumber();

    if (n <= 0 || n > 20)
    {
        Print(L"Invalid size! Must be between 1 and 20.\n");
        WaitForKey();
        return;
    }

    INTN arr[20];
    Print(L"\nEnter %d numbers:\n", n);
    for (INTN i = 0; i < n; i++)
    {
        Print(L"Element %d: ", i + 1);
        arr[i] = GetNumber();
    }

    Print(L"\n=== Selection Sort Process ===\n\n");
    Print(L"Starting array: [ ");
    for (INTN i = 0; i < n; i++)
    {
        Print(L"%d", arr[i]);
        if (i < n - 1)
            Print(L", ");
    }
    Print(L" ]\n");

    for (INTN i = 0; i < n - 1; i++)
    {
        INTN minIdx = i;

        for (INTN j = i + 1; j < n; j++)
        {
            if (arr[j] < arr[minIdx])
            {
                minIdx = j;
            }
        }

        if (minIdx != i)
        {
            Print(L"\nPass %d: Smallest is %d, swapping with %d\n",
                  i + 1, arr[minIdx], arr[i]);
            INTN temp = arr[minIdx];
            arr[minIdx] = arr[i];
            arr[i] = temp;
        }
        else
        {
            Print(L"\nPass %d: %d already in correct position\n",
                  i + 1, arr[i]);
        }

        Print(L"After pass: [ ");
        for (INTN k = 0; k < n; k++)
        {
            Print(L"%d", arr[k]);
            if (k < n - 1)
                Print(L", ");
        }
        Print(L" ]\n");
    }

    Print(L"\nFinal sorted array: [ ");
    for (INTN i = 0; i < n; i++)
    {
        Print(L"%d", arr[i]);
        if (i < n - 1)
            Print(L", ");
    }
    Print(L" ]\n");

    WaitForKey();
}

// Insertion Sort
VOID InsertionSort()
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"╔═══════════════════════════════════╗\n");
    Print(L"║    INSERTION SORT ALGORITHM       ║\n");
    Print(L"╚═══════════════════════════════════╝\n\n");

    Print(L"Enter number of elements (max 20): ");
    INTN n = GetNumber();

    if (n <= 0 || n > 20)
    {
        Print(L"Invalid size! Must be between 1 and 20.\n");
        WaitForKey();
        return;
    }

    INTN arr[20];
    Print(L"\nEnter %d numbers:\n", n);
    for (INTN i = 0; i < n; i++)
    {
        Print(L"Element %d: ", i + 1);
        arr[i] = GetNumber();
    }

    Print(L"\n=== Insertion Sort Process ===\n\n");
    Print(L"Starting array: [ ");
    for (INTN i = 0; i < n; i++)
    {
        Print(L"%d", arr[i]);
        if (i < n - 1)
            Print(L", ");
    }
    Print(L" ]\n");

    for (INTN i = 1; i < n; i++)
    {
        INTN key = arr[i];
        INTN j = i - 1;

        Print(L"\nInserting %d into sorted portion:\n", key);

        while (j >= 0 && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;

        Print(L"Array after inserting %d: [ ", key);
        for (INTN k = 0; k < n; k++)
        {
            Print(L"%d", arr[k]);
            if (k < n - 1)
                Print(L", ");
        }
        Print(L" ]\n");
    }

    Print(L"\nFinal sorted array: [ ");
    for (INTN i = 0; i < n; i++)
    {
        Print(L"%d", arr[i]);
        if (i < n - 1)
            Print(L", ");
    }
    Print(L" ]\n");

    WaitForKey();
}

// Heap Sort
VOID Heapify(INTN arr[], INTN n, INTN i)
{
    INTN largest = i;
    INTN left = 2 * i + 1;
    INTN right = 2 * i + 2;

    if (left < n && arr[left] > arr[largest])
        largest = left;
    if (right < n && arr[right] > arr[largest])
        largest = right;

    if (largest != i)
    {
        INTN temp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = temp;
        Heapify(arr, n, largest);
    }
}

VOID HeapSort()
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"╔═══════════════════════════════════╗\n");
    Print(L"║      HEAP SORT ALGORITHM          ║\n");
    Print(L"╚═══════════════════════════════════╝\n\n");

    Print(L"Enter number of elements (max 20): ");
    INTN n = GetNumber();

    if (n <= 0 || n > 20)
    {
        Print(L"Invalid size! Must be between 1 and 20.\n");
        WaitForKey();
        return;
    }

    INTN arr[20];
    Print(L"\nEnter %d numbers:\n", n);
    for (INTN i = 0; i < n; i++)
    {
        Print(L"Element %d: ", i + 1);
        arr[i] = GetNumber();
    }

    Print(L"\n=== Heap Sort Process ===\n\n");
    Print(L"Starting array: [ ");
    for (INTN i = 0; i < n; i++)
    {
        Print(L"%d", arr[i]);
        if (i < n - 1)
            Print(L", ");
    }
    Print(L" ]\n");

    // Build max heap
    for (INTN i = n / 2 - 1; i >= 0; i--)
        Heapify(arr, n, i);

    Print(L"After building heap: [ ");
    for (INTN i = 0; i < n; i++)
    {
        Print(L"%d", arr[i]);
        if (i < n - 1)
            Print(L", ");
    }
    Print(L" ]\n");

    // Extract elements one by one
    for (INTN i = n - 1; i > 0; i--)
    {
        INTN temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

        Print(L"\nPlaced %d at position %d\n", arr[i], i + 1);

        Heapify(arr, i, 0);

        Print(L"Array now: [ ");
        for (INTN k = 0; k < n; k++)
        {
            Print(L"%d", arr[k]);
            if (k < n - 1)
                Print(L", ");
        }
        Print(L" ]\n");
    }

    Print(L"\nFinal sorted array: [ ");
    for (INTN i = 0; i < n; i++)
    {
        Print(L"%d", arr[i]);
        if (i < n - 1)
            Print(L", ");
    }
    Print(L" ]\n");

    WaitForKey();
}

// Qucik Sort
VOID MyQucikSort()
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"╔═══════════════════════════════════╗\n");
    Print(L"║      QUICK SORT ALGORITHM         ║\n");
    Print(L"╚═══════════════════════════════════╝\n\n");

    Print(L"Enter number of elements (max 20): ");
    INTN n = GetNumber();

    if (n <= 0 || n > 20)
    {
        Print(L"Invalid size! Must be between 1 and 20.\n");
        WaitForKey();
        return;
    }

    INTN arr[20];
    Print(L"\nEnter %d numbers:\n", n);
    for (INTN i = 0; i < n; i++)
    {
        Print(L"Element %d: ", i + 1);
        arr[i] = GetNumber();
    }

    Print(L"\n=== Quick Sort Process ===\n\n");
    Print(L"Starting array: [ ");
    for (INTN i = 0; i < n; i++)
    {
        Print(L"%d", arr[i]);
        if (i < n - 1)
            Print(L", ");
    }
    Print(L" ]\n\n");

    // Iterative quick sort using stack
    INTN stack[20];
    INTN top = -1;
    stack[++top] = 0;
    stack[++top] = n - 1;

    while (top >= 0)
    {
        INTN high = stack[top--];
        INTN low = stack[top--];

        // Partition
        INTN pivot = arr[high];
        INTN i = low - 1;

        for (INTN j = low; j < high; j++)
        {
            if (arr[j] <= pivot)
            {
                i++;
                INTN temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }

        INTN temp = arr[i + 1];
        arr[i + 1] = arr[high];
        arr[high] = temp;

        INTN pi = i + 1;

        Print(L"Pivot %d placed at position %d\n", pivot, pi + 1);
        Print(L"Array: [ ");
        for (INTN k = 0; k < n; k++)
        {
            Print(L"%d", arr[k]);
            if (k < n - 1)
                Print(L", ");
        }
        Print(L" ]\n\n");

        if (pi - 1 > low)
        {
            stack[++top] = low;
            stack[++top] = pi - 1;
        }
        if (pi + 1 < high)
        {
            stack[++top] = pi + 1;
            stack[++top] = high;
        }
    }

    Print(L"\nFinal sorted array: [ ");
    for (INTN i = 0; i < n; i++)
    {
        Print(L"%d", arr[i]);
        if (i < n - 1)
            Print(L", ");
    }
    Print(L" ]\n");

    WaitForKey();
}

// Merge Sort
VOID MergeSort()
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"╔═══════════════════════════════════╗\n");
    Print(L"║      MERGE SORT ALGORITHM         ║\n");
    Print(L"╚═══════════════════════════════════╝\n\n");

    Print(L"Enter number of elements (max 20): ");
    INTN n = GetNumber();

    if (n <= 0 || n > 20)
    {
        Print(L"Invalid size! Must be between 1 and 20.\n");
        WaitForKey();
        return;
    }

    INTN arr[20];
    Print(L"\nEnter %d numbers:\n", n);
    for (INTN i = 0; i < n; i++)
    {
        Print(L"Element %d: ", i + 1);
        arr[i] = GetNumber();
    }

    Print(L"\n=== Merge Sort Process ===\n\n");
    Print(L"Starting array: [ ");
    for (INTN i = 0; i < n; i++)
    {
        Print(L"%d", arr[i]);
        if (i < n - 1)
            Print(L", ");
    }
    Print(L" ]\n\n");

    INTN temp[20];
    INTN width;

    for (width = 1; width < n; width = width * 2)
    {
        INTN left;
        for (left = 0; left < n; left += 2 * width)
        {
            INTN mid = left + width;
            INTN right = left + 2 * width;

            if (mid > n)
                mid = n;
            if (right > n)
                right = n;

            // Merge arr[left..mid] and arr[mid..right]
            INTN i = left;
            INTN j = mid;
            INTN k = left;

            while (i < mid && j < right)
            {
                if (arr[i] <= arr[j])
                {
                    temp[k++] = arr[i++];
                }
                else
                {
                    temp[k++] = arr[j++];
                }
            }
            while (i < mid)
                temp[k++] = arr[i++];
            while (j < right)
                temp[k++] = arr[j++];
            for (i = left; i < right; i++)
                arr[i] = temp[i];
        }

        Print(L"After merging groups of %d: [ ", width);
        for (INTN i = 0; i < n; i++)
        {
            Print(L"%d", arr[i]);
            if (i < n - 1)
                Print(L", ");
        }
        Print(L" ]\n");
    }

    Print(L"\nFinal sorted array: [ ");
    for (INTN i = 0; i < n; i++)
    {
        Print(L"%d", arr[i]);
        if (i < n - 1)
            Print(L", ");
    }
    Print(L" ]\n");

    WaitForKey();
}
// Sorting Algorithm Menu
VOID SortingAlgorithm()
{
    BOOLEAN Running = TRUE;

    while (Running)
    {
        gST->ConOut->ClearScreen(gST->ConOut);

        Print(L"╔═══════════════════════════════════╗\n");
        Print(L"║      SORTING ALGORITHMS           ║\n");
        Print(L"╚═══════════════════════════════════╝\n\n");

        Print(L"  [1] Bubble Sort\n");
        Print(L"  [2] Selection Sort\n");
        Print(L"  [3] Insertion Sort\n");
        Print(L"  [4] Heap Sort\n");
        Print(L"  [5] Qucik Sort\n");
        Print(L"  [6] Merge Sort\n");
        Print(L"  [7] Back to Main Menu\n\n");

        Print(L"Select option: ");

        UINTN choice = GetChoice();
        Print(L"%d\n", choice);

        switch (choice)
        {
        case 1:
            BubbleSort();
            break;
        case 2:
            SelectionSort();
            break;
        case 3:
            InsertionSort();
            break;
        case 4:
            HeapSort();
            break;
        case 5:
            MyQucikSort();
            break;
        case 6:
            MergeSort();
            break;
        case 7:
            Running = FALSE;
            break;
        default:
            Print(L"\nInvalid choice!\n");
            WaitForKey();
        }
    }
}

// Basic Calculator
VOID BasicCalculator()
{
    BOOLEAN Running = TRUE;

    while (Running)
    {
        INTN numbers[20];
        UINTN count = 0;
        INTN result = 0;

        gST->ConOut->ClearScreen(gST->ConOut);

        Print(L"╔═══════════════════════════════════╗\n");
        Print(L"║     BASIC CALCULATOR              ║\n");
        Print(L"╚═══════════════════════════════════╝\n\n");

        Print(L"Select Operation:\n");
        Print(L"[1] Addition (+)\n");
        Print(L"[2] Subtraction (-)\n");
        Print(L"[3] Multiplication (×)\n");
        Print(L"[4] Division (÷)\n");
        Print(L"[5] Back to Calculator Menu\n");
        Print(L"\nYour choice: ");

        UINTN op = GetChoice();
        Print(L"%d\n\n", op);

        if (op == 5)
        {
            Running = FALSE;
            continue;
        }

        if (op < 1 || op > 4)
        {
            Print(L"Invalid operation!\n");
            WaitForKey();
            continue;
        }

        Print(L"────────────────────────────────────\n");

        if (op == 4)
        {
            Print(L"Enter exactly 2 numbers for DIVISION\n\n");
            Print(L"Dividend: ");
            numbers[0] = GetNumber();
            Print(L"Divisor: ");
            numbers[1] = GetNumber();
            count = 2;
        }
        else
        {
            Print(L"Enter numbers (enter 0 to finish):\n\n");
            while (count < 20)
            {
                Print(L"Number %d: ", count + 1);
                INTN num = GetNumber();

                if (num == 0 && count > 0)
                {
                    break;
                }

                numbers[count++] = num;
            }
        }

        if (count == 0)
        {
            Print(L"\nNo numbers entered!\n");
            WaitForKey();
            continue;
        }

        Print(L"\n────────────────────────────────────\n");
        Print(L"Calculation:\n\n");

        switch (op)
        {
        case 1:
            result = 0;
            Print(L"  ");
            for (UINTN i = 0; i < count; i++)
            {
                result += numbers[i];
                Print(L"%d", numbers[i]);
                if (i < count - 1)
                    Print(L" + ");
            }
            Print(L" = %d\n", result);
            break;

        case 2:
            result = numbers[0];
            Print(L"  %d", numbers[0]);
            for (UINTN i = 1; i < count; i++)
            {
                result -= numbers[i];
                Print(L" - %d", numbers[i]);
            }
            Print(L" = %d\n", result);
            break;

        case 3:
            result = 1;
            Print(L"  ");
            for (UINTN i = 0; i < count; i++)
            {
                result *= numbers[i];
                Print(L"%d", numbers[i]);
                if (i < count - 1)
                    Print(L" × ");
            }
            Print(L" = %d\n", result);
            break;

        case 4:
            if (numbers[1] != 0)
            {
                result = numbers[0] / numbers[1];
                INTN remainder = numbers[0] % numbers[1];
                Print(L"  %d ÷ %d = %d", numbers[0], numbers[1], result);
                if (remainder != 0)
                {
                    Print(L" (remainder: %d)", remainder);
                }
                Print(L"\n");
            }
            else
            {
                Print(L"  Error: Division by zero!\n");
            }
            break;
        }

        Print(L"────────────────────────────────────\n");
        WaitForKey();
    }
}

// Scientific Calculator
VOID ScientificCalculator()
{
    BOOLEAN Running = TRUE;

    while (Running)
    {
        gST->ConOut->ClearScreen(gST->ConOut);

        Print(L"╔═══════════════════════════════════╗\n");
        Print(L"║   SCIENTIFIC CALCULATOR           ║\n");
        Print(L"╚═══════════════════════════════════╝\n\n");

        Print(L"Functions:\n");
        Print(L"[1] Square Root (√)\n");
        Print(L"[2] Power (x^y)\n");
        Print(L"[3] Factorial (n!)\n");
        Print(L"[4] Square (x²)\n");
        Print(L"[5] Cube (x³)\n");
        Print(L"[6] Back to Calculator Menu\n");
        Print(L"\nSelect function: ");

        UINTN func = GetChoice();
        Print(L"%d\n\n", func);

        if (func == 6)
        {
            Running = FALSE;
            continue;
        }

        Print(L"────────────────────────────────────\n");

        switch (func)
        {
        case 1:
        {
            Print(L"Enter number: ");
            INTN num = GetNumber();
            INTN result = SquareRoot(num);
            if (result >= 0)
            {
                Print(L"\n  √%d = %d (approx)\n", num, result);
            }
            else
            {
                Print(L"\n  Error: Cannot find square root of negative number!\n");
            }
            break;
        }
        case 2:
        {
            Print(L"Enter base: ");
            INTN base = GetNumber();
            Print(L"Enter exponent: ");
            INTN exp = GetNumber();
            if (exp >= 0)
            {
                Print(L"\n  %d^%d = %d\n", base, exp, Power(base, exp));
            }
            else
            {
                Print(L"\n  Error: Negative exponents not supported!\n");
            }
            break;
        }
        case 3:
        {
            Print(L"Enter number (0-20): ");
            INTN num = GetNumber();
            if (num >= 0 && num <= 20)
            {
                Print(L"\n  %d! = %d\n", num, Factorial(num));
            }
            else
            {
                Print(L"\n  Error: Number must be between 0 and 20!\n");
            }
            break;
        }
        case 4:
        {
            Print(L"Enter number: ");
            INTN num = GetNumber();
            Print(L"\n  %d² = %d\n", num, num * num);
            break;
        }
        case 5:
        {
            Print(L"Enter number: ");
            INTN num = GetNumber();
            Print(L"\n  %d³ = %d\n", num, num * num * num);
            break;
        }
        default:
            Print(L"  Invalid function!\n");
        }

        Print(L"────────────────────────────────────\n");
        WaitForKey();
    }
}

// Currency Exchange
VOID CurrencyExchange()
{
    BOOLEAN Running = TRUE;

    while (Running)
    {
        gST->ConOut->ClearScreen(gST->ConOut);

        Print(L"╔═══════════════════════════════════╗\n");
        Print(L"║      CURRENCY EXCHANGE            ║\n");
        Print(L"╚═══════════════════════════════════╝\n\n");

        Print(L"Available Conversions:\n");
        Print(L"[1] USD to INR  (Rate: 83.25)\n");
        Print(L"[2] EUR to INR  (Rate: 90.50)\n");
        Print(L"[3] GBP to INR  (Rate: 105.75)\n");
        Print(L"[4] JPY to INR  (Rate: 0.56)\n");
        Print(L"[5] AUD to INR  (Rate: 54.80)\n");
        Print(L"[6] Back to Calculator Menu\n");
        Print(L"\nSelect conversion: ");

        UINTN choice = GetChoice();
        Print(L"%d\n\n", choice);

        if (choice == 6)
        {
            Running = FALSE;
            continue;
        }

        Print(L"Enter amount: ");
        INTN amount = GetNumber();

        Print(L"\n────────────────────────────────────\n");

        INTN result = 0;

        switch (choice)
        {
        case 1:
            result = amount * 8325 / 100;
            Print(L"  $%d USD = ₹%d INR\n", amount, result);
            Print(L"  (Rate: 1 USD = 83.25 INR)\n");
            break;
        case 2:
            result = amount * 9050 / 100;
            Print(L"  €%d EUR = ₹%d INR\n", amount, result);
            Print(L"  (Rate: 1 EUR = 90.50 INR)\n");
            break;
        case 3:
            result = amount * 10575 / 100;
            Print(L"  £%d GBP = ₹%d INR\n", amount, result);
            Print(L"  (Rate: 1 GBP = 105.75 INR)\n");
            break;
        case 4:
            result = amount * 56 / 100;
            Print(L"  ¥%d JPY = ₹%d INR\n", amount, result);
            Print(L"  (Rate: 1 JPY = 0.56 INR)\n");
            break;
        case 5:
            result = amount * 5480 / 100;
            Print(L"  $%d AUD = ₹%d INR\n", amount, result);
            Print(L"  (Rate: 1 AUD = 54.80 INR)\n");
            break;
        default:
            Print(L"  Invalid selection!\n");
        }

        Print(L"────────────────────────────────────\n");
        WaitForKey();
    }
}

// Calculator Menu
VOID CalculatorMenu()
{
    BOOLEAN Running = TRUE;

    while (Running)
    {
        gST->ConOut->ClearScreen(gST->ConOut);

        Print(L"╔═══════════════════════════════════╗\n");
        Print(L"║      CALCULATOR SUITE             ║\n");
        Print(L"╚═══════════════════════════════════╝\n\n");

        Print(L"  [1] Basic Calculator\n");
        Print(L"  [2] Scientific Calculator\n");
        Print(L"  [3] Currency Exchange\n");
        Print(L"  [4] Back to Main Menu\n\n");

        Print(L"Select option: ");

        UINTN choice = GetChoice();
        Print(L"%d\n", choice);

        switch (choice)
        {
        case 1:
            BasicCalculator();
            break;
        case 2:
            ScientificCalculator();
            break;
        case 3:
            CurrencyExchange();
            break;
        case 4:
            Running = FALSE;
            break;
        default:
            Print(L"\nInvalid choice!\n");
            WaitForKey();
        }
    }
}
// ============================================================
//   GNSS TELEMETRY PARSER - Menu 6
// ============================================================

// Struct to hold parsed data for ONE constellation
typedef struct
{
    CHAR8 Talker[8];   // "GPGGA", "GLGGA", "BDGGA", "GAGGA"
    CHAR8 UTCTime[12]; // "123519.00"
    INTN LatDeg;       // 48
    INTN LatMin;       // 11  (integer part of minutes)
    INTN LatSec;       // 73  (decimal part scaled x100)
    CHAR8 LatDir;      // 'N' or 'S'
    INTN LonDeg;       // 11
    INTN LonMin;       // 31
    INTN LonSec;       // 00
    CHAR8 LonDir;      // 'E' or 'W'
    INTN AltInt;       // 545  (integer part)
    INTN AltDec;       // 4    (decimal part)
    INTN Satellites;   // 08
    INTN FixQuality;   // 0=No Fix, 1=GPS, 2=DGPS
    INTN HdopInt;      // 0  (integer part of HDOP)
    INTN HdopDec;      // 9  (decimal part)
    BOOLEAN Valid;     // TRUE if successfully parsed
} GNSS_DATA;

// Convert NMEA coordinate string to degrees and minutes
// Input:  "4807.038"  → LatDeg=48, LatMin=7, LatSec=3 (scaled)
// Works for both Latitude (DDMM.mmm) and Longitude (DDDMM.mmm)
VOID ConvertNMEACoord(
    CHAR8 *RawCoord, // e.g. "4807.038"
    BOOLEAN IsLon,   // FALSE=Latitude, TRUE=Longitude
    INTN *Deg,
    INTN *Min,
    INTN *Sec) // decimal part of minutes scaled x100
{
    (VOID)IsLon;
    *Deg = 0;
    *Min = 0;
    *Sec = 0;

    if (RawCoord == NULL || RawCoord[0] == '\0')
        return;

    // Find the dot position
    INTN DotPos = -1;
    for (INTN i = 0; RawCoord[i] != '\0'; i++)
    {
        if (RawCoord[i] == '.')
        {
            DotPos = i;
            break;
        }
    }

    if (DotPos < 0)
        return;

    // Latitude:  DDMM.mmm  → degree digits = DotPos - 2
    // Longitude: DDDMM.mmm → degree digits = DotPos - 2
    // Both share the same rule: last 2 digits before dot = minutes
    INTN DegDigits = DotPos - 2;
    if (DegDigits < 1)
        return;

    // Parse degrees
    *Deg = 0;
    for (INTN i = 0; i < DegDigits; i++)
    {
        if (RawCoord[i] >= '0' && RawCoord[i] <= '9')
            *Deg = *Deg * 10 + (RawCoord[i] - '0');
    }

    // Parse minutes (2 digits before dot)
    INTN M = 0;
    for (INTN i = DegDigits; i < DotPos; i++)
    {
        if (RawCoord[i] >= '0' && RawCoord[i] <= '9')
            M = M * 10 + (RawCoord[i] - '0');
    }
    *Min = M;

    // Parse decimal part of minutes (up to 2 digits after dot)
    INTN S = 0;
    INTN Count = 0;
    for (INTN i = DotPos + 1; RawCoord[i] != '\0' && Count < 2; i++, Count++)
    {
        if (RawCoord[i] >= '0' && RawCoord[i] <= '9')
            S = S * 10 + (RawCoord[i] - '0');
    }
    // Pad to 2 digits if only 1 decimal digit found
    if (Count == 1)
        S *= 10;

    *Sec = S;
}

// Parse a raw $xxGGA sentence into a GNSS_DATA struct
// Handles: $GPGGA, $GLGGA, $BDGGA, $GAGGA
VOID ParseGGASentence(CHAR8 *Line, GNSS_DATA *Data)
{
    Data->Valid = FALSE;

    if (Line == NULL || Line[0] == '\0')
        return;

    // Must start with '$'
    if (Line[0] != '$')
        return;

    // Check talker ID and sentence type
    // Line[1..2] = talker (GP, GL, BD, GA)
    // Line[3..5] = sentence type must be "GGA"
    if (Line[3] != 'G' || Line[4] != 'G' || Line[5] != 'A')
        return;

    // Store talker string e.g. "GPGGA"
    for (INTN i = 0; i < 5 && Line[i + 1] != '\0'; i++)
        Data->Talker[i] = Line[i + 1];
    Data->Talker[5] = '\0';

    // Split by comma — collect up to 15 field pointers
    CHAR8 *Fields[15];
    INTN FieldCount = 0;

    // We work on a local copy so we can insert nulls
    CHAR8 LocalBuf[128];
    INTN Len = 0;
    while (Line[Len] != '\0' && Len < 127)
    {
        LocalBuf[Len] = Line[Len];
        Len++;
    }
    LocalBuf[Len] = '\0';

    // Point fields[0] at start, then replace commas with '\0'
    Fields[FieldCount++] = LocalBuf;
    for (INTN i = 0; i < Len && FieldCount < 15; i++)
    {
        if (LocalBuf[i] == ',')
        {
            LocalBuf[i] = '\0';
            Fields[FieldCount++] = &LocalBuf[i + 1];
        }
        // Stop at checksum '*'
        if (LocalBuf[i] == '*')
        {
            LocalBuf[i] = '\0';
            break;
        }
    }

    // Need at least 10 fields for a valid GGA
    if (FieldCount < 10)
        return;

    // Field 1 — UTC Time e.g. "123519.00"
    INTN Ti = 0;
    while (Fields[1][Ti] != '\0' && Ti < 11)
    {
        Data->UTCTime[Ti] = Fields[1][Ti];
        Ti++;
    }
    Data->UTCTime[Ti] = '\0';

    // Field 2,3 — Latitude
    ConvertNMEACoord(
        Fields[2], FALSE,
        &Data->LatDeg, &Data->LatMin, &Data->LatSec);
    Data->LatDir = (Fields[3][0] != '\0') ? Fields[3][0] : 'N';

    // Field 4,5 — Longitude
    ConvertNMEACoord(
        Fields[4], TRUE,
        &Data->LonDeg, &Data->LonMin, &Data->LonSec);
    Data->LonDir = (Fields[5][0] != '\0') ? Fields[5][0] : 'E';

    // Field 6 — Fix Quality
    Data->FixQuality = 0;
    if (Fields[6][0] >= '0' && Fields[6][0] <= '9')
        Data->FixQuality = Fields[6][0] - '0';

    // Field 7 — Satellites in use
    Data->Satellites = 0;
    for (INTN i = 0; Fields[7][i] >= '0' && Fields[7][i] <= '9'; i++)
        Data->Satellites = Data->Satellites * 10 + (Fields[7][i] - '0');

    // Field 8 — HDOP  e.g. "0.9"
    Data->HdopInt = 0;
    Data->HdopDec = 0;
    {
        INTN DotSeen = 0;
        for (INTN i = 0; Fields[8][i] != '\0'; i++)
        {
            if (Fields[8][i] == '.')
            {
                DotSeen = 1;
                continue;
            }
            if (Fields[8][i] >= '0' && Fields[8][i] <= '9')
            {
                if (!DotSeen)
                    Data->HdopInt = Data->HdopInt * 10 + (Fields[8][i] - '0');
                else
                    Data->HdopDec = Fields[8][i] - '0';
            }
        }
    }

    // Field 9 — Altitude MSL  e.g. "545.4"
    Data->AltInt = 0;
    Data->AltDec = 0;
    {
        INTN DotSeen = 0;
        for (INTN i = 0; Fields[9][i] != '\0'; i++)
        {
            if (Fields[9][i] == '.')
            {
                DotSeen = 1;
                continue;
            }
            if (Fields[9][i] >= '0' && Fields[9][i] <= '9')
            {
                if (!DotSeen)
                    Data->AltInt = Data->AltInt * 10 + (Fields[9][i] - '0');
                else
                    Data->AltDec = Fields[9][i] - '0';
            }
        }
    }

    Data->Valid = TRUE;
}

// Returns index of best constellation (0=GPS, 1=GLONASS, 2=BeiDou, 3=Galileo)
// Scoring: (Satellites x 10) + (FixQuality x 5) - (HdopInt x 2)
// Tie-break: lower HDOP wins
INTN CalculateBestFix(GNSS_DATA *Data, INTN Count)
{
    INTN BestIndex = -1;
    INTN BestScore = -9999;

    for (INTN i = 0; i < Count; i++)
    {
        // Skip invalid or no-fix constellations
        if (!Data[i].Valid || Data[i].FixQuality == 0)
            continue;

        INTN Score = (Data[i].Satellites * 10) + (Data[i].FixQuality * 5) - (Data[i].HdopInt * 2);

        if (Score > BestScore)
        {
            BestScore = Score;
            BestIndex = i;
        }
        // Tie-break: lower HDOP wins
        else if (Score == BestScore && BestIndex >= 0)
        {
            if (Data[i].HdopInt < Data[BestIndex].HdopInt ||
                (Data[i].HdopInt == Data[BestIndex].HdopInt &&
                 Data[i].HdopDec < Data[BestIndex].HdopDec))
            {
                BestIndex = i;
            }
        }
    }

    return BestIndex; // -1 means no valid fix from any constellation
}

// Displays the 2x2 GNSS grid + Best Fix banner
// Data[0]=GPS  Data[1]=GLONASS  Data[2]=BeiDou  Data[3]=Galileo
VOID DisplayGnssScreen(GNSS_DATA *Data)
{
    CHAR16 *FixLabel[] = {
        L"No Fix",
        L"Autonomous",
        L"DGPS Fix",
        L"Unknown",
        L"RTK Fixed",
        L"RTK Float"};

    CHAR16 *ConstellationName[] = {
        L"GPS     (USA)",
        L"GLONASS (RUS)",
        L"BeiDou  (CHN)",
        L"Galileo  (EU)"};

    CHAR8 *SentenceID[] = {
        "$GPGGA",
        "$GLGGA",
        "$BDGGA",
        "$GAGGA"};

    gST->ConOut->ClearScreen(gST->ConOut);

    // ── Top banner ──────────────────────────────────────────
    Print(L"+=====================================================+\n");
    Print(L"|          GNSS TELEMETRY MONITOR                    |\n");
    Print(L"+==========================+==========================+\n");

    // ── Row 1: GPS (left) | GLONASS (right) ─────────────────
    for (INTN col = 0; col < 2; col++)
    {

        if (col == 0)
            Print(L"|  %-24s  |", ConstellationName[col]);
        else
            Print(L"  %-24s  |\n", ConstellationName[col]);
    }

    for (INTN col = 0; col < 2; col++)
    {
        if (col == 0)
            Print(L"|  Sentence : %-13a  |", SentenceID[col]);
        else
            Print(L"  Sentence : %-13a  |\n", SentenceID[col]);
    }

    // UTC Time row
    Print(L"|  UTC Time : %-13a  |", Data[0].Valid ? Data[0].UTCTime : "------");
    Print(L"  UTC Time : %-13a  |\n", Data[1].Valid ? Data[1].UTCTime : "------");

    // Latitude row
    if (Data[0].Valid)
        Print(L"|  Lat      : %d* %d.%d %c  |",
              Data[0].LatDeg, Data[0].LatMin, Data[0].LatSec, (CHAR16)Data[0].LatDir);
    else
        Print(L"|  Lat      : ------          |");

    if (Data[1].Valid)
        Print(L"  Lat      : %d* %d.%d %c  |\n",
              Data[1].LatDeg, Data[1].LatMin, Data[1].LatSec, (CHAR16)Data[1].LatDir);
    else
        Print(L"  Lat      : ------          |\n");

    // Longitude row
    if (Data[0].Valid)
        Print(L"|  Lon      : %d* %d.%d %c  |",
              Data[0].LonDeg, Data[0].LonMin, Data[0].LonSec, (CHAR16)Data[0].LonDir);
    else
        Print(L"|  Lon      : ------          |");

    if (Data[1].Valid)
        Print(L"  Lon      : %d* %d.%d %c  |\n",
              Data[1].LonDeg, Data[1].LonMin, Data[1].LonSec, (CHAR16)Data[1].LonDir);
    else
        Print(L"  Lon      : ------          |\n");

    // Altitude row
    if (Data[0].Valid)
        Print(L"|  Altitude : %d.%d m          |", Data[0].AltInt, Data[0].AltDec);
    else
        Print(L"|  Altitude : ------          |");

    if (Data[1].Valid)
        Print(L"  Altitude : %d.%d m          |\n", Data[1].AltInt, Data[1].AltDec);
    else
        Print(L"  Altitude : ------          |\n");

    // Satellites row
    Print(L"|  Sats     : %-13d  |", Data[0].Valid ? Data[0].Satellites : 0);
    Print(L"  Sats     : %-13d  |\n", Data[1].Valid ? Data[1].Satellites : 0);

    // Fix Quality row
    Print(L"|  Fix      : %-13s  |",
          Data[0].Valid ? FixLabel[Data[0].FixQuality] : L"------");
    Print(L"  Fix      : %-13s  |\n",
          Data[1].Valid ? FixLabel[Data[1].FixQuality] : L"------");

    // HDOP row
    if (Data[0].Valid)
        Print(L"|  HDOP     : %d.%d            |", Data[0].HdopInt, Data[0].HdopDec);
    else
        Print(L"|  HDOP     : ------          |");

    if (Data[1].Valid)
        Print(L"  HDOP     : %d.%d            |\n", Data[1].HdopInt, Data[1].HdopDec);
    else
        Print(L"  HDOP     : ------          |\n");

    // ── Divider ─────────────────────────────────────────────
    Print(L"+==========================+==========================+\n");

    // ── Row 2: BeiDou (left) | Galileo (right) ──────────────
    for (INTN col = 2; col < 4; col++)
    {
        if (col == 2)
            Print(L"|  %-24s  |", ConstellationName[col]);
        else
            Print(L"  %-24s  |\n", ConstellationName[col]);
    }

    for (INTN col = 2; col < 4; col++)
    {
        if (col == 2)
            Print(L"|  Sentence : %-13a  |", SentenceID[col]);
        else
            Print(L"  Sentence : %-13a  |\n", SentenceID[col]);
    }

    // UTC Time
    Print(L"|  UTC Time : %-13a  |", Data[2].Valid ? Data[2].UTCTime : "------");
    Print(L"  UTC Time : %-13a  |\n", Data[3].Valid ? Data[3].UTCTime : "------");

    // Latitude
    if (Data[2].Valid)
        Print(L"|  Lat      : %d* %d.%d %c  |",
              Data[2].LatDeg, Data[2].LatMin, Data[2].LatSec, (CHAR16)Data[2].LatDir);
    else
        Print(L"|  Lat      : ------          |");

    if (Data[3].Valid)
        Print(L"  Lat      : %d* %d.%d %c  |\n",
              Data[3].LatDeg, Data[3].LatMin, Data[3].LatSec, (CHAR16)Data[3].LatDir);
    else
        Print(L"  Lat      : ------          |\n");

    // Longitude
    if (Data[2].Valid)
        Print(L"|  Lon      : %d* %d.%d %c  |",
              Data[2].LonDeg, Data[2].LonMin, Data[2].LonSec, (CHAR16)Data[2].LonDir);
    else
        Print(L"|  Lon      : ------          |");

    if (Data[3].Valid)
        Print(L"  Lon      : %d* %d.%d %c  |\n",
              Data[3].LonDeg, Data[3].LonMin, Data[3].LonSec, (CHAR16)Data[3].LonDir);
    else
        Print(L"  Lon      : ------          |\n");

    // Altitude
    if (Data[2].Valid)
        Print(L"|  Altitude : %d.%d m          |", Data[2].AltInt, Data[2].AltDec);
    else
        Print(L"|  Altitude : ------          |");

    if (Data[3].Valid)
        Print(L"  Altitude : %d.%d m          |\n", Data[3].AltInt, Data[3].AltDec);
    else
        Print(L"  Altitude : ------          |\n");

    // Satellites
    Print(L"|  Sats     : %-13d  |", Data[2].Valid ? Data[2].Satellites : 0);
    Print(L"  Sats     : %-13d  |\n", Data[3].Valid ? Data[3].Satellites : 0);

    // Fix Quality
    Print(L"|  Fix      : %-13s  |",
          Data[2].Valid ? FixLabel[Data[2].FixQuality] : L"------");
    Print(L"  Fix      : %-13s  |\n",
          Data[3].Valid ? FixLabel[Data[3].FixQuality] : L"------");

    // HDOP
    if (Data[2].Valid)
        Print(L"|  HDOP     : %d.%d            |", Data[2].HdopInt, Data[2].HdopDec);
    else
        Print(L"|  HDOP     : ------          |");

    if (Data[3].Valid)
        Print(L"  HDOP     : %d.%d            |\n", Data[3].HdopInt, Data[3].HdopDec);
    else
        Print(L"  HDOP     : ------          |\n");

    // ── Best Fix Banner ──────────────────────────────────────
    Print(L"+==========================+==========================+\n");

    INTN Best = CalculateBestFix(Data, 4);

    if (Best < 0)
    {
        Print(L"|       BEST AVAILABLE FIX  -->  NO FIX            |\n");
        Print(L"|       No constellation has a valid fix            |\n");
    }
    else
    {
        GNSS_DATA *B = &Data[Best];
        Print(L"|    BEST AVAILABLE FIX  -->  %s        |\n",
              ConstellationName[Best]);
        Print(L"|    Lat : %d* %d.%d %c   Lon : %d* %d.%d %c   Alt : %d.%d m  |\n",
              B->LatDeg, B->LatMin, B->LatSec, (CHAR16)B->LatDir,
              B->LonDeg, B->LonMin, B->LonSec, (CHAR16)B->LonDir,
              B->AltInt, B->AltDec);
        Print(L"|    Reason : Sats=%d  Fix=%s  HDOP=%d.%d           |\n",
              B->Satellites,
              FixLabel[B->FixQuality],
              B->HdopInt, B->HdopDec);
    }

    Print(L"+=====================================================+\n");
}

// Read one complete line from serial port into buffer
// Returns TRUE if a complete line was read, FALSE if timeout or error
BOOLEAN ReadSerialLine(
    EFI_SERIAL_IO_PROTOCOL *Serial,
    CHAR8 *Buffer,
    UINTN MaxLen)
{
    UINTN Index = 0;
    UINTN BufferSize = 1;
    CHAR8 Byte = 0;
    UINTN Timeout = 200000; // loop iterations before giving up

    if (Serial == NULL || Buffer == NULL)
        return FALSE;

    while (Index < MaxLen - 1)
    {
        BufferSize = 1;
        EFI_STATUS Status = Serial->Read(Serial, &BufferSize, &Byte);

        if (Status == EFI_SUCCESS && BufferSize == 1)
        {
            // Ignore carriage return
            if (Byte == '\r')
                continue;

            // Newline = end of sentence
            if (Byte == '\n')
            {
                Buffer[Index] = '\0';
                return (Index > 0) ? TRUE : FALSE;
            }

            Buffer[Index++] = Byte;
        }
        else
        {
            // No data yet — countdown timeout
            Timeout--;
            if (Timeout == 0)
            {
                Buffer[Index] = '\0';
                return FALSE;
            }

            // Small delay — let QEMU serial buffer fill
            gBS->Stall(500); // 500 microseconds
        }
    }

    Buffer[Index] = '\0';
    return FALSE; // buffer full without newline
}

// GNSS Telemetry Parser - Main Menu
VOID GnssMenu()
{
    BOOLEAN Running = TRUE;

    while (Running)
    {
        gST->ConOut->ClearScreen(gST->ConOut);

        Print(L"+=====================================================+\n");
        Print(L"|          GNSS TELEMETRY MONITOR                    |\n");
        Print(L"+=====================================================+\n\n");

        Print(L"  [1] Start Live Monitor\n");
        Print(L"  [2] Back to Main Menu\n\n");

        Print(L"  Select option: ");

        UINTN choice = GetChoice();
        Print(L"%d\n", choice);

        switch (choice)
        {
        case 1:
        {
            // ── Locate EFI_SERIAL_IO_PROTOCOL ──────────────────
            EFI_SERIAL_IO_PROTOCOL *Serial = NULL;
            EFI_GUID SerialGuid = EFI_SERIAL_IO_PROTOCOL_GUID;

            EFI_STATUS Status = gBS->LocateProtocol(
                &SerialGuid,
                NULL,
                (VOID **)&Serial);

            if (EFI_ERROR(Status) || Serial == NULL)
            {
                Print(L"\n  ERROR: Serial port not found!\n");
                Print(L"  Make sure QEMU is started with -serial option.\n");
                WaitForKey();
                break;
            }

            // ── Configure serial port 9600 8N1 ─────────────────
            Serial->SetAttributes(
                Serial,
                9600, // BaudRate
                0,    // ReceiveFifoDepth (default)
                0,    // Timeout (default)
                NoParity,
                8, // DataBits
                OneStopBit);

            // ── Initialize 4 constellation structs ─────────────
            GNSS_DATA Data[4];
            for (INTN i = 0; i < 4; i++)
            {
                Data[i].Valid = FALSE;
                Data[i].Satellites = 0;
                Data[i].FixQuality = 0;
                Data[i].HdopInt = 0;
                Data[i].HdopDec = 0;
                Data[i].AltInt = 0;
                Data[i].AltDec = 0;
                Data[i].LatDeg = 0;
                Data[i].LatMin = 0;
                Data[i].LatSec = 0;
                Data[i].LonDeg = 0;
                Data[i].LonMin = 0;
                Data[i].LonSec = 0;
                Data[i].LatDir = 'N';
                Data[i].LonDir = 'E';
                Data[i].UTCTime[0] = '\0';
                Data[i].Talker[0] = '\0';
            }

            Print(L"\n  Reading NMEA data from serial port...\n");
            Print(L"  Press any key to stop monitoring.\n\n");
            gBS->Stall(1000000); // 1 second pause so user reads message

            CHAR8 LineBuf[128];
            UINTN ReadCount = 0;

            // ── Main monitoring loop ────────────────────────────
            while (TRUE)
            {
                // Check if user pressed a key to exit
                EFI_INPUT_KEY Key;
                if (gST->ConIn->ReadKeyStroke(gST->ConIn, &Key) == EFI_SUCCESS)
                    break;

                // Try to read one NMEA sentence from serial
                if (ReadSerialLine(Serial, LineBuf, 128))
                {
                    ReadCount++;

                    // Route to correct constellation slot
                    // LineBuf[1..2] = talker ID
                    // GP=0  GL=1  BD=2  GA=3
                    INTN Slot = -1;

                    if (LineBuf[0] == '$')
                    {
                        if (LineBuf[1] == 'G' && LineBuf[2] == 'P')
                            Slot = 0; // GPS
                        else if (LineBuf[1] == 'G' && LineBuf[2] == 'L')
                            Slot = 1; // GLONASS
                        else if (LineBuf[1] == 'B' && LineBuf[2] == 'D')
                            Slot = 2; // BeiDou
                        else if (LineBuf[1] == 'G' && LineBuf[2] == 'A')
                            Slot = 3; // Galileo
                        // $GN = combined, parse into GPS slot as fallback
                        else if (LineBuf[1] == 'G' && LineBuf[2] == 'N')
                            Slot = 0;
                    }

                    if (Slot >= 0)
                    {
                        ParseGGASentence(LineBuf, &Data[Slot]);
                    }

                    // Refresh display every 4 sentences
                    // (one full cycle of all 4 constellations)
                    if (ReadCount % 4 == 0)
                    {
                        DisplayGnssScreen(Data);
                        Print(L"\n  [Press any key to exit monitor]\n");
                    }
                }
            }

            // Show final state before exiting
            DisplayGnssScreen(Data);
            Print(L"\n  Monitoring stopped.\n");
            WaitForKey();
            break;
        }

        case 2:
            Running = FALSE;
            break;

        default:
            Print(L"\n  Invalid choice!\n");
            WaitForKey();
        }
    }
}
// Main Entry Point
EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
    BOOLEAN Running = TRUE;

    while (Running)
    {
        gST->ConOut->ClearScreen(gST->ConOut);

        Print(L"╔═══════════════════════════════════╗\n");
        Print(L"║  UEFI DEMO SUITE                  ║\n");
        Print(L"╚═══════════════════════════════════╝\n\n");

        Print(L"  [1] Calculator Suite\n");
        Print(L"  [2] Sorting Algorithm\n");
        Print(L"  [3] System Information\n");
        Print(L"  [4] Encryption Tools\n");
        Print(L"  [5] Pattern Renderer\n");
        Print(L"  [6] GNSS Telemetry\n");
        Print(L"  [7] Exit\n\n");

        Print(L"Select option: ");

        UINTN choice = GetChoice();
        Print(L"%d\n", choice);

        switch (choice)
        {
        case 1:
            CalculatorMenu();
            break;
        case 2:
            SortingAlgorithm();
            break;
        case 3:
            SystemInformation();
            break;
        case 4:
            EncryptionTools();
            break;
        case 5:
            PatternRenderer();
            break;

        case 6:
            GnssMenu();
            break;

        case 7:
            Print(L"\nThank you for using UEFI Demo Suite!\n");
            Running = FALSE;
            break;
        default:
            Print(L"\nInvalid choice!\n");
            WaitForKey();
        }
    }

    return EFI_SUCCESS;
}
#include <syscall.h>
#include <windows.h>
#include <intrin.h>
#include <stdint.h>
#include <unordered_map>
#include <vector>
#include <set>

#define LOGGING 1

#ifndef FILE_OPEN
#define FILE_OPEN 0x00000001
#endif
#ifndef OBJ_CASE_INSENSITIVE
#define OBJ_CASE_INSENSITIVE 0x00000040
#endif

#include <winternl.h>

#ifndef FileStandardInformation
#define FileStandardInformation 5
#endif

namespace crc 
{
    constexpr uint32_t tbl[256] = {
        0x00000000,0x77073096,0xee0e612c,0x990951ba,0x076dc419,0x706af48f,0xe963a535,0x9e6495a3,
        0x0edb8832,0x79dcb8a4,0xe0d5e91b,0x97d2d988,0x09b64c2b,0x7eb17cbd,0xe7b82d07,0x90bf1d91,
        0x1db71064,0x6ab020f2,0xf3b97148,0x84be41de,0x1adad47d,0x6ddde4eb,0xf4d4b551,0x83d385c7,
        0x136c9856,0x646ba8c0,0xfd62f97a,0x8a65c9ec,0x14015c4f,0x63066cd9,0xfa0f3d63,0x8d080df5,
        0x3b6e20c8,0x4c69105e,0xd56041e4,0xa2677172,0x3c03e4d1,0x4b04d447,0xd20d85fd,0xa50ab56b,
        0x35b5a8fa,0x42b2986c,0xdbbbc9d6,0xacbcf940,0x32d86ce3,0x45df5c75,0xdcd60dcf,0xabd13d59,
        0x26d930ac,0x51de003a,0xc8d75180,0xbfd06116,0x21b4f4b5,0x56b3c423,0xcfba9599,0xb8bda50f,
        0x2802b89e,0x5f058808,0xc60cd9b2,0xb10be924,0x2f6f7c87,0x58684c11,0xc1611dab,0xb6662d3d,
        0x76dc4190,0x01db7106,0x98d220bc,0xefd5102a,0x71b18589,0x06b6b51f,0x9fbfe4a5,0xe8b8d433,
        0x7807c9a2,0x0f00f934,0x9609a88e,0xe10e9818,0x7f6a0dbb,0x086d3d2d,0x91646c97,0xe6635c01,
        0x6b6b51f4,0x1c6c6162,0x856530d8,0xf262004e,0x6c0695ed,0x1b01a57b,0x8208f4c1,0xf50fc457,
        0x65b0d9c6,0x12b7e950,0x8bbeb8ea,0xfcb9887c,0x62dd1ddf,0x15da2d49,0x8cd37cf3,0xfbd44c65,
        0x4db26158,0x3ab551ce,0xa3bc0074,0xd4bb30e2,0x4adfa541,0x3dd895d7,0xa4d1c46d,0xd3d6f4fb,
        0x4369e96a,0x346ed9fc,0xad678846,0xda60b8d0,0x44042d73,0x33031de5,0xaa0a4c5f,0xdd0d7cc9,
        0x5005713c,0x270241aa,0xbe0b1010,0xc90c2086,0x5768b525,0x206f85b3,0xb966d409,0xce61e49f,
        0x5edef90e,0x29d9c998,0xb0d09822,0xc7d7a8b4,0x59b33d17,0x2eb40d81,0xb7bd5c3b,0xc0ba6cad,
        0xedb88320,0x9abfb3b6,0x03b6e20c,0x74b1d29a,0xead54739,0x9dd277af,0x04db2615,0x73dc1683,
        0xe3630b12,0x94643b84,0x0d6d6a3e,0x7a6a5aa8,0xe40ecf0b,0x9309ff9d,0x0a00ae27,0x7d079eb1,
        0xf00f9344,0x8708a3d2,0x1e01f268,0x6906c2fe,0xf762575d,0x806567cb,0x196c3671,0x6e6b06e7,
        0xfed41b76,0x89d32be0,0x10da7a5a,0x67dd4acc,0xf9b9df6f,0x8ebeeff9,0x17b7be43,0x60b08ed5,
        0xd6d6a3e8,0xa1d1937e,0x38d8c2c4,0x4fdff252,0xd1bb67f1,0xa6bc5767,0x3fb506dd,0x48b2364b,
        0xd80d2bda,0xaf0a1b4c,0x36034af6,0x41047a60,0xdf60efc3,0xa867df55,0x316e8eef,0x4669be79,
        0xcb61b38c,0xbc66831a,0x256fd2a0,0x5268e236,0xcc0c7795,0xbb0b4703,0x220216b9,0x5505262f,
        0xc5ba3bbe,0xb2bd0b28,0x2bb45a92,0x5cb36a04,0xc2d7ffa7,0xb5d0cf31,0x2cd99e8b,0x5bdeae1d,
        0x9b64c2b0,0xec63f226,0x756aa39c,0x026d930a,0x9c0906a9,0xeb0e363f,0x72076785,0x05005713,
        0x95bf4a82,0xe2b87a14,0x7bb12bae,0x0cb61b38,0x92d28e9b,0xe5d5be0d,0x7cdcefb7,0x0bdbdf21,
        0x86d3d2d4,0xf1d4e242,0x68ddb3f8,0x1fda836e,0x81be16cd,0xf6b9265b,0x6fb077e1,0x18b74777,
        0x88085ae6,0xff0f6a70,0x66063bca,0x11010b5c,0x8f659eff,0xf862ae69,0x616bffd3,0x166ccf45,
        0xa00ae278,0xd70dd2ee,0x4e048354,0x3903b3c2,0xa7672661,0xd06016f7,0x4969474d,0x3e6e77db,
        0xaed16a4a,0xd9d65adc,0x40df0b66,0x37d83bf0,0xa9bcae53,0xdebb9ec5,0x47b2cf7f,0x30b5ffe9,
        0xbdbdf21c,0xcabac28a,0x53b39330,0x24b4a3a6,0xbad03605,0xcdd70693,0x54de5729,0x23d967bf,
        0xb3667a2e,0xc4614ab8,0x5d681b02,0x2a6f2b94,0xb40bbe37,0xc30c8ea1,0x5a05df1b,0x2d02ef8d
    };

    __forceinline uint32_t calc(uint8_t* data, uint32_t len)
    {
        uint32_t r = ~0u;
        for (uint32_t i = 0; i < len; i++)
            r = tbl[(r ^ data[i]) & 0xff] ^ (r >> 8);
        return ~r;
    }
}

struct ldr_entry_t 
{
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    ULONG Pad;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
};

__forceinline uint8_t* find_mod(const wchar_t* suffix, size_t suffix_len) 
{
    auto peb = (uint8_t*)__readgsqword(0x60);
    auto ldr = *(uint8_t**)(peb + 0x18);
    auto* head = (LIST_ENTRY*)(ldr + 0x10);
    auto* cur = head->Flink;
    while (cur != head) {
        auto* e = (ldr_entry_t*)cur;
        if (e->BaseDllName.Length >= suffix_len * 2) 
        {
            const wchar_t* name = e->BaseDllName.Buffer;
            size_t len = e->BaseDllName.Length / 2;
            bool match = true;
            for (size_t i = 0; i < suffix_len; i++) 
            {
                wchar_t a = name[len - suffix_len + i];
                if (a >= L'A' && a <= L'Z') a += 32;
                if (a != suffix[i]) { match = false; break; }
            }
            if (match) return (uint8_t*)e->DllBase;
        }
        cur = cur->Flink;
    }
    return nullptr;
}

__forceinline void* get_proc(uint8_t* mod, const char* name)
{
    if (!mod) return nullptr;
    auto dos = (IMAGE_DOS_HEADER*)mod;
    auto nt = (IMAGE_NT_HEADERS64*)(mod + dos->e_lfanew);
    auto& d = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    if (!d.VirtualAddress) return nullptr;
    auto exp = (PIMAGE_EXPORT_DIRECTORY)(mod + d.VirtualAddress);
    auto funcs = (uint32_t*)(mod + exp->AddressOfFunctions);
    auto names = (uint32_t*)(mod + exp->AddressOfNames);
    auto ords = (uint16_t*)(mod + exp->AddressOfNameOrdinals);
    for (DWORD i = 0; i < exp->NumberOfNames; i++) 
    {
        if (strcmp((const char*)(mod + names[i]), name) == 0)
            return mod + funcs[ords[i]];
    }
    return nullptr;
}

__forceinline HANDLE open_self_read()
{
    wchar_t path[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    return CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
}

namespace g 
{
    uintptr_t   range_start = 0;
    uintptr_t   range_end = 0;

    uint8_t* ki_disp = nullptr;
    uint8_t     ki_orig[14] = {};
    void** wow64_ptr = nullptr;
    void* wow64_orig = nullptr;
    bool        hooked = false;

    void* disk_img = nullptr;
    uint32_t    disk_img_sz = 0;

    HANDLE      reenc_thrd = nullptr;
    HANDLE      watch_thrd = nullptr;
    HANDLE      timer_q = nullptr;
    HANDLE      timer_h = nullptr;

    volatile bool       stop = false;
    volatile LONG       fault_ctr = 0;
    volatile ULONGLONG  timer_hb = 0;
    volatile ULONGLONG  thread_hb = 0;
    volatile ULONGLONG  watch_hb = 0;

    CRITICAL_SECTION pg_lock;
    CRITICAL_SECTION tid_lock;
    CRITICAL_SECTION rel_lock;
    CRITICAL_SECTION int_lock;
}

__declspec(thread) volatile long t_in_handler = 0;

namespace pg 
{
    struct info
    {
        uintptr_t       base;
        ULONGLONG       last_access;
        ULONGLONG       dec_time;
        DWORD           access_cnt;
        DWORD           owner_tid;
        bool            decrypted;
    };

    struct tid_info 
    {
        DWORD       tid;
        uintptr_t   last_page;
        ULONGLONG   dec_time;
    };

    struct rel_info
    {
        std::set<uintptr_t> neighbors;
        ULONGLONG           last_pair;
        DWORD               pair_cnt;
    };

    std::unordered_map<uintptr_t, info>     map;
    std::unordered_map<DWORD, tid_info>     tid_map;
    std::unordered_map<uintptr_t, rel_info> rel_map;

    __forceinline void spin_acq(volatile LONG* f)
    {
        while (InterlockedCompareExchange(f, 1, 0) != 0) _mm_pause();
    }
    __forceinline void spin_rel(volatile LONG* f) { InterlockedExchange(f, 0); }
}

// change from xor
#define ENC_KEY 0xCC

__forceinline void pg_encrypt(uint8_t* page) 
{
    for (DWORD i = 0; i < 0x1000; i++) page[i] ^= ENC_KEY;
}

__forceinline void pg_decrypt(uint8_t* page) 
{
    for (DWORD i = 0; i < 0x1000; i++) page[i] ^= ENC_KEY;
}

__forceinline ULONGLONG  ts_ms()
{
    volatile ULONG* lo = (volatile ULONG*)0x7FFE0320;
    volatile LONG* h1 = (volatile LONG*)0x7FFE0324;
    volatile LONG* h2 = (volatile LONG*)0x7FFE0328;
    for (;;)
    {
        LONG a = *h1; ULONG l = *lo; LONG b = *h2;
        if (a == b) return ((ULONGLONG)(ULONG)a << 32) | l;
    }
}
__forceinline bool       in_range(uintptr_t a) { return a >= g::range_start && a <= g::range_end; }
__forceinline uintptr_t  pg_base(uintptr_t a) { return a & ~(uintptr_t)0xfff; }

__forceinline PIMAGE_NT_HEADERS nt_headers(void* base) 
{
    return (PIMAGE_NT_HEADERS)((uint8_t*)base + ((PIMAGE_DOS_HEADER)base)->e_lfanew);
}

__forceinline PIMAGE_SECTION_HEADER find_sec(HMODULE mod, const char* name) 
{
    auto* sec = IMAGE_FIRST_SECTION(nt_headers(mod));
    WORD  cnt = nt_headers(mod)->FileHeader.NumberOfSections;
    for (WORD i = 0; i < cnt; i++, sec++)
        if (!memcmp(sec->Name, name, strlen(name))) return sec;
    return nullptr;
}

// page relationship tracking, avoids insanely slow loops
namespace rel 
{
    __forceinline bool friendly(uintptr_t a, uintptr_t b) 
    {
        if (a == b) return true;
        EnterCriticalSection(&g::rel_lock);
        bool r = false;
        auto it = pg::rel_map.find(a);
        if (it != pg::rel_map.end()
            && it->second.neighbors.count(b)
            && (ts_ms() - it->second.last_pair) < 500)
            r = true;
        LeaveCriticalSection(&g::rel_lock);
        return r;
    }

    __forceinline void record(uintptr_t from, uintptr_t to) 
    {
        if (!from || !to || from == to) return;
        ULONGLONG now = ts_ms();
        EnterCriticalSection(&g::rel_lock);
        auto& r = pg::rel_map[from];
        if (now - r.last_pair < 100) 
        {
            if (++r.pair_cnt >= 3)
            {
                r.neighbors.insert(to);
                pg::rel_map[to].neighbors.insert(from);
                pg::rel_map[to].last_pair = now;
            }
        }
        else 
        {
            r.pair_cnt = 1;
        }
        r.last_pair = now;
        LeaveCriticalSection(&g::rel_lock);
    }

    __forceinline void cleanup() 
    {
        ULONGLONG now = ts_ms();
        EnterCriticalSection(&g::rel_lock);
        for (auto it = pg::rel_map.begin(); it != pg::rel_map.end();)
            it = (now - it->second.last_pair > 1000) ? pg::rel_map.erase(it) : ++it;
        LeaveCriticalSection(&g::rel_lock);
    }
}

//example of how u would do an integrity check with page encryption
namespace integrity
{
    struct sec_info 
    {
        uintptr_t va;
        SIZE_T   size;
        SIZE_T   dsk_off;
        uint32_t crc;
    };
    sec_info cached[16] = {};
    DWORD cached_cnt = 0;

    __forceinline uint64_t rva_to_off(PIMAGE_NT_HEADERS nt, PIMAGE_SECTION_HEADER sec, uint64_t rva)
    {
        for (int i = 0; i < nt->FileHeader.NumberOfSections; i++)
        {
            if (rva >= sec[i].VirtualAddress && rva < sec[i].VirtualAddress + sec[i].Misc.VirtualSize)
                return rva - sec[i].VirtualAddress + sec[i].PointerToRawData;
        }
        return 0;
    }

    __forceinline void prot_sec_crypt(PVOID file_buf, bool encrypt) 
    {
        if (!g::range_start) return;
        auto peb = (uint8_t*)__readgsqword(0x60);
        uint8_t* mod = *(uint8_t**)(peb + 0x10);
        uint32_t prot_rva = (uint32_t)(g::range_start - (uintptr_t)mod);
        auto dos = (PIMAGE_DOS_HEADER)file_buf;
        auto nt = (PIMAGE_NT_HEADERS)((uint8_t*)file_buf + dos->e_lfanew);
        auto sec = IMAGE_FIRST_SECTION(nt);
        for (WORD i = 0; i < nt->FileHeader.NumberOfSections; i++) 
        {
            if (sec[i].VirtualAddress == prot_rva) 
            {
                uint8_t* p = (uint8_t*)file_buf + sec[i].PointerToRawData;
                for (DWORD off = 0; off + 0x1000 <= sec[i].SizeOfRawData; off += 0x1000) 
                {
                    if (encrypt) pg_encrypt(p + off);
                    else pg_decrypt(p + off);
                }
                break;
            }
        }
    }

    __forceinline void disk_reloc(PVOID file_buf, PVOID mod_base) 
    {
        auto dos = (PIMAGE_DOS_HEADER)file_buf;
        auto nt = (PIMAGE_NT_HEADERS)((uint8_t*)file_buf + dos->e_lfanew);
        if (nt->Signature != IMAGE_NT_SIGNATURE) return;
        auto sec = IMAGE_FIRST_SECTION(nt);

        uint64_t delta = (uint64_t)mod_base - nt->OptionalHeader.ImageBase;
        if (delta)
        {
            auto& rd = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
            if (rd.Size) 
            {
                auto* reloc = (PIMAGE_BASE_RELOCATION)((uint8_t*)file_buf + rva_to_off(nt, sec, rd.VirtualAddress));
                auto* reloc_end = (PIMAGE_BASE_RELOCATION)((uint8_t*)reloc + rd.Size);
                while (reloc < reloc_end && reloc->SizeOfBlock)
                {
                    DWORD count = (reloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
                    WORD* entries = (WORD*)(reloc + 1);
                    for (DWORD i = 0; i < count; i++)
                    {
                        WORD type = entries[i] >> 12;
                        WORD offset = entries[i] & 0xFFF;
                        uint64_t target_off = rva_to_off(nt, sec, reloc->VirtualAddress + offset);
                        if (!target_off) continue;
                        switch (type) {
                        case IMAGE_REL_BASED_DIR64:
                            *(uint64_t*)((uint8_t*)file_buf + target_off) += delta;
                            break;
                        case IMAGE_REL_BASED_HIGHLOW:
                            *(uint32_t*)((uint8_t*)file_buf + target_off) += (uint32_t)delta;
                            break;
                        case IMAGE_REL_BASED_ABSOLUTE:
                            break;
                        default:
                            break;
                        }
                    }
                    reloc = (PIMAGE_BASE_RELOCATION)((uint8_t*)reloc + reloc->SizeOfBlock);
                }
            }

            auto& id = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
            if (id.Size) 
            {
                auto imp = (PIMAGE_IMPORT_DESCRIPTOR)((uint8_t*)file_buf + rva_to_off(nt, sec, id.VirtualAddress));
                for (; imp->Name; imp++) 
                {
                    char* dll_name = (char*)file_buf + rva_to_off(nt, sec, imp->Name);
                    if (!dll_name) continue;
                    HMODULE h = (HMODULE)find_mod((wchar_t*)L".dll", 4);
                    if (!h) h = LoadLibraryA(dll_name);
                    if (!h) continue;
                    uint64_t* oft = (uint64_t*)((uint8_t*)file_buf + rva_to_off(nt, sec, imp->OriginalFirstThunk));
                    uint64_t* ft = (uint64_t*)((uint8_t*)file_buf + rva_to_off(nt, sec, imp->FirstThunk));
                    if (!oft) oft = ft;
                    for (; *oft; oft++, ft++)
                    {
                        void* addr = nullptr;
                        if (IMAGE_SNAP_BY_ORDINAL(*oft))
                        {
                            addr = get_proc((uint8_t*)h, (char*)(IMAGE_ORDINAL(*oft)));
                        }
                        else
                        {
                            auto ibn = (PIMAGE_IMPORT_BY_NAME)((uint8_t*)file_buf + rva_to_off(nt, sec, *oft));
                            if (ibn && ibn->Name) addr = get_proc((uint8_t*)h, ibn->Name);
                        }
                        if (addr) *ft = (uint64_t)addr;
                    }
                }
            }
        }
    }

    __forceinline bool load_disk_img() 
    {
        HANDLE f = open_self_read();
        if (!f)
        {
#if LOGGING
            printf("load_disk_img: open_self_read FAILED\n");
#endif
            return false;
        }

        IO_STATUS_BLOCK iosb = {};
        struct { LARGE_INTEGER a; LARGE_INTEGER b; ULONG c; BOOLEAN d; BOOLEAN e; } fsi = {};
        NTSTATUS qst = syscall<NTSTATUS>("NtQueryInformationFile", f, &iosb, &fsi, sizeof(fsi), FileStandardInformation);
        DWORD sz = (DWORD)fsi.b.QuadPart;
#if LOGGING
       // printf("load_disk_img: query st=0x%X sz=%u\n", qst, sz);
#endif
        if (!sz || qst != 0) { syscall<NTSTATUS>("NtClose", f); return false; }

        PVOID b = nullptr; SIZE_T s = sz;
        NTSTATUS ast = syscall<NTSTATUS>("NtAllocateVirtualMemory", (HANDLE)-1, &b, (ULONG_PTR)0, &s, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#if LOGGING
      //  printf("load_disk_img: alloc st=0x%X buf=0x%p\n", ast, b);
#endif
        g::disk_img = b;
        g::disk_img_sz = sz;

        IO_STATUS_BLOCK iosb2 = {};
        NTSTATUS st = syscall<NTSTATUS>("NtReadFile", f, nullptr, nullptr, nullptr, &iosb2, g::disk_img, (ULONG)sz, nullptr, nullptr);
        bool ok = (st == 0 && (SIZE_T)iosb2.Information == sz);
#if LOGGING
     //   printf("load_disk_img: read st=0x%X info=%llu ok=%d\n", st, (uint64_t)iosb2.Information, ok);
#endif
        syscall<NTSTATUS>("NtClose", f);

        if (!ok) 
        {
            PVOID fb = g::disk_img; SIZE_T fs = 0;
            syscall<NTSTATUS>("NtFreeVirtualMemory", (HANDLE)-1, &fb, &fs, MEM_RELEASE);
            g::disk_img = nullptr;
            g::disk_img_sz = 0;
            return false;
        }

        auto peb = (uint8_t*)__readgsqword(0x60);
        uint8_t* mod = *(uint8_t**)(peb + 0x10);
        prot_sec_crypt(g::disk_img, false);
        disk_reloc(g::disk_img, mod);
        prot_sec_crypt(g::disk_img, true);

#if LOGGING
     //   printf("disk img loaded size=%u\n", g::disk_img_sz);
#endif
        return true;
    }

    __forceinline void free_disk_img()
    {
        if (g::disk_img)
        {
            PVOID fb = g::disk_img; SIZE_T fs = 0;
            syscall<NTSTATUS>("NtFreeVirtualMemory", (HANDLE)-1, &fb, &fs, MEM_RELEASE);
            g::disk_img = nullptr;
            g::disk_img_sz = 0;
        }
    }

    __forceinline bool cache_crc()
    {
        auto peb = (uint8_t*)__readgsqword(0x60);
        HMODULE mod = *(HMODULE*)(peb + 0x10);
        auto* nt = nt_headers(mod);
        auto* sec = IMAGE_FIRST_SECTION(nt);

        for (WORD i = 0; i < nt->FileHeader.NumberOfSections; i++, sec++) 
        {
            if (!(sec->Characteristics & IMAGE_SCN_MEM_EXECUTE)) continue;
            if (cached_cnt >= 16) break;

            uintptr_t va = (uintptr_t)mod + sec->VirtualAddress;
            SIZE_T sz = sec->SizeOfRawData;
            if (!sz) continue;

            PVOID buf = nullptr; SIZE_T bs = sz;
            syscall<NTSTATUS>("NtAllocateVirtualMemory", (HANDLE)-1, &buf, (ULONG_PTR)0, &bs, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

            for (uintptr_t pg = va; pg < va + sz; pg += 0x1000)
            {
                SIZE_T cmp = min((SIZE_T)0x1000, va + sz - pg);
                DWORD old;
                PVOID p = (void*)pg; SIZE_T psz = 0x1000;
                syscall<NTSTATUS>("NtProtectVirtualMemory", (HANDLE)-1, &p, &psz, PAGE_EXECUTE_READ, &old);
                memcpy((uint8_t*)buf + (pg - va), (void*)pg, cmp);
                p = (void*)pg; psz = 0x1000;
                syscall<NTSTATUS>("NtProtectVirtualMemory", (HANDLE)-1, &p, &psz, old, &old);
            }

            cached[cached_cnt] = { va, sz, sec->PointerToRawData, crc::calc((uint8_t*)buf, (uint32_t)sz) };
            cached_cnt++;


            PVOID fb = buf; SIZE_T fs = 0;
            syscall<NTSTATUS>("NtFreeVirtualMemory", (HANDLE)-1, &fb, &fs, MEM_RELEASE);
        }

#if LOGGING
        printf("cached %lu section crcs from mem\n", cached_cnt);
#endif
        return cached_cnt > 0;
    }

    __forceinline void check_section(DWORD idx)
    {
        if (!g::disk_img || !g::disk_img_sz || idx >= cached_cnt) return;

        sec_info* ci = &cached[idx];
        SIZE_T sz = ci->size;
        if (ci->dsk_off + sz > g::disk_img_sz) return;

        PVOID buf = nullptr; SIZE_T bs = sz;
        syscall<NTSTATUS>("NtAllocateVirtualMemory", (HANDLE)-1, &buf, (ULONG_PTR)0, &bs, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        for (uintptr_t pg = ci->va; pg < ci->va + sz; pg += 0x1000) 
        {
            SIZE_T cmp = min((SIZE_T)0x1000, ci->va + sz - pg);
            DWORD old;
            PVOID p = (void*)pg; SIZE_T psz = 0x1000;
            syscall<NTSTATUS>("NtProtectVirtualMemory", (HANDLE)-1, &p, &psz, PAGE_EXECUTE_READ, &old);
            memcpy((uint8_t*)buf + (pg - ci->va), (void*)pg, cmp);
            p = (void*)pg; psz = 0x1000;
            syscall<NTSTATUS>("NtProtectVirtualMemory", (HANDLE)-1, &p, &psz, old, &old);

            // re encryt the page if its currently decryped so it matches
            uintptr_t base = pg_base(pg);
            auto it = pg::map.find(base);
            if (it != pg::map.end() && it->second.decrypted)
            {
                pg_encrypt((uint8_t*)buf + (pg - ci->va));
            }
        }

        uint32_t mem_crc = crc::calc((uint8_t*)buf, (uint32_t)sz);
        uint32_t dsk_crc = crc::calc((uint8_t*)g::disk_img + ci->dsk_off, (uint32_t)sz);

#if LOGGING
        //if (idx == 0)  printf("[check] sec0: mem=0x%X disk=0x%X cached=0x%X buf[0]=0x%02X dsk[0]=0x%02X\n", mem_crc, dsk_crc, ci->crc, ((uint8_t*)buf)[0], ((uint8_t*)g::disk_img + ci->dsk_off)[0]);
#endif

        bool fail = (mem_crc != dsk_crc);
        if (mem_crc != ci->crc) fail = true;
        if (dsk_crc != ci->crc) fail = true;

        if (fail) 
        {
#if LOGGING
            printf("crc fail section %lu cached=0x%X disk=0x%X mem=0x%X\n", idx, ci->crc, dsk_crc, mem_crc);
#endif
        }

        PVOID fb = buf; SIZE_T fs = 0;
        syscall<NTSTATUS>("NtFreeVirtualMemory", (HANDLE)-1, &fb, &fs, MEM_RELEASE);
    }

    __forceinline void check() 
    {
        EnterCriticalSection(&g::int_lock);
        if (!load_disk_img()) { LeaveCriticalSection(&g::int_lock); return; }

        for (DWORD i = 0; i < cached_cnt; i++)
            check_section(i);

        free_disk_img();
        LeaveCriticalSection(&g::int_lock);
    }
}

namespace reenc
{
    __forceinline void do_reenc(uintptr_t base
#if LOGGING
        , const char* who
#endif
    )
    {
        if (!in_range(base)) return;

        EnterCriticalSection(&g::pg_lock);
        auto it = pg::map.find(base);
        if (it == pg::map.end() || !it->second.decrypted) 
        {
            LeaveCriticalSection(&g::pg_lock);
            return;
        }

        DWORD old;
        PVOID p = (void*)base; SIZE_T sz = 0x1000;
        syscall<NTSTATUS>("NtProtectVirtualMemory", (HANDLE)-1, &p, &sz, PAGE_READWRITE, &old);
        pg_encrypt((uint8_t*)base);
        p = (void*)base; sz = 0x1000;
        syscall<NTSTATUS>("NtProtectVirtualMemory", (HANDLE)-1, &p, &sz, PAGE_NOACCESS, &old);

        it->second.decrypted = false;
        it->second.owner_tid = 0;
        LeaveCriticalSection(&g::pg_lock);

#if LOGGING
        printf("[%s] re encrypt 0x%p\n", who, (void*)base);
#endif
    }

    __forceinline void sweep(uintptr_t except_pg) 
    {
        ULONGLONG now = ts_ms();
        std::vector<uintptr_t> todo;

        EnterCriticalSection(&g::pg_lock);
        for (auto& [base, info] : pg::map)
            if (base != except_pg && info.decrypted && (now - info.last_access) >= 20)
                todo.push_back(base);
        LeaveCriticalSection(&g::pg_lock);

        for (uintptr_t p : todo)
#if LOGGING
            do_reenc(p, "sweep");
#else
            do_reenc(p);
#endif
    }

    __forceinline void reenc_prev_tid(DWORD tid, uintptr_t cur) 
    {
        EnterCriticalSection(&g::tid_lock);
        auto it = pg::tid_map.find(tid);
        if (it != pg::tid_map.end())
        {
            uintptr_t prev = it->second.last_page;
            if (prev && prev != cur && in_range(prev))
            {
                if (!rel::friendly(prev, cur))
                {
                    LeaveCriticalSection(&g::tid_lock);
#if LOGGING
                    do_reenc(prev, "tid_sw");
#else
                    do_reenc(prev);
#endif
                    rel::record(prev, cur);
                    return;
                }
                rel::record(prev, cur);
            }
        }
        LeaveCriticalSection(&g::tid_lock);
    }

    DWORD WINAPI thread_proc(void*)
    {
#if LOGGING
        printf("[reenc] tid=%lu\n", __readgsdword(0x48));
#endif
        while (!g::stop) 
        {
            g::thread_hb = ts_ms();
            Sleep(100);
            sweep(0);
            integrity::check();
        }
        return 0;
    }

    VOID CALLBACK timer_cb(void*, BOOLEAN) 
    {
        g::timer_hb = ts_ms();
        sweep(0);
        integrity::check();
    }

    DWORD WINAPI watchdog(void*)
    {
#if LOGGING
        printf("[watchdog] tid=%lu\n", __readgsdword(0x48));
#endif
        DWORD tick = 0;
        while (!g::stop) 
        {
            g::watch_hb = ts_ms();
            Sleep(10);

            ULONGLONG now = ts_ms();

            if (now - g::thread_hb > 1000) 
            {
#if LOGGING
                printf("[watchdog] reenc thread suspended\n");
#endif
                if (g::reenc_thrd)
                {
                    ULONG prev = 0;
                    syscall<NTSTATUS>("NtSuspendThread", g::reenc_thrd, &prev);
                    DWORD cnt = prev;
                    ULONG prev2 = 0;
                    syscall<NTSTATUS>("NtResumeThread", g::reenc_thrd, &prev2);
                    if (cnt > 0)
                    {
                        ULONG prev3 = 0;
                        syscall<NTSTATUS>("NtResumeThread", g::reenc_thrd, &prev3);
#if LOGGING
                        printf("[watchdog] fixed suspension cnt=%lu\n", cnt);
#endif
                    }
                }
            }

            sweep(0);
            rel::cleanup();

            if (++tick >= 100)
            {
                tick = 0;
                integrity::check();
            }
        }
        return 0;
    }

    __forceinline void init_reenc()
    {
        g::stop = false;

        g::timer_q = CreateTimerQueue();
        if (g::timer_q)
        {
            CreateTimerQueueTimer(&g::timer_h, g::timer_q,
                timer_cb, nullptr, 100, 100, WT_EXECUTEINTIMERTHREAD);
            g::timer_hb = ts_ms();
        }

        g::thread_hb = ts_ms();
        OBJECT_ATTRIBUTES oa{}; oa.Length = sizeof(oa);
        syscall<NTSTATUS>("NtCreateThreadEx", &g::reenc_thrd, THREAD_ALL_ACCESS, &oa, (HANDLE)-1, (PVOID)thread_proc, nullptr, (ULONG)0, (ULONG_PTR)0, (ULONG_PTR)0, nullptr, nullptr);
        if (g::reenc_thrd) SetThreadPriority(g::reenc_thrd, THREAD_PRIORITY_ABOVE_NORMAL);

        g::watch_hb = ts_ms();
        OBJECT_ATTRIBUTES oa2{}; oa2.Length = sizeof(oa2);
        syscall<NTSTATUS>("NtCreateThreadEx", &g::watch_thrd, THREAD_ALL_ACCESS, &oa2, (HANDLE)-1, (PVOID)watchdog, nullptr, (ULONG)0, (ULONG_PTR)0, (ULONG_PTR)0, nullptr, nullptr);
        if (g::watch_thrd) SetThreadPriority(g::watch_thrd, THREAD_PRIORITY_HIGHEST);

#if LOGGING
        printf("timer=%s reenc=%s watchdog=%s\n",
            g::timer_q ? "ok" : "fail",
            g::reenc_thrd ? "ok" : "fail",
            g::watch_thrd ? "ok" : "fail");
#endif
    }
}

namespace ki {
    void handler(void* exc_rec, void* ctx_rec)
    {
        auto* er = (PEXCEPTION_RECORD)exc_rec;
        auto* ctx = (PCONTEXT)ctx_rec;

        if (t_in_handler) { t_in_handler = 0; return; }
        t_in_handler = 1;

        uintptr_t addr = 0;
        uintptr_t base = 0;
        DWORD ft = 0;
        bool should_handle = false;

#if LOGGING
        printf("handler ExceptionCode: 0x%08X, Address: 0x%p\n",
            er->ExceptionCode, er->ExceptionAddress);
#endif

        if (er->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && er->NumberParameters >= 2)
        {
            ft = (DWORD)er->ExceptionInformation[0];
            addr = er->ExceptionInformation[1];

            if (ft == 8)
            { //maybe check execute came from us
                should_handle = true;
            }
            else if (ft == 0)
            {

                // onlu approve read if it came from currently ecrypted, i wouldnt do this unless u need to
                uintptr_t rip = ctx->Rip;
                uintptr_t rip_base = pg_base(rip);
                EnterCriticalSection(&g::pg_lock);
                auto rip_it = pg::map.find(rip_base);
                should_handle = (rip_it != pg::map.end()
                    && rip_it->second.decrypted
                    && in_range(rip_base));
                LeaveCriticalSection(&g::pg_lock);
            }
        }
        else if (er->ExceptionCode == EXCEPTION_ILLEGAL_INSTRUCTION)
        { // this works without the pages being no access but its better for security and there is a small chanve the xor outputs a valid instruction
            addr = (uintptr_t)er->ExceptionAddress;
            ft = 8;
            should_handle = true;
        }

        if (should_handle)
        {
            base = pg_base(addr);

            if (in_range(base))
            {
                DWORD tid = __readgsdword(0x48);

#if LOGGING
                printf("fault %s 0x%p pg=0x%p tid=%lu\n",
                    ft == 8 ? "X" : ft == 1 ? "W" : "R",
                    (void*)addr, (void*)base, tid);
#endif

                EnterCriticalSection(&g::pg_lock);
                auto it = pg::map.find(base);
                bool need_dec = (it == pg::map.end() || !it->second.decrypted);

                if (need_dec)
                {
                    if (it == pg::map.end()) {
                        pg::map[base] = {};
                        pg::map[base].base = base;
                        it = pg::map.find(base);
                    }

                    DWORD old;
                    PVOID p = (void*)base; SIZE_T sz = 0x1000;
                    syscall<NTSTATUS>("NtProtectVirtualMemory", (HANDLE)-1, &p, &sz, PAGE_READWRITE, &old);
                    pg_decrypt((uint8_t*)base);

                    DWORD prot = (ft == 8) ? PAGE_EXECUTE_READ : (ft == 1) ? PAGE_READWRITE : PAGE_READONLY;
                    p = (void*)base; sz = 0x1000;
                    syscall<NTSTATUS>("NtProtectVirtualMemory", (HANDLE)-1, &p, &sz, prot, &old);

                    ULONGLONG now = ts_ms();
                    it->second.last_access = now;
                    it->second.dec_time = now;
                    it->second.access_cnt = 1;
                    it->second.owner_tid = tid;
                    it->second.decrypted = true;

#if LOGGING
                    printf("fault dec 0x%p prot=0x%X\n", (void*)base, prot);
#endif
                }

                else
                {
                    // already decrypted bump last_access to delay reenc timer
                    it->second.last_access = ts_ms();
                    it->second.access_cnt++;
                }
                LeaveCriticalSection(&g::pg_lock);

                reenc::reenc_prev_tid(tid, base);

                EnterCriticalSection(&g::tid_lock);
                pg::tid_map[tid] = { tid, base, ts_ms() };
                LeaveCriticalSection(&g::tid_lock);

                if (InterlockedIncrement(&g::fault_ctr) % 9 == 0)
                {
                    reenc::sweep(base);
                    integrity::check();
                }

                syscall<NTSTATUS>("NtFlushInstructionCache", (HANDLE)-1, (void*)base, (SIZE_T)0x1000);
                _mm_mfence();

                t_in_handler = 0;
                syscall<NTSTATUS>("NtContinue", ctx, FALSE);
            }
        }
    }


    __forceinline void** find_wow64_ptr()
    {
        uint8_t* ntdll = find_mod(L"ntdll.dll", 9);
        if (!ntdll) return nullptr;
        g::ki_disp = (uint8_t*)get_proc(ntdll, "KiUserExceptionDispatcher");
        if (!g::ki_disp) return nullptr;

        // 48 8B 05 ?? ?? ?? ?? 48 85 C0 74 ?? 48 8B CC
        uint8_t    pat[] = { 0x48,0x8B,0x05,0,0,0,0,0x48,0x85,0xC0,0x74,0,0x48,0x8B,0xCC };
        const char msk[] = "xxx????xxxx?xxx";

        for (int i = 0; i < 0x100; i++)
        {
            uint8_t* p = g::ki_disp + i;
            bool ok = true;
            for (int j = 0; j < (int)sizeof(pat); j++)
                if (msk[j] == 'x' && p[j] != pat[j]) { ok = false; break; }
            if (!ok) continue;

            void** ptr = (void**)(p + 7 + *(DWORD*)(p + 3));
#if LOGGING
            printf("wow64 ptr=0x%p val=0x%p\n", ptr, *ptr);
#endif
            return ptr;
        }
        return nullptr;
    }

    __forceinline bool hook()
    {
        g::wow64_ptr = find_wow64_ptr();
        if (!g::wow64_ptr) return false;

        g::wow64_orig = *g::wow64_ptr;
        DWORD old;
        PVOID p = g::wow64_ptr; SIZE_T sz = sizeof(void*);
        syscall<NTSTATUS>("NtProtectVirtualMemory", (HANDLE)-1, &p, &sz, PAGE_READWRITE, &old);
        *g::wow64_ptr = (void*)handler;
        p = g::wow64_ptr; sz = sizeof(void*);
        syscall<NTSTATUS>("NtProtectVirtualMemory", (HANDLE)-1, &p, &sz, old, &old);

        g::hooked = true;
#if LOGGING
        printf("hooked orig=0x%p\n", g::wow64_orig);
#endif
        return true;
    }
}

namespace enc {
    __forceinline bool install(const char* sec_name) 
    {
        InitializeCriticalSection(&g::pg_lock);
        InitializeCriticalSection(&g::tid_lock);
        InitializeCriticalSection(&g::rel_lock);
        InitializeCriticalSection(&g::int_lock);

        if (!integrity::cache_crc())
        {
#if LOGGING
            printf("disk img failed\n");
#endif
        }

        if (!ki::hook()) return false;

        auto peb = (uint8_t*)__readgsqword(0x60);
        HMODULE mod = *(HMODULE*)(peb + 0x10);
        auto* sec = find_sec(mod, sec_name);
        if (!sec) {
#if LOGGING
            printf("section not found\n");
#endif
            return false;
        }

        g::range_start = (uintptr_t)mod + sec->VirtualAddress;
        g::range_end = g::range_start + sec->Misc.VirtualSize - 1;

#if LOGGING
        printf("%s 0x%p-0x%p\n", sec_name, (void*)g::range_start, (void*)g::range_end);
#endif

        for (uintptr_t a = g::range_start; a <= g::range_end; a += 0x1000)
        {
            DWORD old;
            PVOID p = (void*)a; SIZE_T sz = 0x1000;
            syscall<NTSTATUS>("NtProtectVirtualMemory", (HANDLE)-1, &p, &sz, PAGE_NOACCESS, &old);
        }

        reenc::init_reenc();
        return true;
    }
}

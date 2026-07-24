# jit-page-decryptor
** x64 header‑only anti‑tamper, anti-dump, and anti-debug library that keeps a PE section and pages encrypted in memory and on disk until it executes and automatically re encrypts it as soon as its done**

**obviously not paste ready, at least replace the xor**

**what it does**

post build a designated section with the protected code is encrypted

pages are no access, only execute faults decrypt the page, reads are denied unless the faulting code is already inside a decrypted page

pages are automatically re encrypted if not in use or if execution leaves the page

example of an integrity check, it compares current crc, disk, and crc it cached on install

**how it works**

on install it finds the Wow64PrepareForException pointer inside KiUserExceptionDispatcher which is always called if the pointer is valid which it usually isnt on x64, replaces it with our handler, and sets every page in the target section to no access

when the protected code gets excecuted our handler sees the fault and checks if its an execute fault if it is it restores protection and decrypts the page and continues the execution 

a timer and worker thread re encrypt pages automatically that arent in use and the handler also re encrypts if execution leaves the page

**Usage**

```cpp
#include "jit-page-decryptor.h"

__declspec(code_seg(".prot")) void protected_func() //xor or encrypt the .prot section post build
{
	printf("yo\n");
}

int main() {
    
    if (!enc::install(".prot")) 
	{
        return 1;
    }
    
	protected_func();
}
```
theres obviously a lot of ways to improve this and thins to chang before actual use, this is more of a poc

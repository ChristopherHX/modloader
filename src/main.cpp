#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <android/dlext.h>
#include <vector>
#ifdef NDEBUG
#define printf(...)
#endif

#include <setjmp.h>

void __siglongjmp(sigjmp_buf env, int val) {

}

int __sigsetjmp(sigjmp_buf env, int savemask) {
    return -1;
}

extern "C" __attribute__ ((visibility ("default"))) void mod_preinit() {
    android_dlextinfo ext;
    ext.flags = ANDROID_DLEXT_MCPELAUNCHER_HOOKS;
    std::vector<mcpelauncher_hook_t> hooks;
    hooks.push_back({"siglongjmp", (void*)&__siglongjmp});
    hooks.push_back({"sigsetjmp", (void*)&__sigsetjmp});
    hooks.push_back({(const char*)nullptr, (void*)nullptr});
    ext.mcpelauncher_hooks = hooks.data();
    // Openssl crashs at the moment if unset
    setenv("OPENSSL_armcap", "0", 0);
    auto crypto = android_dlopen_ext("libcrypto.so", 0, &ext);
    if(!crypto) {
        printf("crypto dlerror: %s\n", dlerror());
    }
    auto ssl = android_dlopen_ext("libssl.so", 0, &ext);
    if(!ssl) {
        printf("ssl dlerror: %s\n", dlerror());
    }
    auto mod = android_dlopen_ext("libmcpelauncherupdates.so", 0, &ext);
    if(!mod) {
        printf("mcpelauncherupdates dlerror: %s\n", dlerror());
    } else {
        auto _mod_preinit = (decltype(&mod_preinit))dlsym(mod, "mod_preinit");
        if(_mod_preinit != nullptr) {
            _mod_preinit();
        }
    }

}

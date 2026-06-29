#include <jni.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <android/log.h>
#include "zygisk.hpp"

#define TARGET_PACKAGE "com.dts.freefiremax"

uintptr_t il2cpp_base = 0;

// =======================================================
// SEMUA OFFSET UTUH UNTUK ESP LINE (FREE FIRE MAX)
// =======================================================
#define OFFSET_GET_MAIN                 0xa7ed6c0
#define OFFSET_WORLD_TO_SCREEN          0xa7ed344
#define OFFSET_GET_POSITION             0x8857b00
#define OFFSET_GET_PLAYER_COUNT         0x645d5c4
#define OFFSET_GET_LOCAL_PLAYER         0x64cbde8
#define OFFSET_GET_PLAYER_LIST_TEAM     0x645d00c 

struct Vector3 { float x, y, z; };
struct MonoArray { void* klass; void* monitor; void* bounds; int max_length; void* vector; };

uintptr_t ambil_alamat_il2cpp() {
    uintptr_t addr = 0;
    char line[512]; 
    FILE* fp = fopen("/proc/self/maps", "r");
    if (fp) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, "libil2cpp.so")) {
                addr = strtoul(line, NULL, 16);
                break;
            }
        }
        fclose(fp);
    }
    return addr;
}

void* LoopLatarBelakang(void*) {
    do {
        il2cpp_base = ambil_alamat_il2cpp();
        usleep(500000); 
    } while (!il2cpp_base);

    // Otomatis memuat script visual .so luar buatan mas jika ditaruh di folder HP ini
    dlopen("/data/local/tmp/libESPline.so", RTLD_NOW);
    
    __android_log_print(ANDROID_LOG_INFO, "ZygiskLoader", "Base Memori FFMAX Ditemukan di: %p", (void*)il2cpp_base);
    return nullptr;
}

class MyModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        const char* process_name = env->GetStringUTFChars(args->nice_name, nullptr);
        if (process_name && strcmp(process_name, TARGET_PACKAGE) == 0) {
            pthread_t t;
            pthread_create(&t, nullptr, LoopLatarBelakang, nullptr);
        }
        env->ReleaseStringUTFChars(args->nice_name, process_name);
    }
private:
    zygisk::Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(MyModule)

#pragma once

#include <jni.h>
#include <stdint.h>

namespace zygisk {

struct AppSpecializeArgs {
    jint &uid;
    jint &gid;
    jintArray &gids;
    jint &runtime_flags;
    jobjectArray &rlimits;
    jint &mount_external;
    jstring &se_info;
    jstring &nice_name;
    jobjectArray &is_child_zygote;
    jstring &instruction_set;
    jstring &app_data_dir;
    jboolean &is_top_app;
    jobjectArray &pkg_data_info_list;
    jobjectArray &whitelisted_data_info_list;
    jboolean &bind_mount_app_data_dirs;
    jboolean &bind_mount_app_storage_dirs;
};

struct ServerSpecializeArgs {
    jint &uid;
    jint &gid;
    jintArray &gids;
    jint &runtime_flags;
    jobjectArray &rlimits;
    jlong &permitted_capabilities;
    jlong &effective_capabilities;
};

class Api {
public:
    virtual int getApiVersion() = 0;
    virtual void setOption(int option) = 0;
    virtual int getFlags() = 0;
    virtual void connectCompanion() = 0;
};

class ModuleBase {
public:
    virtual void onLoad(Api *api, JNIEnv *env) {}
    virtual void preAppSpecialize(AppSpecializeArgs *args) {}
    virtual void postAppSpecialize(const AppSpecializeArgs *args) {}
    virtual void preServerSpecialize(ServerSpecializeArgs *args) {}
    virtual void postServerSpecialize(const ServerSpecializeArgs *args) {}
};

}

#define REGISTER_ZYGISK_MODULE(clazz) \
extern "C" [[gnu::visibility("default")]] void zygisk_module_entry(void *api, void *env) { \
    static clazz module; \
    static bool loaded = false; \
    if (!loaded) { \
        loaded = true; \
        module.onLoad(static_cast<zygisk::Api*>(api), static_cast<JNIEnv*>(env)); \
    } \
}

#include <napi.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

// TODO: figure out how to write tests for this

Napi::Value throwError(Napi::Env env, const char *message) {
    Napi::Error::New(env, message).ThrowAsJavaScriptException();
    return env.Null();
}

Napi::Value throwTypeError(Napi::Env env, const char *message) {
    Napi::TypeError::New(env, message).ThrowAsJavaScriptException();
    return env.Null();
}

Napi::Object userInfoToJs(Napi::Env env, passwd *info) {
    Napi::Object out = Napi::Object::New(env);
    out["name"] = Napi::String::New(env, info->pw_name);
    out["password"] = Napi::String::New(env, info->pw_passwd);
    out["uid"] = Napi::Number::New(env, info->pw_uid);
    out["gid"] = Napi::Number::New(env, info->pw_gid);
    out["gecos"] = Napi::String::New(env, info->pw_gecos);
    out["homedir"] = Napi::String::New(env, info->pw_dir);
    out["shell"] = Napi::String::New(env, info->pw_shell);
    return out;
}

Napi::Object groupInfoToJs(Napi::Env env, group *info) {
    Napi::Object out = Napi::Object::New(env);
    out["name"] = Napi::String::New(env, info->gr_name);
    out["password"] = Napi::String::New(env, info->gr_passwd);
    out["gid"] = Napi::Number::New(env, info->gr_gid);
    Napi::Array members = Napi::Array::New(env);
    out["members"] = members;
    for (int i = 0; info->gr_mem[i] != 0; i++) {
        members[i] = Napi::String::New(env, info->gr_mem[i]);
    }
    return out;
}

Napi::Value getUserInfoName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) return throwTypeError(env, "Needs 1 argument");
    if (!info[0].IsString()) return throwTypeError(env, "Argument should be a string");
    std::string username = info[0].ToString();
    passwd *result = getpwnam(username.c_str());
    if (result == NULL) return throwError(env, "No such user or something died");
    return userInfoToJs(env, result);
}

Napi::Value getUserInfoId(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) return throwTypeError(env, "Needs 1 argument");
    if (!info[0].IsNumber()) return throwTypeError(env, "Argument should be an integer");
    uid_t uid = info[0].ToNumber().Int32Value();
    passwd *result = getpwuid(uid);
    if (result == NULL) return throwError(env, "No such user or something died");
    return userInfoToJs(env, result);
}

Napi::Value getGroupInfoName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) return throwTypeError(env, "Needs 1 argument");
    if (!info[0].IsString()) return throwTypeError(env, "Argument should be a string");
    std::string grp = info[0].ToString();
    group *result = getgrnam(grp.c_str());
    if (result == NULL) return throwError(env, "No such group or something died");
    return groupInfoToJs(env, result);
}

Napi::Value getGroupInfoId(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) return throwTypeError(env, "Needs 1 argument");
    if (!info[0].IsNumber()) return throwTypeError(env, "Argument should be an integer");
    gid_t gid = info[0].ToNumber().Int32Value();
    group *result = getgrgid(gid);
    if (result == NULL) return throwError(env, "No such group or something died");
    return groupInfoToJs(env, result);
}

Napi::Value getGids(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) return throwTypeError(env, "Needs 1 argument");
    if (!info[0].IsString()) return throwTypeError(env, "Argument should be a string");
    std::string username = info[0].ToString();
    passwd *user = getpwnam(username.c_str());
    if (user == NULL) return throwError(env, "No such user or something died");
    int ngroups = 5;
    gid_t *groups = new gid_t[ngroups];
    if (getgrouplist(username.c_str(), user->pw_gid, groups, &ngroups) == -1) {
        delete[] groups;
        groups = new gid_t[ngroups];
        getgrouplist(username.c_str(), user->pw_gid, groups, &ngroups);
    }
    Napi::Array out = Napi::Array::New(env);
    for (int i = 0; i < ngroups; i++) out[i] = groups[i];
    delete[] groups;
    return out;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports["getUserInfoByName"] = Napi::Function::New(env, getUserInfoName);
    exports["getUserInfoByUid"] = Napi::Function::New(env, getUserInfoId);
    exports["getGroupInfoByName"] = Napi::Function::New(env, getGroupInfoName);
    exports["getGroupInfoByGid"] = Napi::Function::New(env, getGroupInfoId);
    exports["getGids"] = Napi::Function::New(env, getGids);
    return exports;
}

NODE_API_MODULE(addon, Init)

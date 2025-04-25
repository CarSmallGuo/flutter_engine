#include "dynamic_library_loader.h"
#include "flutter/fml/logging.h"
#include "deviceinfo.h"

namespace flutter {

DynamicLibraryLoader::DynamicLibraryLoader(const char* lib_name)
    : handle_(nullptr), api_version_(0) {
  api_version_ = OH_GetSdkApiVersion();

  handle_ = dlopen(lib_name, RTLD_LAZY | RTLD_LOCAL);
  if (!handle_) {
    FML_LOG(ERROR) << "dlopen(" << lib_name << ") failed: " << dlerror();
  }
}

DynamicLibraryLoader::~DynamicLibraryLoader() {
  if (handle_) {
    dlclose(handle_);
  }
}

void DynamicLibraryLoader::LoadSymbols(const std::vector<SymbolInfo>& entries) {
  if (!handle_) return;

  for (const auto& entry : entries) {
    if (api_version_ >= entry.min_api) {
      *entry.target = dlsym(handle_, entry.name);
      if (*entry.target == nullptr) {
        FML_LOG(ERROR) << "dlsym failed for " << entry.name << ": " << dlerror();
      } else {
        FML_LOG(INFO) << "Loaded symbol " << entry.name;
      }
    } else {
      FML_LOG(INFO) << "Skipping " << entry.name
                    << " (requires API >= " << entry.min_api
                    << ", current = " << api_version_ << ")";
    }
  }
}

}  // namespace flutter
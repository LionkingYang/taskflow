load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")


git_repository(
    name = "oneTBB",
    tag = "v2021.5.0",
    remote = "https://github.com/oneapi-src/oneTBB/",
)

_RAPIDJSON_BUILD_FILE = """
cc_library(
    name = "rapidjson",
    hdrs = glob(["include/rapidjson/**/*.h"]),
    includes = ["include"],
    defines = ["RAPIDJSON_HAS_STDSTRING=1"],
    visibility = [ "//visibility:public" ],
)
"""
rapidjson_ver =  "1.1.0"
rapidjson_name = "rapidjson-1.1.0"
http_archive(
    name = "rapidJson",
    strip_prefix = rapidjson_name,
    urls = [
        "https://github.com/Tencent/rapidjson/archive/v{ver}.tar.gz".format(ver = rapidjson_ver),
    ],
    build_file_content = _RAPIDJSON_BUILD_FILE,
)
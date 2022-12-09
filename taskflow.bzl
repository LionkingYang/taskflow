load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def clean_dep(dep):
    return str(Label(dep))

def taskflow_workspace():
    git_repository(
        name = "oneTBB",
        remote = "https://github.com/oneapi-src/oneTBB/",
        tag = "v2021.5.0",
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

    rapidjson_ver = "1.1.0"

    rapidjson_name = "rapidjson-1.1.0"

    http_archive(
        name = "rapidJson",
        build_file_content = _RAPIDJSON_BUILD_FILE,
        strip_prefix = rapidjson_name,
        urls = [
            "https://github.com/Tencent/rapidjson/archive/v{ver}.tar.gz".format(
                ver = rapidjson_ver,
            ),
        ],
    )

    fmtlib_ver = "7.1.3"

    fmtlib_name = "fmt-{ver}".format(ver = fmtlib_ver)

    _FMT_BUILD_FILE = """
cc_library(
    name = "com_github_fmtlib_fmt",
    hdrs = glob(["include/fmt/**/*.h"]),
    includes = ["include"],
    defines = [],
    visibility = [ "//visibility:public" ],
)
    """

    http_archive(
        name = "com_github_fmtlib_fmt",
        build_file_content = _FMT_BUILD_FILE,
        strip_prefix = fmtlib_name,
        urls = [
            "https://github.com/fmtlib/fmt/archive/{ver}.tar.gz".format(ver = fmtlib_ver),
        ],
    )

    _SPDLOG_BUILD_FILE = """
cc_library(
    name = "com_github_spdlog",
    hdrs = glob(["include/spdlog/**/*.h"]),
    includes = ["include"],
    defines = ["SPDLOG_HEADER_ONLY=1"],
    visibility = [ "//visibility:public" ],
)
    """

    spdlog_ver = "1.8.2"

    spdlog_name = "spdlog-{ver}".format(ver = spdlog_ver)

    http_archive(
        name = "com_github_spdlog",
        build_file_content = _SPDLOG_BUILD_FILE,
        strip_prefix = spdlog_name,
        urls = [
            "https://github.com/gabime/spdlog/archive/v{ver}.tar.gz".format(ver = spdlog_ver),
        ],
    )

    git_repository(
        name = "com_github_absl",
        remote = "https://github.com/abseil/abseil-cpp.git",
        tag = "20211102.0",
    )

    http_archive(
        name = "rules_foreign_cc",
        strip_prefix = "rules_foreign_cc-0.9.0",
        url = "https://github.com/bazelbuild/rules_foreign_cc/archive/0.9.0.zip",
    )

    http_archive(
        name = "com_github_jemalloc",
        build_file = clean_dep("//bazel:jemalloc.BUILD"),
        strip_prefix = "jemalloc-5.3.0",
        urls = [
            "https://github.com/jemalloc/jemalloc/archive/5.3.0.zip",
        ],
    )

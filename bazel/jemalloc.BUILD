load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_foreign_cc//foreign_cc:configure.bzl", "configure_make")

package(
    default_visibility = ["//visibility:public"],
)

filegroup(
    name = "all",
    srcs = glob(["**"]),
    visibility = ["//visibility:public"],
)

# for global
configure_make(
    name = "libjemalloc",
    autogen = True,
    configure_in_place = True,
    lib_source = "@com_github_jemalloc//:all",
    out_static_libs = [
        "libjemalloc.a",
    ],
)

cc_library(
    name = "jemalloc",
    deps = [
        "libjemalloc",
    ],
)

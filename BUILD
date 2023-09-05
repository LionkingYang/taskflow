load("@rules_cc//cc:defs.bzl", "cc_binary")

package(
    default_visibility = ["//visibility:public"],
)

cc_binary(
    name = "math_test",
    srcs = [
        "example/math_test/main.cpp",
    ],
    malloc = "@com_github_jemalloc//:jemalloc",
    deps = ["//:task_flow_dep"],
)

cc_binary(
    name = "recmd_op",
    srcs = glob([
        "example/recmd_test/ops/*.cpp",
        "example/recmd_test/deps/*.h",
        "example/recmd_test/deps/*.cpp",
    ]),
    linkopts = [
        "-lpthread",
        "-rdynamic",
        "-fPIC -shared",
    ],
    linkshared = True,
    linkstatic = True,
    deps = ["//:task_flow_dep"],
)

cc_binary(
    name = "math_op",
    srcs = [
        "example/math_test/ops/math_op.cpp",
    ],
    linkopts = [
        "-lpthread",
        "-rdynamic",
        "-fPIC -shared",
    ],
    linkshared = True,
    linkstatic = True,
    deps = ["//:task_flow_dep"],
)

cc_binary(
    name = "generate_op",
    srcs = [
        "example/generate_test/ops/project_op.cpp",
    ],
    linkopts = [
        "-lpthread",
        "-rdynamic",
        "-fPIC -shared",
    ],
    linkshared = True,
    linkstatic = True,
    deps = ["//:task_flow_dep"],
)

cc_binary(
    name = "generate_test",
    srcs = [
        "example/generate_test/project_main.cpp",
    ],
    deps = ["//:task_flow_dep"],
)

cc_binary(
    name = "recmd_test",
    srcs = glob([
        "example/recmd_test/**/*.cpp",
        "example/recmd_test/**/*.h",
    ]),
    malloc = "@com_github_jemalloc//:jemalloc",
    deps = ["//:task_flow_dep"],
)

cc_library(
    name = "task_flow_dep",
    srcs = glob(["src/**/*.cpp"]),
    hdrs = glob(["taskflow/include/**/*.h"]),
    linkopts = [
        "-lpthread",
        "-rdynamic",
        "-fPIC",
        "-ldl",
        "-lstdc++fs",
    ],
    deps = [
        "@com_github_absl//absl/strings",
        "@com_github_absl//absl/time",
        "@com_github_fmtlib_fmt",
        "@com_github_spdlog",
        "@com_github_taskflow",
        "@oneTBB//:tbb",
        "@rapidJson//:rapidjson",
    ],
)

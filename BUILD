load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "math_test",
    srcs = [
        "test/math_test/main.cpp",
        "test/math_test/math_op.h",
    ],
    deps = ["//:task_flow_dep"],
)

cc_binary(
    name = "recmd_op",
    srcs = [
        "test/recmd_test/ops/recmd_op.cpp",
        "test/recmd_test/ops/struct_define.h",
    ],
    linkopts = [
        "-lpthread",
        "-rdynamic",
        "-fPIC -shared -rdynamic",
    ],
    linkshared = True,
    linkstatic = True,
    deps = ["//:task_flow_dep"],
)

cc_binary(
    name = "math_op",
    srcs = [
        "test/script/math_op.cpp",
    ],
    linkopts = [
        "-lpthread",
        "-rdynamic",
        "-fPIC -shared -rdynamic",
    ],
    linkshared = True,
    linkstatic = True,
    deps = ["//:task_flow_dep"],
)

# cc_binary(
#     name = "generate_test",
#     srcs = [
#         "test/generate_test/project_main.cpp",
#         "test/generate_test/project_op.h",
#     ],
#     deps = ["//:task_flow_dep"],
# )

cc_binary(
    name = "recmd_test",
    srcs = [
        "test/recmd_test/main.cpp",
        "test/recmd_test/ops/struct_define.h",
    ],
    linkopts = [
        "-lpthread",
        "-rdynamic",
        "-fPIC",
        "-ldl",
    ],
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
    ],
    deps = [
        "@com_github_absl//absl/strings",
        "@com_github_fmtlib_fmt",
        "@com_github_spdlog",
        "@oneTBB//:tbb",
        "@rapidJson//:rapidjson",
    ],
)

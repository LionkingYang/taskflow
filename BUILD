load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "math_test",
    srcs = [
        "test/math_test/main.cpp",
        "test/math_test/math_op.h",
    ],
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
        "test/recmd_test/recmd_op.h",
        "test/recmd_test/struct_define.h",
    ],
    deps = ["//:task_flow_dep"],
)

cc_library(
    name = "task_flow_dep",
    srcs = glob(["src/**/*.cpp"]),
    hdrs = glob(["taskflow/include/**/*.h"]),
    linkopts = [
        "-lpthread",
    ],
    deps = [
        "@com_github_fmtlib_fmt",
        "@com_github_spdlog",
        "@oneTBB//:tbb",
        "@rapidJson//:rapidjson",
    ],
)

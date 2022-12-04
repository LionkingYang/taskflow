load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "math_test",
    srcs = [
        "taskflow/test/math_test/main.cpp",
        "taskflow/test/math_test/math_op.h",
    ],
    deps = ["//:task_flow_dep"],
)

# cc_binary(
#     name = "generate_test",
#     srcs = [
#         "taskflow/test/generate_test/project_main.cpp",
#         "taskflow/test/generate_test/project_op.h",
#     ],
#     deps = ["//:task_flow_dep"],
# )

cc_binary(
    name = "recmd_test",
    srcs = [
        "taskflow/test/recmd_test/main.cpp",
        "taskflow/test/recmd_test/recmd_op.h",
        "taskflow/test/recmd_test/struct_define.h",
    ],
    deps = ["//:task_flow_dep"],
)

cc_library(
    name = "task_flow_dep",
    srcs = glob(["taskflow/src/**/*.cpp"]),
    hdrs = glob(["taskflow/include/**/*.h"]),
    linkopts = [
        "-lpthread",
    ],
    deps = [
        "@oneTBB//:tbb",
        "@rapidJson//:rapidjson",
    ],
)

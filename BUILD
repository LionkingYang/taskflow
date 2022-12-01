load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "task_flow",
    srcs = ["taskflow/main.cpp"],
    deps = [ "//:task_flow_dep"],
)

cc_library(
    name = "task_flow_dep",
    srcs = glob(["taskflow/src/**/*.cpp"]),
    hdrs = glob(["taskflow/include/**/*.h"]),
    deps = ["@oneTBB//:tbb"],
    linkopts = [
        "-lpthread"
    ],
)

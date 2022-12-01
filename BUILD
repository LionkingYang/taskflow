load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "math_test",
    srcs = ["taskflow/test/math_test/main.cpp","taskflow/test/math_test/math_op.h"],
    deps = [ "//:task_flow_dep"],
)

cc_library(
    name = "task_flow_dep",
    srcs = glob(["taskflow/src/**/*.cpp"]),
    hdrs = glob(["taskflow/include/**/*.h"]),
    deps = ["@oneTBB//:tbb", "@rapidJson//:rapidjson"],
    linkopts = [
        "-lpthread"
    ],
)

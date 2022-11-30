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
    deps = ["//:tbb"],
    linkopts = [
        "-lpthread"
    ],
)

cc_library(
    name = "tbb",
    srcs = ["thirdparty/tbb/libtbb.so"],
    hdrs = glob(["thirdparty/tbb/include/**/*.h"]),
)

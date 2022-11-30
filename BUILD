load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "task_flow",
    srcs = ["taskflow/src/main.cc"],
    deps = [ "//:task_flow_dep"],
)

cc_library(
    name = "task_flow_dep",
    srcs = glob(["**/*.cpp"]),
    hdrs = glob(["**/*.h"]),
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

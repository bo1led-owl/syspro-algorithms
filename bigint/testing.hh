#pragma once

#include <iostream>

#define ANSI_LITERAL(code) "\x1B[" STR(code) "m"

#define COLOR_RESET ANSI_LITERAL(0)
#define COLOR_BOLD ANSI_LITERAL(1)
#define COLOR_RED ANSI_LITERAL(31)
#define COLOR_GREEN ANSI_LITERAL(32)

#define STR(x) #x
#define REPORT(msg) std::cerr << msg
#define REPORT_ASSERTION_FAILED(msg) REPORT(COLOR_RED "ASSERTION FAILED: " COLOR_RESET msg "\n")

#define EXPECT_TRUE(cond)                               \
    if (!cond) {                                        \
        REPORT_ASSERTION_FAILED(STR(cond) " is false"); \
        result = false;                                 \
    }

#define EXPECT_FALSE(cond)                               \
    if (cond) {                                          \
        REPORT_ASSERTION_FAILED(STR(cond) " is true", ); \
        result = false;                                  \
    }

#define EXPECT_EQ(a, b)                                \
    if (a != b) {                                      \
        REPORT_ASSERTION_FAILED(STR(a) " != " STR(b)); \
        result = false;                                \
    }

#define EXPECT_NEQ(a, b)                               \
    if (a == b) {                                      \
        REPORT_ASSERTION_FAILED(STR(a) " == " STR(b)); \
        result = false;                                \
    }

#define TEST(name) void _test_##name(bool& result)
#define RUN_TEST(name)                                                \
    {                                                                 \
        REPORT(COLOR_GREEN "RUN" COLOR_RESET "\t" STR(name) "\n");    \
        bool _test_##name##_result = true;                            \
        _test_##name(_test_##name##_result);                          \
        if (!_test_##name##_result) {                                 \
            REPORT(COLOR_RED "FAIL" COLOR_RESET "\t" STR(name) "\n"); \
        } else {                                                      \
            REPORT(COLOR_GREEN "OK" COLOR_RESET "\t" STR(name) "\n"); \
        }                                                             \
    }

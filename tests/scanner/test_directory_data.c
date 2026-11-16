#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "scanner/data.h"
#include "scanner/directory_data.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define NB_TESTS (sizeof TESTS / sizeof TESTS[0])

typedef struct {
    const char* name;
    bool (*function)(void);
} test;

static char* create_temp_directory(void)
{
    char* path = strdup("/tmp/tofkel-test-XXXXXX");
    assert(path != NULL);
    assert(mkdtemp(path) != NULL);
    return path;
}

static void create_directory(const char* path) { assert(mkdir(path, 0700) == 0); }

static void create_file(const char* path)
{
    FILE* file = fopen(path, "w");
    assert(file != NULL);
    assert(fclose(file) == 0);
}

static void remove_tree(const char* path)
{
    char command[4096];
    int result = snprintf(command, sizeof(command), "rm -rf -- '%s'", path);

    assert(result > 0 && (size_t)result < sizeof(command));
    assert(system(command) == 0);
}

static data_s* find_data(directory_data_s* directory, const char* name)
{
    data_s** data = directory_get_data(directory);
    size_t count = directory_get_nb_data(directory);

    for (size_t i = 0; i < count; i++) {
        if (strcmp(data_get_name(data[i]), name) == 0)
            return data[i];
    }

    return NULL;
}

/* 1. Empty directory */

static bool test_scan_empty_directory(void)
{
    bool err = false;
    char* path = create_temp_directory();

    directory_data_s* result = scan_directory(path, 0);

    fprintf(stderr, "   Test \"result != NULL\" ");
    if (result != NULL) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    fprintf(stderr, "   Test \"directory_get_nb_data(result) == 0\" ");
    if (result != NULL && directory_get_nb_data(result) == 0) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    fprintf(stderr, "   Test \"directory_get_nb_children(result) == 0\" ");
    if (result != NULL && directory_get_nb_children(result) == 0) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    directory_data_destroy(result);
    remove_tree(path);
    free(path);

    return !err;
}

/* 2. Files only */

static bool test_scan_files(void)
{
    bool err = false;
    char* path = create_temp_directory();

    char file1[4096];
    char file2[4096];

    snprintf(file1, sizeof(file1), "%s/file1.txt", path);
    snprintf(file2, sizeof(file2), "%s/file2.txt", path);

    create_file(file1);
    create_file(file2);

    directory_data_s* result = scan_directory(path, 0);

    fprintf(stderr, "   Test \"result != NULL\" ");
    if (result != NULL) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    fprintf(stderr, "   Test \"directory_get_nb_data(result) == 2\" ");
    if (result != NULL && directory_get_nb_data(result) == 2) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    fprintf(stderr, "   Test \"directory_get_nb_children(result) == 0\" ");
    if (result != NULL && directory_get_nb_children(result) == 0) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    data_s* data1 = result != NULL ? find_data(result, "file1.txt") : NULL;
    data_s* data2 = result != NULL ? find_data(result, "file2.txt") : NULL;

    fprintf(stderr, "   Test \"data_get_type(file1.txt) == REGFILE\" ");
    if (data1 != NULL && data_get_type(data1) == REGFILE) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    fprintf(stderr, "   Test \"data_get_type(file2.txt) == REGFILE\" ");
    if (data2 != NULL && data_get_type(data2) == REGFILE) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    directory_data_destroy(result);
    remove_tree(path);
    free(path);

    return !err;
}

/* 3. Nested directories */

static bool test_scan_subdirectories(void)
{
    bool err = false;
    char* path = create_temp_directory();

    char sub[4096];
    char file[4096];

    snprintf(sub, sizeof(sub), "%s/sub", path);
    snprintf(file, sizeof(file), "%s/sub/child.txt", path);

    create_directory(sub);
    create_file(file);

    directory_data_s* result = scan_directory(path, 1);

    fprintf(stderr, "   Test \"result != NULL\" ");
    if (result != NULL) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    fprintf(stderr, "   Test \"directory_get_nb_data(result) == 1\" ");
    if (result != NULL && directory_get_nb_data(result) == 1) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    fprintf(stderr, "   Test \"directory_get_nb_children(result) == 1\" ");
    if (result != NULL && directory_get_nb_children(result) == 1) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    data_s* sub_data = result != NULL ? find_data(result, "sub") : NULL;

    fprintf(stderr, "   Test \"data_get_type(sub) == REGDIRECTORY\" ");
    if (sub_data != NULL && data_get_type(sub_data) == REGDIRECTORY) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    directory_data_s* child = NULL;

    if (result != NULL && directory_get_nb_children(result) >= 1) {
        child = directory_get_children(result)[0];
    }

    fprintf(stderr, "   Test \"child != NULL\" ");
    if (child != NULL) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    fprintf(stderr, "   Test \"directory_get_nb_data(child) == 1\" ");
    if (child != NULL && directory_get_nb_data(child) == 1) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    data_s* child_data
        = child != NULL && directory_get_nb_data(child) >= 1 ? directory_get_data(child)[0] : NULL;

    fprintf(stderr, "   Test \"data_get_name(child_data) == child.txt\" ");
    if (child_data != NULL && strcmp(data_get_name(child_data), "child.txt") == 0) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    directory_data_destroy(result);
    remove_tree(path);
    free(path);

    return !err;
}

/* 4. Different recursion depths */

static bool test_scan_depth(void)
{
    bool err = false;
    char* path = create_temp_directory();

    char sub[4096];
    char deep[4096];
    char file1[4096];
    char file2[4096];

    snprintf(sub, sizeof(sub), "%s/sub", path);
    snprintf(deep, sizeof(deep), "%s/sub/deep", path);
    snprintf(file1, sizeof(file1), "%s/file.txt", path);
    snprintf(file2, sizeof(file2), "%s/sub/deep/deep.txt", path);

    create_directory(sub);
    create_directory(deep);
    create_file(file1);
    create_file(file2);

    directory_data_s* depth0 = scan_directory(path, 0);
    directory_data_s* depth1 = scan_directory(path, 1);
    directory_data_s* depth2 = scan_directory(path, 2);

    fprintf(stderr, "   Test \"depth0 != NULL\" ");
    if (depth0 != NULL) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    fprintf(stderr, "   Test \"depth1 != NULL\" ");
    if (depth1 != NULL) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    fprintf(stderr, "   Test \"depth2 != NULL\" ");
    if (depth2 != NULL) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    /* Depth 0: root only */
    fprintf(stderr, "   Test \"directory_get_nb_data(depth0) == 2\" ");
    if (depth0 != NULL && directory_get_nb_data(depth0) == 2) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    fprintf(stderr, "   Test \"directory_get_nb_children(depth0) == 0\" ");
    if (depth0 != NULL && directory_get_nb_children(depth0) == 0) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    /* Depth 1: root + direct children */
    fprintf(stderr, "   Test \"directory_get_nb_data(depth1) == 2\" ");
    if (depth1 != NULL && directory_get_nb_data(depth1) == 2) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    fprintf(stderr, "   Test \"directory_get_nb_children(depth1) == 1\" ");
    if (depth1 != NULL && directory_get_nb_children(depth1) == 1) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    if (depth1 != NULL && directory_get_nb_children(depth1) >= 1) {
        directory_data_s* sub1 = directory_get_children(depth1)[0];

        fprintf(stderr, "   Test \"directory_get_nb_data(sub1) == 1\" ");
        if (directory_get_nb_data(sub1) == 1) {
            fprintf(stderr, "SUCCESS\n");
        } else {
            fprintf(stderr, "FAILURE\n");
            err = true;
        }

        fprintf(stderr, "   Test \"directory_get_nb_children(sub1) == 0\" ");
        if (directory_get_nb_children(sub1) == 0) {
            fprintf(stderr, "SUCCESS\n");
        } else {
            fprintf(stderr, "FAILURE\n");
            err = true;
        }
    }

    /* Depth 2: root + two levels */
    fprintf(stderr, "   Test \"directory_get_nb_data(depth2) == 2\" ");
    if (depth2 != NULL && directory_get_nb_data(depth2) == 2) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    fprintf(stderr, "   Test \"directory_get_nb_children(depth2) == 1\" ");
    if (depth2 != NULL && directory_get_nb_children(depth2) == 1) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    if (depth2 != NULL && directory_get_nb_children(depth2) >= 1) {
        directory_data_s* sub2 = directory_get_children(depth2)[0];

        fprintf(stderr, "   Test \"directory_get_nb_data(sub2) == 2\" ");
        if (directory_get_nb_data(sub2) == 1) {
            fprintf(stderr, "SUCCESS\n");
        } else {
            fprintf(stderr, "FAILURE\n");
            err = true;
        }

        fprintf(stderr, "   Test \"directory_get_nb_children(sub2) == 1\" ");
        if (directory_get_nb_children(sub2) == 1) {
            fprintf(stderr, "SUCCESS\n");
        } else {
            fprintf(stderr, "FAILURE\n");
            err = true;
        }

        if (directory_get_nb_children(sub2) >= 1) {
            directory_data_s* deep2 = directory_get_children(sub2)[0];

            fprintf(stderr, "   Test \"directory_get_nb_data(deep2) == 1\" ");
            if (directory_get_nb_data(deep2) == 1) {
                fprintf(stderr, "SUCCESS\n");
            } else {
                fprintf(stderr, "FAILURE\n");
                err = true;
            }

            fprintf(stderr, "   Test \"directory_get_nb_children(deep2) == 0\" ");
            if (directory_get_nb_children(deep2) == 0) {
                fprintf(stderr, "SUCCESS\n");
            } else {
                fprintf(stderr, "FAILURE\n");
                err = true;
            }
        } else {
            fprintf(stderr, "   Test \"directory_get_children(sub2)[0] exists\" FAILURE\n");
            err = true;
        }
    } else {
        fprintf(stderr, "   Test \"directory_get_children(depth2)[0] exists\" FAILURE\n");
        err = true;
    }

    directory_data_destroy(depth0);
    directory_data_destroy(depth1);
    directory_data_destroy(depth2);

    remove_tree(path);
    free(path);

    return !err;
}

static bool test_scan_invalid_path(void)
{
    directory_data_s* result = scan_directory("/tmp/tofkel-this-directory-does-not-exist", 0);

    fprintf(stderr, "   Test \"result == NULL\" ");
    if (result == NULL) {
        fprintf(stderr, "SUCCESS\n");
        return true;
    }
    fprintf(stderr, "FAILURE\n");
    return false;
}

static bool test_scan_trailing_slash(void)
{
    bool err = false;
    char* path = create_temp_directory();

    char file[4096];
    snprintf(file, sizeof(file), "%s/file.txt", path);
    create_file(file);

    char path_with_slash[4096];
    snprintf(path_with_slash, sizeof(path_with_slash), "%s///", path);

    directory_data_s* result = scan_directory(path_with_slash, 0);

    fprintf(stderr, "   Test \"result != NULL\" ");
    if (result != NULL) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    fprintf(stderr, "   Test \"directory_get_nb_data(result) == 1\" ");
    if (result != NULL && directory_get_nb_data(result) == 1) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    fprintf(stderr, "   Test \"find_data(result, \"file.txt\")\" ");
    if (result != NULL && find_data(result, "file.txt")) {
        fprintf(stderr, "SUCCESS\n");
    } else {
        fprintf(stderr, "FAILURE\n");
        err = true;
    }

    directory_data_destroy(result);
    remove_tree(path);
    free(path);
    return !err;
}

/* ------------------------------ MAIN ------------------------------ */

static const test TESTS[] = { { "scan_empty_directory", test_scan_empty_directory },
    { "scan_files", test_scan_files }, { "scan_subdirectories", test_scan_subdirectories },
    { "scan_depth", test_scan_depth }, { "scan_invalid_path", test_scan_invalid_path },
    { "scan_trailing_slash", test_scan_trailing_slash } };

void usage(char* argv[])
{
    fprintf(stderr, "Usage: %s <testname>\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    if (argc == 1) {
        usage(argv);
    }
    bool error = false;
    for (int j = 1; j < argc; j++) {
        bool found = false;
        for (uint i = 0; i < NB_TESTS && !found; i++) {
            if (strcmp(TESTS[i].name, argv[j]) == 0) {
                found = true;
                fprintf(stderr, "Test \"%s\" START\n", argv[j]);
                if (TESTS[i].function()) {
                    fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[j]);
                } else {
                    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[j]);
                    error = true;
                }
            }
        }
        if (!found) {
            fprintf(stderr, "Test \"%s\" unrecognized: FAILURE\n", argv[j]);
            error = true;
        }
    }
    if (error)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

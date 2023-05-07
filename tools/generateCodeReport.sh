 
ADDONS_WITH_NESTED_DIR=("binding" "binding_noexcept" "binding_noexcept_maybe"  "binding_custom_namespace")

generate_coverage_report () {
    addon_name=$1

    TEST_BUILD_DIR="test/build/Release/obj.target/$addon_name"
    NESTED_DIR=("basic_types" "globalObject" "dataview" "maybe" "object" "threadsafe_function" "typed_threadsafe_function")

    for str in ${NESTED_DIR[@]}; do
        mv $TEST_BUILD_DIR/$str/*.gcno $TEST_BUILD_DIR
        mv $TEST_BUILD_DIR/$str/*.gcda $TEST_BUILD_DIR
    done

}

for addon in ${ADDONS_WITH_NESTED_DIR[@]}; do
    generate_coverage_report $addon
done

gcovr --gcov-filter '.*\b(napi-inl.h)\b' > coverage/test-coverage-report.txt
 
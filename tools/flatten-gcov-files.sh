# TODO: Add for all binary combinations
TEST_BUILD_DIR="../test/build/Release/obj.target/binding"
NESTED_DIR=("basic_types" "globalObject" "dataview" "maybe" "object" "threadsafe_function" "typed_threadsafe_function")

for str in ${NESTED_DIR[@]}; do
    mv $TEST_BUILD_DIR/$str/*.gcno $TEST_BUILD_DIR
    mv $TEST_BUILD_DIR/$str/*.gcda $TEST_BUILD_DIR
done

mkdir -p "coverage"
cd "../" && gcovr --gcov-filter '.*\b(napi-inl.h)\b' > coverage/binding-coverage-report.txt
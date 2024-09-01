This project is only for interviewer. Please do not copy.

How to use:
cmake -B build
cd build
make

For server:
./server
ctrl+C to quit

For client:
./client
type "quit" to quit

Use find . -iname "*.h" -o -iname "*.cpp" | xargs clang-format -i to lint code.
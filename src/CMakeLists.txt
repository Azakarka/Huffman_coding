add_executable(
        archiver
        archiver.cpp
        coders/trie.cpp
        bit_stream/bit_stream.cpp
        bit_stream/ibit_stream.cpp
        bit_stream/obit_stream.cpp
        coders/decoder.cpp
        coders/encoder.cpp
        term_parser/terminal_parser.cpp
        bit_stream/stream_buffer.cpp heap/heap.h heap/heap.cpp heap/heap.cpp term_parser/parser_template.h)
add_subdirectory(bit_stream)
add_subdirectory(coders)
add_subdirectory(term_parser)
add_subdirectory(heap)
add_subdirectory(tests)
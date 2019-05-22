add_contract(eosio.stok eosio.stok ${CMAKE_CURRENT_SOURCE_DIR}/src/eosio.stok.cpp)

target_include_directories(eosio.stok
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(eosio.stok
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

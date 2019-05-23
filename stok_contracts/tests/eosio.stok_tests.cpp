#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>

#include "Runtime/Runtime.h"

#include <fc/variant_object.hpp>

#include "contracts.hpp"

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;
using namespace std;

using mvo = fc::mutable_variant_object;

class eosio_token_tester : public tester {
public:

   eosio_token_tester() {
      produce_blocks( 2 );

      create_accounts( { N(alice), N(bob), N(carol), N(eosio.stok) } );
      produce_blocks( 2 );

      set_code( N(eosio.stok), contracts::token_wasm() );
      set_abi( N(eosio.stok), contracts::token_abi().data() );

      produce_blocks();

      const auto& accnt = control->db().get<account_object,by_name>( N(eosio.stok) );
      abi_def abi;
      BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
      abi_ser.set_abi(abi, abi_serializer_max_time);
   }

   action_result push_action( const account_name& signer, const action_name &name, const variant_object &data ) {
      string action_type_name = abi_ser.get_action_type(name);

      action act;
      act.account = N(eosio.stok);
      act.name    = name;
      act.data    = abi_ser.variant_to_binary( action_type_name, data,abi_serializer_max_time );

      return base_tester::push_action( std::move(act), uint64_t(signer));
   }

   fc::variant get_stats( const account_name& issuer )
   {
      vector<char> data = get_row_by_account( N(eosio.stok), issuer.value, N(stat), issuer.value );
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant( "currency_stats", data, abi_serializer_max_time );
   }

   fc::variant get_creditor( account_name issuer, int64_t creditor)
   {
      vector<char> data = get_row_by_account( N(eosio.stok), issuer.value, N(accounts), creditor );
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant( "account", data, abi_serializer_max_time );
   }

   action_result create( account_name issuer,
                asset        maximum_supply_st,
                asset        maximum_supply_ut ) {

      return push_action( N(eosio.stok), N(create), mvo()
           ( "issuer", issuer)
           ( "maximum_supply_st", maximum_supply_st)
           ( "maximum_supply_ut", maximum_supply_ut)
      );
   }

   action_result issue( account_name issuer, asset quant_st, asset quant_ut, string memo ) {
      return push_action( issuer, N(issue), mvo()
           ( "issuer", issuer)
           ( "quant_st", quant_st)
           ( "quant_ut", quant_ut)
           ( "memo", memo)
      );
   }

   action_result transfer( account_name issuer,
                  int64_t creditor_id,
                  asset   quant_st,
                  asset   quant_ut,
                  string  memo ) {
      return push_action( issuer, N(transfer), mvo()
           ( "issuer", issuer)
           ( "creditor_id", creditor_id)
           ( "quant_st", quant_st)
           ( "quant_ut", quant_ut)
           ( "memo", memo)
      );
   }

   action_result retire( account_name issuer, int64_t creditor_id, asset quant_st, asset quant_ut, string memo ) {
      return push_action( issuer, N(retire), mvo()
           ( "issuer", issuer)
           ( "creditor_id", creditor_id)
           ( "quant_st", quant_st)
           ( "quant_ut", quant_ut)
           ( "memo", memo)
      );
   }

   action_result clear( account_name issuer,
                     int64_t creditor_id,
                     asset  quant_st,
                     asset  quant_ut,
                     string repayment,
                     string bond_yield,
                     string expr_yield,
                     string memo) {
      return push_action( issuer, N(clear), mvo()
           ( "issuer", issuer )
           ( "creditor_id", creditor_id )
           ( "quant_st", quant_st )
           ( "quant_ut", quant_ut )
           ( "repayment", repayment )
           ( "bond_yield", bond_yield)
           ( "expr_yield", expr_yield)
           ( "memo", memo)
      );
   }

   abi_serializer abi_ser;
};

BOOST_AUTO_TEST_SUITE(eosio_token_tests)

BOOST_FIXTURE_TEST_CASE( create_tests, eosio_token_tester ) try {

   auto token = create( N(alice), asset::from_string("1000.000 ST"), asset::from_string("1000.000 UT"));
   auto stats = get_stats(N(alice));
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply_st", "0.000 ST")
      ("supply_ut", "0.000 UT")
      ("max_supply_st", "1000.000 ST")
      ("max_supply_ut", "1000.000 UT")
      ("issuer", "alice")
   );
   produce_blocks(1);

} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( create_negative_max_supply_st, eosio_token_tester ) try {

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "max-supply-st must be positive" ),
      create( N(alice), asset::from_string("-1000.000 ST"), asset::from_string("1000.000 UT"))
   );

} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( create_negative_max_supply_ut, eosio_token_tester ) try {

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "max-supply-ut must be positive" ),
      create( N(alice), asset::from_string("1000.000 ST"), asset::from_string("-1000.000 UT"))
   );

} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( symbol_already_exists, eosio_token_tester ) try {

   auto token = create( N(alice), asset::from_string("1000.000 ST"), asset::from_string("1000.000 UT"));
   auto stats = get_stats(N(alice));
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply_st", "0.000 ST")
      ("supply_ut", "0.000 UT")
      ("max_supply_st", "1000.000 ST")
      ("max_supply_ut", "1000.000 UT")
      ("issuer", "alice")
   );
   produce_blocks(1);

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "token with issuer already exists" ),
                        create( N(alice), asset::from_string("1000.000 ST"), asset::from_string("1000.000 UT"))
   );

} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( issue_tests, eosio_token_tester ) try {

   auto token = create( N(alice), asset::from_string("1000.000 ST"), asset::from_string("1000.000 UT"));
   produce_blocks(1);

   issue( N(alice), asset::from_string("500.000 ST"), asset::from_string("500.000 UT"), "hola" );

   auto stats = get_stats(N(alice));
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply_st", "500.000 ST")
      ("supply_ut", "500.000 UT")
      ("max_supply_st", "1000.000 ST")
      ("max_supply_ut", "1000.000 UT")
      ("issuer", "alice")
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "ST quantity exceeds available supply" ),
      issue( N(alice), asset::from_string("500.001 ST"), asset::from_string("500.000 UT"), "hola" )
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "UT quantity exceeds available supply" ),
      issue( N(alice), asset::from_string("500.000 ST"), asset::from_string("500.001 UT"), "hola" )
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "must issue positive ST quantity" ),
      issue( N(alice), asset::from_string("-1.000 ST"), asset::from_string("1.000 UT"), "hola" )
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "must issue positive UT quantity" ),
      issue( N(alice), asset::from_string("1.000 ST"), asset::from_string("-1.000 UT"), "hola" )
   );

   BOOST_REQUIRE_EQUAL( success(),
      issue( N(alice), asset::from_string("1.000 ST"), asset::from_string("1.000 UT"), "hola" )
   );

} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( transfer_tests, eosio_token_tester ) try {

   auto token = create( N(alice), asset::from_string("1000.000 ST"), asset::from_string("1000.000 UT"));;
   produce_blocks(1);

   issue( N(alice), asset::from_string("1000.000 ST"), asset::from_string("1000.000 UT"), "hola" );
   produce_blocks(1);

   auto stats = get_stats(N(alice));
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply_st", "1000.000 ST")
      ("supply_ut", "1000.000 UT")
      ("max_supply_st", "1000.000 ST")
      ("max_supply_ut", "1000.000 UT")
      ("issuer", "alice")
   );

   auto creditor_id = 1;
   transfer( N(alice), creditor_id, asset::from_string("300.000 ST"), asset::from_string("300.000 UT"), "hola" );
   produce_blocks(1);

   auto balance_1 = get_creditor(N(alice), 1);
   BOOST_TEST_MESSAGE(fc::json::to_pretty_string(balance_1));
   REQUIRE_MATCHING_OBJECT( balance_1, mvo()
      ("creditor_id", 1)
      ("balance_st", "300.000 ST")
      ("balance_ut", "300.000 UT")
      ("repayment", "")
      ("bond_yield", "")
      ("expr_yield", "")
   );

} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( retire_tests, eosio_token_tester ) try {

   auto token = create( N(alice), asset::from_string("1000 ST"), asset::from_string("1000 UT"));;
   produce_blocks(1);

   issue( N(alice), asset::from_string("1000 ST"), asset::from_string("1000 UT"), "hola" );
   produce_blocks(1);

   auto stats = get_stats(N(alice));
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply_st", "1000 ST")
      ("supply_ut", "1000 UT")
      ("max_supply_st", "1000 ST")
      ("max_supply_ut", "1000 UT")
      ("issuer", "alice")
   );

   auto creditor_id = 1;
   transfer( N(alice), creditor_id, asset::from_string("300 ST"), asset::from_string("300 UT"), "transfer" );
   produce_blocks(1);

   BOOST_TEST_MESSAGE(retire( N(alice), creditor_id, asset::from_string("100 ST"), asset::from_string("200 UT"), "retire" ));
   produce_blocks(1);

   auto balance_1 = get_creditor(N(alice), creditor_id);
   BOOST_TEST_MESSAGE(fc::json::to_pretty_string(balance_1));
   REQUIRE_MATCHING_OBJECT( balance_1, mvo()
      ("creditor_id", 1)
      ("balance_st", "200 ST")
      ("balance_ut", "100 UT")
      ("repayment", "")
      ("bond_yield", "")
      ("expr_yield", "")
   );

} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( clear_tests, eosio_token_tester ) try {

   auto token = create( N(alice), asset::from_string("100 ST"), asset::from_string("100 UT"));;
   produce_blocks(1);

   issue( N(alice), asset::from_string("60 ST"), asset::from_string("60 UT"), "hola" );

   auto stats = get_stats(N(alice));
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply_st", "60 ST")
      ("supply_ut", "60 UT")
      ("max_supply_st", "100 ST")
      ("max_supply_ut", "100 UT")
      ("issuer", "alice")
   );

   auto creditor_id = 1;
   transfer( N(alice), creditor_id, asset::from_string("2 ST"), asset::from_string("2 UT"), "채권 정보입니다." );
   produce_blocks(1);

   auto balance_1 = get_creditor(N(alice), 1);
   REQUIRE_MATCHING_OBJECT( balance_1, mvo()
      ("creditor_id", 1)
      ("balance_st", "2 ST")
      ("balance_ut", "2 UT")
      ("repayment", "")
      ("bond_yield", "")
      ("expr_yield", "")
   );

   retire( N(alice), creditor_id, asset::from_string("0 ST"), asset::from_string("2 UT"), "채권 정보입니다." );
   produce_blocks(1);

   clear( N(alice), 1, asset::from_string("2 ST"), asset::from_string("0 UT"), "2.0", "3.1", "4.1", "청산된 정보입니다.");
   produce_blocks(1);

   auto balance_11 = get_creditor(N(alice), 1);
   //BOOST_TEST_MESSAGE(fc::json::to_pretty_string(balance_11));

   REQUIRE_MATCHING_OBJECT( balance_11, mvo()
      ("creditor_id", 1)
      ("balance_st", "0 ST")
      ("balance_ut", "0 UT")
      ("repayment", "2.0")
      ("bond_yield", "3.1")
      ("expr_yield", "4.1")
   );

} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()

/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include <eosio.stok/eosio.stok.hpp>

namespace eosio {

void token::create( name   issuer,
                    asset  maximum_supply_st,
                    asset  maximum_supply_ut)
{
    require_auth( _self );

    auto sym_st = maximum_supply_st.symbol;
    check( sym_st.is_valid(), "invalid ST symbol name" );
    check( maximum_supply_st.is_valid(), "invalid ST supply");
    check( maximum_supply_st.amount > 0, "max-supply-st must be positive");

    auto sym_ut = maximum_supply_ut.symbol;
    check( sym_ut.is_valid(), "invalid UT symbol name" );
    check( maximum_supply_ut.is_valid(), "invalid UT supply");
    check( maximum_supply_ut.amount > 0, "max-supply-ut must be positive");

    stats statstable( _self, issuer.value );
    auto existing = statstable.find( issuer.value );
    check( existing == statstable.end(), "token with issuer already exists" );

    statstable.emplace( _self, [&]( auto& s ) {
       s.supply_st.symbol = maximum_supply_st.symbol;
       s.supply_ut.symbol = maximum_supply_ut.symbol;
       s.max_supply_st    = maximum_supply_st;
       s.max_supply_ut    = maximum_supply_ut;
       s.issuer        = issuer;
    });
}

void token::issue( name issuer, asset quant_st, asset quant_ut, string memo )
{
    auto sym_st = quant_st.symbol;
    auto sym_ut = quant_ut.symbol;
    check( sym_st.is_valid(), "invalid UT symbol name" );
    check( sym_ut.is_valid(), "invalid ST symbol name" );

    check( memo.size() <= 256, "memo has more than 256 bytes" );

    stats statstable( _self, issuer.value );
    auto existing = statstable.find( issuer.value );
    check( existing != statstable.end(), "token with issuer does not exist, create token before issue" );
    const auto& st = *existing;

    check(issuer == st.issuer, "dismatch issuer");

    require_auth( st.issuer );

    check( quant_st.is_valid(), "invalid ST quantity" );
    check( quant_st.amount > 0, "must issue positive ST quantity" );

    check( quant_ut.is_valid(), "invalid UT quantity" );
    check( quant_ut.amount > 0, "must issue positive UT quantity" );

    check( quant_st.symbol == st.supply_st.symbol, "ST symbol precision mismatch" );
    check( quant_st.amount <= st.max_supply_st.amount - st.supply_st.amount, "ST quantity exceeds available supply");

    check( quant_ut.symbol == st.supply_ut.symbol, "UT symbol precision mismatch" );
    check( quant_ut.amount <= st.max_supply_ut.amount - st.supply_ut.amount, "UT quantity exceeds available supply");

    statstable.modify( st, same_payer, [&]( auto& s ) {
       s.supply_st += quant_st;
       s.supply_ut += quant_ut;
    });
}

void token::retire( name issuer, asset quant_st, asset quant_ut, string memo )
{
    auto sym_st = quant_st.symbol;
    auto sym_ut = quant_ut.symbol;
    check( sym_st.is_valid(), "invalid UT symbol name" );
    check( sym_ut.is_valid(), "invalid ST symbol name" );

    check( memo.size() <= 256, "memo has more than 256 bytes" );

    stats statstable( _self, issuer.value );
    auto existing = statstable.find( issuer.value );
    check( existing != statstable.end(), "token with issuer does not exist, create token before issue" );
    const auto& st = *existing;

    check(issuer == st.issuer, "dismatch issuer");

    require_auth( st.issuer );

    check( quant_st.is_valid(), "invalid ST quantity" );
    check( quant_st.amount > 0, "must issue positive ST quantity" );

    check( quant_ut.is_valid(), "invalid UT quantity" );
    check( quant_ut.amount > 0, "must issue positive UT quantity" );

    check( quant_st.symbol == st.supply_st.symbol, "ST symbol precision mismatch" );
    check( quant_st.amount <= st.max_supply_st.amount - st.supply_st.amount, "ST quantity exceeds available supply");

    check( quant_ut.symbol == st.supply_ut.symbol, "UT symbol precision mismatch" );
    check( quant_ut.amount <= st.max_supply_ut.amount - st.supply_ut.amount, "UT quantity exceeds available supply");

    statstable.modify( st, same_payer, [&]( auto& s ) {
       s.supply_st -= quant_st;
       s.supply_ut -= quant_ut;
    });
}

void token::transfer( name    issuer,
                      int64_t creditor_id,
                      asset   quant_st,
                      asset   quant_ut,
                      string  memo )
{

    require_auth( issuer );

    stats statstable( _self, issuer.value );
    const auto& st = statstable.get( issuer.value );

    auto sym_st = quant_st.symbol;
    auto sym_ut = quant_ut.symbol;
    check( sym_st.is_valid(), "invalid UT symbol name" );
    check( sym_ut.is_valid(), "invalid ST symbol name" );

    check( memo.size() <= 256, "memo has more than 256 bytes" );

    check( quant_st.is_valid(), "invalid ST quantity" );
    check( quant_st.amount > 0, "must issue positive ST quantity" );

    check( quant_ut.is_valid(), "invalid UT quantity" );
    check( quant_ut.amount > 0, "must issue positive UT quantity" );

    check( quant_st.symbol == st.supply_st.symbol, "ST symbol precision mismatch" );
    check( quant_ut.symbol == st.supply_ut.symbol, "UT symbol precision mismatch" );

    add_balance( issuer, creditor_id, quant_st, quant_ut );
}

void token::add_balance( name issuer, int64_t creditor_id, asset quant_st, asset quant_ut)
{
   accounts to_acnts( _self, issuer.value );
   auto to = to_acnts.find( creditor_id );
   if( to == to_acnts.end() ) {
      to_acnts.emplace( issuer, [&]( auto& a ){
        a.creditor_id = creditor_id;
        a.balance_st = quant_st;
        a.balance_ut = quant_ut;
      });
   } else {
      to_acnts.modify( to, same_payer, [&]( auto& a ) {
        a.balance_st += quant_st;
        a.balance_ut += quant_ut;
      });
   }
}

void token::clear( name issuer, int64_t creditor_id, asset quant_st, asset quant_ut, string dividend, string bond_yield, string expr_yield, string memo )
{
   require_auth( issuer );
   accounts acnts( _self, issuer.value );
   auto it = acnts.find( creditor_id );
   check( it != acnts.end(), "Balance row already cleared or ever existed. Action won't have any effect." );

   check( dividend.size() <= 20, "dividend has more than 20 bytes" );
   check( bond_yield.size() <= 10, "bond_yield has more than 10 bytes" );
   check( expr_yield.size() <= 10, "expr_yield has more than 10 bytes" );
   check( memo.size() <= 256, "memo has more than 256 bytes" );

   acnts.modify( it, same_payer, [&]( auto& a ) {
      a.balance_st = quant_st;
      a.balance_ut = quant_ut;
      a.dividend = dividend;
      a.bond_yield = bond_yield;
      a.expr_yield = expr_yield;
   });
}

} /// namespace eosio

EOSIO_DISPATCH( eosio::token, (create)(issue)(transfer)(retire)(clear) )
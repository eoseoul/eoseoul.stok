/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>

#include <string>

namespace eosiosystem {
   class system_contract;
}

namespace eosio {

   using std::string;

   class [[eosio::contract("eosio.stok")]] token : public contract {
      public:
         using contract::contract;

         [[eosio::action]]
         void create( name   issuer,
                      asset  maximum_supply_st,
                      asset  maximum_supply_ut);

         [[eosio::action]]
         void issue( name issuer, asset quant_st, asset quant_ut, string memo );

         [[eosio::action]]
         void retire( name issuer, asset quant_st, asset quant_ut, string memo );

         [[eosio::action]]
         void transfer( name    issuer,
                        int64_t creditor_id,
                        asset   quant_st,
                        asset   quant_ut,
                        string  memo );

         [[eosio::action]]
         void clear( name    issuer,
                     int64_t creditor_id,
                     asset   quant_st,
                     asset   quant_ut,
                     string  dividend,
                     string  bond_yield,
                     string  expr_yield,
                     string  memo);

      private:
         struct [[eosio::table]] account {
            int64_t creditor_id;
            asset   balance_st;
            asset   balance_ut;
            string  dividend;
            string  bond_yield;
            string  expr_yield;

            uint64_t primary_key()const { return creditor_id; }
         };

         struct [[eosio::table]] currency_stats {
            asset    supply_st;
            asset    supply_ut;
            asset    max_supply_st;
            asset    max_supply_ut;
            name     issuer;

            uint64_t primary_key()const { return issuer.value; }
         };

         typedef eosio::multi_index< "accounts"_n, account > accounts;
         typedef eosio::multi_index< "stat"_n, currency_stats > stats;

         // void sub_balance( name owner, asset value );
         void add_balance( name issuer, int64_t creditor, asset quant_st, asset quant_ut );
   };

} /// namespace eosio

'use strict';

const NODE_ENV = process.env.NODE_ENV;

const _ = require('lodash');
const accounts = require(`./${NODE_ENV}/accounts.json`);

const ram = 20;
const stake_cpu = 100;
const stake_net = 100;
const users = accounts.users;

const newEosAccounts = _.map(users, (user) => {
    return {creator : 'eosio', name : user.name, owner : user.pub, active : user.pub};
});

const newAccountBuyRam = {payer : 'eosio', receiver : '', quant : `${ram}.0000 EOS`};
const newAccountDelegate = {from : 'eosio', receiver : '', stake_net_quantity : `${stake_net}.0000 EOS`, stake_cpu_quantity : `${stake_cpu}.0000 EOS`, transfer : true};

const createStok = {
    maximum_supply_st : '100 ST',
    maximum_supply_ut : '100 UT'
};

const issueStok = {
    quant_st : '64 ST',
    quant_ut : '64 UT',
    memo : '채권을 발행'
};

const creditorTransfer = _.map(_.times(64), (i) => {
    return {
        creditor_id : 2000 + i,
        quant_st : '1 ST',
        quant_ut : '1 UT',
        memo : '구입한 채권 정보'
    };
});

const creditorRetire = _.map(_.times(64), (i) => {
    return {
        creditor_id : 2000 + i,
        quant_st : '0 ST',
        quant_ut : '1 UT',
        memo : 'UT retire 정보'
    };
});

const creditorClear = _.map(_.times(64), (i) => {
    return {
        creditor_id : 2000 + i,
        quant_st : '1 ST',
        quant_ut : '0 UT',
        dividend : '20' + i,
        bond_yield : `${i}.0`,
        expr_yield : `${i + 2}.00`,
        memo : '청산된 채권 정보'
    };
});

module.exports = exports = {
    users,
    newEosAccounts,
    newAccountBuyRam,
    newAccountDelegate,
    createStok,
    issueStok,
    creditorTransfer,
    creditorRetire,
    creditorClear
};

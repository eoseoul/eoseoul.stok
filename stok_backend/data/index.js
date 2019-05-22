'use strict';

const _ = require('lodash'),
    accounts = require('./accounts.json');

const ram = 20;
const stake_cpu = 20;
const stake_net = 20;
const users = accounts.users;

const newEosAccounts = _.map(users, (user) => {
    return {creator : 'eosio', name : user.name, owner : user.pub, active : user.pub};
});

const newAccountBuyRam = {payer : 'eosio', receiver : '', quant : `${ram}.0000 EOS`};
const newAccountDelegate = {from : 'eosio', receiver : '', stake_net_quantity : `${stake_net}.0000 EOS`, stake_cpu_quantity : `${stake_cpu}.0000 EOS`, transfer : true};

const createStok = {
    maximum_supply_st : '10000.0000 ST',
    maximum_supply_ut : '20000.0000 UT'
};

const issueStok = {
    quant_st : '10000.0000 ST',
    quant_ut : '20000.0000 UT',
    memo : '채권을 발행'
};

const creditorTransfer = _.map(_.times(70), (i) => {
    return {
        creditor_id : 2000 + i,
        quant_st : '1000.0000 ST',
        quant_ut : '2000.0000 UT',
        memo : '구입한 채권 정보'
    };
});

const creditorClear = _.map(_.times(70), (i) => {
    return {
        creditor_id : 2000 + i,
        quant_st : '1000.0000 ST',
        quant_ut : '2000.0000 UT',
        dividend : 20 + i,
        bond_yield : 2.0 + i,
        expr_yield : 10.1 + i,
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
    creditorClear
};

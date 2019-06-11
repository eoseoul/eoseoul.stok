'use strict';

const _ = require('lodash');
const files = require('files');
const accounts = files.accounts;

const ram = 20;
const stake_cpu = 100;
const stake_net = 100;

const newEosAccounts = _.map(accounts, (account) => {
    return {creator : 'eosio', name : account.name, owner : account.pub, active : account.pub};
});

const newAccountBuyRam = {payer : 'eosio', receiver : '', quant : `${ram}.0000 EOS`};
const newAccountDelegate = {from : 'eosio', receiver : '', stake_net_quantity : `${stake_net}.0000 EOS`, stake_cpu_quantity : `${stake_cpu}.0000 EOS`, transfer : true};

module.exports = exports = {
    accounts,
    newEosAccounts,
    newAccountBuyRam,
    newAccountDelegate
};

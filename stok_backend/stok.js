'use strict';

let Promise = require('bluebird'),
    _ = require('lodash'),
    util = require('util'),
    config = require('config'),
    eosApi = require('external_apis/eos_api'),
    data = require('./data');

const contract = config.eosNode.contract;
const issuer = config.eosNode.issuer;

class EosStokApi {
    constructor() {
        this.contractName = contract;
    }
    create(params, keyProvider, actor) {
        const options = {keyProvider};
        const authorization = eosApi.createAuthorization(actor, 'active');
        const action = eosApi.createAction(this.contractName, 'create', params, authorization);
        console.log(action);
        return eosApi.transact({actions : [action]}, options);
    }

    issue(params, keyProvider, actor) {
        const options = {keyProvider};
        const authorization = eosApi.createAuthorization(actor, 'active');
        const action = eosApi.createAction(this.contractName, 'issue', params, authorization);
        return eosApi.transact({actions : [action]}, options);
    }

    transfer(params, keyProvider, actor) {
        const options = {keyProvider};
        const authorization = eosApi.createAuthorization(actor, 'active');
        const action = eosApi.createAction(this.contractName, 'transfer', params, authorization);
        return eosApi.transact({actions : [action]}, options);
    }

    retire(params, keyProvider, actor) {
        const options = {keyProvider};
        const authorization = eosApi.createAuthorization(actor, 'active');
        const action = eosApi.createAction(this.contractName, 'retire', params, authorization);
        return eosApi.transact({actions : [action]}, options);
    }

    clear(params, keyProvider, actor) {
        const options = {keyProvider};
        const authorization = eosApi.createAuthorization(actor, 'active');
        const action = eosApi.createAction(this.contractName, 'clear', params, authorization);
        return eosApi.transact({actions : [action]}, options);
    }
}

const eosStok = new EosStokApi();

async function flowStok() {
    const dummy = Object.assign({}, {}, data);

    const user = _.find(dummy.users, {name : issuer});
    const code = _.find(dummy.users, {name : contract});

    const transfers = [];
    const retires = [];
    const clears = [];

    await function() {
        console.log('create');
        const create = _.assign({}, dummy.createStok, {issuer : user.name});
        console.log(create);
        return Promise.resolve(eosStok.create(create, code.pvt, code.name))
            .delay(10);
    }();

    await function() {
        console.log('issue');
        const issue = _.assign({}, dummy.issueStok, {issuer : user.name});
        return Promise.resolve(eosStok.issue(issue, user.pvt, user.name))
            .delay(10);
    }();

    await function() {
        console.log('transfer');
        return Promise.each(dummy.creditorTransfer, (creditor) => {
            console.log(creditor);
            creditor.issuer = user.name;
            return Promise.resolve(eosStok.transfer(creditor, user.pvt, user.name))
                .delay(10)
                .then((trx) => {
                    transfers.push({creditor_id : creditor.creditor_id, trx_id : trx.transaction_id});
                });
        });
    }();

    await function() {
        console.log('retire');
        return Promise.each(dummy.creditorRetire, (creditor) => {
            console.log(creditor);
            creditor.issuer = user.name;
            return Promise.resolve(eosStok.transfer(creditor, user.pvt, user.name))
                .delay(10)
                .then((trx) => {
                    retires.push({creditor_id : creditor.creditor_id, trx_id : trx.transaction_id});
                });
        });
    }();

    await function() {
        console.log('clear');
        return Promise.each(dummy.creditorClear, (creditor) => {
            console.log(creditor);
            creditor.issuer = user.name;
            return Promise.resolve(eosStok.clear(creditor, user.pvt, user.name))
                .delay(10)
                .then((trx) => {
                    clears.push({creditor_id : creditor.creditor_id, trx_id : trx.transaction_id});
                });
        });
    }();

    console.log(util.inspect(transfers, {depth : 3}));
    console.log(util.inspect(retires, {depth : 3}));
    console.log(util.inspect(clears, {depth : 3}));
}

module.exports = exports = { flowStok };

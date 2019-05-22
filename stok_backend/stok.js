'use strict';

let Promise = require('bluebird'),
    _ = require('lodash'),
    eosApi = require('external_apis/eos_api'),
    data = require('./data');

class EosStokApi {
    constructor() {
        this.contractName = 'eosiodotstok';
    }
    create(params, keyProvider, actor) {
        const options = {keyProvider};
        const authorization = eosApi.createAuthorization(actor, 'active');
        const action = eosApi.createAction(this.contractName, 'create', params, authorization);
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
    const user = _.find(dummy.users, {name : 'issuerstok11'});
    const code = _.find(dummy.users, {name : 'eosiodotstok'});
    const actor = user.name;

    await function() {
        const create = _.assign({}, dummy.createStok, {issuer : actor});
        console.log(`create ${create}`);
        console.log(create);
        return Promise.resolve(eosStok.create(create, code.pvt, code.name))
            .delay(10);
    }();

    await function() {
        const issue = _.assign({}, dummy.issueStok, {issuer : actor});
        console.log(`issue ${issue}.`);
        return Promise.resolve(eosStok.issue(issue, user.pvt, actor))
            .delay(10);
    }();

    await function() {
        console.log('transfer');
        return Promise.each(dummy.creditorTransfer, (creditor) => {
            console.log(`transfer ${creditor}.`);
            creditor.issuer = user.name;
            return Promise.resolve(eosStok.transfer(creditor, user.pvt, actor))
                .delay(10);
        });
    }();

    await function() {
        console.log('clear');
        return Promise.each(dummy.creditorClear, (creditor) => {
            console.log(`clear ${creditor}.`);
            creditor.issuer = user.name;
            return Promise.resolve(eosStok.clear(creditor, user.pvt, actor))
                .delay(10);
        });
    }();
}

module.exports = exports = { flowStok };

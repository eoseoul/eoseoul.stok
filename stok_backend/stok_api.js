'use strict';

const eosApi = require('external_apis/eos_api');

class EosStokApi {
    constructor(contract) {
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

module.exports = exports = EosStokApi;

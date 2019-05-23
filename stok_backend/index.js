'use strict';

let Promise = require('bluebird'),
    _ = require('lodash'),
    path = require('path'),
    config = require('config'),
    eosApi = require('external_apis/eos_api'),
    data = require('./data'),
    stok = require('./stok');

bootNode();

async function bootNode() {
    const dummy = Object.assign({}, {}, data);

    if (process.env.NODE_ENV === 'development') {
        await function() {
            console.log('create newaccounts');
            return Promise.each(dummy.newEosAccounts, (newAccount) => {
                console.log(`newaccount ${newAccount.name}`);

                const authorization = eosApi.createAuthorization('eosio', 'active');

                const newAccountBuyRam = Object.assign({}, dummy.newAccountBuyRam, {receiver : newAccount.name});
                const newAccountDelegate = Object.assign({}, dummy.newAccountDelegate, {receiver : newAccount.name});

                const newaccountAction = eosApi.createAction('eosio', 'newaccount', newAccount, authorization);
                const buyramAction = eosApi.createAction('eosio', 'buyram', newAccountBuyRam, authorization);
                const delegatebwAction = eosApi.createAction('eosio', 'delegatebw', newAccountDelegate, authorization);

                return Promise.resolve(eosApi.newaccount(newaccountAction, buyramAction, delegatebwAction))
                    .catch((err) => {
                        console.log(err);
                        if (err.error.code === 3050001) { // already exist
                            console.log(err.error.what);
                            return;
                        }
                        throw err;
                    });
            });
        }();
    }

    await function() {
        console.log('deploy stok contract');

        const contract = config.eosNode.contract;
        const user = _.find(dummy.users, {name : contract});
        const contractPath = path.join(__dirname, 'contract', 'eosio.stok');
        const authorization = eosApi.createAuthorization(contract, 'active');
        const options = {keyProvider : user.pvt};

        return eosApi.deployContract(contract, contractPath, authorization, options)
        .catch((err) => {
            console.log(err);
            if (err.error && err.error.code === 3160008) { // set_exact_code: Contract is already running this version
                console.log(err.error.what);
                return;
            }
            throw err;
        });
    }();

    await stok.flowStok();
}

'use strict';

let Promise = require('bluebird'),
    _ = require('lodash'),
    util = require('util'),
    parseArgs = require('minimist'),
    path = require('path'),
    config = require('config'),
    eosApi = require('external_apis/eos_api'),
    data = require('./data'),
    files = require('./files'),
    StokApi = require('./stok_api');

const contract = config.eosNode.contract;
const issuer = config.eosNode.issuer;

const eosStok = new StokApi(contract);

const dummy = Object.assign({}, {}, data);
const user = _.find(dummy.accounts, {name : issuer});
const code = _.find(dummy.accounts, {name : contract});

Promise.resolve(build())
    .then(() => {
        console.log('build complete~~');
    })
    .catch((err) => {
        console.log(err);
    })
    .delay(500);

async function build() {
    const argv = parseArgs(process.argv.splice(2));
    const cmd = argv.c;

    if (cmd === 'newaccount') {
        return newaccount();
    } else if (cmd === 'deploy') {
        return deploy();
    } else if (cmd === 'create') {
        return create();
    } else if (cmd === 'issue') {
        return issue();
    } else if (cmd === 'transfers') {
        return transfers();
    } else if (cmd === 'retires') {
        return retires();
    } else if (cmd === 'clears') {
        return clears();
    } else {
        throw Error(`${cmd} command is not exist`);
    }
}

async function newaccount() {
    if (process.env.NODE_ENV === 'development') {
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
    } else {
        console.log('error');
    }
}

async function deploy() {
    console.log('deploy stok contract');

    const user = _.find(dummy.accounts, {name : contract});
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
}

function create() {
    console.log('create');
    const create = _.assign({}, files.token.create, {issuer : user.name});
    console.log(create);
    return Promise.resolve(eosStok.create(create, code.pvt, code.name))
        .delay(10);
}

function issue() {
    console.log('issue');
    const issue = _.assign({}, files.token.issue, {issuer : user.name});
    return Promise.resolve(eosStok.issue(issue, user.pvt, user.name))
        .delay(10);
}

function transfers() {
    console.log('transfers');
    const transfers = [];
    return Promise.each(files.transfers, (creditor) => {
        console.log(creditor);
        creditor.issuer = user.name;
        return Promise.resolve(eosStok.transfer(creditor, user.pvt, user.name))
            .delay(100)
            .then((trx) => {
                const result = {creditor_id : creditor.creditor_id, trx_id : trx.transaction_id};
                console.log(result);
                transfers.push(result);
                return result;
            })
            .catch((err) => {
                err.type = 'transfers';
                err.creditor = creditor;
                err.transfers = transfers;
                err.transfers_json = JSON.stringify(transfers);
                throw err;
            });
    })
    .then((results) => {
        console.log(util.inspect(JSON.stringify(results, {depth : 3})));
    });
}

function retires() {
    console.log('retires');
    const retires = [];
    return Promise.each(files.retires, (creditor) => {
        console.log(creditor);
        creditor.issuer = user.name;
        return Promise.resolve(eosStok.retire(creditor, user.pvt, user.name))
            .delay(100)
            .then((trx) => {
                const result = {creditor_id : creditor.creditor_id, trx_id : trx.transaction_id};
                console.log(result);
                retires.push(result);
                return result;
            })
            .catch((err) => {
                err.type = 'retires';
                err.creditor = creditor;
                err.retires = retires;
                err.retires_json = JSON.stringify(retires);
                throw err;
            });
    })
    .then((results) => {
        console.log(util.inspect(JSON.stringify(results, {depth : 3})));
    });
}

function clears() {
    console.log('clears');
    const clears = [];
    return Promise.each(files.clears, (creditor) => {
        console.log(creditor);
        creditor.issuer = user.name;
        return Promise.resolve(eosStok.clear(creditor, user.pvt, user.name))
            .delay(100)
            .then((trx) => {
                const result = {creditor_id : creditor.creditor_id, trx_id : trx.transaction_id};
                console.log(result);
                clears.push(result);
                return result;
            })
            .catch((err) => {
                err.type = 'clears';
                err.creditor = creditor;
                err.clears = clears;
                err.clears_json = JSON.stringify(clears);
                throw err;
            });
    })
    .then((results) => {
        console.log(util.inspect(JSON.stringify(results, {depth : 3})));
    });
}

'use strict';

const NODE_ENV = process.env.NODE_ENV;
const files = {
    accounts : require(`./${NODE_ENV}/account`),
    token : require(`./${NODE_ENV}/token`),
    transfers : require(`./${NODE_ENV}/transfer`),
    retires : require(`./${NODE_ENV}/retire`),
    clears : require(`./${NODE_ENV}/clear`)
};

module.exports = exports = files;

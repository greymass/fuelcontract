// workaround for eosio.cdt missing bits/endian.h in libc includes
#ifndef __BYTE_ORDER__
#warning "Manually setting endianness"
#define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#endif

#include <cmath>
#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>

using namespace eosio;
using namespace std;

class [[eosio::contract("fuel")]] fuel : public contract
{
public:
    using contract::contract;

    struct transfer {
        name from;
        name to;
        asset quantity;
        string memo;
    };

    struct [[eosio::table]] config {
        name fee_account;
        symbol token_symbol;
        double referral_reward;
    };
    typedef singleton<"config"_n, config> config_type;
    config_type _config;

    struct [[eosio::table]] user {
        name account;
        double balance;
        uint64_t primary_key() const { return account.value; }
    };
    typedef eosio::multi_index<"users"_n, user> users;
    users _users;

    struct [[eosio::table]] pool {
        /** Pool name, e.g. "mtgox1" */
        name pool_name;
        /** Display name of the pool, e.g. "Mt. Gox - SafetyPool #1" */
        string display_name;
        /** Owner account name. */
        name owner;
        /** Number of credits available. */
        int64_t credits;
        /** Account filters. */
        vector<string> accounts;
        /** Action include filters. */
        vector<string> include;
        /** Action exclude filters. */
        vector<string> exclude;
        /** How many credits per account and day the pool can spend. */
        int64_t daily_quota;

        uint64_t primary_key() const { return pool_name.value; }
        uint64_t by_owner() const { return owner.value; }
    };
    typedef multi_index<"pools"_n, pool, indexed_by<"byowner"_n, const_mem_fun<pool, uint64_t, &pool::by_owner>>> pools;
    pools _pools;

    fuel(name receiver, name code, datastream<const char*> ds)
        : contract(receiver, code, ds)
        , _users(_self, _self.value)
        , _config(_self, _self.value)
        , _pools(_self, _self.value)
    {
    }

    [[eosio::action]] void configure(config config)
    {
        require_auth(_self);
        check(config.referral_reward > 0 && config.referral_reward < 1, "referral reward must be fraction between 0 and 1");
        _config.set(config, _self);
    }

    [[eosio::action]] void claim(name account)
    {
        require_auth(account);

        auto config = _config.get();
        auto user_itr = _users.find(account.value);
        check(user_itr != _users.end(), "no such user");

        int64_t units = (*user_itr).balance * pow(10, config.token_symbol.precision());
        check(units > 0, "balance too small");

        _users.modify(user_itr, _self, [&](user& user) {
            user.balance -= units / pow(10, config.token_symbol.precision());
        });

        asset quantity = asset(units, config.token_symbol);
        action { permission_level { _self, name("active") }, name("eosio.token"),
            name("transfer"),
            transfer { .from = _self,
                .to = account,
                .quantity = quantity,
                .memo = "Referral reward from Greymass Fuel" } }
            .send();
    }

    /** Create a new credits pool. */
    [[eosio::action]] void createpool(name pool_name, name owner)
    {
        require_auth(owner);
        check(pool_name.value > 5, "invalid pool name");
        check(_pools.find(pool_name.value) == _pools.end(), "pool with that name already exists");
        _pools.emplace(owner, [&](pool& pool) {
            pool.pool_name = pool_name;
            pool.owner = owner;
        });
    }

    /** Configure a credits pool. */
    [[eosio::action]] void updatepool(
        name pool_name,
        string display_name,
        vector<string> accounts,
        vector<string> include,
        vector<string> exclude,
        int64_t daily_quota)
    {
        auto itr = _pools.find(pool_name.value);
        check(itr != _pools.end(), "no such pool");
        require_auth(itr->owner);
        _pools.modify(itr, itr->owner, [&](pool& pool) {
            pool.display_name = display_name;
            check(pool.display_name.length() < 40, "display name too long");
            check(pool.display_name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890 !?#%(),.:") == string::npos, "invalid character in display name");
            pool.accounts = accounts;
            pool.include = include;
            pool.exclude = exclude;
            pool.daily_quota = daily_quota;
            check(pool.daily_quota >= 0, "daily quota must not be negative");
        });
    }

    /** Remove a credits pool. */
    [[eosio::action]] void removepool(name pool_name)
    {
        auto itr = _pools.find(pool_name.value);
        check(itr != _pools.end(), "no such pool");
        require_auth(itr->owner);
        _pools.erase(itr);
    }

    [[eosio::on_notify("eosio.token::transfer")]] void on_transfer(
        name from, name to, asset quantity, string memo)
    {
        if (from == _self || to != _self) {
            return;
        }

        size_t ref_pos = memo.rfind("ref=");
        if (ref_pos != string::npos) {
            auto config = _config.get();

            check(quantity.is_valid(), "invalid fee");
            check(quantity.symbol == config.token_symbol, "invalid fee");
            check(quantity.amount > 0, "invalid fee");

            double amount = quantity.amount / pow(10, quantity.symbol.precision());

            auto account = name(memo.substr(ref_pos + 4, memo.length()));
            double ref_amount = amount * config.referral_reward;
            update_balance(account, ref_amount);
            amount -= ref_amount;
            update_balance(config.fee_account, amount);
        }
    }

    void update_balance(name account, double amount)
    {
        auto user_itr = _users.find(account.value);
        if (user_itr == _users.end()) {
            _users.emplace(_self, [&](user& user) {
                user.account = account;
                user.balance = amount;
            });
        } else {
            _users.modify(user_itr, _self, [&](user& user) {
                user.balance += amount;
            });
        }
    }
};

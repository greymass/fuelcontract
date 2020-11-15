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
    typedef singleton<name("config"), config> config_type;
    config_type _config;

    struct [[eosio::table]] user {
        name account;
        double balance;
        uint64_t primary_key() const { return account.value; }
    };
    typedef eosio::multi_index<name("users"), user> users;
    users _users;

    fuel(name receiver, name code, datastream<const char*> ds)
        : contract(receiver, code, ds)
        , _users(_self, _self.value)
        , _config(_self, _self.value)
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

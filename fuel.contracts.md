<h1 class="contract">claim</h1>

---
spec_version: "0.2.0"
title: Claim referral rewards
summary: 'Claim {{nowrap account}}’s referral rewards'
icon: https://raw.githubusercontent.com/greymass/fuelcontract/master/icons/claim.png#6d109521d0ddf1c7e98cdd79dbe8f126dabb253df8a17a8a20493105be0e9d82
---

{{account}} agrees to have their referral reward balance transferred to their account.

<h1 class="contract">createpool</h1>

---
spec_version: "0.2.0"
title: Create credits pool
summary: '{{nowrap owner}} creates the credits pool {{nowrap pool_name}}'
icon: https://raw.githubusercontent.com/greymass/fuelcontract/master/icons/createpool.png#d59555623946f103aa5463a5797e67ad90e6e577b430c248e245850f9c2a8c5a
---

{{owner}} creates a new Greymass Fuel credits pool with the identifier {{pool_name}}.

The RAM used to store the pool configuration is owned by {{owner}} and can be released by removing the pool.

<h1 class="contract">updatepool</h1>

---
spec_version: "0.2.0"
title: Configure credits pool
summary: 'Configure the credits pool {{nowrap pool_name}}'
icon: https://raw.githubusercontent.com/greymass/fuelcontract/master/icons/updatepool.png#4c79dd7eaf0ac64762570e9e55d22d887414aafc8529a7ab1168f8d0402cdad8
---

Update the configuration of {{pool_name}} to the following:

{{#if display_name}}
The display name of the pool is {{display_name}}.
{{else}}
The pool has no display name.
{{/if}}

{{#if accounts}}
Accounts allowed to use this pool: {{accounts}}
{{else}}
Any account may use this pool.
{{/if}}

{{#if include}}
Actions that are allowed by this pool:
{{#each include}}
{{this}}
{{/each}}
{{else}}
{{#unless exclude}}
This pool allows any contract actions.
{{/unless}}
{{/if}}

{{#if exclude}}
Actions that are disallowed by this pool, these are also matched against inline actions:
{{#each exclude}}
{{this}}
{{/each}}
{{/if}}

{{#if daily_quota}}
Each account may spend up to {{daily_quota}} credits per day.
{{/if}}

<h1 class="contract">removepool</h1>

---
spec_version: "0.2.0"
title: Remove credits pool
summary: '{{nowrap owner}} removes the credits pool {{nowrap pool_name}}'
icon: https://raw.githubusercontent.com/greymass/fuelcontract/master/icons/removepool.png#d5bee7388dd63e9b3464b708283772164c24f863c43f4410df83405616f43ca7
---

{{owner}} removes the Greymass Fuel credits pool with the identifier {{pool_name}}. Any unspent credits still in the pool will be forfeit.

The RAM used to store the pool configuration will be released to {{owner}}.

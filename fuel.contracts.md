<h1 class="contract">claim</h1>

---
spec_version: "0.2.0"
title: Claim referral rewards
summary: 'Claim {{nowrap account}}’s referral rewards'
icon: https://raw.githubusercontent.com/greymass/fuelcontract/master/icons/claim.png#1f118c9ffa3e9371a545af0cd984739700761ba14a0c45f2857d9b541707d748
---

{{account}} agrees to have their referral reward balance transferred to their account.

<h1 class="contract">createpool</h1>

---
spec_version: "0.2.0"
title: Create credits pool
summary: '{{nowrap owner}} creates the credits pool {{nowrap pool_name}}'
icon: https://raw.githubusercontent.com/greymass/fuelcontract/master/icons/createpool.png#c49b8cbe3998ac705c35cdef9bef266126170af3949088fe1f92d0398ceb6cc7
---

{{owner}} creates a new Greymass Fuel credits pool with the identifier {{pool_name}}.

The RAM used to store the pool configuration is owned by {{owner}} and can be released by removing the pool.

<h1 class="contract">updatepool</h1>

---
spec_version: "0.2.0"
title: Configure credits pool
summary: '{{nowrap owner}} configures the credits pool {{nowrap pool_name}}'
icon: https://raw.githubusercontent.com/greymass/fuelcontract/master/icons/updatepool.png#b10bbec25ee3fc470f494818fc241c9f0345527f86795c8d4f1fe0a75beae622
---

{{owner}} updates the configuration of {{pool_name}} to the following:

Pool display name: {{display_name}}
Account filters: {{accounts}}
Action include filters: {{include}}
Action exclude filters: {{exclude}}
Daily quota: {{daily_quota}}

<h1 class="contract">removepool</h1>

---
spec_version: "0.2.0"
title: Remove credits pool
summary: '{{nowrap owner}} removes the credits pool {{nowrap pool_name}}'
icon: https://raw.githubusercontent.com/greymass/fuelcontract/master/icons/removepool.png#dda001e5100b42771c231dbeac533946fd6fcb3a10358197cae7ac42fea62430
---

{{owner}} removes the Greymass Fuel credits pool with the identifier {{pool_name}}. Any unspent credits still in the pool will be forfeit.

The RAM used to store the pool configuration will be released to {{owner}}.

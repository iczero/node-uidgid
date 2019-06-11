# node-uidgid

A native module to get user and group information.

## How to use

```js
const uidgid = require('uidgid');

// get user info by username or uid respectively
console.log(uidgid.getUserInfoByName('root'));
console.log(uidgid.getUserInfoByUid(0));

// get group info by group name or gid respectively
console.log(uidgid.getGroupInfoByName('root'));
console.log(uidgid.getGroupInfoByGid(0));

// get list of gids corresponding to the groups a user is a member of
console.log(uidgid.getGids('root'));
```

User info object:

```js
{
    name: 'username',
    password: 'x',
    uid: 1000,
    gid: 1000,
    gecos: 'realname',
    homedir: '/home/person',
    shell: '/bin/bash'
}
```

Group info object:

```js
{
    name: 'group',
    password: 'x',
    gid: 1000,
    members: ['person1', 'person2']
}
```

## License

ISC

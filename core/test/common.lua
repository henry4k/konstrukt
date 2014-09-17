package.path = package.path..';third-party/lua-bdd4tap/?.lua;third-party/lua-mock/?.lua'

require 'Test.More'
bdd = require 'test/bdd'
describe = require 'test/bdd/describe'
FakeRequire = require 'core/test/FakeRequire'

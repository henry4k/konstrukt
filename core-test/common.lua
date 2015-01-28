package.path = package.path..';third-party/lua-bdd4tap/?.lua;third-party/lua-mock/?.lua;core/third-party/?.lua'

require 'core/bootstrap/math'
require 'core/bootstrap/table'

require 'Test.More'
bdd = require 'test.bdd'
describe = require 'test.bdd.describe'
FakeRequire = require 'core-test/FakeRequire'
